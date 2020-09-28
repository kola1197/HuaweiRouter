//
// Created by nickolay on 27.09.2020.
//

#include <netinet/in.h>
#include <iostream>
#include <arpa/inet.h>
#include <thread>
#include <Utils/ColorMode.h>
#include "ServerConnection.h"
#include "Messages.h"


ServerConnection::ServerConnection(int _port, int _from, int _to):QObject()
{
    port = _port;
    from = _from;
    to = _to;
}

void ServerConnection::connectTo()
{
    if (connected.get())
    {
        std::thread thr([this]() {
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
                std::cout<<"connection "<<from<<" => "<<to<<" status is now: CONNECTED"<<std::endl;
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
            std::cout<<"error"<<std::endl;
            //errorServerStop();
        }
    }
    memcpy(&h,hmsg , sizeof(h));
    if (h.type == HarbingerMessage::PING_MESSAGE)
    {
        getPingMessage();
    }
}

void ServerConnection::getPingMessage()
{
    PingMessage m;
    char msg[sizeof (m)];
    int bytes;
    for (int i = 0; i < sizeof(m); i += bytes) {
        if ((bytes = recv(sock, msg +i, sizeof(m)  - i, 0)) == -1){
            std::cout<<"error"<<std::endl;
        }
    }
    memcpy(&m, msg, sizeof(m));
    m.time[1] = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    sendMessage(m);
}

void ServerConnection::awaitConnection()
{
    if (!connected.get())
    {
        Color::ColorMode yel(Color::FG_YELLOW);
        Color::ColorMode def(Color::FG_DEFAULT);
        std::cout <<yel<< "trying to build Connection "<<def<< "from "<<from<<" to "<< to << std::endl;
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