#ifndef FORM_MANAGEPLUGINS_H
#define FORM_MANAGEPLUGINS_H

#include "UI/UIManagePlugins.h"
#include <QDialog>
#include <QListWidgetItem>


namespace Ui {
class Form_ManagePlugins;
}

class Form_ManagePlugins : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_ManagePlugins(QWidget *parent = 0);
    ~Form_ManagePlugins();
    
    Ui::Form_ManagePlugins *ui;

private slots:
    void on_Enable_pushButton_clicked();
    
    void on_Install_pushButton_clicked();
    
    void on_Uninstall_pushButton_clicked();
    
    void on_buttonBox_accepted();
    
    void on_PluginList_listWidget_currentRowChanged(int currentRow);
    
    void on_Upgrade_pushButton_clicked();
    
private:
    bool DoingEvent;
    bool SendEvent(e_DMPEventType EventType,union DMPEventData *Info,uintptr_t ID);
    bool SendEvent(e_DMPEventType EventType,union DMPEventData *Info);
    bool SendEvent(e_DMPEventType EventType);
};

#endif // FORM_MANAGEPLUGINS_H
