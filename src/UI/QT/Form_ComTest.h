#ifndef FORM_COMTEST_H
#define FORM_COMTEST_H

#include "UI/UIComTest.h"
#include <QDialog>

namespace Ui {
class Form_ComTest;
}

class Form_ComTest : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_ComTest(QWidget *parent = 0);
    ~Form_ComTest();
    Ui::Form_ComTest *ui;
    
private slots:
    void on_Rescan2_pushButton_clicked();
    
    void on_Rescan1_pushButton_clicked();
    
    void on_Start_pushButton_clicked();
    
    void on_Stop_pushButton_clicked();
    
    void on_ConnectionList1_comboBox_currentIndexChanged(int index);
    
    void on_ConnectionList2_comboBox_currentIndexChanged(int index);
    
    void on_Loopback_checkBox_clicked();
    
    void on_FullDuplex_checkBox_clicked();
    
    void on_Pattern_comboBox_currentIndexChanged(int index);
    
private:
    bool DoingEvent;
    bool SendEvent(e_CTEventType EventType,union CTEventData *Info,uintptr_t ID,bool IgnoreEventsInProgress=false);
    bool SendEvent(e_CTEventType EventType,union CTEventData *Info);
    bool SendEvent(e_CTEventType EventType);
};

#endif // FORM_COMTEST_H
