#include "mainwindow.h"
#include <thread>
#include <Network/node.h>
#include <Network/network.h>
#include <QApplication>
#include <QtCore/QDir>
#include <cstdlib>
#include <ctime>
#include <Utils/Settings.h>
#include <Utils/sout.h>

using namespace std;

static void setLeftToSendRight(Channel& left, Channel& right) {
    thread initRecieverThread(&Channel::initReciever, &left);
    thread initSenderThread(&Channel::initSender, &right);
    initRecieverThread.join();
    initSenderThread.join();
}

static void connectChannels(Channel& left, Channel& right) {
    setLeftToSendRight(left, right);
    setLeftToSendRight(right, left);
}

static void startLeftToSendRight(Channel& left, Channel& right) {
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

static void TestNetworkDebug() { //some sockets send each otther 'a'-'f'
    int port = 10000;
    int nodesAmount = 4;
    vector<pair<int,int>> adjList = { {0, 1}, {1, 2}, {0, 3}};
    Network n(adjList, port, nodesAmount);
    n.connectNetwork();
    n.startDebugNetwork();
    while(true) {
        sleep(1000);
    }
}

static void testTwoNodesDebug();

static void testTwoChannelsDebug();

std::string info(1000 * 1000 * 1000 * 100, '0');

void testMultiThreading(int sendingThreadsAmount) {
    int port = 24000;  //CHANGE DIS +- ~10000 IF ITS CANT CONNECT LIKE: "CHANELL 0 or 1 cannot connect"
    int nodesAmount = sendingThreadsAmount + 1;
    vector<pair<int,int>> adjList{};
    for (auto i = 0; i < sendingThreadsAmount; ++i) {
        adjList.push_back({i, i + 1});
    }
    Network network(adjList, port, nodesAmount);
    network.connectNetwork();
    std:: cout << endl << "time of start:"<< clock() << endl;
    network.startInfoOrientedNetwork(&info);
    sleep(1000);
}
//vector<pair<int,int>> adjList = { {0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3}}; //last clock was like 26 000 000
//vector<pair<int,int>> adjList = { {0, 1}, {0, 2} }; //last clock was like 7 000 000
//vector<pair<int,int>> adjList = { {0, 1} }; //last clock was like 3 500 000 //oriented was 2 000 000, btw difs are 2 900 000 1 500 000

void randomTest()
{
    srand(time(NULL));
    int a = rand() % (10);
    int b = rand() % (10);
    int c = rand() % (10);
    sim::sout<<a<<", "<<b<<", "<<c<<sim::endl;
}

void initSettings()
{
    Settings::setDebugFirstPortNum(5555);
    Settings::setConnectionsFirstPortNum(6666);
}

void soutTest()
{
    std::thread thr([]() {
        for (int i=0;i<10;i++) {
            sim::sout  << "test1 " <<1<<sim::endl;
            usleep(100);
        }
    });
    std::thread thr1([]() {
        for (int i=0;i<10;i++) {
            sim::sout  << "test2 " <<2<<sim::endl;
            usleep(100);
        }
    });
    thr.detach();
    thr1.detach();
    while (true)
    {
        usleep(10000);
    }
}

int main(int argc, char *argv[]) {
    //soutTest();
    QApplication a(argc, argv);
    a.setApplicationName("Simulation");
    MainWindow w;
    w.setWindowTitle("Simulation");
    QIcon icon;
    icon.addFile(QStringLiteral("../icon1.ico"), QSize(), QIcon::Normal, QIcon::Off);
    w.setWindowIcon(icon);
    w.setWindowIconText("Simulation");
    w.show();
    return a.exec();

    //using namespace std;
    //TestNetworkDebug();
    //testMultiThreading(4);
}

