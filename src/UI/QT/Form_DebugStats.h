#ifndef FORM_DEBUGSTATS_H
#define FORM_DEBUGSTATS_H

#include <QDialog>
#include <QLabel>
#include <stdint.h>
#include "UI/UIDebug.h"

namespace Ui {
class Form_DebugStats;
}

class Form_DebugStats : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_DebugStats(QWidget *parent = 0);
    ~Form_DebugStats();
    void UpdateTime(e_DBTType item,QLabel *label);

private slots:
    void on_Refresh_pushButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::Form_DebugStats *ui;
    QLabel *Labels[e_DBTMAX];
    QLabel *DataValues[e_DBTMAX];
};

#endif // FORM_DEBUGSTATS_H
