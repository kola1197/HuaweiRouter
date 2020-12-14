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

enum ConnectionType{
    TO, FROM
};

class ServerConnection : public QObject{
    Q_OBJECT
public:
    ConnectionType connectionType;
    ServerConnection(int _port, int _from, int _to, int _id = -1);
    ~ServerConnection();
    void connectTo();
    void awaitConnection();
    MutexBool connected {false};
    void sendMessage(PingMessage m);
    void sendMessage(SystemMessage m);
    void sendMessage(TestMessage m);
    void sendMessage(DebugMessage m);
    void sendMessage(PacketMessage m);
    static AsyncVar<int> connectionsCount;
    static AsyncVar<int> connectionsCountTo;
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
    float sendIntervalMS = 330;
    float sendBytesPerInterval = 64;//64;
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
    MutexBool mayCloseSocket{false};
    std::string ip = "127.0.0.2";
    std::mutex sendMutex;
    //std::vector<PacketMessage> messagesQueue;
    std::vector<char> messagesDataQueue;
    QTimer* timer = new QTimer();
    bool oldway = false;
    bool isServer = false;
    std::thread thr1;
    std::thread thr;
    void updateCount(int i);
    void updateCountTo(int i);
};


#endif //HUAWEIROUTER_SERVERCONNECTION_H
