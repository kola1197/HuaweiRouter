#ifndef NETWORK_H
#define NETWORK_H
#include <Network/node.h>

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
    void startInfoNetwork(std::string* infoPtr);
    void startInfoOrientedNetwork(std::string *infoPtr);

//private:

    bool releaseTheKraken;

    int freePort;

    //network setters of 2 level

    void startTwoChannels(Channel& left,
                          Channel& right);

    void startTwoChannelsDebug(
            Channel& left, Channel& right, char c);

    void startTwoChannelsInfo(
            Channel& left, Channel& right,
            std::string* infoPtr);

    //network setters of 1 level

    void startLeftToSendRight(Channel& left,
                              Channel& right);

    void startLeftToSendRightDebug(
            Channel& left, Channel& right,
            char c);

    void startLeftToSendRightInfo(
            Channel& left, Channel& right,
            std::string* infoPtr);

    //network setters of 0 level

    void connectTwoChannels(Channel& left,
                            Channel& right);

    void setLeftToSendRight(Channel& left,
                            Channel& right);

    std::vector<Node> nodes;
    std::vector<std::pair<int,int>> adjList;
    std::vector<std::pair<int,int>> auxillaryList;
};

#endif // NETWORK_H
