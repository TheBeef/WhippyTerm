#ifndef FORM_HEXDISPLAYCOPYAS_H
#define FORM_HEXDISPLAYCOPYAS_H

#include <QDialog>

namespace Ui {
class Form_HexDisplayCopyAs;
}

class Form_HexDisplayCopyAs : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_HexDisplayCopyAs(QWidget *parent = 0);
    ~Form_HexDisplayCopyAs();
    Ui::Form_HexDisplayCopyAs *ui;

private:
    
};

#endif // FORM_HEXDISPLAYCOPYAS_H
