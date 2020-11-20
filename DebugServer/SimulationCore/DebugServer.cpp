//
// Created by nickolay on 28.09.2020.
//

#include <thread>
#include <iostream>
#include <Utils/ColorMode.h>
#include "DebugServer.h"

DebugServer::DebugServer(int debugSocketAdress, Graph g) : ServerNode(-1,debugSocketAdress,g)
{
    //std::cout<<"Debug server: we have got "<<g.ellipses.size()<<" nodes"<<std::endl;
    //graph = g;
    //std::cout<<"Debug server: now we have got "<<graph.ellipses.size()<<" nodes"<<std::endl;
}

void DebugServer::Start()
{
    std::cout<<"Debug server started his work "<<std::endl;
    std::thread thr([this]() {
        for (int i = 0; i < graph.ellipses.size(); i++) {
            addConnection(graph.ellipses[i].number);
        }
        std::cout<<"Debug server finished his work"<<std::endl;

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
        std::cout<<"Debug server:"<<grn<<" ALL "<<connections.size() <<" NODES CONNECTED"<<def<<std::endl;

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
        std::cout<<"Debug server:"<<grn<<" ALL SERVERS ARE READY NOW, STARTING CONNECTIONS"<<def<<std::endl;

        for (int i=0;i<connections.size();i++)
        {
            SystemMessage m;
            m.type = SystemMessage::SERVERS_READY;
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
        std::cout<<"Debug server:"<<grn<<" ALL NODES ARE READY NOW!!!"<<def<<std::endl;
        for (int i=0;i<connections.size();i++)
        {
            SystemMessage m;
            m.type = SystemMessage::START_SIMULATION_FLAG;
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
    if (m.type == SystemMessage::START_SIMULATION_FLAG)
    {
        std::cout<<"Debug server:"<<" GOT SYSTEM MESSAGE FROM "<<m.i[0]<<" status START_SIMULATION_FLAG is "<<m.i[1]<<std::endl;
        for (int i=0;i<graph.ellipses.size();i++)
        {
            if (graph.ellipses[i].number == m.i[0])
            {
                graph.ellipses[i].ready = m.i[1] == 1;
            }
        }
    }
    if (m.type == SystemMessage::SERVERS_READY)
    {
        std::cout<<"Debug server:"<<grn<<" GOT SYSTEM MESSAGE FROM "<<m.i[0]<<" status SERVERS_READY is "<<m.i[1]<<def<<std::endl;
        for (int i=0;i<graph.ellipses.size();i++)
        {
            if (graph.ellipses[i].number == m.i[0])
            {
                graph.ellipses[i].serversReady = m.i[1] == 1;
            }
        }
    }
}
