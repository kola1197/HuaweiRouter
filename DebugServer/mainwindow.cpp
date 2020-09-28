#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "iostream"
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->openGLWidget,SIGNAL(transmit_info(QString)),this,SLOT(setEllipseInfo(QString)));
    draw();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::draw()
{

}

void MainWindow::on_saveButton_released()
{
    QString q = QFileDialog::getSaveFileName(this,"Save Graph To File","", "Graph (*.grf);;All Files (*)");
    ui->openGLWidget->graph.save(q);
}

void MainWindow::on_loadButton_released()
{
    QString q = QFileDialog::getOpenFileName(this,"Load Graph From File","", "Graph (*.grf);;All Files (*)");
    ui->openGLWidget->graph.load(q);
    ui->openGLWidget->update();
}

void MainWindow::setEllipseInfo(QString q)
{
    std::cout<<"info label"<<std::endl;
    ui->infoLabel->setText(q);
}

void MainWindow::on_deleteButton_released()
{
    ui->openGLWidget->graph.deleteActiveEllips();
    ui->openGLWidget->update();
}

void MainWindow::on_startButton_released()   // lock the screen and start simulation
{
    ui->openGLWidget->screenLocked = true;
    simulation = Simulation(&ui->openGLWidget->graph);
    simulation.Start();
}
