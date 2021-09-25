//
// Created by nickolay on 28.09.2020.
//

#include <thread>
#include <iostream>
#include <Utils/ColorMode.h>
#include <Utils/sout.h>
#include "DebugServer.h"

DebugServer::DebugServer(int debugSocketAdress, Graph g) : ServerNode(-1,debugSocketAdress,g)
{
    //sim::sout<<"Debug server: we have got "<<g.ellipses.size()<<" nodes"<<sim::endl;
    //graph = g;
    //sim::sout<<"Debug server: now we have got "<<graph.ellipses.size()<<" nodes"<<sim::endl;
}

DebugServer::~DebugServer() noexcept
{
    /*for (int i=0;i<connections.size();i++)
    {
        connections[i]->~ServerConnection();
    };*/
}

void DebugServer::Stop()
{
    for (int i=0;i<connections.size();i++)
    {
        connections[i]->stop();
    };
}

void DebugServer::Start()
{
    sim::sout<<"Debug server started his work "<<sim::endl;
    std::thread thr([this]() {
        for (int i = 0; i < graph.ellipses.size(); i++) {
            addConnection(graph.ellipses[i].number);
        }
        sim::sout<<"Debug server finished his work"<<sim::endl;

        bool allNodesConnected = false;
        while (!allNodesConnected)
        {
            bool b = true;
            for (int i=0;i<connections.size();i++)
            {
                b = connections[i]->connected.get() & b;
            }
            allNodesConnected = b;
        }
        Color::ColorMode grn(Color::FG_GREEN);
        Color::ColorMode def(Color::FG_DEFAULT);
        sim::sout<<"Debug server:"<<grn<<" ALL "<<connections.size() <<" NODES CONNECTED"<<def<<sim::endl;

        for (int i=0;i<connections.size();i++)
        {
            SystemMessage m;
            m.function = SystemMessage::DEBUG_SERVER_READY;
            m.i[0]=1;
            connections[i]->sendMessage(m);
        }

        bool allServersReady =false;
        while (!allServersReady)
        {
            bool b = true;
            for (int i=0;i<graph.ellipses.size();i++)
            {
                b = graph.ellipses[i].serversReady & b;
            }
            allServersReady = b;
        }
        sim::sout<<"Debug server:"<<grn<<" ALL SERVERS ARE READY NOW, STARTING CONNECTIONS"<<def<<sim::endl;

        for (int i=0;i<connections.size();i++)
        {
            SystemMessage m;
            m.function = SystemMessage::SERVERS_READY;
            m.i[0]=1;
            connections[i]->sendMessage(m);
        }

        bool allNodesReady = false;
        while (!allNodesReady)
        {
            bool b = true;
            for (int i=0;i<graph.ellipses.size();i++)
            {
                b = graph.ellipses[i].ready && b;
            }
            allNodesReady = b;
        }
        sim::sout<<"Debug server:"<<grn<<" ALL NODES ARE READY NOW!!!"<<def<<sim::endl;
        for (int i=0;i<connections.size();i++)
        {
            SystemMessage m;
            m.function = SystemMessage::START_SIMULATION_FLAG;
            m.i[0]=1;
            connections[i]->sendMessage(m);
        }
    });
    thr.detach();
    isReady.set(true);
}

void DebugServer::get_message_for_debug(SystemMessage m)
{
    Color::ColorMode grn(Color::FG_GREEN);
    Color::ColorMode def(Color::FG_DEFAULT);
    if (m.function == SystemMessage::START_SIMULATION_FLAG)
    {
        sim::sout<<"Debug server:"<<" GOT SYSTEM MESSAGE FROM "<<m.i[0]<<" status START_SIMULATION_FLAG is "<<m.i[1]<<sim::endl;
        for (int i=0;i<graph.ellipses.size();i++)
        {
            if (graph.ellipses[i].number == m.i[0])
            {
                graph.ellipses[i].ready = m.i[1] == 1;
            }
        }
    }
    if (m.function == SystemMessage::SERVERS_READY)
    {
        sim::sout<<"Debug server:"<<grn<<" GOT SYSTEM MESSAGE FROM "<<m.i[0]<<" status SERVERS_READY is "<<m.i[1]<<def<<sim::endl;
        for (int i=0;i<graph.ellipses.size();i++)
        {
            if (graph.ellipses[i].number == m.i[0])
            {
                graph.ellipses[i].serversReady = m.i[1] == 1;
            }
        }
    }
}


void DebugServer::sendMessageTo(int nodeNum, SystemMessage msg){
    ServerConnection * con = getConnection(nodeNum);
    if (con != nullptr){
        con->sendMessage(msg);
    }
}

void DebugServer::stopNode(int nodeNum){
    for (auto & edge : graph.edges) {
        if (edge.to == nodeNum){
            std::cout<<"From "<<edge.from<<" to "<<edge.to<<std::endl;
            SystemMessage msg;
            msg.function = SystemMessage::SET_EDGE_STATUS;
            msg.i[0] = nodeNum;
            msg.i[1] = 0;          // 0 - turn off, 1 - turn on
            sendMessageTo(edge.from, msg);
        }
        if (edge.from == nodeNum){
            std::cout<<"From "<<edge.from<<" to "<<edge.to<<std::endl;
            SystemMessage msg;
            msg.function = SystemMessage::SET_EDGE_STATUS;
            msg.i[0] = nodeNum;
            msg.i[1] = 0;          // 0 - turn off, 1 - turn on
            sendMessageTo(edge.to, msg);
        }
    }
}