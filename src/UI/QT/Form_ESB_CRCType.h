#ifndef FORM_ESB_CRCTYPE_H
#define FORM_ESB_CRCTYPE_H

#include <QDialog>
#include "UI/UI_ESB_CRCType.h"

namespace Ui {
class Form_ESB_CRCType;
}

class Form_ESB_CRCType : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_ESB_CRCType(QWidget *parent = 0);
    ~Form_ESB_CRCType();
    Ui::Form_ESB_CRCType *ui;
private slots:
    void on_ShowSource_pushButton_clicked();

private:
    bool SendEvent(e_ESBCTEventType EventType,union ESBCTEventData *Info,uintptr_t ID);
    bool SendEvent(e_ESBCTEventType EventType,union ESBCTEventData *Info);
    bool SendEvent(e_ESBCTEventType EventType);
    void SendBttnEvent(e_ESBCT_Button Bttn);

    bool DoingEvent;

};

#endif // FORM_ESB_CRCTYPE_H
