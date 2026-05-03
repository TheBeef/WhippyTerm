/*******************************************************************************
 * FILENAME: Dialog_NewVersionCheck.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the check for a new version dialog in it.  This dialog
 *    can also be set to only open if there is actually a new version.
 *
 * COPYRIGHT:
 *    Copyright 21 Apr 2026 Paul Hutchinson.
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
 *    Paul Hutchinson (21 Apr 2026)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/VersionCheckSystem.h"
#include "Dialog_NewVersionCheck.h"
#include "UI/UINewVersionCheck.h"
#include "UI/UIAsk.h"
#include "UI/UISystem.h"
#include "OS/Thread.h"
#include "OS/OSTime.h"
#include <string>

using namespace std;

/*** DEFINES                  ***/
#define     TIMEOUT_QUICK_CHECK_VERSION             1000 /* The system has 1000ms to check for a new version */

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
typedef enum
{
    e_ThreadCmd_Wait,
    e_ThreadCmd_OpenConnection,
    e_ThreadCmd_CloseConnection,
    e_ThreadCmd_ReadVer,
    e_ThreadCmd_Shutdown,
    e_ThreadCmdMAX
} e_ThreadCmdType;

struct ThreadCmd
{
    e_ThreadCmdType Cmd;
    bool Busy;
    bool HadError;
};

/*** FUNCTION PROTOTYPES      ***/
static void Show_Error(const char *ErrorMsg);
static void NVC_Move2NextStep(void);
static void NVC_ExeCurrentStep(void);
static void WorkerThread(void *Data);
static bool CheckForAbort(void);
static bool CheckForAbortWithTimeout(void);

/*** VARIABLE DEFINITIONS     ***/
e_VersionCheckStepType m_CurrentStep;
static bool m_HadError;
static struct ThreadMutex *m_ThreadMutex;
static struct ThreadHandle *m_ThreadHandle;
static volatile struct ThreadCmd m_ThreadCmd;
volatile bool m_AbortConnect;
static uint32_t m_AutoAbortTimeout;

/*******************************************************************************
 * NAME:
 *    CheckForNewVersionDialog
 *
 * SYNOPSIS:
 *    void CheckForNewVersionDialog(bool ShowDialog,bool Quick);
 *
 * PARAMETERS:
 *    ShowDialog [I] -- Do we show the dialog then check for the new version,
 *                      or do we check and only show the dialog if there is
 *                      a new version.
 *    Quick [I] -- If this is true then run the check with is dialog hidden
 *                 and jump directly to the result (unless there's an error
 *                 in which case do the normal dialog).
 *
 * FUNCTION:
 *    This function checks if there is a new version.  If there is a new
 *    version (or 'ShowDialog' is true) the dialog is opened and a button
 *    can be pressed to take them to the web site.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void CheckForNewVersionDialog(bool ShowDialog,bool Quick)
{
    bool AllocatedUI;
    bool OpenDialog;
    bool Busy;
    uint8_t Major;
    uint8_t Minor;
    uint8_t Rev;
    uint8_t Patch;

    m_ThreadMutex=NULL;
    m_ThreadHandle=NULL;
    AllocatedUI=false;
    try
    {
        m_HadError=false;
        m_ThreadCmd.Cmd=e_ThreadCmd_Wait;
        m_ThreadCmd.Busy=false;
        m_ThreadCmd.HadError=false;
        m_CurrentStep=e_VersionCheckStep_Connect;

        OpenDialog=ShowDialog;
        if(!ShowDialog)
        {
            /* Do the actual check and only open dialog if new version exists */
            m_AutoAbortTimeout=GetElapsedTime_ms();
            if(!OpenConnection2WebSite(CheckForAbortWithTimeout))
            {
                /* Switch to the dialog */
                OpenDialog=true;
                Quick=false;
            }
            else
            {
                if(!ReadLatestVersionFromWebSite())
                {
                    /* Switch to the dialog */
                    OpenDialog=true;
                    Quick=false;
                }
                else
                {
                    if(CheckLatestVersionFromWebSite(&Major,&Minor,&Rev,&Patch))
                    {
                        OpenDialog=true;
                    }
                }
                CloseConnection2WebSite();
            }
        }

        if(OpenDialog)
        {
            m_ThreadMutex=AllocMutex();
            if(m_ThreadMutex==NULL)
                throw(0);

            /* Start the worker thread */
            m_ThreadHandle=StartThread(false,WorkerThread,NULL);
            if(m_ThreadHandle==NULL)
                throw(0);

            if(!UIAlloc_NewVersionCheck())
                throw("Failed to open window");

            AllocatedUI=true;

            NVC_SetStepMode(e_VersionCheckStep_Connect,e_VersionCheckMode_Clear);
            NVC_SetStepMode(e_VersionCheckStep_Reading,e_VersionCheckMode_Clear);
            NVC_SetStepMode(e_VersionCheckStep_Closing,e_VersionCheckMode_Clear);
            NVC_SetStepMode(e_VersionCheckStep_Check,e_VersionCheckMode_Clear);
            NVC_ShowHideNewVersionFound(false);
            NVC_EnableOkBttn(false);

            if(Quick)
            {
                /* Skip to the check (MAX = done, just show result) */
                m_CurrentStep=e_VersionCheckStepMAX;
            }
            else
            {
                /* Start first step */
                m_CurrentStep=e_VersionCheckStep_Connect;
            }
            NVC_ExeCurrentStep();

            if(UIShow_NewVersionCheck())
            {
            }
        }
    }
    catch(const char *Error)
    {
        UIAsk("Error opening check for new version dialog",Error,e_AskBox_Error,
                e_AskBttns_Ok);
    }
    catch(...)
    {
        UIAsk("Error opening check for new version dialog",
                "Failed check for new version",
                e_AskBox_Error,e_AskBttns_Ok);
    }

    if(AllocatedUI)
        UIFree_NewVersionCheck();

    if(m_ThreadHandle!=NULL)
    {
        /* Abort the connect (if not connecting then this does nothing) then
           give the thread a command to exit */
        m_AbortConnect=true;

        /* Wait for the thread to be done */
        Busy=true;
        do
        {
            LockMutex(m_ThreadMutex);
            Busy=m_ThreadCmd.Busy;
            UnLockMutex(m_ThreadMutex);
        } while(Busy);

        LockMutex(m_ThreadMutex);
        m_ThreadCmd.Cmd=e_ThreadCmd_Shutdown;
        UnLockMutex(m_ThreadMutex);
        Wait4ThreadToExit(m_ThreadHandle);
    }

    if(m_ThreadMutex!=NULL)
        FreeMutex(m_ThreadMutex);
}

bool NVC_Event(const struct NVCEvent *Event)
{
    bool Busy;
    bool HadError;

    switch(Event->EventType)
    {
        case e_NVCEvent_BttnTriggered:
            switch(Event->Info.Bttn.BttnID)
            {
                case e_NVC_Button_GotoWebPage:
                    UI_GotoWebPage("https://whippyterm.com/Download.php");
                break;
                default:
                case e_NVC_ButtonMAX:
                break;
            }
        break;
        case e_NVCEvent_Timer:
            if(m_CurrentStep<e_VersionCheckStepMAX)
            {
                LockMutex(m_ThreadMutex);
                Busy=m_ThreadCmd.Busy;
                HadError=m_ThreadCmd.HadError;
                UnLockMutex(m_ThreadMutex);
                if(!Busy)
                {
                    if(HadError)
                    {
                        /* The last step had an error */
                        switch(m_CurrentStep)
                        {
                            case e_VersionCheckStep_Connect:
                                Show_Error("Failed to connect to the web site");
                            break;
                            case e_VersionCheckStep_Reading:
                                Show_Error("Failed to read the lastest version from server");
                            break;
                            case e_VersionCheckStep_Closing:
                            case e_VersionCheckStep_Check:
                            case e_VersionCheckStepMAX:
                            default:
                            break;
                        }
                        m_CurrentStep=e_VersionCheckStepMAX;
                    }
                    else
                    {
                        NVC_Move2NextStep();
                    }
                }
            }
        break;
        case e_NVCEventMAX:
        default:
        break;
    }

    return false;
}

/*******************************************************************************
 * NAME:
 *    NVC_ExeCurrentStep
 *
 * SYNOPSIS:
 *    void NVC_ExeCurrentStep(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function does the current step that 'm_CurrentStep' is set to.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    This does not update the GUI
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void NVC_ExeCurrentStep(void)
{
    t_UIButtonCtrl *GotoWebPageBttn;
    t_UILabelCtrl *ResultLabel;
    uint8_t Major;
    uint8_t Minor;
    uint8_t Rev;
    uint8_t Patch;
    string VerStr;
    char buff[100];

    if(m_HadError)
        return;

    GotoWebPageBttn=UINVC_GetButtonHandle(e_NVC_Button_GotoWebPage);
    ResultLabel=UINVC_GetLabelHandle(e_NVC_Label_Result);

    if(m_CurrentStep>e_VersionCheckStep_Connect)
        NVC_SetStepMode(e_VersionCheckStep_Connect,e_VersionCheckMode_Done);
    if(m_CurrentStep>e_VersionCheckStep_Reading)
        NVC_SetStepMode(e_VersionCheckStep_Reading,e_VersionCheckMode_Done);
    if(m_CurrentStep>e_VersionCheckStep_Closing)
        NVC_SetStepMode(e_VersionCheckStep_Closing,e_VersionCheckMode_Done);
    if(m_CurrentStep>e_VersionCheckStep_Check)
        NVC_SetStepMode(e_VersionCheckStep_Check,e_VersionCheckMode_Done);

    switch(m_CurrentStep)
    {
        case e_VersionCheckStep_Connect:
            NVC_SetStepMode(e_VersionCheckStep_Connect,e_VersionCheckMode_Doing);

            LockMutex(m_ThreadMutex);
            m_ThreadCmd.Cmd=e_ThreadCmd_OpenConnection;
            UnLockMutex(m_ThreadMutex);
        break;
        case e_VersionCheckStep_Reading:
            NVC_SetStepMode(e_VersionCheckStep_Reading,e_VersionCheckMode_Doing);

            LockMutex(m_ThreadMutex);
            m_ThreadCmd.Cmd=e_ThreadCmd_ReadVer;
            UnLockMutex(m_ThreadMutex);
        break;
        case e_VersionCheckStep_Closing:
            NVC_SetStepMode(e_VersionCheckStep_Closing,e_VersionCheckMode_Doing);

            LockMutex(m_ThreadMutex);
            m_ThreadCmd.Cmd=e_ThreadCmd_CloseConnection;
            UnLockMutex(m_ThreadMutex);
        break;
        case e_VersionCheckStep_Check:
            NVC_SetStepMode(e_VersionCheckStep_Check,e_VersionCheckMode_Doing);
        break;
        case e_VersionCheckStepMAX:
        default:
            if(CheckLatestVersionFromWebSite(&Major,&Minor,&Rev,&Patch))
            {
                VerStr="Version ";
                sprintf(buff,"%d.%d",Major,Minor);
                VerStr+=buff;
                if(Rev>0 || Patch>0)
                {
                    sprintf(buff,".%d",Rev);
                    VerStr+=buff;
                    if(Patch>0)
                    {
                        sprintf(buff,".%d",Patch);
                        VerStr+=buff;
                    }
                }
                VerStr+=" available";
                UISetLabelText(ResultLabel,VerStr.c_str());
                UIMakeButtonVisible(GotoWebPageBttn,true);
            }
            else
            {
                UISetLabelText(ResultLabel,"Up to date");
                UIMakeButtonVisible(GotoWebPageBttn,false);
            }
            NVC_ShowHideNewVersionFound(true);
            NVC_EnableOkBttn(true);
            UIMakeLabelVisible(ResultLabel,true);
        break;
    }
}

/*******************************************************************************
 * NAME:
 *    NVC_Move2NextStep
 *
 * SYNOPSIS:
 *    void NVC_Move2NextStep(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function moves the state machine to the next step starting it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void NVC_Move2NextStep(void)
{
    int CurrentStep;

    if(m_HadError)
        return;

    if(m_CurrentStep!=e_VersionCheckStepMAX)
    {
        CurrentStep=(int)m_CurrentStep;
        CurrentStep++;
        m_CurrentStep=(e_VersionCheckStepType)CurrentStep;
        NVC_ExeCurrentStep();
    }
}

/*******************************************************************************
 * NAME:
 *    Show_Error
 *
 * SYNOPSIS:
 *    void Show_Error(const char *ErrorMsg);
 *
 * PARAMETERS:
 *    ErrorMsg [I] -- The error message to display
 *
 * FUNCTION:
 *    This function handles when there was an error getting / checking
 *    the version.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Show_Error(const char *ErrorMsg)
{
    t_UILabelCtrl *ResultLabel;
    t_UIButtonCtrl *GotoWebPageBttn;

    m_HadError=true;

    GotoWebPageBttn=UINVC_GetButtonHandle(e_NVC_Button_GotoWebPage);
    ResultLabel=UINVC_GetLabelHandle(e_NVC_Label_Result);

    NVC_SetStepMode(e_VersionCheckStep_Connect,e_VersionCheckMode_Clear);
    NVC_SetStepMode(e_VersionCheckStep_Reading,e_VersionCheckMode_Clear);
    NVC_SetStepMode(e_VersionCheckStep_Closing,e_VersionCheckMode_Clear);
    NVC_SetStepMode(e_VersionCheckStep_Check,e_VersionCheckMode_Clear);

    UISetLabelText(ResultLabel,ErrorMsg);

    NVC_ShowHideNewVersionFound(true);
    NVC_EnableOkBttn(true);
    UIMakeButtonVisible(GotoWebPageBttn,false);

/* DEBUG PAUL: Do ask? */
}

/*******************************************************************************
 * NAME:
 *    WorkerThread
 *
 * SYNOPSIS:
 *    void WorkerThread(void *Data);
 *
 * PARAMETERS:
 *    Data [I] -- Not used
 *
 * FUNCTION:
 *    This function is the worker thread.  It handle talking to the server
 *    because it can block and be very slow.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void WorkerThread(void *Data)
{
    e_ThreadCmdType Cmd;
    bool HadError;
    bool ExitThread;

    ExitThread=false;
    while(!ExitThread)
    {
        /* Check if we have a new command */
        LockMutex(m_ThreadMutex);
        Cmd=m_ThreadCmd.Cmd;
        m_ThreadCmd.Cmd=e_ThreadCmd_Wait;
        if(Cmd!=e_ThreadCmd_Wait)
            m_ThreadCmd.Busy=true;
        UnLockMutex(m_ThreadMutex);

        if(Cmd==e_ThreadCmd_Wait)
        {
            /* We just do a wait and then try again */
            OS_Sleep(1);
            continue;
        }

        HadError=false;
        switch(Cmd)
        {
            case e_ThreadCmd_Wait:
                /* Handled above */
            break;
            case e_ThreadCmd_OpenConnection:
                m_AbortConnect=false;
                if(!OpenConnection2WebSite(CheckForAbort))
                    HadError=true;
            break;
            case e_ThreadCmd_CloseConnection:
                CloseConnection2WebSite();
            break;
            case e_ThreadCmd_ReadVer:
                /* Do the open command */
                if(!ReadLatestVersionFromWebSite())
                    HadError=true;
            break;
            case e_ThreadCmd_Shutdown:
                ExitThread=true;
                CloseConnection2WebSite();
            break;
            case e_ThreadCmdMAX:
            default:
                /* Bad value, exit */
                ExitThread=true;
            break;
        }

        LockMutex(m_ThreadMutex);
        m_ThreadCmd.Busy=false;
        m_ThreadCmd.HadError=HadError;
        UnLockMutex(m_ThreadMutex);
    }
}

/*******************************************************************************
 * NAME:
 *    CheckForAbort
 *
 * SYNOPSIS:
 *    static bool CheckForAbort(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function checks if the user pressed abort while we where trying
 *    to connect to the upgrade server.
 *
 * RETURNS:
 *    true -- We are to abort
 *    false -- Continue
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static bool CheckForAbort(void)
{
    return m_AbortConnect;
}

/*******************************************************************************
 * NAME:
 *    CheckForAbortWithTimeout
 *
 * SYNOPSIS:
 *    static bool CheckForAbortWithTimeout(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function if opening the connection to the version check website
 *    is taking too long.
 *
 * RETURNS:
 *    true -- We are to abort
 *    false -- Continue
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static bool CheckForAbortWithTimeout(void)
{
    if(GetElapsedTime_ms()-m_AutoAbortTimeout>TIMEOUT_QUICK_CHECK_VERSION)
        return true;
    return false;

    return m_AbortConnect;
}

