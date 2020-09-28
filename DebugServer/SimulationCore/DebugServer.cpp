//
// Created by nickolay on 28.09.2020.
//

#include <thread>
#include <iostream>
#include "DebugServer.h"

DebugServer::DebugServer(int debugSocketAdress, Graph g) : ServerNode(-1,debugSocketAdress,g)
{

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
}