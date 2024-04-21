#include "Form_ESB_ViewSource.h"
#include "ui_Form_ESB_ViewSource.h"

Form_ESB_ViewSource::Form_ESB_ViewSource(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_ESB_ViewSource)
{
    ui->setupUi(this);
}

Form_ESB_ViewSource::~Form_ESB_ViewSource()
{
    delete ui;
}
