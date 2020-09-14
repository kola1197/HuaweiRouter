#include "mainwindow.h"
#include <thread>
#include <Network/node.h>
#include <Network/network.h>
#include <QApplication>


void setLeftToSendRight(Channel& left, Channel& right) {
    using namespace std;
    thread initRecieverThread(&Channel::initReciever, &left);
    thread initSenderThread(&Channel::initSender, &right);
    initRecieverThread.join();
    initSenderThread.join();
}

void connectChannels(Channel& left, Channel& right) {
    using namespace std;
    setLeftToSendRight(left, right);
    setLeftToSendRight(right, left);
}

void startLeftToSendRight(Channel& left, Channel& right) {
    using namespace std;
        std::thread startSendingThread
                (&Channel::startSending, left);
        std::thread startRecievingThread
                (&Channel::startRecieving, right);
        startSendingThread.detach();
        startRecievingThread.detach();
}

std::string firstStorageName = "firstStorage.txt";
std::string secondStorageName = "secondStorage.txt";
std::string firstDataName = "firstData.txt";
std::string secondDataName = "secondData.txt";
void initFiles() {
    using namespace std;

    ofstream firstStorage(firstStorageName);
    ofstream secondStorage(secondStorageName);
    string nineA = "AAAAAAAAA";
    string nineB = "BBBBBBBBB";
    string nineC = "CCCCCCCCC";
    string nineD = "DDDDDDDDD";
    string firstData;
    string secondData;
    for(int i = 0; i < 10000; ++i) {
        firstData += nineA + nineA + nineB + nineA;
        secondData += nineC + nineD + nineD;
    }
    firstStorage << firstData;
    secondStorage << secondData;
}

std::string info(1000 * 1000 * 1000 * 100, '0');

void TestNetwork() { //some sockets send each otther 'a'-'f'
    using namespace std;
    int port = 1206;
    int nodesAmount = 4;
    //vector<pair<int,int>> adjList = { {0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3}}; //last clock was like 26 000 000
    //vector<pair<int,int>> adjList = { {0, 1}, {0, 2} }; //last clock was like 7 000 000
    vector<pair<int,int>> adjList = { {0, 1} }; //last clock was like 3 500 000 //oriented was 2 000 000, btw difs are 2 900 000 1 500 000
    Network n(adjList, port, nodesAmount);

    n.connectNetwork(); 
    std:: cout << endl << clock() << endl;
    //n.startInfoNetwork(&info);
    n.startInfoOrientedNetwork(&info);
    while(true) {}
}

void testMultiThreading() {
    using namespace std; 

    int port = 12300;
    int nodesAmount = 4;
    vector<pair<int,int>> adjList = { {0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3}};

    Network n(adjList, port, nodesAmount);

    n.connectNetwork();
    n.startDebugNetwork();
    //while (true) {}
}

int main(int argc, char *argv[]) {
    using namespace std;

    TestNetwork();

//    int port1 = 8453;
//    int port2 = 8459;
//    int port3 = 8350;
//    int port4 = 8351;
//    Channel a(port1, port2);
//    Channel b(port2, port1);
//    std::thread t2(&Channel::initReciever, b);
//    std::thread t1(&Channel::initSender, a);

//    t1.join();
//    t2.join();
//    clock_t time1 = clock();

//    std::thread t3(&Channel::startSendingFromFile, a, "bigfile");
//    std::thread t4(&Channel::startRecievingToFile, b, "anottherbigfile");
//    t3.join();
//    t4.join();
//    auto time2 = clock();
//    std :: cout << time2 << ' ' << time1 << endl;
    //testMultiThreading();
}

//int main(int argc, char *argv[]) {   network works too
//    using namespace std;
//    int port = 31006;
//    int nodesAmount = 3;
//    vector<pair<int,int>> adjList = { {0, 1}, {1, 2}, {0, 2}};

//    Network n(adjList, port, nodesAmount);

//    n.connectNetwork();
//    while (true) {

//    }
//}


//int main(int argc, char *argv[]) {                     TEST DAT TIS ALL WORKIN
//    using namespace std;
//    initFiles();
//    int port1 = 7000;
//    int port2 = 8000;
//    Channel left(port1, port2);
//    Channel right(port2, port1);
//    connectChannels(left, right);

//    std::thread t1(&Channel::startSending, left);
//    std::thread t2(&Channel::startRecieving, right);
//    t1.detach();
//    t2.detach();
//    std::thread t3(&Channel::startSending, right);
//    std::thread t4(&Channel::startRecieving, left);
//    t3.detach();
//    t4.detach();
//    while (true) {

//    }
//}
