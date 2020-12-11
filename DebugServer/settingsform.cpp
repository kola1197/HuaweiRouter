#include <iostream>
#include "settingsform.h"
#include "ui_settingsform.h"

SettingsForm::SettingsForm(QWidget *parent) :
        QWidget(parent)
        , ui(new Ui::SettingsForm)
{
    ui->setupUi(this);
    //std::cout<<"SettingsForm"<<std::endl;

}

SettingsForm::~SettingsForm()
{
    delete ui;
}
