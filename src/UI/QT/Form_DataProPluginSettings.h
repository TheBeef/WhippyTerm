#ifndef FORM_DATAPROPLUGINSETTINGS_H
#define FORM_DATAPROPLUGINSETTINGS_H

#include <QDialog>

namespace Ui {
class Form_DataProPluginSettings;
}

class Form_DataProPluginSettings : public QDialog
{
    Q_OBJECT

public:
    explicit Form_DataProPluginSettings(QWidget *parent = nullptr);
    ~Form_DataProPluginSettings();
    Ui::Form_DataProPluginSettings *ui;

private:
};

#endif // FORM_DATAPROPLUGINSETTINGS_H
