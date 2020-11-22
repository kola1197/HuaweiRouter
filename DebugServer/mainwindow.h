#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <SimulationCore/simulation.h>
#include <VulkanWidget/trianglerenderer.h>

class VulkanWindow;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    MainWindow(QWidget *parent = nullptr, VulkanWindow *w = nullptr);
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
    void onVulkanInfoReceived(const QString &text);
    void onFrameQueued(int colorValue);

    void repaintOGLWidget();
    void onBtnClicked();
    //void get_system_message(SystemMessage m);
private:
    Simulation simulation;
    Ui::MainWindow *ui;
    void connectSlots();
    void createUI();
};

class VulkanRenderer : public TriangleRenderer
{
public:
    VulkanRenderer(VulkanWindow *w);

    void initResources() override;
    void startNextFrame() override;
};

class VulkanWindow : public QVulkanWindow
{
    Q_OBJECT

public:
    QVulkanWindowRenderer *createRenderer() override;

signals:
    void vulkanInfoReceived(const QString &text);
    void frameQueued(int colorValue);
};

#endif // MAINWINDOW_H
