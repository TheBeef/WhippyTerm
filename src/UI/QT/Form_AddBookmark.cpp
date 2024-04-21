#include "Form_AddBookmark.h"
#include "ui_Form_AddBookmark.h"
#include "UI/UIAddBookmark.h"

Form_AddBookmark::Form_AddBookmark(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_AddBookmark)
{
    ui->setupUi(this);
}

Form_AddBookmark::~Form_AddBookmark()
{
    delete ui;
}

void Form_AddBookmark::on_New_pushButton_clicked()
{
    union ABDEventData EventData;

    EventData.Bttn.BttnID=e_UIABDBttn_NewFolder;
    SendEvent(e_ABDEvent_BttnTriggered,&EventData);
}

/*******************************************************************************
 * NAME:
 *    Form_AddBookmark::SendEvent()
 *
 * SYNOPSIS:
 *    bool Form_AddBookmark::SendEvent(e_ABDEventType EventType,
 *          union ABDEventData *Info);
 *
 * PARAMETERS:
 *    EventType [I] -- The event to send (see UIAddBookmark.h)
 *    Info [I] -- Extra info to send with this event.  Can be NULL for none.
 *
 * FUNCTION:
 *    This function sends a main window event out of the UI system to the main
 *    code.
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
bool Form_AddBookmark::SendEvent(e_ABDEventType EventType,union ABDEventData *Info,
        uintptr_t ID)
{
    struct ABDEvent NewEvent;

    NewEvent.EventType=EventType;
    NewEvent.ID=ID;
    if(Info!=NULL)
        NewEvent.Info=*Info;

    return ABD_Event(&NewEvent);
}
bool Form_AddBookmark::SendEvent(e_ABDEventType EventType,union ABDEventData *Info)
{
    return SendEvent(EventType,Info,0);
}
bool Form_AddBookmark::SendEvent(e_ABDEventType EventType)
{
    return SendEvent(EventType,NULL,0);
}
