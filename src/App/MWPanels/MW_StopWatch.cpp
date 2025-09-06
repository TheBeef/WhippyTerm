/*******************************************************************************
 * FILENAME: MW_StopWatch.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the main window's stop watch panel tab in it.
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
 *    Paul Hutchinson (03 Mar 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/MWPanels/MW_StopWatch.h"
#include "App/MWPanels/MWPanels.h"
#include "App/MainWindow.h"
#include "App/Connections.h"
#include "App/Settings.h"
#include "UI/UISystem.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
MWStopWatch::MWStopWatch()
{
    UIWin=NULL;
    MW=NULL;

    PanelActive=false;
}

MWStopWatch::~MWStopWatch()
{
}

/*******************************************************************************
 * NAME:
 *    MWStopWatch::Setup
 *
 * SYNOPSIS:
 *    void MWStopWatch::Setup(class TheMainWindow *Parent,
 *              t_UIMainWindow *Win);
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
void MWStopWatch::Setup(class TheMainWindow *Parent,t_UIMainWindow *Win)
{
    MW=Parent;
    UIWin=Win;
}

/*******************************************************************************
 * NAME:
 *    MWStopWatch::ActivateCtrls
 *
 * SYNOPSIS:
 *    void MWStopWatch::ActivateCtrls(bool Active);
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
void MWStopWatch::ActivateCtrls(bool Active)
{
    PanelActive=Active;
    EnableAllControls(Active);
}

/*******************************************************************************
 * NAME:
 *    MWStopWatch::EnableAllControls
 *
 * SYNOPSIS:
 *    void MWStopWatch::EnableAllControls(bool Enabled);
 *
 * PARAMETERS:
 *    Enabled [I] -- Enable all the controls (true), or disable them (false)
 *
 * FUNCTION:
 *    This function enables/disables all the controls for the stop watch
 *    panel.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWStopWatch::EnableAllControls(bool Enabled)
{
    t_UIButtonCtrl *StartBttn;
    t_UIButtonCtrl *ResetBttn;
    t_UIButtonCtrl *LabBttn;
    t_UIButtonCtrl *ClearBttn;
    t_UICheckboxCtrl *StartOnTxCheckbox;
    t_UICheckboxCtrl *AutoLapCheckbox;
    t_UIListViewCtrl *LapsListView;
    t_UILabelCtrl *TimeLabel;
    e_UIMenuCtrl *MenuStart;
    e_UIMenuCtrl *MenuStop;
    e_UIMenuCtrl *MenuReset;
    e_UIMenuCtrl *MenuLap;
    e_UIMenuCtrl *MenuClear;
    e_UIMenuCtrl *MenuStartOnTx;
    e_UIMenuCtrl *MenuAutoLap;

    if(UIWin==NULL)
        return;

    StartBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_StopWatch_Start);
    ResetBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_StopWatch_Reset);
    LabBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_StopWatch_Lap);
    ClearBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_StopWatch_Clear);
    LapsListView=UIMW_GetListViewHandle(UIWin,e_UIMWListView_StopWatch_Laps);
    TimeLabel=UIMW_GetLabelHandle(UIWin,e_UIMWLabel_StopWatch_Time);
    StartOnTxCheckbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_StopWatch_StartOnTx);
    AutoLapCheckbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_StopWatch_AutoLap);
    MenuStart=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_StopWatch_Start);
    MenuStop=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_StopWatch_Stop);
    MenuReset=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_StopWatch_Reset);
    MenuLap=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_StopWatch_Lap);
    MenuClear=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_StopWatch_Clear);
    MenuStartOnTx=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_StopWatch_StartOnTx);
    MenuAutoLap=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_StopWatch_AutoLap);

    UIEnableButton(StartBttn,Enabled);
    UIEnableButton(ResetBttn,Enabled);
    UIEnableButton(LabBttn,Enabled);
    UIEnableButton(ClearBttn,Enabled);
    UIEnableCheckbox(StartOnTxCheckbox,Enabled);
    UIEnableCheckbox(AutoLapCheckbox,Enabled);
    UIEnableListView(LapsListView,Enabled);
    UIEnableLabel(TimeLabel,Enabled);
    UIEnableMenu(MenuStart,Enabled);
    UIEnableMenu(MenuStop,Enabled);
    UIEnableMenu(MenuReset,Enabled);
    UIEnableMenu(MenuLap,Enabled);
    UIEnableMenu(MenuClear,Enabled);
    UIEnableMenu(MenuStartOnTx,Enabled);
    UIEnableMenu(MenuAutoLap,Enabled);

    if(!Enabled)
    {
        UISetButtonLabel(StartBttn,"Start");
        UIMW_EnableStopWatchTimer(UIWin,false);
        if(MW->ActiveCon!=NULL)
            MW->ActiveCon->StopWatchStartStop(false);
    }
    else
    {
        UIEnableMenu(MenuStop,false);
    }

    UpdateDisplayedTime();
}

/*******************************************************************************
 * NAME:
 *    MWStopWatch::UpdateDisplayedTime
 *
 * SYNOPSIS:
 *    void MWStopWatch::UpdateDisplayedTime(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function updates the display time label.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWStopWatch::UpdateDisplayedTime(void)
{
    t_UILabelCtrl *TimeLabel;
    char buff[100];
    int Hour;
    int Min;
    int Sec;
    int MSec;
    uint64_t TimeDelta;

    if(MW->ActiveCon==NULL)
        return;

    TimeDelta=MW->ActiveCon->StopWatchGetTime();

    MSec=TimeDelta%1000;
    Sec=(TimeDelta/1000)%60;
    Min=(TimeDelta/(1000*60))%60;
    Hour=(TimeDelta/(1000*60*60))%60;

    sprintf(buff,"%02d:%02d:%02d:%03d",Hour,Min,Sec,MSec);

    TimeLabel=UIMW_GetLabelHandle(UIWin,e_UIMWLabel_StopWatch_Time);
    UISetLabelText(TimeLabel,buff);
}

/*******************************************************************************
 * NAME:
 *    MWStopWatch::StartTimer
 *
 * SYNOPSIS:
 *    void MWStopWatch::StartTimer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function starts the stop watch timer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWStopWatch::StartTimer(void)
{
    if(!PanelActive || MW->ActiveCon==NULL)
        return;

    MW->ActiveCon->StopWatchStartStop(true);

    UpdateUIForStartStop(true);

    MW->HandlePanelAutoCloseRight();

    if(g_Settings.StopWatchShowPanel)
        MW->ShowPanel(e_MWPanels_StopWatch);
}

/*******************************************************************************
 * NAME:
 *    MWStopWatch::StopTimer
 *
 * SYNOPSIS:
 *    void MWStopWatch::StopTimer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function stops the stop watch timer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWStopWatch::StopTimer(void)
{
    if(!PanelActive || MW->ActiveCon==NULL)
        return;

    MW->ActiveCon->StopWatchStartStop(false);
    UpdateUIForStartStop(false);

    if(g_Settings.StopWatchShowPanel)
        MW->ShowPanel(e_MWPanels_StopWatch);
}

/*******************************************************************************
 * NAME:
 *    MWStopWatch::ResetTimer
 *
 * SYNOPSIS:
 *    void MWStopWatch::ResetTimer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function resets the stop watch timer to 0.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWStopWatch::ResetTimer(void)
{
    if(!PanelActive || MW->ActiveCon==NULL)
        return;

    MW->ActiveCon->StopWatchReset();

    UpdateDisplayedTime();

    if(MW->ActiveCon->GetStopWatchRunning())
        MW->HandlePanelAutoCloseRight();

    if(g_Settings.StopWatchShowPanel)
        MW->ShowPanel(e_MWPanels_StopWatch);
}

/*******************************************************************************
 * NAME:
 *    MWStopWatch::ClearLaps
 *
 * SYNOPSIS:
 *    void MWStopWatch::ClearLaps(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clear the laps display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWStopWatch::ClearLaps(void)
{
    t_UIListViewCtrl *LapsCtrl;

    if(!PanelActive || MW->ActiveCon==NULL)
        return;

    MW->ActiveCon->StopWatchClearLaps();

    LapsCtrl=UIMW_GetListViewHandle(UIWin,e_UIMWListView_StopWatch_Laps);

    UIClearListView(LapsCtrl);

    if(g_Settings.StopWatchShowPanel)
        MW->ShowPanel(e_MWPanels_StopWatch);
}

/*******************************************************************************
 * NAME:
 *    MWStopWatch::AddLap
 *
 * SYNOPSIS:
 *    void MWStopWatch::AddLap(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function takes the current time and adds a lap entry to the labs
 *    display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWStopWatch::AddLap(void)
{
    if(MW->ActiveCon==NULL)
        return;

    MW->ActiveCon->StopWatchTakeLap();

    if(g_Settings.StopWatchShowPanel)
        MW->ShowPanel(e_MWPanels_StopWatch);
}

/*******************************************************************************
 * NAME:
 *    MWStopWatch::TimerAutoStarted
 *
 * SYNOPSIS:
 *    void MWStopWatch::TimerAutoStarted(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called to tell us that the timer has been auto started
 *    and we need to update the UI.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWStopWatch::TimerAutoStarted(void)
{
    UpdateUIForStartStop(true);
}

/*******************************************************************************
 * NAME:
 *    MWStopWatch::AddLapLine
 *
 * SYNOPSIS:
 *    void MWStopWatch::AddLapLine(uint64_t LapTime, uint64_t LapDelta);
 *
 * PARAMETERS:
 *    LapTime [I] -- The amount of time this lap took
 *    LapDelta [I] -- THe amount of time between this lap and the previous
 *
 * FUNCTION:
 *    This function adds a lap entry to the labs display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWStopWatch::AddLapLine(uint64_t LapTime, uint64_t LapDelta)
{
    t_UIListViewCtrl *LapsCtrl;
    char buff[100];
    int Hour;
    int Min;
    int Sec;
    int MSec;
    int DHour;
    int DMin;
    int DSec;
    int end;

    if(!PanelActive)
        return;

    MSec=LapTime%1000;
    Sec=(LapTime/1000)%60;
    Min=(LapTime/(1000*60))%60;
    Hour=(LapTime/(1000*60*60))%60;

    /* How long has it been since the last lap */
    DSec=(LapDelta/1000)%60;
    DMin=(LapDelta/(1000*60))%60;
    DHour=(LapDelta/(1000*60*60))%60;

    end=sprintf(buff,"%02d:%02d:%02d:%03d (",Hour,Min,Sec,MSec);
    if(DHour>0)
        end+=sprintf(&buff[end],"%02d:",DHour);
    if(DHour>0 || DMin>0)
        end+=sprintf(&buff[end],"%02d:",DMin);

    end+=sprintf(&buff[end],"%02d)",DSec);

    LapsCtrl=UIMW_GetListViewHandle(UIWin,e_UIMWListView_StopWatch_Laps);

    UIAddItem2ListView(LapsCtrl,buff,0);
    UIScrollListViewToBottom(LapsCtrl);

    UpdateDisplayedTime();
}

/*******************************************************************************
 * NAME:
 *    MWStopWatch::ToggleStartStop
 *
 * SYNOPSIS:
 *    void MWStopWatch::ToggleStartStop(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function toggles the timer on / off
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWStopWatch::ToggleStartStop(void)
{
    if(MW->ActiveCon==NULL)
        return;

    if(MW->ActiveCon->GetStopWatchRunning())
        StopTimer();
    else
        StartTimer();
}

/*******************************************************************************
 * NAME:
 *    MWStopWatch::UpdateUIForStartStop
 *
 * SYNOPSIS:
 *    void MWStopWatch::UpdateUIForStartStop(bool Start);
 *
 * PARAMETERS:
 *    Start [I] -- Are we starting or stoping?
 *
 * FUNCTION:
 *    This function updates the UI when the stop watch starts / stops.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWStopWatch::UpdateUIForStartStop(bool Start)
{
    t_UIButtonCtrl *StartBttn;
    e_UIMenuCtrl *MenuStart;
    e_UIMenuCtrl *MenuStop;

    if(!PanelActive || MW->ActiveCon==NULL)
        return;

    StartBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_StopWatch_Start);
    MenuStart=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_StopWatch_Start);
    MenuStop=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_StopWatch_Stop);

    UIMW_EnableStopWatchTimer(UIWin,Start);

    UIEnableMenu(MenuStart,!Start);
    UIEnableMenu(MenuStop,Start);

    if(Start)
        UISetButtonLabel(StartBttn,"Stop");
    else
        UISetButtonLabel(StartBttn,"Start");
}

/*******************************************************************************
 * NAME:
 *    MWStopWatch::ToggleAutoStartOnTx
 *
 * SYNOPSIS:
 *    void MWStopWatch::ToggleAutoStartOnTx(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function toggles the auto start on tx
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWStopWatch::ToggleAutoStartOnTx(void)
{
    e_UIMenuCtrl *MenuStartOnTx;
    t_UICheckboxCtrl *StartOnTxCheckbox;
    bool AutoStart;
    bool AutoLap;

    if(!PanelActive || MW->ActiveCon==NULL)
        return;

    MW->ActiveCon->GetStopWatchOptions(AutoStart,AutoLap);
    AutoStart=!AutoStart;
    MW->ActiveCon->SetStopWatchOptions(AutoStart,AutoLap);

    MenuStartOnTx=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_StopWatch_StartOnTx);
    StartOnTxCheckbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_StopWatch_StartOnTx);

    UICheckMenu(MenuStartOnTx,AutoStart);
    UICheckCheckbox(StartOnTxCheckbox,AutoStart);
}

/*******************************************************************************
 * NAME:
 *    MWStopWatch::ToggleAutoLap
 *
 * SYNOPSIS:
 *    void MWStopWatch::ToggleAutoLap(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function toggles the auto lap
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWStopWatch::ToggleAutoLap(void)
{
    e_UIMenuCtrl *MenuAutoLap;
    t_UICheckboxCtrl *AutoLapCheckbox;
    bool AutoStart;
    bool AutoLap;

    if(!PanelActive || MW->ActiveCon==NULL)
        return;

    MW->ActiveCon->GetStopWatchOptions(AutoStart,AutoLap);
    AutoLap=!AutoLap;
    MW->ActiveCon->SetStopWatchOptions(AutoStart,AutoLap);

    MenuAutoLap=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_StopWatch_AutoLap);
    AutoLapCheckbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_StopWatch_AutoLap);

    UICheckMenu(MenuAutoLap,AutoLap);
    UICheckCheckbox(AutoLapCheckbox,AutoLap);
}

/*******************************************************************************
 * NAME:
 *    MWStopWatch::ConnectionAbout2Changed
 *
 * SYNOPSIS:
 *    void MWStopWatch::ConnectionAbout2Changed(void);
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
void MWStopWatch::ConnectionAbout2Changed(void)
{
    /* This is do differently, it updates the connection has controls are
       changed on the UI, so this function is not needed.  It would have
       been easier to just use this function, but it didn't exist when
       this panel was done */
}

/*******************************************************************************
 * NAME:
 *    MWStopWatch::ConnectionChanged
 *
 * SYNOPSIS:
 *    void MWStopWatch::ConnectionChanged(void);
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
void MWStopWatch::ConnectionChanged(void)
{
    e_UIMenuCtrl *MenuStartOnTx;
    t_UICheckboxCtrl *StartOnTxCheckbox;
    e_UIMenuCtrl *MenuAutoLap;
    t_UICheckboxCtrl *AutoLapCheckbox;
    t_UIListViewCtrl *LapsCtrl;
    uint64_t LapTime;
    uint64_t LastLapTime;
    bool AutoStart;
    bool AutoLap;

    if(MW->ActiveCon==NULL)
        return;

    MenuStartOnTx=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_StopWatch_StartOnTx);
    StartOnTxCheckbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_StopWatch_StartOnTx);
    MenuAutoLap=UIMW_GetMenuHandle(UIWin,e_UIMWMenu_StopWatch_AutoLap);
    AutoLapCheckbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_StopWatch_AutoLap);
    LapsCtrl=UIMW_GetListViewHandle(UIWin,e_UIMWListView_StopWatch_Laps);

    MW->ActiveCon->GetStopWatchOptions(AutoStart,AutoLap);

    UICheckMenu(MenuStartOnTx,AutoStart);
    UICheckCheckbox(StartOnTxCheckbox,AutoStart);

    UICheckMenu(MenuAutoLap,AutoLap);
    UICheckCheckbox(AutoLapCheckbox,AutoLap);

    UpdateUIForStartStop(MW->ActiveCon->GetStopWatchRunning());

    /* Get all the laps from the connection and add them to the display */
    UIClearListView(LapsCtrl);

    if(MW->ActiveCon->StopWatchGetNextLapInfo(true,LapTime))
    {
        LastLapTime=LapTime;
        do
        {
            AddLapLine(LapTime,LapTime-LastLapTime);
            LastLapTime=LapTime;
        } while(MW->ActiveCon->StopWatchGetNextLapInfo(false,LapTime));
    }

    UpdateDisplayedTime();
}
