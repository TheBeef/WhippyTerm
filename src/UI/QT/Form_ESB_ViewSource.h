#ifndef FORM_ESB_VIEWSOURCE_H
#define FORM_ESB_VIEWSOURCE_H

#include <QDialog>

namespace Ui {
class Form_ESB_ViewSource;
}

class Form_ESB_ViewSource : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_ESB_ViewSource(QWidget *parent = 0);
    ~Form_ESB_ViewSource();
    Ui::Form_ESB_ViewSource *ui;
    
private:
};

#endif // FORM_ESB_VIEWSOURCE_H
