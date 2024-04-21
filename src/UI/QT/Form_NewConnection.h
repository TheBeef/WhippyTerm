#ifndef FORM_NEWCONNECTION_H
#define FORM_NEWCONNECTION_H

#include <QDialog>
#include "UI/UINewConnection.h"

namespace Ui {
class Form_NewConnection;
}

class Form_NewConnection : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_NewConnection(QWidget *parent = 0);
    ~Form_NewConnection();
    Ui::Form_NewConnection *ui;

private slots:
    void on_pushButton_Rescan_clicked();

    void on_ConnectionList_comboBox_currentIndexChanged(int index);

    void on_Name_lineEdit_editingFinished();
    
private:
    bool DoingEvent;

    bool SendEvent(e_DNCEventType EventType,union DNCEventData *Info,uintptr_t ID);
    bool SendEvent(e_DNCEventType EventType,union DNCEventData *Info);
    bool SendEvent(e_DNCEventType EventType);
};

#endif // FORM_NEWCONNECTION_H
