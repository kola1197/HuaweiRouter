//
// Created by nickolay on 17.12.2020.
//

#include "SendingQueue.h"


void SendingQueue::updateByteQueue()
{
    //sim::sout<<packets.size()<<sim::endl;
    queueMutex.lock();
    if (messagesDataQueue.size()==0){
        queueMutex.unlock();
        packetsMutex.lock();
        if (packets.size()>0) {
            int selected = 0;
            //MessageType type = packetsTypes[0];
            Priority priority = packetsPriority[0];
            for (int i=0;i<packets.size();i++)
            {
                if (priority < packetsPriority[i] )
                {
                    priority = packetsPriority[i];
                    //type = packetsTypes[i];
                    selected = i;
                }
            }
            MessageType type = packetsTypes[selected];
            packetsTypes.erase(packetsTypes.begin() + selected);
            //Message m = *packets[selected].get();
            QSharedPointer<Message> q = packets[selected];
            packets.erase(packets.begin() + selected);
            priority = packetsPriority[selected];
            packetsPriority.erase(packetsPriority.begin()+selected);

            packetsMutex.unlock();
            HarbingerMessage h;
            h.type = type;
            char hData[sizeof(h)];
            memcpy(hData, &h, sizeof(h));
            queueMutex.lock();
            for (int i = 0; i < sizeof(hData); i++) {
                messagesDataQueue.push_back(hData[i]);
            }
            queueMutex.unlock();
            switch (type) {
                case MessageType::PACKET_MESSAGE:
                    addToQueue(*((PacketMessage *) q.get()));
                    break;
                case MessageType::PING_MESSAGE:
                    addToQueue(*((PingMessage *) q.get()));
                    break;
                case MessageType::TEST_MESSAGE:
                    addToQueue(*((TestMessage *) q.get()));
                    break;
                case MessageType::SYSTEM_MESSAGE:
                    addToQueue(*((SystemMessage *) q.get()));
                    break;
                case MessageType::DEBUG_MESSAGE:
                    addToQueue(*((DebugMessage *) q.get()));
                    break;
                default:
                    break;
            }
            //packetsMutex.lock();
        }
        else{
            packetsMutex.unlock();
        }
    } else{
        queueMutex.unlock();
    }
}

void SendingQueue::updateLoadingSize()
{
    queueMutex.lock();
    int datasize = messagesDataQueue.size();
    queueMutex.unlock();
    packetsMutex.lock();
    for (int i=0;i<packets.size();i++)
    {
        switch (packets[i].get()->type) {
            case MESSAGE:
                datasize += sizeof(Message);
                break;
            case PACKET_MESSAGE:
                datasize += sizeof(PacketMessage);
                break;
        }
    }
    int packetsCountInt = 0;
    for (int i=0;i<packetsTypes.size();i++)
    {
        packetsCountInt += packetsTypes[i]==PACKET_MESSAGE? 1 : 0 ;
    }
    packetsCount.set(packetsCountInt);
    packetsMutex.unlock();
    loadingSize.set(datasize);
}

std::vector<char> SendingQueue::getData(int sendBytesPerInterval)
{
    queueMutex.lock();
    int size = messagesDataQueue.size()>sendBytesPerInterval ? sendBytesPerInterval : messagesDataQueue.size();
    std::vector<char> result;
    for (int i=0;i<size;i++)
    {
        result.push_back(messagesDataQueue[0]);
        messagesDataQueue.erase(messagesDataQueue.begin());
    }
    queueMutex.unlock();
    if (messagesDataQueue.empty())
    {
        updateByteQueue();
    }
    updateLoadingSize();
    return result;
}