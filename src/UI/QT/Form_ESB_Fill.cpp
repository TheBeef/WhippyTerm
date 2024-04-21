#include "Form_ESB_Fill.h"
#include "ui_Form_ESB_Fill.h"

Form_ESB_Fill::Form_ESB_Fill(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_ESB_Fill)
{
    ui->setupUi(this);
}

Form_ESB_Fill::~Form_ESB_Fill()
{
    delete ui;
}
