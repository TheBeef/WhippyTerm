#include "Form_SettingsSelectType.h"
#include "ui_Form_SettingsSelectType.h"
#include "UI/UISettingsSelectEditType.h"

extern e_USSET_UserSelectedType g_SettingsSelectTypeBttnSelected;

Form_SettingsSelectType::Form_SettingsSelectType(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Form_SettingsSelectType)
{
    ui->setupUi(this);
}

Form_SettingsSelectType::~Form_SettingsSelectType()
{
    delete ui;
}

void Form_SettingsSelectType::on_GlobalSettings_pushButton_clicked()
{
    g_SettingsSelectTypeBttnSelected=e_USSET_UserSelected_Global;
    close();
}

void Form_SettingsSelectType::on_ConSettings_pushButton_clicked()
{
    g_SettingsSelectTypeBttnSelected=e_USSET_UserSelected_Connection;
    close();
}

void Form_SettingsSelectType::on_Cancel_pushButton_clicked()
{
    g_SettingsSelectTypeBttnSelected=e_USSET_UserSelected_Cancel;
    close();
}

