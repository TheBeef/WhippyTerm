#include "Form_TransmitDelay.h"
#include "ui_Form_TransmitDelay.h"

Form_TransmitDelay::Form_TransmitDelay(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_TransmitDelay)
{
    ui->setupUi(this);
}

Form_TransmitDelay::~Form_TransmitDelay()
{
    delete ui;
}
