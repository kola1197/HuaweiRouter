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
    void stop();
private:
    int from = -1;
    int to = -1;

    int port = 0;

    int sock = 0;
    int server_fd = 0;

    MutexBool needToStop{false};

    std::string ip = "127.0.0.2";

    void getPingMessage();

    std::mutex sendMutex;
    void sendMessage(PingMessage m);
    void sendMessage(SystemMessage m);

    void getMessage();
};


#endif //HUAWEIROUTER_SERVERCONNECTION_H
