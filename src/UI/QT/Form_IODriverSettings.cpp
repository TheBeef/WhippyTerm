#include "Form_IODriverSettings.h"
#include "ui_Form_IODriverSettings.h"

Form_IODriverSettings::Form_IODriverSettings(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Form_IODriverSettings)
{
    ui->setupUi(this);
}

Form_IODriverSettings::~Form_IODriverSettings()
{
    delete ui;
}
