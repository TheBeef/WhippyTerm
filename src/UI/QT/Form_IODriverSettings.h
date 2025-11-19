#ifndef FORM_IODRIVERSETTINGS_H
#define FORM_IODRIVERSETTINGS_H

#include <QDialog>

namespace Ui {
class Form_IODriverSettings;
}

class Form_IODriverSettings : public QDialog
{
    Q_OBJECT

public:
    explicit Form_IODriverSettings(QWidget *parent = nullptr);
    ~Form_IODriverSettings();
    Ui::Form_IODriverSettings *ui;

private:
};

#endif // FORM_IODRIVERSETTINGS_H
