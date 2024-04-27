#ifndef FORM_SETTINGS_H
#define FORM_SETTINGS_H

#include <QDialog>
#include <QListWidgetItem>
#include "UI/UISettings.h"

namespace Ui {
class Form_Settings;
}

class Form_Settings : public QDialog
{
    Q_OBJECT
    
public:
    explicit Form_Settings(QWidget *parent = 0);
    ~Form_Settings();
    Ui::Form_Settings *ui;

private slots:
    void on_LeftPanel_AutoHide_checkBox_clicked(bool checked);
    void on_RightPanel_AutoHide_checkBox_clicked(bool checked);
    void on_BottomPanel_AutoHide_checkBox_clicked(bool checked);
    void on_LeftPanel_SaveCurrent_pushButton_clicked();
    void on_RightPanel_SaveCurrent_pushButton_clicked();
    void on_BottomPanel_SaveCurrent_pushButton_clicked();
    void on_Area_listWidget_itemClicked(QListWidgetItem *item);

    void on_GrabCurrentWinPos_pushButton_clicked();

    void on_WindowStartupPos_comboBox_activated(int index);

    void on_FixedWidth_checkBox_clicked(bool checked);

    void on_FixedHeight_checkBox_clicked(bool checked);

    void on_SelectFontPushButton_clicked();
    
    void on_InputProcessingType_comboBox_currentIndexChanged(int index);
    
    void on_CharEnc_comboBox_currentIndexChanged(int index);
    
    void on_TermEmu_comboBox_currentIndexChanged(int index);
    
    void on_TextHigh_listWidget_itemClicked(QListWidgetItem *item);
    
    void on_CharEncSettings_pushButton_clicked();
    
    void on_TermEmuSettings_pushButton_clicked();
    
    void on_TextHighSettings_pushButton_clicked();
    
    void on_Other_listWidget_itemClicked(QListWidgetItem *item);

    void on_OtherSettings_pushButton_clicked();

    void on_SysCol_Preset_comboBox_currentIndexChanged(int index);

    void on_SysCol_Apply_pushButton_clicked();

    void on_SysCol_Normal_radioButton_clicked();

    void on_SysCol_Bright_radioButton_clicked();

    void on_SysCol_Dark_radioButton_clicked();

    void on_SysColPrev_Black_radioButton_clicked();

    void on_SysColPrev_Red_radioButton_clicked();

    void on_SysColPrev_Green_radioButton_clicked();

    void on_SysColPrev_Yellow_radioButton_clicked();

    void on_SysColPrev_Blue_radioButton_clicked();

    void on_SysColPrev_Magenta_radioButton_clicked();

    void on_SysColPrev_Cyan_radioButton_clicked();

    void on_SysColPrev_White_radioButton_clicked();

    void on_SysCol_R_horizontalScrollBar_valueChanged(int value);

    void on_SysCol_G_horizontalScrollBar_valueChanged(int value);

    void on_SysCol_B_horizontalScrollBar_valueChanged(int value);

    void on_SysCol_R_spinBox_valueChanged(int arg1);
    
    void on_SysCol_G_spinBox_valueChanged(int arg1);
    
    void on_SysCol_B_spinBox_valueChanged(int arg1);
    
    void on_SysCol_Web_lineEdit_textEdited(const QString &arg1);
    
    void on_SysColPrev_DefaultFG_radioButton_clicked();
    
    void on_SysColPrev_DefaultBG_radioButton_clicked();
    
    void on_Keyboard_CommandList_listWidget_itemClicked(QListWidgetItem *item);
    
    void on_SelectCursorColor_pushButton_clicked();
    
    void on_Keyboard_CommandList_Set_pushButton_clicked();
    
    void on_Keyboard_CommandList_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    
    void on_pushButton_CaptureSelectFilename_clicked();
    
    void on_HexDisplay_SelectFontPushButton_clicked();
    
    void on_HexDisplay_SelectFGColor_pushButton_clicked();
    
    void on_HexDisplay_SelectBGColor_pushButton_clicked();
    
    void on_HexDisplay_SelectSelBGColor_pushButton_clicked();
    
private:
    bool DoingEvent;

    bool SendEvent(e_DSEventType EventType,union DSEventData *Info,uintptr_t ID);
    bool SendEvent(e_DSEventType EventType,union DSEventData *Info);
    bool SendEvent(e_DSEventType EventType);
};

#endif // FORM_SETTINGS_H
