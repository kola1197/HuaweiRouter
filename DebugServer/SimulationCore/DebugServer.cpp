//
// Created by nickolay on 28.09.2020.
//

#include <thread>
#include <iostream>
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
    });
    thr.detach();
    isReady.set(true);
    std::cout<<"Debug server finished his work"<<std::endl;
}