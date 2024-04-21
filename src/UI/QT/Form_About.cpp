#include "Form_About.h"
#include "ui_Form_About.h"

Form_About::Form_About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_About)
{
    ui->setupUi(this);
}

Form_About::~Form_About()
{
    delete ui;
}

