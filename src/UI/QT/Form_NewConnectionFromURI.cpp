#include "Form_NewConnectionFromURI.h"
#include "ui_Form_NewConnectionFromURI.h"
#include "UI/UINewConnectionFromURI.h"

Form_NewConnectionFromURI::Form_NewConnectionFromURI(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Form_NewConnectionFromURI)
{
    ui->setupUi(this);
    DoingEvent=false;
}

Form_NewConnectionFromURI::~Form_NewConnectionFromURI()
{
    delete ui;
}

/*******************************************************************************
 * NAME:
 *    Form_NewConnectionFromURI::SendEvent()
 *
 * SYNOPSIS:
 *    bool Form_NewConnectionFromURI::SendEvent(e_DNCFUEventType EventType,
 *          union DNCFUEventData *Info);
 *
 * PARAMETERS:
 *    EventType [I] -- The event to send (see UINewConnection.h)
 *    Info [I] -- Extra info to send with this event.  Can be NULL for none.
 *
 * FUNCTION:
 *    This function sends an event out of the UI system to the main code.
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
bool Form_NewConnectionFromURI::SendEvent(e_DNCFUEventType EventType,
        union DNCFUEventData *Info,uintptr_t ID)
{
    struct DNCFUEvent NewEvent;
    bool RetValue;

    /* Stop changing a control programmatic from making events */
    if(DoingEvent)
        return true;

    NewEvent.EventType=EventType;
    NewEvent.ID=ID;
    if(Info!=NULL)
        NewEvent.Info=*Info;

    DoingEvent=true;
    RetValue=DNCFU_Event(&NewEvent);
    DoingEvent=false;

    return RetValue;
}
bool Form_NewConnectionFromURI::SendEvent(e_DNCFUEventType EventType,union DNCFUEventData *Info)
{
    return SendEvent(EventType,Info,0);
}
bool Form_NewConnectionFromURI::SendEvent(e_DNCFUEventType EventType)
{
    return SendEvent(EventType,NULL,0);
}

void Form_NewConnectionFromURI::on_DriverList_listWidget_itemSelectionChanged()
{
    SendEvent(e_DNCFUEvent_DriverChanged);
}


void Form_NewConnectionFromURI::on_URI_lineEdit_textEdited(const QString &arg1)
{
    SendEvent(e_DNCFUEvent_URIChanged);
}

