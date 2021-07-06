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