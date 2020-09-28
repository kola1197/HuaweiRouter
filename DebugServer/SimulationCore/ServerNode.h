//
// Created by nickolay on 27.09.2020.
//

#ifndef HUAWEIROUTER_SERVERNODE_H
#define HUAWEIROUTER_SERVERNODE_H


#include "ServerConnection.h"

class ServerNode {
public:
    ServerNode(int serverNum,int debugSocketAdress, Graph g);
    void Start();
    std::vector<QSharedPointer<ServerConnection>> connections;
protected:
    void addConnection(int to);
    Graph graph;
    int serverNum = -1;
    int debugSocketAdress = -1;
};


#endif //HUAWEIROUTER_SERVERNODE_H
