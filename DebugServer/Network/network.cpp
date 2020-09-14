#include "network.h"

Network::Network(): releaseTheKraken(0), freePort(8000) {}

Network::Network(std::vector<std::pair<int,int>> &adjList,
                 int port, int nodesAmount):
         freePort(port), adjList(adjList) {
    createNodes(nodesAmount);
}

Network::Network(std::vector<Node> &nodes,
        std::vector<std::pair<int,int>> &adjList,
        int port = 8000):
    nodes(nodes), adjList(adjList),
    freePort(port) {}

void Network::createNodes(int nodesAmount) {
    using namespace std;
    nodes = {};
    for (int i = 0; i < nodesAmount; ++i) {
        nodes.push_back(Node(i));
    }
}

void Network::setLeftToSendRight(
        Channel& left, Channel& right) {
    using namespace std;
    thread initRecieverThread(&Channel::initReciever, &left);
    thread initSenderThread(&Channel::initSender, &right);
    initRecieverThread.join();
    initSenderThread.join();
}

void Network::connectTwoChannels(
        Channel& left, Channel& right) {
    using namespace std;
    setLeftToSendRight(left, right);
    setLeftToSendRight(right, left);
}

void Network::connectNetwork() {
    using namespace std;
    int channelId = 0;
    auxillaryList = {};
    for (pair<int,int>& edge : adjList) {
        Node& first = nodes[edge.first];
        Node& second = nodes[edge.second];
        first.addChannel(
                    channelId, freePort,
                    freePort + 5, nullptr, nullptr);
        second.addChannel(
                    channelId + 1, freePort + 5,
                    freePort, nullptr, nullptr);
        freePort += 10;
        channelId += 2;
        connectTwoChannels(first.getLastChannel(),
                           second.getLastChannel());
        auxillaryList.push_back({first.getChannelsAmount() - 1,
                                 second.getChannelsAmount() - 1});
    }
}

void Network::startLeftToSendRight(
        Channel& left, Channel& right) {
    using namespace std;
    std::thread startSendingThread
            (&Channel::startSending, left);
    std::thread startRecievingThread
            (&Channel::startRecieving, right);
    startSendingThread.detach();
    startRecievingThread.detach();
}

void Network::startTwoChannels(
        Channel& left, Channel& right) {
    startLeftToSendRight(left, right);
    startLeftToSendRight(right, left);
}

void Network::startLeftToSendRightDebug(Channel& left, Channel& right, char c) {
    using namespace std;
        std::thread startSendingThread
                (&Channel::startSendingDebug, left, c);
        std::thread startRecievingThread
                (&Channel::startRecieving, right);
        startSendingThread.detach();
        startRecievingThread.detach();
}

void Network::startLeftToSendRightInfo(Channel &left,
                                       Channel &right,
                                       std::string *infoPtr){
std::thread startSendingThread
        (&Channel::startSendingInformation, left, infoPtr);
std::thread startRecievingThread
        (&Channel::startRecievingInformation, right, infoPtr->size());
startSendingThread.detach();
startRecievingThread.detach();
}

void Network::startTwoChannelsDebug(Channel& left,
                                    Channel& right,
                                    char c) {
    startLeftToSendRightDebug(left, right, c);
    startLeftToSendRightDebug(right, left, c + 1);
}

void Network::startTwoChannelsInfo(Channel &left,
                                   Channel &right,
                                   std::string *infoPtr) {
    startLeftToSendRightInfo(left, right, infoPtr);
    startLeftToSendRightInfo(right, left, infoPtr);
}

void Network::startDebugNetwork() {
    releaseTheKraken = 0;

    char a = 'a';
    for (int i = 0; i < adjList.size(); ++i) {
        Node& first = nodes[adjList[i].first];
        Node& second = nodes[adjList[i].second];
        Channel& channelOfFirst = first[auxillaryList[i].first];
        Channel& channelOfSecond = second[auxillaryList[i].second];
        startTwoChannelsDebug(channelOfFirst, channelOfSecond, a);
        a += 2;
    }
    releaseTheKraken = 1;
}

void Network::startInfoNetwork(std::string *infoPtr) {
    releaseTheKraken = 0;
    for (int i = 0; i < adjList.size(); ++i) {
        Node& first = nodes[adjList[i].first];
        Node& second = nodes[adjList[i].second];
        Channel& channelOfFirst = first[auxillaryList[i].first];
        Channel& channelOfSecond = second[auxillaryList[i].second];
        startTwoChannelsInfo(channelOfFirst, channelOfSecond, infoPtr);
    }
    releaseTheKraken = 1;
}

void Network::startInfoOrientedNetwork(std::string *infoPtr) {
    releaseTheKraken = 0;
    for (int i = 0; i < adjList.size(); ++i) {
        Node& first = nodes[adjList[i].first];
        Node& second = nodes[adjList[i].second];
        Channel& channelOfFirst = first[auxillaryList[i].first];
        Channel& channelOfSecond = second[auxillaryList[i].second];
        startLeftToSendRightInfo(channelOfFirst, channelOfSecond, infoPtr);
    }
    releaseTheKraken = 1;
}
