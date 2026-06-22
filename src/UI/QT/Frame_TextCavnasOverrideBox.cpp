#include "Frame_TextCavnasOverrideBox.h"
#include "ui_Frame_TextCavnasOverrideBox.h"

Frame_TextCavnasOverrideBox::Frame_TextCavnasOverrideBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Frame_TextCavnasOverrideBox)
{
    QColor bg;
    QString Style;

    ui->setupUi(this);

    /* We want rounded corners, so we have to do that with styles */
    bg=palette().color(QPalette::Window);
    Style="border-radius: 12px; background: ";
    Style+=bg.name();

    setStyleSheet(Style);
}

Frame_TextCavnasOverrideBox::~Frame_TextCavnasOverrideBox()
{
    delete ui;
}
