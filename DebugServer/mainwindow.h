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
    void on_algorithmBox_currentIndexChanged(int index);

public slots:
    void on_settingsButton_released();
    void repaintOGLWidget();
    void onBtnClicked();
    void unBlockInterface();
    //void get_system_message(SystemMessage m);
private:
    Simulation* simulation;
    Ui::MainWindow *ui;
    void connectSlots();
    void createUI();
    Graph savedGraph;

    void checkSimulationStatus();
    bool tableResized = false;

    void createAlgprithmComboBox();
    void blockInterface();
    bool simulationIsActive = false;
    void updateStartButtonText();
    void disconnectSlots();
};
#endif // MAINWINDOW_H
