#include "node.h"
#include <thread>

Node::Node()
{

}

Channel::Channel(int portToRecv, int portToSend)
        :portToRecv(portToRecv), portToSend(portToSend) {}

sockaddr_in Channel::getLocalAddr(int port) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    return addr;
}

void Channel::setListeningToConnection(sockaddr_in& addr, int socket) {
    bind(socket, (sockaddr*)&addr, sizeof(addr));
    listen(socket, 1);
}

int Channel::acceptConnection(sockaddr_in& addr, int socket) {
    socklen_t addrSize = sizeof(addr);
    return accept(socket, (sockaddr*)&addr, &addrSize);
}

void Channel::initReciever() {
    using namespace std;
    int connectionListener = socket(AF_INET, SOCK_STREAM, 0);
    if (connectionListener == -1) {
        cerr << "Can't create a socket! Quitting" << endl;
    }
    auto addr = getLocalAddr(portToRecv);
    //addr.sin_addr.s_addr = htons(INADDR_ANY);
//SOMAXCONN -> 1
    setListeningToConnection(addr, connectionListener);
    recieverSocket = acceptConnection(addr, connectionListener);
    if (recieverSocket != -1) {
        cerr << "accepted";
    } else {
        cout << "not accepted";
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
        cerr << "connected!";
    } else {
        cout << "not connected";
    }
}

void Channel::startRecieving() {
    while(true) {
        sleep(1);
        char msg[256] = "not hey";
        recv(recieverSocket, msg, 256, 0);
        std::cerr << msg;
    }
}

void Channel::startSending() {
    while(true) {
        sleep(1);
        char msg[256] = "hey";
        std::cin >> msg;
        send(senderSocket, msg, 256, 0);
    }
}

void Channel::init() {
    using namespace std;
    thread initSenderThread(&Channel::initSender, this);
    thread initRecieverThread(&Channel::initReciever, this);
    initSenderThread.join();
    initRecieverThread.join();
}
