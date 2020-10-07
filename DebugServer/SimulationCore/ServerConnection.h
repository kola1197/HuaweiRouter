//
// Created by nickolay on 27.09.2020.
//

#ifndef HUAWEIROUTER_SERVERCONNECTION_H
#define HUAWEIROUTER_SERVERCONNECTION_H

#include "QObject"
#include "Messages.h"
#include <Utils/MutexBool.h>

class ServerConnection : public QObject{
    Q_OBJECT
public:
    ServerConnection(int _port, int _from, int _to);
    void connectTo();
    void awaitConnection();
    MutexBool connected {false};
    void sendMessage(PingMessage m);
    void sendMessage(SystemMessage m);
    void sendMessage(TestMessage m);
    void sendMessage(DebugMessage m);
    void sendMessage(PacketMessage m);


    void getMessage();
    void getPingMessage();
    void getTestMessage();
    void getSystemMessage();
    void getDebugMessage();
    void getPacketMessage();

    void stop();

signals:
    void transmit_to_gui(SystemMessage m);
    void transmit_to_gui(DebugMessage m);

    void transmit_to_node(PacketMessage m);

private:
    int from = -1;
    int to = -1;

    int port = 0;

    int sock = 0;
    int server_fd = 0;

    MutexBool needToStop{false};

    std::string ip = "127.0.0.2";



    std::mutex sendMutex;

};


#endif //HUAWEIROUTER_SERVERCONNECTION_H
