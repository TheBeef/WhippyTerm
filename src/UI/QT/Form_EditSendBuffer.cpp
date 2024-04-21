#include "Form_EditSendBuffer.h"
#include "ui_Form_EditSendBuffer.h"

Form_EditSendBuffer::Form_EditSendBuffer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_EditSendBuffer)
{
    ui->setupUi(this);

    DoingEvent=false;

    MoreMenu = new QMenu;
    LoadBufferMenu=MoreMenu->addAction("Load Buffer...", this, SLOT(MoreMenu_LoadBuffer_clicked()));
    SaveBufferMenu=MoreMenu->addAction("Save Buffer...", this, SLOT(MoreMenu_SaveBuffer_clicked()));
    MoreMenu->addSeparator();
    ImportDataMenu=MoreMenu->addAction("Import Data...", this, SLOT(MoreMenu_ImportData_clicked()));
    ExportDataMenu=MoreMenu->addAction("Export Data...", this, SLOT(MoreMenu_ExportData_clicked()));
    MoreMenu->addSeparator();
    InsertFromDiskMenu=MoreMenu->addAction("Insert Data From Disk...", this, SLOT(MoreMenu_InsertFromDisk_clicked()));
    InsertFromClipboardMenu=MoreMenu->addAction("Insert Data From Clipboard...", this, SLOT(MoreMenu_InsertFromClipboard_clicked()));
}

Form_EditSendBuffer::~Form_EditSendBuffer()
{
    delete ui;
}

/*******************************************************************************
 * NAME:
 *    Form_EditSendBuffer::SendEvent()
 *
 * SYNOPSIS:
 *    bool Form_EditSendBuffer::SendEvent(e_ESBEventType EventType,
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
bool Form_EditSendBuffer::SendEvent(e_ESBEventType EventType,
        union ESBEventData *Info,uintptr_t ID)
{
    struct ESBEvent NewEvent;
    bool RetValue;

    /* Stop changing a control programmatic from making events */
    if(DoingEvent)
        return true;

    NewEvent.EventType=EventType;
    NewEvent.ID=ID;
    if(Info!=NULL)
        NewEvent.Info=*Info;

    DoingEvent=true;
    RetValue=ESB_Event(&NewEvent);
    DoingEvent=false;

    return RetValue;
}
bool Form_EditSendBuffer::SendEvent(e_ESBEventType EventType,union ESBEventData *Info)
{
    return SendEvent(EventType,Info,0);
}
bool Form_EditSendBuffer::SendEvent(e_ESBEventType EventType)
{
    return SendEvent(EventType,NULL,0);
}

void Form_EditSendBuffer::SendBttnEvent(e_ESB_Button Bttn)
{
    union ESBEventData Info;

    Info.Bttn.BttnID=Bttn;

    SendEvent(e_ESBEvent_BttnTriggered,&Info);
}

void Form_EditSendBuffer::SendContextEvent(e_ESB_ContextMenu Menu)
{
    union ESBEventData Info;

    Info.Context.MenuID=Menu;

    SendEvent(e_ESBEvent_ContextMenuClicked,&Info);
}

void Form_EditSendBuffer::on_Fill_pushButton_clicked()
{
    SendBttnEvent(e_ESB_Button_Fill);
}

void Form_EditSendBuffer::on_EndianSwap_pushButton_clicked()
{
    SendBttnEvent(e_ESB_Button_EndianSwap);
}

void Form_EditSendBuffer::on_CRCType_pushButton_clicked()
{
    SendBttnEvent(e_ESB_Button_CRCType);
}

void Form_EditSendBuffer::on_InsertCRC_pushButton_clicked()
{
    SendBttnEvent(e_ESB_Button_InsertCRC);
}

void Form_EditSendBuffer::on_InsertAsNumber_pushButton_clicked()
{
    SendBttnEvent(e_ESB_Button_InsertAsNumber);
}

void Form_EditSendBuffer::on_InsertAsText_pushButton_clicked()
{
    SendBttnEvent(e_ESB_Button_InsertAsText);
}

void Form_EditSendBuffer::on_InsertProperties_pushButton_clicked()
{
    SendBttnEvent(e_ESB_Button_InsertProperties);
}

void Form_EditSendBuffer::on_BufferSize_lineEdit_editingFinished()
{
    union ESBEventData Info;

    Info.Txt.TxtID=e_ESB_TextInput_BufferSize;

    SendEvent(e_ESBEvent_TextEditDone,&Info);
}

void Form_EditSendBuffer::on_More_pushButton_customContextMenuRequested(const QPoint &pos)
{
    on_More_pushButton_clicked();
}

void Form_EditSendBuffer::on_More_pushButton_clicked()
{
    MoreMenu->exec(QCursor::pos());
}

void Form_EditSendBuffer::MoreMenu_LoadBuffer_clicked()
{
    SendContextEvent(e_ESB_ContextMenu_LoadBuffer);
}

void Form_EditSendBuffer::MoreMenu_SaveBuffer_clicked()
{
    SendContextEvent(e_ESB_ContextMenu_SaveBuffer);
}

void Form_EditSendBuffer::MoreMenu_ImportData_clicked()
{
    SendContextEvent(e_ESB_ContextMenu_ImportData);
}

void Form_EditSendBuffer::MoreMenu_ExportData_clicked()
{
    SendContextEvent(e_ESB_ContextMenu_ExportData);
}

void Form_EditSendBuffer::MoreMenu_InsertFromDisk_clicked()
{
    SendContextEvent(e_ESB_ContextMenu_InsertFromDisk);
}

void Form_EditSendBuffer::MoreMenu_InsertFromClipboard_clicked()
{
    SendContextEvent(e_ESB_ContextMenu_InsertFromClipboard);
}

void Form_EditSendBuffer::on_Clear_pushButton_clicked()
{
    SendBttnEvent(e_ESB_Button_Clear);
}
