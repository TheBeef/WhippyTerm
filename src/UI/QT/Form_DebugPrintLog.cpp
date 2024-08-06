#include "Form_DebugPrintLog.h"
#include "ui_Form_DebugPrintLog.h"

Form_DebugPrintLog::Form_DebugPrintLog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Form_DebugPrintLog)
{
    ui->setupUi(this);
}

Form_DebugPrintLog::~Form_DebugPrintLog()
{
    delete ui;
}

void Form_DebugPrintLog::AddText(const char *text)
{
    ui->plainTextEdit->moveCursor(QTextCursor::End);
    ui->plainTextEdit->insertPlainText(text);
    ui->plainTextEdit->moveCursor(QTextCursor::End);
}

void Form_DebugPrintLog::on_clear_pushButton_clicked()
{
    ui->plainTextEdit->clear();
}

void Form_DebugPrintLog::ClearText(void)
{
    ui->plainTextEdit->clear();
}

void Form_DebugPrintLog::on_buttonBox_accepted()
{
    close();
}

