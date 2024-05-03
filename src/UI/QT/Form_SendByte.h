#ifndef FORM_SENDBYTE_H
#define FORM_SENDBYTE_H

#include "UI/UISendByte.h"
#include <QDialog>

namespace Ui {
class Form_SendByte;
}

class Form_SendByte : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_SendByte(QWidget *parent = nullptr);
    ~Form_SendByte();
    Ui::Form_SendByte *ui;
    

private:
    bool SendEvent(e_SBDEventType EventType,union SBDEventData *Info,uintptr_t ID);
    bool SendEvent(e_SBDEventType EventType,union SBDEventData *Info);
    bool SendEvent(e_SBDEventType EventType);
    bool DoingEvent;

private slots:
    void on_AscII_tableWidget_doubleClicked(const QModelIndex &index);
    
    void on_AscII_tableWidget_itemSelectionChanged();
    
    void on_Dec_lineEdit_editingFinished();
    
    void on_Hex_lineEdit_editingFinished();
    
    void on_Oct_lineEdit_editingFinished();
    
private:
};

#endif // FORM_SENDBYTE_H
