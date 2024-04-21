#include "Form_InstallPlugin.h"
#include "ui_Form_InstallPlugin.h"
#include "UI/UIInstallPlugin.h"

Form_InstallPlugin::Form_InstallPlugin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_InstallPlugin)
{
    DoingEvent=false;
    ui->setupUi(this);
}

Form_InstallPlugin::~Form_InstallPlugin()
{
    delete ui;
}

/*******************************************************************************
 * NAME:
 *    Form_InstallPlugin::SendEvent()
 *
 * SYNOPSIS:
 *    bool Form_InstallPlugin::SendEvent(e_DIPEventType EventType,
 *          union DSEventData *Info);
 *
 * PARAMETERS:
 *    EventType [I] -- The event to send (see UISettings.h)
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
 *    However there are some that you want to cancel the next part of the
 *    event (close main window for example).
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Form_InstallPlugin::SendEvent(e_DIPEventType EventType,
        union DIPEventData *Info,uintptr_t ID)
{
    struct DIPEvent NewEvent;
    bool RetValue;

    /* Stop changing a control programmatic from making events */
    if(DoingEvent)
        return true;

    NewEvent.EventType=EventType;
    NewEvent.ID=ID;
//    if(Info!=NULL)
//        NewEvent.Info=*Info;

    DoingEvent=true;
    RetValue=DIP_Event(&NewEvent);
    DoingEvent=false;

    return RetValue;
}
bool Form_InstallPlugin::SendEvent(e_DIPEventType EventType,union DIPEventData *Info)
{
    return SendEvent(EventType,Info,0);
}
bool Form_InstallPlugin::SendEvent(e_DIPEventType EventType)
{
    return SendEvent(EventType,NULL,0);
}

void Form_InstallPlugin::on_buttonBox_accepted()
{
    SendEvent(e_DIPEvent_DialogOk);
}

