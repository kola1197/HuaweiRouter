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

#include <list>
#include <vector>


class NetworkManager { // connects nodes before whole work
public:

};

class Channel {
  public:
    Channel(int portToRecv, int portToSend);

    void init();
    void initReciever();
    void startRecieving();

    void initSender();
    void startSending();

    int acceptConnection(sockaddr_in& addr, int socket);
    void setListeningToConnection(sockaddr_in& addr, int socket);
    void bindSocket(int sock, int port);
    sockaddr_in getLocalAddr(int port);

    int portToRecv;
    int portToSend;
    int senderSocket;
    int recieverSocket;
};

class Node {
public:
    Node();
    void addChannel (int portToRecv, int portToSend, char* infoToSend);
    uint32_t getChannelsAmount() const;
private:
    uint32_t channels_amount;
    std::list<Channel> channel;
};

#endif // NODE_H
