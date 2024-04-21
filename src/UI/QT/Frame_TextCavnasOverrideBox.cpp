#include "Frame_TextCavnasOverrideBox.h"
#include "ui_Frame_TextCavnasOverrideBox.h"

Frame_TextCavnasOverrideBox::Frame_TextCavnasOverrideBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Frame_TextCavnasOverrideBox)
{
    ui->setupUi(this);
}

Frame_TextCavnasOverrideBox::~Frame_TextCavnasOverrideBox()
{
    delete ui;
}
