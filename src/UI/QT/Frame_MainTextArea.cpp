#include "Frame_MainTextArea.h"
#include "ui_Frame_MainTextArea.h"
#include "UI/UITextDisplay.h"
#include <string>

using namespace std;

Frame_MainTextArea::Frame_MainTextArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Frame_MainTextArea)
{
    EventHandler=nullptr;
//    DisplayFrame=NULL;
    ui->setupUi(this);
}

Frame_MainTextArea::~Frame_MainTextArea()
{
    delete ui;
}

//void Frame_MainTextArea::Init(bool (*EventHandler)(const struct TCEvent *Event),
//        uintptr_t FrameID)
//{
////    DisplayFrame=UITC_AllocDisplayFrame(ui->DisplayFrameWidget,EventHandler,
////            FrameID);
////    if(DisplayFrame==NULL)
////        throw(0);
//}
//
//void Frame_MainTextArea::ShutDown(void)
//{
////    if(DisplayFrame!=NULL)
////        UITC_FreeDisplayFrame(DisplayFrame);
//}

void Frame_MainTextArea::on_BlockSendSend_pushButton_clicked()
{
    struct TextDisplayEvent NewEvent;
    string EventStr;

    if(EventHandler==nullptr)
        return;

    EventStr=ui->BlockSend_textEdit->toPlainText().toStdString();

    NewEvent.EventType=e_TextDisplayEvent_SendBttn;
    NewEvent.ID=ID;
    NewEvent.Info.SendBttn.Buffer=(uint8_t *)EventStr.c_str();
    NewEvent.Info.SendBttn.Len=EventStr.length();

    EventHandler(&NewEvent);
}

void Frame_MainTextArea::on_TextverticalScrollBar_valueChanged(int value)
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_DisplayFrameScrollY;
    NewEvent.ID=ID;
    NewEvent.Info.Scroll.Amount=value;

    EventHandler(&NewEvent);
}

void Frame_MainTextArea::on_TexthorizontalScrollBar_valueChanged(int value)
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_DisplayFrameScrollX;
    NewEvent.ID=ID;
    NewEvent.Info.Scroll.Amount=value;

    EventHandler(&NewEvent);
}
