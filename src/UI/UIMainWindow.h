/*******************************************************************************
 * FILENAME: UIMainWindow.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the main window interface in it.
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
 * HISTORY:
 *    Paul Hutchinson (05 Jul 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __UIMAINWINDOW_H_
#define __UIMAINWINDOW_H_

/***  HEADER FILES TO INCLUDE          ***/
//#include "App/KeyPressProcessorsSystem.h"
//#include "App/DisplayBuffers.h"
#include "UI/UIControl.h"
#include "PluginSDK/KeyDefines.h"
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
/* Must match GUI, also don't forget to add new values to session */
typedef enum
{
    e_LeftPanelTab_Display,
    e_LeftPanelTab_ConOptions,
    e_LeftPanelTab_Capture,
    e_LeftPanelTab_Upload,
    e_LeftPanelTab_Download,
    e_LeftPanelTab_Bridge,
    e_LeftPanelTabMAX
} e_LeftPanelTabType;

/* Must match GUI, also don't forget to add new values to session */
typedef enum
{
    e_RightPanelTab_StopWatch,
    e_RightPanelTabMAX
} e_RightPanelTabType;

/* Must match GUI, also don't forget to add new values to session */
typedef enum
{
    e_BottomPanelTab_Hex,
    e_BottomPanelTab_Injection,
    e_BottomPanelTab_Buffers,
    e_BottomPanelTabMAX
} e_BottomPanelTabType;

/* Don't forget to update 'm_Cmd2MenuMapping' */
typedef enum
{
    e_UIMWMenu_NewTab,
//    e_UIMWMenu_NewConnection,
    e_UIMWMenu_ResetTerm,
    e_UIMWMenu_InsertHorizontalRule,
    e_UIMWMenu_ClearScreen,
    e_UIMWMenu_ClearScrollBackBuffer,
    e_UIMWMenu_CloseTab,
    e_UIMWMenu_CloseAll,
    e_UIMWMenu_Quit,
    e_UIMWMenu_About,
    e_UIMWMenu_Settings,
    e_UIMWMenu_ImportSettings,
    e_UIMWMenu_ExportSettings,
    e_UIMWMenu_Connect,
    e_UIMWMenu_Disconnect,
    e_UIMWMenu_Copy,
    e_UIMWMenu_Paste,
    e_UIMWMenu_GotoColumn,
    e_UIMWMenu_GotoRow,
    e_UIMWMenu_ChangeConnectionName,
    e_UIMWMenu_ConnectionOptions,
    e_UIMWMenu_ConnectionSettings,
    e_UIMWMenu_TransmitDelay,
    e_UIMWMenu_AddBookmark,
    e_UIMWMenu_ManageBookmarks,
    e_UIMWMenu_StopWatch_Start,
    e_UIMWMenu_StopWatch_Stop,
    e_UIMWMenu_StopWatch_Reset,
    e_UIMWMenu_StopWatch_Lap,
    e_UIMWMenu_StopWatch_Clear,
    e_UIMWMenu_StopWatch_StartOnTx,
    e_UIMWMenu_StopWatch_AutoLap,
    e_UIMWMenu_Capture_CaptureToFile,
    e_UIMWMenu_Capture_TimestampToggle,
    e_UIMWMenu_Capture_AppendToggle,
    e_UIMWMenu_Capture_StripCtrlCharsToggle,
    e_UIMWMenu_Capture_StripEscSeqToggle,
    e_UIMWMenu_Capture_HexDumpToggle,
    e_UIMWMenu_Capture_Stop,
    e_UIMWMenu_Tools_ComTest,
    e_UIMWMenu_GetPlugins,
    e_UIMWMenu_InstallPlugin,
    e_UIMWMenu_ManagePlugin,
    e_UIMWMenu_BridgeConnections,
    e_UIMWMenu_ReleaseBridgedConnections,
    e_UIMWMenu_RestoreConnectionSettings,
    e_UIMWMenu_ShowNonPrintable,
    e_UIMWMenu_ShowEndOfLines,
    e_UIMWMenu_SelectAll,
    e_UIMWMenu_ZoomIn,
    e_UIMWMenu_ZoomOut,
    e_UIMWMenu_ResetZoom,
    e_UIMWMenu_Send_NULL,
    e_UIMWMenu_Send_Backspace,
    e_UIMWMenu_Send_Tab,
    e_UIMWMenu_Send_Line_Feed,
    e_UIMWMenu_Send_Form_Feed,
    e_UIMWMenu_Send_Carriage_Return,
    e_UIMWMenu_Send_Escape,
    e_UIMWMenu_Send_Delete,
    e_UIMWMenu_Send_Other,
    e_UIMWMenu_SettingsQuickJump_TermSize,
    e_UIMWMenu_SettingsQuickJump_TermEmu,
    e_UIMWMenu_SettingsQuickJump_Font,
    e_UIMWMenu_SettingsQuickJump_Colors,
    e_UIMWMenuMAX
} e_UIMWMenuType;

enum e_UIMWTabCtrl
{
    e_UIMWTabCtrl_MainTabs,
    e_UIMWTabCtrl_LeftPanel,
    e_UIMWTabCtrl_RightPanel,
    e_UIMWTabCtrl_BottomPanel,
    e_UIMWTabMAX
};

typedef enum
{
    e_UIMWToolbar_NewTab,
    e_UIMWToolbar_ConnectToggle,
    e_UIMWToolbar_URIGo,
    e_UIMWToolbar_Copy,
    e_UIMWToolbar_Paste,
    e_UIMWToolbarMAX
} e_UIMWToolbarType;

/* Don't forget to add to 'm_Cmd2ButtonMapping' in Commands.cpp */
typedef enum
{
    e_UIMWBttn_ConnectionOptionApply,
    e_UIMWBttn_StopWatch_Start,
    e_UIMWBttn_StopWatch_Reset,
    e_UIMWBttn_StopWatch_Lap,
    e_UIMWBttn_StopWatch_Clear,
    e_UIMWBttn_Capture_Start,
    e_UIMWBttn_Capture_SelectFilename,
    e_UIMWBttn_Upload_Start,
    e_UIMWBttn_Upload_SelectFilename,
    e_UIMWBttn_Upload_Abort,
    e_UIMWBttn_Download_Start,
    e_UIMWBttn_Download_Abort,
    e_UIMWBttn_HexDisplay_Clear,
    e_UIMWBttn_HexDisplay_Copy,
    e_UIMWBttn_HexDisplay_CopyAs,
    e_UIMWBttn_SendBuffers_Edit,
    e_UIMWBttn_SendBuffers_Send,
    e_UIMWBttn_Bridge_Bridge,
    e_UIMWBttn_Bridge_Release,
    e_UIMWBttnMAX
} e_UIMWBttnType;

typedef enum
{
    e_UIMWTxtInput_URI,
    e_UIMWTxtInput_Capture_Filename,
    e_UIMWTxtInput_Upload_Filename,
    e_UIMWTxtInputMAX
} e_UIMWTxtInputType;

typedef enum
{
    e_UIMWCheckbox_StopWatch_StartOnTx,
    e_UIMWCheckbox_StopWatch_AutoLap,
    e_UIMWCheckbox_Capture_Timestamp,
    e_UIMWCheckbox_Capture_Append,
    e_UIMWCheckbox_Capture_StripCtrlChars,
    e_UIMWCheckbox_Capture_StripEscSeq,
    e_UIMWCheckbox_Capture_HexDump,
    e_UIMWCheckbox_HexDisplay_Paused,
    e_UIMWCheckbox_Bridge_Lock1,
    e_UIMWCheckbox_Bridge_Lock2,
    e_UIMWCheckboxMAX
} e_UIMWCheckboxType;

typedef enum
{
    e_UIMWListView_StopWatch_Laps,
    e_UIMWListView_Buffers_List,
    e_UIMWListViewMAX
} e_UIMWListViewType;

typedef enum
{
    e_UIMWLabel_StopWatch_Time,
    e_UIMWLabel_Download_Filename,
    e_UIMWLabel_Download_BytesRx,
    e_UIMWLabel_Upload_BytesTx,
    e_UIMWLabelMAX
} e_UIMWLabelType;

typedef enum
{
    e_UIMWComboBox_Upload_Protocol,
    e_UIMWComboBox_Download_Protocol,
    e_UIMWComboBox_Bridge_Connection1,
    e_UIMWComboBox_Bridge_Connection2,
    e_UIMWComboBoxMAX
} e_UIMWComboBoxType;

typedef enum
{
    e_UIMWProgressBar_Upload,
    e_UIMWProgressBar_Download,
    e_UIMWProgressBarMAX
} e_UIMWProgressBarType;

struct UIMainWindow {int x;};
typedef struct UIMainWindow t_UIMainWindow;

typedef enum
{
    e_MWEvent_MainWindowClose,
    e_MWEvent_MenuTriggered,
    e_MWEvent_BttnTriggered,
    e_MWEvent_ToolbarTriggered,
    e_MWEvent_CheckboxTriggered,
    e_MWEvent_TxtInputEditFinished,
    e_MWEvent_ComboBoxChanged,
    e_MWEvent_FirstShow,
    e_MWEvent_WindowResize,
    e_MWEvent_LeftPanelSizeChange,
    e_MWEvent_RightPanelSizeChange,
    e_MWEvent_BottomPanelSizeChange,
    e_MWEvent_WindowSet2Maximized,
    e_MWEvent_WindowMoved,
    e_MWEvent_MainWindowKeyPress,
    e_MWEvent_BookmarkMenuSelected,
    e_MWEvent_TabClose,
    e_MWEvent_StopWatch_Timer,
    e_MWEvent_UploadMenuTriggered,
    e_MWEvent_DownloadMenuTriggered,
    e_MWEvent_PanelTabChange,
    e_MWEvent_ListViewChange,
    e_MWEventMAX
} e_MWEventType;

struct MWEventDataMenu
{
    e_UIMWMenuType InputID;
};

struct MWEventDataBttn
{
    e_UIMWBttnType InputID;
};

struct MWEventDataToolbar
{
    e_UIMWToolbarType InputID;
};

struct MWEventDataCheckbox
{
    e_UIMWCheckboxType InputID;
    bool Checked;
};

struct MWEventDataTextInput
{
    e_UIMWTxtInputType InputID;
    const char *Text;
};

struct MWEventDataSize
{
    int Width;
    int Height;
};

struct MWEventDataPanelSize
{
    int NewSize;
    bool PanelOpen;
};

struct MWEventDataMaximized
{
    bool Max;
};

struct MWEventDataXY
{
    int x;
    int y;
};

struct MWEventDataKey
{
    uint8_t Mods;
    e_UIKeys Key;
    const uint8_t *Text;
    unsigned int TextLen;
};

struct MWEventDataComboBox
{
    e_UIMWComboBoxType InputID;
    int Index;
};

struct MWEventDataPanelTab
{
    e_UIMWTabCtrl TabID;
    int NewIndex;
};

struct MWEventDataListView
{
    e_UIMWListViewType InputID;
};

union MWEventData
{
    struct MWEventDataMenu Menu;
    struct MWEventDataBttn Bttn;
    struct MWEventDataToolbar Toolbar;
    struct MWEventDataCheckbox Checkbox;
    struct MWEventDataSize NewSize;
    struct MWEventDataPanelSize PanelInfo;
    struct MWEventDataMaximized MaximizedInfo;
    struct MWEventDataXY Moved;
    struct MWEventDataKey Key;
    struct MWEventDataTextInput Txt;
    struct MWEventDataComboBox ComboBox;
    struct MWEventDataPanelTab PanelTab;
    struct MWEventDataListView ListView;
};

struct MWEvent
{
    e_MWEventType EventType;
    uintptr_t ID;
    t_UIMainWindow *UIWindow;
    class TheMainWindow *MW;
    union MWEventData Info;
};

struct UITextAreaFrame {int x;};
typedef struct UITextAreaFrame t_UITextAreaFrameCtrl;

typedef enum
{
    e_TextAreaEvent_TCEvent,
    e_TextAreaEvent_SendBttn,
    e_TextAreaEventMAX
} e_TextAreaEventType;

union MWTextAreaEventData
{
    const struct TCEvent *TCEventData;
};

struct TextAreaEvent
{
    e_TextAreaEventType EventType;
    uintptr_t ID;
    union MWTextAreaEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
t_UITabCtrl *UIMW_GetTabCtrlHandle(t_UIMainWindow *win,e_UIMWTabCtrl UIObj);
t_UIMainWindow *UIMW_AllocMainWindow(class TheMainWindow *MW,uintptr_t ID);
void UIMW_FreeMainWindow(t_UIMainWindow *win);
void UIMW_ShowMainWindow(t_UIMainWindow *win);
void UIMW_CloseMainWindow(t_UIMainWindow *win);
void UIMW_SetWindowPos(t_UIMainWindow *win,int x,int y);
void UIMW_GetWindowPos(t_UIMainWindow *win,int &x,int &y);
void UIMW_GetWindowSize(t_UIMainWindow *win,int &Width,int &Height);
void UIMW_SetWindowSize(t_UIMainWindow *win,int Width,int Height);
void UIMW_Maximize(t_UIMainWindow *win);
void UIMW_SetBottomPanel(t_UIMainWindow *win,int NewSize,bool PanelOpen);
void UIMW_SetLeftPanel(t_UIMainWindow *win,int NewSize,bool PanelOpen);
void UIMW_SetRightPanel(t_UIMainWindow *win,int NewSize,bool PanelOpen);
t_UIContainerFrameCtrl *UIMW_GetContainerFrameCtrlHandle(t_UIMainWindow *win);
void UIMW_SetContainerFrameVisible(t_UIContainerFrameCtrl *ctrl,bool Show);
void UIMW_SetWindowTitle(t_UIMainWindow *win,const char *Title);
void UIMW_SwitchTabControlCloseBttnPos(t_UIMainWindow *win,bool OnTab);
e_UIMenuCtrl *UIMW_AddBookmarkMenuItem(t_UIMainWindow *win,
        e_UISubMenuCtrl *Parent,const char *Title,uintptr_t ID);
e_UISubMenuCtrl *UIMW_AddBookmarkSubMenu(t_UIMainWindow *win,const char *Title);
void UIMW_AddBookmarkClearAllMenus(t_UIMainWindow *win);
t_UIContainerFrameCtrl *UIMW_GetHexDisplayContainerFrameCtrlHandle(t_UIMainWindow *win);
t_UIContainerFrameCtrl *UIMW_GetSendBuffersContainerFrameCtrlHandle(t_UIMainWindow *win);
e_UIMenuCtrl *UIMW_AddFTPUploadMenuItem(t_UIMainWindow *win,const char *Title,
        uintptr_t ID);
e_UIMenuCtrl *UIMW_AddFTPDownloadMenuItem(t_UIMainWindow *win,const char *Title,
        uintptr_t ID);
void UIMW_AddFTPUploadClearAllMenus(t_UIMainWindow *win);
void UIMW_AddFTPDownloadClearAllMenus(t_UIMainWindow *win);

e_UIMenuCtrl *UIMW_GetMenuHandle(t_UIMainWindow *win,e_UIMWMenuType UIObj);
t_UIToolbarCtrl *UIMW_GetToolbarHandle(t_UIMainWindow *win,e_UIMWToolbarType UIObj);
t_UIButtonCtrl *UIMW_GetButtonHandle(t_UIMainWindow *win,e_UIMWBttnType UIObj);
t_UICheckboxCtrl *UIMW_GetCheckboxHandle(t_UIMainWindow *win,e_UIMWCheckboxType UIObj);
t_UIListViewCtrl *UIMW_GetListViewHandle(t_UIMainWindow *win,e_UIMWListViewType UIObj);
t_UITextInputCtrl *UIMW_GetTxtInputHandle(t_UIMainWindow *win,e_UIMWTxtInputType UIObj);
t_UILabelCtrl *UIMW_GetLabelHandle(t_UIMainWindow *win,e_UIMWLabelType UIObj);
t_UIComboBoxCtrl *UIMW_GetComboBoxHandle(t_UIMainWindow *win,e_UIMWComboBoxType UIObj);
t_UIProgressBarCtrl *UIMW_GetProgressBarHandle(t_UIMainWindow *win,e_UIMWProgressBarType UIObj);

t_UIContainerCtrl *UIMW_GetOptionsFrameContainer(t_UIMainWindow *win);
t_UIContainerCtrl *UIMW_GetUploadOptionsFrameContainer(t_UIMainWindow *win);
t_UIContainerCtrl *UIMW_GetDownloadOptionsFrameContainer(t_UIMainWindow *win);
t_UIContainerCtrl *UIMW_GetConAuxControlsFrameContainer(t_UIMainWindow *win);

void UIMW_EnableStopWatchTimer(t_UIMainWindow *win,bool Enable);

/***  APP PROVIDED EXTERNAL FUNCTIONS ***/
bool MW_Event(const struct MWEvent *Event);

/* DEBUG STUFF */
void MWDebug1(uintptr_t ID);
void MWDebug2(uintptr_t ID);
void MWDebug3(uintptr_t ID);
void MWDebug4(uintptr_t ID);
void MWDebug5(uintptr_t ID);
void MWDebug6(uintptr_t ID);

/***  APP PROVIDED EXTERNAL FUNCTIONS ***/
//void Timer_1SecTimer(void);
//void Timer_CursorTimer(void);

#endif   /* end of "#ifndef __UIMAINWINDOW_H_" */
