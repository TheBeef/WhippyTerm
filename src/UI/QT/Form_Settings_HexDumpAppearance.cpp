#include "Form_Settings_HexDumpAppearance.h"
#include "ui_Form_Settings_HexDumpAppearance.h"
#include "UI/UISettingsHexDumpAppearance.h"

Form_Settings_HexDumpAppearance::Form_Settings_HexDumpAppearance(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Form_Settings_HexDumpAppearance)
{
    DoingEvent=false;

    ui->setupUi(this);
}

Form_Settings_HexDumpAppearance::~Form_Settings_HexDumpAppearance()
{
    delete ui;
}

void Form_Settings_HexDumpAppearance::on_HexDisplay_SelectDivLineColor_pushButton_clicked()
{
    union SHDAEventData EventData;

    EventData.Bttn.BttnID=e_UISHDA_Button_SelectDivLineColor;
    SendEvent(e_SHDAEvent_BttnTriggered,&EventData);
}

void Form_Settings_HexDumpAppearance::on_HexDisplay_DivLineWidth_spinBox_valueChanged(int arg1)
{
    union SHDAEventData EventData;

    EventData.Number.InputID=e_UISHDA_NumberInput_DivLineWidth;
    EventData.Number.NewValue=arg1;
    SendEvent(e_SHDAEvent_NumberInputChanged,&EventData);
}

void Form_Settings_HexDumpAppearance::on_HexDisplay_BytesPerLine_spinBox_valueChanged(int arg1)
{
    union SHDAEventData EventData;

    EventData.Number.InputID=e_UISHDA_NumberInput_BytesPerLine;
    EventData.Number.NewValue=arg1;
    SendEvent(e_SHDAEvent_NumberInputChanged,&EventData);
}

/*******************************************************************************
 * NAME:
 *    Form_Settings_HexDumpAppearance::SendEvent()
 *
 * SYNOPSIS:
 *    bool Form_Settings_HexDumpAppearance::SendEvent(
 *          e_SHDAEventType EventType,union SHDAEventData *Info,uintptr_t ID);
 *
 * PARAMETERS:
 *    EventType [I] -- The event to send (see UISettingsHexDumpAppearance.h)
 *    Info [I] -- Extra info to send with this event.  Can be NULL for none.
 *    ID [I] -- A caller defined ID that is passed through with the event.
 *
 * FUNCTION:
 *    This function sends a hex dump appearance settings event out of the UI
 *    system to the main code.
 *
 * RETURNS:
 *    true -- Accept this event
 *    false -- Cancel this event
 *
 * NOTES:
 *    The return value is normally ignored as most events have no other
 *    action.  However there are some where you want to cancel the next part
 *    of the event.
 *
 * SEE ALSO:
 *    SHDA_Event()
 ******************************************************************************/
bool Form_Settings_HexDumpAppearance::SendEvent(e_SHDAEventType EventType,
        union SHDAEventData *Info,uintptr_t ID)
{
    struct SHDAEvent NewEvent;
    bool RetValue;

    /* Stop changing a control programmatic from making events */
    if(DoingEvent)
        return true;

    NewEvent.EventType=EventType;
    NewEvent.ID=ID;
    if(Info!=NULL)
        NewEvent.Info=*Info;

    DoingEvent=true;
    RetValue=SHDA_Event(&NewEvent);
    DoingEvent=false;

    return RetValue;
}
bool Form_Settings_HexDumpAppearance::SendEvent(e_SHDAEventType EventType,
        union SHDAEventData *Info)
{
    return SendEvent(EventType,Info,0);
}
bool Form_Settings_HexDumpAppearance::SendEvent(e_SHDAEventType EventType)
{
    return SendEvent(EventType,NULL,0);
}
