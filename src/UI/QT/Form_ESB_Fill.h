#ifndef FORM_ESB_FILL_H
#define FORM_ESB_FILL_H

#include <QDialog>

namespace Ui {
class Form_ESB_Fill;
}

class Form_ESB_Fill : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_ESB_Fill(QWidget *parent = 0);
    ~Form_ESB_Fill();
    Ui::Form_ESB_Fill *ui;

private:
    
};

#endif // FORM_ESB_FILL_H
