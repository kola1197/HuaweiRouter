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


ServerNode::ServerNode(int _serverNum,int _debugSocketAdress, Graph g):QObject()
{
    graph = Graph(g);
    serverNum= _serverNum;
    debugSocketAdress = _debugSocketAdress;
    connections.clear();
}

void ServerNode::Start()       //on start we connect to debug server
{
    std::thread thr([this]() {
        int counter = 0;
        for (int i=0;i<graph.packets.size();i++)
        {
            if (graph.packets[i].from == serverNum)
            {
                counter++;
                PacketMessage m(graph.packets[i]);
                messagesStack.push_back(m);
            }
        }
        std::cout<<"Node "<<serverNum<<": GOT  "<<counter<<" packets"<<std::endl;


        addDebugConnection();
        //std::cout<<"Node "<<serverNum<<": awaiting for debug server "<<std::endl;
        while (!debugConnection->connected.get())
        {
            usleep(400);
        }
        std::cout<<"Node "<<serverNum<<": debugServer active now "<<std::endl;
        DebugMessage d;
        d.type = DebugMessage::CONNECTION_STATUS;
        d.i[0]=serverNum;
        d.i[1]=2;
        debugConnection->sendMessage(d);
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
        std::cout<<"Node "<<serverNum<<": waiting for our server connections "<<std::endl;

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
        std::cout<<"Node "<<serverNum<<": Servers are ready, waiting for clients connections "<<std::endl;
        SystemMessage mm;
        mm.type = SystemMessage::SERVERS_READY;
        mm.i[0] = serverNum;
        mm.i[1] = 1;
        debugConnection->sendMessage(mm);
        while (!allClientsReady.get())
        {
            usleep(1000);
        }
        std::cout<<"Node "<<serverNum<<": Starting client connections "<<std::endl;

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
        m.type = SystemMessage::START_SIMULATION_FLAG;
        m.i[0] = serverNum;
        m.i[1] = 1;
        debugConnection->sendMessage(m);

        DebugMessage dd;
        dd.type = DebugMessage::CONNECTION_STATUS;
        dd.i[0] = serverNum;
        dd.i[1] = 3;
        debugConnection->sendMessage(dd);
        Color::ColorMode grn(Color::FG_GREEN);
        Color::ColorMode def(Color::FG_DEFAULT);
        Color::ColorMode red(Color::FG_RED);
        std::cout<<"Node "<<serverNum<<":"<<grn<<" AWAITING FOR TEST START"<<def<<std::endl;
        while (!startTest.get())
        {
            usleep(50);
        }
        std::chrono::milliseconds ms = timeNow();
        for (int i=0;i<messagesStack.size();i++)
        {
            messagesStack[i].timeOnCreation = ms;
        }
        updatePacketCountForDebugServer();
        std::cout<<"Node "<<serverNum<<":"<<grn<<" STARTING WORK"<<def<<std::endl;
        while (!stopNode.get())
        {
            if (messagesStack.size()>0)
            {
                //here our algorithm. now random.
                PacketMessage m(messagesStack[0]);
                messagesStack.erase(messagesStack.begin());
                int i = rand() % (connections.size());
                std::cout<<"Node "<<serverNum<<":"<<grn<<" sending packet with id "<<m.id<<" to "<<connections[i]->to<<def<<std::endl;
                connections[i]->sendMessage(m);
                updatePacketCountForDebugServer();
            }
            else{
                //std::cout<<"Node "<<serverNum<<":"<<red<<" I AM EMPTY!!! "<<def<<std::endl;
                usleep(10000);
            }
        }
    });
    thr.detach();
}

std::chrono::milliseconds ServerNode::timeNow()
{
    return std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch());
}

void ServerNode::updatePacketCountForDebugServer()
{
    maxPacketsCount = maxPacketsCount>messagesStack.size() ? maxPacketsCount : messagesStack.size();
    DebugMessage dmsg;
    dmsg.type = DebugMessage::PACKET_COUNT_STATUS;
    dmsg.i[0] = serverNum;
    dmsg.i[1] = messagesStack.size();
    dmsg.i[2] = maxPacketsCount;
    debugConnection->sendMessage(dmsg);
}

void ServerNode::addDebugConnection()
{
    //auto* _debugConnection = new ServerConnection(debugSocketAdress, serverNum,-1);
    //debugConnection = QSharedPointer<ServerConnection>(_debugConnection);
    //ServerConnection debugConnection(debugSocketAdress + serverNum, serverNum,-1);
    debugConnection = new ServerConnection(debugSocketAdress + serverNum, serverNum,-1);
    //std::cout<<"port to recive = "<<debugSocketAdress<<" + "<<serverNum<<std::endl;

    //connections.push_back(debugConnection);
    debugConnection->connectTo();
    connect(debugConnection, SIGNAL(transmit_to_node(SystemMessage)),this,SLOT(get_message(SystemMessage)));

}

void ServerNode::addConnection(int to)
{
    //int portDelta = serverNum>to?serverNum:to;
    //int port = debugSocketAdress + portDelta;
    int port = -1;
    for (int i =0;i<graph.edges.size();i++){
        if ((graph.edges[i].from == serverNum && graph.edges[i].to == to) || (graph.edges[i].to == serverNum && graph.edges[i].from == to)  )
        {
            port = graph.edges[i].id + Settings::ConnectionsFirstPortNum();
        }
    }
    if (serverNum == -1)
    {
        port = debugSocketAdress + to;
    }
    if (port != -1)
    {
        //std::cout<<"debugSocketAdress = "<<debugSocketAdress<<std::endl;

        ServerConnection* newConnection = new ServerConnection(port, serverNum, to);

        connections.push_back(newConnection);
        if (serverNum > to)
        {
            //std::cout<<"Node "<<serverNum<<" creating connection to "<<to<<" on port"<<port<<std::endl;
            //newConnection->connectTo();
         }
        else{
            std::cout<<"Node "<<serverNum<<" trying to connect "<<to<<" on port"<<port<<std::endl;
            newConnection->awaitConnection();
        }
        connect(newConnection, SIGNAL(transmit_to_node(PacketMessage)),this,SLOT(get_message(PacketMessage)) );
    }
    else {
        Color::ColorMode red(Color::FG_RED);
        Color::ColorMode def(Color::FG_DEFAULT);
        std::cout<<red<<"CONNECTION FAIL "<<serverNum<<" <---> "<<to<<def<<std::endl;
    }
}

//TODO: more templates for the god of templates
void ServerNode::get_message(PacketMessage m)
{
    std::cout<<m.id<<" CHECKSUM = "<<m.checkSum<<std::endl;
    DebugMessage d;
    d.type = DebugMessage::PACKET_STATUS;
    d.i[0] = m.id;
    d.i[1] = serverNum;
    debugConnection->sendMessage(d);
    if (m.to != serverNum)
    {
        messagesStack.push_back(m);
        updatePacketCountForDebugServer();
    }
    else{
        m.delivered =true;
        std::chrono::milliseconds ms = timeNow();
        DebugMessage msg;
        msg.type = DebugMessage::PACKET_STATUS_DELIVERED;
        msg.deliveringTime = ms - m.timeOnCreation;
        msg.i[0] = m.id;
        msg.i[1] = serverNum;
        debugConnection->sendMessage(msg);
    }
}

void ServerNode::get_message(SystemMessage m)
{
    if (m.type == SystemMessage::START_SIMULATION_FLAG)
    {
        startTest.set(m.i[0]==1);
    }
    if (m.type == SystemMessage::SERVERS_READY)
    {
        std::cout<<"Node "<<serverNum<<" got SERVERS_READY status "<<m.i[0]<<std::endl;
        allClientsReady.set(m.i[0]==1);
    }
}
