#include "Frame_MainTextColumn.h"
#include "ui_Frame_MainTextColumn.h"
#include "ContextMenuHelper.h"

Frame_MainTextColumn::Frame_MainTextColumn(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Frame_MainTextColumn)
{
    ui->setupUi(this);
    ui->HeaderLabel->ParentSplitter=NULL;

    AddContextMenu2Widget(this,ui->HeaderLabel);
    AddContextMenu2Widget(this,ui->TexthorizontalScrollBar);
}

Frame_MainTextColumn::~Frame_MainTextColumn()
{
    delete ui;
}

void Frame_MainTextColumn::HeaderVisible(bool Visible)
{
    ui->HeaderLabel->setVisible(Visible);
}

void Frame_MainTextColumn::SetParentSplitter(QSplitter *Splitter)
{
    ui->HeaderLabel->ParentSplitter=Splitter;
}

void Frame_MainTextColumn::SetHeaderLabel(const char *Name)
{
    ui->HeaderLabel->setText(Name);
}

void Frame_MainTextColumn::SetEventHandler(bool (*EventHandler)(const struct FMTCEvent *Event),uintptr_t UserData)
{
    FMTCEventHandler=EventHandler;
    FMTCEventHandlerUserData=UserData;
}

/*******************************************************************************
 * NAME:
 *    Frame_MainTextColumn::SendEvent()
 *
 * SYNOPSIS:
 *    bool Frame_MainTextColumn::SendEvent(e_FMTCEventType EventType,
 *          union FMTCEventData *Info);
 *
 * PARAMETERS:
 *    EventType [I] -- The event to send
 *    Info [I] -- Extra info to send with this event.  Can be NULL for none.
 *
 * FUNCTION:
 *    This function sends a text canvas event out to it's registered event
 *    handler.
 *
 * RETURNS:
 *    true -- Accept this event
 *    false -- Cancel this event
 *
 * NOTES:
 *    The return value is normally ignored as most events have no other action.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Frame_MainTextColumn::SendEvent(e_FMTCEventType EventType,union FMTCEventData *Info)
{
    struct FMTCEvent NewEvent;

    NewEvent.EventType=EventType;
    NewEvent.UserData=FMTCEventHandlerUserData;
    NewEvent.Info=Info;
    NewEvent.Source=this;

    if(FMTCEventHandler!=NULL)
        return FMTCEventHandler(&NewEvent);
    return true;
}

bool Frame_MainTextColumn::SendEvent(e_FMTCEventType EventType)
{
    return SendEvent(EventType,nullptr);
}

void Frame_MainTextColumn::on_TexthorizontalScrollBar_valueChanged(int value)
{
    union FMTCEventData Info;

    Info.Scroll.Amount=value;

    SendEvent(e_FMTCEvent_ScrollX,&Info);
}

void Frame_MainTextColumn::ContextMenu(const QPoint &pos)
{
    HandleContextMenuClick(sender(),pos);
}


void Frame_MainTextColumn::on_Menu_HeaderLabel_x1_Hide_triggered()
{
    union FMTCEventData Info;

    Info.Context.Menu=e_UITD_ContextMenu_ColumnHeader_Hide;
    SendEvent(e_FMTCEvent_ContextMenu,&Info);
}

