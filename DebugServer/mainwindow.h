#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <SimulationCore/simulation.h>

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
    void AddButtonClick();
    void SavePacketClick();
    void on_startButton_released();
    void cellChangedCheck(int row,int column);

    void cellFromTextChanged();
    void cellToTextChanged();
    void cellIndexChanged(int i);
    void updateTable();
public slots:
    void repaintOGLWidget();
    void onBtnClicked();
    //void get_system_message(SystemMessage m);
private:
    Simulation simulation;
    Ui::MainWindow *ui;
    void connectSlots();
    void createUI();

    void checkSimulationStatus();
    bool tableResized = false;
};
#endif // MAINWINDOW_H
