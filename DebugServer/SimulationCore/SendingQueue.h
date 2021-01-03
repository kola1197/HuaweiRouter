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
        packets.push_back(QSharedPointer<typeof(T)>(new T{t}));
        packetsTypes.push_back(t.type);
        packetsPriority.push_back(t.priority);
        //TestMessage tt = *(TestMessage*)packets[0].get();
        //packetsCount.set(packets.size());
        packetsMutex.unlock();
        updateByteQueue();
        updateLoadingSize();
        //sim::sout<<"message set"<<sim::endl;
    }


    std::vector<char> getData(int sendBytesPerInterval);

private:
    template <typename T>
    void addToQueue(T t)
    {
        //sim::sout<<"HERE"<<sim::endl;
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
    std::vector<QSharedPointer<Message>> packets;
    std::vector<MessageType> packetsTypes;
    std::vector<Priority> packetsPriority;

    //QVector<QSharedPointer<Message>> qPackets;
    std::vector<char> messagesDataQueue;
    std::mutex queueMutex;
    std::mutex packetsMutex;
    void updateLoadingSize();
};


#endif //HUAWEIROUTER_SENDINGQUEUE_H
