/*******************************************************************************
 * FILENAME: UISettings.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __UISETTINGS_H_
#define __UISETTINGS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"
#include "App/Display/DisplayColors.h"
#include "App/Settings.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
enum e_UIS_Checkbox
{
    e_UIS_Checkbox_LeftPanel_RestoreFromSettings,
    e_UIS_Checkbox_LeftPanel_ShowPanelOnStartup,
    e_UIS_Checkbox_LeftPanel_AutoHidePanel,
    e_UIS_Checkbox_RightPanel_RestoreFromSettings,
    e_UIS_Checkbox_RightPanel_ShowPanelOnStartup,
    e_UIS_Checkbox_RightPanel_AutoHidePanel,
    e_UIS_Checkbox_BottomPanel_RestoreFromSettings,
    e_UIS_Checkbox_BottomPanel_ShowPanelOnStartup,
    e_UIS_Checkbox_BottomPanel_AutoHidePanel,
    e_UIS_Checkbox_StartMax,
    e_UIS_Checkbox_AutoConnectOnNewConnection,
    e_UIS_Checkbox_AlwaysShowTabs,
    e_UIS_Checkbox_CloseButtonOnTabs,
    e_UIS_Checkbox_TerminalSize_FixedWidth,
    e_UIS_Checkbox_TerminalSize_FixedHeight,
    e_UIS_Checkbox_CenterTextInWindow,
    e_UIS_Checkbox_CursorBlink,
    e_UIS_Checkbox_BookmarksOpenNewTab,
    e_UIS_Checkbox_StopWatchAutoLap,
    e_UIS_Checkbox_StopWatchAutoStart,
    e_UIS_Checkbox_StopWatchShowPanel,
    e_UIS_Checkbox_CaptureTimestamp,
    e_UIS_Checkbox_CaptureAppend,
    e_UIS_Checkbox_CaptureStripCtrl,
    e_UIS_Checkbox_CaptureStripEsc,
    e_UIS_Checkbox_CaptureHexDump,
    e_UIS_Checkbox_CaptureShowPanel,
    e_UIS_Checkbox_HexDisplayEnabled,
    e_UIS_Checkbox_BoldFontEnable,
    e_UIS_Checkbox_ItalicFontEnable,
    e_UIS_Checkbox_UnderlineEnable,
    e_UIS_Checkbox_OverlineEnable,
    e_UIS_Checkbox_ReverseEnable,
    e_UIS_Checkbox_StrikeThroughEnable,
    e_UIS_Checkbox_ColorEnable,
    e_UIS_Checkbox_UseCustomSounds,
    e_UIS_Checkbox_MouseCursorUseIBeam,
    e_UIS_Checkbox_AutoReopen,
    e_UIS_Checkbox_AutoCROnLF,
    e_UIS_Checkbox_LocalEcho,
    e_UIS_CheckboxMAX
};

enum e_UIS_ListView
{
    e_UIS_ListView_AreaList=0,
    e_UIS_ListView_InputProTextHighlight,
    e_UIS_ListView_InputProTextOther,
    e_UIS_ListView_KeyBinding_CommandList,
    e_UIS_ListView_BinaryProcessorDecoder,
    e_UIS_ListView_BinaryProcessorOther,
    e_UIS_ListViewMAX
};

enum e_UIS_TabCtrl
{
    e_UIS_TabCtrl_PanelTab,
    e_UIS_TabCtrl_StartupTab,
    e_UIS_TabCtrl_ConnectionsTab,
    e_UIS_TabCtrl_Display,
    e_UIS_TabCtrl_Terminal,
    e_UIS_TabCtrl_Behaviour,
    e_UIS_TabMAX
};

enum e_UIS_Button
{
    e_UIS_Button_LeftPanel_GrabLayout,
    e_UIS_Button_RightPanel_GrabLayout,
    e_UIS_Button_BottomPanel_GrabLayout,
    e_UIS_Button_GrabCurrentWinPos,
    e_UIS_Button_InputProCharEnc_Settings,
    e_UIS_Button_InputProTermEmu_Settings,
    e_UIS_Button_InputProHighLighting_Settings,
    e_UIS_Button_InputProOther_Settings,
    e_UIS_Button_BinaryPro_Settings,
    e_UIS_Button_BinaryProOther_Settings,
    e_UIS_Button_SysCol_Apply,
    e_UIS_Button_SelectFont,
    e_UIS_Button_SelectCursorColor,
    e_UIS_Button_KeyBindingCmdSet,
    e_UIS_Button_CaptureSelectFilename,
    e_UIS_Button_SelectHexDisplayFGColor,
    e_UIS_Button_SelectHexDisplayBGColor,
    e_UIS_Button_SelectHexDisplayFont,
    e_UIS_Button_SelectHexDisplaySelBGColor,
    e_UIS_Button_CustomSound_SelectFilename,
    e_UIS_ButtonMAX
};

enum e_UIS_ComboBox
{
    e_UIS_ComboBox_WindowStartupPos,
    e_UIS_ComboBox_DataProcessor,
    e_UIS_ComboBox_TextProCharEnc,
    e_UIS_ComboBox_TextProTermEmu,
    e_UIS_ComboBox_SysCol_Preset,
    e_UIS_ComboBoxMAX
};

enum e_UIS_NumberInput
{
    e_UIS_NumberInput_TermSizeWidth,
    e_UIS_NumberInput_TermSizeHeight,
    e_UIS_NumberInput_ScrollBufferLines,
    e_UIS_NumberInput_SysCol_RedInput,
    e_UIS_NumberInput_SysCol_GreenInput,
    e_UIS_NumberInput_SysCol_BlueInput,
    e_UIS_NumberInput_HexDisplay_BufferSize,
    e_UIS_NumberInput_AutoReopenWaitTime,
    e_UIS_NumberInput_DelayBetweenBytes,
    e_UIS_NumberInput_DelayAfterNewLineSent,
    e_UIS_NumberInputMAX
};

enum e_UIS_GroupBox
{
    e_UIS_GroupBox_TextProcessingSettings,
    e_UIS_GroupBox_Display_Tabs,
    e_UIS_GroupBox_Display_ClearScreen,
    e_UIS_GroupBox_Display_MouseCursor,
    e_UIS_GroupBoxMAX
};

enum e_UIS_RadioBttns
{
    e_UIS_RadioBttn_SysCol_Shade_Normal,
    e_UIS_RadioBttn_SysCol_Shade_Bright,
    e_UIS_RadioBttn_SysCol_Shade_Dark,
    e_UIS_RadioBttn_SysColPrev_Black,
    e_UIS_RadioBttn_SysColPrev_Red,
    e_UIS_RadioBttn_SysColPrev_Green,
    e_UIS_RadioBttn_SysColPrev_Yellow,
    e_UIS_RadioBttn_SysColPrev_Blue,
    e_UIS_RadioBttn_SysColPrev_Magenta,
    e_UIS_RadioBttn_SysColPrev_Cyan,
    e_UIS_RadioBttn_SysColPrev_White,
    e_UIS_RadioBttn_DefaultColorPrev_Forground,
    e_UIS_RadioBttn_DefaultColorPrev_Background,
    e_UIS_RadioBttn_Display_ClearScreen_Clear,
    e_UIS_RadioBttn_Display_ClearScreen_Scroll,
    e_UIS_RadioBttn_Display_ClearScreen_ScrollAll,
    e_UIS_RadioBttn_Display_ClearScreen_ScrollWithHR,
    e_UIS_RadioBttn_Keyboard_Backspace_BS,
    e_UIS_RadioBttn_Keyboard_Backspace_DEL,
    e_UIS_RadioBttn_Keyboard_Enter_CR,
    e_UIS_RadioBttn_Keyboard_Enter_LF,
    e_UIS_RadioBttn_Keyboard_Enter_CRLF,
    e_UIS_RadioBttn_Keyboard_Clipboard_None,
    e_UIS_RadioBttn_Keyboard_Clipboard_Normal,
    e_UIS_RadioBttn_Keyboard_Clipboard_ShiftCtrl,
    e_UIS_RadioBttn_Keyboard_Clipboard_Alt,
    e_UIS_RadioBttn_Keyboard_Clipboard_Smart,
    e_UIS_RadioBttn_SysBell_None,
    e_UIS_RadioBttn_SysBell_System,
    e_UIS_RadioBttn_SysBell_BuiltIn,
    e_UIS_RadioBttn_SysBell_AudioOnly,
    e_UIS_RadioBttn_SysBell_VisualOnly,
    e_UIS_RadioBttn_SelectionColorPrev_Forground,
    e_UIS_RadioBttn_SelectionColorPrev_Background,
    e_UIS_RadioBttnMAX
};

enum e_UIS_ScrollBar
{
    e_UIS_ScrollBar_SysCol_RedScroll,
    e_UIS_ScrollBar_SysCol_GreenScroll,
    e_UIS_ScrollBar_SysCol_BlueScroll,
    e_UIS_ScrollBar_SysColMAX
};

enum e_UIS_TextInput
{
    e_UIS_TextInput_SysCol_Web,   // SysCol_Web_lineEdit
    e_UIS_TextInput_KeyBinding_Assigned2,
    e_UIS_TextInput_Capture_DefaultFilename,
    e_UIS_TextInput_CustomSoundFilename,
    e_UIS_TextInputMAX
};

enum e_UIS_ColorPreview
{
    e_UIS_ColorPreview_CursorColor,
    e_UIS_ColorPreview_HexDisplay_FGDisplay,
    e_UIS_ColorPreview_HexDisplay_BGDisplay,
    e_UIS_ColorPreview_HexDisplay_SelBGDisplay,
    e_UIS_ColorPreviewMAX
};

enum e_UIS_Labels
{
    e_UIS_Labels_CurrentFont,
    e_UIS_Labels_HexDisplayCurrentFont,
    e_UIS_LabelsMAX
};

enum e_UIS_PagePanel
{
    e_UIS_PagePanel_DataProcessing,
    e_UIS_PagePanelMAX
};

/* Must match UI */
enum e_UIS_InputProcessingPages
{
    e_UIS_DataProcessingPages_Text     =0,
    e_UIS_DataProcessingPages_Binary   =1,
    e_UIS_DataProcessingPagesMAX
};

/* Must match UI */
enum e_UIS_TabCtrl_Terminal_Page
{
    e_UIS_TabCtrl_Terminal_Page_Terminal,
    e_UIS_TabCtrl_Terminal_Page_DataProcessing,
    e_UIS_TabCtrl_Terminal_Page_Keyboard,
    e_UIS_TabCtrl_Terminal_Page_KeyBinding,
    e_UIS_TabCtrl_Terminal_PageMAX
};

/* Must match UI */
enum e_UIS_TabCtrl_Display_Page
{
    e_UIS_TabCtrl_Display_Page_Display,
    e_UIS_TabCtrl_Display_Page_Colors,
    e_UIS_TabCtrl_Display_Page_Attrib,
    e_UIS_TabCtrl_Display_Page_HexDumps,
    e_UIS_TabCtrl_Display_PageMAX
};

/* Events */
typedef enum
{
    e_DSEvent_BttnTriggered,
    e_DSEvent_CheckboxClick,
    e_DSEvent_RadioBttnClick,
    e_DSEvent_ScrollBarChange,
    e_DSEvent_NumberInputChange,
    e_DSEvent_StringInputTextChange,
    e_DSEvent_ComboBoxIndexChange,
    e_DSEvent_ListViewChange,
    e_DSEvent_TabChange,
    e_DSEventMAX
} e_DSEventType;

struct DSEventDataTab
{
    enum e_UIS_TabCtrl TabID;
    e_UIS_TabCtrl_Terminal_Page Index;
};

struct DSEventDataBttn
{
    enum e_UIS_Button InputID;
};

struct DSEventDataCheckbox
{
    enum e_UIS_Checkbox InputID;
    bool Checked;
};

struct DSEventDataRadioBttn
{
    e_UIS_RadioBttns InputID;
};

struct DSEventDataScrollBar
{
    enum e_UIS_ScrollBar InputID;
    int position;
};

struct DSEventDataNumberInput
{
    enum e_UIS_NumberInput InputID;
    int value;
};

struct DSEventDataStringChange
{
    enum e_UIS_TextInput InputID;
    const char *NewText;
};

struct DSEventDataComboBox
{
    enum e_UIS_ComboBox InputID;
    int Index;
};

struct DSEventDataListView
{
    enum e_UIS_ListView InputID;
};

union DSEventData
{
    struct DSEventDataBttn Bttn;
    struct DSEventDataCheckbox Checkbox;
    struct DSEventDataRadioBttn RadioBttn;
    struct DSEventDataScrollBar ScrollBar;
    struct DSEventDataNumberInput NumberInput;
    struct DSEventDataStringChange StrInput;
    struct DSEventDataComboBox ComboBox;
    struct DSEventDataListView ListView;
    struct DSEventDataTab Tab;
};

struct DSEvent
{
    e_DSEventType EventType;
    uintptr_t ID;
    union DSEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

bool UIS_Alloc_Settings(void);
bool UIS_Show_Settings(void);
void UIS_Free_Settings(void);
void UIS_MakeTabVisable(e_UIS_TabCtrl Tab);
t_UICheckboxCtrl *UIS_GetCheckboxHandle(e_UIS_Checkbox UIObj);
t_UIListViewCtrl *UIS_GetListViewHandle(e_UIS_ListView UIObj);
t_UIButtonCtrl *UIS_GetButtonHandle(e_UIS_Button UIObj);
t_UITabCtrl *UIS_GetTabCtrlHandle(e_UIS_TabCtrl UIObj);
t_UIComboBoxCtrl *UIS_GetComboBoxCtrlHandle(e_UIS_ComboBox UIObj);
t_UINumberInput *UIS_GetNumberInputCtrlHandle(e_UIS_NumberInput UIObj);
t_UIGroupBox *UIS_GetGroupBoxHandle(e_UIS_GroupBox UIObj);
t_UIRadioBttnCtrl *UIS_GetRadioBttnHandle(e_UIS_RadioBttns UIObj);
t_UIScrollBarCtrl *UIS_GetScrollBarHandle(e_UIS_ScrollBar UIObj);
t_UITextInputCtrl *UIS_GetTextInputHandle(e_UIS_TextInput UIObj);
t_UIColorPreviewCtrl *UIS_GetColorPreviewHandle(e_UIS_ColorPreview UIObj);
t_UILabelCtrl *UIS_GetLabelHandle(e_UIS_Labels UIObj);
t_UIPagePanelCtrl *UIS_GetPagePanelHandle(e_UIS_PagePanel UIObj);
void UIS_SetDefaultPreviewColor(e_DefaultColorsType Color,uint32_t RGB);
void UIS_SetSelectionPreviewColor(e_ColorType Color,uint32_t RGB);

void UIS_SetPresetPreviewColor(e_SysColType Color,uint32_t RGB);
void UIS_SetSysColPreviewColor(e_SysColType Color,uint32_t RGB);
void UIS_SetSysColPreviewSelected(e_SysColType Selected);

bool DS_Event(const struct DSEvent *Event);

#endif
