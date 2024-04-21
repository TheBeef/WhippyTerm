#include "Form_ESB_CRCType.h"
#include "ui_Form_ESB_CRCType.h"

Form_ESB_CRCType::Form_ESB_CRCType(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_ESB_CRCType)
{
    ui->setupUi(this);

    DoingEvent=false;
}

Form_ESB_CRCType::~Form_ESB_CRCType()
{
    delete ui;
}

void Form_ESB_CRCType::on_ShowSource_pushButton_clicked()
{
    SendBttnEvent(e_ESBCT_Button_ShowSource);
}

/*******************************************************************************
 * NAME:
 *    Form_ESB_CRCType::SendEvent()
 *
 * SYNOPSIS:
 *    bool Form_ESB_CRCType::SendEvent(e_ESBEventType EventType,
 *          union ESBEventData *Info,uintptr_t ID);
 *
 * PARAMETERS:
 *    EventType [I] -- The event to send (see UIEditSendBuffer.h)
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
bool Form_ESB_CRCType::SendEvent(e_ESBCTEventType EventType,
        union ESBCTEventData *Info,uintptr_t ID)
{
    struct ESBCTEvent NewEvent;
    bool RetValue;

    /* Stop changing a control programmatic from making events */
    if(DoingEvent)
        return true;

    NewEvent.EventType=EventType;
    NewEvent.ID=ID;
    if(Info!=NULL)
        NewEvent.Info=*Info;

    DoingEvent=true;
    RetValue=ESBCT_Event(&NewEvent);
    DoingEvent=false;

    return RetValue;
}
bool Form_ESB_CRCType::SendEvent(e_ESBCTEventType EventType,union ESBCTEventData *Info)
{
    return SendEvent(EventType,Info,0);
}
bool Form_ESB_CRCType::SendEvent(e_ESBCTEventType EventType)
{
    return SendEvent(EventType,NULL,0);
}

void Form_ESB_CRCType::SendBttnEvent(e_ESBCT_Button Bttn)
{
    union ESBCTEventData Info;

    Info.Bttn.BttnID=Bttn;

    SendEvent(e_ESBCTEvent_BttnTriggered,&Info);
}
