#include <Utils/Settings.h>
#include <QtCore/QSharedPointer>
#include "simulation.h"
#include <unistd.h>
#include <iostream>
#include <Utils/sout.h>
#include <QMessageBox>

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
        sim::sout<<"SIMULATION DESTRUCTION"<<sim::endl;
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
//        sim::sout<<"SIMULATION STOP. Count of active connections: "<<ServerConnection::connectionsCount.get()<<sim::endl;
        for (int i=0;i<serverNodes.size();i++)
        {
            serverNodes[i]->StopToConnections();
        }
        usleep(100000);
        for (int i=0;i<serverNodes.size();i++)
        {
            serverNodes[i]->Stop();
        }
        debugServer->Stop();
    }
    sim::sout<<"SIMULATION STOPED. Count of active connections: "<<ServerConnection::connectionsCount.get()<<sim::endl;
    if (ServerConnection::connectionsCount.get()!=0)
    {
        QMessageBox msgBox;
        msgBox.setText("Error. Some socket threads has not closed, please restart app.");
        msgBox.exec();
    }
}

void Simulation::Start()
{
    //sim::sout<<"Packet count "<<graph->packets.size()<<sim::endl;
    ServerConnection::connectionsCount.set(0);
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
        sim::sout<<"node "<<graph->ellipses[i].number<<" started"<<sim::endl;
    }
}


