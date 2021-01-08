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
        if (packetsData.size()>0) {
            int selected = 0;
            //MessageType type = packetsTypes[0];
            Priority priority = packetsPriority[0];
            for (int i=0;i<packetsData.size();i++)
            {
                if (priority < packetsPriority[i] )
                {
                    priority = packetsPriority[i];
                    //type = packetsTypes[i];
                    selected = i;
                }
            }
            MessageType type = packetsTypes[selected];

            char cdata [packetsData[selected].get()->size()] ;
            for (int i = 0;  i < packetsData[selected].get()->size(); i++)
            {
                cdata[i] = (*packetsData[selected].get())[i];
            }

            priority = packetsPriority[selected];
            //packetsPriority.erase(packetsPriority.begin()+selected);
            packetsData.erase(packetsData.begin() + selected);
            packetsTypes.erase(packetsTypes.begin() + selected);
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
            //char data [pdata.size()] = pdata.data();
            PacketMessage p;
            PingMessage pingP;
            TestMessage testP;
            SystemMessage sysP;
            DebugMessage debP;
            switch (type) {
                case MessageType::PACKET_MESSAGE:
                    //p = PacketMessage(*((PacketMessage *) q.get()));
                    memcpy(&p, cdata, sizeof(p));
                    if (p.checkSum != 239239239)
                    {
                        sim::sout<<"ERROR HERE!!!"<<sim::endl;
                    }

                    //memcpy(&pp, &p, sizeof(p));
                    //sim::sout<<"Node "<<from<<": updateByteQueue message with id "<<p.id<<" to "<<to<<" checksum: "<<p.checkSum<< sim::endl;
                    //addToQueue(*((PacketMessage *) q.get()));
                    /*if (pp.checkSum != 239239239)
                    {
                        sim::sout<<"ERROR HERE!!!"<<sim::endl;
                    }*/
                    addToQueue(p);
                    break;
                case MessageType::PING_MESSAGE:
                    memcpy(&pingP, cdata, sizeof(pingP));
                    addToQueue(pingP);
                    //addToQueue(*((PingMessage *) q.get()));
                    break;
                case MessageType::TEST_MESSAGE:
                    memcpy(&testP, cdata, sizeof(testP));
                    addToQueue(testP);
                    //addToQueue(*((TestMessage *) q.get()));
                    break;
                case MessageType::SYSTEM_MESSAGE:
                    memcpy(&sysP, cdata, sizeof(sysP));
                    addToQueue(sysP);
                    //addToQueue(*((SystemMessage *) q.get()));
                    break;
                case MessageType::DEBUG_MESSAGE:
                    memcpy(&debP, cdata, sizeof(debP));
                    addToQueue(debP);
                    //addToQueue(*((DebugMessage *) q.get()));
                    break;
                default:
                    break;
            }
            /*packetsMutex.lock();
            packets.erase(packets.begin() + selected);
            packetsTypes.erase(packetsTypes.begin() + selected);
            packetsPriority.erase(packetsPriority.begin()+selected);
            packetsMutex.unlock();*/
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
    for (int i=0;i<packetsTypes.size();i++)
    {
        switch (packetsTypes[i]) {
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