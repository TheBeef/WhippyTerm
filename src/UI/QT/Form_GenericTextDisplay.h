#ifndef FORM_GENERICTEXTDISPLAY_H
#define FORM_GENERICTEXTDISPLAY_H

#include <QDialog>
#include <QString>

namespace Ui {
class Form_GenericTextDisplay;
}

class Form_GenericTextDisplay : public QDialog
{
    Q_OBJECT

public:
    explicit Form_GenericTextDisplay(QWidget *parent = nullptr);
    ~Form_GenericTextDisplay();
    QString RawHTML; // We have to track this because we can't really get it back out of the widget
    Ui::Form_GenericTextDisplay *ui;

private:
};

#endif // FORM_GENERICTEXTDISPLAY_H
