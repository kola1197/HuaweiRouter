#ifndef SIMLATION_H
#define SIMLATION_H


#include <graph.h>

class Simulation
{
public:
    Simulation();
    Simulation(Graph *_graph);
    Graph *graph;
    void Start();
};

#endif // SIMLATION_H
