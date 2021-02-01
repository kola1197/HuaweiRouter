//
// Created by nickolay on 27.09.2020.
//
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <Utils/ColorMode.h>
#include <zconf.h>
#include <Utils/Settings.h>
#include "ServerConnection.h"
#include "Messages.h"
#include "QObject"
#include <Utils/sout.h>


AsyncVar<int> ServerConnection::connectionsCount{0};
AsyncVar<int> ServerConnection::connectionsCountTo{0};
ServerConnection::ServerConnection(int _port, int _from, int _to, int _id):QObject()
{
    port = _port;
    from = _from;
    to = _to;
    id = _id;
    sendIntervalMS = Settings::getsendIntervalMS();
    sendBytesPerInterval = Settings::getSendBytesPerInterval();
    sendingQueue.from = from;
    sendingQueue.to = to;
    mkdir("Debug", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    //dwout.open(std::string ("Debug//Debug_write_"+QString::number(from).toStdString()+"___"+QString::number(to).toStdString()+".txt").c_str(),std::ios_base::out);
    //drout.open(std::string ("Debug//Debug_read_"+QString::number(from).toStdString()+"___"+QString::number(to).toStdString()+".txt").c_str(),std::ios_base::out);
}

ServerConnection::~ServerConnection() noexcept
{
    //needToStop.set(true);
    //connected.set(true); // for cases, when we has not connected yet
}

void ServerConnection::stop()
{
    //sim::sout<<"GOT STOP COMMAND ( "<<from<<" ==> "<<to<<" )"<<sim::endl;
    needToStop.set(true);
    //sim::sout<<"connection ( "<<from<<" ==> "<<to<<" ) needToStop = "<<needToStop.get()<<sim::endl;
    connected.set(true); // for cases, when we has not connected yet
    if (isServer){
        while (!mayCloseSocket.get())
        {
            usleep(100);
        }
        //shutdown(sock, 2);
        close(sock);
        close(server_fd);
    }
}

void ServerConnection::updateCount(int i)
{
    int c = connectionsCount.get();
    c += i;
    connectionsCount.set(c);
}

void ServerConnection::updateCountTo(int i)
{
    int c = connectionsCountTo.get();
    c += i;
    connectionsCountTo.set(c);
}

void ServerConnection::connectTo()
{
    connectionType = ConnectionType::TO;
    started.set(true);
    connectionsCount.increase(1);
    connectionsCountTo.increase(1);
    if (!connected.get())
    {
        thr = std::thread([this]() {
            while (!connected.get())
            {
                sim::sout<<"trying to connect "<<from<<" => "<<to<<". Port: "<<port<<sim::endl;
                struct sockaddr_in serv_addr;
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    sim::sout << "\n Socket creation error \n" << sim::endl;
                }
                struct timeval tv;
                tv.tv_sec = 2;
                tv.tv_usec = 0;
                //if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT | SO_RCVTIMEO, &opt, sizeof(opt))) {
                if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO , (const char *)&tv, sizeof(tv))) {
                    perror("setsockopt");
                    exit(EXIT_FAILURE);
                }
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port);
                // Convert IPv4 and IPv6 addresses from text to binary form
                if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
                    sim::sout << "\nInvalid address/ Address not supported \n" << sim::endl;
                }
                if (::connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
                    //printf("\nConnection Failed \n");
                } else{
                    connected.set(true);
                }
            }

            thr1 = std::thread([this]()
                               {
                                   while (!needToStop.get()){
                                       sendMessagesFromBufferTick();
                                       usleep(sendIntervalMS);
                                   }
                               });
            if (!sendingWithoutQueue){
                thr1.detach();
            }
            while (!needToStop.get())
            {
                getMessage();
            }
            mayCloseSocket.set(true);
            Color::ColorMode grn(Color::FG_GREEN);
            Color::ColorMode def(Color::FG_DEFAULT);
            connectionsCount.increase(-1);
            connectionsCountTo.increase(-1);
            stopped.set(true);
            sim::sout<<"Node "<<from<<grn<<" CONNECTION TO "<<def<<to<<grn<<" SUCCESSFULLY CLOSED (To)"<<def<<sim::endl;
        });
        thr.detach();
    }
}

void ServerConnection::getMessage()
{
    HarbingerMessage h;
    char hmsg[sizeof (h)];
    int hbytes;
    for (int i = 0; i < sizeof(h); i += hbytes) {
        if ((hbytes = recv(sock, hmsg +i, sizeof(h)  - i, 0)) == -1){
            //sim::sout<<"error on receive HarbingerMessage"<<sim::endl;
            return;
        }
    }
    if (!needToStop.get()){
        memcpy(&h,hmsg , sizeof(h));
        if (h.type == PING_MESSAGE)
        {
            getPingMessage();
        }
        if (h.type == TEST_MESSAGE)
        {
            getTestMessage();
        }
        if (h.type == SYSTEM_MESSAGE)
        {
            getSystemMessage();
        }
        if (h.type == DEBUG_MESSAGE)
        {
            getDebugMessage();
        }
        if (h.type == PACKET_MESSAGE)
        {
            getPacketMessage();
        }
        if (h.type == NODE_LOAD_MESSAGE)
        {
            getNodeLoadMessage();
        }
        if (h.type == NODE_LOAD_FOR_DE_TAIL_MESSAGE)
        {
            getNodeLoadForDetailMessage();
        }
    }
}

void ServerConnection::getNodeLoadForDetailMessage()
{
    NodeLoadForDeTailMessage m;
    char msg[sizeof (m)];
    int bytes;
    for (int i = 0; i < sizeof(m); i += bytes) {
        if ((bytes = recv(sock, msg + i, sizeof(m)  - i, 0)) == -1){
            sim::sout<<"error on receive DebugMessage"<<sim::endl;
            return;
        }
    }
    memcpy(&m, msg, sizeof(m));
    nodeLoadForDeTeil.set(m.load);
    sim::sout<<"node load for detail: "<<m.load<<" = "<<m.secondLoad<<sim::endl;
    if (m.load != m.secondLoad)
    {
        sim::sout<<"Error on recieving Node load for detail message"<<sim::endl;
    }
}

void ServerConnection::getPacketMessage()
{
    PacketMessage m;
    char msg[sizeof (m)];
    int bytes;
    for (int i = 0; i < sizeof(m); i += bytes) {
        if ((bytes = recv(sock, msg + i, sizeof(m)  - i, 0)) == -1){
            sim::sout<<"error on receive PacketMessage "<<sim::endl;
            return;
        }
    }
    memcpy(&m, msg, sizeof(m));
    //sim::sout<<"Node "<<from<<": from "<<to<<" got PacketMessage with id "<<m.id<<" checksum: "<<m.checkSum <<sim::endl;
    if (m.checkSum!= Messages::getChecksum(&m) || m.firstCheckSum!=239239239){
        /*PacketMessage mm;
        mm.checkSum=239239239;
        mm.firstCheckSum = 239239239;
        mm.from = 0;
        mm.to = 1;
        mm.currentPosition = 0;
        mm.delivered = false;
        char mhData[sizeof(mm)];
        memcpy(mhData, &mm, sizeof(mm));
        for (int i=0; i<sizeof(mhData); i++)
        {
            std::cout<<std::hex<<(int)mhData[i]<<" ";
        }
        std::cout<<std::endl;
        std::cout<<"---------------"<<std::endl;*/

        /*char hData[sizeof(m)];
        memcpy(hData, &m, sizeof(m));
        for (int i=0; i<sizeof(hData); i++)
        {
            std::cout<<std::hex<<(int)hData[i]<<" ";
            drout<<std::hex<<(int)hData[i]<<" ";
        }
        std::cout<<std::endl;*/
        sim::sout<<"Node "<<from<<": from "<<to<<" got PacketMessage with id "<<m.id<<" checksum: "<<m.checkSum <<sim::endl;
    }
    m.prevposition = m.currentPosition;
    m.currentPosition = from;
    emit transmit_to_node(m);
}

void ServerConnection::getDebugMessage()
{

    DebugMessage m;
    char msg[sizeof (m)];
    int bytes;
    for (int i = 0; i < sizeof(m); i += bytes) {
        if ((bytes = recv(sock, msg + i, sizeof(m)  - i, 0)) == -1){
            sim::sout<<"error on receive DebugMessage"<<sim::endl;
            return;
        }
    }
    memcpy(&m, msg, sizeof(m));
    if (m.checksum == 239239239)
    {
        emit transmit_to_gui(m);
    }
    else {
        sim::sout<<"DebugMessage checksum error"<<sim::endl;
    }
}

void ServerConnection::getSystemMessage()
{
    SystemMessage m;
    char msg[sizeof (m)];
    int bytes;
    for (int i = 0; i < sizeof(m); i += bytes) {
        if ((bytes = recv(sock, msg +i, sizeof(m)  - i, 0)) == -1){
            sim::sout<<"error on receive SystemMessage"<<sim::endl;
            return;
        }
    }
    memcpy(&m, msg, sizeof(m));
    emit transmit_to_gui(m);
    emit transmit_to_node(m);
}

void ServerConnection::getPingMessage()
{
    PingMessage m;
    char msg[sizeof (m)];
    int bytes;
    for (int i = 0; i < sizeof(m); i += bytes) {
        if ((bytes = recv(sock, msg +i, sizeof(m)  - i, 0)) == -1){
            sim::sout<<"error on receive PingMessage"<<sim::endl;
            return;
        }
    }
    memcpy(&m, msg, sizeof(m));
    m.time[1] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    sendMessage(m);
}

void ServerConnection::getTestMessage()
{

    //sim::sout<<"awaiting test message"<<sim::endl;
    TestMessage m;
    //m.testTexst = "";
    char msg[sizeof (m)];
    int bytes;
    for (int i = 0; i < sizeof(m); i += bytes) {
        if ((bytes = recv(sock, msg +i, sizeof(m)  - i, 0)) == -1){
            sim::sout<<"error"<<sim::endl;
            return;
        }
    }
    memcpy(&m, msg, sizeof(m));
    sim::sout<<"Got test Message from "<<from<<" => "<<to<<" message '"<<m.text<<"'  checkCode = "<<m.checkCode<<sim::endl;
}

void ServerConnection::getNodeLoadMessage()
{
    NodeLoadMessage m;
    char msg[sizeof (m)];
    int bytes;
    for (int i = 0; i < sizeof(m); i += bytes) {
        if ((bytes = recv(sock, msg + i, sizeof(m)  - i, 0)) == -1){
            sim::sout<<"error on receive PacketMessage "<<sim::endl;
            return;
        }
    }
    memcpy(&m, msg, sizeof(m));
    if (m.load != m.secondLoad)
    {
        sim::sout<<"Error in NodeLoad "<<m.load<<sim::endl;
    }
    //sim::sout<<"Got NodeLoad "<<m.load<<sim::endl;
    nodeLoad.set(m.load);
}

void ServerConnection::awaitConnection()
{
    connectionType = ConnectionType::FROM;
    //updateCount(1);
    connectionsCount.increase(1);
    isServer = true;
    started.set(true);
    if (!connected.get())
    {
        Color::ColorMode yel(Color::FG_YELLOW);
        Color::ColorMode def(Color::FG_DEFAULT);
        //sim::sout <<yel<< "Awaiting for Connection "<<def<< "from "<<from<<" to "<< to << sim::endl;
        std::thread thr([this]() {
            struct sockaddr_in address;
            int opt = 1;

            int addrlen = sizeof(address);
            char buffer[1024] = {0};
            if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
                perror("socket failed");
                exit(EXIT_FAILURE);
            }
            //sim::sout<<"here2"<<sim::endl;

            struct timeval tv;
            tv.tv_sec = 2;
            tv.tv_usec = 0;
            //if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT | SO_RCVTIMEO, &opt, sizeof(opt))) {
            if (setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO , (const char *)&tv, sizeof(tv))) {
                perror("setsockopt");
                exit(EXIT_FAILURE);
            }
            if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , (const char *)&tv, sizeof(tv))) {
                perror("setsockopt");
                exit(EXIT_FAILURE);
            }
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(port);
            //sim::sout<<"here3"<<sim::endl;

            // Forcefully attaching socket to the port 8080
            if (bind(server_fd, (struct sockaddr *) &address,
                     sizeof(address)) < 0) {
                perror("bind failed");
                exit(EXIT_FAILURE);
            }
            waitingForConnection.set(true);

            if (listen(server_fd, 3) < 0) {
                perror("listen");
                exit(EXIT_FAILURE);
            }
            if ((sock = accept(server_fd, (struct sockaddr *) &address,
                               (socklen_t *) &addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            connected.set(true);
            Color::ColorMode grn(Color::FG_GREEN);
            Color::ColorMode def(Color::FG_DEFAULT);
            //sim::sout <<grn<< "CONNECTED "<<def<< " "<<from<<" <---> "<< to << sim::endl;
            std::thread thr1([this]()
            {
                while (!needToStop.get()){
                    sendMessagesFromBufferTick();
                    usleep(sendIntervalMS);
                }
            });
            if (!sendingWithoutQueue){
                thr1.detach();
            }
            while (!needToStop.get())
            {
                getMessage();
            }
            //sendMutex.lock();
            //close(server_fd);
            //shutdown(sock, 2);
            //close(sock);
            mayCloseSocket.set(true);
            //updateCount(-1);
            connectionsCount.increase(-1);
            stopped.set(true);
            sim::sout<<"Node "<<from<<grn<<" CONNECTION TO "<<def<<to<<grn<<" SUCCESSFULLY CLOSED (FROm)"<<def<<sim::endl;
        });
        thr.detach();
    }
}

/*template <typename T> void ServerConnection::sendMessage(T t)
{
    HarbingerMessage h;
    std::string type = typeid(t).name();
    if (Messages::getMessageTypeByName(type, &h.type)) //HarbingerMessage::PING_MESSAGE;
    {
        h.code = 239;
        if (!oldway)
        {
            messageBuffer.lock();
            char hData[sizeof(h)];
            memcpy(hData, &h, sizeof(h));
            for (int i=0; i<sizeof(hData); i++)
            {
                messagesDataQueue.push_back(hData[i]);
            }
            char mData[sizeof(t)];
            memcpy(mData, &t, sizeof(t));
            for (int i=0; i<sizeof(mData); i++)
            {
                messagesDataQueue.push_back(mData[i]);
            }
            messageBuffer.unlock();
        }
        else{
            sendMutex.lock();
            char hData[sizeof(h)];
            memcpy(hData, &h, sizeof(h));
            send(sock, &hData, sizeof(h), 0);
            //sim::sout<<"sizeof m"<< sizeof(m)<<sim::endl;
            char mData[sizeof(t)];
            memcpy(mData, &t, sizeof(t));
            send(sock, &mData, sizeof(t), 0);
            sendMutex.unlock();
        }
    }
}*/
/*
void ServerConnection::sendMessage(PingMessage m)
{
    HarbingerMessage h;
    h.type = HarbingerMessage::PING_MESSAGE;
    h.code = 239;
    sendMutex.lock();
    send(sock, &h, sizeof(h), 0);
    send(sock, &m, sizeof(m), 0);
    sendMutex.unlock();
}

void ServerConnection::sendMessage(TestMessage m)
{
    HarbingerMessage h;
    h.type = HarbingerMessage::TEST_MESSAGE;
    h.code = 239;
    sendMutex.lock();
    send(sock, &h, sizeof(h), 0);
    send(sock, &m, sizeof(m), 0);
    sendMutex.unlock();
}

void ServerConnection::sendMessage(DebugMessage m)
{
    HarbingerMessage h;
    h.type = HarbingerMessage::DEBUG_MESSAGE;
    h.code = 239;
    sendMutex.lock();
    send(sock, &h, sizeof(h), 0);
    send(sock, &m, sizeof(m), 0);
    sendMutex.unlock();
}

void ServerConnection::sendMessage(SystemMessage m)
{
    HarbingerMessage h;
    h.type = HarbingerMessage::SYSTEM_MESSAGE;
    h.code = 239;
    sendMutex.lock();
    send(sock, &h, sizeof(h), 0);
    //sim::sout<<"sizeof m"<< sizeof(m)<<sim::endl;
    send(sock, &m, sizeof(m), 0);
    sendMutex.unlock();
}*/

void ServerConnection::sendMessagesFromBufferTick()
{
    /*messageBuffer.lock();
    if (!messagesDataQueue.empty()) {
        //sim::sout<<"tick from "<<from<<" to"<<to<<" messagesDataQueue size: "<<messagesDataQueue.size()<<sim::endl;
        int size = messagesDataQueue.size()>sendBytesPerInterval ? sendBytesPerInterval : messagesDataQueue.size();
        char data[size];
        int counter = 0;
        while (counter < sendBytesPerInterval && messagesDataQueue.size() > 0)
        {
            char c = messagesDataQueue[0];
            messagesDataQueue.erase(messagesDataQueue.begin());
            data[counter] = c;
            counter++;
        }
        //sim::sout<<"Second tick from "<<from<<" to"<<to<<" messagesDataQueue size: "<<messagesDataQueue.size()<<" data: "<< data <<sim::endl;
        sendMutex.lock();
        send(sock, &data, sizeof(data), 0);
        //sim::sout<<"sizeof m"<< sizeof(m)<<sim::endl;
        //send(sock, &data, sizeof(data), 0);
        sendMutex.unlock();
        bufferLoad.set(messagesDataQueue.size() * 100 / (1000000 * sendBytesPerInterval / sendIntervalMS));
    }
    messageBuffer.unlock();*/

    //if (from == 0){
    std::vector<char> dataToSend = sendingQueue.getData(sendBytesPerInterval);
    char data[dataToSend.size()];
    for (int i=0;i<dataToSend.size();i++)
    {
        data[i] = dataToSend[i];
        //dwout<<std::hex<<(int)dataToSend[i]<<" ";
        //debugBuffer.push_back(data[i]);
        //sim::sout<<"from "<<from<<" to "<<to<<" sending "<<std::hex<<(int)data[i]<<sim::endl;
    }
    //dwout<<"\n";
    sendMutex.lock();

    send(sock, &data, sizeof(data), 0);
    sendMutex.unlock();
    //}
    bufferLoad.set( sendingQueue.loadingSize.get() * 100 / (1000000 * sendBytesPerInterval / sendIntervalMS));
    //bufferLoad.set(messagesDataQueue.size() * 100 / (1000000 * sendBytesPerInterval / sendIntervalMS));
}

/*void ServerConnection::sendMessage(PacketMessage m)
{
    HarbingerMessage h;
    h.type = HarbingerMessage::PACKET_MESSAGE;
    h.code = 239;

    if (!oldway)
    {
        messageBuffer.lock();
        char hData[sizeof(h)];
        memcpy(hData, &h, sizeof(h));
        for (int i=0; i<sizeof(hData); i++)
        {
            messagesDataQueue.push_back(hData[i]);
        }
        char mData[sizeof(m)];
        memcpy(mData, &m, sizeof(m));
        for (int i=0; i<sizeof(mData); i++)
        {
            messagesDataQueue.push_back(mData[i]);
        }
        messageBuffer.unlock();
    }
    else{
        sendMutex.lock();
        char hData[sizeof(h)];
        memcpy(hData, &h, sizeof(h));
        send(sock, &hData, sizeof(h), 0);
        //sim::sout<<"sizeof m"<< sizeof(m)<<sim::endl;
        char mData[sizeof(m)];
        memcpy(mData, &m, sizeof(m));
        send(sock, &mData, sizeof(m), 0);
        sendMutex.unlock();
    }
}
*/
