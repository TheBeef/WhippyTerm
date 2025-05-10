#include "Frame_MainTextArea.h"
#include "ui_Frame_MainTextArea.h"
#include "UI/UITextMainArea.h"
#include <string>

using namespace std;

Frame_MainTextArea::Frame_MainTextArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Frame_MainTextArea)
{
    EventHandler=nullptr;
    ui->setupUi(this);

    ContextMenu = new QMenu;
    ContextMenu->addAction(ui->actionSend_Buffer);
    ContextMenu->addAction(ui->actionEdit);
    ContextMenu->addSeparator();
    ContextMenu->addAction(ui->actionCopy);
    ContextMenu->addAction(ui->actionPaste);
    ContextMenu->addSeparator();
    ContextMenu->addAction(ui->actionEndian_Swap);
    ContextMenu->addAction(ui->actionClear_Screen);
    ContextMenu->addSeparator();
    ContextMenu->addAction(ui->actionZoom_In);
    ContextMenu->addAction(ui->actionZoom_Out);
}

Frame_MainTextArea::~Frame_MainTextArea()
{
    delete ui;
}

void Frame_MainTextArea::on_BlockSendSend_pushButton_clicked()
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_ButtonPress;
    NewEvent.ID=ID;
    NewEvent.Info.ButtonPress.Bttn=e_UITC_Bttn_Send;

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

void Frame_MainTextArea::on_actionSend_Buffer_triggered()
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_ContextMenu;
    NewEvent.ID=ID;
    NewEvent.Info.Context.Menu=e_UITD_ContextMenu_SendBuffers;

    EventHandler(&NewEvent);
}


void Frame_MainTextArea::on_actionCopy_triggered()
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_ContextMenu;
    NewEvent.ID=ID;
    NewEvent.Info.Context.Menu=e_UITD_ContextMenu_Copy;

    EventHandler(&NewEvent);
}


void Frame_MainTextArea::on_actionPaste_triggered()
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_ContextMenu;
    NewEvent.ID=ID;
    NewEvent.Info.Context.Menu=e_UITD_ContextMenu_Paste;

    EventHandler(&NewEvent);
}


void Frame_MainTextArea::on_actionClear_Screen_triggered()
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_ContextMenu;
    NewEvent.ID=ID;
    NewEvent.Info.Context.Menu=e_UITD_ContextMenu_ClearScreen;

    EventHandler(&NewEvent);
}


void Frame_MainTextArea::on_actionZoom_In_triggered()
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_ContextMenu;
    NewEvent.ID=ID;
    NewEvent.Info.Context.Menu=e_UITD_ContextMenu_ZoomIn;

    EventHandler(&NewEvent);
}


void Frame_MainTextArea::on_actionZoom_Out_triggered()
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_ContextMenu;
    NewEvent.ID=ID;
    NewEvent.Info.Context.Menu=e_UITD_ContextMenu_ZoomOut;

    EventHandler(&NewEvent);
}


void Frame_MainTextArea::on_actionEdit_triggered()
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_ContextMenu;
    NewEvent.ID=ID;
    NewEvent.Info.Context.Menu=e_UITD_ContextMenu_Edit;

    EventHandler(&NewEvent);
}


void Frame_MainTextArea::on_actionEndian_Swap_triggered()
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_ContextMenu;
    NewEvent.ID=ID;
    NewEvent.Info.Context.Menu=e_UITD_ContextMenu_EndianSwap;

    EventHandler(&NewEvent);
}


void Frame_MainTextArea::on_BlockSendHex_Clear_pushButton_clicked()
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_ButtonPress;
    NewEvent.ID=ID;
    NewEvent.Info.ButtonPress.Bttn=e_UITC_Bttn_Clear;

    EventHandler(&NewEvent);
}

void Frame_MainTextArea::on_BlockSendHex_Edit_pushButton_clicked()
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_ButtonPress;
    NewEvent.ID=ID;
    NewEvent.Info.ButtonPress.Bttn=e_UITC_Bttn_HexEdit;

    EventHandler(&NewEvent);
}

void Frame_MainTextArea::on_BlockSend_TextMode_radioButton_clicked()
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_RadioButtonPress;
    NewEvent.ID=ID;

    NewEvent.Info.RadioButton.BttnID=e_UITC_RadioButton_Text;

    EventHandler(&NewEvent);
}

void Frame_MainTextArea::on_BlockSend_HexMode_radioButton_clicked()
{
    struct TextDisplayEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_RadioButtonPress;
    NewEvent.ID=ID;

    NewEvent.Info.RadioButton.BttnID=e_UITC_RadioButton_Hex;

    EventHandler(&NewEvent);
}

void Frame_MainTextArea::on_BlockSend_LineEnd_comboBox_activated(int index)
{
    struct TextDisplayEvent NewEvent;
    uintptr_t ComboxID;   // The ID for this item

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_TextDisplayEvent_ComboxChange;
    NewEvent.ID=ID;

    ComboxID=ui->BlockSend_LineEnd_comboBox->itemData(index).toULongLong();
    NewEvent.Info.Combox.ID=ComboxID;
    NewEvent.Info.Combox.BoxID=e_UITC_Combox_LineEnd;

    EventHandler(&NewEvent);
}

