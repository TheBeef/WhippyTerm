#include "Form_HexDisplayCopyAs.h"
#include "ui_Form_HexDisplayCopyAs.h"

Form_HexDisplayCopyAs::Form_HexDisplayCopyAs(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_HexDisplayCopyAs)
{
    ui->setupUi(this);
}

Form_HexDisplayCopyAs::~Form_HexDisplayCopyAs()
{
    delete ui;
}
