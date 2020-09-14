#include "node.h"

Node::Node(int id):id(id) {}

size_t Node::getChannelsAmount() const {
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


