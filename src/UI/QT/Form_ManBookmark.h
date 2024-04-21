#ifndef FORM_MANBOOKMARK_H
#define FORM_MANBOOKMARK_H

#include "UI/UIManBookmark.h"
#include <QDialog>
#include <QTreeWidgetItem>
#include <QTreeWidget>
#include <stdint.h>

namespace Ui {
class Form_ManBookmark;
}

class MB_DragItems_QTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    MB_DragItems_QTreeWidget(QWidget *parent = 0);
    QTreeWidgetItem *draggedItem;

signals:
    void Dropped(QTreeWidgetItem *ItemDragged,QTreeWidgetItem *DroppedOn,int DropRelPos);

protected:
    void dropEvent(QDropEvent * event);
    void dragEnterEvent(QDragEnterEvent *event);
};

class Form_ManBookmark : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_ManBookmark(QWidget *parent = 0);
    ~Form_ManBookmark();
    Ui::Form_ManBookmark *ui;
    
private slots:
    void FolderDropped(QTreeWidgetItem *ItemDragged,QTreeWidgetItem *DroppedOn,int DropRelPos);
    
    void on_Rename_pushButton_clicked();
    
    void on_Delete_pushButton_clicked();
    
    void on_Folder_treeWidget_itemSelectionChanged();
    
    void on_BookmarkName_lineEdit_textChanged(const QString &arg1);
    
    void on_URI_lineEdit_editingFinished();
    
private:
    bool DoingEvent;
    bool SendEvent(e_DMBEventType EventType,union DMBEventData *Info,uintptr_t ID);
    bool SendEvent(e_DMBEventType EventType,union DMBEventData *Info);
    bool SendEvent(e_DMBEventType EventType);
};

#endif // FORM_MANBOOKMARK_H
