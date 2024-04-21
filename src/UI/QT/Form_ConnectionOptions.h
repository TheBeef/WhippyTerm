#ifndef FORM_CONNECTIONOPTIONS_H
#define FORM_CONNECTIONOPTIONS_H

#include <QDialog>

namespace Ui {
class Form_ConnectionOptions;
}

class Form_ConnectionOptions : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_ConnectionOptions(QWidget *parent = 0);
    ~Form_ConnectionOptions();
    Ui::Form_ConnectionOptions *ui;
    
private:
    
};

#endif // FORM_CONNECTIONOPTIONS_H
