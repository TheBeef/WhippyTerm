#include "Form_SendBufferSelect.h"
#include "ui_Form_SendBufferSelect.h"

Form_SendBufferSelect::Form_SendBufferSelect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_SendBufferSelect)
{
    ui->setupUi(this);
    DoingEvent=false;
}

Form_SendBufferSelect::~Form_SendBufferSelect()
{
    delete ui;
}

/*******************************************************************************
 * NAME:
 *    Form_SendBufferSelect::SendEvent()
 *
 * SYNOPSIS:
 *    bool Form_SendBufferSelect::SendEvent(e_SBSEventType EventType,
 *          union SBSEventData *Info,uintptr_t ID,bool IgnoreEventsInProgress);
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
bool Form_SendBufferSelect::SendEvent(e_SBSEventType EventType,
        union SBSEventData *Info,uintptr_t ID,bool IgnoreEventsInProgress)
{
    struct SBSEvent NewEvent;
    bool RetValue;

    /* Stop changing a control programmatic from making events */
    if(DoingEvent && !IgnoreEventsInProgress)
        return true;

    NewEvent.EventType=EventType;
    NewEvent.ID=ID;
    if(Info!=NULL)
        NewEvent.Info=*Info;

    DoingEvent=true;
    RetValue=SBS_Event(&NewEvent);
    DoingEvent=false;

    return RetValue;
}
bool Form_SendBufferSelect::SendEvent(e_SBSEventType EventType,union SBSEventData *Info)
{
    return SendEvent(EventType,Info,0);
}
bool Form_SendBufferSelect::SendEvent(e_SBSEventType EventType)
{
    return SendEvent(EventType,NULL,0);
}

void Form_SendBufferSelect::on_pushButton_Cancel_clicked()
{
    union SBSEventData Info;
    Info.Bttn.BttnID=e_SBS_Button_Cancel;
    SendEvent(e_SBSEvent_BttnTriggered,&Info);
}


void Form_SendBufferSelect::on_pushButton_Go_clicked()
{
    union SBSEventData Info;
    Info.Bttn.BttnID=e_SBS_Button_GoButton;
    SendEvent(e_SBSEvent_BttnTriggered,&Info);
}


void Form_SendBufferSelect::on_treeWidget_BufferList_itemSelectionChanged()
{
    union SBSEventData Info;
    QTreeWidgetItem *TreeItem;
    int Row;

    if(ui->treeWidget_BufferList->selectedItems().isEmpty())
    {
        Row=-1;
    }
    else
    {
        TreeItem=ui->treeWidget_BufferList->selectedItems().first();
        Row=ui->treeWidget_BufferList->indexOfTopLevelItem(TreeItem);
    }

    Info.ColumnView.InputID=e_SBSColumnView_Buffers_List;
    Info.ColumnView.NewRow=Row;

    SendEvent(e_SBSEvent_ColumnViewSelectionChanged,&Info);
}

