#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "iostream"
#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>
#include <qtextedit.h>
#include <QVulkanFunctions>
#include <QWidget>
#include <QVBoxLayout>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qRegisterMetaType<DebugMessage>("DebugMessage");             //now we can use this messages in signal/slot system as QObjects
    qRegisterMetaType<SystemMessage>("SystemMessage");
    qRegisterMetaType<PacketMessage>("PacketMessage");

    ui->setupUi(this);
    createUI();
    //connect(ui->openGLWidget,SIGNAL(transmit_info(QString)),this,SLOT(setEllipseInfo(QString)));
    draw();
}

MainWindow::MainWindow(QWidget *parent, VulkanWindow *w)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    qRegisterMetaType<DebugMessage>("DebugMessage");             //now we can use this messages in signal/slot system as QObjects
    qRegisterMetaType<SystemMessage>("SystemMessage");
    qRegisterMetaType<PacketMessage>("PacketMessage");

    ui->setupUi(this);
    QWidget *wrapper = QWidget::createWindowContainer(w);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(wrapper);
    ui->WidgetForVulkan->setLayout(layout);
    createUI();
    //connect(ui->openGLWidget,SIGNAL(transmit_info(QString)),this,SLOT(setEllipseInfo(QString)));
    draw();
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::draw()
{

}

void MainWindow::createUI()
{
    ui->tableWidget->clear();
    //int s = ui->tableWidget->rowCount();
    while (ui->tableWidget->rowCount()>0) {
        ui->tableWidget->removeRow(0);
    }
    //connect(ui->tableWidget, SIGNAL(cellChanged(int,int)),this,SLOT(cellChangedCheck(int,int)));
    ui->tableWidget->setColumnCount(7);
    ui->tableWidget->setShowGrid(true);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    //QStringList headers =
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() <<trUtf8("№") <<trUtf8("id") <<trUtf8("Type") <<trUtf8("From") <<trUtf8("To")<<trUtf8("Current Position") <<trUtf8("Delete"));
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->hideColumn(0);
    /*for (int j=0;j<ui->openGLWidget->graph.packets.size();j++)
    {
        ui->tableWidget->insertRow(j);
        ui->tableWidget->setItem(j,0, new QTableWidgetItem(QString::number(j)));
        ui->tableWidget->setItem(j,1, new QTableWidgetItem(QString::number(ui->openGLWidget->graph.packets[j].id)));

        QComboBox *box = new QComboBox();
        box->addItems(QStringList() <<trUtf8("DEFAULT"));      //enums from PacketMessage in right order!!!!
        box->setCurrentIndex(ui->openGLWidget->graph.packets[j].type);
        ui->tableWidget->setCellWidget(j,2,box);
        connect(box, SIGNAL(currentIndexChanged(int)),this, SLOT(cellIndexChanged(int)));

        QTextEdit *editFrom = new QTextEdit();
        editFrom->setToolTip(QString::number(j));
        editFrom->setToolTipDuration(0);
        editFrom->setText(QString::number(ui->openGLWidget->graph.packets[j].from));
        ui->tableWidget->setCellWidget(j,3,editFrom);
        connect(editFrom, SIGNAL(textChanged()),this,SLOT(cellFromTextChanged()));

        QTextEdit *editTo = new QTextEdit();
        editTo->setToolTip(QString::number(j));
        editTo->setToolTipDuration(0);
        editTo->setText(QString::number(ui->openGLWidget->graph.packets[j].to));
        ui->tableWidget->setCellWidget(j,4,editTo);
        connect(editTo, SIGNAL(textChanged()),this,SLOT(cellToTextChanged()));

        ui->tableWidget->setItem(j,5, new QTableWidgetItem(QString::number(ui->openGLWidget->graph.packets[j].currentPosition)));

        QPushButton *btn = new QPushButton();
        btn->setText("Delete");
        btn->setToolTip(QString::number(j));
        btn->setToolTipDuration(0);
        ui->tableWidget->setCellWidget(j,6,btn);


        connect( btn, SIGNAL( clicked( bool ) ), SLOT( onBtnClicked() ) );
    }*/
    ui->tableWidget->resizeColumnsToContents();
}

void MainWindow::onBtnClicked()
{
    if( QPushButton* btn = qobject_cast< QPushButton* >( sender() ) )
    {
        int index = btn->toolTip().toInt();
        //QMessageBox::information( this, "The button was clicked", QString::number(index));
        //ui->openGLWidget->graph.packets.erase(ui->openGLWidget->graph.packets.begin()+index);
        createUI();
        repaintOGLWidget();
    }
}

void MainWindow::cellIndexChanged(int i)
{
    if (QComboBox* box = qobject_cast< QComboBox* >( sender() ))
    {
        int index = box->toolTip().toInt();
        if (i == 0)
        {
            //ui->openGLWidget->graph.packets[index].type = PacketMessage::DEFAULT_PACKET;
        }
    }
}

void MainWindow::cellFromTextChanged()
{
    if (QTextEdit* edit = qobject_cast< QTextEdit* >( sender() ))
    {
        int index = edit->toolTip().toInt();
        QString s = edit->toPlainText();

        bool b;
        int data = s.toInt(&b,10);
        if (!b)                                  //check data
        {
            data = -1;
            QMessageBox::information( this, "The button was clicked", edit->toPlainText());
            //QColor c ()
            edit->setTextColor(QColor(255,0,0));
            edit->setText("-1");
        }
        else
        {
            if (edit->textColor() != QColor(0,0,0) && data != -1 ){
            edit->setTextColor(QColor(0,0,0));
            edit->setText(QString::number(data));
            }
        }
        //ui->openGLWidget->graph.packets[index].from = data;
    }
}

void MainWindow::cellToTextChanged()
{
    if (QTextEdit* edit = qobject_cast< QTextEdit* >( sender() ))
    {
        int index = edit->toolTip().toInt();
        QString s = edit->toPlainText();

        bool b;
        int data = s.toInt(&b,10);
        if (!b)                                  //check data
        {
            data = -1;
            QMessageBox::information( this, "The button was clicked", edit->toPlainText());
            //QColor c ()
            edit->setTextColor(QColor(255,0,0));
            edit->setText("-1");
        }
        else
        {
            if (edit->textColor() != QColor(0,0,0) && data != -1 ){
            edit->setTextColor(QColor(0,0,0));
            edit->setText(QString::number(data));
            }
        }
        //ui->openGLWidget->graph.packets[index].to = data;
    }
}

void MainWindow::cellChangedCheck(int row,int column)
{
    QMessageBox::information( this, "The button was clicked", QString::number(row)+" - "+QString::number(column));

}

void MainWindow::SavePacketClick()
{
    for (int i=0;i<ui->tableWidget->rowCount();i++)
    {
        QWidget *_editTo = ui->tableWidget->cellWidget(i,3);
        QPushButton *editTo = new QPushButton(_editTo);
        QMessageBox::information( this, "The button was clicked", editTo->text());
        //QString s = ui->tableWidget->itemAt(i,3)->data().toReal();
    }
}

void MainWindow::on_saveButton_released()
{
    QString q = QFileDialog::getSaveFileName(this,"Save Graph To File","", "Graph (*.grf);;All Files (*)");
    //ui->openGLWidget->graph.save(q);
}

void MainWindow::on_loadButton_released()
{
    //QString q = QFileDialog::getOpenFileName(this,"Load Graph From File","", "Graph (*.grf);;All Files (*)");
    //ui->openGLWidget->graph.load(q);
    //ui->openGLWidget->update();
    //createUI();
}

void MainWindow::setEllipseInfo(QString q)
{
    std::cout<<"info label"<<std::endl;
    ui->infoLabel->setText(q);
}

void MainWindow::on_deleteButton_released()
{
    //ui->openGLWidget->graph.deleteActiveEllips();
    //ui->openGLWidget->update();
}

void MainWindow::on_startButton_released()   // lock the screen and start simulation
{
    //ui->openGLWidget->screenLocked = true;
    //simulation = Simulation(&ui->openGLWidget->graph);
    //simulation.Start();
    //connectSlots();
}

void MainWindow::updateTable()
{
    createUI();
    //std::cout<<"updating table"<<std::endl;
}

void MainWindow::connectSlots()
{/*
    connect(&ui->openGLWidget->graph,SIGNAL(repaint()),this,SLOT(repaintOGLWidget()));
    connect(&ui->openGLWidget->graph,SIGNAL(updateTable()),this,SLOT(updateTable()));

    for (int i=0;i<simulation.debugServer->connections.size();i++ )
    {
        connect(simulation.debugServer->connections[i],SIGNAL(transmit_to_gui(SystemMessage)),simulation.debugServer,SLOT(get_message_for_debug(SystemMessage)));
        connect(simulation.debugServer->connections[i],SIGNAL(transmit_to_gui(DebugMessage)),&ui->openGLWidget->graph,SLOT(get_system_message(DebugMessage)));
    }*/
    //connect(simulation.debugServer->debugConnection,SIGNAL(transmit_to_gui(SystemMessage)),&ui->openGLWidget->graph,SLOT(get_system_message(SystemMessage)));

    //connect(simulation.debugServer->debugConnection,SIGNAL(transmit_to_gui(SystemMessage)),this,SLOT(get_system_message(SystemMessage)));
}

void MainWindow::repaintOGLWidget()
{
    //ui->openGLWidget->update();

}

void MainWindow::AddButtonClick()
{
    //ui->openGLWidget->graph.addPacket();
    createUI();
}

void MainWindow::onVulkanInfoReceived(const QString &text)
{
    //m_info->setPlainText(text);
}

void MainWindow::onFrameQueued(int colorValue)
{
    //m_number->display(colorValue);
}

//vulcans part

QVulkanWindowRenderer *VulkanWindow::createRenderer()
{
    return new VulkanRenderer(this);
}

VulkanRenderer::VulkanRenderer(VulkanWindow *w)
    : TriangleRenderer(w)
{
}

void VulkanRenderer::initResources()
{
    TriangleRenderer::initResources();

    QVulkanInstance *inst = m_window->vulkanInstance();
    m_devFuncs = inst->deviceFunctions(m_window->device());

    QString info;
    info += QString::asprintf("Number of physical devices: %d\n", m_window->availablePhysicalDevices().count());

    QVulkanFunctions *f = inst->functions();
    VkPhysicalDeviceProperties props;
    f->vkGetPhysicalDeviceProperties(m_window->physicalDevice(), &props);
    info += QString::asprintf("Active physical device name: '%s' version %d.%d.%d\nAPI version %d.%d.%d\n",
                              props.deviceName,
                              VK_VERSION_MAJOR(props.driverVersion), VK_VERSION_MINOR(props.driverVersion),
                              VK_VERSION_PATCH(props.driverVersion),
                              VK_VERSION_MAJOR(props.apiVersion), VK_VERSION_MINOR(props.apiVersion),
                              VK_VERSION_PATCH(props.apiVersion));

    info += QStringLiteral("Supported instance layers:\n");
    for (const QVulkanLayer &layer : inst->supportedLayers())
        info += QString::asprintf("    %s v%u\n", layer.name.constData(), layer.version);
    info += QStringLiteral("Enabled instance layers:\n");
    for (const QByteArray &layer : inst->layers())
        info += QString::asprintf("    %s\n", layer.constData());

    info += QStringLiteral("Supported instance extensions:\n");
    for (const QVulkanExtension &ext : inst->supportedExtensions())
        info += QString::asprintf("    %s v%u\n", ext.name.constData(), ext.version);
    info += QStringLiteral("Enabled instance extensions:\n");
    for (const QByteArray &ext : inst->extensions())
        info += QString::asprintf("    %s\n", ext.constData());

    info += QString::asprintf("Color format: %u\nDepth-stencil format: %u\n",
                              m_window->colorFormat(), m_window->depthStencilFormat());

    info += QStringLiteral("Supported sample counts:");
    const QVector<int> sampleCounts = m_window->supportedSampleCounts();
    for (int count : sampleCounts)
        info += QLatin1Char(' ') + QString::number(count);
    info += QLatin1Char('\n');

    emit static_cast<VulkanWindow *>(m_window)->vulkanInfoReceived(info);
}

void VulkanRenderer::startNextFrame()
{
    TriangleRenderer::startNextFrame();
    emit static_cast<VulkanWindow *>(m_window)->frameQueued(int(m_rotation) % 360);
}
