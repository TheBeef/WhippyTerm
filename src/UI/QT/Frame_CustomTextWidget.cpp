#include "Frame_CustomTextWidget.h"
#include "ui_Frame_CustomTextWidget.h"
#include "UI/UICustomTextWidget.h"
#include <string>

using namespace std;

Frame_CustomTextWidget::Frame_CustomTextWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Frame_CustomTextWidget)
{
    EventHandler=nullptr;
    ui->setupUi(this);

    ContextMenu = new QMenu;
    ContextMenu->addAction(ui->actionEdit);
    ContextMenu->addAction(ui->actionFind_CRC_Algorithm);
    ContextMenu->addAction(ui->actionCopy_To_Send_Buffer);
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

Frame_CustomTextWidget::~Frame_CustomTextWidget()
{
    delete ui;
}

void Frame_CustomTextWidget::on_TextverticalScrollBar_valueChanged(int value)
{
    struct UICTWEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_UICTWEvent_DisplayFrameScrollY;
    NewEvent.ID=ID;
    NewEvent.Info.Scroll.Amount=value;

    EventHandler(&NewEvent);
}

void Frame_CustomTextWidget::on_TexthorizontalScrollBar_valueChanged(int value)
{
    struct UICTWEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_UICTWEvent_DisplayFrameScrollX;
    NewEvent.ID=ID;
    NewEvent.Info.Scroll.Amount=value;

    EventHandler(&NewEvent);
}

void Frame_CustomTextWidget::on_actionCopy_triggered()
{
    struct UICTWEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_UICTWEvent_ContextMenu;
    NewEvent.ID=ID;
    NewEvent.Info.Context.Menu=e_UICTW_ContextMenu_Copy;

    EventHandler(&NewEvent);
}


void Frame_CustomTextWidget::on_actionPaste_triggered()
{
    struct UICTWEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_UICTWEvent_ContextMenu;
    NewEvent.ID=ID;
    NewEvent.Info.Context.Menu=e_UICTW_ContextMenu_Paste;

    EventHandler(&NewEvent);
}


void Frame_CustomTextWidget::on_actionClear_Screen_triggered()
{
    struct UICTWEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_UICTWEvent_ContextMenu;
    NewEvent.ID=ID;
    NewEvent.Info.Context.Menu=e_UICTW_ContextMenu_ClearScreen;

    EventHandler(&NewEvent);
}


void Frame_CustomTextWidget::on_actionZoom_In_triggered()
{
    struct UICTWEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_UICTWEvent_ContextMenu;
    NewEvent.ID=ID;
    NewEvent.Info.Context.Menu=e_UICTW_ContextMenu_ZoomIn;

    EventHandler(&NewEvent);
}


void Frame_CustomTextWidget::on_actionZoom_Out_triggered()
{
    struct UICTWEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_UICTWEvent_ContextMenu;
    NewEvent.ID=ID;
    NewEvent.Info.Context.Menu=e_UICTW_ContextMenu_ZoomOut;

    EventHandler(&NewEvent);
}


void Frame_CustomTextWidget::on_actionEdit_triggered()
{
    struct UICTWEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_UICTWEvent_ContextMenu;
    NewEvent.ID=ID;
    NewEvent.Info.Context.Menu=e_UICTW_ContextMenu_Edit;

    EventHandler(&NewEvent);
}


void Frame_CustomTextWidget::on_actionEndian_Swap_triggered()
{
    struct UICTWEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_UICTWEvent_ContextMenu;
    NewEvent.ID=ID;
    NewEvent.Info.Context.Menu=e_UICTW_ContextMenu_EndianSwap;

    EventHandler(&NewEvent);
}


void Frame_CustomTextWidget::on_actionFind_CRC_Algorithm_triggered()
{
    struct UICTWEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_UICTWEvent_ContextMenu;
    NewEvent.ID=ID;
    NewEvent.Info.Context.Menu=e_UICTW_ContextMenu_FindCRCAlgorithm;

    EventHandler(&NewEvent);
}


void Frame_CustomTextWidget::on_actionCopy_To_Send_Buffer_triggered()
{
    struct UICTWEvent NewEvent;

    if(EventHandler==nullptr)
        return;

    NewEvent.EventType=e_UICTWEvent_ContextMenu;
    NewEvent.ID=ID;
    NewEvent.Info.Context.Menu=e_UICTW_ContextMenu_CopyToSendBuffer;

    EventHandler(&NewEvent);
}

