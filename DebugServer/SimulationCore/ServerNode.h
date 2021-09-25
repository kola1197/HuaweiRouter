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
    ~ServerNode();
    void Start();
    void loadPackets();
    //std::vector<std::shared_ptr<ServerConnection>> connections;
    std::vector<ServerConnection*> connections;
    ServerConnection* debugConnection;
    std::vector<PacketMessage> messagesStack;
    MutexBool debugServerReady {false};
    MutexBool startTest{false};
    MutexBool stopNode{false};
    MutexBool allClientsReady{false};
    void Stop();
    void StopToConnections();
public slots:
    void get_message(PacketMessage m);
    void get_message(SystemMessage m);
protected:
    void addConnection(int to, EdgeData * edgeData = nullptr);
    Graph graph;
    int serverNum = -1;
    int debugSocketAdress = -1;

    ServerConnection *getConnection(int nodeNum);

private:
    void addDebugConnection();
    std::chrono::milliseconds timeNow();
    void updatePacketCountForDebugServer();
    int maxPacketsCount = 0;
    void updateEdgesUsage();
    void updateNodeLoadForLocalVoting();
    int randomSelectionAlgorithm(int prevNodeNum, int to);
    int drillSelectionAlgorithm(int prevNodeNum, int to);
    int localVotingSelectionAlgorithm(int prevNodeNum, int to);
    int deTailSelectionAlgorithm(int prevNodeNum, int to);
    int MyLocalVotingSelectionAlgorithm(int prevNodeNum, int to);
    int localFlowSelectionAlgorithm(int prevNodeNum, int to);
    int selectPacketPath(int prevNodeNum, int to);
    AsyncVar<int> packetMessagesCounter {0};
    std::chrono::milliseconds updatePacketPrevSendingTime;
    std::mutex getPacketsMutex;

    int pathLength(int nodeFrom, int nodeTo);
    std::vector<PacketMessage> localFlowStack;
    bool localFlowBufferOpened = true;
    std::mutex messageStackMutex;
    void updateNodeLoadForDeTails();

    int alpha = 10;
    float nodeLoad = 0;
    std::vector<int> recivedPacketsId;
    std::chrono::milliseconds localFlowLastUpdate;

    void checkConnectionsForBreak();

    void updateBreakStatuses();

    int debugPrevPacketFromConnectionId = -1;
    int debugPrevPacketInputId = -1;
    int debugPrevPacketSendedId = -1;

    bool connectionRestarted = false;

};


#endif //HUAWEIROUTER_SERVERNODE_H
