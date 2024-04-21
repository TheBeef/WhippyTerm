#include "Form_PasteData.h"
#include "ui_Form_PasteData.h"

Form_PasteData::Form_PasteData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_PasteData)
{
    ui->setupUi(this);
    SelectedDataType=e_PasteData_Cancel;
}

Form_PasteData::~Form_PasteData()
{
    delete ui;
}

void Form_PasteData::on_pushButton_HexDump_clicked()
{
    SelectedDataType=e_PasteData_HexDump;
    accept();
}

void Form_PasteData::on_pushButton_Text_clicked()
{
    SelectedDataType=e_PasteData_Text;
    accept();
}
