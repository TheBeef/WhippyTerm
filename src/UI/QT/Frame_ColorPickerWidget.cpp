#include "Frame_ColorPickerWidget.h"
#include "ui_Frame_ColorPickerWidget.h"
#include <QColorDialog>

Frame_ColorPickerWidget::Frame_ColorPickerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Frame_ColorPickerWidget)
{
    ui->setupUi(this);

    EventCB=NULL;
    UserData=NULL;
    RGB=0;
}

Frame_ColorPickerWidget::~Frame_ColorPickerWidget()
{
    delete ui;
}

void Frame_ColorPickerWidget::Init(void (*InitEventCB)(const struct PIColorPickEvent *Event,void *UserData),
        void *InitUserData,uint32_t InitRGB)
{
    EventCB=InitEventCB;
    UserData=InitUserData;
    RGB=InitRGB;
    UpdatePreviewColor();
}

void Frame_ColorPickerWidget::SetRGBValue(uint32_t NewValue)
{
    RGB=NewValue;
    UpdatePreviewColor();
}

uint32_t Frame_ColorPickerWidget::GetRGBValue(void)
{
    return RGB;
}

void Frame_ColorPickerWidget::on_Select_pushButton_clicked()
{
    struct PIColorPickEvent Event;
    QColor SelColor;

    SelColor=QColorDialog::getColor(QColor(RGB),this);
    if(SelColor.isValid())
    {
        RGB=SelColor.rgba()&0xFFFFFF;

        UpdatePreviewColor();

        if(EventCB==NULL)
            return;

        Event.EventType=e_PIEColorPick_Press;
        EventCB(&Event,UserData);
    }
}

void Frame_ColorPickerWidget::UpdatePreviewColor(void)
{
    QPalette pal;
    QColor col;

    col.setRgb(RGB);
    pal=QApplication::palette();
    pal.setColor(QPalette::Window,col);

    ui->ColorPreview_frame->setPalette(pal);
}
