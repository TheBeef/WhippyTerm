#include "Form_SendByte.h"
#include "ui_Form_SendByte.h"

Form_SendByte::Form_SendByte(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_SendByte)
{
    DoingEvent=false;
    ui->setupUi(this);
}

Form_SendByte::~Form_SendByte()
{
    delete ui;
}

/*******************************************************************************
 * NAME:
 *    Form_SendByte::SendEvent()
 *
 * SYNOPSIS:
 *    bool Form_SendByte::SendEvent(e_SBDEventType EventType,
 *          union SBDEventData *Info,uintptr_t ID);
 *
 * PARAMETERS:
 *    EventType [I] -- The event to send (see UISendByte.h)
 *    Info [I] -- Extra info to send with this event.  Can be NULL for none.
 *    ID [I] -- The value to fill in the ID field of the event.  Normally user
 *              set.
 *
 * FUNCTION:
 *    This function sends a edit send buffer event out of the UI system
 *    to the main code.
 *
 * RETURNS:
 *    true -- Accept this event
 *    false -- Cancel this event
 *
 * NOTES:
 *    The return value is normally ignored as most events have no other action.
 *    However there are some that you want to cancel the next part of the
 *    event.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Form_SendByte::SendEvent(e_SBDEventType EventType,
        union SBDEventData *Info,uintptr_t ID)
{
    struct SBDEvent NewEvent;
    bool RetValue;

    /* Stop changing a control programmatic from making events */
    if(DoingEvent)
        return true;

    NewEvent.EventType=EventType;
    NewEvent.ID=ID;
    if(Info!=NULL)
        NewEvent.Info=*Info;

    DoingEvent=true;
    RetValue=SBD_Event(&NewEvent);
    DoingEvent=false;

    return RetValue;
}
bool Form_SendByte::SendEvent(e_SBDEventType EventType,union SBDEventData *Info)
{
    return SendEvent(EventType,Info,0);
}
bool Form_SendByte::SendEvent(e_SBDEventType EventType)
{
    return SendEvent(EventType,NULL,0);
}


void Form_SendByte::on_AscII_tableWidget_itemSelectionChanged()
{
    union SBDEventData Info;
    int row;
    int col;

    row=ui->AscII_tableWidget->currentRow();
    col=ui->AscII_tableWidget->currentColumn();

    Info.AscII.ByteValue=row*16+col;
    SendEvent(e_SBDEvent_AscIISelected,&Info);
}

void Form_SendByte::on_AscII_tableWidget_doubleClicked(const QModelIndex &index)
{
    accept();
}

void Form_SendByte::on_Dec_lineEdit_editingFinished()
{
    union SBDEventData Info;

    Info.Txt.TxtID=e_SBD_TextInput_Dec;
    SendEvent(e_SBDEvent_TextEditDone,&Info);
}


void Form_SendByte::on_Hex_lineEdit_editingFinished()
{
    union SBDEventData Info;

    Info.Txt.TxtID=e_SBD_TextInput_Hex;
    SendEvent(e_SBDEvent_TextEditDone,&Info);
}


void Form_SendByte::on_Oct_lineEdit_editingFinished()
{
    union SBDEventData Info;

    Info.Txt.TxtID=e_SBD_TextInput_Oct;
    SendEvent(e_SBDEvent_TextEditDone,&Info);
}

