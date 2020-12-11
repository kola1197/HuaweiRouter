#include <Utils/Settings.h>
#include <QtCore/QSharedPointer>
#include "simulation.h"
#include <unistd.h>
#include <iostream>

Simulation::Simulation()
{

}

Simulation::Simulation(Graph *_graph)
{
    graph = _graph;
}

Simulation::~Simulation()
{
/*    if (started){
        std::cout<<"SIMULATION DESTRUCTION"<<std::endl;
        for (int i=0;i<serverNodes.size();i++)
        {
            serverNodes[i]->~ServerNode();
        }
        debugServer->~DebugServer();
    }*/
}

void Simulation::stop()
{
    if (started){
        std::cout<<"SIMULATION STOP"<<std::endl;
        for (int i=0;i<serverNodes.size();i++)
        {
            serverNodes[i]->Stop();
        }
        debugServer->Stop();
    }
}

void Simulation::Start()
{
    //std::cout<<"Packet count "<<graph->packets.size()<<std::endl;
    started = true;
    debugServer = new DebugServer(Settings::getDebugFirstPortNum(), *graph);
    debugServer->Start();
    usleep(100000);
    while (!debugServer->isReady.get())
    {
        usleep(1000);
    }
    for (int i=0;i<graph->ellipses.size();i++)
    {
        QSharedPointer<ServerNode> node = QSharedPointer<ServerNode>(new ServerNode(graph->ellipses[i].number,Settings::getDebugFirstPortNum(),*graph));
        serverNodes.push_back(node);
        node->Start();
        std::cout<<"node "<<graph->ellipses[i].number<<" started"<<std::endl;
    }
}


