#include "Form_CRCFinder.h"
#include "ui_Form_CRCFinder.h"

Form_CRCFinder::Form_CRCFinder(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Form_CRCFinder)
{
    ui->setupUi(this);
    DoingEvent=false;
}

Form_CRCFinder::~Form_CRCFinder()
{
    delete ui;
}

/*******************************************************************************
 * NAME:
 *    Form_CRCFinder::SendEvent()
 *
 * SYNOPSIS:
 *    bool Form_CRCFinder::SendEvent(e_CFEventType EventType,
 *          union CFEventData *Info,uintptr_t ID);
 *
 * PARAMETERS:
 *    EventType [I] -- The event to send (see UICRCFinder.h)
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
bool Form_CRCFinder::SendEvent(e_CFEventType EventType,
        union CFEventData *Info,uintptr_t ID)
{
    struct CFEvent NewEvent;
    bool RetValue;

    /* Stop changing a control programmatic from making events */
    if(DoingEvent)
        return true;

    NewEvent.EventType=EventType;
    NewEvent.ID=ID;
    if(Info!=NULL)
        NewEvent.Info=*Info;

    DoingEvent=true;
    RetValue=CF_Event(&NewEvent);
    DoingEvent=false;

    return RetValue;
}
bool Form_CRCFinder::SendEvent(e_CFEventType EventType,union CFEventData *Info)
{
    return SendEvent(EventType,Info,0);
}
bool Form_CRCFinder::SendEvent(e_CFEventType EventType)
{
    return SendEvent(EventType,NULL,0);
}

void Form_CRCFinder::on_FindCRC_pushButton_clicked()
{
    union CFEventData Info;

    Info.Bttn.BttnID=e_CF_Button_FindCRC;
    SendEvent(e_CFEvent_BttnTriggered,&Info);
}


void Form_CRCFinder::on_ShowSource_pushButton_clicked()
{
    union CFEventData Info;

    Info.Bttn.BttnID=e_CF_Button_ShowSource;
    SendEvent(e_CFEvent_BttnTriggered,&Info);
}


void Form_CRCFinder::on_CRC_lineEdit_editingFinished()
{
    union CFEventData Info;

    Info.Txt.TxtID=e_CF_TextInput_CRC;

    SendEvent(e_CFEvent_TextEditDone,&Info);
}

void Form_CRCFinder::on_CRCType_comboBox_currentIndexChanged(int index)
{
    union CFEventData Info;
    uintptr_t ID;   // The ID for this item

    ID=ui->CRCType_comboBox->itemData(index).toULongLong();

    Info.Combox.BoxID=e_CF_Combox_CRCType;
    SendEvent(e_CTEvent_ComboxChange,&Info,ID);
}


void Form_CRCFinder::on_buttonBox_accepted()
{
    close();
}


void Form_CRCFinder::on_CRC_lineEdit_textEdited(const QString &arg1)
{
    union CFEventData Info;
    
    Info.Txt.TxtID=e_CF_TextInput_CRC;
    
    SendEvent(e_CFEvent_TextEdited,&Info);
}

