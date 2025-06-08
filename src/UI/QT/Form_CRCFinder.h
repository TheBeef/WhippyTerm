#ifndef FORM_CRCFINDER_H
#define FORM_CRCFINDER_H

#include "UI/UICRCFinder.h"
#include <QDialog>

namespace Ui {
class Form_CRCFinder;
}

class Form_CRCFinder : public QDialog
{
    Q_OBJECT

public:
    explicit Form_CRCFinder(QWidget *parent = nullptr);
    ~Form_CRCFinder();
    Ui::Form_CRCFinder *ui;
    
private slots:
    void on_FindCRC_pushButton_clicked();
    
    void on_ShowSource_pushButton_clicked();
    
    void on_CRC_lineEdit_editingFinished();
    
    void on_CRCType_comboBox_currentIndexChanged(int index);
    
    void on_buttonBox_accepted();
    
    void on_CRC_lineEdit_textEdited(const QString &arg1);
    
private:
    bool DoingEvent;

    bool SendEvent(e_CFEventType EventType,union CFEventData *Info,uintptr_t ID);
    bool SendEvent(e_CFEventType EventType,union CFEventData *Info);
    bool SendEvent(e_CFEventType EventType);
};

#endif // FORM_CRCFINDER_H
