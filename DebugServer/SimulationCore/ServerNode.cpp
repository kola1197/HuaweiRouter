//
// Created by nickolay on 27.09.2020.
//

#include <graph.h>
#include <QtCore/QSharedPointer>
#include "ServerNode.h"


ServerNode::ServerNode(int _serverNum,int _debugSocketAdress, Graph g)
{
    graph = Graph(g);
    serverNum= _serverNum;
    debugSocketAdress = _debugSocketAdress;
    connections.clear();
}

void ServerNode::Start()
{

}

void ServerNode::addConnection(int to)
{
    ServerConnection* _debugConnection = new ServerConnection(debugSocketAdress, serverNum,to);
    QSharedPointer<ServerConnection> debugConnection(_debugConnection);
    connections.push_back(debugConnection);
    if (serverNum > to)
    {
        debugConnection->connectTo();
    }
    else{
        debugConnection->awaitConnection();
    }
}