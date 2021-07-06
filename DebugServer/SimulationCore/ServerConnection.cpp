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
#include <linux/prctl.h>
#include <sys/prctl.h>


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
    connectionBreakChance = Settings::getConnectionBreakChance();
    sendingQueue.from = from;
    sendingQueue.to = to;
    sendingQueue.connectionBreakChance.set(connectionBreakChance);
}

ServerConnection::~ServerConnection() noexcept
{

}

void ServerConnection::stop()
{
    needToStop.set(true);
    connected.set(true); // for cases, when we has not connected yet
    if (isServer){
        while (!mayCloseSocket.get())
        {
            usleep(100);
        }
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
                                   QString s = "SC_FROM_" + QString::number(from)+"_TO_"+QString::number(to);
                                   prctl(PR_SET_NAME,(char *)s.toStdString().c_str());
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
    sim::sout<<"Node "<<from<<": from "<<to<<" got PacketMessage with id "<<m.id<<" checksum: "<<m.checkSum <<sim::endl;
    if (m.checkSum!= Messages::getChecksum(&m) || m.firstCheckSum!=239239239){
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

    TestMessage m;
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
    nodeLoad.set(m.load);
}

void ServerConnection::awaitConnection()
{
    connectionType = ConnectionType::FROM;
    connectionsCount.increase(1);
    isServer = true;
    started.set(true);
    if (!connected.get())
    {
        Color::ColorMode yel(Color::FG_YELLOW);
        Color::ColorMode def(Color::FG_DEFAULT);
        std::thread thr([this]() {
            struct sockaddr_in address;
            int opt = 1;
            int addrlen = sizeof(address);
            char buffer[1024] = {0};
            if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
                perror("socket failed");
                exit(EXIT_FAILURE);
            }

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
            mayCloseSocket.set(true);
            connectionsCount.increase(-1);
            stopped.set(true);
            sim::sout<<"Node "<<from<<grn<<" CONNECTION TO "<<def<<to<<grn<<" SUCCESSFULLY CLOSED (FROm)"<<def<<sim::endl;
        });
        thr.detach();
    }
}

void ServerConnection::sendMessagesFromBufferTick()
{
    std::vector<char> dataToSend = sendingQueue.getData(sendBytesPerInterval);
    char data[dataToSend.size()];
    for (int i=0;i<dataToSend.size();i++)
    {
        data[i] = dataToSend[i];
    }
    sendMutex.lock();

    send(sock, &data, sizeof(data), 0);
    sendMutex.unlock();
    bufferLoad.set( sendingQueue.loadingSize.get() * 100 / (1000000 * sendBytesPerInterval / sendIntervalMS));
}

