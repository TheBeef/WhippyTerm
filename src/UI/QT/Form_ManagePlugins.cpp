#include "Form_ManagePlugins.h"
#include "ui_Form_ManagePlugins.h"
#include "UI/UIManagePlugins.h"

Form_ManagePlugins::Form_ManagePlugins(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_ManagePlugins)
{
    DoingEvent=false;
    ui->setupUi(this);
}

Form_ManagePlugins::~Form_ManagePlugins()
{
    delete ui;
}

/*******************************************************************************
 * NAME:
 *    Form_ManagePlugins::SendEvent()
 *
 * SYNOPSIS:
 *    bool Form_ManagePlugins::SendEvent(e_DMPEventType EventType,
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
bool Form_ManagePlugins::SendEvent(e_DMPEventType EventType,
        union DMPEventData *Info,uintptr_t ID)
{
    struct DMPEvent NewEvent;
    bool RetValue;

    /* Stop changing a control programmatic from making events */
    if(DoingEvent)
        return true;

    NewEvent.EventType=EventType;
    NewEvent.ID=ID;
    if(Info!=NULL)
        NewEvent.Info=*Info;

    DoingEvent=true;
    RetValue=DMP_Event(&NewEvent);
    DoingEvent=false;

    return RetValue;
}
bool Form_ManagePlugins::SendEvent(e_DMPEventType EventType,union DMPEventData *Info)
{
    return SendEvent(EventType,Info,0);
}
bool Form_ManagePlugins::SendEvent(e_DMPEventType EventType)
{
    return SendEvent(EventType,NULL,0);
}

void Form_ManagePlugins::on_Enable_pushButton_clicked()
{
    union DMPEventData EventData;

    EventData.Bttn.InputID=e_UIMP_Button_Enable;
    SendEvent(e_DMPEvent_BttnTriggered,&EventData);
}

void Form_ManagePlugins::on_Install_pushButton_clicked()
{
    union DMPEventData EventData;

    EventData.Bttn.InputID=e_UIMP_Button_Install;
    SendEvent(e_DMPEvent_BttnTriggered,&EventData);
}

void Form_ManagePlugins::on_Uninstall_pushButton_clicked()
{
    union DMPEventData EventData;

    EventData.Bttn.InputID=e_UIMP_Button_Uninstall;
    SendEvent(e_DMPEvent_BttnTriggered,&EventData);
}

void Form_ManagePlugins::on_buttonBox_accepted()
{
    SendEvent(e_DMPEvent_DialogOk);
}

void Form_ManagePlugins::on_PluginList_listWidget_currentRowChanged(int currentRow)
{
    uintptr_t ID;   // The ID for this item
    union DMPEventData EventData;
    QListWidgetItem *item;

    item=ui->PluginList_listWidget->currentItem();
    if(item==NULL)
        return;

    ID=(uintptr_t)(item->data(Qt::UserRole).toULongLong());

    EventData.ListView.InputID=e_UIMP_ListView_PluginList;
    SendEvent(e_DMPEvent_ListViewChange,&EventData,ID);
}


void Form_ManagePlugins::on_Upgrade_pushButton_clicked()
{
    union DMPEventData EventData;

    EventData.Bttn.InputID=e_UIMP_Button_Upgrade;
    SendEvent(e_DMPEvent_BttnTriggered,&EventData);
}

