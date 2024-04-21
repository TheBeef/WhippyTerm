#include "Form_ESB_InsertProp.h"
#include "ui_Form_ESB_InsertProp.h"

Form_ESB_InsertProp::Form_ESB_InsertProp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_ESB_InsertProp)
{
    ui->setupUi(this);
}

Form_ESB_InsertProp::~Form_ESB_InsertProp()
{
    delete ui;
}
