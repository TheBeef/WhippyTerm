#ifndef FORM_TRANSMITDELAY_H
#define FORM_TRANSMITDELAY_H

#include <QDialog>

namespace Ui {
class Form_TransmitDelay;
}

class Form_TransmitDelay : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_TransmitDelay(QWidget *parent = 0);
    ~Form_TransmitDelay();
    Ui::Form_TransmitDelay *ui;

private:
};

#endif // FORM_TRANSMITDELAY_H
