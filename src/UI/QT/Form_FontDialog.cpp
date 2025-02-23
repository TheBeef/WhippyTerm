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

    DoingEvent=false;

    First=true;

    foreach(const QString &family,FontDatabase.families())
    {
        foreach(const QString &style,FontDatabase.styles(family))
        {
//#warning DEBUG PAUL: Need this for fixed width only to work, Put this back in
/* DEBUG PAUL: Need this for fixed width only to work */
            if(FontDatabase.isFixedPitch(family,style))
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
    if(DoingEvent)
        return;

    if(ui->FontlistWidget->currentRow()>=0)
        ui->FontlineEdit->setText(ui->FontlistWidget->currentItem()->text());
}

void Form_FontDialog::on_StylelistWidget_itemSelectionChanged()
{
    ui->StylelineEdit->setText(ui->StylelistWidget->currentItem()->text());

    FillInSizeList();
    UpdatePreview();
}

void Form_FontDialog::on_FontlineEdit_textChanged(const QString &arg1)
{
    int r;

    (void)arg1; // Shut up the compiler

    /* Find the first entry in the list of fonts that matches this input */
    for(r=0;r<ui->FontlistWidget->count();r++)
    {
        if(ui->FontlistWidget->item(r)->text().contains(ui->FontlineEdit->text(),Qt::CaseInsensitive))
            break;
    }
    if(r!=ui->FontlistWidget->count())
    {
        DoingEvent=true;
        ui->FontlistWidget->setCurrentRow(r);
        DoingEvent=false;
    }

    FillInSizeList();
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

void Form_FontDialog::FillInSizeList()
{
    int LastPoint;
    QListWidgetItem *NewItem;
    bool Found;
    QString StyleStr;
    unsigned int r;
    int DefaultFontSizes[]=
    {
        8,
        10,
        12,
        14,
        18,
        24,
        36,
        72
    };

    LastPoint=ui->SizelineEdit->text().toInt();
    if(LastPoint!=0)
        LastPointSizeSelected=LastPoint;

    if(ui->StylelistWidget->currentItem()!=NULL)
        StyleStr=ui->StylelistWidget->currentItem()->text();
    else
        StyleStr="";

    Found=false;
    ui->SizelistWidget->clear();
    foreach (int points, FontDatabase.smoothSizes(ui->FontlineEdit->text(),StyleStr))
    {
        NewItem=new QListWidgetItem(QString::number(points),ui->SizelistWidget);
        ui->SizelistWidget->addItem(NewItem);
        if(LastPointSizeSelected==points)
            ui->SizelistWidget->setCurrentItem(NewItem);
        Found=true;
    }
    if(!Found)
    {
        /* Fill in a default set */
        for(r=0;r<sizeof(DefaultFontSizes)/sizeof(int);r++)
        {
            NewItem=new QListWidgetItem(QString::number(DefaultFontSizes[r]),
                    ui->SizelistWidget);
            ui->SizelistWidget->addItem(NewItem);
            if(LastPointSizeSelected==DefaultFontSizes[r])
                ui->SizelistWidget->setCurrentItem(NewItem);
        }
    }
}
