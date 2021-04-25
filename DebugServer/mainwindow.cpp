#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "iostream"
#include "settingsform.h"
#include <QComboBox>
#include <QFileDialog>
#include <QMessageBox>
#include <qtextedit.h>
#include <zconf.h>
#include <Utils/Settings.h>
#include <Utils/sout.h>
#include <Utils/CpuInfo.h>
#include <SimulationCore/SimulationReport.h>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
        , ui(new Ui::MainWindow)
{
    qRegisterMetaType<DebugMessage>("DebugMessage");             //now we can use this messages in signal/slot system as QObjects
    qRegisterMetaType<SystemMessage>("SystemMessage");
    qRegisterMetaType<PacketMessage>("PacketMessage");
    //lastTableUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    //lastOGLWUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    ui->setupUi(this);
    setCentralWidget(ui->scrollArea);
    createUI();
    createAlgprithmComboBox();
    setDefaultSettings();
    connect(ui->openGLWidget,SIGNAL(transmit_info(QString)),this,SLOT(setEllipseInfo(QString)));
    connect(this,SIGNAL(simulation_finish_done()), this, SLOT(repaint_on_simulation_finish_done()));
    connect(ui->openGLWidget,SIGNAL(transmitActiveEdgeChange()),this,SLOT(updateEdgePerfomanceLabels()));
    draw();
    tmr = new QTimer();
    tmr->setInterval(84);
    //connect(tmr, SIGNAL(timeout()), this, SLOT(updateCpuLabel()));
    connect(tmr, SIGNAL(timeout()), this, SLOT(updateAllScreen()));
    tmr->start();
    //ui->openGLWidget->graph = new Graph();
}


void MainWindow::updateAllScreen()
{
    std::chrono::microseconds render1 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
    if (screenUpdateFrameCounter%21==0)
    {
        checkSimulationStatus();
    }
    createUI();
    std::chrono::microseconds render2 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
    ui->openGLWidget->update();
    std::chrono::microseconds render3 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
    if (screenUpdateFrameCounter%21==0)
    {
        updateCpuLabel();
        updatePacketsLabel();
        screenUpdateFrameCounter = 0;
    }
    screenUpdateFrameCounter++;
    std::chrono::microseconds end = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
    //sim::sout<<"updateAllScreen: 1) "<<(render2 - render1).count()<<"2) "<<(render3 - render2).count()<<"3) "<<(end - render3).count()<<" microseconds"<<sim::endl;
}

void MainWindow::updatePacketsLabel()
{
    int counter = 0;
    for (int i=0;i<ui->openGLWidget->graph.packets.size();i++)
    {
        counter += ui->openGLWidget->graph.packets[i].delivered? 1:0;
    }
    QString text = QString::number(counter)+"/"+QString::number(ui->openGLWidget->graph.packets.size());
    ui->PacketsCounterLabel->setText(text);
}

void MainWindow::updateCpuLabel()
{
    float limValue = 95.00f;
    float criticalTemp = 75.0f;
    std::vector<float> cpuUsage = CpuInfo::getCPULoad();
    bool limit = false;
    limit = cpuUsage[0] > limValue || limit;;
    QString result = "AVG] " + QString::number(cpuUsage[0], 'f',2)+"% ";
    int length = result.size();
    for (int i=1; i < cpuUsage.size();i=i+2)
    {
        limit = cpuUsage[i] > limValue || limit;
        result += QString::number(i-1)+"] " ;
        result += QString::number(cpuUsage[i], 'f',2)+"% ";
    }
    result += "\n             ";
    for (int i=0;i<length;i++)
    {
        result+=" ";
    }
    for (int i=2; i < cpuUsage.size();i=i+2)
    {
        limit = cpuUsage[i] > limValue || limit;
        result += QString::number(i-1)+"] " ;
        result += QString::number(cpuUsage[i], 'f',2)+"% ";
    }
    ui->CpuInfoLabel->setStyleSheet(limit ? "color: rgb(200, 0, 0)" : "color: rgb(0, 0, 0)");
    ui->CpuInfoLabel->setText(result);
    float ftemp = CpuInfo::getCPUTemp();
    ui->CpuTempLabel->setStyleSheet(ftemp > criticalTemp ? "color: rgb(200, 0, 0)" : "color: rgb(0, 0, 0)");
    QString temp = ftemp!=0? "CPU temp = "+QString::number(ftemp,'f',2)+"°C" : "Cannot read your CPU \n temp from driver";
    ui->openGLWidget->graph.cpuFrames++;
    if (limit)
    {
        ui->openGLWidget->graph.cpuLoadCriticalFrames++;
    }
    if (ftemp > criticalTemp)
    {
        ui->openGLWidget->graph.cpuTermCriticalFrames++;
    }
    ui->CpuTempLabel->setText(temp);
    if (simulationIsActive){
        ui->openGLWidget->graph.cpuCorrect = !limit && ftemp <= 75.0f && ui->openGLWidget->graph.cpuCorrect;
    }
}

void MainWindow::setDefaultSettings()
{
    Settings::setsendIntervalMS(330);
    Settings::setSendBytesPerInterval(64);
    Settings::setAlpha(10);
    updateEdgePerfomanceLabels();
//    ui->CountOfBytes->setText(QString::number(Settings::getSendBytesPerInterval()));
//    ui->sendIntervalMS->setText(QString::number(Settings::getsendIntervalMS()));
    ui->lambdaText->setText(QString::number(Settings::getAlpha()));
    ui->zText->setText(QString::number(Settings::getZCoef()));
    ui->uText->setText(QString::number(Settings::getUCoef()));
    ui->wText->setText(QString::number(Settings::getWCoef()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createAlgprithmComboBox()
{
    ui->algorithmBox->addItems(QStringList() <<trUtf8("RANDOM") << trUtf8("DRILL") << trUtf8("LOCAL VOTING") << trUtf8("DE TAIl") << trUtf8("MY LOCAL VOTING") <<  trUtf8("LOCAL FLOW")  /*<< trUtf8("LocalFlow")*/);
    //ui->algorithmBox.
}



void MainWindow::draw()
{

}
void MainWindow::createUI()
{
    if (ui->openGLWidget->graph.needReaintTable || true) {
        if (ui->openGLWidget->graph.packets.size() != ui->tableWidget->rowCount()) {
            ui->tableWidget->clear();
            //int s = ui->tableWidget->rowCount();
            while (ui->tableWidget->rowCount() > 0) {
                ui->tableWidget->removeRow(0);
            }
            //connect(ui->tableWidget, SIGNAL(cellChanged(int,int)),this,SLOT(cellChangedCheck(int,int)));
            ui->tableWidget->setColumnCount(8);
            ui->tableWidget->setShowGrid(true);
            ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
            //QStringList headers =
            ui->tableWidget->setHorizontalHeaderLabels(
                    QStringList() << trUtf8("№") << trUtf8("    id    ") << trUtf8("    Function    ")
                                  << trUtf8("    From    ") << trUtf8("    To    ")
                                  << trUtf8("    Current Position    ")
                                  << trUtf8("    Delivering time (ms)    ") << trUtf8("Delete"));
            ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
            ui->tableWidget->hideColumn(0);
            QTableWidgetItem *protoitem = new QTableWidgetItem();
            protoitem->setTextAlignment(Qt::AlignmentFlag::AlignCenter);

            for (int j = 0; j < ui->openGLWidget->graph.packets.size(); j++) {
                ui->tableWidget->insertRow(j);

                QTableWidgetItem *newitem = protoitem->clone();
                newitem->setText(QString::number(j));
                ui->tableWidget->setItem(j, 0, newitem);

                QTableWidgetItem *newitem1 = protoitem->clone();
                newitem1->setText(QString::number(ui->openGLWidget->graph.packets[j].id));
                ui->tableWidget->setItem(j, 1, newitem1);

                QComboBox *box = new QComboBox();
                box->addItems(QStringList() << trUtf8("DEFAULT"));      //enums from PacketMessage in right order!!!!
                box->setCurrentIndex(ui->openGLWidget->graph.packets[j].type);
                ui->tableWidget->setCellWidget(j, 2, box);
                connect(box, SIGNAL(currentIndexChanged(int)), this, SLOT(cellIndexChanged(int)));

                QTextEdit *editFrom = new QTextEdit();
                editFrom->setToolTip(QString::number(j));
                editFrom->setToolTipDuration(0);
                editFrom->setAlignment(Qt::AlignmentFlag::AlignCenter);
                editFrom->setText(QString::number(ui->openGLWidget->graph.packets[j].from));
                ui->tableWidget->setCellWidget(j, 3, editFrom);
                connect(editFrom, SIGNAL(textChanged()), this, SLOT(cellFromTextChanged()));

                QTextEdit *editTo = new QTextEdit();
                editTo->setToolTip(QString::number(j));
                editTo->setToolTipDuration(0);
                editTo->setAlignment(Qt::AlignmentFlag::AlignCenter);
                editTo->setText(QString::number(ui->openGLWidget->graph.packets[j].to));
                ui->tableWidget->setCellWidget(j, 4, editTo);
                connect(editTo, SIGNAL(textChanged()), this, SLOT(cellToTextChanged()));

                QTableWidgetItem *newitem2 = protoitem->clone();
                newitem2->setText(QString::number(ui->openGLWidget->graph.packets[j].currentPosition));
                ui->tableWidget->setItem(j, 5, newitem2);
                //ui->tableWidget->setItem(j,5, new QTableWidgetItem(QString::number(ui->openGLWidget->graph.packets[j].currentPosition)));

                QString deliveringTime = ui->openGLWidget->graph.packets[j].delivered ? QString::number(
                        ui->openGLWidget->graph.packets[j].timeOnCreation.count()) : "Not delivered";
                QTableWidgetItem *newitem3 = protoitem->clone();
                newitem3->setText(deliveringTime);
                ui->tableWidget->setItem(j, 6, newitem3);
                //ui->tableWidget->setItem(j,6, new QTableWidgetItem(deliveringTime));

                QPushButton *btn = new QPushButton();
                btn->setText("Delete");
                btn->setToolTip(QString::number(j));
                btn->setToolTipDuration(0);
                ui->tableWidget->setCellWidget(j, 7, btn);

                connect(btn, SIGNAL(clicked(bool)), SLOT(onBtnClicked()));
            }
            if (!tableResized) {
                ui->tableWidget->resizeColumnsToContents();
                //ui->tableWidget->setColumnWidth(7, 10);
                if (ui->openGLWidget->graph.packets.size() > 0) {
                    tableResized = true;
                }
            }
        }
        else{
            ui->openGLWidget->graph.packetsToUpdateListMutex.lock();
            QTableWidgetItem *protoitem = new QTableWidgetItem();
            protoitem->setTextAlignment(Qt::AlignmentFlag::AlignCenter);
            for (int j = 0; j < ui->openGLWidget->graph.packets.size(); j++) {
                for (int i = 0; i< ui->openGLWidget->graph.tableIndexesToUpdate.size(); i++)
                {
                    if (j == ui->openGLWidget->graph.tableIndexesToUpdate[i])
                    {
                        QTableWidgetItem *newitem2 = protoitem->clone();
                        newitem2->setText(QString::number(ui->openGLWidget->graph.packets[j].currentPosition));
                        ui->tableWidget->setItem(j, 5, newitem2);
                        //ui->tableWidget->setItem(j,5, new QTableWidgetItem(QString::number(ui->openGLWidget->graph.packets[j].currentPosition)));
                        QString deliveringTime = ui->openGLWidget->graph.packets[j].delivered ? QString::number(
                                ui->openGLWidget->graph.packets[j].timeOnCreation.count()) : "Not delivered";
                        QTableWidgetItem *newitem3 = protoitem->clone();
                        newitem3->setText(deliveringTime);
                        ui->tableWidget->setItem(j, 6, newitem3);
                    }
                }
            }
            ui->openGLWidget->graph.tableIndexesToUpdate.clear();
            ui->openGLWidget->graph.packetsToUpdateListMutex.unlock();
        }
    }
}

void MainWindow::onBtnClicked()
{
    if (!ui->openGLWidget->screenLocked)
    {
        if( QPushButton* btn = qobject_cast< QPushButton* >( sender() ) )
        {
            int index = btn->toolTip().toInt();
            //QMessageBox::information( this, "The button was clicked", QString::number(index));
            ui->openGLWidget->graph.packets.erase(ui->openGLWidget->graph.packets.begin()+index);
            createUI();
            repaintOGLWidget();
        }
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
        if (!b )                                  //check data
        {
            if (s.toStdString() != "" && s.toStdString() != " " ) {
                data = -1;
                QMessageBox::information(this, "The button was pressed", edit->toPlainText());
                //QColor c ()
                edit->setTextColor(QColor(255, 0, 0));
                edit->setText("-1");
            }
            else{}
        }
        else
        {
            if (edit->textColor() != QColor(0,0,0) && data != -1 ){
                edit->setTextColor(QColor(0,0,0));
                edit->setText(QString::number(data));
            }
        }
        ui->openGLWidget->graph.packets[index].from = data;
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
            if (s.toStdString() != "" && s.toStdString() != " " ) {
                data = -1;
                QMessageBox::information(this, "The button was pressed", edit->toPlainText());
                //QColor c ()
                edit->setTextColor(QColor(255, 0, 0));
                edit->setText("-1");
            }else {}
        }
        else
        {
            if (edit->textColor() != QColor(0,0,0) && data != -1 ){
                edit->setTextColor(QColor(0,0,0));
                edit->setText(QString::number(data));
            }
        }
        ui->openGLWidget->graph.packets[index].to = data;
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
    ui->openGLWidget->graph.save(q);
}

void MainWindow::on_loadButton_released()
{
    QString q = QFileDialog::getOpenFileName(this,"Load Graph From File","", "Graph (*.grf);;All Files (*)");
    if (q!= ""){
        ui->openGLWidget->graph.load(q);
        ui->openGLWidget->update();
        createUI();
    }
}

void MainWindow::setEllipseInfo(QString q)
{
    sim::sout<<"info label"<<sim::endl;
    ui->infoLabel->setText(q);
}

void MainWindow::on_deleteButton_released()
{
    ui->openGLWidget->graph.deleteActiveEllips();
    ui->openGLWidget->update();
}

void MainWindow::on_startButton_released()   // lock the screen and start simulation
{
    //sim::sout<<"START PRESS"<<sim::endl;
    if (!simulationIsActive){
        simulationIsActive = true;
        ui->openGLWidget->graph.cpuCorrect = true;
        ui->openGLWidget->graph.cpuTermCriticalFrames = 0;
        ui->openGLWidget->graph.cpuLoadCriticalFrames = 0;
        ui->openGLWidget->graph.cpuFrames = 0;
        blockInterface();
        savedGraph = Graph(ui->openGLWidget->graph);
        on_algorithmBox_currentIndexChanged(ui->algorithmBox->currentIndex());
        simulation = new Simulation(&ui->openGLWidget->graph);
        simulation->Start();
        connectSlots();
        averageTimeShoved = false;
    }
    else {
        ui->startButton->setText("Please wait");
        blockInterface();
        disconnectSlots();
        std::thread thr1 = std::thread([this](){
            simulationIsActive = false;
            simulation->stop();
            delete simulation;
            //usleep(100000);
            //ui->openGLWidget->graph.~Graph();
            //new(&ui->openGLWidget->graph) Graph(savedGraph);
            ui->openGLWidget->graph = Graph(savedGraph);
            sim::sout<<ui->openGLWidget->graph.edges.size()<<sim::endl;
            emit simulation_finish_done();
        });
        thr1.detach();
        //simulation = Simulation(&ui->openGLWidget->graph);
        //simulation.~Simulation();
    }
}

void MainWindow::repaint_on_simulation_finish_done()
{
    updateStartButtonText();
    updateTable();
    repaintOGLWidget();
    unBlockInterface();
}

void MainWindow::disconnectSlots()
{
    //disconnect(0,0,&ui->openGLWidget->graph,0);
    Graph * g= &ui->openGLWidget->graph;
    g->disconnect();
    disconnect(&ui->openGLWidget->graph,SIGNAL(repaint()),this,SLOT(repaintOGLWidget()));
    disconnect(&ui->openGLWidget->graph,SIGNAL(updateTable()),this,SLOT(updateTable()));
    for (int i=0;i<simulation->debugServer->connections.size();i++ )
    {
        //connect(simulation.debugServer->connections[i],SIGNAL(transmit_to_gui(SystemMessage)),&ui->openGLWidget->graph,SLOT(get_system_message(SystemMessage)));
        disconnect(simulation->debugServer->connections[i],SIGNAL(transmit_to_gui(SystemMessage)),simulation->debugServer,SLOT(get_message_for_debug(SystemMessage)));
        disconnect(simulation->debugServer->connections[i],SIGNAL(transmit_to_gui(DebugMessage)),&ui->openGLWidget->graph,SLOT(get_system_message(DebugMessage)));

    }
}

void MainWindow::connectSlots()
{
    connect(&ui->openGLWidget->graph,SIGNAL(repaint()),this,SLOT(repaintOGLWidget()));
    connect(&ui->openGLWidget->graph,SIGNAL(updateTable()),this,SLOT(updateTable()));
    for (int i=0;i<simulation->debugServer->connections.size();i++ )
    {
        //connect(simulation.debugServer->connections[i],SIGNAL(transmit_to_gui(SystemMessage)),&ui->openGLWidget->graph,SLOT(get_system_message(SystemMessage)));
        connect(simulation->debugServer->connections[i],SIGNAL(transmit_to_gui(SystemMessage)),simulation->debugServer,SLOT(get_message_for_debug(SystemMessage)));
        connect(simulation->debugServer->connections[i],SIGNAL(transmit_to_gui(DebugMessage)),&ui->openGLWidget->graph,SLOT(get_system_message(DebugMessage)));

    }
    ui->openGLWidget->graph.graphId++;
    //connect(simulation.debugServer->debugConnection,SIGNAL(transmit_to_gui(SystemMessage)),&ui->openGLWidget->graph,SLOT(get_system_message(SystemMessage)));

    //connect(simulation.debugServer->debugConnection,SIGNAL(transmit_to_gui(SystemMessage)),this,SLOT(get_system_message(SystemMessage)));
}

void MainWindow::blockInterface()
{
    ui->openGLWidget->screenLocked = true;
    ui->algorithmBox->setEnabled(false);
    ui->loadButton->setEnabled(false);
    ui->saveButton->setEnabled(false);
    ui->AddButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
    ui->startButton->setEnabled(false);
    ui->CountOfBytes->setEnabled(false);
    ui->sendIntervalMS->setEnabled(false);
    ui->lambdaText->setEnabled(false);
    ui->zText->setEnabled(false);
    ui->uText->setEnabled(false);
    ui->wText->setEnabled(false);
}

void MainWindow::unBlockInterface()
{
    ui->openGLWidget->screenLocked = false;
    ui->algorithmBox->setEnabled(true);
    ui->loadButton->setEnabled(true);
    ui->saveButton->setEnabled(true);
    ui->AddButton->setEnabled(true);
    ui->deleteButton->setEnabled(true);
    ui->startButton->setEnabled(true);
    ui->CountOfBytes->setEnabled(true);
    ui->sendIntervalMS->setEnabled(true);
    ui->lambdaText->setEnabled(true);
    ui->zText->setEnabled(true);
    ui->uText->setEnabled(true);
    ui->wText->setEnabled(true);
    updateStartButtonText();
}

void MainWindow::updateStartButtonText()
{
    if (simulationIsActive)
    {
        ui->startButton->setText("Finish simulation");
    }
    else{
        ui->startButton->setText("Start");
    }
}

void MainWindow::updateTable()
{
    /*std::chrono::milliseconds timeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    if ((timeNow - lastTableUpdate).count()>42)
    {
        checkSimulationStatus();
        createUI();
        lastTableUpdate = timeNow;
    }
    //sim::sout<<"updating table"<<sim::endl;*/
}

void MainWindow::checkSimulationStatus()
{

    bool allPacketsDelivered = true;
    float time = 0;
    float maxTime = 0;
    float maxLoad = 0;
    for (int i=0;i< ui->openGLWidget->graph.packets.size();i++)
    {
        //sim::sout<<"Packet "<<i<<" delivering  status = "<<ui->openGLWidget->graph.packets[i].delivered<<sim::endl;
        //allPacketsDelivered *= ui->openGLWidget->graph.packets[i].delivered;
        allPacketsDelivered = allPacketsDelivered && ui->openGLWidget->graph.packets[i].delivered;
        time += ui->openGLWidget->graph.packets[i].timeOnCreation.count();
        maxTime = ui->openGLWidget->graph.packets[i].timeOnCreation.count() > maxTime ? ui->openGLWidget->graph.packets[i].timeOnCreation.count() : maxTime;
    }
    for (int j=0;j<ui->openGLWidget->graph.edges.size();j++)
    {
        maxLoad = ui->openGLWidget->graph.edges[j].maxLoadToFrom > maxLoad ? ui->openGLWidget->graph.edges[j].maxLoadToFrom : maxLoad;
        maxLoad = ui->openGLWidget->graph.edges[j].maxLoadFromTo > maxLoad ? ui->openGLWidget->graph.edges[j].maxLoadFromTo : maxLoad;
    }
    if (allPacketsDelivered) {
        if (!averageTimeShoved) {
            time /= ui->openGLWidget->graph.packets.size();
            ui->openGLWidget->graph.averageTime = time;
            ui->openGLWidget->graph.maxTime = maxTime;
            ui->openGLWidget->graph.maxLoad = maxLoad;
            QMessageBox msgBox;
            msgBox.setText("All packets delivered. \nAverage delivery time: " + QString::number(time) +"\nMax delivery time: " + QString::number(maxTime) +"\nMax node Load: " +  QString::number(maxLoad)+"%");
            msgBox.exec();
            averageTimeShoved = true;
            unBlockInterface();
            if (ui->saveReportCheckBox->isChecked()) {
                SimulationReport::generateReport(&ui->openGLWidget->graph,
                                                 ui->algorithmBox->currentText().toStdString());
            }
            ui->openGLWidget->graph.cpuCorrect = true;
        }
    }
}



void MainWindow::repaintOGLWidget()
{
    /*std::chrono::milliseconds timeNow = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    if ((timeNow - lastOGLWUpdate).count()>42)
    {
        ui->openGLWidget->update();
        lastOGLWUpdate = timeNow;
    }*/
}

void MainWindow::AddButtonClick()
{
    ui->openGLWidget->graph.addPacket();
    createUI();
}

void MainWindow::on_algorithmBox_currentIndexChanged(int index)
{
    ui->openGLWidget->graph.selectedAlgorithm = static_cast<Algorithms>(index);
    std::string  res = ui->openGLWidget->graph.selectedAlgorithm == Algorithms::MY_LOCAL_VOTING ? "MY_LOCAL_VOTING" : "NOT MY_LOCAL_VOTING";
    sim::sout<<ui->openGLWidget->graph.selectedAlgorithm<<"   "<<res<<sim::endl;
}

void MainWindow::updateEdgePerfomanceLabels()
{
    if (ui->openGLWidget->graph.activeEdgeData == nullptr) {
        ui->CountOfBytes->setText(QString::number(Settings::getSendBytesPerInterval()));
        ui->sendIntervalMS->setText(QString::number(Settings::getsendIntervalMS()));
        ui->linkBreakChance->setText(QString::number(Settings::getConnectionBreakChance()));
    } else {
        ui->CountOfBytes->setText(QString::number(ui->openGLWidget->graph.activeEdgeData->SendBytesPerInterval == -1 ?
                                                  Settings::getSendBytesPerInterval() :ui->openGLWidget->graph.activeEdgeData->SendBytesPerInterval));
        ui->sendIntervalMS->setText(QString::number(ui->openGLWidget->graph.activeEdgeData->sendIntervalMS == -1 ?
                                                    Settings::getsendIntervalMS():ui->openGLWidget->graph.activeEdgeData->sendIntervalMS));
        ui->linkBreakChance->setText(QString::number(ui->openGLWidget->graph.activeEdgeData->connectionBreakChance == -1 ?
                                                    Settings::getConnectionBreakChance():ui->openGLWidget->graph.activeEdgeData->connectionBreakChance));
    }
}

void MainWindow::onnn_settingsButton_released()
{
    /*settingsForm = new SettingsForm;
    settingsForm->setWindowTitle("Simulation settings");
    QIcon icon;
    icon.addFile(QStringLiteral("../icon1.ico"), QSize(), QIcon::Normal, QIcon::Off);
    settingsForm->setWindowIcon(icon);
    settingsForm->setWindowIconText("Simulation");
    settingsForm->setWindowFlag(Qt::WindowStaysOnTopHint);
    settingsForm->show();
    sim::sout<<"SETTINGS"<<sim::endl;*/
}

void MainWindow::onnn_break_chance_editingFinished()
{
    bool* b = new bool;
    *b = false;
    QString text = ui->linkBreakChance->text();
    int res = text.toInt(b);
    if (*b && res < 1000000 && res > 200)
    {
        ui->linkBreakChance->setStyleSheet("QLineEdit { background: rgb(255, 255, 255); selection-background-color: rgb(233, 99, 0); }");
        if (ui->openGLWidget->graph.activeEdgeData == nullptr) {
            Settings::setConnectionBreakChance(res);
        } else {
            ui->openGLWidget->graph.activeEdgeData->connectionBreakChance = res == Settings::getConnectionBreakChance() ? -1 : res;
        }
    }
    else {
        ui->linkBreakChance->setStyleSheet("QLineEdit { background: rgb(255, 65, 65); selection-background-color: rgb(233, 99, 0); }");
    }
}

void MainWindow::onnn_count_of_bytes_editingFinished()
{
    bool* b = new bool;
    *b = false;
    QString text = ui->CountOfBytes->text();
    int res = text.toInt(b);
    if (*b && res < 1024)
    {
        ui->CountOfBytes->setStyleSheet("QLineEdit { background: rgb(255, 255, 255); selection-background-color: rgb(233, 99, 0); }");
        if (ui->openGLWidget->graph.activeEdgeData == nullptr) {
            Settings::setSendBytesPerInterval(res);
        } else {
            ui->openGLWidget->graph.activeEdgeData->SendBytesPerInterval = res;
        }
    }
    else {
        ui->CountOfBytes->setStyleSheet("QLineEdit { background: rgb(255, 65, 65); selection-background-color: rgb(233, 99, 0); }");
    }
}

void MainWindow::onnn_send_interval_editingFinished()
{
    //sim::sout<<"changed"<<sim::endl;
    bool* b = new bool;
    *b = false;
    QString text = ui->sendIntervalMS->text();
    int res = text.toInt(b);
    if (*b && res < 100000)
    {
        ui->sendIntervalMS->setStyleSheet("QLineEdit { background: rgb(255, 255, 255); selection-background-color: rgb(233, 99, 0); }");
        if (ui->openGLWidget->graph.activeEdgeData == nullptr) {
            Settings::setsendIntervalMS(res);
        } else {
            ui->openGLWidget->graph.activeEdgeData->sendIntervalMS = res;
        }
    }
    else {
        ui->sendIntervalMS->setStyleSheet("QLineEdit { background: rgb(255, 65, 65); selection-background-color: rgb(233, 99, 0); }");
    }
}

void MainWindow::onnn_lambda_editingFinished()
{
    //sim::sout<<"changed"<<sim::endl;
    bool* b = new bool;
    *b = false;
    QString text = ui->sendIntervalMS->text();
    int res = text.toInt(b);
    if (*b && res < 100000)
    {
        ui->sendIntervalMS->setStyleSheet("QLineEdit { background: rgb(255, 255, 255); selection-background-color: rgb(233, 99, 0); }");
        Settings::setsendIntervalMS(res);
    }
    else {
        ui->sendIntervalMS->setStyleSheet("QLineEdit { background: rgb(255, 65, 65); selection-background-color: rgb(233, 99, 0); }");
        //Settings::
    }
}

void MainWindow::onnn_z_text_editingFinished()
{
    //sim::sout<<"changed"<<sim::endl;
    bool* b = new bool;
    *b = false;
    QString text = ui->zText->text();
    int res = text.toInt(b);
    if (*b && res < 100000)
    {
        ui->zText->setStyleSheet("QLineEdit { background: rgb(255, 255, 255); selection-background-color: rgb(233, 99, 0); }");
        Settings::setZCoef(res);
    }
    else {
        ui->zText->setStyleSheet("QLineEdit { background: rgb(255, 65, 65); selection-background-color: rgb(233, 99, 0); }");
    }
    //sim::sout<<Settings::getZCoef()<<" "<<Settings::getUCoef()<<" "<<Settings::getWCoef()<<sim::endl;
}

void MainWindow::onnn_w_text_editingFinished()
{
    //sim::sout<<"changed"<<sim::endl;
    bool* b = new bool;
    *b = false;
    QString text = ui->wText->text();
    int res = text.toInt(b);
    if (*b && res < 100000)
    {
        ui->wText->setStyleSheet("QLineEdit { background: rgb(255, 255, 255); selection-background-color: rgb(233, 99, 0); }");
        Settings::setWCoef(res);
    }
    else {
        ui->wText->setStyleSheet("QLineEdit { background: rgb(255, 65, 65); selection-background-color: rgb(233, 99, 0); }");
    }
    //sim::sout<<Settings::getZCoef()<<" "<<Settings::getUCoef()<<" "<<Settings::getWCoef()<<sim::endl;
}

void MainWindow::onnn_u_text_editingFinished()
{
    //sim::sout<<"changed"<<sim::endl;
    bool* b = new bool;
    *b = false;
    QString text = ui->uText->text();
    int res = text.toInt(b);
    if (*b && res < 100000)
    {
        ui->uText->setStyleSheet("QLineEdit { background: rgb(255, 255, 255); selection-background-color: rgb(233, 99, 0); }");
        Settings::setUCoef(res);
    }
    else {
        ui->uText->setStyleSheet("QLineEdit { background: rgb(255, 65, 65); selection-background-color: rgb(233, 99, 0); }");
    }
    //sim::sout<<Settings::getZCoef()<<" "<<Settings::getUCoef()<<" "<<Settings::getWCoef()<<sim::endl;
}