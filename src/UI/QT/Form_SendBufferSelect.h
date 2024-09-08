#ifndef FORM_SENDBUFFERSELECT_H
#define FORM_SENDBUFFERSELECT_H

#include <QDialog>

#include "UI/UISendBufferSelect.h"

namespace Ui {
class Form_SendBufferSelect;
}

class Form_SendBufferSelect : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_SendBufferSelect(QWidget *parent = nullptr);
    ~Form_SendBufferSelect();
    Ui::Form_SendBufferSelect *ui;
    
private slots:
    void on_pushButton_Cancel_clicked();
    
    void on_pushButton_Go_clicked();
    
    void on_treeWidget_BufferList_itemSelectionChanged();
    
private:
    bool DoingEvent;

    bool SendEvent(e_SBSEventType EventType);
    bool SendEvent(e_SBSEventType EventType,union SBSEventData *Info);
    bool SendEvent(e_SBSEventType EventType,union SBSEventData *Info,uintptr_t ID,bool IgnoreEventsInProgress=false);
};

#endif // FORM_SENDBUFFERSELECT_H
