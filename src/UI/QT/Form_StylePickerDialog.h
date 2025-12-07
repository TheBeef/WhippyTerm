#ifndef FORM_STYLEPICKERDIALOG_H
#define FORM_STYLEPICKERDIALOG_H

#include "PluginSDK/DataProcessors.h"
#include <QDialog>

namespace Ui {
class Form_StylePickerDialog;
}

class Form_StylePickerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit Form_StylePickerDialog(QWidget *parent = nullptr);
    ~Form_StylePickerDialog();
    Ui::Form_StylePickerDialog *ui;
    struct StyleData CurStyle;
    void UpdateDisplay(void);
    void CollectStylesFromGUI(void);

private slots:
    void on_FG_Select_pushButton_clicked();
    
    void on_BG_Select_pushButton_clicked();
    
    void on_ULine_Select_pushButton_clicked();
    
private:
};

#endif // FORM_STYLEPICKERDIALOG_H
