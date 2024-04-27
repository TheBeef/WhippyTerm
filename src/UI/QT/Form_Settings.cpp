#include "Form_Settings.h"
#include "ui_Form_Settings.h"
#include "UI/UISettings.h"
#include <QMovie>

extern class Form_Settings *g_SettingsDialog;

Form_Settings::Form_Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Form_Settings)
{
    DoingEvent=false;
    ui->setupUi(this);
}

Form_Settings::~Form_Settings()
{
    delete ui;
}

/*******************************************************************************
 * NAME:
 *    Form_Settings::SendEvent()
 *
 * SYNOPSIS:
 *    bool Form_Settings::SendEvent(e_DSEventType EventType,
 *          union DSEventData *Info);
 *
 * PARAMETERS:
 *    EventType [I] -- The event to send (see UISettings.h)
 *    Info [I] -- Extra info to send with this event.  Can be NULL for none.
 *
 * FUNCTION:
 *    This function sends a man bookmark event out of the UI system to the main
 *    code.
 *
 * RETURNS:
 *    true -- Accept this event
 *    false -- Cancel this event
 *
 * NOTES:
 *    The return value is normally ignored as most events have no other action.
 *    However there are some that you want to cancel the next part of the
 *    event (close main window for example).
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Form_Settings::SendEvent(e_DSEventType EventType,
        union DSEventData *Info,uintptr_t ID)
{
    struct DSEvent NewEvent;
    bool RetValue;

    /* Stop changing a control programmatic from making events */
    if(DoingEvent)
        return true;

    NewEvent.EventType=EventType;
    NewEvent.ID=ID;
    if(Info!=NULL)
        NewEvent.Info=*Info;

    DoingEvent=true;
    RetValue=DS_Event(&NewEvent);
    DoingEvent=false;

    return RetValue;
}
bool Form_Settings::SendEvent(e_DSEventType EventType,union DSEventData *Info)
{
    return SendEvent(EventType,Info,0);
}
bool Form_Settings::SendEvent(e_DSEventType EventType)
{
    return SendEvent(EventType,NULL,0);
}

void Form_Settings::on_LeftPanel_AutoHide_checkBox_clicked(bool checked)
{
    union DSEventData EventData;

    EventData.Checkbox.InputID=e_UIS_Checkbox_LeftPanel_AutoHidePanel;
    EventData.Checkbox.Checked=checked;
    SendEvent(e_DSEvent_CheckboxClick,&EventData);
}

void Form_Settings::on_RightPanel_AutoHide_checkBox_clicked(bool checked)
{
    union DSEventData EventData;

    EventData.Checkbox.InputID=e_UIS_Checkbox_RightPanel_AutoHidePanel;
    EventData.Checkbox.Checked=checked;
    SendEvent(e_DSEvent_CheckboxClick,&EventData);
}

void Form_Settings::on_BottomPanel_AutoHide_checkBox_clicked(bool checked)
{
    union DSEventData EventData;

    EventData.Checkbox.InputID=e_UIS_Checkbox_BottomPanel_AutoHidePanel;
    EventData.Checkbox.Checked=checked;
    SendEvent(e_DSEvent_CheckboxClick,&EventData);
}

void Form_Settings::on_LeftPanel_SaveCurrent_pushButton_clicked()
{
    union DSEventData EventData;

    EventData.Bttn.InputID=e_UIS_Button_LeftPanel_GrabLayout;
    SendEvent(e_DSEvent_BttnTriggered,&EventData);
}

void Form_Settings::on_RightPanel_SaveCurrent_pushButton_clicked()
{
    union DSEventData EventData;

    EventData.Bttn.InputID=e_UIS_Button_RightPanel_GrabLayout;
    SendEvent(e_DSEvent_BttnTriggered,&EventData);
}

void Form_Settings::on_BottomPanel_SaveCurrent_pushButton_clicked()
{
    union DSEventData EventData;

    EventData.Bttn.InputID=e_UIS_Button_BottomPanel_GrabLayout;
    SendEvent(e_DSEvent_BttnTriggered,&EventData);
}

void Form_Settings::on_Area_listWidget_itemClicked(QListWidgetItem *item)
{
    uintptr_t ID;   // The ID for this item
    union DSEventData EventData;

    ID=(uintptr_t)(item->data(Qt::UserRole).toULongLong());

    EventData.ListView.InputID=e_UIS_ListView_AreaList;
    SendEvent(e_DSEvent_ListViewChange,&EventData,ID);
}

void Form_Settings::on_GrabCurrentWinPos_pushButton_clicked()
{
    union DSEventData EventData;

    EventData.Bttn.InputID=e_UIS_Button_GrabCurrentWinPos;
    SendEvent(e_DSEvent_BttnTriggered,&EventData);
}

void Form_Settings::on_WindowStartupPos_comboBox_activated(int index)
{
    uintptr_t ID;   // The ID for this item
    union DSEventData EventData;

    ID=ui->WindowStartupPos_comboBox->itemData(index).toULongLong();

    EventData.ComboBox.InputID=e_UIS_ComboBox_WindowStartupPos;
    EventData.ComboBox.Index=index;
    SendEvent(e_DSEvent_ComboBoxIndexChange,&EventData,ID);
}

void Form_Settings::on_FixedWidth_checkBox_clicked(bool checked)
{
    union DSEventData EventData;

    EventData.Checkbox.InputID=e_UIS_Checkbox_TerminalSize_FixedWidth;
    EventData.Checkbox.Checked=checked;
    SendEvent(e_DSEvent_CheckboxClick,&EventData);
}

void Form_Settings::on_FixedHeight_checkBox_clicked(bool checked)
{
    union DSEventData EventData;

    EventData.Checkbox.InputID=e_UIS_Checkbox_TerminalSize_FixedHeight;
    EventData.Checkbox.Checked=checked;
    SendEvent(e_DSEvent_CheckboxClick,&EventData);
}

void Form_Settings::on_SelectFontPushButton_clicked()
{
    union DSEventData EventData;

    EventData.Bttn.InputID=e_UIS_Button_SelectFont;
    SendEvent(e_DSEvent_BttnTriggered,&EventData);
}

void Form_Settings::on_InputProcessingType_comboBox_currentIndexChanged(int index)
{
    uintptr_t ID;   // The ID for this item
    union DSEventData EventData;

    ID=ui->InputProcessingType_comboBox->itemData(index).toULongLong();

    EventData.ComboBox.InputID=e_UIS_ComboBox_DataProcessor;
    EventData.ComboBox.Index=index;
    SendEvent(e_DSEvent_ComboBoxIndexChange,&EventData,ID);
}

void Form_Settings::on_CharEnc_comboBox_currentIndexChanged(int index)
{
    uintptr_t ID;   // The ID for this item
    union DSEventData EventData;

    ID=ui->CharEnc_comboBox->itemData(index).toULongLong();

    EventData.ComboBox.InputID=e_UIS_ComboBox_TextProCharEnc;
    EventData.ComboBox.Index=index;
    SendEvent(e_DSEvent_ComboBoxIndexChange,&EventData,ID);
}

void Form_Settings::on_TermEmu_comboBox_currentIndexChanged(int index)
{
    uintptr_t ID;   // The ID for this item
    union DSEventData EventData;

    ID=ui->TermEmu_comboBox->itemData(index).toULongLong();

    EventData.ComboBox.InputID=e_UIS_ComboBox_TextProTermEmu;
    EventData.ComboBox.Index=index;
    SendEvent(e_DSEvent_ComboBoxIndexChange,&EventData,ID);
}

void Form_Settings::on_TextHigh_listWidget_itemClicked(QListWidgetItem *item)
{
    uintptr_t ID;   // The ID for this item
    union DSEventData EventData;

    ID=(uintptr_t)(item->data(Qt::UserRole).toULongLong());

    EventData.ListView.InputID=e_UIS_ListView_InputProTextHighlight;
    SendEvent(e_DSEvent_ListViewChange,&EventData,ID);
}

void Form_Settings::on_CharEncSettings_pushButton_clicked()
{
    union DSEventData EventData;

    EventData.Bttn.InputID=e_UIS_Button_InputProCharEnc_Settings;
    SendEvent(e_DSEvent_BttnTriggered,&EventData);
}

void Form_Settings::on_TermEmuSettings_pushButton_clicked()
{
    union DSEventData EventData;

    EventData.Bttn.InputID=e_UIS_Button_InputProTermEmu_Settings;
    SendEvent(e_DSEvent_BttnTriggered,&EventData);
}

void Form_Settings::on_TextHighSettings_pushButton_clicked()
{
    union DSEventData EventData;

    EventData.Bttn.InputID=e_UIS_Button_InputProHighLighting_Settings;
    SendEvent(e_DSEvent_BttnTriggered,&EventData);
}

void Form_Settings::on_Other_listWidget_itemClicked(QListWidgetItem *item)
{
    uintptr_t ID;   // The ID for this item
    union DSEventData EventData;

    ID=(uintptr_t)(item->data(Qt::UserRole).toULongLong());

    EventData.ListView.InputID=e_UIS_ListView_InputProTextOther;
    SendEvent(e_DSEvent_ListViewChange,&EventData,ID);
}

void Form_Settings::on_OtherSettings_pushButton_clicked()
{
    union DSEventData EventData;

    EventData.Bttn.InputID=e_UIS_Button_InputProOther_Settings;
    SendEvent(e_DSEvent_BttnTriggered,&EventData);
}

void Form_Settings::on_SysCol_Preset_comboBox_currentIndexChanged(int index)
{
    uintptr_t ID;   // The ID for this item
    union DSEventData EventData;

    ID=ui->SysCol_Preset_comboBox->itemData(index).toULongLong();

    EventData.ComboBox.InputID=e_UIS_ComboBox_SysCol_Preset;
    EventData.ComboBox.Index=index;
    SendEvent(e_DSEvent_ComboBoxIndexChange,&EventData,ID);
}

void Form_Settings::on_SysCol_Apply_pushButton_clicked()
{
    union DSEventData EventData;

    EventData.Bttn.InputID=e_UIS_Button_SysCol_Apply;
    SendEvent(e_DSEvent_BttnTriggered,&EventData);
}

void Form_Settings::on_SysCol_Normal_radioButton_clicked()
{
    union DSEventData EventData;

    EventData.RadioBttn.InputID=e_UIS_RadioBttn_SysCol_Shade_Normal;
    SendEvent(e_DSEvent_RadioBttnClick,&EventData);
}

void Form_Settings::on_SysCol_Bright_radioButton_clicked()
{
    union DSEventData EventData;

    EventData.RadioBttn.InputID=e_UIS_RadioBttn_SysCol_Shade_Bright;
    SendEvent(e_DSEvent_RadioBttnClick,&EventData);
}

void Form_Settings::on_SysCol_Dark_radioButton_clicked()
{
    union DSEventData EventData;

    EventData.RadioBttn.InputID=e_UIS_RadioBttn_SysCol_Shade_Dark;
    SendEvent(e_DSEvent_RadioBttnClick,&EventData);
}

void Form_Settings::on_SysColPrev_Black_radioButton_clicked()
{
    union DSEventData EventData;

    EventData.RadioBttn.InputID=e_UIS_RadioBttn_SysColPrev_Black;
    SendEvent(e_DSEvent_RadioBttnClick,&EventData);
}

void Form_Settings::on_SysColPrev_Red_radioButton_clicked()
{
    union DSEventData EventData;

    EventData.RadioBttn.InputID=e_UIS_RadioBttn_SysColPrev_Red;
    SendEvent(e_DSEvent_RadioBttnClick,&EventData);
}

void Form_Settings::on_SysColPrev_Green_radioButton_clicked()
{
    union DSEventData EventData;

    EventData.RadioBttn.InputID=e_UIS_RadioBttn_SysColPrev_Green;
    SendEvent(e_DSEvent_RadioBttnClick,&EventData);
}

void Form_Settings::on_SysColPrev_Yellow_radioButton_clicked()
{
    union DSEventData EventData;

    EventData.RadioBttn.InputID=e_UIS_RadioBttn_SysColPrev_Yellow;
    SendEvent(e_DSEvent_RadioBttnClick,&EventData);
}

void Form_Settings::on_SysColPrev_Blue_radioButton_clicked()
{
    union DSEventData EventData;

    EventData.RadioBttn.InputID=e_UIS_RadioBttn_SysColPrev_Blue;
    SendEvent(e_DSEvent_RadioBttnClick,&EventData);
}

void Form_Settings::on_SysColPrev_Magenta_radioButton_clicked()
{
    union DSEventData EventData;

    EventData.RadioBttn.InputID=e_UIS_RadioBttn_SysColPrev_Magenta;
    SendEvent(e_DSEvent_RadioBttnClick,&EventData);
}

void Form_Settings::on_SysColPrev_Cyan_radioButton_clicked()
{
    union DSEventData EventData;

    EventData.RadioBttn.InputID=e_UIS_RadioBttn_SysColPrev_Cyan;
    SendEvent(e_DSEvent_RadioBttnClick,&EventData);
}

void Form_Settings::on_SysColPrev_White_radioButton_clicked()
{
    union DSEventData EventData;

    EventData.RadioBttn.InputID=e_UIS_RadioBttn_SysColPrev_White;
    SendEvent(e_DSEvent_RadioBttnClick,&EventData);
}

void Form_Settings::on_SysCol_R_horizontalScrollBar_valueChanged(int value)
{
    union DSEventData EventData;

    EventData.ScrollBar.InputID=e_UIS_ScrollBar_SysCol_RedScroll;
    EventData.ScrollBar.position=value;
    SendEvent(e_DSEvent_ScrollBarChange,&EventData);
}

void Form_Settings::on_SysCol_G_horizontalScrollBar_valueChanged(int value)
{
    union DSEventData EventData;

    EventData.ScrollBar.InputID=e_UIS_ScrollBar_SysCol_GreenScroll;
    EventData.ScrollBar.position=value;
    SendEvent(e_DSEvent_ScrollBarChange,&EventData);
}

void Form_Settings::on_SysCol_B_horizontalScrollBar_valueChanged(int value)
{
    union DSEventData EventData;

    EventData.ScrollBar.InputID=e_UIS_ScrollBar_SysCol_BlueScroll;
    EventData.ScrollBar.position=value;
    SendEvent(e_DSEvent_ScrollBarChange,&EventData);
}

void Form_Settings::on_SysCol_R_spinBox_valueChanged(int arg1)
{
    union DSEventData EventData;

    EventData.NumberInput.InputID=e_UIS_NumberInput_SysCol_RedInput;
    EventData.NumberInput.value=arg1;
    SendEvent(e_DSEvent_NumberInputChange,&EventData);
}

void Form_Settings::on_SysCol_G_spinBox_valueChanged(int arg1)
{
    union DSEventData EventData;

    EventData.NumberInput.InputID=e_UIS_NumberInput_SysCol_GreenInput;
    EventData.NumberInput.value=arg1;
    SendEvent(e_DSEvent_NumberInputChange,&EventData);
}

void Form_Settings::on_SysCol_B_spinBox_valueChanged(int arg1)
{
    union DSEventData EventData;

    EventData.NumberInput.InputID=e_UIS_NumberInput_SysCol_BlueInput;
    EventData.NumberInput.value=arg1;
    SendEvent(e_DSEvent_NumberInputChange,&EventData);
}

void Form_Settings::on_SysCol_Web_lineEdit_textEdited(const QString &arg1)
{
    union DSEventData EventData;

    EventData.StrInput.InputID=e_UIS_TextInput_SysCol_Web;
    EventData.StrInput.NewText=qPrintable(arg1);
    SendEvent(e_DSEvent_StringInputTextChange,&EventData);
}

void Form_Settings::on_SysColPrev_DefaultFG_radioButton_clicked()
{
    union DSEventData EventData;

    EventData.RadioBttn.InputID=e_UIS_RadioBttn_DefaultColorPrev_Forground;
    SendEvent(e_DSEvent_RadioBttnClick,&EventData);
}

void Form_Settings::on_SysColPrev_DefaultBG_radioButton_clicked()
{
    union DSEventData EventData;

    EventData.RadioBttn.InputID=e_UIS_RadioBttn_DefaultColorPrev_Background;
    SendEvent(e_DSEvent_RadioBttnClick,&EventData);
}

void Form_Settings::on_SelectCursorColor_pushButton_clicked()
{
    union DSEventData EventData;

    EventData.Bttn.InputID=e_UIS_Button_SelectCursorColor;
    SendEvent(e_DSEvent_BttnTriggered,&EventData);
}

void Form_Settings::on_Keyboard_CommandList_listWidget_itemClicked(QListWidgetItem *item)
{
    uintptr_t ID;   // The ID for this item
    union DSEventData EventData;

    ID=(uintptr_t)(item->data(Qt::UserRole).toULongLong());

    EventData.ListView.InputID=e_UIS_ListView_Keyboard_CommandList;
    SendEvent(e_DSEvent_ListViewChange,&EventData,ID);
}

void Form_Settings::on_Keyboard_CommandList_Set_pushButton_clicked()
{
    union DSEventData EventData;

    EventData.Bttn.InputID=e_UIS_Button_KeyboardCmdSet;
    SendEvent(e_DSEvent_BttnTriggered,&EventData);
}

void Form_Settings::on_Keyboard_CommandList_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    uintptr_t ID;   // The ID for this item
    union DSEventData EventData;

    if(current==NULL)
        return;

    ID=(uintptr_t)(current->data(Qt::UserRole).toULongLong());

    EventData.ListView.InputID=e_UIS_ListView_Keyboard_CommandList;
    SendEvent(e_DSEvent_ListViewChange,&EventData,ID);
}

void Form_Settings::on_pushButton_CaptureSelectFilename_clicked()
{
    union DSEventData EventData;

    EventData.Bttn.InputID=e_UIS_Button_CaptureSelectFilename;
    SendEvent(e_DSEvent_BttnTriggered,&EventData);
}

void Form_Settings::on_HexDisplay_SelectFontPushButton_clicked()
{
    union DSEventData EventData;

    EventData.Bttn.InputID=e_UIS_Button_SelectHexDisplayFont;
    SendEvent(e_DSEvent_BttnTriggered,&EventData);
}

void Form_Settings::on_HexDisplay_SelectFGColor_pushButton_clicked()
{
    union DSEventData EventData;

    EventData.Bttn.InputID=e_UIS_Button_SelectHexDisplayFGColor;
    SendEvent(e_DSEvent_BttnTriggered,&EventData);
}

void Form_Settings::on_HexDisplay_SelectBGColor_pushButton_clicked()
{
    union DSEventData EventData;

    EventData.Bttn.InputID=e_UIS_Button_SelectHexDisplayBGColor;
    SendEvent(e_DSEvent_BttnTriggered,&EventData);
}

void Form_Settings::on_HexDisplay_SelectSelBGColor_pushButton_clicked()
{
    union DSEventData EventData;

    EventData.Bttn.InputID=e_UIS_Button_SelectHexDisplaySelBGColor;
    SendEvent(e_DSEvent_BttnTriggered,&EventData);
}
