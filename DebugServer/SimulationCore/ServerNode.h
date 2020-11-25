//
// Created by nickolay on 27.09.2020.
//

#ifndef HUAWEIROUTER_SERVERNODE_H
#define HUAWEIROUTER_SERVERNODE_H


#include <graph.h>
#include <functional>
#include "ServerConnection.h"

class ServerNode :public QObject{
Q_OBJECT
public:
    ServerNode(int serverNum,int debugSocketAdress, Graph g);
    void Start();
    //std::vector<std::shared_ptr<ServerConnection>> connections;
    std::vector<ServerConnection*> connections;
    ServerConnection* debugConnection;
    std::vector<PacketMessage> messagesStack;
    MutexBool startTest{false};
    MutexBool stopNode{false};
    MutexBool allClientsReady{false};

public slots:
    void get_message(PacketMessage m);
    void get_message(SystemMessage m);
protected:
    void addConnection(int to);
    Graph graph;
    int serverNum = -1;
    int debugSocketAdress = -1;
private:
    void addDebugConnection();
    std::chrono::milliseconds timeNow();
    void updatePacketCountForDebugServer();

};


#endif //HUAWEIROUTER_SERVERNODE_H
