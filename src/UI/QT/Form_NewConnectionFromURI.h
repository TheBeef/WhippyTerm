#ifndef FORM_NEWCONNECTIONFROMURI_H
#define FORM_NEWCONNECTIONFROMURI_H
#include "UI/UINewConnectionFromURI.h"

#include <QDialog>

namespace Ui {
class Form_NewConnectionFromURI;
}

class Form_NewConnectionFromURI : public QDialog
{
    Q_OBJECT

    friend t_UITextInputCtrl *UINC_GetTxtInputHandle(e_UINCFU_TxtInput UIObj);
    friend t_UIListViewCtrl *UINC_GetListViewInputHandle(e_UINCFU_ListViewType UIObj);
    friend t_UIHTMLViewCtrl *UINC_GetHTMLViewInputHandle(e_UINCFU_HTMLViewType UIObj);
    friend t_UIButtonCtrl *UINC_GetButtonInputHandle(e_UINCFU_ButtonInput UIObj);
    friend void UINC_PrivURIHelp_AppendText(const char *NewLine,e_PrivURIHelp_StyleType Style);
    friend void UINC_PrivURIHelp_ClearCtrlText(void);

public:
    explicit Form_NewConnectionFromURI(QWidget *parent = nullptr);
    ~Form_NewConnectionFromURI();
    
private slots:
    void on_DriverList_listWidget_itemSelectionChanged();
    
    void on_URI_lineEdit_textEdited(const QString &arg1);
    
private:
    Ui::Form_NewConnectionFromURI *ui;
    QString PrivURIRawHTML; // We have to track this because we can't really get it back out of the widget
    bool DoingEvent;
    bool SendEvent(e_DNCFUEventType EventType,union DNCFUEventData *Info,uintptr_t ID);
    bool SendEvent(e_DNCFUEventType EventType,union DNCFUEventData *Info);
    bool SendEvent(e_DNCFUEventType EventType);
};

#endif // FORM_NEWCONNECTIONFROMURI_H
