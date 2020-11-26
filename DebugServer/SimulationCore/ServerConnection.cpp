//
// Created by nickolay on 27.09.2020.
//

#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>
#include <thread>
#include <Utils/ColorMode.h>
#include <QtCore/QThread>
#include <zconf.h>
#include "ServerConnection.h"
#include "Messages.h"
#include "QObject"

ServerConnection::ServerConnection(int _port, int _from, int _to):QObject()
{
    port = _port;
    from = _from;
    to = _to;
}

void ServerConnection::connectTo()
{
    started.set(true);
    if (!connected.get())
    {
        std::thread thr([this]() {
            while (!connected.get())
            {
                std::cout<<"trying to connect "<<from<<" => "<<to<<". Port: "<<port<<std::endl;
                struct sockaddr_in serv_addr;
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    std::cout << "\n Socket creation error \n" << std::endl;
                }
                serv_addr.sin_family = AF_INET;
                serv_addr.sin_port = htons(port);
                // Convert IPv4 and IPv6 addresses from text to binary form
                if (inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr) <= 0) {
                    std::cout << "\nInvalid address/ Address not supported \n" << std::endl;
                }
                if (::connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
                    printf("\nConnection Failed \n");
                } else{
                    connected.set(true);
                    //std::cout<<"connection "<<from<<" => "<<to<<" status is now: CONNECTED"<<std::endl;
                }
            }
            //connect(timer, SIGNAL(timeout()), this, SLOT(sendMessagesFromBufferTick()));
            //timer->start();
            std::thread thr1([this]()
            {
                while (!needToStop.get()){
                    sendMessagesFromBufferTick();
                    usleep(sendIntervalMS);
                }
            });
            if (!oldway){
                thr1.detach();
            }
            while (!needToStop.get())
            {
                getMessage();
            }
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
            std::cout<<"error on receive HarbingerMessage"<<std::endl;
            //errorServerStop();
        }
    }
    memcpy(&h,hmsg , sizeof(h));
    if (h.type == HarbingerMessage::PING_MESSAGE)
    {
        getPingMessage();
    }
    if (h.type == HarbingerMessage::TEST_MESSAGE)
    {
        getTestMessage();
    }
    if (h.type == HarbingerMessage::SYSTEM_MESSAGE)
    {
        getSystemMessage();
    }
    if (h.type == HarbingerMessage::DEBUG_MESSAGE)
    {
        getDebugMessage();
    }
    if (h.type == HarbingerMessage::PACKET_MESSAGE)
    {
        getPacketMessage();
    }
}

void ServerConnection::getPacketMessage()
{
    PacketMessage m;
    char msg[sizeof (m)];
    int bytes;
    for (int i = 0; i < sizeof(m); i += bytes) {
        if ((bytes = recv(sock, msg + i, sizeof(m)  - i, 0)) == -1){
            std::cout<<"error on receive PacketMessage "<<std::endl;
        }
    }
    memcpy(&m, msg, sizeof(m));
    emit transmit_to_node(m);
}

void ServerConnection::getDebugMessage()
{
    DebugMessage m;
    char msg[sizeof (m)];
    int bytes;
    for (int i = 0; i < sizeof(m); i += bytes) {
        if ((bytes = recv(sock, msg + i, sizeof(m)  - i, 0)) == -1){
            std::cout<<"error on receive DebugMessage"<<std::endl;
        }
    }
    memcpy(&m, msg, sizeof(m));
    emit transmit_to_gui(m);
}

void ServerConnection::getSystemMessage()
{
    SystemMessage m;
    char msg[sizeof (m)];
    int bytes;
    for (int i = 0; i < sizeof(m); i += bytes) {
        if ((bytes = recv(sock, msg +i, sizeof(m)  - i, 0)) == -1){
            std::cout<<"error on receive SystemMessage"<<std::endl;
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
            std::cout<<"error on receive PingMessage"<<std::endl;
        }
    }
    memcpy(&m, msg, sizeof(m));
    m.time[1] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    sendMessage(m);
}

void ServerConnection::getTestMessage()
{
    TestMessage m;
    //m.testTexst = "";
    char msg[sizeof (m)];
    int bytes;
    for (int i = 0; i < sizeof(m); i += bytes) {
        if ((bytes = recv(sock, msg +i, sizeof(m)  - i, 0)) == -1){
            std::cout<<"error"<<std::endl;
        }
    }
    memcpy(&m, msg, sizeof(m));

    std::cout<<"Got test Message from "<<from<<" => "<<to<<" message '"<<m.text<<"'"<<std::endl;
}

void ServerConnection::awaitConnection()
{
    started.set(true);
    if (!connected.get())
    {
        Color::ColorMode yel(Color::FG_YELLOW);
        Color::ColorMode def(Color::FG_DEFAULT);
        std::cout <<yel<< "Awaiting for Connection "<<def<< "from "<<from<<" to "<< to << std::endl;
        std::thread thr([this]() {
            struct sockaddr_in address;
            int opt = 1;
            int addrlen = sizeof(address);
            char buffer[1024] = {0};
            if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
                perror("socket failed");
                exit(EXIT_FAILURE);
            }
            //std::cout<<"here2"<<std::endl;

            // Forcefully attaching socket to the port 8080
            if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                           &opt, sizeof(opt))) {
                perror("setsockopt");
                exit(EXIT_FAILURE);
            }
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons(port);
            //std::cout<<"here3"<<std::endl;

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
            std::cout <<grn<< "CONNECTED "<<def<< " "<<from<<" <---> "<< to << std::endl;
            std::thread thr1([this]()
            {
                while (!needToStop.get()){
                    sendMessagesFromBufferTick();
                    usleep(sendIntervalMS);
                }
            });
            if (!oldway){
                thr1.detach();
            }
            while (!needToStop.get())
            {
                getMessage();
            }
        });
        thr.detach();
    }
}

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
    //std::cout<<"sizeof m"<< sizeof(m)<<std::endl;
    send(sock, &m, sizeof(m), 0);
    sendMutex.unlock();
}

void ServerConnection::sendMessagesFromBufferTick()
{
    if (!messagesDataQueue.empty()) {
        std::cout<<"tick from "<<from<<" to"<<to<<" messagesDataQueue size: "<<messagesDataQueue.size()<<std::endl;
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
        std::cout<<"Second tick from "<<from<<" to"<<to<<" messagesDataQueue size: "<<messagesDataQueue.size()<<" data: "<< data <<std::endl;
        sendMutex.lock();
        send(sock, &data, sizeof(data), 0);
        //std::cout<<"sizeof m"<< sizeof(m)<<std::endl;
        //send(sock, &data, sizeof(data), 0);
        sendMutex.unlock();
    }
}

void ServerConnection::sendMessage(PacketMessage m)
{
    HarbingerMessage h;
    h.type = HarbingerMessage::PACKET_MESSAGE;
    h.code = 239;

    if (!oldway)
    {
        char hData[sizeof(h)];
        memcpy(hData, &h, sizeof(h));
        for (int i=0; i<sizeof(h); i++)
        {
            messagesDataQueue.push_back(hData[i]);
        }
        char mData[sizeof(m)];
        memcpy(mData, &m, sizeof(m));
        for (int i=0; i<sizeof(m); i++)
        {
            messagesDataQueue.push_back(mData[i]);
        }
    }
    else{
        sendMutex.lock();
        char hData[sizeof(h)];
        memcpy(hData, &h, sizeof(h));
        send(sock, &hData, sizeof(h), 0);
        //std::cout<<"sizeof m"<< sizeof(m)<<std::endl;
        char mData[sizeof(m)];
        memcpy(mData, &m, sizeof(m));
        send(sock, &mData, sizeof(m), 0);
        sendMutex.unlock();
    }
}

