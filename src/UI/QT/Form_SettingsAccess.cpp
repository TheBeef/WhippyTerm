/*******************************************************************************
 * FILENAME: Form_SettingsAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * CREATED BY:
 *    Paul Hutchinson (27 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_Settings.h"
#include "ui_Form_Settings.h"
#include "Form_MainWindow.h"
#include "UI/UISettings.h"
#include "main.h"
#include <QApplication>
#include <QPalette>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_Settings *g_SettingsDialog;

t_UICheckboxCtrl *UIS_GetCheckboxHandle(e_UIS_Checkbox UIObj)
{
    switch(UIObj)
    {
        case e_UIS_Checkbox_LeftPanel_RestoreFromSettings:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->LeftPanel_FromSettings_checkBox;
        case e_UIS_Checkbox_LeftPanel_ShowPanelOnStartup:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->LeftPanel_ShowOnStartup_checkBox;
        case e_UIS_Checkbox_LeftPanel_AutoHidePanel:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->LeftPanel_AutoHide_checkBox;
        case e_UIS_Checkbox_RightPanel_RestoreFromSettings:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->RightPanel_FromSettings_checkBox;
        case e_UIS_Checkbox_RightPanel_ShowPanelOnStartup:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->RightPanel_ShowOnStartup_checkBox;
        case e_UIS_Checkbox_RightPanel_AutoHidePanel:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->RightPanel_AutoHide_checkBox;
        case e_UIS_Checkbox_BottomPanel_RestoreFromSettings:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->BottomPanel_FromSettings_checkBox;
        case e_UIS_Checkbox_BottomPanel_ShowPanelOnStartup:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->BottomPanel_ShowOnStartup_checkBox;
        case e_UIS_Checkbox_BottomPanel_AutoHidePanel:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->BottomPanel_AutoHide_checkBox;
        case e_UIS_Checkbox_StartMax:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->StartMax_checkBox;
        case e_UIS_Checkbox_AutoConnectOnNewConnection:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->AutoConnectOnNewConnection_checkBox;
        case e_UIS_Checkbox_AlwaysShowTabs:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->AlwaysShowTabs_checkBox;
        case e_UIS_Checkbox_CloseButtonOnTabs:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->CloseBttnOnTabs_checkBox;
        case e_UIS_Checkbox_TerminalSize_FixedWidth:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->FixedWidth_checkBox;
        case e_UIS_Checkbox_TerminalSize_FixedHeight:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->FixedHeight_checkBox;
        case e_UIS_Checkbox_CenterTextInWindow:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->CenterTextInWindow_checkBox;
        case e_UIS_Checkbox_CursorBlink:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->BlinkingCursor_checkBox;
        case e_UIS_Checkbox_BookmarksOpenNewTab:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->BookmarksOpenNewTab_checkBox;
        case e_UIS_Checkbox_StopWatchAutoLap:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->checkBox_StopWatch_AutoLap;
        case e_UIS_Checkbox_StopWatchAutoStart:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->checkBox_StopWatch_AutoStart;
        case e_UIS_Checkbox_StopWatchShowPanel:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->checkBox_StopWatch_ShowPanel;
        case e_UIS_Checkbox_CaptureTimestamp:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->checkBox_CaptureTimestamp;
        case e_UIS_Checkbox_CaptureAppend:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->checkBox_CaptureAppend;
        case e_UIS_Checkbox_CaptureStripCtrl:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->checkBox_CaptureStripCtrl;
        case e_UIS_Checkbox_CaptureStripEsc:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->checkBox_CaptureStripEsc;
        case e_UIS_Checkbox_CaptureHexDump:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->checkBox_CaptureHexDump;
        case e_UIS_Checkbox_CaptureShowPanel:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->checkBox_CaptureShowPanel;
        case e_UIS_Checkbox_HexDisplayEnabled:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->HexDisplay_Enable_checkBox;
        case e_UIS_Checkbox_BoldFontEnable:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->BoldFontEnable_checkBox;
        case e_UIS_Checkbox_ItalicFontEnable:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->ItalicFontEnable_checkBox;
        case e_UIS_Checkbox_UnderlineEnable:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->UnderlineEnable_checkBox;
        case e_UIS_Checkbox_OverlineEnable:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->OverlineEnable_checkBox;
        case e_UIS_Checkbox_ReverseEnable:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->ReverseEnable_checkBox;
        case e_UIS_Checkbox_StrikeThroughEnable:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->StrikeThroughEnable_checkBox;
        case e_UIS_Checkbox_ColorEnable:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->ColorEnable_checkBox;
        case e_UIS_Checkbox_UseCustomSounds:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->UseCustomSounds_checkBox;
        case e_UIS_Checkbox_MouseCursorUseIBeam:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->MouseCursor_UseIBeam_checkBox;
        case e_UIS_Checkbox_AutoReopen:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->AutoReopen_checkBox;
        case e_UIS_Checkbox_AutoCROnLF:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->AutoCR_checkBox;
        case e_UIS_Checkbox_LocalEcho:
            return (t_UICheckboxCtrl *)g_SettingsDialog->ui->LocalEcho_checkBox;
        case e_UIS_CheckboxMAX:
        default:
        break;
    }
    return NULL;
}

t_UIListViewCtrl *UIS_GetListViewHandle(e_UIS_ListView UIObj)
{
    switch(UIObj)
    {
        case e_UIS_ListView_AreaList:
            return (t_UIListViewCtrl *)g_SettingsDialog->ui->Area_listWidget;
        case e_UIS_ListView_InputProTextHighlight:
            return (t_UIListViewCtrl *)g_SettingsDialog->ui->TextHigh_listWidget;
        case e_UIS_ListView_InputProTextOther:
            return (t_UIListViewCtrl *)g_SettingsDialog->ui->Other_listWidget;
        case e_UIS_ListView_KeyBinding_CommandList:
            return (t_UIListViewCtrl *)g_SettingsDialog->ui->KeyBinding_CommandList_listWidget;
        case e_UIS_ListView_BinaryProcessorDecoder:
            return (t_UIListViewCtrl *)g_SettingsDialog->ui->Binary_Processor_Decoder_listWidget;
        case e_UIS_ListView_BinaryProcessorOther:
            return (t_UIListViewCtrl *)g_SettingsDialog->ui->Binary_Other_listWidget;

        case e_UIS_ListViewMAX:
        default:
        break;
    }
    return NULL;
}

t_UIButtonCtrl *UIS_GetButtonHandle(e_UIS_Button UIObj)
{
    switch(UIObj)
    {
        case e_UIS_Button_LeftPanel_GrabLayout:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->LeftPanel_SaveCurrent_pushButton;
        case e_UIS_Button_RightPanel_GrabLayout:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->RightPanel_SaveCurrent_pushButton;
        case e_UIS_Button_BottomPanel_GrabLayout:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->BottomPanel_SaveCurrent_pushButton;
        case e_UIS_Button_GrabCurrentWinPos:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->GrabCurrentWinPos_pushButton;
        case e_UIS_Button_InputProCharEnc_Settings:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->CharEncSettings_pushButton;
        case e_UIS_Button_InputProTermEmu_Settings:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->TermEmuSettings_pushButton;
        case e_UIS_Button_InputProHighLighting_Settings:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->TextHighSettings_pushButton;
        case e_UIS_Button_InputProOther_Settings:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->OtherSettings_pushButton;
        case e_UIS_Button_BinaryPro_Settings:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->BinaryProSettings_pushButton;
        case e_UIS_Button_BinaryProOther_Settings:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->Binary_OtherSettings_pushButton;
        case e_UIS_Button_SysCol_Apply:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->SysCol_Apply_pushButton;
        case e_UIS_Button_SelectFont:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->SelectFontPushButton;
        case e_UIS_Button_SelectCursorColor:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->SelectCursorColor_pushButton;
        case e_UIS_Button_KeyBindingCmdSet:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->KeyBinding_CommandList_Set_pushButton;
        case e_UIS_Button_CaptureSelectFilename:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->pushButton_CaptureSelectFilename;
        case e_UIS_Button_SelectHexDisplayFGColor:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->HexDisplay_SelectFGColor_pushButton;
        case e_UIS_Button_SelectHexDisplayBGColor:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->HexDisplay_SelectBGColor_pushButton;
        case e_UIS_Button_SelectHexDisplayFont:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->HexDisplay_SelectFontPushButton;
        case e_UIS_Button_SelectHexDisplaySelBGColor:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->HexDisplay_SelectSelBGColor_pushButton;
        case e_UIS_Button_CustomSound_SelectFilename:
            return (t_UIButtonCtrl *)g_SettingsDialog->ui->CustomSound_SelectFilename_pushButton;

        case e_UIS_ButtonMAX:
        default:
        break;
    }
    return NULL;
}

t_UITabCtrl *UIS_GetTabCtrlHandle(e_UIS_TabCtrl UIObj)
{
    switch(UIObj)
    {
        case e_UIS_TabCtrl_PanelTab:
            return (t_UITabCtrl *)g_SettingsDialog->ui->Panels_tabWidget;
        case e_UIS_TabCtrl_StartupTab:
            return (t_UITabCtrl *)g_SettingsDialog->ui->Startup_tabWidget;
        case e_UIS_TabCtrl_ConnectionsTab:
            return (t_UITabCtrl *)g_SettingsDialog->ui->Connection_tabWidget;
        case e_UIS_TabCtrl_Display:
            return (t_UITabCtrl *)g_SettingsDialog->ui->Display_tabWidget;
        case e_UIS_TabCtrl_Terminal:
            return (t_UITabCtrl *)g_SettingsDialog->ui->Terminal_tabWidget;
        case e_UIS_TabCtrl_Behaviour:
            return (t_UITabCtrl *)g_SettingsDialog->ui->Behaviour_tabWidget;

        case e_UIS_TabMAX:
        default:
        break;
    }
    return NULL;
}

t_UIComboBoxCtrl *UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox UIObj)
{
    switch(UIObj)
    {
        case e_UIS_ComboBox_WindowStartupPos:
            return (t_UIComboBoxCtrl *)g_SettingsDialog->ui->WindowStartupPos_comboBox;
        case e_UIS_ComboBox_DataProcessor:
            return (t_UIComboBoxCtrl *)g_SettingsDialog->ui->InputProcessingType_comboBox;
        case e_UIS_ComboBox_TextProCharEnc:
            return (t_UIComboBoxCtrl *)g_SettingsDialog->ui->CharEnc_comboBox;
        case e_UIS_ComboBox_TextProTermEmu:
            return (t_UIComboBoxCtrl *)g_SettingsDialog->ui->TermEmu_comboBox;
        case e_UIS_ComboBox_SysCol_Preset:
            return (t_UIComboBoxCtrl *)g_SettingsDialog->ui->SysCol_Preset_comboBox;

        case e_UIS_ComboBoxMAX:
        default:
        break;
    }
    return NULL;
}

t_UINumberInput *UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput UIObj)
{
    switch(UIObj)
    {
        case e_UIS_NumberInput_TermSizeWidth:
            return (t_UINumberInput *)g_SettingsDialog->ui->TermSizeWidth_spinBox;
        case e_UIS_NumberInput_TermSizeHeight:
            return (t_UINumberInput *)g_SettingsDialog->ui->TermSizeHeight_spinBox;
        case e_UIS_NumberInput_ScrollBufferLines:
            return (t_UINumberInput *)g_SettingsDialog->ui->ScrollBufferLines_spinBox;
        case e_UIS_NumberInput_SysCol_RedInput:
            return (t_UINumberInput *)g_SettingsDialog->ui->SysCol_R_spinBox;
        case e_UIS_NumberInput_SysCol_GreenInput:
            return (t_UINumberInput *)g_SettingsDialog->ui->SysCol_G_spinBox;
        case e_UIS_NumberInput_SysCol_BlueInput:
            return (t_UINumberInput *)g_SettingsDialog->ui->SysCol_B_spinBox;
        case e_UIS_NumberInput_HexDisplay_BufferSize:
            return (t_UINumberInput *)g_SettingsDialog->ui->HexDisplay_BufferSize_spinBox;
        case e_UIS_NumberInput_AutoReopenWaitTime:
            return (t_UINumberInput *)g_SettingsDialog->ui->AutoReopenWaitTime_spinBox;
        case e_UIS_NumberInput_DelayBetweenBytes:
            return (t_UINumberInput *)g_SettingsDialog->ui->TxByteDelay_spinBox;
        case e_UIS_NumberInput_DelayAfterNewLineSent:
            return (t_UINumberInput *)g_SettingsDialog->ui->TxLineDelay_spinBox;
        case e_UIS_NumberInputMAX:
        default:
        break;
    }
    return NULL;
}

t_UIGroupBox *UIS_GetGroupBoxHandle(e_UIS_GroupBox UIObj)
{
    switch(UIObj)
    {
        case e_UIS_GroupBox_TextProcessingSettings:
            return (t_UIGroupBox *)g_SettingsDialog->ui->TextProcessingSettings_groupBox;
        case e_UIS_GroupBox_Display_Tabs:
            return (t_UIGroupBox *)g_SettingsDialog->ui->groupBox_DisplayTabs;
        case e_UIS_GroupBox_Display_ClearScreen:
            return (t_UIGroupBox *)g_SettingsDialog->ui->groupBox_DisplayClearScreen;
        case e_UIS_GroupBox_Display_MouseCursor:
            return (t_UIGroupBox *)g_SettingsDialog->ui->MouseCursor_groupBox;

        case e_UIS_GroupBoxMAX:
        default:
        break;
    }
    return NULL;
}

t_UIRadioBttnCtrl *UIS_GetRadioBttnHandle(e_UIS_RadioBttns UIObj)
{
    switch(UIObj)
    {
        case e_UIS_RadioBttn_SysCol_Shade_Normal:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysCol_Normal_radioButton;
        case e_UIS_RadioBttn_SysCol_Shade_Bright:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysCol_Bright_radioButton;
        case e_UIS_RadioBttn_SysCol_Shade_Dark:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysCol_Dark_radioButton;
        case e_UIS_RadioBttn_SysColPrev_Black:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysColPrev_Black_radioButton;
        case e_UIS_RadioBttn_SysColPrev_Red:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysColPrev_Red_radioButton;
        case e_UIS_RadioBttn_SysColPrev_Green:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysColPrev_Green_radioButton;
        case e_UIS_RadioBttn_SysColPrev_Yellow:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysColPrev_Yellow_radioButton;
        case e_UIS_RadioBttn_SysColPrev_Blue:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysColPrev_Blue_radioButton;
        case e_UIS_RadioBttn_SysColPrev_Magenta:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysColPrev_Magenta_radioButton;
        case e_UIS_RadioBttn_SysColPrev_Cyan:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysColPrev_Cyan_radioButton;
        case e_UIS_RadioBttn_SysColPrev_White:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysColPrev_White_radioButton;
        case e_UIS_RadioBttn_DefaultColorPrev_Forground:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysColPrev_DefaultFG_radioButton;
        case e_UIS_RadioBttn_DefaultColorPrev_Background:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysColPrev_DefaultBG_radioButton;
        case e_UIS_RadioBttn_Display_ClearScreen_Clear:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->ClearScreen_radioButton;
        case e_UIS_RadioBttn_Display_ClearScreen_Scroll:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->ClearScreen_Scroll_radioButton;
        case e_UIS_RadioBttn_Display_ClearScreen_ScrollAll:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->ClearScreen_ScrollAll_radioButton;
        case e_UIS_RadioBttn_Display_ClearScreen_ScrollWithHR:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->ClearScreen_ScrollAndHR_radioButton;
        case e_UIS_RadioBttn_Keyboard_Backspace_BS:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->Backspace_SendBS_radioButton;
        case e_UIS_RadioBttn_Keyboard_Backspace_DEL:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->Backspace_SendDEL_radioButton;
        case e_UIS_RadioBttn_Keyboard_Enter_CR:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->Enter_SendCR_radioButton;
        case e_UIS_RadioBttn_Keyboard_Enter_LF:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->Enter_SendLF_radioButton;
        case e_UIS_RadioBttn_Keyboard_Enter_CRLF:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->Enter_SendCRLF_radioButton;
        case e_UIS_RadioBttn_Keyboard_Clipboard_None:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->ClipboardKeys_None_radioButton;
        case e_UIS_RadioBttn_Keyboard_Clipboard_Normal:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->ClipboardKeys_Normal_radioButton;
        case e_UIS_RadioBttn_Keyboard_Clipboard_ShiftCtrl:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->ClipboardKeys_ShiftCtrlClip_radioButton;
        case e_UIS_RadioBttn_Keyboard_Clipboard_Alt:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->ClipboardKeys_AltClip_radioButton;
        case e_UIS_RadioBttn_Keyboard_Clipboard_Smart:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->ClipboardKeys_SmartClip_radioButton;
        case e_UIS_RadioBttn_SysBell_None:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysBell_None_radioButton;
        case e_UIS_RadioBttn_SysBell_System:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysBell_System_radioButton;
        case e_UIS_RadioBttn_SysBell_BuiltIn:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysBell_BuiltIn_radioButton;
        case e_UIS_RadioBttn_SysBell_AudioOnly:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysBell_AudioOnly_radioButton;
        case e_UIS_RadioBttn_SysBell_VisualOnly:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysBell_VisualOnly_radioButton;
        case e_UIS_RadioBttn_SelectionColorPrev_Forground:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysColPrev_SelectionFG_radioButton;
        case e_UIS_RadioBttn_SelectionColorPrev_Background:
            return (t_UIRadioBttnCtrl *)g_SettingsDialog->ui->SysColPrev_SelectionBG_radioButton;
        case e_UIS_RadioBttnMAX:
        default:
        break;
    }
    return NULL;
}

t_UIScrollBarCtrl *UIS_GetScrollBarHandle(e_UIS_ScrollBar UIObj)
{
    switch(UIObj)
    {
        case e_UIS_ScrollBar_SysCol_RedScroll:
            return (t_UIScrollBarCtrl *)g_SettingsDialog->ui->SysCol_R_horizontalScrollBar;
        case e_UIS_ScrollBar_SysCol_GreenScroll:
            return (t_UIScrollBarCtrl *)g_SettingsDialog->ui->SysCol_G_horizontalScrollBar;
        case e_UIS_ScrollBar_SysCol_BlueScroll:
            return (t_UIScrollBarCtrl *)g_SettingsDialog->ui->SysCol_B_horizontalScrollBar;

        case e_UIS_ScrollBar_SysColMAX:
        default:
        break;
    }
    return NULL;
}

t_UITextInputCtrl *UIS_GetTextInputHandle(e_UIS_TextInput UIObj)
{
    switch(UIObj)
    {
        case e_UIS_TextInput_SysCol_Web:
            return (t_UITextInputCtrl *)g_SettingsDialog->ui->SysCol_Web_lineEdit;
        case e_UIS_TextInput_KeyBinding_Assigned2:
            return (t_UITextInputCtrl *)g_SettingsDialog->ui->KeyBinding_Assigned2_lineEdit;
        case e_UIS_TextInput_Capture_DefaultFilename:
            return (t_UITextInputCtrl *)g_SettingsDialog->ui->lineEdit_CaptureDefaultFilename;
        case e_UIS_TextInput_CustomSoundFilename:
            return (t_UITextInputCtrl *)g_SettingsDialog->ui->CustomSoundFilename_lineEdit;

        case e_UIS_TextInputMAX:
        default:
        break;
    }
    return NULL;
}

t_UIColorPreviewCtrl *UIS_GetColorPreviewHandle(e_UIS_ColorPreview UIObj)
{
    switch(UIObj)
    {
        case e_UIS_ColorPreview_CursorColor:
            return (t_UIColorPreviewCtrl *)g_SettingsDialog->ui->CursorColorDisplay_frame;
        case e_UIS_ColorPreview_HexDisplay_FGDisplay:
            return (t_UIColorPreviewCtrl *)g_SettingsDialog->ui->HexDisplay_FGDisplay_frame;
        case e_UIS_ColorPreview_HexDisplay_BGDisplay:
            return (t_UIColorPreviewCtrl *)g_SettingsDialog->ui->HexDisplay_BGDisplay_frame;
        case e_UIS_ColorPreview_HexDisplay_SelBGDisplay:
            return (t_UIColorPreviewCtrl *)g_SettingsDialog->ui->HexDisplay_SelBGDisplay_frame;

        case e_UIS_ColorPreviewMAX:
        default:
        break;
    }
    return NULL;
}

t_UILabelCtrl *UIS_GetLabelHandle(e_UIS_Labels UIObj)
{
    switch(UIObj)
    {
        case e_UIS_Labels_CurrentFont:
            return (t_UILabelCtrl *)g_SettingsDialog->ui->CurrentFontLabel;
        case e_UIS_Labels_HexDisplayCurrentFont:
            return (t_UILabelCtrl *)g_SettingsDialog->ui->HexDisplay_CurrentFontLabel;

        case e_UIS_LabelsMAX:
        default:
        break;
    }
    return NULL;
}

t_UIPagePanelCtrl *UIS_GetPagePanelHandle(e_UIS_PagePanel UIObj)
{
    switch(UIObj)
    {
        case e_UIS_PagePanel_DataProcessing:
            return (t_UIPagePanelCtrl *)g_SettingsDialog->ui->DataProcessing_stackedWidget;

        case e_UIS_PagePanelMAX:
        default:
        break;
    }
    return NULL;
}

bool UIS_Alloc_Settings(void)
{
    try
    {
        g_SettingsDialog=new Form_Settings(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_SettingsDialog=NULL;
        return false;
    }
    return true;
}

bool UIS_Show_Settings(void)
{
    if(g_SettingsDialog->exec()==QDialog::Accepted)
    {
        return true;
    }

    return false;
}

void UIS_Free_Settings(void)
{
    delete g_SettingsDialog;

    g_SettingsDialog=NULL;
}

void UIS_MakeTabVisable(e_UIS_TabCtrl Tab)
{
    switch(Tab)
    {
        case e_UIS_TabCtrl_PanelTab:
            g_SettingsDialog->ui->Editing_stackedWidget->setCurrentIndex(0);
        break;
        case e_UIS_TabCtrl_StartupTab:
            g_SettingsDialog->ui->Editing_stackedWidget->setCurrentIndex(1);
        break;
        case e_UIS_TabCtrl_ConnectionsTab:
            g_SettingsDialog->ui->Editing_stackedWidget->setCurrentIndex(2);
        break;
        case e_UIS_TabCtrl_Display:
            g_SettingsDialog->ui->Editing_stackedWidget->setCurrentIndex(3);
        break;
        case e_UIS_TabCtrl_Terminal:
            g_SettingsDialog->ui->Editing_stackedWidget->setCurrentIndex(4);
        break;
        case e_UIS_TabCtrl_Behaviour:
            g_SettingsDialog->ui->Editing_stackedWidget->setCurrentIndex(5);
        break;
        case e_UIS_TabMAX:
        default:
        break;
    }
}

void UIS_SetPresetPreviewColor(e_SysColType Color,uint32_t RGB)
{
    QPalette pal;
    QColor col;

    col.setRgb(RGB);
    pal=QApplication::palette();
//    pal.setColor(QPalette::Background,col);
    pal.setColor(QPalette::Window,col);

    switch(Color)
    {
        case e_SysCol_Black:
            g_SettingsDialog->ui->SysCol_Preset_Black_frame->setPalette(pal);
        break;
        case e_SysCol_Red:
            g_SettingsDialog->ui->SysCol_Preset_Red_frame->setPalette(pal);
        break;
        case e_SysCol_Green:
            g_SettingsDialog->ui->SysCol_Preset_Green_frame->setPalette(pal);
        break;
        case e_SysCol_Yellow:
            g_SettingsDialog->ui->SysCol_Preset_Yellow_frame->setPalette(pal);
        break;
        case e_SysCol_Blue:
            g_SettingsDialog->ui->SysCol_Preset_Blue_frame->setPalette(pal);
        break;
        case e_SysCol_Magenta:
            g_SettingsDialog->ui->SysCol_Preset_Magenta_frame->setPalette(pal);
        break;
        case e_SysCol_Cyan:
            g_SettingsDialog->ui->SysCol_Preset_Cyan_frame->setPalette(pal);
        break;
        case e_SysCol_White:
            g_SettingsDialog->ui->SysCol_Preset_White_frame->setPalette(pal);
        break;
        case e_SysColMAX:
        default:
        break;
    }
}

void UIS_SetSysColPreviewColor(e_SysColType Color,uint32_t RGB)
{
    QPalette pal;
    QColor col;

    col.setRgb(RGB);
    pal=QApplication::palette();
//    pal.setColor(QPalette::Background,col);
    pal.setColor(QPalette::Window,col);

    switch(Color)
    {
        case e_SysCol_Black:
            g_SettingsDialog->ui->SysCol_Black_frame->setPalette(pal);
        break;
        case e_SysCol_Red:
            g_SettingsDialog->ui->SysCol_Red_frame->setPalette(pal);
        break;
        case e_SysCol_Green:
            g_SettingsDialog->ui->SysCol_Green_frame->setPalette(pal);
        break;
        case e_SysCol_Yellow:
            g_SettingsDialog->ui->SysCol_Yellow_frame->setPalette(pal);
        break;
        case e_SysCol_Blue:
            g_SettingsDialog->ui->SysCol_Blue_frame->setPalette(pal);
        break;
        case e_SysCol_Magenta:
            g_SettingsDialog->ui->SysCol_Magenta_frame->setPalette(pal);
        break;
        case e_SysCol_Cyan:
            g_SettingsDialog->ui->SysCol_Cyan_frame->setPalette(pal);
        break;
        case e_SysCol_White:
            g_SettingsDialog->ui->SysCol_White_frame->setPalette(pal);
        break;
        case e_SysColMAX:
        default:
        break;
    }
}

void UIS_SetSysColPreviewSelected(e_SysColType Selected)
{
    g_SettingsDialog->ui->SysCol_Black_frame->setLineWidth(1);
    g_SettingsDialog->ui->SysCol_Red_frame->setLineWidth(1);
    g_SettingsDialog->ui->SysCol_Green_frame->setLineWidth(1);
    g_SettingsDialog->ui->SysCol_Yellow_frame->setLineWidth(1);
    g_SettingsDialog->ui->SysCol_Blue_frame->setLineWidth(1);
    g_SettingsDialog->ui->SysCol_Magenta_frame->setLineWidth(1);
    g_SettingsDialog->ui->SysCol_Cyan_frame->setLineWidth(1);
    g_SettingsDialog->ui->SysCol_White_frame->setLineWidth(1);

    switch(Selected)
    {
        case e_SysCol_Black:
            g_SettingsDialog->ui->SysCol_Black_frame->setLineWidth(5);
        break;
        case e_SysCol_Red:
            g_SettingsDialog->ui->SysCol_Red_frame->setLineWidth(5);
        break;
        case e_SysCol_Green:
            g_SettingsDialog->ui->SysCol_Green_frame->setLineWidth(5);
        break;
        case e_SysCol_Yellow:
            g_SettingsDialog->ui->SysCol_Yellow_frame->setLineWidth(5);
        break;
        case e_SysCol_Blue:
            g_SettingsDialog->ui->SysCol_Blue_frame->setLineWidth(5);
        break;
        case e_SysCol_Magenta:
            g_SettingsDialog->ui->SysCol_Magenta_frame->setLineWidth(5);
        break;
        case e_SysCol_Cyan:
            g_SettingsDialog->ui->SysCol_Cyan_frame->setLineWidth(5);
        break;
        case e_SysCol_White:
            g_SettingsDialog->ui->SysCol_White_frame->setLineWidth(5);
        break;
        case e_SysColMAX:
        default:
        break;
    }
}

void UIS_SetDefaultPreviewColor(e_DefaultColorsType Color,uint32_t RGB)
{
    QPalette pal;
    QColor col;

    col.setRgb(RGB);
    pal=QApplication::palette();
//    pal.setColor(QPalette::Background,col);
    pal.setColor(QPalette::Window,col);

    switch(Color)
    {
        case e_DefaultColors_BG:
            g_SettingsDialog->ui->SysCol_DefaultBG_frame->setPalette(pal);
        break;
        case e_DefaultColors_FG:
            g_SettingsDialog->ui->SysCol_DefaultFG_frame->setPalette(pal);
        break;
        case e_DefaultColorsMAX:
        default:
        break;
    }
}

void UIS_SetSelectionPreviewColor(e_ColorType Color,uint32_t RGB)
{
    QPalette pal;
    QColor col;

    col.setRgb(RGB);
    pal=QApplication::palette();
//    pal.setColor(QPalette::Background,col);
    pal.setColor(QPalette::Window,col);

    switch(Color)
    {
        case e_Color_BG:
            g_SettingsDialog->ui->SysCol_SelectionBG_frame->setPalette(pal);
        break;
        case e_Color_FG:
            g_SettingsDialog->ui->SysCol_SelectionFG_frame->setPalette(pal);
        break;
        case e_ColorMAX:
        default:
        break;
    }
}
