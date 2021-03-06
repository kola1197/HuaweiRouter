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
#include <SimulationCore/SendingQueue.h>
#include <Utils/CpuInfo.h>

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
    int r[10]{0};
    srand(time(NULL));
    for (int i=0;i<10000;i++)
    {
        r[rand() % (10)] += 1;
    }
    /*
    srand(time(NULL));
    int a = rand() % (10);
    int b = rand() % (10);
    int c = rand() % (10);*/
    for (int i=0;i<10;i++) {
        sim::sout << r[i] << sim::endl;
    }
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

void newMessagesQueueTest()
{
    ServerConnection* s1 = new ServerConnection(5239, 0, 1, 0);
    ServerConnection* s2 = new ServerConnection(5239, 1, 0, 0);
    s1->awaitConnection();
    usleep(1000);
    s2->connectTo();
    TestMessage t;
    t.checkCode = 1001001;
    char test[] = "SUCKSESS!!!";
    for (int i=0;i<sizeof (test);i++)
    {
        t.text[i] = test[i];
    }
    TestMessage tt;
    tt.priority = HIGH;
    tt.checkCode = 239239;
    char test1[] = "IMPORTANT SUCKSESS!!! kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk";
    for (int i=0;i<sizeof (test1);i++)
    {
        tt.text[i] = test1[i];
    }
    PacketMessage packetMessage{};
    packetMessage.checkSum=2391197;
    packetMessage.priority = HIGH;
    packetMessage.to = 1;
    s1->sendMessage(t);
    s1->sendMessage(packetMessage);
    s1->sendMessage(t);
    //usleep(100000);
    s1->sendMessage(tt);
    s1->sendMessage(t);
    s1->sendMessage(t);
    s1->sendMessage(t);
    s1->sendMessage(tt);
    while (true)
    {
        usleep(100000);
    }
}

int main(int argc, char *argv[]) {
    //randomTest();
    //newMessagesQueueTest();
    //sim::sout<<CpuInfo::getCPUName()<<sim::endl;
    QApplication a(argc, argv);
    srand(time(NULL));
    a.setApplicationName("Simulation");
    MainWindow w;
    w.setWindowTitle("Simulation");
    QIcon icon;
    icon.addFile(":/resource/images/icon1.ico", QSize(), QIcon::Normal, QIcon::Off);
    //icon.addFile(QStringLiteral("../icon1.ico"), QSize(), QIcon::Normal, QIcon::Off);
    w.setWindowIcon(icon);
    w.setWindowIconText("Simulation");
    w.show();
    return a.exec();

    //using namespace std;
    //TestNetworkDebug();
    //testMultiThreading(4);
}

