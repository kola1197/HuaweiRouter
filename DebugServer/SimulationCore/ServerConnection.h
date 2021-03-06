//
// Created by nickolay on 27.09.2020.
//

#ifndef HUAWEIROUTER_SERVERCONNECTION_H
#define HUAWEIROUTER_SERVERCONNECTION_H

#include "QObject"
#include "Messages.h"
#include "SendingQueue.h"
#include <Utils/MutexBool.h>
#include <Utils/AsyncVar.h>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <arpa/inet.h>
#include <fstream>

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
    /*void sendMessage(PingMessage m);
    void sendMessage(SystemMessage m);
    void sendMessage(TestMessage m);
    void sendMessage(DebugMessage m);
    void sendMessage(PacketMessage m);
*/
    static AsyncVar<int> connectionsCount;
    static AsyncVar<int> connectionsCountTo;
    void getMessage();
    void getPingMessage();
    void getTestMessage();
    void getSystemMessage();
    void getDebugMessage();
    void getPacketMessage();
    void getNodeLoadMessage();
    AsyncVar<float> nodeLoad{0};
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
    AsyncVar<bool> stopped{false};
    SendingQueue sendingQueue;
    bool sendingWithoutQueue = false;

    template <typename T>
    void sendMessage(T t){                       //in header because of stupid gcc compilation
        HarbingerMessage h{};
        std::string type = typeid(t).name();
        if (Messages::getMessageTypeByName(type, &h.type)) //HarbingerMessage::PING_MESSAGE;
        {
            if (h.type == DEBUG_MESSAGE)
            {
                //t.checksum;
                //sim::sout<<"Sending DebugMessage from "<<t.from<<" to "<<t.to<<" checksum = "<<t.checksum<<sim::endl;
                if (t.checksum != 239239239)
                {
                    sim::sout<<"Sending BROKEN DebugMessage from "<<t.from<<" to "<<t.to<<" checksum = "<<t.checksum<<sim::endl;
                }
            }
            h.code = 239;
            if (!sendingWithoutQueue)
            {
                sendingQueue.addMessage(t);
                /*messageBuffer.lock();
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
                messageBuffer.unlock();*/
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
    }
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
    //std::vector<char> messagesDataQueue;
    QTimer* timer = new QTimer();
    bool isServer = false;
    std::thread thr1;
    std::thread thr;
    void updateCount(int i);
    void updateCountTo(int i);
    //SendingQueue sendingQueue;
    std::vector<char> debugBuffer;
    //std::ofstream dwout;
    //std::ofstream drout;
};


#endif //HUAWEIROUTER_SERVERCONNECTION_H
