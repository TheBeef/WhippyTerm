#include "Form_ConnectionOptions.h"
#include "ui_Form_ConnectionOptions.h"

Form_ConnectionOptions::Form_ConnectionOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_ConnectionOptions)
{
    ui->setupUi(this);
}

Form_ConnectionOptions::~Form_ConnectionOptions()
{
    delete ui;
}
