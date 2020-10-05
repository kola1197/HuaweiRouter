//
// Created by nickolay on 27.09.2020.
//

#ifndef HUAWEIROUTER_SERVERNODE_H
#define HUAWEIROUTER_SERVERNODE_H


#include <graph.h>
#include <functional>
#include "ServerConnection.h"

class ServerNode {
public:
    ServerNode(int serverNum,int debugSocketAdress, Graph g);
    void Start();
    //std::vector<std::shared_ptr<ServerConnection>> connections;
    std::vector<ServerConnection*> connections;
    ServerConnection* debugConnection;
protected:
    void addConnection(int to);
    Graph graph;
    int serverNum = -1;
    int debugSocketAdress = -1;
private:
    void addDebugConnection();

};


#endif //HUAWEIROUTER_SERVERNODE_H
