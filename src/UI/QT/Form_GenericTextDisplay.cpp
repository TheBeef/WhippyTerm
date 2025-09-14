#include "Form_GenericTextDisplay.h"
#include "ui_Form_GenericTextDisplay.h"

Form_GenericTextDisplay::Form_GenericTextDisplay(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Form_GenericTextDisplay)
{
    ui->setupUi(this);
}

Form_GenericTextDisplay::~Form_GenericTextDisplay()
{
    delete ui;
}
