#include "channel.h"
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
    char msg[1000 * 1000] = "hey";
    for (int i = 0; i < 1000000; ++i)  {
        recv(senderSocket, msg, 1000 * 1000, 0);
    }
}

static void getNChars(size_t n, char*a,
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
    char msg[1000 * 1000] = "hey";
    ifstream file(filename);
    for (int i = 0; i < 1000000; ++i) {
        send(senderSocket, msg, 1000 * 1000, 0);
    }
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
