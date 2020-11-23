#include "channel.h"
using namespace  std;

Channel::Channel(int portToRecv, int portToSend)
        : portToRecv(portToRecv),
          portToSend(portToSend) {}

Channel::Channel(int id, int nodeId,
                 int portToRecv, int portToSend,
                 char *infoToSend = nullptr,
                 char *dataToSend = nullptr)
        :id(id), nodeId(nodeId), portToRecv(portToRecv),
          portToSend(portToSend) {}

static sockaddr_in getLocalAddr(int port) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    return addr;
}

static void setListeningToConnection(
        sockaddr_in& addr, int socket) {
    bind(socket, (sockaddr*)&addr, sizeof(addr));
    listen(socket, 1);
}

static int acceptConnection(    //no static
        sockaddr_in& addr, int socket) {

    socklen_t addrSize = sizeof(addr);
    return accept(socket, (sockaddr*)&addr, &addrSize);
}

void Channel::initReciever() {
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
    senderSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (recieverSocket == -1) {
        cerr << "Can't create a socket! Quitting" << endl;
    }
    sockaddr_in addr = getLocalAddr(portToSend);
    while (connect(senderSocket, (sockaddr*)&addr, sizeof(addr)) != 0) {
        cerr << "Channel " << id << " not connected\n";
        sleep(1);
    }
    cerr << "Channel " << id << " connected!\n";
}

void Channel::startRecieving() {
    cerr << "recieving started" << endl;
    while(true) {
        sleep(1);
        char msg[256];
        recv(recieverSocket, msg, 256, 0);
    }
}

void Channel::startSending() {
    cerr << "sending started" << endl;
    while(true) {
        sleep(1);
        char msg[256] = "hey";
        send(senderSocket, msg, 256, 0);
    }
}

void Channel::startSendingDebug(char c) {
    cerr << "debug sending started" << endl;
    while(true) {
        sleep(1);
        char msg[256] = "a";
        msg[0] = c;
        send(senderSocket, msg, 256, 0);
    }
}

void Channel::startRecievingDebug() {
    cerr << "debug recieving started" << endl;
    while(true) {
        sleep(1);
        char msg[256];
        recv(recieverSocket, msg, 256, 0);
        cerr << msg;
    }
}

void Channel::startSendingInformation(
        string *informationPtr) {
    cerr << "information sending started" << endl;
    uint64_t infoSize = informationPtr->size(); //
    uint64_t step = infoSize / (1000 * 1000);
    for (uint64_t i = 0; i < infoSize; i += step) {
        auto ptr = (char*)&((*informationPtr)[i]);
        send(senderSocket, ptr, step, 0);
    }
    cerr << "send ended"  << endl;
}

void Channel::startRecievingInformation(uint64_t infoSize) {
    cerr << "information recieving started" << endl;
    uint64_t step = infoSize / (1000 * 1000);
    string buffer(step, '\n');
    char* ptr = &(buffer[0]);
    for (uint64_t i = 0; i < infoSize; i += step) {
       recv(recieverSocket, ptr, step, 0);
    }
    cerr << "recieve ended" << clock() << endl;
}

//void Channel::startSendingFromFile(
//        const std::string &filename) {
//    using namespace std;
//    cerr << "sending started";
//    char msg[100 * 100] = "hey";
//    ifstream file(filename);
//    for (int i = 0; i < 100; ++i) {
//        sleep(1);
//        getNChars(100 * 100, msg, file);
//        send(senderSocket, msg, 100 * 100, 0);
//        cerr << "smthing send!\n";
//    }
//}
