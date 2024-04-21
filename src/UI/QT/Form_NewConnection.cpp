#include "Form_NewConnection.h"
#include "ui_Form_NewConnection.h"
#include "UI/UINewConnection.h"

Form_NewConnection::Form_NewConnection(QWidget *parent) :
    QDialog(parent,Qt::WindowSystemMenuHint | Qt::WindowTitleHint),
    ui(new Ui::Form_NewConnection)
{
    DoingEvent=false;

    ui->setupUi(this);
}

Form_NewConnection::~Form_NewConnection()
{
    delete ui;
}

void Form_NewConnection::on_pushButton_Rescan_clicked()
{
    union DNCEventData EventData;

    EventData.Bttn.BttnID=e_UINC_Button_Rescan;
    SendEvent(e_DNCEvent_BttnTriggered,&EventData);
}

void Form_NewConnection::on_ConnectionList_comboBox_currentIndexChanged(int index)
{
    uintptr_t ID;   // The ID for this item

    ID=ui->ConnectionList_comboBox->itemData(index).toULongLong();

    SendEvent(e_DNCEvent_ConnectionListChange,NULL,ID);
}

/*******************************************************************************
 * NAME:
 *    Form_NewConnection::SendEvent()
 *
 * SYNOPSIS:
 *    bool Form_NewConnection::SendEvent(e_DNCEventType EventType,
 *          union DNCEventData *Info);
 *
 * PARAMETERS:
 *    EventType [I] -- The event to send (see UINewConnection.h)
 *    Info [I] -- Extra info to send with this event.  Can be NULL for none.
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
bool Form_NewConnection::SendEvent(e_DNCEventType EventType,
        union DNCEventData *Info,uintptr_t ID)
{
    struct DNCEvent NewEvent;
    bool RetValue;

    /* Stop changing a control programmatic from making events */
    if(DoingEvent)
        return true;

    NewEvent.EventType=EventType;
    NewEvent.ID=ID;
    if(Info!=NULL)
        NewEvent.Info=*Info;

    DoingEvent=true;
    RetValue=DNC_Event(&NewEvent);
    DoingEvent=false;

    return RetValue;
}
bool Form_NewConnection::SendEvent(e_DNCEventType EventType,union DNCEventData *Info)
{
    return SendEvent(EventType,Info,0);
}
bool Form_NewConnection::SendEvent(e_DNCEventType EventType)
{
    return SendEvent(EventType,NULL,0);
}

void Form_NewConnection::on_Name_lineEdit_editingFinished()
{
    SendEvent(e_DNCEvent_NameChange,NULL,0);
}
