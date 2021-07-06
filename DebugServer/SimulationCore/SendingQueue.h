//
// Created by nickolay on 17.12.2020.
// This class is part of server connection, when serverNode sends packet with serverConnection - serverConnection puts it to sendingQueue to send with priority when socket connection will be ready
//

#ifndef HUAWEIROUTER_SENDINGQUEUE_H
#define HUAWEIROUTER_SENDINGQUEUE_H

#include "QObject"
#include <vector>
#include <mutex>
#include <Utils/AsyncVar.h>
#include <QSharedPointer>
#include <Utils/sout.h>
#include "Messages.h"
#include "QVector"

class SendingQueue : public QObject{
Q_OBJECT

public:
    SendingQueue();
    AsyncVar<int> loadingSize;
    AsyncVar<int> packetsCount{0};
    AsyncVar<bool> broken{false};
    AsyncVar<bool> brokenStatusChecked{true};
    AsyncVar<int> connectionBreakChance{0};
    std::vector<MessageType> packetsTypes;
    std::vector<int> packetsId;
    std::vector<Priority> packetsPriority;
    template <typename T>
    void addMessage(T t)
    {
        packetsMutex.lock();
        char c [sizeof(t)];
        memcpy(&c, &t, sizeof(t));
        std::vector<char> v;
        for (int i=0;i< sizeof(c); i++)
        {
            v.push_back(c[i]);
        }
        packetsData.push_back(QSharedPointer<std::vector<char>>(new std::vector<char>(v)));
        if (t.type == PACKET_MESSAGE)
        {
            packetsFromMutex.lock();
            packetsFrom.push_back(std::tuple<int,int>{t.id, t.prevposition});
            if (!packetsId.empty() && packetsId[packetsId.size()-1] == t.id){
                sim::sout<<"duplicate shit here"<<sim::endl;
            }
            packetsId.push_back(t.id);
            packetsFromMutex.unlock();
        }
        packetsTypes.push_back(t.type);
        packetsPriority.push_back(t.priority);
        packetsMutex.unlock();
        updateByteQueue();
        updateLoadingSize();
    }
    int from = -1;
    int to = -1;
    std::vector<char> getData(int sendBytesPerInterval);
    std::vector<std::tuple<int,int>> packetsFrom;
    std::mutex packetsFromMutex;
    void updateLoadingSize();
    std::vector<QSharedPointer<std::vector<char>>> packetsData;
    std::mutex packetsMutex;
    std::mutex queueMutex;
    void tryToChangeBreakStatus();

signals:
    void updateBreakedStatus();
private:
    template <typename T>
    void addToQueue(T t)
    {
        //sim::sout<<"HERE"<<sim::endl;
        if (typeid(T).name() == typeid(PacketMessage).name()){
            PacketMessage * p = (PacketMessage*) &t;
            if ( p->checkSum!= Messages::getChecksum(p))
            {
                sim::sout<<"ERROR! WRONG CHECKSUM!!!"<<sim::endl;
                std::cout<< ("Error on Node %s !!! Packet with id %s got wrong checksum ( %s )!!! Check your RAM!!!",p->id, p->checkSum)<<std::endl;
                qFatal("Error on Node %s !!! Packet with id %s got wrong checksum ( %s )!!! Check your RAM!!!",p->id, p->checkSum);
            }
        }
        char mData[sizeof(t)];
        memcpy(mData, &t, sizeof(t));
        queueMutex.lock();
        for (int i=0; i<sizeof(mData); i++)
        {
            messagesDataQueue.push_back(mData[i]);
        }
        queueMutex.unlock();
    }
    bool coinFlipLinkBreak();
    void updateByteQueue();
    std::vector<char> messagesDataQueue;

};


#endif //HUAWEIROUTER_SENDINGQUEUE_H
