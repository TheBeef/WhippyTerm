/*******************************************************************************
 * FILENAME: Form_MainWindowAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the access functions for the main window in it.
 *
 * CREATED BY:
 *    Paul Hutchinson (27 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "UI/UIMainWindow.h"
#include "Form_MainWindow.h"
#include "ui_Form_MainWindow.h"
#include "Widget_CloseBttn.h"
#include <QScrollBar>
#include <stdio.h>
#include <map>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
//static void AllocNewDisplayFrame(QWidget *Parent,uintptr_t ID);
//static bool DisplayFrameEventHandler(const struct TCEvent *Event);

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    UIMW_GetTabCtrlHandle
 *
 * SYNOPSIS:
 *    t_UITabCtrl *UIMW_GetTabCtrlHandle(t_UIMainWindow *win,
 *              e_UIMWTabCtrl UIObj);
 *
 * PARAMETERS:
 *    win [I] -- The main window that has the control in it.
 *    UIObj [I] -- The tab control to get the handle for
 *
 * FUNCTION:
 *    This function gets a handle to a tab control from the UI.
 *
 * RETURNS:
 *    The handle to the tab control or NULL if there was an error.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UITabCtrl *UIMW_GetTabCtrlHandle(t_UIMainWindow *win,e_UIMWTabCtrl UIObj)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    switch(UIObj)
    {
        case e_UIMWTabCtrl_MainTabs:
            return (t_UITabCtrl *)realwin->ui->Main_tabWidget;
        case e_UIMWTabCtrl_LeftPanel:
            return (t_UITabCtrl *)realwin->ui->LeftPanel_tabWidget;
        case e_UIMWTabCtrl_RightPanel:
            return (t_UITabCtrl *)realwin->ui->RightPanel_tabWidget;
        case e_UIMWTabCtrl_BottomPanel:
            return (t_UITabCtrl *)realwin->ui->BottomPanel_tabWidget;
        case e_UIMWTabMAX:
        default:
        break;
    }
    return NULL;
}

e_UIMenuCtrl *UIMW_GetMenuHandle(t_UIMainWindow *win,e_UIMWMenuType UIObj)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    switch(UIObj)
    {
        case e_UIMWMenu_NewTab:
            return (e_UIMenuCtrl *)realwin->ui->actionNew_Tab;
        case e_UIMWMenu_CloseTab:
            return (e_UIMenuCtrl *)realwin->ui->actionClose_Tab;
        case e_UIMWMenu_CloseAll:
            return (e_UIMenuCtrl *)realwin->ui->actionClose_All;
        case e_UIMWMenu_Quit:
            return (e_UIMenuCtrl *)realwin->ui->actionQuit;
        case e_UIMWMenu_About:
            return (e_UIMenuCtrl *)realwin->ui->actionAbout_Whippy_Term;
        case e_UIMWMenu_Settings:
            return (e_UIMenuCtrl *)realwin->ui->actionSettings;
        case e_UIMWMenu_ImportSettings:
            return (e_UIMenuCtrl *)realwin->ui->actionImport_Settings;
        case e_UIMWMenu_ExportSettings:
            return (e_UIMenuCtrl *)realwin->ui->actionExport_Settings;
////        case e_UIMWMenu_NewConnection:
////            return (e_UIMenuCtrl *)realwin->ui->actionNew_Connection;
        case e_UIMWMenu_Connect:
            return (e_UIMenuCtrl *)realwin->ui->actionConnect_Menu;
        case e_UIMWMenu_Disconnect:
            return (e_UIMenuCtrl *)realwin->ui->actionDisconnect_Menu;
        case e_UIMWMenu_InsertHorizontalRule:
            return (e_UIMenuCtrl *)realwin->ui->actionInsert_Horizontal_Rule;
        case e_UIMWMenu_ResetTerm:
            return (e_UIMenuCtrl *)realwin->ui->actionReset;
        case e_UIMWMenu_ClearScreen:
            return (e_UIMenuCtrl *)realwin->ui->actionClear_Screen;
        case e_UIMWMenu_ClearScrollBackBuffer:
            return (e_UIMenuCtrl *)realwin->ui->actionClear_Scroll_Buffer;
        case e_UIMWMenu_Copy:
            return (e_UIMenuCtrl *)realwin->ui->actionCopy;
        case e_UIMWMenu_Paste:
            return (e_UIMenuCtrl *)realwin->ui->actionPaste;
        case e_UIMWMenu_GotoColumn:
            return (e_UIMenuCtrl *)realwin->ui->actionGoto_Column;
        case e_UIMWMenu_GotoRow:
            return (e_UIMenuCtrl *)realwin->ui->actionGoto_Line;
        case e_UIMWMenu_ChangeConnectionName:
            return (e_UIMenuCtrl *)realwin->ui->actionMenu_Change_Name;
        case e_UIMWMenu_AddBookmark:
            return (e_UIMenuCtrl *)realwin->ui->actionBookmark_this_connection;
        case e_UIMWMenu_ManageBookmarks:
            return (e_UIMenuCtrl *)realwin->ui->actionManage_Bookmarks;
        case e_UIMWMenu_StopWatch_Start:
            return (e_UIMenuCtrl *)realwin->ui->actionStopWatch_Start;
        case e_UIMWMenu_StopWatch_Stop:
            return (e_UIMenuCtrl *)realwin->ui->actionStopWatch_Stop;
        case e_UIMWMenu_StopWatch_Reset:
            return (e_UIMenuCtrl *)realwin->ui->actionStopWatch_Reset;
        case e_UIMWMenu_StopWatch_Lap:
            return (e_UIMenuCtrl *)realwin->ui->actionStopWatch_Lap;
        case e_UIMWMenu_StopWatch_Clear:
            return (e_UIMenuCtrl *)realwin->ui->actionStopWatch_Clear;
        case e_UIMWMenu_StopWatch_StartOnTx:
            return (e_UIMenuCtrl *)realwin->ui->actionStopWatch_Start_On_Tx;
        case e_UIMWMenu_StopWatch_AutoLap:
            return (e_UIMenuCtrl *)realwin->ui->actionStopWatch_Auto_Lap;
        case e_UIMWMenu_Capture_CaptureToFile:
            return (e_UIMenuCtrl *)realwin->ui->actionCapture_To_File;
        case e_UIMWMenu_Capture_TimestampToggle:
            return (e_UIMenuCtrl *)realwin->ui->actionTimestamp_new_lines;
        case e_UIMWMenu_Capture_AppendToggle:
            return (e_UIMenuCtrl *)realwin->ui->actionAppend_To_Existing_File;
        case e_UIMWMenu_Capture_StripCtrlCharsToggle:
            return (e_UIMenuCtrl *)realwin->ui->actionStrip_Control_Char_s;
        case e_UIMWMenu_Capture_StripEscSeqToggle:
            return (e_UIMenuCtrl *)realwin->ui->actionStrip_Esc_Sequences;
        case e_UIMWMenu_Capture_HexDumpToggle:
            return (e_UIMenuCtrl *)realwin->ui->actionHex_Dump;
        case e_UIMWMenu_Capture_Stop:
            return (e_UIMenuCtrl *)realwin->ui->actionCapture_Stop;
        case e_UIMWMenu_ConnectionOptions:
            return (e_UIMenuCtrl *)realwin->ui->actionConnection_Options;
        case e_UIMWMenu_ConnectionSettings:
            return (e_UIMenuCtrl *)realwin->ui->actionConnection_Settings;
        case e_UIMWMenu_TransmitDelay:
            return (e_UIMenuCtrl *)realwin->ui->actionTransmit_delay;
        case e_UIMWMenu_Tools_ComTest:
            return (e_UIMenuCtrl *)realwin->ui->actionCom_Test;
        case e_UIMWMenu_GetPlugins:
            return (e_UIMenuCtrl *)realwin->ui->actionGet_Plugins;
        case e_UIMWMenu_InstallPlugin:
            return (e_UIMenuCtrl *)realwin->ui->actionInstall_Plugin;
        case e_UIMWMenu_ManagePlugin:
            return (e_UIMenuCtrl *)realwin->ui->actionManage_Plugins;
        case e_UIMWMenu_BridgeConnections:
            return (e_UIMenuCtrl *)realwin->ui->actionBridge_Connections;
        case e_UIMWMenu_ReleaseBridgedConnections:
            return (e_UIMenuCtrl *)realwin->ui->actionRelease_Bridged_Connections;
        case e_UIMWMenu_RestoreConnectionSettings:
            return (e_UIMenuCtrl *)realwin->ui->actionRestore_Connection_Settings_to_Defaults;
        case e_UIMWMenu_ShowNonPrintable:
            return (e_UIMenuCtrl *)realwin->ui->actionShow_NonPrintables;
        case e_UIMWMenu_ShowEndOfLines:
            return (e_UIMenuCtrl *)realwin->ui->actionShow_End_Of_Lines;
        case e_UIMWMenu_SelectAll:
            return (e_UIMenuCtrl *)realwin->ui->actionSelect_All;
        case e_UIMWMenu_ZoomIn:
            return (e_UIMenuCtrl *)realwin->ui->actionZoom_In;
        case e_UIMWMenu_ZoomOut:
            return (e_UIMenuCtrl *)realwin->ui->actionZoom_Out;
        case e_UIMWMenu_ResetZoom:
            return (e_UIMenuCtrl *)realwin->ui->actionReset_Zoom;

        case e_UIMWMenu_Send_NULL:
            return (e_UIMenuCtrl *)realwin->ui->actionSend_NULL;
        case e_UIMWMenu_Send_Backspace:
            return (e_UIMenuCtrl *)realwin->ui->actionSend_Backspace;
        case e_UIMWMenu_Send_Tab:
            return (e_UIMenuCtrl *)realwin->ui->actionSend_Tab;
        case e_UIMWMenu_Send_Line_Feed:
            return (e_UIMenuCtrl *)realwin->ui->actionSend_Line_Feed;
        case e_UIMWMenu_Send_Form_Feed:
            return (e_UIMenuCtrl *)realwin->ui->actionSend_Form_Feed;
        case e_UIMWMenu_Send_Carriage_Return:
            return (e_UIMenuCtrl *)realwin->ui->actionSend_Carriage_Return;
        case e_UIMWMenu_Send_Escape:
            return (e_UIMenuCtrl *)realwin->ui->actionSend_Escape;
        case e_UIMWMenu_Send_Delete:
            return (e_UIMenuCtrl *)realwin->ui->actionSend_Delete;
        case e_UIMWMenu_Send_Other:
            return (e_UIMenuCtrl *)realwin->ui->actionSend_Other;
        case e_UIMWMenu_SettingsQuickJump_TermSize:
            return (e_UIMenuCtrl *)realwin->ui->actionTerminal_Size;
        case e_UIMWMenu_SettingsQuickJump_TermEmu:
            return (e_UIMenuCtrl *)realwin->ui->actionTerminal_Emulation;
        case e_UIMWMenu_SettingsQuickJump_Font:
            return (e_UIMenuCtrl *)realwin->ui->actionFont;
        case e_UIMWMenu_SettingsQuickJump_Colors:
            return (e_UIMenuCtrl *)realwin->ui->actionColors;

        case e_UIMWMenuMAX:
        default:
        break;
    }
    return NULL;
}

t_UIToolbarCtrl *UIMW_GetToolbarHandle(t_UIMainWindow *win,
        e_UIMWToolbarType UIObj)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    switch(UIObj)
    {
        case e_UIMWToolbar_NewTab:
            return (t_UIToolbarCtrl *)realwin->ui->actionNew_Tab_Toolbar;
        case e_UIMWToolbar_ConnectToggle:
            return (t_UIToolbarCtrl *)realwin->ui->actionConnectToggle;
        case e_UIMWToolbar_URIGo:
            return (t_UIToolbarCtrl *)realwin->ui->actionactionURIGo;
        case e_UIMWToolbar_Copy:
            return (t_UIToolbarCtrl *)realwin->ui->actionCopy_toolbar;
        case e_UIMWToolbar_Paste:
            return (t_UIToolbarCtrl *)realwin->ui->actionPaste_toolbar;
        case e_UIMWToolbarMAX:
        default:
        break;
    }
    return NULL;
}

t_UITextInputCtrl *UIMW_GetTxtInputHandle(t_UIMainWindow *win,
        e_UIMWTxtInputType UIObj)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    switch(UIObj)
    {
        case e_UIMWTxtInput_URI:
            return (t_UITextInputCtrl *)realwin->ToolBarURI_LineEdit;
        case e_UIMWTxtInput_Capture_Filename:
            return (t_UITextInputCtrl *)realwin->ui->lineEdit_Cap_FileName;
        case e_UIMWTxtInput_Upload_Filename:
            return (t_UITextInputCtrl *)realwin->ui->lineEdit_UploadFilename;
        case e_UIMWTxtInputMAX:
        default:
        break;
    }
    return NULL;
}

t_UIButtonCtrl *UIMW_GetButtonHandle(t_UIMainWindow *win,e_UIMWBttnType UIObj)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    switch(UIObj)
    {
        case e_UIMWBttn_ConnectionOptionApply:
            return (t_UIButtonCtrl *)realwin->ui->pushButton_CO_Apply;
        case e_UIMWBttn_StopWatch_Start:
            return (t_UIButtonCtrl *)realwin->ui->pushButton_SW_Start;
        case e_UIMWBttn_StopWatch_Reset:
            return (t_UIButtonCtrl *)realwin->ui->pushButton_SW_Reset;
        case e_UIMWBttn_StopWatch_Lap:
            return (t_UIButtonCtrl *)realwin->ui->pushButton_SW_Lap;
        case e_UIMWBttn_StopWatch_Clear:
            return (t_UIButtonCtrl *)realwin->ui->pushButton_SW_Clear;
        case e_UIMWBttn_Capture_Start:
            return (t_UIButtonCtrl *)realwin->ui->pushButton_Cap_Start;
        case e_UIMWBttn_Capture_SelectFilename:
            return (t_UIButtonCtrl *)realwin->ui->pushButton_Cap_GetFilename;
        case e_UIMWBttn_Upload_Start:
            return (t_UIButtonCtrl *)realwin->ui->pushButton_UploadStart;
        case e_UIMWBttn_Upload_SelectFilename:
            return (t_UIButtonCtrl *)realwin->ui->pushButton_UploadSelectFilename;
        case e_UIMWBttn_Upload_Abort:
            return (t_UIButtonCtrl *)realwin->ui->pushButton_UploadAbort;
        case e_UIMWBttn_Download_Start:
            return (t_UIButtonCtrl *)realwin->ui->pushButton_DownloadStart;
        case e_UIMWBttn_Download_Abort:
            return (t_UIButtonCtrl *)realwin->ui->pushButton_DownloadAbort;
        case e_UIMWBttn_HexDisplay_Clear:
            return (t_UIButtonCtrl *)realwin->ui->pushButton_HexClear;
        case e_UIMWBttn_HexDisplay_Copy:
            return (t_UIButtonCtrl *)realwin->ui->pushButton_HexCopy;
        case e_UIMWBttn_HexDisplay_CopyAs:
            return (t_UIButtonCtrl *)realwin->ui->pushButton_HexCopyAs;
        case e_UIMWBttn_SendBuffers_Edit:
            return (t_UIButtonCtrl *)realwin->ui->pushButton_Buffer_Edit;
        case e_UIMWBttn_SendBuffers_Send:
            return (t_UIButtonCtrl *)realwin->ui->pushButton_Buffer_Send;
        case e_UIMWBttn_Bridge_Bridge:
            return (t_UIButtonCtrl *)realwin->ui->pushButton_Bridge_Bridge;
        case e_UIMWBttn_Bridge_Release:
            return (t_UIButtonCtrl *)realwin->ui->pushButton_Bridge_Release;
        case e_UIMWBttnMAX:
        default:
        break;
    }
    return NULL;
}

t_UICheckboxCtrl *UIMW_GetCheckboxHandle(t_UIMainWindow *win,e_UIMWCheckboxType UIObj)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    switch(UIObj)
    {
        case e_UIMWCheckbox_StopWatch_StartOnTx:
            return (t_UICheckboxCtrl *)realwin->ui->checkBox_SW_StartOnTx;
        case e_UIMWCheckbox_StopWatch_AutoLap:
            return (t_UICheckboxCtrl *)realwin->ui->checkBox_SW_AutoLap;
        case e_UIMWCheckbox_Capture_Timestamp:
            return (t_UICheckboxCtrl *)realwin->ui->checkBox_Cap_Timestamp;
        case e_UIMWCheckbox_Capture_Append:
            return (t_UICheckboxCtrl *)realwin->ui->checkBox_Cap_Append;
        case e_UIMWCheckbox_Capture_StripCtrlChars:
            return (t_UICheckboxCtrl *)realwin->ui->checkBox_Cap_StripCtrlChars;
        case e_UIMWCheckbox_Capture_StripEscSeq:
            return (t_UICheckboxCtrl *)realwin->ui->checkBox_Cap_StripEscSeq;
        case e_UIMWCheckbox_Capture_HexDump:
            return (t_UICheckboxCtrl *)realwin->ui->checkBox_Cap_HexDump;
        case e_UIMWCheckbox_HexDisplay_Paused:
            return (t_UICheckboxCtrl *)realwin->ui->checkBox_HexPaused;
        case e_UIMWCheckbox_Bridge_Lock1:
            return (t_UICheckboxCtrl *)realwin->ui->checkBox_Bridge_Lockout1;
        case e_UIMWCheckbox_Bridge_Lock2:
            return (t_UICheckboxCtrl *)realwin->ui->checkBox_Bridge_Lockout2;

        case e_UIMWCheckboxMAX:
        default:
        break;
    }
    return NULL;
}

t_UIListViewCtrl *UIMW_GetListViewHandle(t_UIMainWindow *win,e_UIMWListViewType UIObj)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    switch(UIObj)
    {
        case e_UIMWListView_StopWatch_Laps:
            return (t_UIListViewCtrl *)realwin->ui->listWidget_SW_Laps;
        case e_UIMWListView_Buffers_List:
            return (t_UIListViewCtrl *)realwin->ui->listWidget_Buffer_BufferList;

        case e_UIMWListViewMAX:
        default:
        break;
    }
    return NULL;
}

t_UILabelCtrl *UIMW_GetLabelHandle(t_UIMainWindow *win,e_UIMWLabelType UIObj)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    switch(UIObj)
    {
        case e_UIMWLabel_StopWatch_Time:
            return (t_UILabelCtrl *)realwin->ui->label_SW_Time;
        case e_UIMWLabel_Download_Filename:
            return (t_UILabelCtrl *)realwin->ui->label_DownloadFilename;
        case e_UIMWLabel_Download_BytesRx:
            return (t_UILabelCtrl *)realwin->ui->label_DownloadBytesTrans;
        case e_UIMWLabel_Upload_BytesTx:
            return (t_UILabelCtrl *)realwin->ui->label_UploadBytesTrans;

        case e_UIMWLabelMAX:
        default:
        break;
    }
    return NULL;
}

t_UIComboBoxCtrl *UIMW_GetComboBoxHandle(t_UIMainWindow *win,
        e_UIMWComboBoxType UIObj)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    switch(UIObj)
    {
        case e_UIMWComboBox_Upload_Protocol:
            return (t_UIComboBoxCtrl *)realwin->ui->comboBox_UploadProtocol;
        case e_UIMWComboBox_Download_Protocol:
            return (t_UIComboBoxCtrl *)realwin->ui->comboBox_DownloadProtocol;
        case e_UIMWComboBox_Bridge_Connection1:
            return (t_UIComboBoxCtrl *)realwin->ui->comboBox_Bridge_Con1;
        case e_UIMWComboBox_Bridge_Connection2:
            return (t_UIComboBoxCtrl *)realwin->ui->comboBox_Bridge_Con2;
        case e_UIMWComboBoxMAX:
        default:
        break;
    }
    return NULL;
}

t_UIProgressBarCtrl *UIMW_GetProgressBarHandle(t_UIMainWindow *win,
        e_UIMWProgressBarType UIObj)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    switch(UIObj)
    {
        case e_UIMWProgressBar_Upload:
            return (t_UIProgressBarCtrl *)realwin->ui->progressBar_UploadProgress;
        case e_UIMWProgressBar_Download:
            return (t_UIProgressBarCtrl *)realwin->ui->progressBar_DownloadProgress;
        case e_UIMWProgressBarMAX:
        default:
        break;
    }
    return NULL;
}

t_UIMainWindow *UIMW_AllocMainWindow(class TheMainWindow *MW,uintptr_t ID)
{
    Form_MainWindow *w;

    w=NULL;
    try
    {
        w=new Form_MainWindow();
        w->setObjectName(QString::number(ID));
        w->Setup(MW);
    }
    catch(...)
    {
        if(w!=NULL)
            delete w;
        w=NULL;
    }

    return (t_UIMainWindow *)w;
}

void UIMW_FreeMainWindow(t_UIMainWindow *win)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    delete realwin;
}

void UIMW_ShowMainWindow(t_UIMainWindow *win)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    realwin->show();
}

void UIMW_CloseMainWindow(t_UIMainWindow *win)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    realwin->close();
}

/*******************************************************************************
 * NAME:
 *    UIMW_SetWindowPos
 *
 * SYNOPSIS:
 *    void UIMW_SetWindowPos(t_UIMainWindow *win,int x,int y);
 *
 * PARAMETERS:
 *    win [I] -- The main window to work on
 *    x [I] -- The x pos for the window
 *    y [I] -- The y pos for the window
 *
 * FUNCTION:
 *    This function sets the position of the main window.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 ******************************************************************************/
void UIMW_SetWindowPos(t_UIMainWindow *win,int x,int y)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    realwin->move(x,y);
}

/*******************************************************************************
 * NAME:
 *    UIMW_GetWindowPos
 *
 * SYNOPSIS:
 *    void UIMW_GetWindowPos(t_UIMainWindow *win,int &x,int &y);
 *
 * PARAMETERS:
 *    win [I] -- The main window to work on
 *    x [O] -- The window x pos on the screen
 *    y [O] -- The window y pos on the screen
 *
 * FUNCTION:
 *    This function gets the current window's pos.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIMW_GetWindowPos(t_UIMainWindow *win,int &x,int &y)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    x=realwin->x();
    y=realwin->y();
}

/*******************************************************************************
 * NAME:
 *    UIMW_SetWindowSize
 *
 * SYNOPSIS:
 *    void UIMW_SetWindowSize(t_UIMainWindow *win,int Width,int Height);
 *
 * PARAMETERS:
 *    win [I] -- The main window to work on
 *    Width [I] -- The new width for the main window
 *    Height [I] -- The new height for the main window
 *
 * FUNCTION:
 *    This function sets the width and height of the main window.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UIMW_GetWindowSize
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 ******************************************************************************/
void UIMW_SetWindowSize(t_UIMainWindow *win,int Width,int Height)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    realwin->resize(Width,Height);
}

/*******************************************************************************
 * NAME:
 *    UIMW_GetWindowSize
 *
 * SYNOPSIS:
 *    void UIMW_GetWindowSize(int &Width,int &Height);
 *
 * PARAMETERS:
 *    win [I] -- The main window to work on
 *    Width [O] -- The width of the main window is stored here.
 *    Height [O] -- The height of the main window is stored here.
 *
 * FUNCTION:
 *    This function gets the size of the main window.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 ******************************************************************************/
void UIMW_GetWindowSize(t_UIMainWindow *win,int &Width,int &Height)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    Width=realwin->size().width();
    Height=realwin->size().height();
}

/*******************************************************************************
 * NAME:
 *    UIMW_Maximize
 *
 * SYNOPSIS:
 *    void UIMW_Maximize(t_UIMainWindow *win);
 *
 * PARAMETERS:
 *    win [I] -- The main window to work on
 *
 * FUNCTION:
 *    This function maximizes a main window.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIMW_Maximize(t_UIMainWindow *win)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    realwin->showMaximized();
}

/*******************************************************************************
 * NAME:
 *    UIMW_SetBottomPanel
 *
 * SYNOPSIS:
 *    void UIMW_SetBottomPanel(t_UIMainWindow *win,int NewSize,bool PanelOpen);
 *
 * PARAMETERS:
 *    win [I] -- The main window to work on
 *    NewSize [I] -- The new size for this panel
 *    PanelOpen [I] -- Is the panel open or closed (true=open)
 *
 * FUNCTION:
 *    This function sets the bottom panels size and if it is open or closed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIMW_SetBottomPanel(t_UIMainWindow *win,int NewSize,bool PanelOpen)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;
    int TotalSpace;
    QList<int> newSizes;
    QList<int> CurrentSizes;

    realwin->BottomPanelLastSize=NewSize;
    realwin->BottomClosed=!PanelOpen;

    CurrentSizes=realwin->ui->TopBottomSplitter->sizes();
    TotalSpace=CurrentSizes[0]+CurrentSizes[1];

    if(PanelOpen)
    {
        /* Restore the saved height */
        newSizes.append(TotalSpace-NewSize);
        newSizes.append(NewSize);
    }
    else
    {
        /* Close the panel */
        newSizes.append(TotalSpace);
        newSizes.append(16); // the min size of a hidden panel is 16
    }

    realwin->ui->TopBottomSplitter->setSizes(newSizes);
    realwin->EnableBottomSplitter(PanelOpen);
    realwin->ui->BottomResizeFrame->ClearHighlight();
    realwin->on_TopBottomSplitter_splitterMoved(0,0);
}

/*******************************************************************************
 * NAME:
 *    UIMW_SetLeftPanel
 *
 * SYNOPSIS:
 *    void UIMW_SetLeftPanel(t_UIMainWindow *win,int NewSize,bool PanelOpen);
 *
 * PARAMETERS:
 *    win [I] -- The main window to work on
 *    NewSize [I] -- The new size for this panel
 *    PanelOpen [I] -- Is the panel open or closed (true=open)
 *
 * FUNCTION:
 *    This function sets the left panels size and if it is open or closed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIMW_SetLeftPanel(t_UIMainWindow *win,int NewSize,bool PanelOpen)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;
    int TotalSpace;
    QList<int> newSizes;
    QList<int> CurrentSizes;
    int CalSize;
    int OtherPanelWidth;

    realwin->LeftPanelLastSize=NewSize;
    realwin->LeftClosed=!PanelOpen;

    CurrentSizes=realwin->ui->LeftRightSplitter->sizes();
    TotalSpace=CurrentSizes[0]+CurrentSizes[1]+CurrentSizes[2];
    OtherPanelWidth=CurrentSizes[2];

    if(PanelOpen)
    {
        /* Restore the saved width */
        newSizes.append(NewSize);
        newSizes.append(TotalSpace-OtherPanelWidth-NewSize);
        newSizes.append(OtherPanelWidth);
    }
    else
    {
        /* Close the panel */
        CalSize=16; // the min size of a hidden panel is 16

        newSizes.append(CalSize);
        newSizes.append(TotalSpace-OtherPanelWidth-CalSize);
        newSizes.append(OtherPanelWidth);
    }

    realwin->EnableLeftSplitter(PanelOpen);

    realwin->ui->LeftRightSplitter->setSizes(newSizes);
    realwin->ui->LeftResizeFrame->ClearHighlight();
    realwin->on_LeftRightSplitter_splitterMoved(0,1);
}

/*******************************************************************************
 * NAME:
 *    UIMW_SetRightPanel
 *
 * SYNOPSIS:
 *    void UIMW_SetRightPanel(t_UIMainWindow *win,int NewSize,bool PanelOpen);
 *
 * PARAMETERS:
 *    win [I] -- The main window to work on
 *    NewSize [I] -- The new size for this panel
 *    PanelOpen [I] -- Is the panel open or closed (true=open)
 *
 * FUNCTION:
 *    This function sets the right panels size and if it is open or closed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIMW_SetRightPanel(t_UIMainWindow *win,int NewSize,bool PanelOpen)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;
    int TotalSpace;
    QList<int> newSizes;
    QList<int> CurrentSizes;
    int OtherPanelWidth;
    int CalSize;

    realwin->RightPanelLastSize=NewSize;
    realwin->RightClosed=!PanelOpen;

    CurrentSizes=realwin->ui->LeftRightSplitter->sizes();
    TotalSpace=CurrentSizes[0]+CurrentSizes[1]+CurrentSizes[2];
    OtherPanelWidth=CurrentSizes[0];

    if(PanelOpen)
    {
        /* Restore the saved width */
        newSizes.append(OtherPanelWidth);
        newSizes.append(TotalSpace-OtherPanelWidth-NewSize);
        newSizes.append(NewSize);
    }
    else
    {
        CalSize=16; // the min size of a hidden panel is 16

        newSizes.append(OtherPanelWidth);
        newSizes.append(TotalSpace-OtherPanelWidth-CalSize);
        newSizes.append(CalSize);
    }

    realwin->EnableRightSplitter(PanelOpen);

    realwin->ui->LeftRightSplitter->setSizes(newSizes);
    realwin->ui->RightResizeFrame->ClearHighlight();
    realwin->on_LeftRightSplitter_splitterMoved(0,2);
}

/*******************************************************************************
 * NAME:
 *    UIMW_GetContainerFrameCtrlHandle
 *
 * SYNOPSIS:
 *    t_UIContainerFrameCtrl *UIMW_GetContainerFrameCtrlHandle(
 *              t_UIMainWindow *win);
 *
 * PARAMETERS:
 *    win [I] -- The main window with the control to get the handle for
 *
 * FUNCTION:
 *    This function gets a handle to the standalone frame that can be used
 *    to take up the whole tab area (for when you don't want tabs)
 *
 * RETURNS:
 *    A handle to the container frame.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIContainerFrameCtrl *UIMW_GetContainerFrameCtrlHandle(t_UIMainWindow *win)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    return (t_UIContainerFrameCtrl *)realwin->ui->ContainerFrame;
}

/*******************************************************************************
 * NAME:
 *    UIMW_SetContainerFrameVisible
 *
 * SYNOPSIS:
 *    void UIMW_SetContainerFrameVisible(t_UIContainerFrameCtrl *ctrl,
 *              bool Show);
 *
 * PARAMETERS:
 *    ctrl [I] -- The control to work on
 *    Show [I] -- true = show it, false = hide it
 *
 * FUNCTION:
 *    This function shows / hides a container frame.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIMW_SetContainerFrameVisible(t_UIContainerFrameCtrl *ctrl,bool Show)
{
    QFrame *Frame=(QFrame *)ctrl;

    Frame->setVisible(Show);
}

/*******************************************************************************
 * NAME:
 *    UIMW_GetOptionsFrameContainer
 *
 * SYNOPSIS:
 *    t_UIContainerCtrl *UIMW_GetOptionsFrameContainer(t_UIMainWindow *win);
 *
 * PARAMETERS:
 *    win [I] -- The main window that has the control in it.
 *
 * FUNCTION:
 *    This function gets the frame for the connection options tab (where
 *    you place the widgets for connection options).
 *
 * RETURNS:
 *    A handle to the connection options frame.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIContainerCtrl *UIMW_GetOptionsFrameContainer(t_UIMainWindow *win)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    return (t_UIContainerCtrl *)realwin->ui->formLayout_ConOptions;
}

/*******************************************************************************
 * NAME:
 *    UIMW_GetUploadOptionsFrameContainer
 *
 * SYNOPSIS:
 *    t_UIContainerCtrl *UIMW_GetUploadOptionsFrameContainer(t_UIMainWindow *win);
 *
 * PARAMETERS:
 *    win [I] -- The main window that has the control in it.
 *
 * FUNCTION:
 *    This function gets the frame for the upload options tab (where
 *    you place the widgets for upload options).
 *
 * RETURNS:
 *    A handle to the upload options frame.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIContainerCtrl *UIMW_GetUploadOptionsFrameContainer(t_UIMainWindow *win)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    return (t_UIContainerCtrl *)realwin->ui->formLayout_UploadOptions;
}

/*******************************************************************************
 * NAME:
 *    UIMW_GetDownloadOptionsFrameContainer
 *
 * SYNOPSIS:
 *    t_UIContainerCtrl *UIMW_GetDownloadOptionsFrameContainer(t_UIMainWindow *win);
 *
 * PARAMETERS:
 *    win [I] -- The main window that has the control in it.
 *
 * FUNCTION:
 *    This function gets the frame for the upload options tab (where
 *    you place the widgets for upload options).
 *
 * RETURNS:
 *    A handle to the upload options frame.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIContainerCtrl *UIMW_GetDownloadOptionsFrameContainer(t_UIMainWindow *win)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    return (t_UIContainerCtrl *)realwin->ui->formLayout_DownloadOptions;
}

void UIMW_SetWindowTitle(t_UIMainWindow *win,const char *Title)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    realwin->setWindowTitle(Title);
}

void UIMW_SwitchTabControlCloseBttnPos(t_UIMainWindow *win,bool OnTab)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;
    MyCloseBttn *CloseBttn;

    if(OnTab)
    {
        realwin->ui->Main_tabWidget->setTabsClosable(true);

        CloseBttn=(MyCloseBttn *)realwin->ui->Main_tabWidget->cornerWidget(Qt::TopRightCorner);
        if(CloseBttn!=NULL)
        {
            realwin->ui->Main_tabWidget->setCornerWidget(NULL, Qt::TopRightCorner);
            delete CloseBttn;
        }
    }
    else
    {
        realwin->ui->Main_tabWidget->setTabsClosable(false);
        realwin->ui->Main_tabWidget->setVisible(false);   /* Must hide for the widget to be added */
        CloseBttn = new MyCloseBttn(realwin->ui->Main_tabWidget);
        CloseBttn->setGeometry(QRect(0, 0, CLOSEBTTN_WIDTH,CLOSEBTTN_HEIGHT));

        realwin->ui->Main_tabWidget->setCornerWidget(CloseBttn, Qt::TopRightCorner);
        realwin->ui->Main_tabWidget->setVisible(true);

        QObject::connect(CloseBttn, SIGNAL(Clicked(void)),
                         realwin, SLOT(GlobalCloseTabBttnClicked(void)));
    }
}

e_UIMenuCtrl *UIMW_AddBookmarkMenuItem(t_UIMainWindow *win,
        e_UISubMenuCtrl *Parent,const char *Title,uintptr_t ID)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;
    QMenu *ParentMenu=(QMenu *)Parent;
    QAction *NewAction;
    QMenu *UseParent;

    try
    {
        UseParent=ParentMenu;
        if(UseParent==NULL)
            UseParent=realwin->ui->menuBookmarks;

        NewAction=UseParent->addAction(Title);
        NewAction->setObjectName(QString::number(ID));

        realwin->connect(NewAction, SIGNAL(triggered()), realwin,
                SLOT(actionBookmarkMenuItem_triggered()));

        realwin->BookmarkMenuItems.push_back(NewAction);
    }
    catch(...)
    {
        return NULL;
    }
    return (e_UIMenuCtrl *)NewAction;
}

void UIMW_AddBookmarkClearAllMenus(t_UIMainWindow *win)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;
    t_MWListOfActionsType::iterator a;
    t_MWListOfMenusType::iterator m;

    for(a=realwin->BookmarkMenuItems.begin();
            a!=realwin->BookmarkMenuItems.end();a++)
    {
        delete *a;
    }
    realwin->BookmarkMenuItems.clear();

    for(m=realwin->BookmarkMenus.begin();m!=realwin->BookmarkMenus.end();m++)
    {
        delete *m;
    }
    realwin->BookmarkMenus.clear();
}

e_UISubMenuCtrl *UIMW_AddBookmarkSubMenu(t_UIMainWindow *win,const char *Title)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;
    QMenu *NewMenu;

    try
    {
        NewMenu=realwin->ui->menuBookmarks->addMenu(Title);
        realwin->BookmarkMenus.push_back(NewMenu);
    }
    catch(...)
    {
        return NULL;
    }
    return (e_UISubMenuCtrl *)NewMenu;
}

/* Upload menu */
e_UIMenuCtrl *UIMW_AddFTPUploadMenuItem(t_UIMainWindow *win,const char *Title,
        uintptr_t ID)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;
    QAction *NewAction;
    QMenu *UseParent;

    try
    {
        UseParent=realwin->ui->menu_Upload;

        NewAction=UseParent->addAction(Title);
        NewAction->setObjectName(QString::number(ID));

        realwin->connect(NewAction, SIGNAL(triggered()), realwin,
                SLOT(actionFTPUploadMenuItem_triggered()));

        realwin->FTPUploadMenuItems.push_back(NewAction);
    }
    catch(...)
    {
        return NULL;
    }
    return (e_UIMenuCtrl *)NewAction;
}

void UIMW_AddFTPUploadClearAllMenus(t_UIMainWindow *win)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;
    t_MWListOfActionsType::iterator a;

    for(a=realwin->FTPUploadMenuItems.begin();
            a!=realwin->FTPUploadMenuItems.end();a++)
    {
        delete *a;
    }
    realwin->FTPUploadMenuItems.clear();
}

/* Download menu */
e_UIMenuCtrl *UIMW_AddFTPDownloadMenuItem(t_UIMainWindow *win,const char *Title,
        uintptr_t ID)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;
    QAction *NewAction;
    QMenu *UseParent;

    try
    {
        UseParent=realwin->ui->menu_Download;

        NewAction=UseParent->addAction(Title);
        NewAction->setObjectName(QString::number(ID));

        realwin->connect(NewAction, SIGNAL(triggered()), realwin,
                SLOT(actionFTPDownloadMenuItem_triggered()));

        realwin->FTPDownloadMenuItems.push_back(NewAction);
    }
    catch(...)
    {
        return NULL;
    }
    return (e_UIMenuCtrl *)NewAction;
}

void UIMW_AddFTPDownloadClearAllMenus(t_UIMainWindow *win)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;
    t_MWListOfActionsType::iterator a;

    for(a=realwin->FTPDownloadMenuItems.begin();
            a!=realwin->FTPDownloadMenuItems.end();a++)
    {
        delete *a;
    }
    realwin->FTPDownloadMenuItems.clear();
}

/*******************************************************************************
 * NAME:
 *    UIMW_GetHexDisplayContainerFrameCtrlHandle
 *
 * SYNOPSIS:
 *    t_UIContainerFrameCtrl *UIMW_GetHexDisplayContainerFrameCtrlHandle(
 *              t_UIMainWindow *win);
 *
 * PARAMETERS:
 *    win [I] -- The main window with the control to get the handle for
 *
 * FUNCTION:
 *    This function gets a handle to hex display frame that can be used
 *    to display the hex view.
 *
 * RETURNS:
 *    A handle to the container frame.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIContainerFrameCtrl *UIMW_GetHexDisplayContainerFrameCtrlHandle(t_UIMainWindow *win)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    return (t_UIContainerFrameCtrl *)realwin->ui->frame_HexDisplayContainer;
}

/*******************************************************************************
 * NAME:
 *    UIMW_GetSendBuffersContainerFrameCtrlHandle
 *
 * SYNOPSIS:
 *    t_UIContainerFrameCtrl *UIMW_GetSendBuffersContainerFrameCtrlHandle(
 *              t_UIMainWindow *win);
 *
 * PARAMETERS:
 *    win [I] -- The main window with the control to get the handle for
 *
 * FUNCTION:
 *    This function gets a handle to send buffers hex display frame that can
 *    be used to display the a hex view.
 *
 * RETURNS:
 *    A handle to the container frame.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIContainerFrameCtrl *UIMW_GetSendBuffersContainerFrameCtrlHandle(t_UIMainWindow *win)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    return (t_UIContainerFrameCtrl *)realwin->ui->frame_Buffer_HexDisplayContainer;
}

/*******************************************************************************
 * NAME:
 *    UIMW_EnableStopWatchTimer
 *
 * SYNOPSIS:
 *    void UIMW_EnableStopWatchTimer(bool Enable);
 *
 * PARAMETERS:
 *    Enable [I] -- Start sending events (true), or stop (false)
 *
 * FUNCTION:
 *    This function enables / disables the stop watch timer.
 *
 *    The stop watch timer makes the following events:
 *      e_MWEvent_StopWatch_Timer
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIMW_EnableStopWatchTimer(t_UIMainWindow *win,bool Enable)
{
    Form_MainWindow *realwin=(Form_MainWindow *)win;

    if(Enable)
        realwin->StopWatchTimer->start();
    else
        realwin->StopWatchTimer->stop();
}
