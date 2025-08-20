#ifndef FORM_CALCCRC_H
#define FORM_CALCCRC_H

#include "UI/UICalcCrc.h"
#include <QDialog>
#include <QMenu>
#include <QAction>

namespace Ui {
class Form_CalcCrc;
}

class Form_CalcCrc : public QDialog
{
    Q_OBJECT

public:
    explicit Form_CalcCrc(QWidget *parent = 0);
    ~Form_CalcCrc();
    Ui::Form_CalcCrc *ui;

private slots:
    void on_CalcCRC_pushButton_clicked();
    
    void on_ShowSource_pushButton_clicked();
    
    void on_CRCType_comboBox_currentIndexChanged(int index);

private:
    bool DoingEvent;
    bool SendEvent(e_CCRCEventType EventType,union CCRCEventData *Info,uintptr_t ID);
    bool SendEvent(e_CCRCEventType EventType,union CCRCEventData *Info);
    bool SendEvent(e_CCRCEventType EventType);
    void SendBttnEvent(e_CCRC_Button Bttn);
};

#endif // FORM_CALCCRC_H
