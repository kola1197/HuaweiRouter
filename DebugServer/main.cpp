#include "mainwindow.h"
#include <thread>
#include <node.h>
#include <QApplication>

#include <QApplication>

int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
//    return a.exec();
    using namespace std;
    int port1 = 2000;
    int port2 = 2001;
    Channel send1recv2(port1, port2);
    Channel send2recv1(port2, port1);
    thread initRecieverThread(&Channel::initReciever, &send2recv1);
    thread initSenderThread(&Channel::initSender, &send1recv2);
    initRecieverThread.join();
    initSenderThread.join();
    cerr << "hey";

    std::thread t1(&Channel::startSending, send1recv2);
    std::thread t2(&Channel::startRecieving, send2recv1);
    t1.detach();
    t2.detach();

    int p1 = 3000;
    int p2 = 3001;
    Channel ch1(p1, p2);
    Channel ch2(p2, p1);
    thread th1(&Channel::initReciever, &ch1);
    thread th2(&Channel::initSender, &ch2);
    cerr << "hey";
    th1.join();
    th2.join();

    while(true) {

    }
}
