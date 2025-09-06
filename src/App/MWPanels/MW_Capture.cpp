/*******************************************************************************
 * FILENAME: MW_Capture.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This has the capture panel in it.
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
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
 *    Paul Hutchinson (09 Mar 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/MWPanels/MW_Capture.h"
#include "App/Connections.h"
#include "App/MainWindow.h"
#include "App/Settings.h"
#include "UI/UIFileReq.h"
#include "UI/UIAsk.h"
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

MWCapture::MWCapture()
{
    UIWin=NULL;
    MW=NULL;

    PanelActive=false;
}

MWCapture::~MWCapture()
{
}

/*******************************************************************************
 * NAME:
 *    MWCapture::Setup
 *
 * SYNOPSIS:
 *    void MWCapture::Setup(class TheMainWindow *Parent,t_UIMainWindow *Win);
 *
 * PARAMETERS:
 *    Parent [I] -- The main window that this function lives in
 *    Win [I] -- The UI window handle.
 *
 * FUNCTION:
 *    This function setups things needed by this class.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWCapture::Setup(class TheMainWindow *Parent,t_UIMainWindow *Win)
{
    MW=Parent;
    UIWin=Win;
}

/*******************************************************************************
 * NAME:
 *    MWCapture::ActivateCtrls
 *
 * SYNOPSIS:
 *    void MWCapture::ActivateCtrls(bool Active);
 *
 * PARAMETERS:
 *    Active [I] -- Should we activate the controls (true), or deactivate them
 *                  (false).
 *
 * FUNCTION:
 *    This function activates/deactivates the controls for this panel.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWCapture::ActivateCtrls(bool Active)
{
    PanelActive=Active;

    if(UIWin==NULL)
        return;

    if(!Active)
    {
        /* Stop the save */
        Stop();
    }

    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWCapture::SelectFilename
 *
 * SYNOPSIS:
 *    void MWCapture::SelectFilename(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function prompts the user to for the filename to save to and fills
 *    in the file input.
 *
 * RETURNS:
 *    true -- We selected a filename
 *    false -- User canceled.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool MWCapture::SelectFilename(void)
{
    std::string Path;
    std::string Filename;
    t_UITextInputCtrl *TxtInputFilename;
    bool RetValue;

    if(!PanelActive)
        return false;

    RetValue=false;

    TxtInputFilename=UIMW_GetTxtInputHandle(UIWin,e_UIMWTxtInput_Capture_Filename);

    UIGetTextCtrlText(TxtInputFilename,Path);

    Filename="";
    if(Path=="")
        Filename="Capture.log";
    if(UI_SaveFileReq("Capture to file",Path,Filename,
            "All Files|*\n"
            "Log Files|*.log\n"
            "Binary|*.bin\n",0))
    {
        Filename=UI_ConcatFile2Path(Path,Filename);
        UISetTextCtrlText(TxtInputFilename,Filename.c_str());
        MW->ActiveCon->SetCaptureFilename(Filename.c_str());
        RetValue=true;
    }

    RethinkUI();

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    MWCapture::Prompt4FileAndStart
 *
 * SYNOPSIS:
 *    void MWCapture::Prompt4FileAndStart(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function prompts for the file to save to and then starts the
 *    save.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWCapture::Prompt4FileAndStart(void)
{
    if(SelectFilename())
    {
        Start();

        if(g_Settings.CaptureShowPanel)
            MW->ShowPanel(e_MWPanels_Capture);
    }
}

/*******************************************************************************
 * NAME:
 *    MWCapture::Start
 *
 * SYNOPSIS:
 *    void MWCapture::Start(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function starts the capture.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWCapture::Start(void)
{
    string Filename;
    string ErrorStr;

    if(!PanelActive || MW->ActiveCon==NULL)
        return;

    if(!MW->ActiveCon->StartCapture())
    {
        MW->ActiveCon->GetCaptureFilename(Filename);
        ErrorStr="Failed to capture to \"";
        ErrorStr+=Filename;
        ErrorStr+="\"";
        UIAsk("Error",ErrorStr.c_str(),e_AskBox_Error);
    }
    else
    {
        MW->HandlePanelAutoCloseLeft();
    }
    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWCapture::Stop
 *
 * SYNOPSIS:
 *    void MWCapture::Stop(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function stops capturing and closes the file.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWCapture::Stop(void)
{
    if(MW->ActiveCon==NULL)
        return;

    MW->ActiveCon->StopCapture();

    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWCapture::RethinkUI
 *
 * SYNOPSIS:
 *    void MWCapture::RethinkUI(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rethinks the UI.  It enables / disables things as needed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWCapture::RethinkUI(void)
{
    t_UIButtonCtrl *BttnStart;
    t_UIButtonCtrl *BttnSelectFilename;
    t_UICheckboxCtrl *CheckboxTimestamp;
    t_UICheckboxCtrl *CheckboxAppend;
    t_UICheckboxCtrl *CheckboxStripCtrlChars;
    t_UICheckboxCtrl *CheckboxStripEscSeq;
    t_UICheckboxCtrl *CheckboxHexDump;
    e_UIMenuCtrl *MenuCaptureToFile;
    e_UIMenuCtrl *MenuTimestampToggle;
    e_UIMenuCtrl *MenuAppendToggle;
    e_UIMenuCtrl *MenuStripCtrlCharsToggle;
    e_UIMenuCtrl *MenuStripEscSeqToggle;
    e_UIMenuCtrl *MenuStop;
    e_UIMenuCtrl *MenuHexDump;
    t_UITextInputCtrl *TxtInputFilename;
    bool StartEnabled;
    bool StartBttnIsAStopButton;
    bool StopEnabled;
    bool SelectFileEnabled;
    bool MainCheckboxesEnabled;
    bool HexDumpCheckboxEnabled;
    string Text;
    bool Active;
    struct CaptureToFileOptions Options;

    Active=PanelActive;

    BttnStart=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_Capture_Start);
    BttnSelectFilename=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_Capture_SelectFilename);

    CheckboxTimestamp=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Capture_Timestamp);
    CheckboxAppend=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Capture_Append);
    CheckboxStripCtrlChars=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Capture_StripCtrlChars);
    CheckboxStripEscSeq=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Capture_StripEscSeq);
    CheckboxHexDump=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Capture_HexDump);

    MenuCaptureToFile=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Capture_CaptureToFile);
    MenuTimestampToggle=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Capture_TimestampToggle);
    MenuAppendToggle=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Capture_AppendToggle);
    MenuStripCtrlCharsToggle=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Capture_StripCtrlCharsToggle);
    MenuStripEscSeqToggle=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Capture_StripEscSeqToggle);
    MenuStop=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Capture_Stop);
    MenuHexDump=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Capture_HexDumpToggle);

    TxtInputFilename=UIMW_GetTxtInputHandle(UIWin,e_UIMWTxtInput_Capture_Filename);

    StartBttnIsAStopButton=false;
    StartEnabled=Active;
    StopEnabled=Active;
    SelectFileEnabled=Active;
    MainCheckboxesEnabled=Active;
    HexDumpCheckboxEnabled=Active;
    if(Active)
    {
        if(MW->ActiveCon->GetCaptureSaving())
        {
            StartBttnIsAStopButton=true;
            SelectFileEnabled=false;
            MainCheckboxesEnabled=false;
            HexDumpCheckboxEnabled=false;
            StopEnabled=true;
        }
        else
        {
            StartBttnIsAStopButton=false;
            StopEnabled=false;
            MW->ActiveCon->GetCaptureFilename(Text);
            if(Text=="")
                StartEnabled=false;
            else
                StartEnabled=true;
            SelectFileEnabled=true;

            MainCheckboxesEnabled=true;
            MW->ActiveCon->GetCaptureOptions(Options);
            if(Options.SaveAsHexDump)
            {
                HexDumpCheckboxEnabled=true;
                MainCheckboxesEnabled=false;
            }
        }
    }

    /* Change the name of the start button */
    if(StartBttnIsAStopButton)
        UISetButtonLabel(BttnStart,"Stop");
    else
        UISetButtonLabel(BttnStart,"Start");

    UIEnableButton(BttnStart,StartEnabled);
    UIEnableButton(BttnSelectFilename,SelectFileEnabled);

    UIEnableCheckbox(CheckboxTimestamp,MainCheckboxesEnabled);
    UIEnableCheckbox(CheckboxAppend,MainCheckboxesEnabled);
    UIEnableCheckbox(CheckboxStripCtrlChars,MainCheckboxesEnabled);
    UIEnableCheckbox(CheckboxStripEscSeq,MainCheckboxesEnabled);
    UIEnableCheckbox(CheckboxHexDump,HexDumpCheckboxEnabled);

    UIEnableMenu(MenuCaptureToFile,Active);
    UIEnableMenu(MenuStop,StopEnabled);
    UIEnableMenu(MenuTimestampToggle,MainCheckboxesEnabled);
    UIEnableMenu(MenuAppendToggle,MainCheckboxesEnabled);
    UIEnableMenu(MenuStripCtrlCharsToggle,MainCheckboxesEnabled);
    UIEnableMenu(MenuStripEscSeqToggle,MainCheckboxesEnabled);
    UIEnableMenu(MenuHexDump,HexDumpCheckboxEnabled);

    UIEnableTextCtrl(TxtInputFilename,SelectFileEnabled);
}

/*******************************************************************************
 * NAME:
 *    MWCapture::PressStartBttn
 *
 * SYNOPSIS:
 *    void MWCapture::PressStartBttn(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function handles clicking of the "start" button.  Because the
 *    "start" button can be a start or a stop button this function handles
 *    calling the correct function.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Start(), Stop()
 ******************************************************************************/
void MWCapture::PressStartBttn(void)
{
    if(MW->ActiveCon==NULL)
        return;

    if(MW->ActiveCon->GetCaptureSaving())
        Stop();
    else
        Start();
}

/*******************************************************************************
 * NAME:
 *    MWCapture::NoteFilenameChanged
 *
 * SYNOPSIS:
 *    void MWCapture::NoteFilenameChanged(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when the filename input is changed by the
 *    user.  It rethinks the UI.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWCapture::NoteFilenameChanged(void)
{
    t_UITextInputCtrl *TxtInputFilename;
    string Text;

    if(MW->ActiveCon==NULL)
        return;

    TxtInputFilename=UIMW_GetTxtInputHandle(UIWin,e_UIMWTxtInput_Capture_Filename);
    UIGetTextCtrlText(TxtInputFilename,Text);

    MW->ActiveCon->SetCaptureFilename(Text.c_str());

    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWCapture::ToggleTimestamp
 *
 * SYNOPSIS:
 *    void MWCapture::ToggleTimestamp(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function toggles the timestamp option.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWCapture::ToggleTimestamp(void)
{
    t_UICheckboxCtrl *CheckboxTimestamp;
    e_UIMenuCtrl *MenuTimestampToggle;
    struct CaptureToFileOptions Options;
    bool NewValue;

    if(MW->ActiveCon==NULL)
        return;

    CheckboxTimestamp=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Capture_Timestamp);
    MenuTimestampToggle=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Capture_TimestampToggle);

    MW->ActiveCon->GetCaptureOptions(Options);
    NewValue=!Options.Timestamp;
    MW->ActiveCon->SetCaptureOption_Timestamp(NewValue);

    UICheckCheckbox(CheckboxTimestamp,NewValue);
    UICheckMenu(MenuTimestampToggle,NewValue);
}

/*******************************************************************************
 * NAME:
 *    MWCapture::ToggleAppend
 *
 * SYNOPSIS:
 *    void MWCapture::ToggleAppend(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function toggles the append option.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWCapture::ToggleAppend(void)
{
    t_UICheckboxCtrl *CheckboxAppend;
    e_UIMenuCtrl *MenuAppendToggle;
    struct CaptureToFileOptions Options;
    bool NewValue;

    if(MW->ActiveCon==NULL)
        return;

    CheckboxAppend=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Capture_Append);
    MenuAppendToggle=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Capture_AppendToggle);

    MW->ActiveCon->GetCaptureOptions(Options);
    NewValue=!Options.Append;
    MW->ActiveCon->SetCaptureOption_Append(NewValue);

    UICheckCheckbox(CheckboxAppend,NewValue);
    UICheckMenu(MenuAppendToggle,NewValue);
}

/*******************************************************************************
 * NAME:
 *    MWCapture::ToggleStripCtrlChars
 *
 * SYNOPSIS:
 *    void MWCapture::ToggleStripCtrlChars(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function toggles the strip control chars option.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWCapture::ToggleStripCtrlChars(void)
{
    t_UICheckboxCtrl *CheckboxStripCtrlChars;
    e_UIMenuCtrl *MenuStripCtrlCharsToggle;
    struct CaptureToFileOptions Options;
    bool NewValue;

    if(MW->ActiveCon==NULL)
        return;

    CheckboxStripCtrlChars=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Capture_StripCtrlChars);
    MenuStripCtrlCharsToggle=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Capture_StripCtrlCharsToggle);

    MW->ActiveCon->GetCaptureOptions(Options);
    NewValue=!Options.StripCtrl;
    MW->ActiveCon->SetCaptureOption_StripCtrl(NewValue);

    UICheckCheckbox(CheckboxStripCtrlChars,NewValue);
    UICheckMenu(MenuStripCtrlCharsToggle,NewValue);
}

/*******************************************************************************
 * NAME:
 *    MWCapture::ToggleStripEscSeq
 *
 * SYNOPSIS:
 *    void MWCapture::ToggleStripEscSeq(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function toggles the strip esc sequences option.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWCapture::ToggleStripEscSeq(void)
{
    t_UICheckboxCtrl *CheckboxStripEscSeq;
    e_UIMenuCtrl *MenuStripEscSeqToggle;
    struct CaptureToFileOptions Options;
    bool NewValue;

    if(MW->ActiveCon==NULL)
        return;

    CheckboxStripEscSeq=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Capture_StripEscSeq);
    MenuStripEscSeqToggle=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Capture_StripEscSeqToggle);

    MW->ActiveCon->GetCaptureOptions(Options);
    NewValue=!Options.StripEsc;
    MW->ActiveCon->SetCaptureOption_StripEsc(NewValue);

    UICheckCheckbox(CheckboxStripEscSeq,NewValue);
    UICheckMenu(MenuStripEscSeqToggle,NewValue);
}

/*******************************************************************************
 * NAME:
 *    MWCapture::ToggleHexDump
 *
 * SYNOPSIS:
 *    void MWCapture::ToggleHexDump(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function toggles the hex dump option.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWCapture::ToggleHexDump(void)
{
    t_UICheckboxCtrl *Checkbox;
    e_UIMenuCtrl *MenuItem;
    struct CaptureToFileOptions Options;
    bool NewValue;

    if(MW->ActiveCon==NULL)
        return;

    Checkbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Capture_HexDump);
    MenuItem=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Capture_HexDumpToggle);

    MW->ActiveCon->GetCaptureOptions(Options);
    NewValue=!Options.SaveAsHexDump;
    MW->ActiveCon->SetCaptureOption_HexDump(NewValue);

    UICheckCheckbox(Checkbox,NewValue);
    UICheckMenu(MenuItem,NewValue);

    RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    MWCapture::ConnectionAbout2Changed
 *
 * SYNOPSIS:
 *    void MWCapture::ConnectionAbout2Changed(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called just before the active connection is able to
 *    change to a different connection.
 *
 *    It stores the current UI in to the connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWCapture::ConnectionAbout2Changed(void)
{
    t_UITextInputCtrl *TxtInputFilename;
    string FileName;

    if(MW->ActiveCon==NULL)
        return;

    /* TxtInputFilename doesn't always get it's edited event so we make sure
       it's updated (all controls should be done this way but it was written
       before this function was added) */
    TxtInputFilename=UIMW_GetTxtInputHandle(UIWin,e_UIMWTxtInput_Capture_Filename);

    UIGetTextCtrlText(TxtInputFilename,FileName);
    MW->ActiveCon->SetCaptureFilename(FileName.c_str());
}

/*******************************************************************************
 * NAME:
 *    MWCapture::ConnectionChanged
 *
 * SYNOPSIS:
 *    void MWCapture::ConnectionChanged(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function updates the UI after the active connection has changed
 *    to a new connection (MW->ActiveCon).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWCapture::ConnectionChanged(void)
{
    t_UICheckboxCtrl *CheckboxTimestamp;
    e_UIMenuCtrl *MenuTimestampToggle;
    t_UICheckboxCtrl *CheckboxAppend;
    e_UIMenuCtrl *MenuAppendToggle;
    t_UICheckboxCtrl *CheckboxStripCtrlChars;
    e_UIMenuCtrl *MenuStripCtrlCharsToggle;
    t_UICheckboxCtrl *CheckboxStripEscSeq;
    e_UIMenuCtrl *MenuStripEscSeqToggle;
    t_UICheckboxCtrl *CheckboxHexDump;
    e_UIMenuCtrl *MenuItemHexDump;
    t_UITextInputCtrl *TxtInputFilename;
    struct CaptureToFileOptions Options;
    string FileName;

    if(MW->ActiveCon==NULL)
        return;

    MW->ActiveCon->GetCaptureOptions(Options);

    CheckboxTimestamp=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Capture_Timestamp);
    MenuTimestampToggle=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Capture_TimestampToggle);
    CheckboxAppend=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Capture_Append);
    MenuAppendToggle=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Capture_AppendToggle);
    CheckboxStripCtrlChars=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Capture_StripCtrlChars);
    MenuStripCtrlCharsToggle=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Capture_StripCtrlCharsToggle);
    CheckboxStripEscSeq=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Capture_StripEscSeq);
    MenuStripEscSeqToggle=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Capture_StripEscSeqToggle);
    CheckboxHexDump=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Capture_HexDump);
    MenuItemHexDump=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_Capture_HexDumpToggle);
    TxtInputFilename=UIMW_GetTxtInputHandle(UIWin,e_UIMWTxtInput_Capture_Filename);

    UICheckCheckbox(CheckboxTimestamp,Options.Timestamp);
    UICheckMenu(MenuTimestampToggle,Options.Timestamp);

    UICheckCheckbox(CheckboxAppend,Options.Append);
    UICheckMenu(MenuAppendToggle,Options.Append);

    UICheckCheckbox(CheckboxStripCtrlChars,Options.StripCtrl);
    UICheckMenu(MenuStripCtrlCharsToggle,Options.StripCtrl);

    UICheckCheckbox(CheckboxStripEscSeq,Options.StripEsc);
    UICheckMenu(MenuStripEscSeqToggle,Options.StripEsc);

    UICheckCheckbox(CheckboxHexDump,Options.SaveAsHexDump);
    UICheckMenu(MenuItemHexDump,Options.SaveAsHexDump);

    MW->ActiveCon->GetCaptureFilename(FileName);
    UISetTextCtrlText(TxtInputFilename,FileName.c_str());

    RethinkUI();
}
