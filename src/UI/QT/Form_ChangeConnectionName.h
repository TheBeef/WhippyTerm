#ifndef FORM_CHANGECONNECTIONNAME_H
#define FORM_CHANGECONNECTIONNAME_H

#include <QDialog>

namespace Ui {
class Form_ChangeConnectionName;
}

class Form_ChangeConnectionName : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_ChangeConnectionName(QWidget *parent = 0);
    ~Form_ChangeConnectionName();
    Ui::Form_ChangeConnectionName *ui;

private:

};

#endif // FORM_CHANGECONNECTIONNAME_H
