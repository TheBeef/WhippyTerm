/* DEBUG PAUL: Rename this to Frame_MainTextColumn */
#include "Frame_MainTextCanvas.h"
#include "ui_Frame_MainTextCanvas.h"
#include "ContextMenuHelper.h"

Frame_MainTextCanvas::Frame_MainTextCanvas(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Frame_MainTextCanvas)
{
    ui->setupUi(this);
    ui->HeaderLabel->ParentSplitter=NULL;

    AddContextMenu2Widget(this,ui->HeaderLabel);
    AddContextMenu2Widget(this,ui->TexthorizontalScrollBar);
}

Frame_MainTextCanvas::~Frame_MainTextCanvas()
{
    delete ui;
}

void Frame_MainTextCanvas::HeaderVisible(bool Visible)
{
    ui->HeaderLabel->setVisible(Visible);
}

void Frame_MainTextCanvas::SetParentSplitter(QSplitter *Splitter)
{
    ui->HeaderLabel->ParentSplitter=Splitter;
}

void Frame_MainTextCanvas::SetHeaderLabel(const char *Name)
{
    ui->HeaderLabel->setText(Name);
}

void Frame_MainTextCanvas::SetEventHandler(bool (*EventHandler)(const struct FMTCEvent *Event),uintptr_t UserData)
{
    FMTCEventHandler=EventHandler;
    FMTCEventHandlerUserData=UserData;
}

/*******************************************************************************
 * NAME:
 *    Frame_MainTextCanvas::SendEvent()
 *
 * SYNOPSIS:
 *    bool Frame_MainTextCanvas::SendEvent(e_FMTCEventType EventType,
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
bool Frame_MainTextCanvas::SendEvent(e_FMTCEventType EventType,union FMTCEventData *Info)
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

bool Frame_MainTextCanvas::SendEvent(e_FMTCEventType EventType)
{
    return SendEvent(EventType,nullptr);
}

void Frame_MainTextCanvas::on_TexthorizontalScrollBar_valueChanged(int value)
{
    union FMTCEventData Info;

    Info.Scroll.Amount=value;

    SendEvent(e_FMTCEvent_ScrollX,&Info);
}

void Frame_MainTextCanvas::ContextMenu(const QPoint &pos)
{
    HandleContextMenuClick(sender(),pos);
}


void Frame_MainTextCanvas::on_Menu_HeaderLabel_x1_Hide_triggered()
{
    union FMTCEventData Info;

    Info.Context.Menu=e_UITD_ContextMenu_ColumnHeader_Hide;
    SendEvent(e_FMTCEvent_ContextMenu,&Info);
}

