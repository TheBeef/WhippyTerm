#include "Form_CalcCrc.h"
#include "ui_Form_CalcCrc.h"

Form_CalcCrc::Form_CalcCrc(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_CalcCrc)
{
    ui->setupUi(this);

    DoingEvent=false;
}

Form_CalcCrc::~Form_CalcCrc()
{
    delete ui;
}

/*******************************************************************************
 * NAME:
 *    Form_CalcCrc::SendEvent()
 *
 * SYNOPSIS:
 *    bool Form_CalcCrc::SendEvent(e_CCRCEventType EventType,
 *          union CCRCEventData *Info,uintptr_t ID);
 *
 * PARAMETERS:
 *    EventType [I] -- The event to send (see UICalcCrc.h)
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
bool Form_CalcCrc::SendEvent(e_CCRCEventType EventType,
        union CCRCEventData *Info,uintptr_t ID)
{
    struct CCRCEvent NewEvent;
    bool RetValue;

    /* Stop changing a control programmatic from making events */
    if(DoingEvent)
        return true;

    NewEvent.EventType=EventType;
    NewEvent.ID=ID;
    if(Info!=NULL)
        NewEvent.Info=*Info;

    DoingEvent=true;
    RetValue=CCRC_Event(&NewEvent);
    DoingEvent=false;

    return RetValue;
}
bool Form_CalcCrc::SendEvent(e_CCRCEventType EventType,union CCRCEventData *Info)
{
    return SendEvent(EventType,Info,0);
}
bool Form_CalcCrc::SendEvent(e_CCRCEventType EventType)
{
    return SendEvent(EventType,NULL,0);
}

void Form_CalcCrc::SendBttnEvent(e_CCRC_Button Bttn)
{
    union CCRCEventData Info;

    Info.Bttn.BttnID=Bttn;

    SendEvent(e_CCRCEvent_BttnTriggered,&Info);
}

void Form_CalcCrc::on_CalcCRC_pushButton_clicked()
{
    SendBttnEvent(e_CCRC_Button_CalcCRC);
}

void Form_CalcCrc::on_ShowSource_pushButton_clicked()
{
    SendBttnEvent(e_CCRC_Button_ShowSource);
}

void Form_CalcCrc::on_CRCType_comboBox_currentIndexChanged(int index)
{
    union CCRCEventData Info;
    uintptr_t ID;   // The ID for this item

    ID=ui->CRCType_comboBox->itemData(index).toULongLong();

    Info.Combox.BoxID=e_CCRC_Combox_CRCType;
    SendEvent(e_CCRCEvent_ComboxChange,&Info,ID);
}
