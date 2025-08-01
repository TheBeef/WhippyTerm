#include "Form_ManBookmark.h"
#include "ui_Form_ManBookmark.h"
#include "UI/UIManBookmark.h"
#include <QDropEvent>

MB_DragItems_QTreeWidget::MB_DragItems_QTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
}

void MB_DragItems_QTreeWidget::dropEvent(QDropEvent *event)
{
    QModelIndex droppedIndex;
    QTreeWidgetItem *DroppedOnItem;
    int DropPlacement;

    if(draggedItem==NULL)
        return;

    droppedIndex=indexAt(event->pos());
    DroppedOnItem=itemFromIndex(droppedIndex);

    switch(dropIndicatorPosition())
    {
        case QAbstractItemView::OnItem:
            DropPlacement=TREEVIEW_DROP_ON;
        break;
        case QAbstractItemView::AboveItem:
            DropPlacement=TREEVIEW_DROP_ABOVE;
        break;
        case QAbstractItemView::BelowItem:
            DropPlacement=TREEVIEW_DROP_BELOW;
        break;
        case QAbstractItemView::OnViewport:
            DropPlacement=TREEVIEW_DROP_WIDGET;
        break;
        default:
            DropPlacement=TREEVIEW_DROP_ON;
        break;
    }

    emit Dropped(draggedItem,DroppedOnItem,DropPlacement);
}

void MB_DragItems_QTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    draggedItem = currentItem();
    QTreeWidget::dragEnterEvent(event);
}

Form_ManBookmark::Form_ManBookmark(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_ManBookmark)
{
    DoingEvent=false;

    ui->setupUi(this);

    connect(ui->Folder_treeWidget, SIGNAL(Dropped(QTreeWidgetItem *,QTreeWidgetItem *,int)), this, SLOT(FolderDropped(QTreeWidgetItem *,QTreeWidgetItem *,int)));
}

Form_ManBookmark::~Form_ManBookmark()
{
    delete ui;
}

void Form_ManBookmark::on_Rename_pushButton_clicked()
{
    union DMBEventData EventData;

    EventData.Bttn.BttnID=e_UIDMB_Button_Rename;
    SendEvent(e_DMBEvent_BttnTriggered,&EventData);
}

void Form_ManBookmark::on_Delete_pushButton_clicked()
{
    union DMBEventData EventData;

    EventData.Bttn.BttnID=e_UIDMB_Button_Delete;
    SendEvent(e_DMBEvent_BttnTriggered,&EventData);
}

void Form_ManBookmark::on_Folder_treeWidget_itemSelectionChanged()
{
    union DMBEventData EventData;
    QList<QTreeWidgetItem *>SelectedItems;

    SelectedItems=ui->Folder_treeWidget->selectedItems();
    if(SelectedItems.empty())
        EventData.Item.NewItem=NULL;
    else
        EventData.Item.NewItem=(t_UITreeItem *)ui->Folder_treeWidget->currentItem();
    SendEvent(e_DMBEvent_SelectedFolderItemChanged,&EventData);
}

void Form_ManBookmark::FolderDropped(QTreeWidgetItem *ItemDragged,QTreeWidgetItem *DroppedOn,int DropRelPos)
{
    union DMBEventData EventData;

    EventData.Drag.ItemDragged=(t_UITreeItem *)ItemDragged;
    EventData.Drag.DroppedOn=(t_UITreeItem *)DroppedOn;
    EventData.Drag.DropPlacement=DropRelPos;
    SendEvent(e_DMBEvent_FolderDragAndDropped,&EventData);
}

void Form_ManBookmark::on_BookmarkName_lineEdit_textChanged(const QString &arg1)
{
    union DMBEventData EventData;

    EventData.StringChange.NewText=qPrintable(arg1);
    SendEvent(e_DMBEvent_BookmarkNameChanged,&EventData);
}

void Form_ManBookmark::on_URI_lineEdit_editingFinished()
{
    union DMBEventData EventData;

    EventData.StringChange.NewText=qPrintable(ui->URI_lineEdit->text());
    SendEvent(e_DMBEvent_URITextEditFinished,&EventData);
}

/*******************************************************************************
 * NAME:
 *    Form_ManBookmark::SendEvent()
 *
 * SYNOPSIS:
 *    bool Form_ManBookmark::SendEvent(e_DMBEventType EventType,
 *          union DMBEventData *Info);
 *
 * PARAMETERS:
 *    EventType [I] -- The event to send (see UIManBookmark.h)
 *    Info [I] -- Extra info to send with this event.  Can be NULL for none.
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
 *    However there are some that you want to cancel the next part of the
 *    event (close main window for example).
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Form_ManBookmark::SendEvent(e_DMBEventType EventType,
        union DMBEventData *Info,uintptr_t ID)
{
    struct DMBEvent NewEvent;
    bool RetValue;

    /* Stop changing a control programmatic from making events */
    if(DoingEvent)
        return true;

    NewEvent.EventType=EventType;
    NewEvent.ID=ID;
    if(Info!=NULL)
        NewEvent.Info=*Info;

    DoingEvent=true;
    RetValue=DMB_Event(&NewEvent);
    DoingEvent=false;

    return RetValue;
}
bool Form_ManBookmark::SendEvent(e_DMBEventType EventType,union DMBEventData *Info)
{
    return SendEvent(EventType,Info,0);
}
bool Form_ManBookmark::SendEvent(e_DMBEventType EventType)
{
    return SendEvent(EventType,NULL,0);
}

void Form_ManBookmark::on_Options_pushButton_clicked()
{
    union DMBEventData EventData;

    EventData.Bttn.BttnID=e_UIDMB_Button_Options;
    SendEvent(e_DMBEvent_BttnTriggered,&EventData);
}


void Form_ManBookmark::on_Settings_pushButton_clicked()
{
    union DMBEventData EventData;

    EventData.Bttn.BttnID=e_UIDMB_Button_Settings;
    SendEvent(e_DMBEvent_BttnTriggered,&EventData);
}

void Form_ManBookmark::on_GlobalSettings_checkBox_clicked()
{
    union DMBEventData Info;
    Info.Checkbox.BoxID=e_UIDMB_Checkbox_UseGlobalSettings;
    SendEvent(e_DMBEvent_CheckboxChange,&Info);
}

