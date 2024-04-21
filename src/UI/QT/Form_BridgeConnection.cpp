#include "Form_BridgeConnection.h"
#include "ui_Form_BridgeConnection.h"

Form_BridgeConnection::Form_BridgeConnection(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_BridgeConnection)
{
    ui->setupUi(this);
    DoingEvent=false;
}

Form_BridgeConnection::~Form_BridgeConnection()
{
    delete ui;
}

/*******************************************************************************
 * NAME:
 *    Form_BridgeConnection::SendEvent()
 *
 * SYNOPSIS:
 *    bool Form_BridgeConnection::SendEvent(e_BCEventType EventType,
 *          union BCEventData *Info,uintptr_t ID,bool IgnoreEventsInProgress);
 *
 * PARAMETERS:
 *    EventType [I] -- The event to send (see UINewConnection.h)
 *    Info [I] -- Extra info to send with this event.  Can be NULL for none.
 *    ID [I] -- The value to set the event ID to
 *    IgnoreEventsInProgress [I] -- Do we ignore repeat events
 *
 * FUNCTION:
 *    This function sends a man bookmark event out of the UI system to the main
 *    code.
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
bool Form_BridgeConnection::SendEvent(e_BCEventType EventType,
        union BCEventData *Info,uintptr_t ID,bool IgnoreEventsInProgress)
{
    struct BCEvent NewEvent;
    bool RetValue;

    /* Stop changing a control programmatic from making events */
    if(DoingEvent && !IgnoreEventsInProgress)
        return true;

    NewEvent.EventType=EventType;
    NewEvent.ID=ID;
    if(Info!=NULL)
        NewEvent.Info=*Info;

    DoingEvent=true;
    RetValue=BC_Event(&NewEvent);
    DoingEvent=false;

    return RetValue;
}
bool Form_BridgeConnection::SendEvent(e_BCEventType EventType,
        union BCEventData *Info)
{
    return SendEvent(EventType,Info,0);
}
bool Form_BridgeConnection::SendEvent(e_BCEventType EventType)
{
    return SendEvent(EventType,NULL,0);
}

void Form_BridgeConnection::on_comboBox_Connection2_activated(int index)
{
    union BCEventData EventData;
    uintptr_t ID;   // The ID for this item

    ID=ui->comboBox_Connection2->itemData(index).toULongLong();

    EventData.Combox.BoxID=e_BC_Combox_Connection2;
    SendEvent(e_BCEvent_ComboxChange,&EventData,ID);
}
