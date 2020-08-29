#ifndef NETWORK_H
#define NETWORK_H
#include <node.h>

class Network { // connects nodes before whole work
public:

    Network();
    Network(std::vector<Node> &nodes,
            std::vector<std::pair<int,int>> &adjList,
            int port);

    Network(std::vector<std::pair<int,int>> &adjList,
            int port, int nodesAmount);

    void setAdjList(std::vector<std::pair<int,int>> &adjList);

    uint32_t getNodesAmount() const;

    void connectNetwork();

    void createNodes(int n);

    void startDebugNetwork();

//private:

    bool releaseTheKraken;

    int freePort;

    void setLeftToSendRight(Channel& left,
                            Channel& right);

    void connectTwoChannels(Channel& left,
                            Channel& right);

    void startLeftToSendRight(Channel& left,
                              Channel& right);

    void startTwoChannels(Channel& left,
                          Channel& right);

    void startLeftToSendRightDebug(
            Channel& left, Channel& right,
            char c);

    void startLeftToSendRightFromFile(
            Channel& left, Channel& right,
            char c);

    void startTwoChannelsDebug(
            Channel& left, Channel& right, char c);

    std::vector<Node> nodes;
    std::vector<std::pair<int,int>> adjList;
    std::vector<std::pair<int,int>> auxillaryList;
};

#endif // NETWORK_H
