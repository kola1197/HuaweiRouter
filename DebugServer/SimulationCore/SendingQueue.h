//
// Created by nickolay on 17.12.2020.
//

#ifndef HUAWEIROUTER_SENDINGQUEUE_H
#define HUAWEIROUTER_SENDINGQUEUE_H


#include <vector>
#include <mutex>
#include <Utils/AsyncVar.h>
#include <QSharedPointer>
#include <Utils/sout.h>
#include "Messages.h"
#include "QVector"

class SendingQueue {

public:
    AsyncVar<int> loadingSize;
    AsyncVar<int> packetsCount{0};

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
        //packetsData.push_back(QSharedPointer<typeof(T)>(new T{t}));
        packetsData.push_back(QSharedPointer<std::vector<char>>(new std::vector<char>(v)));
        packetsTypes.push_back(t.type);
        packetsPriority.push_back(t.priority);
        //TestMessage tt = *(TestMessage*)packets[0].get();
        //packetsCount.set(packets.size());
        packetsMutex.unlock();
        updateByteQueue();
        updateLoadingSize();
        //sim::sout<<"message set"<<sim::endl;
    }

    int from = -1;
    int to = -1;
    std::vector<char> getData(int sendBytesPerInterval);

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

    void updateByteQueue();
    //std::vector<QSharedPointer<Message>> packets;
    std::vector<QSharedPointer<std::vector<char>>> packetsData;
    std::vector<MessageType> packetsTypes;
    std::vector<Priority> packetsPriority;

    //QVector<QSharedPointer<Message>> qPackets;
    std::vector<char> messagesDataQueue;
    std::mutex queueMutex;
    std::mutex packetsMutex;
    void updateLoadingSize();
};


#endif //HUAWEIROUTER_SENDINGQUEUE_H
