//
// Created by nickolay on 27.09.2020.
//

#ifndef HUAWEIROUTER_SERVERCONNECTION_H
#define HUAWEIROUTER_SERVERCONNECTION_H

#include "QObject"
#include "Messages.h"
#include <Utils/MutexBool.h>
#include <Utils/AsyncVar.h>
#include <QtCore/QTimer>
#include <QtCore/QThread>

class ServerConnection : public QObject{
    Q_OBJECT
public:
    ServerConnection(int _port, int _from, int _to, int _id = -1);
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

    int from = -1;
    int to = -1;
    int id = -1;
    MutexBool started {false};
    MutexBool waitingForConnection{false};
    std::mutex messageBuffer;
    //connection limit settings
    float sendIntervalMS = 33;
    float sendBytesPerInterval = 6;//64;
    int updateUsageDataPerTicks = 5;
    int updateUsageDataCounter = 0;
    AsyncVar<float> bufferLoad{0};
signals:
    void transmit_to_gui(SystemMessage m);
    void transmit_to_gui(DebugMessage m);

    void transmit_to_node(PacketMessage m);
    void transmit_to_node(SystemMessage m);

private slots:
    void sendMessagesFromBufferTick();

private:
    int port = 0;
    int sock = 0;
    int server_fd = 0;
    MutexBool needToStop{false};
    std::string ip = "127.0.0.2";
    std::mutex sendMutex;
    //std::vector<PacketMessage> messagesQueue;
    std::vector<char> messagesDataQueue;
    QTimer* timer = new QTimer();
    bool oldway = false;
};


#endif //HUAWEIROUTER_SERVERCONNECTION_H
