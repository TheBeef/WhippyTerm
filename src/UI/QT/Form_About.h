#ifndef FORM_ABOUT_H
#define FORM_ABOUT_H

#include <QDialog>

namespace Ui {
class Form_About;
}

class Form_About : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_About(QWidget *parent = 0);
    ~Form_About();
    Ui::Form_About *ui;
    
private slots:

private:
};

#endif // FORM_ABOUT_H
