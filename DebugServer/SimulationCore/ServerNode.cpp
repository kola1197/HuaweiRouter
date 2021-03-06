//
// Created by nickolay on 27.09.2020.
//

#include <graph.h>
#include <QtCore/QSharedPointer>
#include "ServerNode.h"
#include <unistd.h>
#include <iostream>
#include <thread>
#include <Utils/ColorMode.h>
#include <Utils/Settings.h>
#include "QObject"
#include <cstdlib>
#include <ctime>
#include <Utils/sout.h>
#include "string"

ServerNode::ServerNode(int _serverNum,int _debugSocketAdress, Graph g):QObject()
{
    graph = Graph(g);
    serverNum= _serverNum;
    debugSocketAdress = _debugSocketAdress;
    connections.clear();

}

ServerNode::~ServerNode() noexcept
{
 /*   for (int i=0; i < connections.size();i++)
    {
        connections[i]->~ServerConnection();
    }
    debugConnection->~ServerConnection();*/
}

void ServerNode::StopToConnections()
{
    stopNode.set(true);
    //usleep(15000);
    for (int i=0; i < connections.size();i++)
    {
        if (connections[i]->connectionType == ConnectionType::TO){
            connections[i]->stop();
        }
    }
    usleep(15000);
    debugConnection->stop();
}

void ServerNode::Stop()
{
    stopNode.set(true);
    //usleep(15000);
    for (int i=0; i < connections.size();i++)
    {
        if (connections[i]->connectionType == ConnectionType::FROM){
            connections[i]->stop();
        }
    }
    usleep(10000);
    //debugConnection->stop();
}

void ServerNode::loadPackets()
{
    for (int i=0;i<graph.packets.size();i++)
    {
        if (graph.packets[i].from == serverNum)
        {
            PacketMessage m;//(graph.packets[i]);
            m.from = graph.packets[i].from;
            m.to = graph.packets[i].to;
            m.id = graph.packets[i].id;
            //m.currentPosition = graph.packets[i].currentPosition;
            //m.prevposition = m.currentPosition;
            m.currentPosition = serverNum;
            m.prevposition = m.currentPosition;
            m.type = graph.packets[i].type;
            m.checkSum = Messages::getChecksum(&m);
            m.firstCheckSum = 239239239;
            messagesStack.push_back(m);
        }
    }
}

void ServerNode::Start()       //on start we connect to debug server
{
    std::thread thr([this]() {
        /*int counter = 0;
        for (int i=0;i<graph.packets.size();i++)
        {
            if (graph.packets[i].from == serverNum)
            {
                counter++;
                PacketMessage m(graph.packets[i]);
                messagesStack.push_back(m);
            }
        }*/
        //sim::sout<<"Node "<<serverNum<<": GOT  "<<counter<<" packets"<<sim::endl;

        updatePacketPrevSendingTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        addDebugConnection();
        //sim::sout<<"Node "<<serverNum<<": awaiting for debug server "<<sim::endl;
        while (!debugConnection->connected.get())
        {
            usleep(400);
        }
        //sim::sout<<"Node "<<serverNum<<": debugServer active now "<<sim::endl;
        DebugMessage d;
        d.checksum = 239239239;
        d.function = DebugMessage::CONNECTION_STATUS;
        d.i[0]=serverNum;
        d.i[1]=2;
        debugConnection->sendMessage(d);

        while (!debugServerReady.get())
        {
            usleep(400);
        }

        //here we adding all other connections
        int c =0;
        for (int i=0;i<graph.edges.size();i++)       //trying to set server sockets, when they are ready - starting client sockets
        {
            if (graph.edges[i].from == serverNum)
            {
                c++;
                addConnection(graph.edges[i].to);
            }
            if (graph.edges[i].to == serverNum)
            {
                c++;
                addConnection(graph.edges[i].from);
            }
        }
        bool allServerConnectionsReady = false;
        //sim::sout<<"Node "<<serverNum<<": waiting for our server connections "<<sim::endl;

        while (!allServerConnectionsReady)
        {
            if (c == connections.size())
            {
                bool b = true;
                for (int i=0;i<connections.size();i++) {
                    b = (connections[i]->started.get() xor !connections[i]->waitingForConnection.get()) && b;
                }
                allServerConnectionsReady = b;
            }
        }
        //sim::sout<<"Node "<<serverNum<<": Servers are ready, waiting for clients connections "<<sim::endl;
        SystemMessage mm;
        mm.function = SystemMessage::SERVERS_READY;
        mm.i[0] = serverNum;
        mm.i[1] = 1;
        debugConnection->sendMessage(mm);
        while (!allClientsReady.get())
        {
            usleep(1000);
        }
        //sim::sout<<"Node "<<serverNum<<": Starting client connections "<<sim::endl;

        for (int i =0; i<connections.size();i++)
        {
            if (!connections[i]->started.get())
            {
                connections[i]->connectTo();
            }
        }
        bool allNodesConnected = false;
        while (!allNodesConnected)
        {
            if (connections.size() == c)
            {
                bool b = true;
                for (int i=0;i<connections.size();i++)
                {
                    b = connections[i]->connected.get() & b;
                }
                allNodesConnected = b;
            }
        }
        SystemMessage m;
        m.function = SystemMessage::START_SIMULATION_FLAG;
        m.i[0] = serverNum;
        m.i[1] = 1;
        debugConnection->sendMessage(m);

        DebugMessage dd;
        dd.checksum = 239239239;
        dd.function = DebugMessage::CONNECTION_STATUS;
        dd.i[0] = serverNum;
        dd.i[1] = 3;
        debugConnection->sendMessage(dd);
        Color::ColorMode grn(Color::FG_GREEN);
        Color::ColorMode def(Color::FG_DEFAULT);
        Color::ColorMode red(Color::FG_RED);
        //sim::sout<<"Node "<<serverNum<<":"<<grn<<" AWAITING FOR TEST START"<<def<<sim::endl;
        while (!startTest.get())
        {
            usleep(50);
        }
        usleep(100);
        std::chrono::milliseconds ms = timeNow();
        for (int i=0;i<messagesStack.size();i++)
        {
            messagesStack[i].timeOnCreation = ms;
        }
        updatePacketCountForDebugServer();
        bool zeroPacketCountSent = false;
        //sim::sout<<"Node "<<serverNum<<":"<<grn<<" STARTING WORK"<<def<<sim::endl;
        while (!stopNode.get())
        {
            updateEdgesUsage();
            if (!messagesStack.empty())
            {
                PacketMessage m(messagesStack[0]);
                messagesStack.erase(messagesStack.begin());
                int prevNodeNum = m.prevposition;
                int i = selectPacketPath(prevNodeNum, m.to);
                sim::sout<<"Node "<<serverNum<<":"<<grn<<" sending packet with id "<<m.id<<" to "<<connections[i]->to<<def<<sim::endl;
                connections[i]->sendMessage(m);
                updatePacketCountForDebugServer();
                updateNodeLoadForLocalVoting();
                zeroPacketCountSent = false;
            }
            else{
                //if (!zeroPacketCountSent){
                    updatePacketCountForDebugServer();
                    //zeroPacketCountSent = true;
                //}
                //sim::sout<<"Node "<<serverNum<<":"<<red<<" I AM EMPTY!!! "<<def<<sim::endl;
                usleep(10000);
            }
        }
        sim::sout<<"NODE "<<serverNum<<" STOPPED"<<sim::endl;
    });
    thr.detach();
}

void ServerNode::updateNodeLoadForLocalVoting()
{
    float connectionsLoad = 0;
    for (int i=0;i<connections.size();i++)
    {
        connectionsLoad += connections[i]->bufferLoad.get();
    }
    float stackload = 1;
    for (int i=0;i<messagesStack.size();i++)
    {
        stackload += sizeof(messagesStack[i]);
    }
    for (int i=0;i<connections.size();i++)
    {
        if (connections[i]->to != -1)
        {
            NodeLoadMessage m;
            m.load = stackload * connectionsLoad;
            connections[i]->sendMessage(m);
        }
    }
}

int ServerNode::selectPacketPath(int prevNodeNum, int to)
{
    switch (graph.selectedAlgorithm) {
        case Algorithms::RANDOM:
            return randomSelectionAlgorithm(prevNodeNum);
            break;
        case Algorithms::DRILL:
            return drillSelectionAlgorithm();
            break;
        case Algorithms::DE_TAILS:
            return drillSelectionAlgorithm();
            break;
        case Algorithms::LOCAL_FLOW:
            return drillSelectionAlgorithm();
            break;
        case Algorithms::LOCAL_VOTING:
            return localVotingSelectionAlgorithm(prevNodeNum, to);
            break;
    }
}

int ServerNode::randomSelectionAlgorithm(int prevNodeNum)
{
    //srand(time(0));
    //sim::sout<<"prev node num "<<prevNodeNum<<sim::endl;
    int a = prevNodeNum;
    a = rand() % (connections.size());
    while (prevNodeNum == connections[a]->to){
        a = rand() % (connections.size());
    }
    return a;
}

int ServerNode::drillSelectionAlgorithm()
{
    //srand(time(0));
    int a = rand() % (connections.size());
    int b = a;
    while (b == a)
    {
        b = rand() % (connections.size());
    }
    return connections[a]->bufferLoad.get() > connections[b]->bufferLoad.get() ? b : a;
}

int ServerNode::localVotingSelectionAlgorithm(int prevNodeNum, int to)
{
    //sim::sout<<"localVotingSelectionAlgorithm"<<sim::endl;
    std::vector<float> nodesLoad;
    float sum=0;
    for (int i=0;i<connections.size();i++)
    {
        float a = connections[i]->nodeLoad.get();
        float b = connections[i]->bufferLoad.get();
        float c = pathLength(connections[i]->to,to);
        nodesLoad.push_back( (a + b + c * c) * (a + b + c * c) + 1);
        sum += nodesLoad[nodesLoad.size()-1];
    }
    int isum = 0;
    for (int i=0;i<nodesLoad.size();i++)
    {
        nodesLoad[i] = sum/nodesLoad[i];
        nodesLoad[i] = qRound(nodesLoad[i]);
        isum += nodesLoad[i];
    }
    //srand(time(0));
    int result = prevNodeNum;
    while (result == prevNodeNum) {
        int a = rand() % isum;
        for (int i = 0; i < nodesLoad.size(); i++) {
            a -= nodesLoad[i];
            if (a < 0) {
                if (connections[i]->to != prevNodeNum) {
                    result = i;
                    return i;
                } else {
                    i = nodesLoad.size();
                }
            }
        }
    }
}

int ServerNode::pathLength(int nodeFrom, int nodeTo)
{
    graph.calculatePathLength(nodeFrom);
    return graph.getEllipseByNumber(nodeTo)->pathLength;
}

void ServerNode::updateEdgesUsage()      // it will be broken with more than 99 edges in one node
{
    DebugMessage d;
    d.checksum = 239239239;
    d.function = DebugMessage::EDGES_USAGE_STATUS;
    d.i[0] = connections.size();
    for (int j=0;j<d.i[0];j++)
    {
        d.i[1+j*3] = connections[j]->id;
        d.i[2+j*3] = connections[j]->from;
        d.i[3+j*3] = qRound(connections[j]->bufferLoad.get());
    }
    debugConnection->sendMessage(d);
}

std::chrono::milliseconds ServerNode::timeNow()
{
    return std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch());
}

void ServerNode::updatePacketCountForDebugServer()
{
    std::chrono::milliseconds end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    if ((end - updatePacketPrevSendingTime).count() > 1000)
    {
        updatePacketPrevSendingTime = end;
        int packetsStackSize = messagesStack.size();
        for (int i=0;i<connections.size();i++)
        {
            packetsStackSize += connections[i]->sendingQueue.packetsCount.get();
        }
        maxPacketsCount = maxPacketsCount > packetsStackSize ? maxPacketsCount : packetsStackSize;
        DebugMessage dmsg;
        dmsg.checksum = 239239239;
        dmsg.function = DebugMessage::PACKET_COUNT_STATUS;
        dmsg.i[0] = serverNum;
        dmsg.i[1] = packetsStackSize;
        dmsg.i[2] = maxPacketsCount;
        debugConnection->sendMessage(dmsg);
    }
}

void ServerNode::addDebugConnection()
{
    //auto* _debugConnection = new ServerConnection(debugSocketAdress, serverNum,-1);
    //debugConnection = QSharedPointer<ServerConnection>(_debugConnection);
    //ServerConnection debugConnection(debugSocketAdress + serverNum, serverNum,-1);
    debugConnection = new ServerConnection(debugSocketAdress + serverNum, serverNum,-1);
    //sim::sout<<"port to recive = "<<debugSocketAdress<<" + "<<serverNum<<sim::endl;
    debugConnection->sendBytesPerInterval = 6000;
    debugConnection->sendIntervalMS = 33;
    debugConnection->sendingWithoutQueue = true;
    //connections.push_back(debugConnection);
    debugConnection->connectTo();
    connect(debugConnection, SIGNAL(transmit_to_node(SystemMessage)),this,SLOT(get_message(SystemMessage)));

}

void ServerNode::addConnection(int to)
{
    //int portDelta = serverNum>to?serverNum:to;
    //int port = debugSocketAdress + portDelta;
    int port = -1;
    int edgeID = -1;
    for (int i =0;i<graph.edges.size();i++){
        if ((graph.edges[i].from == serverNum && graph.edges[i].to == to) || (graph.edges[i].to == serverNum && graph.edges[i].from == to)  )
        {
            port = graph.edges[i].id + Settings::getConnectionsFirstPortNum();
            edgeID = graph.edges[i].id;
        }
    }
    if (serverNum == -1)
    {
        port = debugSocketAdress + to;
    }
    if (port != -1)
    {
        //sim::sout<<"debugSocketAdress = "<<debugSocketAdress<<sim::endl;

        ServerConnection* newConnection = new ServerConnection(port, serverNum, to, edgeID);

        connections.push_back(newConnection);
        if (serverNum > to)
        {
            //sim::sout<<"Node "<<serverNum<<" creating connection to "<<to<<" on port"<<port<<sim::endl;
            //newConnection->connectTo();
         }
        else{
            sim::sout<<"Node "<<serverNum<<" trying to connect "<<to<<" on port"<<port<<sim::endl;
            newConnection->awaitConnection();
        }
        connect(newConnection, SIGNAL(transmit_to_node(PacketMessage)),this,SLOT(get_message(PacketMessage)) );
    }
    else {
        Color::ColorMode red(Color::FG_RED);
        Color::ColorMode def(Color::FG_DEFAULT);
        sim::sout<<red<<"CONNECTION FAIL "<<serverNum<<" <---> "<<to<<def<<sim::endl;
    }
}

void ServerNode::get_message(PacketMessage m)
{
    getPacketsMutex.lock();
    //sim::sout<<"Node"<< serverNum<<", message with id "<<m.id<<" got CHECKSUM = "<<m.checkSum<<sim::endl;
    if (m.checkSum!= Messages::getChecksum(&m) || m.firstCheckSum!=239239239)
    {
        /*PacketMessage mm;
        mm.checkSum=239239239;
        mm.from = 0;
        mm.to = 1;
        mm.currentPosition = 0;
        mm.delivered = false;
        char mhData[sizeof(mm)];
        memcpy(mhData, &mm, sizeof(mm));
        for (int i=0; i<sizeof(mhData); i++)
        {
            std::cout<<std::hex<<mhData[i];
        }
        std::cout<<std::endl;
        std::cout<<"---------------"<<std::endl;

        char hData[sizeof(m)];
        memcpy(hData, &m, sizeof(m));
        for (int i=0; i<sizeof(hData); i++)
        {
            std::cout<<std::hex<<hData[i];
        }
        std::cout<<std::endl;*/
        sim::sout<<"Error on Node "<<serverNum<<" !!! Packet with id "<<m.id <<" got wrong checksum ( "<<m.checkSum<<" )!!! Check your RAM!!!"<<sim::endl;
        qFatal("Error on Node %s !!! Packet with id %s got wrong checksum ( %s )!!! Check your RAM!!!",serverNum,m.id, m.checkSum);
    }
    DebugMessage d;
    d.checksum = 239239239;
    d.function = DebugMessage::PACKET_STATUS;
    d.i[0] = m.id;
    d.i[1] = serverNum;
    debugConnection->sendMessage(d);
    if (m.to != serverNum)
    {
        messagesStack.push_back(m);
        updatePacketCountForDebugServer();
    }
    else{
        packetMessagesCounter.increase(1);
        sim::sout<<"Node "<<serverNum<<": PacketMessage with id "<<m.id<<" is now at home. Already got "<<packetMessagesCounter.get()<<" messages!"<<sim::endl;
        m.delivered = true;
        std::chrono::milliseconds ms = timeNow();
        DebugMessage msg;
        msg.checksum = 239239239;
        msg.function = DebugMessage::PACKET_STATUS_DELIVERED;
        msg.deliveringTime = ms - m.timeOnCreation;
        msg.i[0] = m.id;
        msg.i[1] = serverNum;
        debugConnection->sendMessage(msg);
    }
    getPacketsMutex.unlock();
}

void ServerNode::get_message(SystemMessage m)
{
    if (m.function == SystemMessage::START_SIMULATION_FLAG)
    {
        startTest.set(m.i[0]==1);
    }
    if (m.function == SystemMessage::SERVERS_READY)
    {
        sim::sout<<"Node "<<serverNum<<" got SERVERS_READY status "<<m.i[0]<<sim::endl;
        allClientsReady.set(m.i[0]==1);
    }
    if (m.function == SystemMessage::DEBUG_SERVER_READY)
    {
        sim::sout<<"Node "<<serverNum<<" got DEBUG_SERVER_READY status "<<m.i[0]<<sim::endl;
        debugServerReady.set(m.i[0]==1);
    }
}
