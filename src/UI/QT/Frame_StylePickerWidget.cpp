#include "Frame_StylePickerWidget.h"
#include "ui_Frame_StylePickerWidget.h"
#include "App/Dialogs/Dialog_StylePicker.h"
#include "PluginSDK/DataProcessors.h"
#include <QFrame>
#include <QColor>

Frame_StylePickerWidget::Frame_StylePickerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Frame_StylePickerWidget)
{
    ui->setupUi(this);

    EventCB=NULL;
    UserData=NULL;
    CurStyle.FGColor=0xFFFFFF;
    CurStyle.BGColor=0x000000;
    CurStyle.Attribs=0;
}

Frame_StylePickerWidget::~Frame_StylePickerWidget()
{
    delete ui;
}

void Frame_StylePickerWidget::Init(void (*InitEventCB)(const struct PIStylePickEvent *Event,void *UserData),
        void *InitUserData,struct StyleData *SD)
{
    EventCB=InitEventCB;
    UserData=InitUserData;
    CurStyle=*SD;
    UpdatePreviewStyle();
}

void Frame_StylePickerWidget::SetStyleValue(struct StyleData *SD)
{
    CurStyle=*SD;
    UpdatePreviewStyle();
}

void Frame_StylePickerWidget::GetStyleValue(struct StyleData *SD)
{
    *SD=CurStyle;
}

void Frame_StylePickerWidget::on_pushButton_clicked()
{
    struct PIStylePickEvent Event;

    if(RunStylePickerDialog(&CurStyle))
    {
        UpdatePreviewStyle();

        if(EventCB!=NULL)
        {
            Event.EventType=e_PIEStylePick_NewStyle;
            EventCB(&Event,UserData);
        }
    }
}

void Frame_StylePickerWidget::UpdatePreviewStyle(void)
{
    QPalette pal;
    QColor col;
    QColor col2;
    QFont font;
    char text[100];

    col.setRgb(CurStyle.FGColor);
    col2.setRgb(CurStyle.BGColor);
    pal=QApplication::palette();
    pal.setColor(QPalette::WindowText,col);
    pal.setColor(QPalette::Window,col2);
    ui->StylePreview_label->setPalette(pal);

    if(CurStyle.Attribs&TXT_ATTRIB_BOLD)
        font.setBold(true);
    if(CurStyle.Attribs&TXT_ATTRIB_UNDERLINE)
        font.setUnderline(true);
    if(CurStyle.Attribs&TXT_ATTRIB_ITALIC)
        font.setItalic(true);
    if(CurStyle.Attribs&TXT_ATTRIB_LINETHROUGH)
        font.setStrikeOut(true);
    ui->StylePreview_label->setFont(font);

    /* This has a problem where the line throught also gets colored.  Oh well,
       the only way to really fix this is to change it over to a text cavas.
       But it's a lot of work.  On the plus side it would allow for all the
       different styles.  Future me problem. */

    if(CurStyle.Attribs&TXT_ATTRIB_UNDERLINE)
        sprintf(text,"<span style='text-decoration-color: #%06X;'>Example</span>",CurStyle.ULineColor);
    else
        sprintf(text,"Example");
    ui->StylePreview_label->setText(text);
}
