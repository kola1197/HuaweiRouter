#ifndef CHANNEL_H
#define CHANNEL_H
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

//private:
    int id;
    int nodeId;

    char dataRecieved[10000];
    char dataToSend[10000];

    int portToRecv;
    int portToSend;

    int recieverSocket;
    int senderSocket;

    //
    //sockaddr_in getLocalAddr(int port);

    //int acceptConnection(sockaddr_in& addr, int socket);

    //void setListeningToConnection(
    //        sockaddr_in& addr, int socket);

    void bindSocket(int sock, int port);
};
#endif // CHANNEL_H
