#ifndef FORM_SETTINGSSELECTTYPE_H
#define FORM_SETTINGSSELECTTYPE_H

#include <QDialog>

namespace Ui {
class Form_SettingsSelectType;
}

class Form_SettingsSelectType : public QDialog
{
    Q_OBJECT

public:
    explicit Form_SettingsSelectType(QWidget *parent = nullptr);
    ~Form_SettingsSelectType();
    
private slots:
    void on_GlobalSettings_pushButton_clicked();
    
    void on_ConSettings_pushButton_clicked();
    
    void on_Cancel_pushButton_clicked();
    
private:
    Ui::Form_SettingsSelectType *ui;
};

#endif // FORM_SETTINGSSELECTTYPE_H
