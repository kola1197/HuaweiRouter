#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void draw();
private slots:
    void on_saveButton_released();
    void setEllipseInfo(QString info);
    void on_loadButton_released();

    void on_deleteButton_released();

    void on_startButton_released();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
