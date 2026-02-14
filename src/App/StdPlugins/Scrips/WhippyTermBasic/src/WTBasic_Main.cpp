/*******************************************************************************
 * FILENAME: WTBasic_Main.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the WhippyTerm scripting language plugin it in.  This is
 *    based on MY-BASIC and uses the MY-BASIC parser
 *    (https://github.com/paladin-t/my_basic), but has WhippyTerm extentions
 *    so it is called WTBasic.
 *
 * COPYRIGHT:
 *    Copyright 25 Jan 2026 Paul Hutchinson.
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
 *    Paul Hutchinson (25 Jan 2026)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "WTBasic_Main.h"
#include "PluginSDK/Plugin.h"
#include "ThirdParty/my_basic.h"
#include "OS/WTB_OSTime.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      WTBasic // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x02020000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct WTBasicContext
{
    t_ScriptingEngineInstType *Inst;
    struct mb_interpreter_t *bas;
    string LastErrorMsg;
//    bool AbortScript;
    uint32_t WTBasic_Colors[e_SysColMAX*2];
    int ActiveFGColor;
    int ActiveBGColor;
};

/*** FUNCTION PROTOTYPES      ***/
PG_BOOL WTBasic_Init(void);
const char *WTBasic_GetLastError(t_ScriptingEngineContextType *Context);
t_ScriptingEngineContextType *WTBasic_AllocateContext(t_ScriptingEngineInstType *Inst);
void WTBasic_FreeContext(t_ScriptingEngineContextType *Context);
PG_BOOL WTBasic_LoadScriptFromString(t_ScriptingEngineContextType *Context,const char *Str);
PG_BOOL WTBasic_RunLoadedScript(t_ScriptingEngineContextType *Context);
void WTBasic_AbortScript(t_ScriptingEngineContextType *Context);
static void ConvertKey2String(struct PluginKeyPress *key,char *RetStr,int Size);

int WriteStdOut(struct mb_interpreter_t *bas,const char *fmt,...);
int ReadLineStdIn(struct mb_interpreter_t *bas,const char *pmt,char *buf,int s);

int SleepFn(struct mb_interpreter_t *bas, void **arg);
int mSleepFn(struct mb_interpreter_t *bas, void **arg);
int LPrintFn(struct mb_interpreter_t *bas, void **arg);
int LocateFn(struct mb_interpreter_t *bas, void **arg);
int ClsFn(struct mb_interpreter_t *bas, void **arg);
int PosFn(struct mb_interpreter_t *bas, void **arg);
int CsrLinFn(struct mb_interpreter_t *bas, void **arg);
int ScreenSizeFn(struct mb_interpreter_t *bas, void **arg);
int ColorFn(struct mb_interpreter_t *bas, void **arg);
int PaletteFn(struct mb_interpreter_t *bas, void **arg);
int SetTitleFn(struct mb_interpreter_t *bas, void **arg);
int GetTitleFn(struct mb_interpreter_t *bas, void **arg);
int ClearAreaFn(struct mb_interpreter_t *bas, void **arg);
int ScrollAreaFn(struct mb_interpreter_t *bas, void **arg);
int InKeyFn(struct mb_interpreter_t *bas, void **arg);
int SendFn(struct mb_interpreter_t *bas, void **arg);
int RPrintFn(struct mb_interpreter_t *bas, void **arg);
int RecvFn(struct mb_interpreter_t *bas, void **arg);
int KeyboardFn(struct mb_interpreter_t *bas, void **arg);
int ScreenFn(struct mb_interpreter_t *bas, void **arg);
int StdioFn(struct mb_interpreter_t *bas, void **arg);

//static int WTBasic_PreStepCallback(struct mb_interpreter_t *bas, void **l, const char *Filename, int SourcePos, unsigned short SourceRow, unsigned short SourceCol);

/*** VARIABLE DEFINITIONS     ***/
const struct ScriptingEngineAPI g_WTBasicPluginAPI=
{
    WTBasic_Init,
    WTBasic_AllocateContext,
    WTBasic_FreeContext,
    WTBasic_GetLastError,
    WTBasic_LoadScriptFromString,
    WTBasic_RunLoadedScript,
    WTBasic_AbortScript,
    NULL, // NewKeyPressDetected
};

struct ScriptingEngineInfo m_WTBasicInfo=
{
    "WTBasic",
    "WhippyTerm Basic",
    "Basic",
    "MY-BASIC",
    "bas",
    sizeof(struct ScriptingEngineAPI),
    &g_WTBasicPluginAPI
};

//const struct PI_UIAPI *g_WTB_UI;
const struct PI_SystemAPI *g_WTB_System;
const struct ScriptingSystem_API *g_WTB_Scripting;
bool m_StdioGoes2Com=true;
char *g_PrintBuff;
unsigned int g_PrintBuffSize;

/*******************************************************************************
 * NAME:
 *    WTBasic_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int WTBasic_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
 *          unsigned int Version);
 *
 * PARAMETERS:
 *    SysAPI [I] -- The main API to WhippyTerm
 *    Version [I] -- What version of WhippyTerm is running.  This is used
 *                   to make sure we are compatible.  This is in the
 *                   Major<<24 | Minor<<16 | Rev<<8 | Patch format
 *
 * FUNCTION:
 *    This function registers this plugin with the system.
 *
 * RETURNS:
 *    0 if we support this version of WhippyTerm, and the minimum version
 *    we need if we are not.
 *
 * NOTES:
 *    This function is normally is called from the RegisterPlugin() when
 *    it is being used as a normal plugin.  As a std plugin it is called
 *    from RegisterStdPlugins() instead.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
/* This needs to be extern "C" because it is the main entry point for the
   plugin system */
extern "C"
{
    unsigned int REGISTER_PLUGIN_FUNCTION(const struct PI_SystemAPI *SysAPI,
            unsigned int Version)
    {
        if(Version<NEEDED_MIN_API_VERSION)
            return NEEDED_MIN_API_VERSION;

        g_WTB_System=SysAPI;
        g_WTB_Scripting=g_WTB_System->GetAPI_Scripting();
//        g_WTB_UI=g_WTB_Scripting->GetAPI_UI();

        /* If we are have the correct experimental API */
        if(g_WTB_System->GetExperimentalID()>0 &&
                g_WTB_System->GetExperimentalID()<1)
        {
            return 0xFFFFFFFF;
        }

        g_WTB_Scripting->RegisterScriptingLanguage(&m_WTBasicInfo,
                sizeof(m_WTBasicInfo));

        return 0;
    }
}

PG_BOOL WTBasic_Init(void)
{
    mb_init();

    return true;
}

t_ScriptingEngineContextType *WTBasic_AllocateContext(t_ScriptingEngineInstType *Inst)
{
    struct WTBasicContext *NewContext;
    bool InitCalled;
    unsigned int c;

    NewContext=NULL;
    InitCalled=false;
    try
    {
        NewContext=new struct WTBasicContext;
        NewContext->Inst=Inst;
        NewContext->bas=NULL;
//        NewContext->AbortScript=false;

        if(mb_open(&NewContext->bas)!=MB_FUNC_OK)
            throw(0);
        InitCalled=true;

        mb_set_userdata(NewContext->bas,(void *)NewContext);

        /* Connect in the debugging interface */
//        mb_debug_set_stepped_handler(NewContext->bas,WTBasic_PreStepCallback,
//                NULL);

        /* Extend this basic instance */
        mb_register_func(NewContext->bas,"SLEEP",SleepFn);
        mb_register_func(NewContext->bas,"MSLEEP",mSleepFn);
        mb_register_func(NewContext->bas,"LOCATE",LocateFn);
        mb_register_func(NewContext->bas,"CLS",ClsFn);
        mb_register_func(NewContext->bas,"POS",PosFn);
        mb_register_func(NewContext->bas,"CSRLIN",CsrLinFn);
        mb_register_func(NewContext->bas,"SCREENSIZE",ScreenSizeFn);
        mb_register_func(NewContext->bas,"COLOR",ColorFn);
        mb_register_func(NewContext->bas,"PALETTE",PaletteFn);
        mb_register_func(NewContext->bas,"SETTITLE",SetTitleFn);
        mb_register_func(NewContext->bas,"GETTITLE",GetTitleFn);
        mb_register_func(NewContext->bas,"CLEARAREA",ClearAreaFn);
        mb_register_func(NewContext->bas,"SCROLLAREA",ScrollAreaFn);

        /* IO */
        mb_register_func(NewContext->bas,"INKEY$",InKeyFn);
        mb_register_func(NewContext->bas,"LPRINT",LPrintFn);
        mb_register_func(NewContext->bas,"RPRINT",RPrintFn);
        mb_register_func(NewContext->bas,"SEND",SendFn);
        mb_register_func(NewContext->bas,"RECV",RecvFn);
        mb_register_func(NewContext->bas,"KEYBOARD",KeyboardFn);
        mb_register_func(NewContext->bas,"SCREEN",ScreenFn);
        mb_register_func(NewContext->bas,"STDIO",StdioFn);
//        mb_register_func(NewContext->bas,"RECVLN",InKeyFn);

        /* Override stdio */
        mb_set_printer(NewContext->bas,WriteStdOut);
        mb_set_inputer(NewContext->bas,ReadLineStdIn);

        NewContext->ActiveFGColor=1;
        NewContext->ActiveBGColor=0;
        for(c=0;c<(int)e_SysColMAX;c++)
        {
            NewContext->WTBasic_Colors[c]=g_WTB_Scripting->GetSysColor(Inst,
                    e_SysColShade_Normal,c);
            NewContext->WTBasic_Colors[c+e_SysColMAX]=g_WTB_Scripting->GetSysColor(Inst,
                    e_SysColShade_Bright,c);
        }
    }
    catch(...)
    {
        if(NewContext!=NULL)
        {
            if(InitCalled)
                mb_close(&NewContext->bas);
            delete NewContext;
        }
        return NULL;
    }

    return (t_ScriptingEngineContextType *)NewContext;
}

//int WTBasic_PreStepCallback(struct mb_interpreter_t *bas, void **l, const char *Filename, int SourcePos, unsigned short SourceRow, unsigned short SourceCol)
//{
//    struct WTBasicContext *Data;
//
//    mb_get_userdata(bas,(void **)&Data);
//
//    if(Data->AbortScript)
//    {
//        /* Abort the script */
//        mb_raise_error(bas,l,(mb_error_e)(SE_EA_EXTENDED_ABORT+1),0);
//    }
//    return MB_FUNC_OK;
//}

void WTBasic_FreeContext(t_ScriptingEngineContextType *Context)
{
    struct WTBasicContext *Data=(struct WTBasicContext *)Context;

    mb_close(&Data->bas);
    delete Data;
}

PG_BOOL WTBasic_LoadScriptFromString(t_ScriptingEngineContextType *Context,const char *Str)
{
    struct WTBasicContext *Data=(struct WTBasicContext *)Context;
    const char *FileName;
    int Pos;
    unsigned short Row;
    unsigned short Col;
    const char *ErrorMsg;
    mb_error_e ErrorCode;
    char buff[100];

    Data->LastErrorMsg="";

    if(mb_load_string(Data->bas,Str,true)!=MB_FUNC_OK)
    {
        ErrorCode=mb_get_last_error(Data->bas,&FileName,&Pos,&Row,&Col);
        ErrorMsg=mb_get_error_desc(ErrorCode);

        Data->LastErrorMsg="Error on line ";
        sprintf(buff,"%d",Row);
        Data->LastErrorMsg+=buff;
        Data->LastErrorMsg+=" of file \"";
        Data->LastErrorMsg+=FileName;
        Data->LastErrorMsg+="\":\n";

        Data->LastErrorMsg+=ErrorMsg;
        Data->LastErrorMsg+="\n";

        return false;
    }
    return true;
}

PG_BOOL WTBasic_RunLoadedScript(t_ScriptingEngineContextType *Context)
{
    struct WTBasicContext *Data=(struct WTBasicContext *)Context;
    const char *FileName;
    int Pos;
    unsigned short Row;
    unsigned short Col;
    const char *ErrorMsg;
    mb_error_e ErrorCode;
    char buff[100];

    Data->LastErrorMsg="";
    if(mb_run(Data->bas,true)>0)
    {
        ErrorCode=mb_get_last_error(Data->bas,&FileName,&Pos,&Row,&Col);
        if(ErrorCode==SE_NO_ERR)
        {
            Data->LastErrorMsg+="Unexpected error";
            return true;
        }
        if(ErrorCode==SE_EA_EXTENDED_ABORT)
        {
            /* User abort */
            ErrorMsg="User abort";
            Data->LastErrorMsg="";
        }
        else
        {
            ErrorMsg=mb_get_error_desc(ErrorCode);

            Data->LastErrorMsg="Error on line ";
            sprintf(buff,"%d",Row);
            Data->LastErrorMsg+=buff;
            Data->LastErrorMsg+=" of file \"";
            Data->LastErrorMsg+=FileName;
            Data->LastErrorMsg+="\":\n";
        }

        Data->LastErrorMsg+=ErrorMsg;
        Data->LastErrorMsg+="\n";

        return false;
    }

    return true;
}

const char *WTBasic_GetLastError(t_ScriptingEngineContextType *Context)
{
    struct WTBasicContext *Data=(struct WTBasicContext *)Context;

    return Data->LastErrorMsg.c_str();
}

void WTBasic_AbortScript(t_ScriptingEngineContextType *Context)
{
    struct WTBasicContext *Data=(struct WTBasicContext *)Context;

//    /* Not sure how to abort a running script, so we connect into the debug
//       interface and use it to check for abort every statement. */
    mb_schedule_suspend(Data->bas,MB_EXTENDED_ABORT);
}

void ConvertKey2String(struct PluginKeyPress *key,char *RetStr,int Size)
{
    memset(RetStr,0x00,Size);
    switch(key->ExtendedKey)
    {
        case e_UIKeys_Escape:
            RetStr[0]=27;
        break;
        case e_UIKeys_Tab:
            RetStr[0]=9;
        break;
        case e_UIKeys_Backtab:
        break;
        case e_UIKeys_Backspace:
            RetStr[0]=8;
        break;
        case e_UIKeys_Return:
            RetStr[0]=13;
        break;
        case e_UIKeys_Enter:
            RetStr[0]=10;
        break;
        case e_UIKeys_Insert:
            RetStr[0]=255;
            RetStr[1]=82;
        break;
        break;
        case e_UIKeys_Delete:
            RetStr[0]=127;
        break;
        case e_UIKeys_Pause:
        case e_UIKeys_Print:
        case e_UIKeys_SysReq:
        case e_UIKeys_Clear:
        break;
        case e_UIKeys_Home:
            RetStr[0]=255;
            RetStr[1]=71;
        break;
        case e_UIKeys_End:
            RetStr[0]=255;
            RetStr[1]=79;
        break;
        case e_UIKeys_Left:
            RetStr[0]=255;
            RetStr[1]=75;
        break;
        case e_UIKeys_Up:
            RetStr[0]=255;
            RetStr[1]=72;
        break;
        case e_UIKeys_Right:
            RetStr[0]=255;
            RetStr[1]=77;
        break;
        case e_UIKeys_Down:
            RetStr[0]=255;
            RetStr[1]=80;
        break;
        case e_UIKeys_PageUp:
            RetStr[0]=255;
            RetStr[1]=73;
        break;
        case e_UIKeys_PageDown:
            RetStr[0]=255;
            RetStr[1]=81;
        break;
        case e_UIKeys_Shift:
        case e_UIKeys_Control:
        case e_UIKeys_Meta:
        case e_UIKeys_Alt:
        case e_UIKeys_AltGr:
        case e_UIKeys_CapsLock:
        case e_UIKeys_NumLock:
        case e_UIKeys_ScrollLock:
        break;
        case e_UIKeys_F1:
            RetStr[0]=255;
            RetStr[1]=59;
        break;
        case e_UIKeys_F2:
            RetStr[0]=255;
            RetStr[1]=60;
        break;
        case e_UIKeys_F3:
            RetStr[0]=255;
            RetStr[1]=61;
        break;
        case e_UIKeys_F4:
            RetStr[0]=255;
            RetStr[1]=62;
        break;
        case e_UIKeys_F5:
            RetStr[0]=255;
            RetStr[1]=63;
        break;
        case e_UIKeys_F6:
            RetStr[0]=255;
            RetStr[1]=64;
        break;
        case e_UIKeys_F7:
            RetStr[0]=255;
            RetStr[1]=65;
        break;
        case e_UIKeys_F8:
            RetStr[0]=255;
            RetStr[1]=66;
        break;
        case e_UIKeys_F9:
            RetStr[0]=255;
            RetStr[1]=67;
        break;
        case e_UIKeys_F10:
            RetStr[0]=255;
            RetStr[1]=68;
        break;
        case e_UIKeys_F11:
            RetStr[0]=255;
            RetStr[1]=133;
        break;
        case e_UIKeys_F12:
            RetStr[0]=255;
            RetStr[1]=134;
        break;
        case e_UIKeys_F13:
        case e_UIKeys_F14:
        case e_UIKeys_F15:
        case e_UIKeys_F16:
        case e_UIKeys_F17:
        case e_UIKeys_F18:
        case e_UIKeys_F19:
        case e_UIKeys_F20:
        case e_UIKeys_F21:
        case e_UIKeys_F22:
        case e_UIKeys_F23:
        case e_UIKeys_F24:
        case e_UIKeys_F25:
        case e_UIKeys_F26:
        case e_UIKeys_F27:
        case e_UIKeys_F28:
        case e_UIKeys_F29:
        case e_UIKeys_F30:
        case e_UIKeys_F31:
        case e_UIKeys_F32:
        case e_UIKeys_F33:
        case e_UIKeys_F34:
        case e_UIKeys_F35:
        case e_UIKeys_Super_L:
        case e_UIKeys_Super_R:
        case e_UIKeys_Menu:
        case e_UIKeys_Hyper_L:
        case e_UIKeys_Hyper_R:
        case e_UIKeys_Help:
        case e_UIKeys_Direction_L:
        case e_UIKeys_Direction_R:
        case e_UIKeys_Back:
        case e_UIKeys_Forward:
        case e_UIKeys_Stop:
        case e_UIKeys_Refresh:
        case e_UIKeys_VolumeDown:
        case e_UIKeys_VolumeMute:
        case e_UIKeys_VolumeUp:
        case e_UIKeys_BassBoost:
        case e_UIKeys_BassUp:
        case e_UIKeys_BassDown:
        case e_UIKeys_TrebleUp:
        case e_UIKeys_TrebleDown:
        case e_UIKeys_MediaPlay:
        case e_UIKeys_MediaStop:
        case e_UIKeys_MediaPrevious:
        case e_UIKeys_MediaNext:
        case e_UIKeys_MediaRecord:
        case e_UIKeys_MediaPause:
        case e_UIKeys_MediaTogglePlayPause:
        case e_UIKeys_HomePage:
        case e_UIKeys_Favorites:
        case e_UIKeys_Search:
        case e_UIKeys_Standby:
        case e_UIKeys_OpenUrl:
        case e_UIKeys_LaunchMail:
        case e_UIKeys_LaunchMedia:
        case e_UIKeys_Launch0:
        case e_UIKeys_Launch1:
        case e_UIKeys_Launch2:
        case e_UIKeys_Launch3:
        case e_UIKeys_Launch4:
        case e_UIKeys_Launch5:
        case e_UIKeys_Launch6:
        case e_UIKeys_Launch7:
        case e_UIKeys_Launch8:
        case e_UIKeys_Launch9:
        case e_UIKeys_LaunchA:
        case e_UIKeys_LaunchB:
        case e_UIKeys_LaunchC:
        case e_UIKeys_LaunchD:
        case e_UIKeys_LaunchE:
        case e_UIKeys_LaunchF:
        case e_UIKeys_LaunchG:
        case e_UIKeys_LaunchH:
        case e_UIKeys_MonBrightnessUp:
        case e_UIKeys_MonBrightnessDown:
        case e_UIKeys_KeyboardLightOnOff:
        case e_UIKeys_KeyboardBrightnessUp:
        case e_UIKeys_KeyboardBrightnessDown:
        case e_UIKeys_PowerOff:
        case e_UIKeys_WakeUp:
        case e_UIKeys_Eject:
        case e_UIKeys_ScreenSaver:
        case e_UIKeys_WWW:
        case e_UIKeys_Memo:
        case e_UIKeys_LightBulb:
        case e_UIKeys_Shop:
        case e_UIKeys_History:
        case e_UIKeys_AddFavorite:
        case e_UIKeys_HotLinks:
        case e_UIKeys_BrightnessAdjust:
        case e_UIKeys_Finance:
        case e_UIKeys_Community:
        case e_UIKeys_AudioRewind:
        case e_UIKeys_BackForward:
        case e_UIKeys_ApplicationLeft:
        case e_UIKeys_ApplicationRight:
        case e_UIKeys_Book:
        case e_UIKeys_CD:
        case e_UIKeys_Calculator:
        case e_UIKeys_ToDoList:
        case e_UIKeys_ClearGrab:
        case e_UIKeys_Close:
        case e_UIKeys_Copy:
        case e_UIKeys_Cut:
        case e_UIKeys_Display:
        case e_UIKeys_DOS:
        case e_UIKeys_Documents:
        case e_UIKeys_Excel:
        case e_UIKeys_Explorer:
        case e_UIKeys_Game:
        case e_UIKeys_Go:
        case e_UIKeys_iTouch:
        case e_UIKeys_LogOff:
        case e_UIKeys_Market:
        case e_UIKeys_Meeting:
        case e_UIKeys_MenuKB:
        case e_UIKeys_MenuPB:
        case e_UIKeys_MySites:
        case e_UIKeys_News:
        case e_UIKeys_OfficeHome:
        case e_UIKeys_Option:
        case e_UIKeys_Paste:
        case e_UIKeys_Phone:
        case e_UIKeys_Calendar:
        case e_UIKeys_Reply:
        case e_UIKeys_Reload:
        case e_UIKeys_RotateWindows:
        case e_UIKeys_RotationPB:
        case e_UIKeys_RotationKB:
        case e_UIKeys_Save:
        case e_UIKeys_Send:
        case e_UIKeys_Spell:
        case e_UIKeys_SplitScreen:
        case e_UIKeys_Support:
        case e_UIKeys_TaskPane:
        case e_UIKeys_Terminal:
        case e_UIKeys_Tools:
        case e_UIKeys_Travel:
        case e_UIKeys_Video:
        case e_UIKeys_Word:
        case e_UIKeys_Xfer:
        case e_UIKeys_ZoomIn:
        case e_UIKeys_ZoomOut:
        case e_UIKeys_Away:
        case e_UIKeys_Messenger:
        case e_UIKeys_WebCam:
        case e_UIKeys_MailForward:
        case e_UIKeys_Pictures:
        case e_UIKeys_Music:
        case e_UIKeys_Battery:
        case e_UIKeys_Bluetooth:
        case e_UIKeys_WLAN:
        case e_UIKeys_UWB:
        case e_UIKeys_AudioForward:
        case e_UIKeys_AudioRepeat:
        case e_UIKeys_AudioRandomPlay:
        case e_UIKeys_Subtitle:
        case e_UIKeys_AudioCycleTrack:
        case e_UIKeys_Time:
        case e_UIKeys_Hibernate:
        case e_UIKeys_View:
        case e_UIKeys_TopMenu:
        case e_UIKeys_PowerDown:
        case e_UIKeys_Suspend:
        case e_UIKeys_ContrastAdjust:
        case e_UIKeys_MediaLast:
        case e_UIKeys_unknown:
        case e_UIKeys_Call:
        case e_UIKeys_Camera:
        case e_UIKeys_CameraFocus:
        case e_UIKeys_Context1:
        case e_UIKeys_Context2:
        case e_UIKeys_Context3:
        case e_UIKeys_Context4:
        case e_UIKeys_Flip:
        case e_UIKeys_Hangup:
        case e_UIKeys_No:
        case e_UIKeys_Select:
        case e_UIKeys_Yes:
        case e_UIKeys_ToggleCallHangup:
        case e_UIKeys_VoiceDial:
        case e_UIKeys_LastNumberRedial:
        case e_UIKeys_Execute:
        case e_UIKeys_Printer:
        case e_UIKeys_Play:
        case e_UIKeys_Sleep:
        case e_UIKeys_Zoom:
        case e_UIKeys_Cancel:
        break;
        case e_UIKeysMAX:
            strncpy(RetStr,(const char *)key->KeyChar,Size-1);
            RetStr[Size-1]=0;
        break;
    }
}

/* Move to a new file... */
int SleepFn(struct mb_interpreter_t *bas, void **arg)
{
    int s;

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_pop_int(bas,arg,&s));
    mb_check(mb_attempt_func_end(bas,arg));

    WTB_Sleep(s*1000);

    return MB_FUNC_OK;
}

int mSleepFn(struct mb_interpreter_t *bas, void **arg)
{
    int s;

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_pop_int(bas,arg,&s));
    mb_check(mb_attempt_func_end(bas,arg));

    WTB_Sleep(s);

    return MB_FUNC_OK;
}

int WriteStdOut(struct mb_interpreter_t *bas,const char *fmt,...)
{
    va_list arg;
    int ret;
    struct WTBasicContext *Data;
    char *p;
    char *Start;

    mb_get_userdata(bas,(void **)&Data);

    if(g_PrintBuff==NULL)
    {
        g_PrintBuff=(char *)malloc(100);
        if(g_PrintBuff==NULL)
            return 0;
        g_PrintBuffSize=100;
    }

    va_start(arg,fmt);
    ret=vsnprintf(NULL,0,fmt,arg);
    va_end(arg);
    if(ret>(int)g_PrintBuffSize)
    {
        /* Resize */
        g_PrintBuff=(char *)realloc(g_PrintBuff,ret+1);
        if(g_PrintBuff==NULL)
            return 0;
    }
    va_start(arg,fmt);
    ret=vsnprintf(g_PrintBuff,g_PrintBuffSize,fmt,arg);
    va_end(arg);

    Start=g_PrintBuff;
    p=g_PrintBuff;
    while(*p!=0)
    {
        if(*p=='\n')
        {
            /* Write what we have already */
            if(m_StdioGoes2Com)
            {
                g_WTB_Scripting->WriteCom(Data->Inst,(const uint8_t *)Start,
                        p-Start);
                g_WTB_Scripting->SendEnter(Data->Inst);
            }
            else
            {
                g_WTB_Scripting->WriteScreen(Data->Inst,(const uint8_t *)Start,
                        p-Start);
                g_WTB_Scripting->DoReturn(Data->Inst);
                g_WTB_Scripting->DoNewLine(Data->Inst);
            }
            Start=p+1;
        }
        else if(*p=='\b')
        {
            /* Write what we have already */
            if(m_StdioGoes2Com)
            {
                g_WTB_Scripting->WriteCom(Data->Inst,(const uint8_t *)Start,
                        p-Start);
                g_WTB_Scripting->SendBackspace(Data->Inst);
            }
            else
            {
                g_WTB_Scripting->WriteScreen(Data->Inst,(const uint8_t *)Start,
                        p-Start);
                g_WTB_Scripting->DoBackspace(Data->Inst);
            }
            Start=p+1;
        }
        p++;
    }
    if(m_StdioGoes2Com)
    {
        g_WTB_Scripting->WriteCom(Data->Inst,(const uint8_t *)Start,
                p-Start);
    }
    else
    {
        g_WTB_Scripting->WriteScreen(Data->Inst,(const uint8_t *)Start,
                p-Start);
    }
    return ret;
}

int ReadLineStdIn(struct mb_interpreter_t *bas,const char *pmt,char *buf,int s)
{
    struct WTBasicContext *Data;
    struct PluginKeyPress key;
    unsigned int l;
    char keystr[10];
    unsigned int InsertPos;
    char c;

    mb_get_userdata(bas,(void **)&Data);

    if(s==0)
        return 0;

    if(m_StdioGoes2Com)
    {
        InsertPos=0;
        buf[0]=0;
        keystr[1]=0;
        for(;;)
        {
            l=g_WTB_Scripting->ReadCom(Data->Inst,(uint8_t *)keystr,1);
            if(l>0)
            {
                c=keystr[0];
                if(c==10)
                {
                    g_WTB_Scripting->WriteCom(Data->Inst,
                            (const uint8_t *)keystr,1);
                    break;
                }
                if(c==8)
                {
                    /* We have to handle backspace our self */
                    if(InsertPos>0)
                    {
                        InsertPos--;
                        buf[InsertPos]=0;
                        g_WTB_Scripting->WriteCom(Data->Inst,
                                (const uint8_t *)keystr,1);
                    }
                    continue;
                }

                if(c<32)
                {
                    /* Ignore control codes */
                    continue;
                }
                if(InsertPos+1>=(unsigned int)s)
                    break;
                strcat(&buf[InsertPos],keystr);

                InsertPos++;

                g_WTB_Scripting->WriteCom(Data->Inst,(const uint8_t *)keystr,1);
            }
            WTB_Sleep(1);
        }
    }
    else
    {
        InsertPos=0;
        buf[0]=0;
        for(;;)
        {
            if(g_WTB_Scripting->ReadKeyboard(Data->Inst,&key,1)>0)
            {
                if(key.ExtendedKey==e_UIKeys_Enter ||
                        key.ExtendedKey==e_UIKeys_Return)
                {
                    g_WTB_Scripting->DoReturn(Data->Inst);
                    g_WTB_Scripting->DoNewLine(Data->Inst);
                    break;
                }
                if(key.ExtendedKey==e_UIKeys_Backspace)
                {
                    /* We have to handle backspace our self */
                    if(InsertPos>0)
                    {
                        InsertPos--;
                        buf[InsertPos]=0;
                        g_WTB_Scripting->DoBackspace(Data->Inst);
                    }
                    continue;
                }

                ConvertKey2String(&key,keystr,sizeof(keystr)-1);
                if(keystr[0]<32)
                {
                    /* Ignore control codes */
                    continue;
                }
                l=strlen(keystr);
                if(InsertPos+l>=(unsigned int)s)
                    break;
                strcat(&buf[InsertPos],keystr);

                InsertPos+=l;

                g_WTB_Scripting->WriteScreen(Data->Inst,(const uint8_t *)keystr,l);
            }
            WTB_Sleep(1);
        }
    }

    return InsertPos;
}


int LPrintFn(struct mb_interpreter_t *bas, void **arg)
{
    mb_value_t val;
    string BuildStr;
    int RetValue;
    char buff[100];
    struct WTBasicContext *Data;
    const char *p;

    mb_get_userdata(bas,(void **)&Data);

    RetValue=MB_FUNC_OK;
    BuildStr="";
    mb_check(mb_attempt_func_begin(bas,arg));
    while(mb_has_arg(bas,arg))
    {
        mb_check(mb_pop_value(bas,arg,&val));
        switch(val.type)
        {
            case MB_DT_INT:
                sprintf(buff,"%d",val.value.integer);
                BuildStr+=buff;
            break;
            case MB_DT_REAL:
                sprintf(buff,"%g",val.value.float_point);
                BuildStr+=buff;
            break;
            case MB_DT_STRING:
                p=val.value.string;
                while(*p!=0)
                {
                    if(*p=='\n')
                    {
                        /* Write what we have already */
                        g_WTB_Scripting->WriteScreen(Data->Inst,
                                (const uint8_t *)BuildStr.c_str(),
                                BuildStr.length());
                        BuildStr="";
                        g_WTB_Scripting->DoNewLine(Data->Inst);
                    }
                    else if(*p=='\r')
                    {
                        /* Write what we have already */
                        g_WTB_Scripting->WriteScreen(Data->Inst,
                                (const uint8_t *)BuildStr.c_str(),
                                BuildStr.length());
                        BuildStr="";
                        g_WTB_Scripting->DoReturn(Data->Inst);
                    }
                    else if(*p=='\b')
                    {
                        /* Write what we have already */
                        g_WTB_Scripting->WriteScreen(Data->Inst,
                                (const uint8_t *)BuildStr.c_str(),
                                BuildStr.length());
                        BuildStr="";
                        g_WTB_Scripting->DoBackspace(Data->Inst);
                    }
                    else if(*p=='\t')
                    {
                        /* Write what we have already */
                        g_WTB_Scripting->WriteScreen(Data->Inst,
                                (const uint8_t *)BuildStr.c_str(),
                                BuildStr.length());
                        BuildStr="";
                        g_WTB_Scripting->DoTab(Data->Inst);
                    }
                    else if(*p=='\f')
                    {
                        /* Write what we have already */
                        g_WTB_Scripting->WriteScreen(Data->Inst,
                                (const uint8_t *)BuildStr.c_str(),
                                BuildStr.length());
                        BuildStr="";
                        g_WTB_Scripting->DoClearScreen(Data->Inst);
                    }
                    else if(*p=='\a')
                    {
                        /* Write what we have already */
                        g_WTB_Scripting->WriteScreen(Data->Inst,
                                (const uint8_t *)BuildStr.c_str(),
                                BuildStr.length());
                        BuildStr="";
                        g_WTB_Scripting->DoSystemBell(Data->Inst,false);
                    }
                    else
                    {
                        BuildStr+=*p;
                    }
                    p++;
                }
            break;
            case MB_DT_NIL:
            case MB_DT_UNKNOWN:
            case MB_DT_NUM:
            case MB_DT_TYPE:
            case MB_DT_USERTYPE:
            case MB_DT_USERTYPE_REF:
            case MB_DT_ARRAY:
            case MB_DT_LIST:
            case MB_DT_LIST_IT:
            case MB_DT_DICT:
            case MB_DT_DICT_IT:
            case MB_DT_COLLECTION:
            case MB_DT_ITERATOR:
            case MB_DT_CLASS:
            case MB_DT_ROUTINE:
            default:
                RetValue=MB_FUNC_ERR;
            break;
        }
    }
    mb_check(mb_attempt_func_end(bas,arg));

    /* Write the last of the string */
    g_WTB_Scripting->WriteScreen(Data->Inst,
            (const uint8_t *)BuildStr.c_str(),BuildStr.length());

    return RetValue;
}

int ClsFn(struct mb_interpreter_t *bas, void **arg)
{
    struct WTBasicContext *Data;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_attempt_func_end(bas,arg));

    /* Write the last of the string */
    g_WTB_Scripting->DoClearScreen(Data->Inst);

    return MB_FUNC_OK;
}

int LocateFn(struct mb_interpreter_t *bas, void **arg)
{
    int x,y;
    struct WTBasicContext *Data;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_pop_int(bas,arg,&x));
    mb_check(mb_pop_int(bas,arg,&y));
    mb_check(mb_attempt_func_end(bas,arg));

    /* Write the last of the string */
    g_WTB_Scripting->SetCursorXY(Data->Inst,x,y);

    return MB_FUNC_OK;
}

int PosFn(struct mb_interpreter_t *bas, void **arg)
{
    struct WTBasicContext *Data;
    int xory;
    int32_t X;
    int32_t Y;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_open_bracket(bas,arg));
    mb_check(mb_pop_int(bas,arg,&xory));
    mb_check(mb_attempt_close_bracket(bas,arg));

    g_WTB_Scripting->GetCursorXY(Data->Inst,&X,&Y);

    if(xory==0)
        mb_check(mb_push_int(bas,arg,X));
    else
        mb_check(mb_push_int(bas,arg,Y));

    return MB_FUNC_OK;
}

int CsrLinFn(struct mb_interpreter_t *bas, void **arg)
{
    struct WTBasicContext *Data;
    int32_t X;
    int32_t Y;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_attempt_func_end(bas,arg));

    g_WTB_Scripting->GetCursorXY(Data->Inst,&X,&Y);

    mb_check(mb_push_int(bas,arg,Y));

    return MB_FUNC_OK;
}

int ScreenSizeFn(struct mb_interpreter_t *bas, void **arg)
{
    struct WTBasicContext *Data;
    int xory;
    int32_t Rows;
    int32_t Cols;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_open_bracket(bas,arg));
    mb_check(mb_pop_int(bas,arg,&xory));
    mb_check(mb_attempt_close_bracket(bas,arg));

    g_WTB_Scripting->GetScreenSize(Data->Inst,&Rows,&Cols);

    if(xory==0)
        mb_check(mb_push_int(bas,arg,Cols));
    else
        mb_check(mb_push_int(bas,arg,Rows));

    return MB_FUNC_OK;
}

int ColorFn(struct mb_interpreter_t *bas, void **arg)
{
    int fg,bg;
    struct WTBasicContext *Data;
    struct StyleData Style;

    mb_get_userdata(bas,(void **)&Data);

    bg=Data->ActiveBGColor;

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_pop_int(bas,arg,&fg));
    if(mb_has_arg(bas,arg))
        mb_check(mb_pop_int(bas,arg,&bg));
    mb_check(mb_attempt_func_end(bas,arg));

    if(fg<0 || fg>=(int)(sizeof(Data->WTBasic_Colors)/sizeof(uint32_t)))
    {
        mb_raise_error(bas,arg,SE_RN_INDEX_OUT_OF_BOUND,MB_FUNC_ERR);
        return MB_FUNC_ERR;
    }
    if(bg<0 || bg>=(int)(sizeof(Data->WTBasic_Colors)/sizeof(uint32_t)))
    {
        mb_raise_error(bas,arg,SE_RN_INDEX_OUT_OF_BOUND,MB_FUNC_ERR);
        return MB_FUNC_ERR;
    }

    g_WTB_Scripting->GetStyle(Data->Inst,&Style);
    Style.FGColor=Data->WTBasic_Colors[fg];
    Style.BGColor=Data->WTBasic_Colors[bg];
    g_WTB_Scripting->SetStyle(Data->Inst,&Style);

    Data->ActiveFGColor=fg;
    Data->ActiveBGColor=bg;

    return MB_FUNC_OK;
}

int PaletteFn(struct mb_interpreter_t *bas, void **arg)
{
    int index,color;
    struct WTBasicContext *Data;
    struct StyleData Style;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_pop_int(bas,arg,&index));
    mb_check(mb_pop_int(bas,arg,&color));
    mb_check(mb_attempt_func_end(bas,arg));

    if(index<0 || index>=(int)(sizeof(Data->WTBasic_Colors)/sizeof(uint32_t)))
    {
        mb_raise_error(bas,arg,SE_RN_INDEX_OUT_OF_BOUND,MB_FUNC_ERR);
        return MB_FUNC_ERR;
    }

    Data->WTBasic_Colors[index]=color;

    /* Now if this is the active color set the style as well */
    if(index==Data->ActiveFGColor || index==Data->ActiveBGColor)
    {
        g_WTB_Scripting->GetStyle(Data->Inst,&Style);
        Style.FGColor=Data->WTBasic_Colors[Data->ActiveFGColor];
        Style.BGColor=Data->WTBasic_Colors[Data->ActiveBGColor];
        g_WTB_Scripting->SetStyle(Data->Inst,&Style);
    }

    return MB_FUNC_OK;
}

int SetTitleFn(struct mb_interpreter_t *bas, void **arg)
{
    char *Title;
    struct WTBasicContext *Data;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_pop_string(bas,arg,&Title));
    mb_check(mb_attempt_func_end(bas,arg));

    g_WTB_Scripting->SetTitle(Data->Inst,Title);

    return MB_FUNC_OK;
}

int GetTitleFn(struct mb_interpreter_t *bas, void **arg)
{
    int junk;
    char Title[256];
    struct WTBasicContext *Data;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_open_bracket(bas,arg));
    mb_check(mb_pop_int(bas,arg,&junk));
    mb_check(mb_attempt_close_bracket(bas,arg));

    g_WTB_Scripting->GetTitle(Data->Inst,Title,sizeof(Title));
    mb_check(mb_push_string(bas,arg,mb_memdup(Title,(unsigned)(strlen(Title)+1))));

    return MB_FUNC_OK;
}

int ClearAreaFn(struct mb_interpreter_t *bas, void **arg)
{
    int x1,y1,x2,y2;
    struct WTBasicContext *Data;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_pop_int(bas,arg,&x1));
    mb_check(mb_pop_int(bas,arg,&y1));
    mb_check(mb_pop_int(bas,arg,&x2));
    mb_check(mb_pop_int(bas,arg,&y2));
    mb_check(mb_attempt_func_end(bas,arg));

    g_WTB_Scripting->DoClearArea(Data->Inst,x1,y1,x2,y2);

    return MB_FUNC_OK;
}

int ScrollAreaFn(struct mb_interpreter_t *bas, void **arg)
{
    int x1,y1,x2,y2;
    int dx,dy;
    struct WTBasicContext *Data;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_pop_int(bas,arg,&x1));
    mb_check(mb_pop_int(bas,arg,&y1));
    mb_check(mb_pop_int(bas,arg,&x2));
    mb_check(mb_pop_int(bas,arg,&y2));
    mb_check(mb_pop_int(bas,arg,&dx));
    mb_check(mb_pop_int(bas,arg,&dy));
    mb_check(mb_attempt_func_end(bas,arg));

    g_WTB_Scripting->DoScrollArea(Data->Inst,x1,y1,x2,y2,dx,dy);

    return MB_FUNC_OK;
}

int InKeyFn(struct mb_interpreter_t *bas, void **arg)
{
    struct WTBasicContext *Data;
    struct PluginKeyPress key;
    char keystr[10];

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_attempt_func_end(bas,arg));

    keystr[0]=0;
    if(g_WTB_Scripting->ReadKeyboard(Data->Inst,&key,1)>0)
    {
        ConvertKey2String(&key,keystr,sizeof(keystr));
    }

    mb_check(mb_push_string(bas,arg,
            mb_memdup(keystr,(unsigned)(strlen(keystr)+1))));

    return MB_FUNC_OK;
}

int SendFn(struct mb_interpreter_t *bas, void **arg)
{
    mb_value_t val;
    string BuildStr;
    int RetValue;
    char buff[100];
    struct WTBasicContext *Data;
    const char *p;

    mb_get_userdata(bas,(void **)&Data);

    RetValue=MB_FUNC_OK;
    BuildStr="";
    mb_check(mb_attempt_func_begin(bas,arg));
    while(mb_has_arg(bas,arg))
    {
        mb_check(mb_pop_value(bas,arg,&val));
        switch(val.type)
        {
            case MB_DT_INT:
                sprintf(buff,"%d",val.value.integer);
                BuildStr+=buff;
            break;
            case MB_DT_REAL:
                sprintf(buff,"%g",val.value.float_point);
                BuildStr+=buff;
            break;
            case MB_DT_STRING:
                p=val.value.string;
                while(*p!=0)
                {
                    BuildStr+=*p;
                    p++;
                }
            break;
            case MB_DT_NIL:
            case MB_DT_UNKNOWN:
            case MB_DT_NUM:
            case MB_DT_TYPE:
            case MB_DT_USERTYPE:
            case MB_DT_USERTYPE_REF:
            case MB_DT_ARRAY:
            case MB_DT_LIST:
            case MB_DT_LIST_IT:
            case MB_DT_DICT:
            case MB_DT_DICT_IT:
            case MB_DT_COLLECTION:
            case MB_DT_ITERATOR:
            case MB_DT_CLASS:
            case MB_DT_ROUTINE:
            default:
                RetValue=MB_FUNC_ERR;
            break;
        }
    }
    mb_check(mb_attempt_func_end(bas,arg));

    /* Write the last of the string */
    g_WTB_Scripting->WriteCom(Data->Inst,
            (const uint8_t *)BuildStr.c_str(),BuildStr.length());

    return RetValue;
}

int RPrintFn(struct mb_interpreter_t *bas, void **arg)
{
    mb_value_t val;
    string BuildStr;
    int RetValue;
    char buff[100];
    struct WTBasicContext *Data;
    const char *p;

    mb_get_userdata(bas,(void **)&Data);

    RetValue=MB_FUNC_OK;
    BuildStr="";
    mb_check(mb_attempt_func_begin(bas,arg));
    while(mb_has_arg(bas,arg))
    {
        mb_check(mb_pop_value(bas,arg,&val));
        switch(val.type)
        {
            case MB_DT_INT:
                sprintf(buff,"%d",val.value.integer);
                BuildStr+=buff;
            break;
            case MB_DT_REAL:
                sprintf(buff,"%g",val.value.float_point);
                BuildStr+=buff;
            break;
            case MB_DT_STRING:
                p=val.value.string;
                while(*p!=0)
                {
                    if(*p=='\r')
                    {
                        /* Write what we have already */
                        g_WTB_Scripting->WriteCom(Data->Inst,
                                (const uint8_t *)BuildStr.c_str(),
                                BuildStr.length());
                        BuildStr="";
                        g_WTB_Scripting->SendEnter(Data->Inst);
                    }
                    else if(*p=='\b')
                    {
                        /* Write what we have already */
                        g_WTB_Scripting->WriteCom(Data->Inst,
                                (const uint8_t *)BuildStr.c_str(),
                                BuildStr.length());
                        BuildStr="";
                        g_WTB_Scripting->SendBackspace(Data->Inst);
                    }
                    else
                    {
                        BuildStr+=*p;
                    }
                    p++;
                }
            break;
            case MB_DT_NIL:
            case MB_DT_UNKNOWN:
            case MB_DT_NUM:
            case MB_DT_TYPE:
            case MB_DT_USERTYPE:
            case MB_DT_USERTYPE_REF:
            case MB_DT_ARRAY:
            case MB_DT_LIST:
            case MB_DT_LIST_IT:
            case MB_DT_DICT:
            case MB_DT_DICT_IT:
            case MB_DT_COLLECTION:
            case MB_DT_ITERATOR:
            case MB_DT_CLASS:
            case MB_DT_ROUTINE:
            default:
                RetValue=MB_FUNC_ERR;
            break;
        }
    }
    mb_check(mb_attempt_func_end(bas,arg));

    /* Write the last of the string */
    g_WTB_Scripting->WriteCom(Data->Inst,
            (const uint8_t *)BuildStr.c_str(),BuildStr.length());

    return RetValue;
}

int RecvFn(struct mb_interpreter_t *bas, void **arg)
{
    struct WTBasicContext *Data;
    uint8_t InBytes[256];
    unsigned int BytesReady;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_attempt_func_end(bas,arg));

    InBytes[0]=0;
    BytesReady=g_WTB_Scripting->ReadCom(Data->Inst,InBytes,sizeof(InBytes)-1);
    InBytes[BytesReady]=0;

    mb_check(mb_push_string(bas,arg,
            mb_memdup((char *)InBytes,(unsigned)(BytesReady+1))));

    return MB_FUNC_OK;
}

int KeyboardFn(struct mb_interpreter_t *bas, void **arg)
{
    struct WTBasicContext *Data;
    int Enable;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_pop_int(bas,arg,&Enable));
    mb_check(mb_attempt_func_end(bas,arg));

    g_WTB_Scripting->DisableKeyboardSend(Data->Inst,Enable);

    return MB_FUNC_OK;
}

int ScreenFn(struct mb_interpreter_t *bas, void **arg)
{
    struct WTBasicContext *Data;
    int Enable;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_pop_int(bas,arg,&Enable));
    mb_check(mb_attempt_func_end(bas,arg));

    g_WTB_Scripting->DisableScreenDisplay(Data->Inst,Enable);

    return MB_FUNC_OK;
}

int StdioFn(struct mb_interpreter_t *bas, void **arg)
{
    struct WTBasicContext *Data;
    int Local;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_pop_int(bas,arg,&Local));
    mb_check(mb_attempt_func_end(bas,arg));

    m_StdioGoes2Com=!Local;

    return MB_FUNC_OK;
}
