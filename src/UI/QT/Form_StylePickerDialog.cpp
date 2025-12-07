#include "Form_StylePickerDialog.h"
#include "ui_Form_StylePickerDialog.h"
#include "PluginSDK/DataProcessors.h"
#include <QColorDialog>

Form_StylePickerDialog::Form_StylePickerDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Form_StylePickerDialog)
{
    ui->setupUi(this);
}

Form_StylePickerDialog::~Form_StylePickerDialog()
{
    delete ui;
}

void Form_StylePickerDialog::on_FG_Select_pushButton_clicked()
{
    QColor SelColor;

    SelColor=QColorDialog::getColor(QColor(CurStyle.FGColor),this);
    if(SelColor.isValid())
    {
        CurStyle.FGColor=SelColor.rgba()&0xFFFFFF;

        UpdateDisplay();
    }
}

void Form_StylePickerDialog::on_BG_Select_pushButton_clicked()
{
    QColor SelColor;

    SelColor=QColorDialog::getColor(QColor(CurStyle.BGColor),this);
    if(SelColor.isValid())
    {
        CurStyle.BGColor=SelColor.rgba()&0xFFFFFF;

        UpdateDisplay();
    }
}

void Form_StylePickerDialog::on_ULine_Select_pushButton_clicked()
{
    QColor SelColor;

    SelColor=QColorDialog::getColor(QColor(CurStyle.ULineColor),this);
    if(SelColor.isValid())
    {
        CurStyle.ULineColor=SelColor.rgba()&0xFFFFFF;

        UpdateDisplay();
    }
}

void Form_StylePickerDialog::UpdateDisplay(void)
{
    QPalette pal;
    QColor col;

    col.setRgb(CurStyle.FGColor);
    pal=QApplication::palette();
    pal.setColor(QPalette::Window,col);
    ui->FG_frame->setPalette(pal);

    col.setRgb(CurStyle.BGColor);
    pal=QApplication::palette();
    pal.setColor(QPalette::Window,col);
    ui->BG_frame->setPalette(pal);

    col.setRgb(CurStyle.ULineColor);
    pal=QApplication::palette();
    pal.setColor(QPalette::Window,col);
    ui->ULine_frame->setPalette(pal);

    ui->ULine_checkBox->setChecked(CurStyle.Attribs&TXT_ATTRIB_UNDERLINE);
    ui->LineThrough_checkBox->setChecked(CurStyle.Attribs&TXT_ATTRIB_LINETHROUGH);
    ui->Bold_checkBox->setChecked(CurStyle.Attribs&TXT_ATTRIB_BOLD);
    ui->Italic_checkBox->setChecked(CurStyle.Attribs&TXT_ATTRIB_ITALIC);
}

void Form_StylePickerDialog::CollectStylesFromGUI(void)
{
    CurStyle.Attribs=0;
    if(ui->ULine_checkBox->isChecked())
        CurStyle.Attribs|=TXT_ATTRIB_UNDERLINE;
    if(ui->LineThrough_checkBox->isChecked())
        CurStyle.Attribs|=TXT_ATTRIB_LINETHROUGH;
    if(ui->Bold_checkBox->isChecked())
        CurStyle.Attribs|=TXT_ATTRIB_BOLD;
    if(ui->Italic_checkBox->isChecked())
        CurStyle.Attribs|=TXT_ATTRIB_ITALIC;
}
