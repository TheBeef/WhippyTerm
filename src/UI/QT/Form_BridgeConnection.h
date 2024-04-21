#ifndef FORM_BRIDGECONNECTION_H
#define FORM_BRIDGECONNECTION_H

#include "UI/UIBridgeConnections.h"
#include <QDialog>

namespace Ui {
class Form_BridgeConnection;
}

class Form_BridgeConnection : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_BridgeConnection(QWidget *parent = 0);
    ~Form_BridgeConnection();
    Ui::Form_BridgeConnection *ui;
    
private slots:
    void on_comboBox_Connection2_activated(int index);
    
private:
    bool DoingEvent;
    bool SendEvent(e_BCEventType EventType,union BCEventData *Info,uintptr_t ID,bool IgnoreEventsInProgress=false);
    bool SendEvent(e_BCEventType EventType,union BCEventData *Info);
    bool SendEvent(e_BCEventType EventType);
};

#endif // FORM_BRIDGECONNECTION_H
