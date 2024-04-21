#ifndef FORM_INSTALLPLUGIN_H
#define FORM_INSTALLPLUGIN_H

#include <QDialog>
#include "UI/UIInstallPlugin.h"

namespace Ui {
class Form_InstallPlugin;
}

class Form_InstallPlugin : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_InstallPlugin(QWidget *parent = 0);
    ~Form_InstallPlugin();
    Ui::Form_InstallPlugin *ui;
    
private slots:
    void on_buttonBox_accepted();
    
private:
    bool DoingEvent;
    bool SendEvent(e_DIPEventType EventType,union DIPEventData *Info,uintptr_t ID);
    bool SendEvent(e_DIPEventType EventType,union DIPEventData *Info);
    bool SendEvent(e_DIPEventType EventType);
};

#endif // FORM_INSTALLPLUGIN_H
