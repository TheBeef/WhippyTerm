#ifndef FORM_ADDBOOKMARK_H
#define FORM_ADDBOOKMARK_H

#include "UI/UIAddBookmark.h"
#include <QDialog>
#include <stdint.h>

namespace Ui {
class Form_AddBookmark;
}

class Form_AddBookmark : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_AddBookmark(QWidget *parent = 0);
    ~Form_AddBookmark();
    Ui::Form_AddBookmark *ui;
    
private slots:
    void on_New_pushButton_clicked();
    
private:
    bool SendEvent(e_ABDEventType EventType,union ABDEventData *Info,uintptr_t ID);
    bool SendEvent(e_ABDEventType EventType,union ABDEventData *Info);
    bool SendEvent(e_ABDEventType EventType);
};

#endif // FORM_ADDBOOKMARK_H
