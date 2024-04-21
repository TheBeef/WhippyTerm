#include "Form_ComTest.h"
#include "ui_Form_ComTest.h"
#include "UI/UIComTest.h"

Form_ComTest::Form_ComTest(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_ComTest)
{
    ui->setupUi(this);
    DoingEvent=false;
}

Form_ComTest::~Form_ComTest()
{
    delete ui;
}

/*******************************************************************************
 * NAME:
 *    Form_ComTest::SendEvent()
 *
 * SYNOPSIS:
 *    bool Form_ComTest::SendEvent(e_CTEventType EventType,
 *          union CTEventData *Info,uintptr_t ID,bool IgnoreEventsInProgress);
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
bool Form_ComTest::SendEvent(e_CTEventType EventType,
        union CTEventData *Info,uintptr_t ID,bool IgnoreEventsInProgress)
{
    struct CTEvent NewEvent;
    bool RetValue;

    /* Stop changing a control programmatic from making events */
    if(DoingEvent && !IgnoreEventsInProgress)
        return true;

    NewEvent.EventType=EventType;
    NewEvent.ID=ID;
    if(Info!=NULL)
        NewEvent.Info=*Info;

    DoingEvent=true;
    RetValue=CT_Event(&NewEvent);
    DoingEvent=false;

    return RetValue;
}
bool Form_ComTest::SendEvent(e_CTEventType EventType,union CTEventData *Info)
{
    return SendEvent(EventType,Info,0);
}
bool Form_ComTest::SendEvent(e_CTEventType EventType)
{
    return SendEvent(EventType,NULL,0);
}

void Form_ComTest::on_Rescan2_pushButton_clicked()
{
    union CTEventData EventData;

    EventData.Bttn.BttnID=e_CT_Button_Rescan2;
    SendEvent(e_CTEvent_BttnTriggered,&EventData);
}

void Form_ComTest::on_Rescan1_pushButton_clicked()
{
    union CTEventData EventData;

    EventData.Bttn.BttnID=e_CT_Button_Rescan1;
    SendEvent(e_CTEvent_BttnTriggered,&EventData);
}

void Form_ComTest::on_Start_pushButton_clicked()
{
    union CTEventData EventData;

    EventData.Bttn.BttnID=e_CT_Button_Start;
    SendEvent(e_CTEvent_BttnTriggered,&EventData,0,true);
}

void Form_ComTest::on_Stop_pushButton_clicked()
{
    union CTEventData EventData;

    EventData.Bttn.BttnID=e_CT_Button_Stop;

    SendEvent(e_CTEvent_BttnTriggered,&EventData,0,true);
}

void Form_ComTest::on_ConnectionList1_comboBox_currentIndexChanged(int index)
{
    union CTEventData EventData;
    uintptr_t ID;   // The ID for this item

    ID=ui->ConnectionList1_comboBox->itemData(index).toULongLong();

    EventData.Combox.BoxID=e_CT_Combox_ConnectionList1;
    SendEvent(e_CTEvent_ComboxChange,&EventData,ID);
}

void Form_ComTest::on_ConnectionList2_comboBox_currentIndexChanged(int index)
{
    union CTEventData EventData;
    uintptr_t ID;   // The ID for this item

    ID=ui->ConnectionList2_comboBox->itemData(index).toULongLong();

    EventData.Combox.BoxID=e_CT_Combox_ConnectionList2;
    SendEvent(e_CTEvent_ComboxChange,&EventData,ID);
}

void Form_ComTest::on_Loopback_checkBox_clicked()
{
    union CTEventData EventData;

    EventData.Checkbox.BoxID=e_CT_Checkbox_Loopback;
    SendEvent(e_CTEvent_CheckboxChange,&EventData);
}

void Form_ComTest::on_FullDuplex_checkBox_clicked()
{
    union CTEventData EventData;

    EventData.Checkbox.BoxID=e_CT_Checkbox_FullDuplex;
    SendEvent(e_CTEvent_CheckboxChange,&EventData);
}

void Form_ComTest::on_Pattern_comboBox_currentIndexChanged(int index)
{
    union CTEventData EventData;
    uintptr_t ID;   // The ID for this item

    ID=ui->Pattern_comboBox->itemData(index).toULongLong();

    EventData.Combox.BoxID=e_CT_Combox_Pattern;
    SendEvent(e_CTEvent_ComboxChange,&EventData,ID);
}
