#ifndef FORM_ESB_INSERTNUMBER_H
#define FORM_ESB_INSERTNUMBER_H

#include <QDialog>

namespace Ui {
class Form_ESB_InsertProp;
}

class Form_ESB_InsertProp : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_ESB_InsertProp(QWidget *parent = 0);
    ~Form_ESB_InsertProp();
    Ui::Form_ESB_InsertProp *ui;

private:
    
};

#endif // FORM_ESB_INSERTNUMBER_H
