#include "Form_NewVersionCheck.h"
#include "ui_Form_NewVersionCheck.h"

Form_NewVersionCheck::Form_NewVersionCheck(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Form_NewVersionCheck)
{
    DoingEvent=false;
    ui->setupUi(this);

    SimProgressTick=new QTimer(this);
    connect(SimProgressTick, &QTimer::timeout, this, &Form_NewVersionCheck::SimProgressTick_timeout);
    SimProgressTick->start(1000);
}

Form_NewVersionCheck::~Form_NewVersionCheck()
{
    delete SimProgressTick;
    delete ui;
}

/*******************************************************************************
 * NAME:
 *    Form_NewVersionCheck::SendEvent()
 *
 * SYNOPSIS:
 *    bool Form_NewVersionCheck::SendEvent(e_NVCEventType EventType,
 *          union NVCEventData *Info,bool IgnoreEventsInProgress);
 *
 * PARAMETERS:
 *    EventType [I] -- The event to send (see UINewConnection.h)
 *    Info [I] -- Extra info to send with this event.  Can be NULL for none.
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
bool Form_NewVersionCheck::SendEvent(e_NVCEventType EventType,
        union NVCEventData *Info,bool IgnoreEventsInProgress)
{
    struct NVCEvent NewEvent;
    bool RetValue;

    /* Stop changing a control programmatic from making events */
    if(DoingEvent && !IgnoreEventsInProgress)
        return true;

    NewEvent.EventType=EventType;
    if(Info!=NULL)
        NewEvent.Info=*Info;

    DoingEvent=true;
    RetValue=NVC_Event(&NewEvent);
    DoingEvent=false;

    return RetValue;
}
bool Form_NewVersionCheck::SendEvent(e_NVCEventType EventType,
        union NVCEventData *Info)
{
    return SendEvent(EventType,Info,0);
}
bool Form_NewVersionCheck::SendEvent(e_NVCEventType EventType)
{
    return SendEvent(EventType,NULL,0);
}

void Form_NewVersionCheck::on_pushButton_clicked()
{
    union NVCEventData Info;

    Info.Bttn.BttnID=e_NVC_Button_GotoWebPage;
    SendEvent(e_NVCEvent_BttnTriggered,&Info);
}

void Form_NewVersionCheck::SimProgressTick_timeout()
{
    SendEvent(e_NVCEvent_Timer);
}
