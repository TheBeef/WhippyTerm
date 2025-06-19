#include "Form_DataProPluginSettings.h"
#include "ui_Form_DataProPluginSettings.h"

Form_DataProPluginSettings::Form_DataProPluginSettings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Form_DataProPluginSettings)
{
    ui->setupUi(this);
}

Form_DataProPluginSettings::~Form_DataProPluginSettings()
{
    delete ui;
}
