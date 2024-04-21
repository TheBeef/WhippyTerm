#include "Form_ChangeConnectionName.h"
#include "ui_Form_ChangeConnectionName.h"

Form_ChangeConnectionName::Form_ChangeConnectionName(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_ChangeConnectionName)
{
    ui->setupUi(this);
}

Form_ChangeConnectionName::~Form_ChangeConnectionName()
{
    delete ui;
}
