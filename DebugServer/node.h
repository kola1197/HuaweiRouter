#ifndef NODE_H
#define NODE_H
#include <channel.h>

class Node {
public:
    Node();
    Node(int id);
    void addChannel (int ChannelId,
                     int portToRecv, int portToSend,
                     char infoToSend[10000],
                     char dataToSend[10000]);
    Channel& getLastChannel();
    size_t getChannelsAmount() const;

    Channel& operator[] (int i);

//private:
    int id;
    std::vector<Channel> channels;
};
#endif // NODE_H
