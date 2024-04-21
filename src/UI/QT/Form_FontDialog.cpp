#include "Form_FontDialog.h"
#include "ui_Form_FontDialog.h"
#include <stdio.h>
#include "UI/UIFontReq.h"

Form_FontDialog::Form_FontDialog(QWidget *parent) :
    QDialog(parent,Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
    ui(new Ui::Form_FontDialog)
{
    bool First;

    ui->setupUi(this);

    First=true;

    foreach(const QString &family,FontDatabase.families())
    {
        foreach(const QString &style,FontDatabase.styles(family))
        {
#warning DEBUG PAUL: Need this for fixed width only to work, Put this back in
/* DEBUG PAUL: Need this for fixed width only to work */
// DEBUG PAUL: Put this back in
//`            if(FontDatabase.isFixedPitch(family,style))
            {
                if(First)
                {
                    First=false;
                    DefaultFontName=family;
                }
                new QListWidgetItem(family, ui->FontlistWidget);
                break;  // Only need to add it once
            }
        }
    }
    ui->StyleGroupbox->setVisible(false);
}

Form_FontDialog::~Form_FontDialog()
{
    delete ui;
}

void Form_FontDialog::on_FontlistWidget_itemSelectionChanged()
{
    if(ui->FontlistWidget->currentRow()>=0)
        ui->FontlineEdit->setText(ui->FontlistWidget->currentItem()->text());
}

void Form_FontDialog::on_StylelistWidget_itemSelectionChanged()
{
    int LastPoint;
    QListWidgetItem *NewItem;
    bool Found;

    ui->StylelineEdit->setText(ui->StylelistWidget->currentItem()->text());

    LastPoint=ui->SizelineEdit->text().toInt();
    if(LastPoint!=0)
        LastPointSizeSelected=LastPoint;

    Found=false;
    ui->SizelistWidget->clear();
    foreach (int points, FontDatabase.smoothSizes(ui->FontlineEdit->text(), ui->StylelistWidget->currentItem()->text()))
    {
        if(LastPointSizeSelected==points)
        {
            NewItem=new QListWidgetItem(QString::number(points),ui->SizelistWidget);
            ui->SizelistWidget->setCurrentItem(NewItem);
            Found=true;
        }
    }
    if(!Found)
    {
        if(ui->SizelistWidget->count()>0)
            ui->SizelistWidget->setCurrentRow(0);
        else
            ui->SizelineEdit->clear();
    }
    UpdatePreview();
}

void Form_FontDialog::on_FontlineEdit_textChanged(const QString &arg1)
{
    QList<QListWidgetItem *> FoundItems;
    bool IncludeAllStyles;

    (void)arg1; // Shut up the compiler

    FoundItems=ui->FontlistWidget->findItems(ui->FontlineEdit->text(),Qt::MatchExactly);
    if(!FoundItems.empty())
    {
        IncludeAllStyles=false; // Fixed pitch only
        if(ui->FontlistWidget->currentItem()==NULL ||
                ui->FontlistWidget->currentItem()->text()!=
                ui->FontlineEdit->text())
        {
            ui->FontlistWidget->setCurrentItem(FoundItems.first());
        }
    }
    else
    {
        /* The item is not in the list of fonts, user has typed their own font name */

        /* Clear the font list selection */
        ui->FontlistWidget->setCurrentRow(-1);
        IncludeAllStyles=true;
    }

    ui->StylelistWidget->clear();
    foreach(const QString &style,FontDatabase.styles(ui->FontlineEdit->text()))
    {
        if(IncludeAllStyles || FontDatabase.isFixedPitch(ui->FontlineEdit->text(),style))
        {
            new QListWidgetItem(style,ui->StylelistWidget);
        }
    }
    ui->StylelistWidget->setCurrentItem(ui->StylelistWidget->item(0));
    UpdatePreview();
}

void Form_FontDialog::on_SizelistWidget_itemSelectionChanged()
{
    if(ui->SizelistWidget->currentItem()!=NULL)
        ui->SizelineEdit->setText(ui->SizelistWidget->currentItem()->text());
}

void Form_FontDialog::on_SizelineEdit_textChanged(const QString &arg1)
{
    int LastPoint;
    QList<QListWidgetItem *> FoundItems;

    (void)arg1;

    LastPoint=ui->SizelineEdit->text().toInt();
    if(LastPoint!=0)
        LastPointSizeSelected=LastPoint;

    FoundItems=ui->SizelistWidget->findItems(QString::number(LastPoint),Qt::MatchExactly);
    if(!FoundItems.empty())
    {
        if(ui->SizelistWidget->currentItem()==NULL || ui->SizelistWidget->currentItem()->text()!=QString::number(LastPoint))
            ui->SizelistWidget->setCurrentItem(FoundItems.first());
    }
    else
    {
        /* Empty list, user must have typed a number, clear the selection list */
        ui->SizelistWidget->setCurrentRow(-1);
    }
    UpdatePreview();
}

void Form_FontDialog::UpdatePreview(void)
{
    QFont NewFont;
    
    if(ui->SizelineEdit->text().toInt()==0)
    {
        ui->PreviewTextEdit->clear();
        return;
    }

    NewFont.setFixedPitch(true);
    NewFont.setFamily(ui->FontlineEdit->text());
    NewFont.setPointSize(ui->SizelineEdit->text().toInt());
//    NewFont.setStyleName(ui->StylelineEdit->text()); // Doesn't work
    NewFont.setBold(ui->StyleBoldCheckBox->isChecked());
    NewFont.setItalic(ui->StyleItalicCheckBox->isChecked());
    ui->PreviewTextEdit->setCurrentFont(NewFont);

    ui->PreviewTextEdit->setPlainText(
                "Test text here\n"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\n"
                "The quick brown fox jumps over the lazy dog\n"
                "  123\n"
                "+9876\n"
                " ----\n"
                " 9999\n");
}

void Form_FontDialog::on_StyleItalicCheckBox_clicked()
{
    UpdatePreview();
}

void Form_FontDialog::on_StyleBoldCheckBox_clicked()
{
    UpdatePreview();
}

void Form_FontDialog::on_buttonBox_accepted()
{
    accept();
}

void Form_FontDialog::on_buttonBox_rejected()
{
    reject();
}

void Form_FontDialog::SetOptions(const char *Title,const std::string &FontName,
        int FontSize,long FontStyle,long Flags)
{
    char buff[100];
    QString UseFontName;

    setWindowTitle(Title);

    ui->StyleBoldCheckBox->setChecked(FontStyle&UIFONT_STYLE_BOLD);
    ui->StyleItalicCheckBox->setChecked(FontStyle&UIFONT_STYLE_ITALIC);

    if(FontName=="")
        UseFontName=DefaultFontName.toUtf8().data();
    else
        UseFontName=FontName.c_str();
    ui->FontlineEdit->setText(UseFontName);

    sprintf(buff,"%d",FontSize);
    ui->SizelineEdit->setText(buff);
}

void Form_FontDialog::GetFontInfo(std::string &FontName,
        int &FontSize,long &FontStyle)
{
    FontName=ui->FontlineEdit->text().toUtf8().data();
    FontSize=ui->SizelineEdit->text().toInt();
    FontStyle=0;
    if(ui->StyleBoldCheckBox->isChecked())
        FontStyle|=UIFONT_STYLE_BOLD;
    if(ui->StyleItalicCheckBox->isChecked())
        FontStyle|=UIFONT_STYLE_ITALIC;
}

