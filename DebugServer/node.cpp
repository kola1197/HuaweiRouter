#include "node.h"

Node::Node(int id):id(id) {}

uint32_t Node::getChannelsAmount() const {
    return channels.size();
}

void Node::addChannel(int ChannelId,
                      int portToRecv, int portToSend,
                      char *infoToSend = nullptr,
                      char *dataToSend = nullptr) {
    using namespace std;

    channels.push_back(Channel(ChannelId, this->id,
                               portToRecv, portToSend,
                               nullptr, nullptr));
}

Channel& Node::getLastChannel() {
    using namespace std;

    return channels[channels.size() - 1];
}

Channel& Node::operator[] (int i) {
    return channels[i];
}

Channel::Channel(int portToRecv, int portToSend)
        : portToRecv(portToRecv),
          portToSend(portToSend) {}

Channel::Channel(int id, int nodeId,
                 int portToRecv, int portToSend,
                 char *infoToSend = nullptr,
                 char *dataToSend = nullptr)
        :id(id), nodeId(nodeId), portToRecv(portToRecv),
          portToSend(portToSend) {}

sockaddr_in Channel::getLocalAddr(int port) {

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    return addr;
}

void Channel::setListeningToConnection(
        sockaddr_in& addr, int socket) {

    bind(socket, (sockaddr*)&addr, sizeof(addr));
    listen(socket, 1);
}

int Channel::acceptConnection(
        sockaddr_in& addr, int socket) {

    socklen_t addrSize = sizeof(addr);
    return accept(socket, (sockaddr*)&addr, &addrSize);
}

void Channel::initReciever() {
    using namespace std;

    int connectionListener = socket(AF_INET, SOCK_STREAM, 0);
    if (connectionListener == -1) {
        cerr << "Can't create a socket! Quitting" << endl;
    }
    sockaddr_in addr = getLocalAddr(portToRecv);
    setListeningToConnection(addr, connectionListener);
    recieverSocket = acceptConnection(addr, connectionListener);

    if (recieverSocket != -1) {
        cerr << "accepted\n";
    } else {
        cerr << "not accepted\n";
    }
}

void Channel::initSender() {
    using namespace std;

    senderSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (recieverSocket == -1) {
        cerr << "Can't create a socket! Quitting" << endl;
    }
    sockaddr_in addr = getLocalAddr(portToSend);
    if (connect(senderSocket, (sockaddr*)&addr, sizeof(addr)) == 0) {
        cerr << "connected!\n";
    } else {
        cout << "not connected\n";
    }
}

void Channel::startRecieving() {
    using namespace std;

    while(true) {
        sleep(1);
        char msg[256] = "not hey";
        recv(recieverSocket, msg, 256, 0);
        std::cerr << msg << endl;
    }
}

void Channel::startSending() {
    using namespace std;

    cerr << "send started";
    while(true) {
        sleep(1);
        char msg[256] = "hey";
        send(senderSocket, msg, 256, 0);
    }
}

void Channel::startSendingDebug(char c) {
    using namespace std;
    cerr << "send started";
    while(true) {
        sleep(1);
        char msg[256] = "a";
        msg[0] = c;
        send(senderSocket, msg, 256, 0);
    }
}

void Channel::startRecievingToFile(
        const std :: string &filename) { //doesnt work
    using namespace std;

    cerr << "recieving started";
    char msg[10] = "hey";
    for (int i = 0; i < 2; ++i)  {
        recv(senderSocket, msg, 10, 0);
        cerr << msg << " recv" << endl;
    }
}

void getNChars(size_t n, char*a,
               std::ifstream& f) {
    using namespace std;

    for (size_t i = 0; i < n; ++i) {
        f >> a[i];
    }
}

void Channel::startSendingFromFile(
        const std::string &filename) {
    using namespace std;

    cerr << "sending started";
    char msg[256] = "hey";
    ifstream file(filename);
    for (int i = 0; i < 100; ++i) {
        sleep(1);
        getNChars(10, msg, file);
        send(senderSocket, msg, 256, 0);
        //cerr << msg << " sent" << endl;
    }
}

Network::Network(): releaseTheKraken(0), freePort(8000) {}

void Network::createNodes(int nodesAmount) {
    using namespace std;

    nodes = {};
    for (int i = 0; i < nodesAmount; ++i) {
        nodes.push_back(Node(i));
    }
}

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

void Network::startTwoChannelsDebug(Channel& left, Channel& right, char c) {
    startLeftToSendRightDebug(left, right, c);
    startLeftToSendRightDebug(right, left, c + 1);
}

void Network::startDebugNetwork() {
    using namespace std;

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
