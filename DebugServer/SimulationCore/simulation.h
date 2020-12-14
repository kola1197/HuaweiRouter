#ifndef SIMLATION_H
#define SIMLATION_H


#include <graph.h>
#include "DebugServer.h"

class Simulation
{
public:
    Simulation();
    Simulation(Graph *_graph);
    ~Simulation();
    Graph *graph;
    std::vector<QSharedPointer<ServerNode>> serverNodes;
    DebugServer *debugServer;
    bool started = false;
    void Start();
    void stop();
};

#endif // SIMLATION_H
