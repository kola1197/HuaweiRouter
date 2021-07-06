//
//  MainWindow for visualization of simulation process
//


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <SimulationCore/simulation.h>
#include "settingsform.h"

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

    void onnn_count_of_bytes_editingFinished();                      // QT thinks that "on_" is system slot ?!
    void onnn_send_interval_editingFinished();                       // QT thinks that "on_" is system slot ?!
    void onnn_lambda_editingFinished();
    void repaint_on_simulation_finish_done();

    void onnn_z_text_editingFinished();
    void onnn_u_text_editingFinished();
    void onnn_w_text_editingFinished();
signals:
    void simulation_finish_done();
public slots:
    void updateAllScreen();
    void onnn_settingsButton_released();                              // QT thinks that "on_" is system slot ?!
    void repaintOGLWidget();
    void onBtnClicked();
    void unBlockInterface();
    void updateCpuLabel();
    void updateEdgePerfomanceLabels();
    void onnn_break_chance_editingFinished();


    //void get_system_message(SystemMessage m);
private:
    int screenUpdateFrameCounter;
    void updatePacketsLabel();
    Simulation* simulation;
    Ui::MainWindow *ui;
    void connectSlots();
    void createUI();
    Graph savedGraph;

    QTimer* tmr;
    bool averageTimeShoved = true;
    void checkSimulationStatus();
    bool tableResized = false;

    void createAlgprithmComboBox();
    void blockInterface();
    bool simulationIsActive = false;
    void updateStartButtonText();
    void disconnectSlots();
    SettingsForm* settingsForm;

    void setDefaultSettings();
};
#endif // MAINWINDOW_H
