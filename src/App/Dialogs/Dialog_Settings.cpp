/* DEBUG PAUL:
I think the settings need to be copied, and restored on cancel button.  This
is because I see some stuff that changes the data in the global structure
instead of in the GUI (window pos for example, and plugin settings)

 * All the settings buttons
 * Add setting button to binary misc
 * 
*/

/*******************************************************************************
 * FILENAME: Dialog_Settings.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file runs the settings dialog.
 *
 * COPYRIGHT:
 *    Copyright 2018 Paul Hutchinson.
 *
 *    This program is free software: you can redistribute it and/or modify it
 *    under the terms of the GNU General Public License as published by the
 *    Free Software Foundation, either version 3 of the License, or (at your
 *    option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program. If not, see https://www.gnu.org/licenses/.
 *
 * CREATED BY:
 *    Paul Hutchinson (27 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_Settings.h"
#include "App/Dialogs/Dialog_DataProPluginSettings.h"
#include "App/Display/DisplayColors.h"
#include "App/DataProcessorsSystem.h"
#include "App/Settings.h"
#include "App/Session.h"
#include "App/IOSystem.h"
#include "App/MainApp.h"
#include "UI/UIAsk.h"
#include "UI/UISettings.h"
#include "UI/UIMainWindow.h"
#include "UI/UIFileReq.h"
#include "UI/UIFontReq.h"
#include "UI/UIColorReq.h"
#include "OS/Directorys.h"
#include "Version.h"
#include <time.h>
#include <string>
#include <algorithm>
#include <string.h>

using namespace std;

/*** DEFINES                  ***/

/*********
 * Term
 *********/
#define MIN_TERM_SIZE_X     1
#define MIN_TERM_SIZE_Y     1

#define MAX_TERM_SIZE_X     1000
#define MAX_TERM_SIZE_Y     1000

#define MIN_TERM_SCROLLBUFFER_LINES 0
#define MAX_TERM_SCROLLBUFFER_LINES 10000

/* Flags for DS_UnselectColorRadioButtons() */
#define DS_USCRB_DEFAULT            (1<<0)
#define DS_USCRB_COLORS             (1<<1)
#define DS_USCRB_SELECTION          (1<<2)

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
enum e_DS_SettingsArea
{
    e_DS_SettingsArea_Panels,
    e_DS_SettingsArea_Startup,
    e_DS_SettingsArea_Connections,
    e_DS_SettingsArea_Display,
    e_DS_SettingsArea_Terminal,
    e_DS_SettingsArea_Behaviour,
    e_DS_SettingsAreaMAX
};

enum e_DS_EditingColorType
{
    e_DS_EditingColor_Defaults,
    e_DS_EditingColor_Colors,
    e_DS_EditingColor_Selection,
    e_DS_EditingColorMAX
};

/*** FUNCTION PROTOTYPES      ***/
static void DS_SetSettingGUI(void);
static void DS_GetSettingsFromGUI(void);
static void DS_RethinkGUI(void);
static void DS_RethinkTerminalDisplay(void);
static void DS_RethinkDisplayDisplay(void);
static void DS_RethinkHexDisplayDisplay(void);
static void DS_CopySysColors2GUI(void);
static void DS_UpdateColorPreview(bool UpdateWeb);
static void DS_SetCurrentColor(uint32_t RGB);
static uint32_t DS_GetCurrentColor(void);
static void DS_SelectCaptureFilename(void);
static void DS_DoSelectFont(void);
static void DS_DoSelectHexDisplayFont(void);
static void DS_DoSysColApply(void);
static void UIS_HandleLeftPanelAutoHideClick(bool checked);
static void UIS_HandleRightPanelAutoHideClick(bool checked);
static void UIS_HandleBottomPanelAutoHideClick(bool checked);
static void UIS_HandleSysColPreviewColorClick(void);
static void UIS_HandleSysColDefaultColorClick(void);
static void UIS_ProcessSysColWebInputValue(const char *Text);
static void UIS_HandleWindowStartPosChanged(uintptr_t ID);
static void UIS_HandleInputProcessingChange(uintptr_t ID);
static void UIS_HandleInputProcessingCharEncChange(void);
static void UIS_HandleInputProcessingTermEmuChange(void);
static void UIS_HandleInputProcessingHighlighterChange(void);
static void UIS_HandleInputProcessingOtherChange(void);
static void UIS_HandleInputProcessingBinaryDecoderChange(void);
static void UIS_HandleInputProcessingBinaryOtherChange(void);
static void UIS_HandleSysColPresetChange(uintptr_t ID);
static void UIS_HandleAreaChanged(uintptr_t ID);
static void UIS_HandleKeyBindingsCmdListChange(uintptr_t ID);
static void UIS_HandleKeyBindingsCmdShortCutChange(void);
static void DS_SetKeyboardRadioBttns(void);
static void DS_GetSettingsFromGUI_KeyboardRadioBttns(void);
static void DS_UnselectColorRadioButtons(uint32_t Bttns);
static void UIS_HandleSysColSelectionColorClick(void);
static e_DS_EditingColorType DS_FindCurrentEditingColor(int *Color);

/*** VARIABLE DEFINITIONS     ***/
t_DPS_ProInfoType m_CharEncodingInputPros;
t_DPS_ProInfoType m_TermEmulationInputPros;
t_DPS_ProInfoType m_HighlighterInputPros;
t_DPS_ProInfoType m_OtherInputPros;
t_DPS_ProInfoType m_BinaryDecoders;
t_DPS_ProInfoType m_BinaryOtherDecoders;
uint32_t m_DS_SysColors[e_SysColShadeMAX][e_SysColMAX];
uint32_t m_DS_DefaultColors[e_DefaultColorsMAX];
uint32_t m_DS_SelectionColors[e_ColorMAX];
bool m_DS_UpdatingColorPreview;
class TheMainWindow *m_SettingsMW;
struct ConnectionInfoList *m_DS_ConList;
t_ConnectionOptionsDataType *m_DS_ConOptions;
struct ConnectionInfoList *m_DS_SelectedConnection;
uint32_t m_CursorColor;
uint32_t m_HexDisplaysFGColor;
uint32_t m_HexDisplaysBGColor;
uint32_t m_HexDisplaysSelBGColor;
struct CommandKeySeq m_CopyOfKeyMapping[e_CmdMAX];
class ConSettings *m_SettingConSettings;
bool m_SettingConSettingsOnly;

struct ProInfoSortCB
{
    inline bool operator() (const struct DPS_ProInfo &ent1,
            const struct DPS_ProInfo &ent2)
    {
        return strcasecmp(ent1.DisplayName,ent2.DisplayName)<0;
    }
};
struct DPS_ProInfo DS_DPSNoneEntry=
{
    "",             // ID
    "NONE",
    NULL,
    NULL
};

/*******************************************************************************
 * NAME:
 *    RunSettingsDialog
 *
 * SYNOPSIS:
 *    bool RunSettingsDialog(class TheMainWindow *MW,
 *          class ConSettings *SetConSettings,e_SettingsJump2Type Jump2);
 *
 * PARAMETERS:
 *    MW [I] -- The main window to take info about (the main window that
 *              was active when the command was run).  You can pass NULL if
 *              are doing a con settings.
 *    SetConSettings [I] -- If this set to a ConSettings structure then
 *                          the dialog will only set connection settings.
 *                          If this is set to NULL then it will set all
 *                          settings including the default connection settings.
 *    Jump2 [I] -- What to display after opening the settings dialog
 *
 * FUNCTION:
 *    This function shows the new connection dialog and fill in all the
 *    defaults.
 *
 * RETURNS:
 *    true -- User pressed ok
 *    false -- User canceled or there was an error
 *
 * SEE ALSO:
 *    
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 ******************************************************************************/
bool RunSettingsDialog(class TheMainWindow *MW,
        class ConSettings *SetConSettings,e_SettingsJump2Type Jump2)
{
    bool RetValue;
    unsigned int r;
    t_UIListViewCtrl *AreaList;
    t_UIListViewCtrl *InputProTextHighlight;
    t_UIListViewCtrl *InputProTextOther;
    t_UIListViewCtrl *BinaryProcessorsDecoder;
    t_UIComboBoxCtrl *WindowStartupPos;
    t_UINumberInput *TermSize_Width;
    t_UINumberInput *TermSize_Height;
    t_UINumberInput *ScrollBufferLines;
    t_UIComboBoxCtrl *DataProcessor;
    t_UIComboBoxCtrl *TextProCharEnc;
    t_UIComboBoxCtrl *TextProTermEmu;
    t_UIComboBoxCtrl *SysColPreset;
    t_UIRadioBttnCtrl *SysColShade;
    t_UIRadioBttnCtrl *SysColPrev;
    t_UIScrollBarCtrl *RedScroll;
    t_UIScrollBarCtrl *GreenScroll;
    t_UIScrollBarCtrl *BlueScroll;
    t_UINumberInput *RedInput;
    t_UINumberInput *GreenInput;
    t_UINumberInput *BlueInput;
    t_UIListViewCtrl *KeyBindingsCmdList;
    t_UITabCtrl *TerminalTabCtrl;
    t_UITabCtrl *DisplayTabCtrl;
    t_UIGroupBox *Display_Tabs;
    t_UIGroupBox *Display_ClearScreen;
    t_UIGroupBox *Display_MouseCursor;
    e_DS_SettingsArea FirstSelectedArea;
    e_UIS_TabCtrl_Terminal_Page SelectTerminalPage;
    e_UIS_TabCtrl_Display_Page SelectDisplayPage;
    t_UICheckboxCtrl *CheckboxHandle;

    m_SettingsMW=MW;
    m_DS_ConList=NULL;
    m_DS_SelectedConnection=NULL;
    m_DS_ConOptions=NULL;

    if(SetConSettings==NULL)
    {
        m_SettingConSettingsOnly=false;
        m_SettingConSettings=&g_Settings.DefaultConSettings;
    }
    else
    {
        m_SettingConSettingsOnly=true;
        m_SettingConSettings=SetConSettings;
    }

    if(!UIS_Alloc_Settings())
    {
        /* DEBUG PAUL: Ask here */
        return false;
    }

    memcpy(&m_CopyOfKeyMapping,&g_Settings.KeyMapping,
            sizeof(m_CopyOfKeyMapping));

    /* Setup the UI */
    AreaList=UIS_GetListViewHandle(e_UIS_ListView_AreaList);
    UIClearListView(AreaList);
    if(!m_SettingConSettingsOnly)
    {
        UIAddItem2ListView(AreaList,"Behaviour",e_DS_SettingsArea_Behaviour);
    }
    UIAddItem2ListView(AreaList,"Connections",e_DS_SettingsArea_Connections);
    UIAddItem2ListView(AreaList,"Display",e_DS_SettingsArea_Display);
    if(!m_SettingConSettingsOnly)
    {
        UIAddItem2ListView(AreaList,"Panels",e_DS_SettingsArea_Panels);
        UIAddItem2ListView(AreaList,"Startup",e_DS_SettingsArea_Startup);
    }
    UIAddItem2ListView(AreaList,"Terminal",e_DS_SettingsArea_Terminal);

    WindowStartupPos=UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox_WindowStartupPos);
    UIClearComboBox(WindowStartupPos);
    UIAddItem2ComboBox(WindowStartupPos,"OS Default",e_WindowStartupPos_OSDefault);
    UIAddItem2ComboBox(WindowStartupPos,"Session",e_WindowStartupPos_RestoreFromSession);
    UIAddItem2ComboBox(WindowStartupPos,"Settings",e_WindowStartupPos_RestoreFromSettings);

    TermSize_Width=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_TermSizeWidth);
    TermSize_Height=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_TermSizeHeight);
    UISetNumberInputCtrlMin(TermSize_Width,MIN_TERM_SIZE_X);
    UISetNumberInputCtrlMin(TermSize_Height,MIN_TERM_SIZE_Y);
    UISetNumberInputCtrlMax(TermSize_Width,MAX_TERM_SIZE_X);
    UISetNumberInputCtrlMax(TermSize_Height,MAX_TERM_SIZE_Y);

    ScrollBufferLines=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_ScrollBufferLines);
    UISetNumberInputCtrlMin(ScrollBufferLines,MIN_TERM_SCROLLBUFFER_LINES);
    UISetNumberInputCtrlMax(ScrollBufferLines,MAX_TERM_SCROLLBUFFER_LINES);

    /*** Input Processors ***/
    DataProcessor=UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox_DataProcessor);
    UIClearComboBox(DataProcessor);
    UIAddItem2ComboBox(DataProcessor,"Text",e_DataProcessorType_Text);
    UIAddItem2ComboBox(DataProcessor,"Binary",e_DataProcessorType_Binary);

    /* Fill in the available input processors */
    DPS_GetListOfTextProcessors(e_TextDataProcessorClass_CharEncoding,
            m_CharEncodingInputPros);
    DPS_GetListOfTextProcessors(e_TextDataProcessorClass_TermEmulation,
            m_TermEmulationInputPros);
    DPS_GetListOfTextProcessors(e_TextDataProcessorClass_Highlighter,
            m_HighlighterInputPros);
    DPS_GetListOfTextProcessors(e_TextDataProcessorClass_Other,
            m_OtherInputPros);
    DPS_GetListOfBinaryProcessors(e_BinaryDataProcessorClass_Decoder,
            m_BinaryDecoders);
    DPS_GetListOfBinaryProcessors(e_BinaryDataProcessorClass_Other,
            m_BinaryOtherDecoders);

    /* Sort them */
    std::sort(m_CharEncodingInputPros.begin(),m_CharEncodingInputPros.end(),
            ProInfoSortCB());
    std::sort(m_TermEmulationInputPros.begin(),m_TermEmulationInputPros.end(),
            ProInfoSortCB());
    std::sort(m_HighlighterInputPros.begin(),m_HighlighterInputPros.end(),
            ProInfoSortCB());
    std::sort(m_OtherInputPros.begin(),m_OtherInputPros.end(),ProInfoSortCB());
    std::sort(m_BinaryDecoders.begin(),m_BinaryDecoders.end(),ProInfoSortCB());
    std::sort(m_BinaryOtherDecoders.begin(),m_BinaryOtherDecoders.end(),
            ProInfoSortCB());

    /* Add NONE to the top */
    m_CharEncodingInputPros.insert(m_CharEncodingInputPros.begin(),
            DS_DPSNoneEntry);
    m_TermEmulationInputPros.insert(m_TermEmulationInputPros.begin(),
            DS_DPSNoneEntry);

    TextProCharEnc=UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox_TextProCharEnc);
    UIClearComboBox(TextProCharEnc);
    for(r=0;r<m_CharEncodingInputPros.size();r++)
    {
        UIAddItem2ComboBox(TextProCharEnc,
                m_CharEncodingInputPros[r].DisplayName,r);

        if(m_CharEncodingInputPros[r].Tip!=NULL)
        {
            UISetComboBoxItemToolTip(TextProCharEnc,r,
                    m_CharEncodingInputPros[r].Tip);
        }
    }

    TextProTermEmu=UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox_TextProTermEmu);
    UIClearComboBox(TextProTermEmu);
    for(r=0;r<m_TermEmulationInputPros.size();r++)
    {
        UIAddItem2ComboBox(TextProTermEmu,
                m_TermEmulationInputPros[r].DisplayName,r);
        if(m_TermEmulationInputPros[r].Tip!=NULL)
        {
            UISetComboBoxItemToolTip(TextProTermEmu,r,
                    m_TermEmulationInputPros[r].Tip);
        }
    }

    InputProTextHighlight=UIS_GetListViewHandle(e_UIS_ListView_InputProTextHighlight);
    UIClearListView(InputProTextHighlight);
    for(r=0;r<m_HighlighterInputPros.size();r++)
    {
        UIAddItem2ListView(InputProTextHighlight,
                m_HighlighterInputPros[r].DisplayName,r);
        UISetListViewEntryCheckable(InputProTextHighlight,r,true);
        UISetListViewEntryToolTip(InputProTextHighlight,r,
                m_HighlighterInputPros[r].Tip);
    }

    InputProTextOther=UIS_GetListViewHandle(e_UIS_ListView_InputProTextOther);
    UIClearListView(InputProTextOther);
    for(r=0;r<m_OtherInputPros.size();r++)
    {
        UIAddItem2ListView(InputProTextOther,m_OtherInputPros[r].DisplayName,r);
        UISetListViewEntryCheckable(InputProTextOther,r,true);
        UISetListViewEntryToolTip(InputProTextOther,r,m_OtherInputPros[r].Tip);
    }

    BinaryProcessorsDecoder=UIS_GetListViewHandle(e_UIS_ListView_BinaryProcessorDecoder);
    UIClearListView(BinaryProcessorsDecoder);
    for(r=0;r<m_BinaryDecoders.size();r++)
    {
        UIAddItem2ListView(BinaryProcessorsDecoder,
                m_BinaryDecoders[r].DisplayName,r);

        if(m_BinaryDecoders[r].Tip!=NULL)
        {
            UISetListViewEntryToolTip(BinaryProcessorsDecoder,r,
                    m_BinaryDecoders[r].Tip);
        }
    }

    BinaryProcessorsDecoder=UIS_GetListViewHandle(e_UIS_ListView_BinaryProcessorOther);
    UIClearListView(BinaryProcessorsDecoder);
    for(r=0;r<m_BinaryOtherDecoders.size();r++)
    {
        UIAddItem2ListView(BinaryProcessorsDecoder,
                m_BinaryOtherDecoders[r].DisplayName,r);
        UISetListViewEntryCheckable(BinaryProcessorsDecoder,r,true);

        if(m_BinaryOtherDecoders[r].Tip!=NULL)
        {
            UISetListViewEntryToolTip(BinaryProcessorsDecoder,r,
                    m_BinaryOtherDecoders[r].Tip);
        }
    }

    SysColPreset=UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox_SysCol_Preset);
    UIClearComboBox(SysColPreset);
    UIAddItem2ComboBox(SysColPreset,"",e_SysColPresetMAX);
    UIAddItem2ComboBox(SysColPreset,WHIPPYTERM_NAME,e_SysColPreset_WhippyTerm);
    UIAddItem2ComboBox(SysColPreset,"VGA",e_SysColPreset_VGA);
    UIAddItem2ComboBox(SysColPreset,"CMD",e_SysColPreset_CMD);
    UIAddItem2ComboBox(SysColPreset,"Terminal.app",e_SysColPreset_TerminalApp);
    UIAddItem2ComboBox(SysColPreset,"PuTTY",e_SysColPreset_PuTTY);
    UIAddItem2ComboBox(SysColPreset,"mIRC",e_SysColPreset_mIRC);
    UIAddItem2ComboBox(SysColPreset,"xterm",e_SysColPreset_xterm);
    UIAddItem2ComboBox(SysColPreset,"Ubuntu",e_SysColPreset_Ubuntu);

    SysColShade=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysCol_Shade_Normal);
    UISelectRadioBttn(SysColShade);
    SysColPrev=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysColPrev_Black);
    UISelectRadioBttn(SysColPrev);

    RedScroll=UIS_GetScrollBarHandle(e_UIS_ScrollBar_SysCol_RedScroll);
    UISetScrollBarTotalSize(RedScroll,255+10);
    UISetScrollBarPageSize(RedScroll,10);
    GreenScroll=UIS_GetScrollBarHandle(e_UIS_ScrollBar_SysCol_GreenScroll);
    UISetScrollBarTotalSize(GreenScroll,255+10);
    UISetScrollBarPageSize(GreenScroll,10);
    BlueScroll=UIS_GetScrollBarHandle(e_UIS_ScrollBar_SysCol_BlueScroll);
    UISetScrollBarTotalSize(BlueScroll,255+10);
    UISetScrollBarPageSize(BlueScroll,10);

    RedInput=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_SysCol_RedInput);
    UISetNumberInputCtrlMin(RedInput,0);
    UISetNumberInputCtrlMax(RedInput,255);
    GreenInput=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_SysCol_GreenInput);
    UISetNumberInputCtrlMin(GreenInput,0);
    UISetNumberInputCtrlMax(GreenInput,255);
    BlueInput=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_SysCol_BlueInput);
    UISetNumberInputCtrlMin(BlueInput,0);
    UISetNumberInputCtrlMax(BlueInput,255);

    /* Key bindings */
    KeyBindingsCmdList=UIS_GetListViewHandle(e_UIS_ListView_KeyBinding_CommandList);
    UIClearListView(KeyBindingsCmdList);
    for(r=0;r<e_CmdMAX;r++)
        UIAddItem2ListView(KeyBindingsCmdList,GetCmdName((e_CmdType)r),r);

//    UIAddItem2ComboBox(WindowStartupPos,"None",e_WindowStartupPos_OSDefault);

    TerminalTabCtrl=UIS_GetTabCtrlHandle(e_UIS_TabCtrl_Terminal);
    DisplayTabCtrl=UIS_GetTabCtrlHandle(e_UIS_TabCtrl_Display);

    /* Hide anything we can't set if we are in Con Settings Only */
    if(m_SettingConSettingsOnly)
    {
        /* Hide global only items */
        Display_Tabs=UIS_GetGroupBoxHandle(e_UIS_GroupBox_Display_Tabs);
        Display_ClearScreen=UIS_GetGroupBoxHandle(e_UIS_GroupBox_Display_ClearScreen);
        Display_MouseCursor=UIS_GetGroupBoxHandle(e_UIS_GroupBox_Display_MouseCursor);

        UITabCtrlSetTabVisibleByIndex(TerminalTabCtrl,
                e_UIS_TabCtrl_Terminal_Page_KeyBinding,false);
        UITabCtrlSetTabVisibleByIndex(DisplayTabCtrl,
                e_UIS_TabCtrl_Display_Page_HexDumps,false);

        UIGroupBoxVisible(Display_Tabs,false);
        UIGroupBoxVisible(Display_ClearScreen,false);
        UIGroupBoxVisible(Display_MouseCursor,false);

        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_AutoConnectOnNewConnection);
        UICheckboxVisible(CheckboxHandle,false);
    }

    /* Load settings into UI */
    DS_SetSettingGUI();

    DS_RethinkGUI();

    SelectTerminalPage=e_UIS_TabCtrl_Terminal_Page_Terminal;
    SelectDisplayPage=e_UIS_TabCtrl_Display_Page_Display;
    switch(Jump2)
    {
        case e_SettingsJump2_TermSize:
            FirstSelectedArea=e_DS_SettingsArea_Terminal;
            SelectTerminalPage=e_UIS_TabCtrl_Terminal_Page_Terminal;
        break;
        case e_SettingsJump2_TermEmu:
            FirstSelectedArea=e_DS_SettingsArea_Terminal;
            SelectTerminalPage=e_UIS_TabCtrl_Terminal_Page_DataProcessing;
        break;
        case e_SettingsJump2_Font:
            FirstSelectedArea=e_DS_SettingsArea_Display;
            SelectDisplayPage=e_UIS_TabCtrl_Display_Page_Display;
        break;
        case e_SettingsJump2_Colors:
            FirstSelectedArea=e_DS_SettingsArea_Display;
            SelectDisplayPage=e_UIS_TabCtrl_Display_Page_Colors;
        break;
        case e_SettingsJump2_ClipboardHandling:
            FirstSelectedArea=e_DS_SettingsArea_Terminal;
            SelectTerminalPage=e_UIS_TabCtrl_Terminal_Page_Keyboard;
        break;
        case e_SettingsJump2_Default:
        case e_SettingsJump2MAX:
        default:
            if(m_SettingConSettingsOnly)
                FirstSelectedArea=e_DS_SettingsArea_Connections;
            else
                FirstSelectedArea=e_DS_SettingsArea_Behaviour;
        break;
    }

    UITabCtrlMakeTabActiveUsingIndex(TerminalTabCtrl,SelectTerminalPage);
    UITabCtrlMakeTabActiveUsingIndex(DisplayTabCtrl,SelectDisplayPage);

    UISetListViewSelectedEntry(AreaList,FirstSelectedArea);
    UIS_HandleAreaChanged(FirstSelectedArea);
    UIS_HandleSysColPresetChange(e_SysColPresetMAX);

    RetValue=false;
    if(UIS_Show_Settings())
    {
        /* Save */
        RetValue=true;

        DS_GetSettingsFromGUI();
        DPS_PrunePluginSettings(m_SettingConSettings);

        if(!m_SettingConSettingsOnly)
        {
            while(!SaveSettings())
            {
                if(UIAsk("Error saving settings","Failed to save the setting.",
                        e_AskBox_Error,e_AskBttns_RetryCancel)==e_AskRet_Cancel)
                {
                    break;
                }
            }
            ApplySettings();
        }
    }

    if(m_DS_ConList!=NULL)
    {
        if(m_DS_ConOptions!=NULL)
            IOS_FreeConnectionOptions(m_DS_ConOptions);
        IOS_FreeListOfAvailableConnections(m_DS_ConList);
    }

    UIS_Free_Settings();

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    DS_SetSettingGUI
 *
 * SYNOPSIS:
 *    static void DS_SetSettingGUI(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function reads the settings out of the settings system and
 *    fills in the GUI.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DS_SetSettingGUI(void)
{
    unsigned int r;
    t_UICheckboxCtrl *CheckboxHandle;
    t_UIComboBoxCtrl *ComboBoxHandle;
    t_UINumberInput *NumberInputHandle;
    t_UIListViewCtrl *ListViewHandle;
    t_UIRadioBttnCtrl *ClearScreen_Clear;
    t_UIRadioBttnCtrl *ClearScreen_Scroll;
    t_UIRadioBttnCtrl *ClearScreen_ScrollAll;
    t_UIRadioBttnCtrl *ClearScreen_ScrollWithHr;
    t_UITextInputCtrl *TxtHandle;
    t_UIColorPreviewCtrl *ColorPreviewHandle;
    i_StringListType CurStr;
    t_UIRadioBttnCtrl *SysBell_None;
    t_UIRadioBttnCtrl *SysBell_System;
    t_UIRadioBttnCtrl *SysBell_BuiltIn;
    t_UIRadioBttnCtrl *SysBell_AudioOnly;
    t_UIRadioBttnCtrl *SysBell_VisualOnly;

    /********************/
    /* Behaviour        */
    /********************/
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_BookmarksOpenNewTab);
    UICheckCheckbox(CheckboxHandle,g_Settings.BookmarksOpenNewTabs);

    /********************/
    /* Panels           */
    /********************/

    /* Left panel */
    /* RestoreFromSettings */
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_LeftPanel_RestoreFromSettings);
    UICheckCheckbox(CheckboxHandle,g_Settings.LeftPanelFromSettings);
    /* ShowPanelOnStartup */
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_LeftPanel_ShowPanelOnStartup);
    UICheckCheckbox(CheckboxHandle,g_Settings.LeftPanelOpenOnStartup);
    /* AutoHidePanel */
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_LeftPanel_AutoHidePanel);
    UICheckCheckbox(CheckboxHandle,g_Settings.LeftPanelAutoHide);

    /* Right panel */
    /* RestoreFromSettings */
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_RightPanel_RestoreFromSettings);
    UICheckCheckbox(CheckboxHandle,g_Settings.RightPanelFromSettings);
    /* ShowPanelOnStartup */
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_RightPanel_ShowPanelOnStartup);
    UICheckCheckbox(CheckboxHandle,g_Settings.RightPanelOpenOnStartup);
    /* AutoHidePanel */
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_RightPanel_AutoHidePanel);
    UICheckCheckbox(CheckboxHandle,g_Settings.RightPanelAutoHide);

    /* Bottom panel */
    /* RestoreFromSettings */
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_BottomPanel_RestoreFromSettings);
    UICheckCheckbox(CheckboxHandle,g_Settings.BottomPanelFromSettings);
    /* ShowPanelOnStartup */
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_BottomPanel_ShowPanelOnStartup);
    UICheckCheckbox(CheckboxHandle,g_Settings.BottomPanelOpenOnStartup);
    /* AutoHidePanel */
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_BottomPanel_AutoHidePanel);
    UICheckCheckbox(CheckboxHandle,g_Settings.BottomPanelAutoHide);

    /* Stop Watch */
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_StopWatchAutoLap);
    UICheckCheckbox(CheckboxHandle,g_Settings.StopWatchAutoLap);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_StopWatchAutoStart);
    UICheckCheckbox(CheckboxHandle,g_Settings.StopWatchAutoStart);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_StopWatchShowPanel);
    UICheckCheckbox(CheckboxHandle,g_Settings.StopWatchShowPanel);

    /* Capture */
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_CaptureTimestamp);
    UICheckCheckbox(CheckboxHandle,g_Settings.CaptureTimestamp);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_CaptureAppend);
    UICheckCheckbox(CheckboxHandle,g_Settings.CaptureAppend);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_CaptureStripCtrl);
    UICheckCheckbox(CheckboxHandle,g_Settings.CaptureStripCtrl);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_CaptureStripEsc);
    UICheckCheckbox(CheckboxHandle,g_Settings.CaptureStripEsc);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_CaptureHexDump);
    UICheckCheckbox(CheckboxHandle,g_Settings.CaptureHexDump);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_CaptureShowPanel);
    UICheckCheckbox(CheckboxHandle,g_Settings.CaptureShowPanel);
    TxtHandle=UIS_GetTextInputHandle(e_UIS_TextInput_Capture_DefaultFilename);
    UISetTextCtrlText(TxtHandle,g_Settings.CaptureDefaultFilename.c_str());

    /* Hex Display */
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_HexDisplayEnabled);
    UICheckCheckbox(CheckboxHandle,g_Settings.HexDisplayEnabled);
    NumberInputHandle=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_HexDisplay_BufferSize);
    UISetNumberInputCtrlValue(NumberInputHandle,g_Settings.HexDisplayBufferSize);

    m_HexDisplaysFGColor=g_Settings.HexDisplaysFGColor;
    ColorPreviewHandle=UIS_GetColorPreviewHandle(e_UIS_ColorPreview_HexDisplay_FGDisplay);
    UISetColorPreviewColor(ColorPreviewHandle,m_HexDisplaysFGColor);

    m_HexDisplaysBGColor=g_Settings.HexDisplaysBGColor;
    ColorPreviewHandle=UIS_GetColorPreviewHandle(e_UIS_ColorPreview_HexDisplay_BGDisplay);
    UISetColorPreviewColor(ColorPreviewHandle,m_HexDisplaysBGColor);

    m_HexDisplaysSelBGColor=g_Settings.HexDisplaysSelBGColor;
    ColorPreviewHandle=UIS_GetColorPreviewHandle(e_UIS_ColorPreview_HexDisplay_SelBGDisplay);
    UISetColorPreviewColor(ColorPreviewHandle,m_HexDisplaysSelBGColor);

    /********************/
    /* Startup          */
    /********************/
    ComboBoxHandle=UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox_WindowStartupPos);
    UISetComboBoxSelectedEntry(ComboBoxHandle,g_Settings.RestoreWindowPos);

    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_StartMax);
    UICheckCheckbox(CheckboxHandle,g_Settings.AppMaximized);

    /********************/
    /* Connections      */
    /********************/
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_AutoConnectOnNewConnection);
    UICheckCheckbox(CheckboxHandle,g_Settings.AutoConnectOnNewConnection);

    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_AutoReopen);
    UICheckCheckbox(CheckboxHandle,m_SettingConSettings->AutoReopen);

    NumberInputHandle=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_AutoReopenWaitTime);
    UISetNumberInputCtrlValue(NumberInputHandle,m_SettingConSettings->AutoReopenWaitTime);

    NumberInputHandle=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_DelayBetweenBytes);
    UISetNumberInputCtrlValue(NumberInputHandle,m_SettingConSettings->DelayBetweenBytes);
    NumberInputHandle=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_DelayAfterNewLineSent);
    UISetNumberInputCtrlValue(NumberInputHandle,m_SettingConSettings->DelayAfterNewLineSent);

    /********************/
    /* Display          */
    /********************/
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_AlwaysShowTabs);
    UICheckCheckbox(CheckboxHandle,g_Settings.AlwaysShowTabs);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_CloseButtonOnTabs);
    UICheckCheckbox(CheckboxHandle,g_Settings.CloseButtonOnTabs);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_CursorBlink);
    UICheckCheckbox(CheckboxHandle,m_SettingConSettings->CursorBlink);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_MouseCursorUseIBeam);
    UICheckCheckbox(CheckboxHandle,g_Settings.MouseCursorIBeam);

    m_CursorColor=m_SettingConSettings->CursorColor;
    ColorPreviewHandle=UIS_GetColorPreviewHandle(e_UIS_ColorPreview_CursorColor);
    UISetColorPreviewColor(ColorPreviewHandle,m_CursorColor);

    /* Attributes */
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_BoldFontEnable);
    UICheckCheckbox(CheckboxHandle,m_SettingConSettings->BoldEnabled);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_ItalicFontEnable);
    UICheckCheckbox(CheckboxHandle,m_SettingConSettings->ItalicEnabled);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_UnderlineEnable);
    UICheckCheckbox(CheckboxHandle,m_SettingConSettings->UnderlineEnabled);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_OverlineEnable);
    UICheckCheckbox(CheckboxHandle,m_SettingConSettings->OverlineEnabled);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_ReverseEnable);
    UICheckCheckbox(CheckboxHandle,m_SettingConSettings->ReverseEnabled);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_StrikeThroughEnable);
    UICheckCheckbox(CheckboxHandle,m_SettingConSettings->LineThroughEnabled);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_ColorEnable);
    UICheckCheckbox(CheckboxHandle,m_SettingConSettings->ColorsEnabled);

    memcpy(m_DS_SysColors,m_SettingConSettings->SysColors,sizeof(m_DS_SysColors));
    memcpy(m_DS_DefaultColors,m_SettingConSettings->DefaultColors,sizeof(m_DS_DefaultColors));
    memcpy(m_DS_SelectionColors,m_SettingConSettings->SelectionColors,sizeof(m_DS_SelectionColors));

    DS_RethinkDisplayDisplay();
    DS_RethinkHexDisplayDisplay();

    /********************/
    /* Terminal         */
    /********************/
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_TerminalSize_FixedWidth);
    UICheckCheckbox(CheckboxHandle,m_SettingConSettings->TermSizeFixedWidth);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_TerminalSize_FixedHeight);
    UICheckCheckbox(CheckboxHandle,m_SettingConSettings->TermSizeFixedHeight);
    NumberInputHandle=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_TermSizeWidth);
    UISetNumberInputCtrlValue(NumberInputHandle,m_SettingConSettings->TermSizeWidth);
    NumberInputHandle=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_TermSizeHeight);
    UISetNumberInputCtrlValue(NumberInputHandle,m_SettingConSettings->TermSizeHeight);
    NumberInputHandle=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_ScrollBufferLines);
    UISetNumberInputCtrlValue(NumberInputHandle,m_SettingConSettings->ScrollBufferLines);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_CenterTextInWindow);
    UICheckCheckbox(CheckboxHandle,m_SettingConSettings->CenterTextInWindow);

    ClearScreen_Clear=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Display_ClearScreen_Clear);
    ClearScreen_Scroll=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Display_ClearScreen_Scroll);
    ClearScreen_ScrollAll=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Display_ClearScreen_ScrollAll);
    ClearScreen_ScrollWithHr=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Display_ClearScreen_ScrollWithHR);
    switch(g_Settings.ScreenClear)
    {
        case e_ScreenClear_Clear:
        case e_ScreenClearMAX:
            UISelectRadioBttn(ClearScreen_Clear);
        break;
        case e_ScreenClear_Scroll:
            UISelectRadioBttn(ClearScreen_Scroll);
        break;
        case e_ScreenClear_ScrollAll:
            UISelectRadioBttn(ClearScreen_ScrollAll);
        break;
        case e_ScreenClear_ScrollWithHR:
            UISelectRadioBttn(ClearScreen_ScrollWithHr);
        break;
        default:
        break;
    }

    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_AutoCROnLF);
    UICheckCheckbox(CheckboxHandle,m_SettingConSettings->AutoCROnLF);

    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_LocalEcho);
    UICheckCheckbox(CheckboxHandle,m_SettingConSettings->LocalEcho);

    /* Keyboard */
    DS_SetKeyboardRadioBttns();
    DS_RethinkTerminalDisplay();

    /* Sounds */
    SysBell_None=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysBell_None);
    SysBell_System=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysBell_System);
    SysBell_BuiltIn=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysBell_BuiltIn);
    SysBell_AudioOnly=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysBell_AudioOnly);
    SysBell_VisualOnly=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysBell_VisualOnly);
    switch(m_SettingConSettings->BeepMode)
    {
        case e_Beep_None:
            UISelectRadioBttn(SysBell_None);
        break;
        default:
        case e_BeepMAX:
        case e_Beep_System:
            UISelectRadioBttn(SysBell_System);
        break;
        case e_Beep_BuiltIn:
            UISelectRadioBttn(SysBell_BuiltIn);
        break;
        case e_Beep_AudioOnly:
            UISelectRadioBttn(SysBell_AudioOnly);
        break;
        case e_Beep_VisualOnly:
            UISelectRadioBttn(SysBell_VisualOnly);
        break;
    }
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_UseCustomSounds);
    UICheckCheckbox(CheckboxHandle,m_SettingConSettings->UseCustomSound);

    TxtHandle=UIS_GetTextInputHandle(e_UIS_TextInput_CustomSoundFilename);
    UISetTextCtrlText(TxtHandle,m_SettingConSettings->BeepFilename.c_str());

    /*******************/
    /* Data Processors */
    /*******************/
    ComboBoxHandle=UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox_DataProcessor);
    UISetComboBoxSelectedEntry(ComboBoxHandle,m_SettingConSettings->DataProcessorType);

    for(CurStr=m_SettingConSettings->EnabledTextDataProcessors.begin();
            CurStr!=m_SettingConSettings->EnabledTextDataProcessors.end();CurStr++)
    {
        /* Char Enc */
        for(r=0;r<m_CharEncodingInputPros.size();r++)
        {
            if(m_CharEncodingInputPros[r].IDStr==NULL)
                continue;

            if(strcmp(m_CharEncodingInputPros[r].IDStr,CurStr->c_str())==0)
            {
                /* Found it */
                ComboBoxHandle=UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox_TextProCharEnc);
                UISetComboBoxSelectedEntry(ComboBoxHandle,r);
                break;
            }
        }

        /* Highlighter */
        for(r=0;r<m_HighlighterInputPros.size();r++)
        {
            if(strcmp(m_HighlighterInputPros[r].IDStr,CurStr->c_str())==0)
            {
                /* Found it */
                ListViewHandle=UIS_GetListViewHandle(e_UIS_ListView_InputProTextHighlight);
                UISetListViewEntryCheckedState(ListViewHandle,r,true);
                break;
            }
        }

        /* Other */
        for(r=0;r<m_OtherInputPros.size();r++)
        {
            if(strcmp(m_OtherInputPros[r].IDStr,CurStr->c_str())==0)
            {
                /* Found it */
                ListViewHandle=UIS_GetListViewHandle(e_UIS_ListView_InputProTextOther);
                UISetListViewEntryCheckedState(ListViewHandle,r,true);
                break;
            }
        }
    }

    for(CurStr=m_SettingConSettings->EnabledTermEmuDataProcessors.begin();
            CurStr!=m_SettingConSettings->EnabledTermEmuDataProcessors.end();CurStr++)
    {
        /* Term Emulation */
        for(r=0;r<m_TermEmulationInputPros.size();r++)
        {
            if(m_TermEmulationInputPros[r].IDStr==NULL)
                continue;

            if(strcmp(m_TermEmulationInputPros[r].IDStr,CurStr->c_str())==0)
            {
                /* Found it */
                ComboBoxHandle=UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox_TextProTermEmu);
                UISetComboBoxSelectedEntry(ComboBoxHandle,r);
                break;
            }
        }
    }

    for(CurStr=m_SettingConSettings->EnabledBinaryDataProcessors.begin();
            CurStr!=m_SettingConSettings->EnabledBinaryDataProcessors.end();
            CurStr++)
    {
        for(r=0;r<m_BinaryDecoders.size();r++)
        {
            if(strcmp(m_BinaryDecoders[r].IDStr,CurStr->c_str())==0)
            {
                /* Found it */
                ListViewHandle=UIS_GetListViewHandle(e_UIS_ListView_BinaryProcessorDecoder);
                UISetListViewSelectedEntry(ListViewHandle,r);
                break;
            }
        }

        for(r=0;r<m_BinaryOtherDecoders.size();r++)
        {
            if(strcmp(m_BinaryOtherDecoders[r].IDStr,CurStr->c_str())==0)
            {
                /* Found it */
                ListViewHandle=UIS_GetListViewHandle(e_UIS_ListView_BinaryProcessorOther);
                UISetListViewEntryCheckedState(ListViewHandle,r,true);
                break;
            }
        }
    }
}

static void DS_RethinkTerminalDisplay(void)
{
    t_UICheckboxCtrl *FixedWidthHandle;
    t_UICheckboxCtrl *FixedHeightHandle;
    t_UICheckboxCtrl *CenterTextInWindowHandle;
    t_UINumberInput *Width;
    t_UINumberInput *Height;
    bool CenterTextInWindowEnable;
    bool Enable;

    FixedWidthHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_TerminalSize_FixedWidth);
    FixedHeightHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_TerminalSize_FixedHeight);
    CenterTextInWindowHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_CenterTextInWindow);
    Width=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_TermSizeWidth);
    Height=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_TermSizeHeight);

    CenterTextInWindowEnable=true;
    if(!UIGetCheckboxCheckStatus(FixedWidthHandle) &&
            !UIGetCheckboxCheckStatus(FixedHeightHandle))
    {
        CenterTextInWindowEnable=false;
    }
    UIEnableCheckbox(CenterTextInWindowHandle,CenterTextInWindowEnable);

    Enable=true;
    if(!UIGetCheckboxCheckStatus(FixedWidthHandle))
        Enable=false;
    UIEnableNumberInputCtrl(Width,Enable);

    Enable=true;
    if(!UIGetCheckboxCheckStatus(FixedHeightHandle))
        Enable=false;
    UIEnableNumberInputCtrl(Height,Enable);
}

static void DS_RethinkDisplayDisplay(void)
{
    t_UILabelCtrl *FontLabelCtrl;
    string FontLabel;
    char buff[100];

    FontLabelCtrl=UIS_GetLabelHandle(e_UIS_Labels_CurrentFont);

    sprintf(buff,"%d",m_SettingConSettings->FontSize);
    FontLabel=m_SettingConSettings->FontName;
    FontLabel+=" / ";
    FontLabel+=buff;
    UISetLabelText(FontLabelCtrl,FontLabel.c_str());

    DS_CopySysColors2GUI();
}

static void DS_RethinkHexDisplayDisplay(void)
{
    t_UILabelCtrl *FontLabelCtrl;
    string FontLabel;
    char buff[100];

    FontLabelCtrl=UIS_GetLabelHandle(e_UIS_Labels_HexDisplayCurrentFont);

    sprintf(buff,"%d",g_Settings.HexDisplaysFontSize);
    FontLabel=g_Settings.HexDisplaysFontName;
    FontLabel+=" / ";
    FontLabel+=buff;
    UISetLabelText(FontLabelCtrl,FontLabel.c_str());

    DS_CopySysColors2GUI();
}

static void DS_GetSettingsFromGUI(void)
{
    t_UICheckboxCtrl *CheckboxHandle;
    t_UIComboBoxCtrl *ComboBoxHandle;
    t_UINumberInput *NumberInputHandle;
    t_UIListViewCtrl *ListViewHandle;
    t_UIRadioBttnCtrl *RadioHandle;
    t_UITextInputCtrl *TxtHandle;
    uintptr_t ID;

    if(!m_SettingConSettingsOnly)
    {
        /* Global settings only */

        /********************/
        /* Behaviour        */
        /********************/
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_BookmarksOpenNewTab);
        g_Settings.BookmarksOpenNewTabs=UIGetCheckboxCheckStatus(CheckboxHandle);

        /********************/
        /* Panels           */
        /********************/
        /* Left panel */
        /* RestoreFromSettings */
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_LeftPanel_RestoreFromSettings);
        g_Settings.LeftPanelFromSettings=UIGetCheckboxCheckStatus(CheckboxHandle);
        /* ShowPanelOnStartup */
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_LeftPanel_ShowPanelOnStartup);
        g_Settings.LeftPanelOpenOnStartup=UIGetCheckboxCheckStatus(CheckboxHandle);
        /* AutoHidePanel */
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_LeftPanel_AutoHidePanel);
        g_Settings.LeftPanelAutoHide=UIGetCheckboxCheckStatus(CheckboxHandle);

        /* Right panel */
        /* RestoreFromSettings */
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_RightPanel_RestoreFromSettings);
        g_Settings.RightPanelFromSettings=UIGetCheckboxCheckStatus(CheckboxHandle);
        /* ShowPanelOnStartup */
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_RightPanel_ShowPanelOnStartup);
        g_Settings.RightPanelOpenOnStartup=UIGetCheckboxCheckStatus(CheckboxHandle);
        /* AutoHidePanel */
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_RightPanel_AutoHidePanel);
        g_Settings.RightPanelAutoHide=UIGetCheckboxCheckStatus(CheckboxHandle);

        /* Bottom panel */
        /* RestoreFromSettings */
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_BottomPanel_RestoreFromSettings);
        g_Settings.BottomPanelFromSettings=UIGetCheckboxCheckStatus(CheckboxHandle);
        /* ShowPanelOnStartup */
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_BottomPanel_ShowPanelOnStartup);
        g_Settings.BottomPanelOpenOnStartup=UIGetCheckboxCheckStatus(CheckboxHandle);
        /* AutoHidePanel */
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_BottomPanel_AutoHidePanel);
        g_Settings.BottomPanelAutoHide=UIGetCheckboxCheckStatus(CheckboxHandle);

        /* Stop Watch */
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_StopWatchAutoLap);
        g_Settings.StopWatchAutoLap=UIGetCheckboxCheckStatus(CheckboxHandle);
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_StopWatchAutoStart);
        g_Settings.StopWatchAutoStart=UIGetCheckboxCheckStatus(CheckboxHandle);
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_StopWatchShowPanel);
        g_Settings.StopWatchShowPanel=UIGetCheckboxCheckStatus(CheckboxHandle);

        /* Capture */
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_CaptureTimestamp);
        g_Settings.CaptureTimestamp=UIGetCheckboxCheckStatus(CheckboxHandle);
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_CaptureAppend);
        g_Settings.CaptureAppend=UIGetCheckboxCheckStatus(CheckboxHandle);
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_CaptureStripCtrl);
        g_Settings.CaptureStripCtrl=UIGetCheckboxCheckStatus(CheckboxHandle);
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_CaptureStripEsc);
        g_Settings.CaptureStripEsc=UIGetCheckboxCheckStatus(CheckboxHandle);
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_CaptureHexDump);
        g_Settings.CaptureHexDump=UIGetCheckboxCheckStatus(CheckboxHandle);
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_CaptureShowPanel);
        g_Settings.CaptureShowPanel=UIGetCheckboxCheckStatus(CheckboxHandle);
        TxtHandle=UIS_GetTextInputHandle(e_UIS_TextInput_Capture_DefaultFilename);
        UIGetTextCtrlText(TxtHandle,g_Settings.CaptureDefaultFilename);

        /* Hex Display */
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_HexDisplayEnabled);
        g_Settings.HexDisplayEnabled=UIGetCheckboxCheckStatus(CheckboxHandle);
        NumberInputHandle=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_HexDisplay_BufferSize);
        g_Settings.HexDisplayBufferSize=UIGetNumberInputCtrlValue(NumberInputHandle);

        g_Settings.HexDisplaysFGColor=m_HexDisplaysFGColor;
        g_Settings.HexDisplaysBGColor=m_HexDisplaysBGColor;
        g_Settings.HexDisplaysSelBGColor=m_HexDisplaysSelBGColor;

        /********************/
        /* Startup          */
        /********************/
        ComboBoxHandle=UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox_WindowStartupPos);
        g_Settings.RestoreWindowPos=(e_WindowStartupPosType)
                UIGetComboBoxSelectedEntry(ComboBoxHandle);

        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_StartMax);
        g_Settings.AppMaximized=UIGetCheckboxCheckStatus(CheckboxHandle);

        /********************/
        /* Connections      */
        /********************/
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_AutoConnectOnNewConnection);
        g_Settings.AutoConnectOnNewConnection=UIGetCheckboxCheckStatus(CheckboxHandle);

        /********************/
        /* Display          */
        /********************/
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_AlwaysShowTabs);
        g_Settings.AlwaysShowTabs=UIGetCheckboxCheckStatus(CheckboxHandle);
        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_CloseButtonOnTabs);
        g_Settings.CloseButtonOnTabs=UIGetCheckboxCheckStatus(CheckboxHandle);

        CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_MouseCursorUseIBeam);
        g_Settings.MouseCursorIBeam=UIGetCheckboxCheckStatus(CheckboxHandle);

        /* Keyboard */
        RadioHandle=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Display_ClearScreen_Clear);
        if(UIIsRadioBttnSelected(RadioHandle))
            g_Settings.ScreenClear=e_ScreenClear_Clear;
        RadioHandle=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Display_ClearScreen_Scroll);
        if(UIIsRadioBttnSelected(RadioHandle))
            g_Settings.ScreenClear=e_ScreenClear_Scroll;
        RadioHandle=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Display_ClearScreen_ScrollAll);
        if(UIIsRadioBttnSelected(RadioHandle))
            g_Settings.ScreenClear=e_ScreenClear_ScrollAll;
        RadioHandle=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Display_ClearScreen_ScrollWithHR);
        if(UIIsRadioBttnSelected(RadioHandle))
            g_Settings.ScreenClear=e_ScreenClear_ScrollWithHR;

        /* KeyBindings */
        memcpy(&g_Settings.KeyMapping,&m_CopyOfKeyMapping,
                sizeof(m_CopyOfKeyMapping));
    }

    /* Common to global settings and connections */

    /********************/
    /* Connections      */
    /********************/
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_AutoReopen);
    m_SettingConSettings->AutoReopen=UIGetCheckboxCheckStatus(CheckboxHandle);

    NumberInputHandle=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_AutoReopenWaitTime);
    m_SettingConSettings->AutoReopenWaitTime=UIGetNumberInputCtrlValue(NumberInputHandle);

    NumberInputHandle=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_DelayBetweenBytes);
    m_SettingConSettings->DelayBetweenBytes=UIGetNumberInputCtrlValue(NumberInputHandle);
    NumberInputHandle=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_DelayAfterNewLineSent);
    m_SettingConSettings->DelayAfterNewLineSent=UIGetNumberInputCtrlValue(NumberInputHandle);

    /********************/
    /* Display          */
    /********************/
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_CursorBlink);
    m_SettingConSettings->CursorBlink=UIGetCheckboxCheckStatus(CheckboxHandle);
    m_SettingConSettings->CursorColor=m_CursorColor;

    /* Attributes */
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_BoldFontEnable);
    m_SettingConSettings->BoldEnabled=UIGetCheckboxCheckStatus(CheckboxHandle);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_ItalicFontEnable);
    m_SettingConSettings->ItalicEnabled=UIGetCheckboxCheckStatus(CheckboxHandle);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_UnderlineEnable);
    m_SettingConSettings->UnderlineEnabled=UIGetCheckboxCheckStatus(CheckboxHandle);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_OverlineEnable);
    m_SettingConSettings->OverlineEnabled=UIGetCheckboxCheckStatus(CheckboxHandle);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_ReverseEnable);
    m_SettingConSettings->ReverseEnabled=UIGetCheckboxCheckStatus(CheckboxHandle);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_StrikeThroughEnable);
    m_SettingConSettings->LineThroughEnabled=UIGetCheckboxCheckStatus(CheckboxHandle);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_ColorEnable);
    m_SettingConSettings->ColorsEnabled=UIGetCheckboxCheckStatus(CheckboxHandle);

    /********************/
    /* Terminal         */
    /********************/
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_TerminalSize_FixedWidth);
    m_SettingConSettings->TermSizeFixedWidth=UIGetCheckboxCheckStatus(CheckboxHandle);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_TerminalSize_FixedHeight);
    m_SettingConSettings->TermSizeFixedHeight=UIGetCheckboxCheckStatus(CheckboxHandle);
    NumberInputHandle=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_TermSizeWidth);
    m_SettingConSettings->TermSizeWidth=UIGetNumberInputCtrlValue(NumberInputHandle);
    NumberInputHandle=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_TermSizeHeight);
    m_SettingConSettings->TermSizeHeight=UIGetNumberInputCtrlValue(NumberInputHandle);
    NumberInputHandle=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_ScrollBufferLines);
    m_SettingConSettings->ScrollBufferLines=UIGetNumberInputCtrlValue(NumberInputHandle);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_CenterTextInWindow);
    m_SettingConSettings->CenterTextInWindow=UIGetCheckboxCheckStatus(CheckboxHandle);

    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_AutoCROnLF);
    m_SettingConSettings->AutoCROnLF=UIGetCheckboxCheckStatus(CheckboxHandle);
    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_LocalEcho);
    m_SettingConSettings->LocalEcho=UIGetCheckboxCheckStatus(CheckboxHandle);

    /* Keyboard */
    DS_GetSettingsFromGUI_KeyboardRadioBttns();

    /* Sounds */
    RadioHandle=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysBell_None);
    if(UIIsRadioBttnSelected(RadioHandle))
        m_SettingConSettings->BeepMode=e_Beep_None;
    RadioHandle=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysBell_System);
    if(UIIsRadioBttnSelected(RadioHandle))
        m_SettingConSettings->BeepMode=e_Beep_System;
    RadioHandle=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysBell_BuiltIn);
    if(UIIsRadioBttnSelected(RadioHandle))
        m_SettingConSettings->BeepMode=e_Beep_BuiltIn;
    RadioHandle=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysBell_AudioOnly);
    if(UIIsRadioBttnSelected(RadioHandle))
        m_SettingConSettings->BeepMode=e_Beep_AudioOnly;
    RadioHandle=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysBell_VisualOnly);
    if(UIIsRadioBttnSelected(RadioHandle))
        m_SettingConSettings->BeepMode=e_Beep_VisualOnly;

    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_UseCustomSounds);
    m_SettingConSettings->UseCustomSound=UIGetCheckboxCheckStatus(CheckboxHandle);

    TxtHandle=UIS_GetTextInputHandle(e_UIS_TextInput_CustomSoundFilename);
    UIGetTextCtrlText(TxtHandle,m_SettingConSettings->BeepFilename);

    /*******************/
    /* Data Processors */
    /*******************/
    ComboBoxHandle=UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox_DataProcessor);
    m_SettingConSettings->DataProcessorType=(e_DataProcessorTypeType)
            UIGetComboBoxSelectedEntry(ComboBoxHandle);

    /* We use one master list of processors and then split it up for the GUI */
    m_SettingConSettings->EnabledTextDataProcessors.clear();

    /* Char Enc */
    ComboBoxHandle=UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox_TextProCharEnc);
    ID=UIGetComboBoxSelectedEntry(ComboBoxHandle);
    if(m_CharEncodingInputPros[ID].IDStr!=NULL)
    {
        m_SettingConSettings->EnabledTextDataProcessors.
                push_back(m_CharEncodingInputPros[ID].IDStr);
    }

    /* Highlighter */
    ListViewHandle=UIS_GetListViewHandle(e_UIS_ListView_InputProTextHighlight);
    for(ID=0;ID<m_HighlighterInputPros.size();ID++)
    {
        if(UIGetListViewEntryCheckedState(ListViewHandle,ID))
        {
            m_SettingConSettings->EnabledTextDataProcessors.
                    push_back(m_HighlighterInputPros[ID].IDStr);
        }
    }

    /* Other */
    ListViewHandle=UIS_GetListViewHandle(e_UIS_ListView_InputProTextOther);
    for(ID=0;ID<m_OtherInputPros.size();ID++)
    {
        if(UIGetListViewEntryCheckedState(ListViewHandle,ID))
        {
            m_SettingConSettings->EnabledTextDataProcessors.
                    push_back(m_OtherInputPros[ID].IDStr);
        }
    }

    /* Term Emulation */
    m_SettingConSettings->EnabledTermEmuDataProcessors.clear();
    ComboBoxHandle=UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox_TextProTermEmu);
    ID=UIGetComboBoxSelectedEntry(ComboBoxHandle);
    if(m_TermEmulationInputPros[ID].IDStr!=NULL)
    {
        m_SettingConSettings->EnabledTermEmuDataProcessors.
                push_back(m_TermEmulationInputPros[ID].IDStr);
    }

    /* Binary Data Processors */
    ListViewHandle=UIS_GetListViewHandle(e_UIS_ListView_BinaryProcessorDecoder);
    ID=UIGetListViewSelectedEntry(ListViewHandle);
    m_SettingConSettings->EnabledBinaryDataProcessors.clear();
    if(ID<m_BinaryDecoders.size())
    {
        m_SettingConSettings->EnabledBinaryDataProcessors.
                push_back(m_BinaryDecoders[ID].IDStr);
    }

    /* Binary Other */
    ListViewHandle=UIS_GetListViewHandle(e_UIS_ListView_BinaryProcessorOther);
    for(ID=0;ID<m_BinaryOtherDecoders.size();ID++)
    {
        if(UIGetListViewEntryCheckedState(ListViewHandle,ID))
        {
            m_SettingConSettings->EnabledBinaryDataProcessors.
                    push_back(m_BinaryOtherDecoders[ID].IDStr);
        }
    }

    /********************/
    /* Colors           */
    /********************/
    memcpy(m_SettingConSettings->SysColors,m_DS_SysColors,sizeof(m_DS_SysColors));
    memcpy(m_SettingConSettings->DefaultColors,m_DS_DefaultColors,sizeof(m_DS_DefaultColors));
    memcpy(m_SettingConSettings->SelectionColors,m_DS_SelectionColors,sizeof(m_DS_SelectionColors));
}

static void DS_RethinkGUI(void)
{
    t_UIButtonCtrl *ButtonHandle;
    t_UIListViewCtrl *ListViewHandle;
    t_UICheckboxCtrl *CheckboxHandle;
    t_UIRadioBttnCtrl *SysBell_BuiltIn;
    t_UIRadioBttnCtrl *SysBell_AudioOnly;
    t_UITextInputCtrl *TxtHandle;

    UIS_HandleLeftPanelAutoHideClick(g_Settings.LeftPanelAutoHide);
    UIS_HandleRightPanelAutoHideClick(g_Settings.RightPanelAutoHide);
    UIS_HandleBottomPanelAutoHideClick(g_Settings.BottomPanelAutoHide);
    UIS_HandleWindowStartPosChanged(g_Settings.RestoreWindowPos);

    UIS_HandleInputProcessingChange(m_SettingConSettings->DataProcessorType);
    UIS_HandleInputProcessingCharEncChange();
    UIS_HandleInputProcessingTermEmuChange();

    ListViewHandle=UIS_GetListViewHandle(e_UIS_ListView_InputProTextHighlight);
    UIClearListViewSelectedEntry(ListViewHandle);

    ListViewHandle=UIS_GetListViewHandle(e_UIS_ListView_InputProTextOther);
    UIClearListViewSelectedEntry(ListViewHandle);

    DS_UpdateColorPreview(true);

    SysBell_BuiltIn=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysBell_BuiltIn);
    SysBell_AudioOnly=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysBell_AudioOnly);

    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_UseCustomSounds);
    TxtHandle=UIS_GetTextInputHandle(e_UIS_TextInput_Capture_DefaultFilename);
    TxtHandle=UIS_GetTextInputHandle(e_UIS_TextInput_CustomSoundFilename);
    ButtonHandle=UIS_GetButtonHandle(e_UIS_Button_CustomSound_SelectFilename);
    if(UIIsRadioBttnSelected(SysBell_BuiltIn) ||
            UIIsRadioBttnSelected(SysBell_AudioOnly))
    {
        UIEnableCheckbox(CheckboxHandle,true);
        UIEnableTextCtrl(TxtHandle,true);
        UIEnableButton(ButtonHandle,true);
    }
    else
    {
        UIEnableCheckbox(CheckboxHandle,false);
        UIEnableTextCtrl(TxtHandle,false);
        UIEnableButton(ButtonHandle,false);
    }

    UIS_HandleInputProcessingHighlighterChange();
    UIS_HandleInputProcessingOtherChange();
    UIS_HandleInputProcessingCharEncChange();
    UIS_HandleInputProcessingTermEmuChange();
    UIS_HandleInputProcessingBinaryDecoderChange();
    UIS_HandleInputProcessingBinaryOtherChange();
}

/*******************************************************************************
 * NAME:
 *    UIS_HandleLeftPanelAutoHideClick
 *
 * SYNOPSIS:
 *    void UIS_HandleLeftPanelAutoHideClick(bool checked);
 *
 * PARAMETERS:
 *    checked [I] -- Is the input checked?
 *
 * FUNCTION:
 *    This function handles the left panel auto hide checkbox being clicked.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIS_HandleLeftPanelAutoHideClick(bool checked)
{
    t_UICheckboxCtrl *CheckboxHandle;

    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_LeftPanel_ShowPanelOnStartup);

    UIEnableCheckbox(CheckboxHandle,!checked);
}

/*******************************************************************************
 * NAME:
 *    UIS_HandleRightPanelAutoHideClick
 *
 * SYNOPSIS:
 *    void UIS_HandleRightPanelAutoHideClick(bool checked);
 *
 * PARAMETERS:
 *    checked [I] -- Is the input checked?
 *
 * FUNCTION:
 *    This function handles the right panel auto hide checkbox being clicked.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIS_HandleRightPanelAutoHideClick(bool checked)
{
    t_UICheckboxCtrl *CheckboxHandle;

    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_RightPanel_ShowPanelOnStartup);

    UIEnableCheckbox(CheckboxHandle,!checked);
}

/*******************************************************************************
 * NAME:
 *    UIS_HandleBottomPanelAutoHideClick
 *
 * SYNOPSIS:
 *    void UIS_HandleBottomPanelAutoHideClick(bool checked);
 *
 * PARAMETERS:
 *    checked [I] -- Is the input checked?
 *
 * FUNCTION:
 *    This function handles the bottom panel auto hide checkbox being clicked.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIS_HandleBottomPanelAutoHideClick(bool checked)
{
    t_UICheckboxCtrl *CheckboxHandle;

    CheckboxHandle=UIS_GetCheckboxHandle(e_UIS_Checkbox_BottomPanel_ShowPanelOnStartup);

    UIEnableCheckbox(CheckboxHandle,!checked);
}

/*******************************************************************************
 * NAME:
 *    UIS_HandleAreaChanged
 *
 * SYNOPSIS:
 *    void UIS_HandleAreaChanged(uintptr_t ID);
 *
 * PARAMETERS:
 *    ID [I] -- The new area that was selected
 *
 * FUNCTION:
 *    This function updates the UI when the area select input changes.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIS_HandleAreaChanged(uintptr_t ID)
{
    switch(ID)
    {
        case e_DS_SettingsArea_Panels:
            UIS_MakeTabVisable(e_UIS_TabCtrl_PanelTab);
        break;
        case e_DS_SettingsArea_Startup:
            UIS_MakeTabVisable(e_UIS_TabCtrl_StartupTab);
        break;
        case e_DS_SettingsArea_Connections:
            UIS_MakeTabVisable(e_UIS_TabCtrl_ConnectionsTab);
        break;
        case e_DS_SettingsArea_Display:
            UIS_MakeTabVisable(e_UIS_TabCtrl_Display);
        break;
        case e_DS_SettingsArea_Terminal:
            UIS_MakeTabVisable(e_UIS_TabCtrl_Terminal);
        break;
        case e_DS_SettingsArea_Behaviour:
            UIS_MakeTabVisable(e_UIS_TabCtrl_Behaviour);
        break;
        default:
        break;
    }
}

/*******************************************************************************
 * NAME:
 *    UIS_HandleKeyBindingsCmdListChange
 *
 * SYNOPSIS:
 *    void UIS_HandleKeyBindingsCmdListChange(uintptr_t ID);
 *
 * PARAMETERS:
 *    ID [I] -- The command ID that has been selected
 *
 * FUNCTION:
 *    This function updates the UI when the keyboard cmd list changes.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIS_HandleKeyBindingsCmdListChange(uintptr_t ID)
{
    e_CmdType Cmd;
    t_UITextInputCtrl *ShortcutInput;

    Cmd=(e_CmdType)ID;

    if(Cmd>=e_CmdMAX)
        return;

    ShortcutInput=UIS_GetTextInputHandle(e_UIS_TextInput_KeyBinding_Assigned2);
    UISetTextCtrlText(ShortcutInput,
            ConvertKeySeq2String(&m_CopyOfKeyMapping[Cmd]));

//const char *ConvertKeySeq2String(struct CommandKeySeq *KeySeq);
//void ConvertString2KeySeq(struct CommandKeySeq *KeySeq,const char *Str);
}

/*******************************************************************************
 * NAME:
 *    UIS_HandleKeyBindingsCmdShortCutChange
 *
 * SYNOPSIS:
 *    void UIS_HandleKeyBindingsCmdShortCutChange(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function converts a key seq string from the UI into a cmd and
 *    stores it in the currently selected key cmd list entry.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIS_HandleKeyBindingsCmdShortCutChange(void)
{
    struct CommandKeySeq KeySeq;
    t_UIListViewCtrl *KeyList;
    e_CmdType Cmd;
    t_UITextInputCtrl *ShortcutInput;
    char KeyNameBuffer[100];
    char buff[100];
    int r;

    ShortcutInput=UIS_GetTextInputHandle(e_UIS_TextInput_KeyBinding_Assigned2);
    KeyList=UIS_GetListViewHandle(e_UIS_ListView_KeyBinding_CommandList);
    if(ShortcutInput==NULL || KeyList==NULL)
        return;

    UIGetTextCtrlText(ShortcutInput,KeyNameBuffer,sizeof(KeyNameBuffer));

    if(!ConvertString2KeySeq(&KeySeq,KeyNameBuffer))
    {
        /* Invalid */
        UIAsk("Error getting key sequence",
                "The key sequence is not understood.",e_AskBox_Error,
                e_AskBttns_Ok);
        return;
    }

    /* Check if this key seq is already in use (only if it's not blank) */
    if(KeySeq.Key!=e_UIKeysMAX || KeySeq.Letter!=0)
    {
        for(r=0;r<e_CmdMAX;r++)
        {
            if(m_CopyOfKeyMapping[r].Mod==KeySeq.Mod &&
                    m_CopyOfKeyMapping[r].Key==KeySeq.Key &&
                    m_CopyOfKeyMapping[r].Letter==KeySeq.Letter)
            {
                sprintf(buff,"The key sequence is already used by %s.",
                        GetCmdName((e_CmdType)r));
                UIAsk("Key sequence already in use",buff,e_AskBox_Error,
                        e_AskBttns_Ok);
                return;
            }
        }
    }

    Cmd=(e_CmdType)UIGetListViewSelectedEntry(KeyList);
    if(Cmd>=e_CmdMAX)
        return;

    m_CopyOfKeyMapping[Cmd]=KeySeq;
}

/*******************************************************************************
 * NAME:
 *    UIS_HandleWindowStartPosChanged
 *
 * SYNOPSIS:
 *    void UIS_HandleWindowStartPosChanged(uintptr_t ID);
 *
 * PARAMETERS:
 *    ID [I] -- The userdata from the selected item.
 *
 * FUNCTION:
 *    This function handles the window start pos combox box changing.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIS_HandleWindowStartPosChanged(uintptr_t ID)
{
    t_UIButtonCtrl *GrabCurrent;
    bool EnableAll;

    EnableAll=false;
    switch(ID)
    {
        case e_WindowStartupPos_OSDefault:
            EnableAll=false;
        break;
        case e_WindowStartupPos_RestoreFromSession:
            EnableAll=false;
        break;
        case e_WindowStartupPos_RestoreFromSettings:
            EnableAll=true;
        break;
        default:
        break;
    }

    GrabCurrent=UIS_GetButtonHandle(e_UIS_Button_GrabCurrentWinPos);

    UIEnableButton(GrabCurrent,EnableAll);
}

/*******************************************************************************
 * NAME:
 *    DS_DoSelectFont
 *
 * SYNOPSIS:
 *    void DS_DoSelectFont(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function asks the user to select the default font for the text
 *    canvas.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DS_DoSelectFont(void)
{
    long FontStyle;

    FontStyle=0;
    if(m_SettingConSettings->FontBold)
        FontStyle|=UIFONT_STYLE_BOLD;
    if(m_SettingConSettings->FontItalic)
        FontStyle|=UIFONT_STYLE_ITALIC;

    if(UI_FontReq("Select font",m_SettingConSettings->FontName,m_SettingConSettings->FontSize,
            FontStyle,UIFONT_FLAGS_FIXEDWIDTH))
    {
        m_SettingConSettings->FontBold=FontStyle&UIFONT_STYLE_BOLD;
        m_SettingConSettings->FontItalic=FontStyle&UIFONT_STYLE_ITALIC;
        DS_RethinkDisplayDisplay();
    }
}

/*******************************************************************************
 * NAME:
 *    DS_DoSelectHexDisplayFont
 *
 * SYNOPSIS:
 *    void DS_DoSelectHexDisplayFont(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function asks the user to select the hex display font.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DS_DoSelectHexDisplayFont(void)
{
    long FontStyle;

    FontStyle=0;
    if(g_Settings.HexDisplaysFontBold)
        FontStyle|=UIFONT_STYLE_BOLD;
    if(g_Settings.HexDisplaysFontItalic)
        FontStyle|=UIFONT_STYLE_ITALIC;

    if(UI_FontReq("Select font",g_Settings.HexDisplaysFontName,
            g_Settings.HexDisplaysFontSize,FontStyle,UIFONT_FLAGS_FIXEDWIDTH))
    {
        g_Settings.HexDisplaysFontBold=FontStyle&UIFONT_STYLE_BOLD;
        g_Settings.HexDisplaysFontItalic=FontStyle&UIFONT_STYLE_ITALIC;
        DS_RethinkHexDisplayDisplay();
    }
}

/*******************************************************************************
 * NAME:
 *    UIS_HandleInputProcessingChange
 *
 * SYNOPSIS:
 *    void UIS_HandleInputProcessingChange(uintptr_t ID);
 *
 * PARAMETERS:
 *    ID [I] -- Type of data processor selected
 *
 * FUNCTION:
 *    This function handles updating the UI with the selected data
 *    processor type.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIS_HandleInputProcessingChange(uintptr_t ID)
{
    t_UIPagePanelCtrl *DataProcessing;

    DataProcessing=UIS_GetPagePanelHandle(e_UIS_PagePanel_DataProcessing);
    switch(ID)
    {
        case e_DataProcessorType_Binary:
            UIPagePanelCtrlSetPage(DataProcessing,
                    e_UIS_DataProcessingPages_Binary);
        break;
        case e_DataProcessorType_Text:
            UIPagePanelCtrlSetPage(DataProcessing,
                    e_UIS_DataProcessingPages_Text);
        break;
        default:
        break;
    }
}

/*******************************************************************************
 * NAME:
 *    UIS_HandleInputProcessingCharEncChange
 *
 * SYNOPSIS:
 *    void UIS_HandleInputProcessingCharEncChange(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    Updates the UI when the input processor encoding is changed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIS_HandleInputProcessingCharEncChange(void)
{
    bool DataProcessorHasSettings;
    t_UIButtonCtrl *InputProCharEnc_Settings;
    t_UIComboBoxCtrl *TextProCharEnc;
    unsigned int Selected;

    TextProCharEnc=UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox_TextProCharEnc);
    InputProCharEnc_Settings=UIS_GetButtonHandle(e_UIS_Button_InputProCharEnc_Settings);

    Selected=UIGetComboBoxSelectedEntry(TextProCharEnc);
    if(Selected>=m_CharEncodingInputPros.size())
        return;

    if(DPS_DoesPluginHaveSettings(m_CharEncodingInputPros[Selected].IDStr))
        DataProcessorHasSettings=true;
    else
        DataProcessorHasSettings=false;

    UIEnableButton(InputProCharEnc_Settings,DataProcessorHasSettings);
}

/*******************************************************************************
 * NAME:
 *    UIS_HandleInputProcessingTermEmuChange
 *
 * SYNOPSIS:
 *    void UIS_HandleInputProcessingTermEmuChange(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function updates the UI when the input processing term emulation
 *    changes.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIS_HandleInputProcessingTermEmuChange(void)
{
    bool DataProcessorHasSettings;
    t_UIButtonCtrl *InputProTermEmu_Settings;
    t_UIComboBoxCtrl *TextProTermEmu;
    unsigned int Selected;

    TextProTermEmu=UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox_TextProTermEmu);
    InputProTermEmu_Settings=UIS_GetButtonHandle(e_UIS_Button_InputProTermEmu_Settings);

    Selected=UIGetComboBoxSelectedEntry(TextProTermEmu);
    if(Selected>=m_TermEmulationInputPros.size())
        return;

    if(DPS_DoesPluginHaveSettings(m_TermEmulationInputPros[Selected].IDStr))
        DataProcessorHasSettings=true;
    else
        DataProcessorHasSettings=false;

    UIEnableButton(InputProTermEmu_Settings,DataProcessorHasSettings);
}

/*******************************************************************************
 * NAME:
 *    UIS_HandleInputProcessingHighlighterChange
 *
 * SYNOPSIS:
 *    void UIS_HandleInputProcessingHighlighterChange(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function updates the UI when the input processing highlighter
 *    changes.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIS_HandleInputProcessingHighlighterChange(void)
{
    bool DataProcessorHasSettings;
    t_UIButtonCtrl *SettingsBttn;
    t_UIListViewCtrl *ListView;
    unsigned int Selected;

    ListView=UIS_GetListViewHandle(e_UIS_ListView_InputProTextHighlight);
    SettingsBttn=UIS_GetButtonHandle(e_UIS_Button_InputProHighLighting_Settings);

    DataProcessorHasSettings=false;
    if(UIListViewHasSelectedEntry(ListView))
    {
        Selected=UIGetListViewSelectedEntry(ListView);
        if(Selected<m_HighlighterInputPros.size())
        {
            if(DPS_DoesPluginHaveSettings(m_HighlighterInputPros[Selected].
                    IDStr))
            {
                DataProcessorHasSettings=true;
            }
        }
    }

    UIEnableButton(SettingsBttn,DataProcessorHasSettings);
}

/*******************************************************************************
 * NAME:
 *    UIS_HandleInputProcessingOtherChange
 *
 * SYNOPSIS:
 *    void UIS_HandleInputProcessingOtherChange(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function updates the UI when the input processing other
 *    changes.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIS_HandleInputProcessingOtherChange(void)
{
    bool DataProcessorHasSettings;
    t_UIButtonCtrl *SettingsBttn;
    t_UIListViewCtrl *ListView;
    unsigned int Selected;

    ListView=UIS_GetListViewHandle(e_UIS_ListView_InputProTextOther);
    SettingsBttn=UIS_GetButtonHandle(e_UIS_Button_InputProOther_Settings);

    DataProcessorHasSettings=false;
    if(UIListViewHasSelectedEntry(ListView))
    {
        Selected=UIGetListViewSelectedEntry(ListView);
        if(Selected<m_OtherInputPros.size())
        {
            if(DPS_DoesPluginHaveSettings(m_OtherInputPros[Selected].IDStr))
                DataProcessorHasSettings=true;
        }
    }

    UIEnableButton(SettingsBttn,DataProcessorHasSettings);
}

/*******************************************************************************
 * NAME:
 *    UIS_HandleInputProcessingBinaryDecoderChange
 *
 * SYNOPSIS:
 *    void UIS_HandleInputProcessingBinaryDecoderChange(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function updates the UI when the binary processing changes.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIS_HandleInputProcessingBinaryDecoderChange(void)
{
    bool DataProcessorHasSettings;
    t_UIButtonCtrl *SettingsBttn;
    t_UIListViewCtrl *ListView;
    unsigned int Selected;

    ListView=UIS_GetListViewHandle(e_UIS_ListView_BinaryProcessorDecoder);
    SettingsBttn=UIS_GetButtonHandle(e_UIS_Button_BinaryPro_Settings);

    Selected=UIGetListViewSelectedEntry(ListView);
    if(Selected>=m_BinaryDecoders.size())
        return;

    if(DPS_DoesPluginHaveSettings(m_BinaryDecoders[Selected].IDStr))
        DataProcessorHasSettings=true;
    else
        DataProcessorHasSettings=false;

    UIEnableButton(SettingsBttn,DataProcessorHasSettings);
}

/*******************************************************************************
 * NAME:
 *    UIS_HandleInputProcessingBinaryOtherChange
 *
 * SYNOPSIS:
 *    void UIS_HandleInputProcessingBinaryOtherChange(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function updates the UI when the binary other processors changes.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIS_HandleInputProcessingBinaryOtherChange(void)
{
    bool DataProcessorHasSettings;
    t_UIButtonCtrl *SettingsBttn;
    t_UIListViewCtrl *ListView;
    unsigned int Selected;

    ListView=UIS_GetListViewHandle(e_UIS_ListView_BinaryProcessorOther);
    SettingsBttn=UIS_GetButtonHandle(e_UIS_Button_BinaryProOther_Settings);

    Selected=UIGetListViewSelectedEntry(ListView);
    if(Selected>=m_BinaryOtherDecoders.size())
        return;

    if(DPS_DoesPluginHaveSettings(m_BinaryOtherDecoders[Selected].IDStr))
        DataProcessorHasSettings=true;
    else
        DataProcessorHasSettings=false;

    UIEnableButton(SettingsBttn,DataProcessorHasSettings);
}

/*******************************************************************************
 * NAME:
 *    UIS_HandleSysColPresetChange
 *
 * SYNOPSIS:
 *    void UIS_HandleSysColPresetChange(uintptr_t ID);
 *
 * PARAMETERS:
 *    ID [I] -- The new preset selected
 *
 * FUNCTION:
 *    This function handles updating the UI when the color preset is changed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIS_HandleSysColPresetChange(uintptr_t ID)
{
    uint32_t Colors[e_SysColShadeMAX][e_SysColMAX];
    t_UIButtonCtrl *SysCol_Apply;
    int r;

    SysCol_Apply=UIS_GetButtonHandle(e_UIS_Button_SysCol_Apply);

    if(ID<e_SysColPresetMAX)
    {
        UIEnableButton(SysCol_Apply,true);
        GetPresetSysColors((e_SysColPresetType)ID,Colors);
    }
    else
    {
        UIEnableButton(SysCol_Apply,false);
        memset(Colors,0x00,sizeof(Colors));
    }

    for(r=0;r<e_SysColMAX;r++)
    {
        UIS_SetPresetPreviewColor((e_SysColType)r,
                Colors[e_SysColShade_Normal][r]);
    }
}

/*******************************************************************************
 * NAME:
 *    DS_DoSysColApply
 *
 * SYNOPSIS:
 *    void DS_DoSysColApply(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function applies the default colors that user has selected in the
 *    UI.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DS_DoSysColApply(void)
{
    t_UIComboBoxCtrl *SysColPreset;
    e_SysColPresetType Preset;

    SysColPreset=UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox_SysCol_Preset);

    Preset=(e_SysColPresetType)UIGetComboBoxSelectedEntry(SysColPreset);

    if(Preset<e_SysColPresetMAX)
    {
        GetPresetSysColors(Preset,m_DS_SysColors);
        m_DS_DefaultColors[e_DefaultColors_BG]=
                m_DS_SysColors[e_SysColShade_Normal][e_SysCol_Black];
        m_DS_DefaultColors[e_DefaultColors_FG]=
                m_DS_SysColors[e_SysColShade_Normal][e_SysCol_White];

        GetPresetSelectionColors(m_DS_SelectionColors);

        DS_CopySysColors2GUI();
    }
}

static void DS_CopySysColors2GUI(void)
{
    int r;
    t_UIRadioBttnCtrl *SysColShade_Normal;
    t_UIRadioBttnCtrl *SysColShade_Bright;
    t_UIRadioBttnCtrl *SysColShade_Dark;
    e_SysColShadeType Shade;

    SysColShade_Normal=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysCol_Shade_Normal);
    SysColShade_Bright=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysCol_Shade_Bright);
    SysColShade_Dark=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysCol_Shade_Dark);

    Shade=e_SysColShade_Normal;
    if(UIIsRadioBttnSelected(SysColShade_Normal))
        Shade=e_SysColShade_Normal;
    if(UIIsRadioBttnSelected(SysColShade_Bright))
        Shade=e_SysColShade_Bright;
    if(UIIsRadioBttnSelected(SysColShade_Dark))
        Shade=e_SysColShade_Dark;

    for(r=0;r<e_SysColMAX;r++)
        UIS_SetSysColPreviewColor((e_SysColType)r,m_DS_SysColors[Shade][r]);

    for(r=0;r<e_DefaultColorsMAX;r++)
    {
        UIS_SetDefaultPreviewColor((e_DefaultColorsType)r,
                m_DS_DefaultColors[r]);
    }

    for(r=0;r<e_ColorMAX;r++)
        UIS_SetSelectionPreviewColor((e_ColorType)r,m_DS_SelectionColors[r]);
}

void DS_SetCurrentColor(uint32_t RGB)
{
    int Color;
    e_SysColShadeType Shade;
    t_UIRadioBttnCtrl *SysColShade_Normal;
    t_UIRadioBttnCtrl *SysColShade_Bright;
    t_UIRadioBttnCtrl *SysColShade_Dark;
    e_DS_EditingColorType Editing;

    SysColShade_Normal=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysCol_Shade_Normal);
    SysColShade_Bright=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysCol_Shade_Bright);
    SysColShade_Dark=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysCol_Shade_Dark);

    /* Find the selected color */
    Editing=DS_FindCurrentEditingColor(&Color);
    if(Editing==e_DS_EditingColorMAX)
        return;

    switch(Editing)
    {
        case e_DS_EditingColor_Defaults:
            m_DS_DefaultColors[Color]=RGB;
        break;
        case e_DS_EditingColor_Colors:
            Shade=e_SysColShade_Normal;
            if(UIIsRadioBttnSelected(SysColShade_Normal))
                Shade=e_SysColShade_Normal;
            if(UIIsRadioBttnSelected(SysColShade_Bright))
                Shade=e_SysColShade_Bright;
            if(UIIsRadioBttnSelected(SysColShade_Dark))
                Shade=e_SysColShade_Dark;

            m_DS_SysColors[Shade][Color]=RGB;
        break;
        case e_DS_EditingColor_Selection:
            m_DS_SelectionColors[Color]=RGB;
        break;
        case e_DS_EditingColorMAX:
        default:
        break;
    }
}

uint32_t DS_GetCurrentColor(void)
{
    int Color;
    e_SysColShadeType Shade;
    t_UIRadioBttnCtrl *SysColShade_Normal;
    t_UIRadioBttnCtrl *SysColShade_Bright;
    t_UIRadioBttnCtrl *SysColShade_Dark;
    e_DS_EditingColorType Editing;

    SysColShade_Normal=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysCol_Shade_Normal);
    SysColShade_Bright=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysCol_Shade_Bright);
    SysColShade_Dark=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysCol_Shade_Dark);

    /* Find the selected color */
    Editing=DS_FindCurrentEditingColor(&Color);
    if(Editing==e_DS_EditingColorMAX)
        return 0;

    switch(Editing)
    {
        case e_DS_EditingColor_Defaults:
            return m_DS_DefaultColors[Color];
        case e_DS_EditingColor_Colors:
            Shade=e_SysColShade_Normal;
            if(UIIsRadioBttnSelected(SysColShade_Normal))
                Shade=e_SysColShade_Normal;
            if(UIIsRadioBttnSelected(SysColShade_Bright))
                Shade=e_SysColShade_Bright;
            if(UIIsRadioBttnSelected(SysColShade_Dark))
                Shade=e_SysColShade_Dark;

            return m_DS_SysColors[Shade][Color];
        break;
        case e_DS_EditingColor_Selection:
            return m_DS_SelectionColors[Color];
        case e_DS_EditingColorMAX:
        default:
            return 0x000000;
        break;
    }
}

void DS_UpdateColorPreview(bool UpdateWeb)
{
    int Color;
    e_SysColShadeType Shade;
    t_UIScrollBarCtrl *RedScroll;
    t_UIScrollBarCtrl *GreenScroll;
    t_UIScrollBarCtrl *BlueScroll;
    t_UIRadioBttnCtrl *SysColShade_Normal;
    t_UIRadioBttnCtrl *SysColShade_Bright;
    t_UIRadioBttnCtrl *SysColShade_Dark;
    t_UINumberInput *RedInput;
    t_UINumberInput *GreenInput;
    t_UINumberInput *BlueInput;
    t_UITextInputCtrl *WebInput;
    uint32_t RGB;
    char buff[100];
    e_DS_EditingColorType Editing;

    m_DS_UpdatingColorPreview=true;

    SysColShade_Normal=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysCol_Shade_Normal);
    SysColShade_Bright=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysCol_Shade_Bright);
    SysColShade_Dark=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SysCol_Shade_Dark);
    RedScroll=UIS_GetScrollBarHandle(e_UIS_ScrollBar_SysCol_RedScroll);
    GreenScroll=UIS_GetScrollBarHandle(e_UIS_ScrollBar_SysCol_GreenScroll);
    BlueScroll=UIS_GetScrollBarHandle(e_UIS_ScrollBar_SysCol_BlueScroll);
    RedInput=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_SysCol_RedInput);
    GreenInput=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_SysCol_GreenInput);
    BlueInput=UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput_SysCol_BlueInput);
    WebInput=UIS_GetTextInputHandle(e_UIS_TextInput_SysCol_Web);

    Editing=DS_FindCurrentEditingColor(&Color);
    if(Editing==e_DS_EditingColorMAX)
        return;

    switch(Editing)
    {
        case e_DS_EditingColor_Defaults:
            RGB=m_DS_DefaultColors[Color];
        break;
        case e_DS_EditingColor_Colors:
            Shade=e_SysColShade_Normal;
            if(UIIsRadioBttnSelected(SysColShade_Normal))
                Shade=e_SysColShade_Normal;
            if(UIIsRadioBttnSelected(SysColShade_Bright))
                Shade=e_SysColShade_Bright;
            if(UIIsRadioBttnSelected(SysColShade_Dark))
                Shade=e_SysColShade_Dark;

            RGB=m_DS_SysColors[Shade][Color];
        break;
        case e_DS_EditingColor_Selection:
            RGB=m_DS_SelectionColors[Color];
        break;
        case e_DS_EditingColorMAX:
        default:
            RGB=0;
        break;
    }

    UISetScrollBarPos(RedScroll,(RGB>>16)&0xFF);
    UISetScrollBarPos(GreenScroll,(RGB>>8)&0xFF);
    UISetScrollBarPos(BlueScroll,(RGB)&0xFF);

    UISetNumberInputCtrlValue(RedInput,(RGB>>16)&0xFF);
    UISetNumberInputCtrlValue(GreenInput,(RGB>>8)&0xFF);
    UISetNumberInputCtrlValue(BlueInput,(RGB)&0xFF);

    if(UpdateWeb)
    {
        sprintf(buff,"#%06X",RGB);
        UISetTextCtrlText(WebInput,buff);
    }

    switch(Editing)
    {
        case e_DS_EditingColor_Defaults:
            UIS_SetDefaultPreviewColor((e_DefaultColorsType)Color,RGB);
        break;
        case e_DS_EditingColor_Colors:
            UIS_SetSysColPreviewColor((e_SysColType)Color,RGB);
        break;
        case e_DS_EditingColor_Selection:
            UIS_SetSelectionPreviewColor((e_ColorType)Color,RGB);
        break;
        case e_DS_EditingColorMAX:
        default:
        break;
    }

    m_DS_UpdatingColorPreview=false;
}

/*******************************************************************************
 * NAME:
 *    UIS_ProcessSysColWebInputValue
 *
 * SYNOPSIS:
 *    void UIS_ProcessSysColWebInputValue(const char *Text);
 *
 * PARAMETERS:
 *    Text [I] -- The text from the system color web input control
 *
 * FUNCTION:
 *    This function processes the color info that was input into the
 *    system color web input.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIS_ProcessSysColWebInputValue(const char *Text)
{
    int Len;
    const char *Start;
    char buff[100];
    uint32_t RGB;

    if(m_DS_UpdatingColorPreview)
        return;

    Len=strlen(Text);
    Start=Text;
    if(Text[0]=='#')
    {
        Start++;
        Len--;
    }

    if(Len==6)
    {
        buff[0]=Start[0];
        buff[1]=Start[1];
        buff[2]=0;
        RGB=strtol(buff,NULL,16)<<16;
        buff[0]=Start[2];
        buff[1]=Start[3];
        buff[2]=0;
        RGB|=strtol(buff,NULL,16)<<8;
        buff[0]=Start[4];
        buff[1]=Start[5];
        buff[2]=0;
        RGB|=strtol(buff,NULL,16);
    }
    else if(Len==3)
    {
        buff[0]=Start[0];
        buff[1]=Start[0];
        buff[2]=0;
        RGB=strtol(buff,NULL,16)<<16;
        buff[0]=Start[1];
        buff[1]=Start[1];
        buff[2]=0;
        RGB|=strtol(buff,NULL,16)<<8;
        buff[0]=Start[2];
        buff[1]=Start[2];
        buff[2]=0;
        RGB|=strtol(buff,NULL,16);
    }
    else
    {
        return;
    }
    DS_SetCurrentColor(RGB);
    DS_UpdateColorPreview(false);
}

/*******************************************************************************
 * NAME:
 *    DS_Event
 *
 * SYNOPSIS:
 *    bool DS_Event(const struct DSEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event we should process.
 *
 * FUNCTION:
 *    This function is called from the UI to tell us there was a UI event.
 *
 * RETURNS:
 *    true -- The event should act normally
 *    false -- There event should be canceled
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DS_Event(const struct DSEvent *Event)
{
    bool AcceptEvent;
    uint32_t SelColor;
    uint32_t RGB;
    t_UIColorPreviewCtrl *ColorPreviewHandle;
    t_UIComboBoxCtrl *ComboxBox;
    t_UIListViewCtrl *ListView;
    t_UITextInputCtrl *TxtHandle;
    string FileNameStr;
    string PathStr;
    const char *FilePart;
    const char *TmpCStr;
    unsigned int Selected;

    AcceptEvent=true;
    switch(Event->EventType)
    {
        case e_DSEvent_BttnTriggered:
            switch(Event->Info.Bttn.InputID)
            {
                case e_UIS_Button_LeftPanel_GrabLayout:
                    /* Copy the session pos over to settings */
                    g_Settings.LeftPanelSize=g_Session.LeftPanelSize;
                break;
                case e_UIS_Button_RightPanel_GrabLayout:
                    /* Copy the session pos over to settings */
                    g_Settings.RightPanelSize=g_Session.RightPanelSize;
                break;
                case e_UIS_Button_BottomPanel_GrabLayout:
                    /* Copy the session pos over to settings */
                    g_Settings.BottomPanelSize=g_Session.BottomPanelSize;
                break;
                case e_UIS_Button_GrabCurrentWinPos:
                    if(m_SettingsMW!=NULL)
                    {
                        m_SettingsMW->GetSizeAndPos(g_Settings.WindowPosX,
                                g_Settings.WindowPosY,g_Settings.WindowWidth,
                                g_Settings.WindowHeight);
                    }
                break;
                case e_UIS_Button_InputProCharEnc_Settings:
                    ComboxBox=UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox_TextProCharEnc);
                    Selected=UIGetComboBoxSelectedEntry(ComboxBox);
                    if(Selected!=0 && Selected<m_CharEncodingInputPros.size())
                    {
                        RunDataProPluginSettingsDialog(m_SettingConSettings,
                                m_CharEncodingInputPros[Selected].IDStr);
                    }
                break;
                case e_UIS_Button_InputProTermEmu_Settings:
                    ComboxBox=UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox_TextProTermEmu);
                    Selected=UIGetComboBoxSelectedEntry(ComboxBox);
                    if(Selected!=0 && Selected<m_TermEmulationInputPros.size())
                    {
                        RunDataProPluginSettingsDialog(m_SettingConSettings,
                                m_TermEmulationInputPros[Selected].IDStr);
                    }
                break;
                case e_UIS_Button_InputProHighLighting_Settings:
                    ListView=UIS_GetListViewHandle(e_UIS_ListView_InputProTextHighlight);
                    Selected=UIGetListViewSelectedEntry(ListView);
                    if(Selected<m_HighlighterInputPros.size())
                    {
                        RunDataProPluginSettingsDialog(m_SettingConSettings,
                                m_HighlighterInputPros[Selected].IDStr);
                    }
                break;
                case e_UIS_Button_InputProOther_Settings:
                    ListView=UIS_GetListViewHandle(e_UIS_ListView_InputProTextOther);
                    Selected=UIGetListViewSelectedEntry(ListView);
                    if(Selected<m_OtherInputPros.size())
                    {
                        RunDataProPluginSettingsDialog(m_SettingConSettings,
                                m_OtherInputPros[Selected].IDStr);
                    }
                break;
                case e_UIS_Button_BinaryPro_Settings:
                    ListView=UIS_GetListViewHandle(e_UIS_ListView_BinaryProcessorDecoder);
                    Selected=UIGetListViewSelectedEntry(ListView);
                    if(Selected<m_BinaryDecoders.size())
                    {
                        RunDataProPluginSettingsDialog(m_SettingConSettings,
                                m_BinaryDecoders[Selected].IDStr);
                    }
                break;
                case e_UIS_Button_BinaryProOther_Settings:
                    ListView=UIS_GetListViewHandle(e_UIS_ListView_BinaryProcessorOther);
                    Selected=UIGetListViewSelectedEntry(ListView);
                    if(Selected<m_BinaryOtherDecoders.size())
                    {
                        RunDataProPluginSettingsDialog(m_SettingConSettings,
                                m_BinaryOtherDecoders[Selected].IDStr);
                    }
                break;
                case e_UIS_Button_SysCol_Apply:
                    DS_DoSysColApply();
                break;
                case e_UIS_Button_SelectFont:
                    DS_DoSelectFont();
                break;
                case e_UIS_Button_SelectCursorColor:
                    SelColor=UIGetColor(m_CursorColor);
                    if(SelColor!=0xFFFFFFFF)
                    {
                        m_CursorColor=SelColor;
                        ColorPreviewHandle=UIS_GetColorPreviewHandle(
                                e_UIS_ColorPreview_CursorColor);
                        UISetColorPreviewColor(ColorPreviewHandle,
                                m_CursorColor);
                    }
                break;
                case e_UIS_Button_KeyBindingCmdSet:
                    UIS_HandleKeyBindingsCmdShortCutChange();
                break;
                case e_UIS_Button_CaptureSelectFilename:
                    DS_SelectCaptureFilename();
                break;

                case e_UIS_Button_SelectHexDisplayFGColor:
                    SelColor=UIGetColor(m_HexDisplaysFGColor);
                    if(SelColor!=0xFFFFFFFF)
                    {
                        m_HexDisplaysFGColor=SelColor;
                        ColorPreviewHandle=UIS_GetColorPreviewHandle(
                                e_UIS_ColorPreview_HexDisplay_FGDisplay);
                        UISetColorPreviewColor(ColorPreviewHandle,
                                m_HexDisplaysFGColor);
                    }
                break;

                case e_UIS_Button_SelectHexDisplayBGColor:
                    SelColor=UIGetColor(m_HexDisplaysBGColor);
                    if(SelColor!=0xFFFFFFFF)
                    {
                        m_HexDisplaysBGColor=SelColor;
                        ColorPreviewHandle=UIS_GetColorPreviewHandle(
                                e_UIS_ColorPreview_HexDisplay_BGDisplay);
                        UISetColorPreviewColor(ColorPreviewHandle,
                                m_HexDisplaysBGColor);
                    }
                break;

                case e_UIS_Button_SelectHexDisplaySelBGColor:
                    SelColor=UIGetColor(m_HexDisplaysSelBGColor);
                    if(SelColor!=0xFFFFFFFF)
                    {
                        m_HexDisplaysSelBGColor=SelColor;
                        ColorPreviewHandle=UIS_GetColorPreviewHandle(
                                e_UIS_ColorPreview_HexDisplay_SelBGDisplay);
                        UISetColorPreviewColor(ColorPreviewHandle,
                                m_HexDisplaysSelBGColor);
                    }
                break;

                case e_UIS_Button_SelectHexDisplayFont:
                    DS_DoSelectHexDisplayFont();
                break;

                case e_UIS_Button_CustomSound_SelectFilename:
                    TxtHandle=UIS_GetTextInputHandle(e_UIS_TextInput_CustomSoundFilename);
                    UIGetTextCtrlText(TxtHandle,FileNameStr);

                    TmpCStr=FileNameStr.c_str();
                    FilePart=Basename(TmpCStr);
                    if(FilePart==TmpCStr)
                    {
                        /* No path */
                        PathStr="";
                    }
                    else
                    {
                        PathStr.assign(TmpCStr,FilePart-TmpCStr);
                        FileNameStr.erase(0,FilePart-TmpCStr);
                    }

                    if(UI_LoadFileReq("Custom System Bell Sound",PathStr,
                            FileNameStr,"All Files|*\nWave|*.wav",1))
                    {
                        PathStr+=GetOSPathSeparator();
                        PathStr+=FileNameStr;
                        UISetTextCtrlText(TxtHandle,PathStr.c_str());
                    }
                break;

                case e_UIS_ButtonMAX:
                default:
                break;
            }
        break;
        case e_DSEvent_CheckboxClick:
            switch(Event->Info.Checkbox.InputID)
            {
                case e_UIS_Checkbox_LeftPanel_AutoHidePanel:
                    UIS_HandleLeftPanelAutoHideClick(Event->
                            Info.Checkbox.Checked);
                break;
                case e_UIS_Checkbox_RightPanel_AutoHidePanel:
                    UIS_HandleRightPanelAutoHideClick(Event->
                            Info.Checkbox.Checked);
                break;
                case e_UIS_Checkbox_BottomPanel_AutoHidePanel:
                    UIS_HandleBottomPanelAutoHideClick(Event->
                            Info.Checkbox.Checked);
                break;
                case e_UIS_Checkbox_TerminalSize_FixedWidth:
                case e_UIS_Checkbox_TerminalSize_FixedHeight:
                    DS_RethinkTerminalDisplay();
                break;

                case e_UIS_Checkbox_LeftPanel_RestoreFromSettings:
                case e_UIS_Checkbox_LeftPanel_ShowPanelOnStartup:
                case e_UIS_Checkbox_RightPanel_RestoreFromSettings:
                case e_UIS_Checkbox_RightPanel_ShowPanelOnStartup:
                case e_UIS_Checkbox_BottomPanel_RestoreFromSettings:
                case e_UIS_Checkbox_BottomPanel_ShowPanelOnStartup:
                case e_UIS_Checkbox_StartMax:
                case e_UIS_Checkbox_AutoConnectOnNewConnection:
                case e_UIS_Checkbox_AutoReopen:
                case e_UIS_Checkbox_AlwaysShowTabs:
                case e_UIS_Checkbox_CloseButtonOnTabs:
                case e_UIS_Checkbox_CenterTextInWindow:
                case e_UIS_Checkbox_AutoCROnLF:
                case e_UIS_Checkbox_LocalEcho:
                case e_UIS_Checkbox_CursorBlink:
                case e_UIS_Checkbox_BookmarksOpenNewTab:
                case e_UIS_Checkbox_StopWatchAutoLap:
                case e_UIS_Checkbox_StopWatchAutoStart:
                case e_UIS_Checkbox_StopWatchShowPanel:
                case e_UIS_Checkbox_CaptureTimestamp:
                case e_UIS_Checkbox_CaptureAppend:
                case e_UIS_Checkbox_CaptureStripCtrl:
                case e_UIS_Checkbox_CaptureStripEsc:
                case e_UIS_Checkbox_CaptureHexDump:
                case e_UIS_Checkbox_CaptureShowPanel:
                case e_UIS_Checkbox_HexDisplayEnabled:
                case e_UIS_Checkbox_BoldFontEnable:
                case e_UIS_Checkbox_ItalicFontEnable:
                case e_UIS_Checkbox_UnderlineEnable:
                case e_UIS_Checkbox_OverlineEnable:
                case e_UIS_Checkbox_ReverseEnable:
                case e_UIS_Checkbox_StrikeThroughEnable:
                case e_UIS_Checkbox_ColorEnable:
                case e_UIS_Checkbox_UseCustomSounds:
                case e_UIS_Checkbox_MouseCursorUseIBeam:
                case e_UIS_CheckboxMAX:
                default:
                break;
            }
        break;
        case e_DSEvent_RadioBttnClick:
            switch(Event->Info.RadioBttn.InputID)
            {
                case e_UIS_RadioBttn_SysCol_Shade_Normal:
                case e_UIS_RadioBttn_SysCol_Shade_Bright:
                case e_UIS_RadioBttn_SysCol_Shade_Dark:
                    DS_CopySysColors2GUI();
                    DS_UpdateColorPreview(true);
                break;
                case e_UIS_RadioBttn_SysColPrev_Black:
                case e_UIS_RadioBttn_SysColPrev_Red:
                case e_UIS_RadioBttn_SysColPrev_Green:
                case e_UIS_RadioBttn_SysColPrev_Yellow:
                case e_UIS_RadioBttn_SysColPrev_Blue:
                case e_UIS_RadioBttn_SysColPrev_Magenta:
                case e_UIS_RadioBttn_SysColPrev_Cyan:
                case e_UIS_RadioBttn_SysColPrev_White:
                    UIS_HandleSysColPreviewColorClick();
                break;
                case e_UIS_RadioBttn_DefaultColorPrev_Forground:
                case e_UIS_RadioBttn_DefaultColorPrev_Background:
                    UIS_HandleSysColDefaultColorClick();
                break;
                case e_UIS_RadioBttn_SelectionColorPrev_Forground:
                case e_UIS_RadioBttn_SelectionColorPrev_Background:
                    UIS_HandleSysColSelectionColorClick();
                break;
                case e_UIS_RadioBttn_Display_ClearScreen_Clear:
                case e_UIS_RadioBttn_Display_ClearScreen_Scroll:
                case e_UIS_RadioBttn_Display_ClearScreen_ScrollAll:
                case e_UIS_RadioBttn_Display_ClearScreen_ScrollWithHR:
                case e_UIS_RadioBttn_Keyboard_Backspace_BS:
                case e_UIS_RadioBttn_Keyboard_Backspace_DEL:
                case e_UIS_RadioBttn_Keyboard_Enter_CR:
                case e_UIS_RadioBttn_Keyboard_Enter_LF:
                case e_UIS_RadioBttn_Keyboard_Enter_CRLF:
                case e_UIS_RadioBttn_Keyboard_Clipboard_None:
                case e_UIS_RadioBttn_Keyboard_Clipboard_Normal:
                case e_UIS_RadioBttn_Keyboard_Clipboard_ShiftCtrl:
                case e_UIS_RadioBttn_Keyboard_Clipboard_Alt:
                case e_UIS_RadioBttn_Keyboard_Clipboard_Smart:
                break;
                case e_UIS_RadioBttn_SysBell_None:
                case e_UIS_RadioBttn_SysBell_System:
                case e_UIS_RadioBttn_SysBell_BuiltIn:
                case e_UIS_RadioBttn_SysBell_AudioOnly:
                case e_UIS_RadioBttn_SysBell_VisualOnly:
                    DS_RethinkGUI();
                break;
                case e_UIS_RadioBttnMAX:
                default:
                break;
            }
        break;
        case e_DSEvent_ScrollBarChange:
            switch(Event->Info.ScrollBar.InputID)
            {
                case e_UIS_ScrollBar_SysCol_RedScroll:
                    if(m_DS_UpdatingColorPreview)   // DEBUG PAUL: Still needed?
                        break;

                    RGB=DS_GetCurrentColor();
                    RGB&=~0xFF0000;
                    RGB|=Event->Info.ScrollBar.position<<16;
                    DS_SetCurrentColor(RGB);
                    DS_UpdateColorPreview(true);
                break;
                case e_UIS_ScrollBar_SysCol_GreenScroll:
                    if(m_DS_UpdatingColorPreview)   // DEBUG PAUL: Still needed?
                        break;

                    RGB=DS_GetCurrentColor();
                    RGB&=~0x00FF00;
                    RGB|=Event->Info.ScrollBar.position<<8;
                    DS_SetCurrentColor(RGB);
                    DS_UpdateColorPreview(true);
                break;
                case e_UIS_ScrollBar_SysCol_BlueScroll:
                    if(m_DS_UpdatingColorPreview)   // DEBUG PAUL: Still needed?
                        break;

                    RGB=DS_GetCurrentColor();
                    RGB&=~0x0000FF;
                    RGB|=Event->Info.ScrollBar.position;
                    DS_SetCurrentColor(RGB);
                    DS_UpdateColorPreview(true);
                break;
                case e_UIS_ScrollBar_SysColMAX:
                default:
                break;
            }
        break;
        case e_DSEvent_NumberInputChange:
            switch(Event->Info.NumberInput.InputID)
            {
                case e_UIS_NumberInput_SysCol_RedInput:
                    if(m_DS_UpdatingColorPreview)   // DEBUG PAUL: Still needed?
                        break;

                    RGB=DS_GetCurrentColor();
                    RGB&=~0xFF0000;
                    RGB|=Event->Info.NumberInput.value<<16;
                    DS_SetCurrentColor(RGB);
                    DS_UpdateColorPreview(true);
                break;
                case e_UIS_NumberInput_SysCol_GreenInput:
                    if(m_DS_UpdatingColorPreview)   // DEBUG PAUL: Still needed?
                        break;

                    RGB=DS_GetCurrentColor();
                    RGB&=~0x00FF00;
                    RGB|=Event->Info.NumberInput.value<<8;
                    DS_SetCurrentColor(RGB);
                    DS_UpdateColorPreview(true);
                break;
                case e_UIS_NumberInput_SysCol_BlueInput:
                    if(m_DS_UpdatingColorPreview)   // DEBUG PAUL: Still needed?
                        break;

                    RGB=DS_GetCurrentColor();
                    RGB&=~0x0000FF;
                    RGB|=Event->Info.NumberInput.value;
                    DS_SetCurrentColor(RGB);
                    DS_UpdateColorPreview(true);
                break;
                case e_UIS_NumberInput_TermSizeWidth:
                case e_UIS_NumberInput_TermSizeHeight:
                case e_UIS_NumberInput_ScrollBufferLines:
                case e_UIS_NumberInput_HexDisplay_BufferSize:
                case e_UIS_NumberInput_AutoReopenWaitTime:
                case e_UIS_NumberInput_DelayBetweenBytes:
                case e_UIS_NumberInput_DelayAfterNewLineSent:
                case e_UIS_NumberInputMAX:
                default:
                break;
            }
        break;

        case e_DSEvent_StringInputTextChange:
            switch(Event->Info.StrInput.InputID)
            {
                case e_UIS_TextInput_SysCol_Web:
                    UIS_ProcessSysColWebInputValue(Event->Info.StrInput.NewText);
                break;
                case e_UIS_TextInput_KeyBinding_Assigned2:
                case e_UIS_TextInput_Capture_DefaultFilename:
                case e_UIS_TextInput_CustomSoundFilename:
                case e_UIS_TextInputMAX:
                default:
                break;
            }
        break;

        case e_DSEvent_ComboBoxIndexChange:
            switch(Event->Info.ComboBox.InputID)
            {
                case e_UIS_ComboBox_WindowStartupPos:
                    UIS_HandleWindowStartPosChanged(Event->ID);
                break;
                case e_UIS_ComboBox_DataProcessor:
                    UIS_HandleInputProcessingChange(Event->ID);
                break;
                case e_UIS_ComboBox_TextProCharEnc:
                    UIS_HandleInputProcessingCharEncChange();
                break;
                case e_UIS_ComboBox_TextProTermEmu:
                    UIS_HandleInputProcessingTermEmuChange();
                break;
                case e_UIS_ComboBox_SysCol_Preset:
                    UIS_HandleSysColPresetChange(Event->ID);
                break;
                case e_UIS_ComboBoxMAX:
                default:
                break;
            }
        break;
        case e_DSEvent_ListViewChange:
            switch(Event->Info.ListView.InputID)
            {
                case e_UIS_ListView_AreaList:
                    UIS_HandleAreaChanged(Event->ID);
                break;
                case e_UIS_ListView_InputProTextHighlight:
                    UIS_HandleInputProcessingHighlighterChange();
                break;
                case e_UIS_ListView_InputProTextOther:
                    UIS_HandleInputProcessingOtherChange();
                break;
                case e_UIS_ListView_KeyBinding_CommandList:
                    UIS_HandleKeyBindingsCmdListChange(Event->ID);
                break;
                case e_UIS_ListView_BinaryProcessorDecoder:
                    UIS_HandleInputProcessingBinaryDecoderChange();
                break;
                case e_UIS_ListView_BinaryProcessorOther:
                    UIS_HandleInputProcessingBinaryOtherChange();
                break;
                case e_UIS_ListViewMAX:
                default:
                break;
            }
        break;
        case e_DSEvent_TabChange:
            switch(Event->Info.Tab.Index)
            {
                case e_UIS_TabCtrl_Terminal_Page_KeyBinding:
                case e_UIS_TabCtrl_Terminal_Page_DataProcessing:
                case e_UIS_TabCtrl_Terminal_Page_Terminal:
                case e_UIS_TabCtrl_Terminal_Page_Keyboard:
                case e_UIS_TabCtrl_Terminal_PageMAX:
                default:
                break;
            }
        break;
        case e_DSEventMAX:
        default:
        break;
    }
    return AcceptEvent;
}

/*******************************************************************************
 * NAME:
 *    DS_SelectCaptureFilename
 *
 * SYNOPSIS:
 *    static void DS_SelectCaptureFilename(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function prompts the user for the default capture filename.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DS_SelectCaptureFilename(void)
{
    std::string Path;
    std::string Filename;
    t_UITextInputCtrl *TxtHandle;

    TxtHandle=UIS_GetTextInputHandle(e_UIS_TextInput_Capture_DefaultFilename);
    UIGetTextCtrlText(TxtHandle,Path);

    Filename="";
    if(Path=="")
        Filename="Capture.log";
    if(UI_SaveFileReq("Default Capture to Filename",Path,Filename,
            "All Files|*\n"
            "Log Files|*.log\n"
            "Binary|*.bin\n",0))
    {
        Filename=UI_ConcatFile2Path(Path,Filename);
        UISetTextCtrlText(TxtHandle,Filename.c_str());
    }
}

/*******************************************************************************
 * NAME:
 *    DS_SetKeyboardRadioBttns
 *
 * SYNOPSIS:
 *    void DS_SetKeyboardRadioBttns(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function sets the GUI to settings for the keyboard radio buttons.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DS_GetSettingsFromGUI_KeyboardRadioBttns()
 ******************************************************************************/
static void DS_SetKeyboardRadioBttns(void)
{
    t_UIRadioBttnCtrl *Keyboard_Backspace_BS;
    t_UIRadioBttnCtrl *Keyboard_Backspace_DEL;
    t_UIRadioBttnCtrl *Keyboard_Enter_CR;
    t_UIRadioBttnCtrl *Keyboard_Enter_LF;
    t_UIRadioBttnCtrl *Keyboard_Enter_CRLF;
    t_UIRadioBttnCtrl *Keyboard_Clipboard_None;
    t_UIRadioBttnCtrl *Keyboard_Clipboard_Normal;
    t_UIRadioBttnCtrl *Keyboard_Clipboard_ShiftCtrl;
    t_UIRadioBttnCtrl *Keyboard_Clipboard_Alt;
    t_UIRadioBttnCtrl *Keyboard_Clipboard_Smart;

    Keyboard_Backspace_BS=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Backspace_BS);
    Keyboard_Backspace_DEL=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Backspace_DEL);
    Keyboard_Enter_CR=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Enter_CR);
    Keyboard_Enter_LF=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Enter_LF);
    Keyboard_Enter_CRLF=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Enter_CRLF);
    Keyboard_Clipboard_None=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Clipboard_None);
    Keyboard_Clipboard_Normal=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Clipboard_Normal);
    Keyboard_Clipboard_ShiftCtrl=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Clipboard_ShiftCtrl);
    Keyboard_Clipboard_Alt=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Clipboard_Alt);
    Keyboard_Clipboard_Smart=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Clipboard_Smart);

    if(Keyboard_Backspace_BS==NULL ||
            Keyboard_Backspace_DEL==NULL ||
            Keyboard_Enter_CR==NULL || Keyboard_Enter_LF==NULL ||
            Keyboard_Enter_CRLF==NULL ||
            Keyboard_Clipboard_None==NULL || Keyboard_Clipboard_Normal==NULL ||
            Keyboard_Clipboard_ShiftCtrl==NULL ||
            Keyboard_Clipboard_Alt==NULL || Keyboard_Clipboard_Smart==NULL)
    {
        return;
    }

    switch(m_SettingConSettings->BackspaceKeyMode)
    {
        default:
        case e_BackspaceKeyMAX:
        case e_BackspaceKey_BS:
            UISelectRadioBttn(Keyboard_Backspace_BS);
        break;
        case e_BackspaceKey_DEL:
            UISelectRadioBttn(Keyboard_Backspace_DEL);
        break;
    }

    switch(m_SettingConSettings->EnterKeyMode)
    {
        default:
        case e_EnterKeyMAX:
        case e_EnterKey_LF:
            UISelectRadioBttn(Keyboard_Enter_LF);
        break;
        case e_EnterKey_CR:
            UISelectRadioBttn(Keyboard_Enter_CR);
        break;
        case e_EnterKey_CRLF:
            UISelectRadioBttn(Keyboard_Enter_CRLF);
        break;
    }

    switch(m_SettingConSettings->ClipboardMode)
    {
        default:
        case e_ClipboardModeMAX:
        case e_ClipboardMode_Smart:
            UISelectRadioBttn(Keyboard_Clipboard_Smart);
        break;
        case e_ClipboardMode_None:
            UISelectRadioBttn(Keyboard_Clipboard_None);
        break;
        case e_ClipboardMode_Normal:
            UISelectRadioBttn(Keyboard_Clipboard_Normal);
        break;
        case e_ClipboardMode_ShiftCtrl:
            UISelectRadioBttn(Keyboard_Clipboard_ShiftCtrl);
        break;
        case e_ClipboardMode_Alt:
            UISelectRadioBttn(Keyboard_Clipboard_Alt);
        break;
    }
}

/*******************************************************************************
 * NAME:
 *    DS_GetSettingsFromGUI_KeyboardRadioBttns
 *
 * SYNOPSIS:
 *    static void DS_GetSettingsFromGUI_KeyboardRadioBttns(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function reads the values from the GUI and sets the settings
 *    for the keyboard radio buttons.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DS_SetKeyboardRadioBttns()
 ******************************************************************************/
static void DS_GetSettingsFromGUI_KeyboardRadioBttns(void)
{
    t_UIRadioBttnCtrl *Keyboard_Backspace_BS;
    t_UIRadioBttnCtrl *Keyboard_Backspace_DEL;
    t_UIRadioBttnCtrl *Keyboard_Enter_CR;
    t_UIRadioBttnCtrl *Keyboard_Enter_LF;
    t_UIRadioBttnCtrl *Keyboard_Enter_CRLF;
    t_UIRadioBttnCtrl *Keyboard_Clipboard_None;
    t_UIRadioBttnCtrl *Keyboard_Clipboard_Normal;
    t_UIRadioBttnCtrl *Keyboard_Clipboard_ShiftCtrl;
    t_UIRadioBttnCtrl *Keyboard_Clipboard_Alt;
    t_UIRadioBttnCtrl *Keyboard_Clipboard_Smart;

    Keyboard_Backspace_BS=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Backspace_BS);
    Keyboard_Backspace_DEL=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Backspace_DEL);
    Keyboard_Enter_CR=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Enter_CR);
    Keyboard_Enter_LF=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Enter_LF);
    Keyboard_Enter_CRLF=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Enter_CRLF);
    Keyboard_Clipboard_None=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Clipboard_None);
    Keyboard_Clipboard_Normal=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Clipboard_Normal);
    Keyboard_Clipboard_ShiftCtrl=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Clipboard_ShiftCtrl);
    Keyboard_Clipboard_Alt=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Clipboard_Alt);
    Keyboard_Clipboard_Smart=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_Keyboard_Clipboard_Smart);

    if(Keyboard_Backspace_BS==NULL || Keyboard_Backspace_DEL==NULL ||
            Keyboard_Enter_CR==NULL || Keyboard_Enter_LF==NULL ||
            Keyboard_Enter_CRLF==NULL || Keyboard_Clipboard_None==NULL ||
            Keyboard_Clipboard_Normal==NULL ||
            Keyboard_Clipboard_ShiftCtrl==NULL ||
            Keyboard_Clipboard_Alt==NULL || Keyboard_Clipboard_Smart==NULL)
    {
        return;
    }

    /* Backspace */
    if(UIIsRadioBttnSelected(Keyboard_Backspace_BS))
        m_SettingConSettings->BackspaceKeyMode=e_BackspaceKey_BS;
    if(UIIsRadioBttnSelected(Keyboard_Backspace_DEL))
        m_SettingConSettings->BackspaceKeyMode=e_BackspaceKey_DEL;

    if(UIIsRadioBttnSelected(Keyboard_Enter_LF))
        m_SettingConSettings->EnterKeyMode=e_EnterKey_LF;
    if(UIIsRadioBttnSelected(Keyboard_Enter_CR))
        m_SettingConSettings->EnterKeyMode=e_EnterKey_CR;
    if(UIIsRadioBttnSelected(Keyboard_Enter_CRLF))
        m_SettingConSettings->EnterKeyMode=e_EnterKey_CRLF;

    if(UIIsRadioBttnSelected(Keyboard_Clipboard_None))
        m_SettingConSettings->ClipboardMode=e_ClipboardMode_None;
    if(UIIsRadioBttnSelected(Keyboard_Clipboard_Normal))
        m_SettingConSettings->ClipboardMode=e_ClipboardMode_Normal;
    if(UIIsRadioBttnSelected(Keyboard_Clipboard_ShiftCtrl))
        m_SettingConSettings->ClipboardMode=e_ClipboardMode_ShiftCtrl;
    if(UIIsRadioBttnSelected(Keyboard_Clipboard_Alt))
        m_SettingConSettings->ClipboardMode=e_ClipboardMode_Alt;
    if(UIIsRadioBttnSelected(Keyboard_Clipboard_Smart))
        m_SettingConSettings->ClipboardMode=e_ClipboardMode_Smart;
}

/*******************************************************************************
 * NAME:
 *    DS_UnselectColorRadioButtons
 *
 * SYNOPSIS:
 *    static void DS_UnselectColorRadioButtons(uint32_t Bttns);
 *
 * PARAMETERS:
 *    Bttns [I] -- What radio buttons to unselect.  This is bit field
 *                  DS_USCRB_DEFAULT -- The default colors
 *                  DS_USCRB_COLORS -- The colors definitions
 *                  DS_USCRB_SELECTION -- The selected text colors
 *
 * FUNCTION:
 *    This function unselects buttons for what color is being edited in the
 *    colors tab.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DS_UnselectColorRadioButtons(uint32_t Bttns)
{
    t_UIRadioBttnCtrl *UIBttn;
    int Bttn;
    int Color;

    if(Bttns&DS_USCRB_DEFAULT)
    {
        for(Bttn=e_UIS_RadioBttn_DefaultColorPrev_Forground;
                Bttn<=e_UIS_RadioBttn_DefaultColorPrev_Background;Bttn++)
        {
            UIBttn=UIS_GetRadioBttnHandle((e_UIS_RadioBttns)(Bttn));
            UIUnselectRadioBttn(UIBttn);
        }
    }
    if(Bttns&DS_USCRB_COLORS)
    {
        for(Color=0;Color<e_SysColMAX;Color++)
        {
            UIBttn=UIS_GetRadioBttnHandle(
                    (e_UIS_RadioBttns)(e_UIS_RadioBttn_SysColPrev_Black+Color));
            UIUnselectRadioBttn(UIBttn);
        }
    }
    if(Bttns&DS_USCRB_SELECTION)
    {
        for(Bttn=e_UIS_RadioBttn_SelectionColorPrev_Forground;
                Bttn<=e_UIS_RadioBttn_SelectionColorPrev_Background;Bttn++)
        {
            UIBttn=UIS_GetRadioBttnHandle((e_UIS_RadioBttns)(Bttn));
            UIUnselectRadioBttn(UIBttn);
        }
    }
}

/*******************************************************************************
 * NAME:
 *    UIS_HandleSysColPreviewColorClick
 *
 * SYNOPSIS:
 *    void UIS_HandleSysColPreviewColorClick(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when one of the preview colors is clicked.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIS_HandleSysColPreviewColorClick(void)
{
    DS_UnselectColorRadioButtons(DS_USCRB_DEFAULT|DS_USCRB_SELECTION);

    DS_UpdateColorPreview(true);
}

/*******************************************************************************
 * NAME:
 *    UIS_HandleSysColDefaultColorClick
 *
 * SYNOPSIS:
 *    void UIS_HandleSysColDefaultColorClick(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when one of the default color button are clicked.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIS_HandleSysColDefaultColorClick(void)
{
    DS_UnselectColorRadioButtons(DS_USCRB_COLORS|DS_USCRB_SELECTION);
    DS_UpdateColorPreview(true);
}

/*******************************************************************************
 * NAME:
 *    UIS_HandleSysColSelectionColorClick
 *
 * SYNOPSIS:
 *    void UIS_HandleSysColSelectionColorClick(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when one of the selection color buttons are
 *    clicked.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void UIS_HandleSysColSelectionColorClick(void)
{
    DS_UnselectColorRadioButtons(DS_USCRB_COLORS|DS_USCRB_DEFAULT);

    DS_UpdateColorPreview(true);
}

/*******************************************************************************
 * NAME:
 *    DS_FindCurrentEditingColor
 *
 * SYNOPSIS:
 *    e_DS_EditingColorType DS_FindCurrentEditingColor(int *Color);
 *
 * PARAMETERS:
 *    Color [O] -- What color index was selected.  The type will depend on
 *                 what is selected for editing.
 *                      e_DS_EditingColor_Defaults -- 'e_DefaultColorsType'
 *                      e_DS_EditingColor_Colors -- 'e_SysColType'
 *                      e_DS_EditingColor_Selection -- 'e_ColorType'
 *
 * FUNCTION:
 *    This function figures out what color is currently selected for editing
 *    in the color tab.
 *
 * RETURNS:
 *    What is current selected for editing.
 *          e_DS_EditingColor_Defaults -- The defaults
 *          e_DS_EditingColor_Colors -- The colors
 *          e_DS_EditingColor_Selection -- The selection colors
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
e_DS_EditingColorType DS_FindCurrentEditingColor(int *Color)
{
    int Index;
    t_UIRadioBttnCtrl *DefaultBttnFG;
    t_UIRadioBttnCtrl *DefaultBttnBG;
    t_UIRadioBttnCtrl *SelBttnFG;
    t_UIRadioBttnCtrl *SelBttnBG;
    t_UIRadioBttnCtrl *PreviewBttn;
    e_DS_EditingColorType RetValue;

    DefaultBttnFG=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_DefaultColorPrev_Forground);
    DefaultBttnBG=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_DefaultColorPrev_Background);
    SelBttnFG=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SelectionColorPrev_Forground);
    SelBttnBG=UIS_GetRadioBttnHandle(e_UIS_RadioBttn_SelectionColorPrev_Background);

    /* Figure out what we are editing */
    for(Index=0;Index<e_SysColMAX;Index++)
    {
        PreviewBttn=UIS_GetRadioBttnHandle(
                (e_UIS_RadioBttns)(e_UIS_RadioBttn_SysColPrev_Black+Index));
        if(UIIsRadioBttnSelected(PreviewBttn))
        {
            /* Found it */
            break;
        }
    }

    if(Index!=e_SysColMAX)
    {
        *Color=Index;
        RetValue=e_DS_EditingColor_Colors;
    }
    else if(UIIsRadioBttnSelected(DefaultBttnFG))
    {
        *Color=e_DefaultColors_FG;
        RetValue=e_DS_EditingColor_Defaults;
    }
    else if(UIIsRadioBttnSelected(DefaultBttnBG))
    {
        *Color=e_DefaultColors_BG;
        RetValue=e_DS_EditingColor_Defaults;
    }
    else if(UIIsRadioBttnSelected(SelBttnFG))
    {
        *Color=e_Color_FG;
        RetValue=e_DS_EditingColor_Selection;
    }
    else if(UIIsRadioBttnSelected(SelBttnBG))
    {
        *Color=e_Color_BG;
        RetValue=e_DS_EditingColor_Selection;
    }
    else
    {
        /* Don't have anything selected? */
        RetValue=e_DS_EditingColorMAX;
    }

    return RetValue;
}
