#ifndef FORM_DEBUGPRINTLOG_H
#define FORM_DEBUGPRINTLOG_H

#include <QMainWindow>

namespace Ui {
class Form_DebugPrintLog;
}

class Form_DebugPrintLog : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Form_DebugPrintLog(QWidget *parent = nullptr);
    ~Form_DebugPrintLog();
    void AddText(const char *text);
    void ClearText(void);
    
private slots:
    void on_clear_pushButton_clicked();
    
    void on_buttonBox_accepted();
    
private:
    Ui::Form_DebugPrintLog *ui;
};

#endif // FORM_DEBUGPRINTLOG_H
