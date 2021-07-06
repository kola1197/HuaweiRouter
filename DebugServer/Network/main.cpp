#include "mainwindow.h"
#include <thread>
#include <QApplication>
#include <QtCore/QDir>
#include <cstdlib>
#include <ctime>
#include <Utils/Settings.h>
#include <Utils/sout.h>
#include <SimulationCore/SendingQueue.h>
#include <Utils/CpuInfo.h>
#include <QSurfaceFormat>
#include <unistd.h>

using namespace std;

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
    NodeLoadMessage n;
    n.load = 1239;
    n.secondLoad = 1239;
    n.priority = HIGH;

    NodeLoadForDeTailMessage m;
    m.load = 12391239;
    m.secondLoad = 12391239;
    m.priority= MEDIUM;

    PacketMessage packetMessage{};
    packetMessage.priority = HIGH;
    packetMessage.to = 1;
    packetMessage.checkSum = Messages::getChecksum(&packetMessage);
    packetMessage.firstCheckSum = Messages::getChecksum(&packetMessage);
    s1->sendMessage(n);
    s1->sendMessage(t);
    s1->sendMessage(packetMessage);
    s1->sendMessage(m);
    //usleep(100000);
    s1->sendMessage(tt);
    s1->sendMessage(t);
    s1->sendMessage(n);
    s1->sendMessage(t);
    s1->sendMessage(t);
    s1->sendMessage(tt);
    s1->sendMessage(m);

    while (true)
    {
        usleep(100000);
    }
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QSurfaceFormat fmt;
    fmt.setSamples(20);
    QSurfaceFormat::setDefaultFormat(fmt);
    srand(time(NULL));
    a.setApplicationName("Simulation");
    MainWindow w;
    w.setWindowTitle("Simulation");
    QIcon icon;
    icon.addFile(":/resource/images/icon1.ico", QSize(), QIcon::Normal, QIcon::Off);
    w.setWindowIcon(icon);
    w.setWindowIconText("Simulation");
    w.show();
    return a.exec();

}