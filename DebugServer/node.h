#ifndef NODE_H
#define NODE_H
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <thread>
#include <fstream>
#include <list>
#include <vector>

class Channel {
  public:
    Channel(int portToRecv, int portToSend);
    Channel(int id, int nodeId,
            int portToRecv, int portToSend,
            char infoToSend[10000],
            char dataToSend[10000]);

    void initReciever();
    void initSender();

    void startRecieving();

    void startSending();
    void startSendingDebug(char c);

    void startSendingFromFile(const std::string &filename); //better not to touch these
    void startRecievingToFile(const std::string &filename); //doesnt work

private:
    int id;
    int nodeId;

    char dataRecieved[10000];
    char dataToSend[10000];

    int portToRecv;
    int portToSend;

    int recieverSocket;
    int senderSocket;

    //
    sockaddr_in getLocalAddr(int port);

    int acceptConnection(sockaddr_in& addr, int socket);

    void setListeningToConnection(
            sockaddr_in& addr, int socket);

    void bindSocket(int sock, int port);
};

class Node {
public:
    Node();
    Node(int id);
    void addChannel (int ChannelId,
                     int portToRecv, int portToSend,
                     char infoToSend[10000],
                     char dataToSend[10000]);
    Channel& getLastChannel();
    uint32_t getChannelsAmount() const;

    Channel& operator[] (int i);

private:
    int id;
    std::vector<Channel> channels;
};

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

private:

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

    void startTwoChannelsDebug(
            Channel& left, Channel& right, char c);

    std::vector<Node> nodes;
    std::vector<std::pair<int,int>> adjList;
    std::vector<std::pair<int,int>> auxillaryList;
};




#endif // NODE_H
