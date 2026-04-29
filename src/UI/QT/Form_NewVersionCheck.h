#ifndef FORM_NEWVERSIONCHECK_H
#define FORM_NEWVERSIONCHECK_H

#include "UI/UINewVersionCheck.h"
#include <QDialog>
#include <QTimer>

namespace Ui {
class Form_NewVersionCheck;
}

class Form_NewVersionCheck : public QDialog
{
    Q_OBJECT

public:
    explicit Form_NewVersionCheck(QWidget *parent = nullptr);
    ~Form_NewVersionCheck();
    Ui::Form_NewVersionCheck *ui;
    
private slots:
    void on_pushButton_clicked();
    void SimProgressTick_timeout();

private:
    bool DoingEvent;
    QTimer *SimProgressTick;

    bool SendEvent(e_NVCEventType EventType,union NVCEventData *Info,bool IgnoreEventsInProgress);
    bool SendEvent(e_NVCEventType EventType,union NVCEventData *Info);
    bool SendEvent(e_NVCEventType EventType);
};

#endif // FORM_NEWVERSIONCHECK_H
