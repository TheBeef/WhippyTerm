#ifndef FORM_EDITSENDBUFFER_H
#define FORM_EDITSENDBUFFER_H

#include "UI/UIEditSendBuffer.h"
#include <QDialog>
#include <QMenu>
#include <QAction>

namespace Ui {
class Form_EditSendBuffer;
}

class Form_EditSendBuffer : public QDialog
{
    Q_OBJECT

    friend t_UIContextMenuCtrl *UIESB_GetContextMenu(e_ESB_ContextMenu UIObj);

public:
    explicit Form_EditSendBuffer(QWidget *parent = 0);
    ~Form_EditSendBuffer();
    Ui::Form_EditSendBuffer *ui;

private slots:
    void on_Fill_pushButton_clicked();
    
    void on_EndianSwap_pushButton_clicked();
    
    void on_CRCType_pushButton_clicked();
    
    void on_InsertCRC_pushButton_clicked();
    
    void on_InsertAsNumber_pushButton_clicked();
    
    void on_InsertAsText_pushButton_clicked();
    
    void on_InsertProperties_pushButton_clicked();
    
    void on_BufferSize_lineEdit_editingFinished();
    
    void on_More_pushButton_customContextMenuRequested(const QPoint &pos);
    
    void on_More_pushButton_clicked();

    void MoreMenu_InsertFromDisk_clicked();
    void MoreMenu_InsertFromClipboard_clicked();

    void MoreMenu_LoadBuffer_clicked();
    void MoreMenu_SaveBuffer_clicked();
    void MoreMenu_ImportData_clicked();
    void MoreMenu_ExportData_clicked();

    void on_Clear_pushButton_clicked();
    
private:
    bool DoingEvent;
    bool SendEvent(e_ESBEventType EventType,union ESBEventData *Info,uintptr_t ID);
    bool SendEvent(e_ESBEventType EventType,union ESBEventData *Info);
    bool SendEvent(e_ESBEventType EventType);
    void SendBttnEvent(e_ESB_Button Bttn);
    void SendContextEvent(e_ESB_ContextMenu Menu);

    QMenu *MoreMenu;
    QAction *LoadBufferMenu;
    QAction *SaveBufferMenu;
    QAction *ImportDataMenu;
    QAction *ExportDataMenu;
    QAction *InsertFromDiskMenu;
    QAction *InsertFromClipboardMenu;
};

#endif // FORM_EDITSENDBUFFER_H
