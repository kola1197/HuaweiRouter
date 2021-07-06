//
// Created by nickolay on 28.09.2020.
// Additional Node that is linked with every ServerNode for debugging, visualization on screen and help with first handshake between serverNodes
//

#ifndef HUAWEIROUTER_DEBUGSERVER_H
#define HUAWEIROUTER_DEBUGSERVER_H


#include <graph.h>
#include "ServerNode.h"

class DebugServer: public ServerNode {
    Q_OBJECT
public:
    DebugServer(int debugSocketAdress, Graph g);
    ~DebugServer();
    void Start();
    MutexBool isReady {false};   //true when all connections for nodes are ready
    void Stop();
public slots:
    void get_message_for_debug(SystemMessage);

private:
};


#endif //HUAWEIROUTER_DEBUGSERVER_H
