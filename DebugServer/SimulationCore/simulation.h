#ifndef SIMLATION_H
#define SIMLATION_H


#include <graph.h>
#include "DebugServer.h"

class Simulation
{
public:
    Simulation();
    Simulation(Graph *_graph);
    Graph *graph;
    std::vector<QSharedPointer<ServerNode>> serverNodes;
    DebugServer *debugServer;
    void Start();
};

#endif // SIMLATION_H
