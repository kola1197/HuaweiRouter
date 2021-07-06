#include <Utils/Settings.h>
#include <QtCore/QSharedPointer>
#include "simulation.h"
#include <unistd.h>
#include <Utils/sout.h>
#include <QMessageBox>
#include <Utils/ColorMode.h>

Simulation::Simulation()
{

}

Simulation::Simulation(Graph *_graph)
{
    graph = _graph;
    sim::sout<<"Alghoritm: "<<graph->selectedAlgorithm<<sim::endl;
}

Simulation::~Simulation()
{

}

void Simulation::stop()
{
    if (started){
        for (int i=0;i<serverNodes.size();i++)
        {
            serverNodes[i]->StopToConnections();
        }
        int counter = 0;
        while (counter <1000 && ServerConnection::connectionsCountTo.get() != 0)
        {
            counter++;
            sim::sout<<"turning off "<<counter<<"%"<<sim::endl;
            usleep(20000);
        }
        Color::ColorMode red(Color::FG_RED);
        Color::ColorMode def(Color::FG_DEFAULT);
        if (ServerConnection::connectionsCountTo.get()!=0)
        {
            sim::sout<<red<<"ERROR!!! "<<def<<ServerConnection::connectionsCountTo.get()<<red<<" threads alive"<<def<<sim::endl;
        }
        for (int i=0;i<serverNodes.size();i++)
        {
            serverNodes[i]->Stop();
        }
        debugServer->Stop();
    }
    usleep(100000);
    sim::sout<<"SIMULATION STOPED. Count of active connections: "<<ServerConnection::connectionsCount.get()<<sim::endl;
    if (ServerConnection::connectionsCount.get()!=0)
    {
        QMessageBox msgBox;
        msgBox.setText("Error. Some socket threads has not closed, please restart app.");
        msgBox.exec();
        for (int i=0;i<serverNodes.size();i++)
        {
            for (int j=0;j<serverNodes[i]->connections.size();j++)
            {
                if (!serverNodes[i]->connections[j]->stopped.get())
                {
                    Color::ColorMode red(Color::FG_RED);
                    Color::ColorMode def(Color::FG_DEFAULT);
                    sim::sout<<red<<"CONNECTION "<<def<<serverNodes[i]->connections[j]->from<<red<<" ==> "<<def<<serverNodes[i]->connections[j]->to<<red<<" STIL ACTIVE"<<sim::endl;
                }
            }
        }
    }
}

void Simulation::Start()
{
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
        node->loadPackets();
    }
    for (int i=0;i<graph->ellipses.size();i++)
    {
        serverNodes[i]->Start();
        sim::sout<<"node "<<graph->ellipses[i].number<<" started"<<sim::endl;
    }
}


