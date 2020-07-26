#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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
    QString q = QFileDialog::getSaveFileName(this,"Save Graph To File","", "Graph (*.grp);;All Files (*)");
    ui->openGLWidget->graph.save(q);
}

void MainWindow::on_loadButton_released()
{
    QString q = QFileDialog::getOpenFileName(this,"Load Graph From File","", "Graph (*.txt);;All Files (*)");
    ui->openGLWidget->graph.load(q);
    ui->openGLWidget->update();
}
