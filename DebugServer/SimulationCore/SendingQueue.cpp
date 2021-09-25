//
// Created by nickolay on 17.12.2020.
// This class is part of server connection, when serverNode sends packet with serverConnection - serverConnection puts it to sendingQueue to send with priority when socket connection will be ready
//

#include <Utils/ColorMode.h>
#include "SendingQueue.h"
SendingQueue::SendingQueue():QObject() {

}

void SendingQueue::updateByteQueue()
{
    queueMutex.lock();
    if (messagesDataQueue.size()==0) {
            queueMutex.unlock();
            packetsMutex.lock();
            if (packetsData.size() > 0) {
                int selected = 0;
                Priority priority = packetsPriority[0];
                for (int i = 0; i < packetsData.size(); i++) {
                    if (priority < packetsPriority[i]) {
                        priority = packetsPriority[i];
                        selected = i;
                    }
                }
                MessageType type = packetsTypes[selected];

                char cdata[packetsData[selected].get()->size()];
                for (int i = 0; i < packetsData[selected].get()->size(); i++) {
                    cdata[i] = (*packetsData[selected].get())[i];
                }
                priority = packetsPriority[selected];
                packetsData.erase(packetsData.begin() + selected);
                packetsTypes.erase(packetsTypes.begin() + selected);
                packetsPriority.erase(packetsPriority.begin() + selected);
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
                PacketMessage p;
                PingMessage pingP;
                TestMessage testP;
                SystemMessage sysP;
                DebugMessage debP;
                NodeLoadMessage nodP;
                NodeLoadForDeTailMessage nodDP;
                int j = -1;
                switch (type) {
                    case MessageType::PACKET_MESSAGE:
                        memcpy(&p, cdata, sizeof(p));
                        if (p.checkSum != Messages::getChecksum(&p)) {
                            sim::sout << "ERROR HERE!!!" << sim::endl;
                            qFatal("Error !!! Packet with id %s got wrong checksum ( %s )!!! Check your RAM!!!", p.id,
                                   p.checkSum);
                        }
                        packetsFromMutex.lock();
                        for (int i = 0; i < packetsFrom.size(); i++) {
                            if (std::get<0>(packetsFrom[i]) == p.id) {
                                j = i;
                            }
                        }
                        if (j != -1) {
                            packetsId.erase(packetsId.begin() + j);
                            packetsFrom.erase(packetsFrom.begin() + j);
                        }
                        packetsFromMutex.unlock();
                        addToQueue(p);
                        break;
                    case MessageType::PING_MESSAGE:
                        memcpy(&pingP, cdata, sizeof(pingP));
                        addToQueue(pingP);
                        break;
                    case MessageType::TEST_MESSAGE:
                        memcpy(&testP, cdata, sizeof(testP));
                        addToQueue(testP);
                        break;
                    case MessageType::SYSTEM_MESSAGE:
                        memcpy(&sysP, cdata, sizeof(sysP));
                        addToQueue(sysP);
                        break;
                    case MessageType::DEBUG_MESSAGE:
                        memcpy(&debP, cdata, sizeof(debP));
                        addToQueue(debP);
                        break;
                    case MessageType::NODE_LOAD_MESSAGE:
                        memcpy(&nodP, cdata, sizeof(nodP));
                        if (nodP.secondLoad != nodP.load) {
                            sim::sout << "ERROR nodP" << sim::endl;
                        }
                        addToQueue(nodP);
                        break;
                    case MessageType::NODE_LOAD_FOR_DE_TAIL_MESSAGE:
                        memcpy(&nodDP, cdata, sizeof(nodDP));
                        addToQueue(nodDP);
                        break;
                    default:
                        break;
                }
            } else {
                packetsMutex.unlock();
            }
    } else{
        queueMutex.unlock();
    }
}

void SendingQueue::tryToChangeBreakStatus(){
    if (coinFlipLinkBreak() && false){
        bool br = broken.get();
        broken.set(!br);
        brokenStatusChecked.set(false);
    }
}

void SendingQueue::setBreakStatus(bool status){
    Color::ColorMode yel(Color::FG_YELLOW);
    Color::ColorMode def(Color::FG_DEFAULT);
    sim::sout<<yel<<"Connection from "<<def<<from<<yel<<" to "<< def<<to<<yel<<" paused"<<def<<sim::endl;
    broken.set(status);
    brokenStatusChecked.set(false);
}

bool SendingQueue::coinFlipLinkBreak()
{
    bool result = false;
    if (broken.get()){
        result = 0 == rand() % (connectionBreakChance.get()/10);
    }
    else{
        result = 0 == rand() % connectionBreakChance.get();
    }
    return result;
}

void SendingQueue::updateLoadingSize()
{
    queueMutex.lock();
    int datasize = messagesDataQueue.size();
    queueMutex.unlock();
    packetsMutex.lock();
    for (auto & packetsType : packetsTypes)
    {
        switch (packetsType) {
            case MESSAGE:
                datasize += sizeof(Message);
                break;
            case PACKET_MESSAGE:
                datasize += sizeof(PacketMessage);
                break;
            default: break;
        }
    }
    int packetsCountInt = 0;
    for (auto & packetsType : packetsTypes)
    {
        packetsCountInt += packetsType==PACKET_MESSAGE? 1 : 0 ;
    }
    packetsCount.set(packetsCountInt);
    packetsMutex.unlock();
    loadingSize.set(datasize);
}

std::vector<char> SendingQueue::getData(int sendBytesPerInterval)
{
    queueMutex.lock();
    u_long size = messagesDataQueue.size()>sendBytesPerInterval ? sendBytesPerInterval : messagesDataQueue.size();
    std::vector<char> result;
    for (u_long i=0;i<size;i++)
    {
        result.push_back(messagesDataQueue[0]);
        messagesDataQueue.erase(messagesDataQueue.begin());
    }
    bool messagesStackIsEmpty = messagesDataQueue.empty();
    queueMutex.unlock();
    if (messagesStackIsEmpty)
    {
        updateByteQueue();
    }
    updateLoadingSize();
    return result;
}