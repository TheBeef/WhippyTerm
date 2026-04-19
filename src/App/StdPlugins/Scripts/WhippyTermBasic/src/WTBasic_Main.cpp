/*

Need a call from the script thread when the script is done so the GUI can
update it's self (diable the stop button for example)
---
Basic commands:
    x Open
    x Close
    x Write
    x Read
    x Seek
    x Tell
    x mkdir
    x rmdir
    x DeleteFile
    x Dir
    x IsDir

    x WriteCom
    x ReadCom
    x WriteScreen
    x ReadKeyboard
    x Wait <- Wait for one of the provided strings
    x WaitCom <- Wait for one of the provided strings
    * GetListOfUploadTypes
    * GetListOfDownloadTypes
    x StartDownload
    x StartUpload
    * AbortFileTransfer
    x SendFile?
    x RecvFile?
    * FlushKeyboard
    * FlushCom

    * OpenURI
    * OpenConnection
    * CloseConnection
    * GetConnectionStatus   <- Open/Close
    * ChangeOptions
    * ChangeAuxControl
    * BridgeConnections
    * SetTerminalEmulation
    * SetCharacterEncoding
    * Enable/Disable Data Processor
    * SetTerminalDataProcessorMode
    * SetAutoConnect
    * SetTranDelay
    * GetConnectionTitle
    * SetConnectionTitle

    x Cls
    x Sleep
    * ClearHex
    * SetZoom
    * InsertHR
    * ShowPanel
    * beep
    * MessageBox
    * CRC
    * Date
    * StopWatch
    * Capture

    * Strings need to support 0 (chr$(0))

*/
/*******************************************************************************
 * FILENAME: WTBasic_Main.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the WhippyTerm scripting language plugin it in.  This is
 *    based on MY-BASIC and uses the MY-BASIC parser
 *    (https://github.com/paladin-t/my_basic), but has WhippyTerm extensions
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
#include "OS/WTB_OSFile.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;

/*** DEFINES                  ***/
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      WTBasic // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x02020000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct RegisteredKeyword
{
    string Namespace;
    string Keyword;
    e_ScriptDataArgType RetType;
    const struct ScriptDataType *Args;
    unsigned int ArgCount;
    int OptionalArgStart;
};

typedef map<string,struct RegisteredKeyword> t_RegisteredKeywordList;
typedef t_RegisteredKeywordList::iterator i_RegisteredKeywordList;

struct WTBasicContext
{
    t_ScriptingEngineInstType *Inst;
    struct mb_interpreter_t *bas;
    string LastErrorMsg;
//    bool AbortScript;
    uint32_t WTBasic_Colors[e_SysColMAX*2];
    int ActiveFGColor;
    int ActiveBGColor;
    bool StdioGoes2Com;
    char *PrintBuff;
    unsigned int PrintBuffSize;
    t_RegisteredKeywordList RegKeywords;
};

/*** FUNCTION PROTOTYPES      ***/
PG_BOOL WTBasic_Init(void);
const char *WTBasic_GetLastError(t_ScriptingEngineContextType *Context);
t_ScriptingEngineContextType *WTBasic_AllocateContext(t_ScriptingEngineInstType *Inst);
void WTBasic_FreeContext(t_ScriptingEngineContextType *Context);
PG_BOOL WTBasic_LoadScriptFromString(t_ScriptingEngineContextType *Context,const char *Str);
PG_BOOL WTBasic_RunLoadedScript(t_ScriptingEngineContextType *Context);
void WTBasic_AbortScript(t_ScriptingEngineContextType *Context);
static bool WTBasic_RegisterKeyword(t_ScriptingEngineContextType *Context,
        const char *Namespace,const char *Keyword,
        e_ScriptDataArgType RetType,int OptionalArgStart,
        const struct ScriptDataType *Args,unsigned int ArgCount);
static void ConvertKey2String(struct PluginKeyPress *key,char *RetStr,int Size);

int WriteStdOut(struct mb_interpreter_t *bas,const char *fmt,...);
int ReadLineStdIn(struct mb_interpreter_t *bas,const char *pmt,char *buf,int s);
static int WaitFnCommon(struct mb_interpreter_t *bas, void **arg,bool ForceCom);

static int SleepFn(struct mb_interpreter_t *bas, void **arg);
static int mSleepFn(struct mb_interpreter_t *bas, void **arg);
static int SPrintFn(struct mb_interpreter_t *bas, void **arg);
static int LocateFn(struct mb_interpreter_t *bas, void **arg);
static int ClsFn(struct mb_interpreter_t *bas, void **arg);
static int PosFn(struct mb_interpreter_t *bas, void **arg);
static int CsrLinFn(struct mb_interpreter_t *bas, void **arg);
static int ScreenSizeFn(struct mb_interpreter_t *bas, void **arg);
static int ColorFn(struct mb_interpreter_t *bas, void **arg);
static int PaletteFn(struct mb_interpreter_t *bas, void **arg);
static int SetTitleFn(struct mb_interpreter_t *bas, void **arg);
static int GetTitleFn(struct mb_interpreter_t *bas, void **arg);
static int ClearAreaFn(struct mb_interpreter_t *bas, void **arg);
static int ScrollAreaFn(struct mb_interpreter_t *bas, void **arg);
static int InKeyFn(struct mb_interpreter_t *bas, void **arg);
static int SendFn(struct mb_interpreter_t *bas, void **arg);
static int CPrintFn(struct mb_interpreter_t *bas, void **arg);
static int RecvFn(struct mb_interpreter_t *bas, void **arg);
static int KeyboardFn(struct mb_interpreter_t *bas, void **arg);
static int ScreenFn(struct mb_interpreter_t *bas, void **arg);
static int CMDFn(struct mb_interpreter_t *bas, void **arg);
static int OpenFn(struct mb_interpreter_t *bas, void **arg);
static int CloseFn(struct mb_interpreter_t *bas, void **arg);
static int TellFn(struct mb_interpreter_t *bas, void **arg);
static int SeekFn(struct mb_interpreter_t *bas, void **arg);
static int ReadFn(struct mb_interpreter_t *bas, void **arg);
static int WriteFn(struct mb_interpreter_t *bas, void **arg);
static int MkdirFn(struct mb_interpreter_t *bas, void **arg);
static int RmdirFn(struct mb_interpreter_t *bas, void **arg);
static int DelFileFn(struct mb_interpreter_t *bas, void **arg);
static int DirFn(struct mb_interpreter_t *bas, void **arg);
static int IsDirFn(struct mb_interpreter_t *bas, void **arg);
static int WaitFn(struct mb_interpreter_t *bas, void **arg);
static int WaitComFn(struct mb_interpreter_t *bas, void **arg);
static int NlFn(struct mb_interpreter_t *bas, void **arg);
static int GenericKeywordFn(struct mb_interpreter_t *bas, void **arg);

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
    WTBasic_RegisterKeyword,
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
//char *g_PrintBuff;              // Move to inst
//unsigned int g_PrintBuffSize;   // Move to inst

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
        NewContext->StdioGoes2Com=true;
        NewContext->PrintBuff=NULL;
        NewContext->PrintBuffSize=0;

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
        mb_register_func(NewContext->bas,"SPRINT",SPrintFn);
        mb_register_func(NewContext->bas,"CPRINT",CPrintFn);
        mb_register_func(NewContext->bas,"SEND",SendFn);
        mb_register_func(NewContext->bas,"RECV",RecvFn);
        mb_register_func(NewContext->bas,"KEYBOARD",KeyboardFn);
        mb_register_func(NewContext->bas,"SCREEN",ScreenFn);
        mb_register_func(NewContext->bas,"CMD",CMDFn);
        mb_register_func(NewContext->bas,"WAIT",WaitFn);
        mb_register_func(NewContext->bas,"WAITCOM",WaitComFn);
        mb_register_func(NewContext->bas,"NL",NlFn);

        /* File IO */
        mb_register_func(NewContext->bas,"OPEN",OpenFn);
        mb_register_func(NewContext->bas,"CLOSE",CloseFn);
        mb_register_func(NewContext->bas,"TELL",TellFn);
        mb_register_func(NewContext->bas,"SEEK",SeekFn);
        mb_register_func(NewContext->bas,"READ",ReadFn);
        mb_register_func(NewContext->bas,"WRITE",WriteFn);
        mb_register_func(NewContext->bas,"MKDIR",MkdirFn);
        mb_register_func(NewContext->bas,"RMDIR",RmdirFn);
        mb_register_func(NewContext->bas,"DELFILE",DelFileFn);
        mb_register_func(NewContext->bas,"DIR$",DirFn);
        mb_register_func(NewContext->bas,"ISDIR",IsDirFn);

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

    if(Data->PrintBuff!=NULL)
    {
        free(Data->PrintBuff);
        Data->PrintBuffSize=0;
    }

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
        if(FileName!=NULL)
        {
            Data->LastErrorMsg+=" of file \"";
            Data->LastErrorMsg+=FileName;
            Data->LastErrorMsg+="\"";
        }
        Data->LastErrorMsg+=":\n";

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
            if(FileName!=NULL)
            {
                Data->LastErrorMsg+=" of file \"";
                Data->LastErrorMsg+=FileName;
                Data->LastErrorMsg+="\"";
            }
            Data->LastErrorMsg+=":\n";
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

bool WTBasic_RegisterKeyword(t_ScriptingEngineContextType *Context,
        const char *Namespace,const char *Keyword,
        e_ScriptDataArgType RetType,int OptionalArgStart,
        const struct ScriptDataType *Args,unsigned int ArgCount)
{
    struct WTBasicContext *Data=(struct WTBasicContext *)Context;
    string TmpName;
    struct RegisteredKeyword NewRegKeyword;

    try
    {
        /* For basic we just put the namespace and keyword together */
        TmpName=Namespace;
        TmpName+=".";
        TmpName+=Keyword;
        transform(TmpName.begin(),TmpName.end(),TmpName.begin(),::toupper);

        if(mb_register_func(Data->bas,TmpName.c_str(),GenericKeywordFn)==0)
            throw(0);

        NewRegKeyword.Namespace=Namespace;
        NewRegKeyword.Keyword=Keyword;
        NewRegKeyword.RetType=RetType;
        NewRegKeyword.Args=Args;
        NewRegKeyword.ArgCount=ArgCount;
        NewRegKeyword.OptionalArgStart=OptionalArgStart;

        Data->RegKeywords.insert({TmpName,NewRegKeyword});
    }
    catch(...)
    {
        return false;
    }

    return true;
}

static int GenericKeywordFn(struct mb_interpreter_t *bas, void **arg)
{
    struct WTBasicContext *Data;
    const char *fnname;
    i_RegisteredKeywordList keyword;
    unsigned int a;
    const struct ScriptDataType *curarg;
    float ArgReal;
    int ArgInt;
    struct ScriptArgValue *ExeArgs;
    unsigned int ExeArgsCount;
    char *ValueStr;
    char TmpBuff[100];
    int RetValue;
    int ArgRet;
    char *RetStr;
    bool Success;

    ExeArgs=NULL;
    ExeArgsCount=0;
    RetStr=NULL;
    try
    {
        mb_get_userdata(bas,(void **)&Data);

        fnname=GetCurrentFuncName(bas,arg);

        /* We need to find this keyword */
        keyword=Data->RegKeywords.find(fnname);
        if(keyword==Data->RegKeywords.end())
        {
            throw(SE_CM_FUNC_DOES_NOT_EXIST);
        }

        if(keyword->second.RetType!=e_ScriptDataArg_None)
        {
            /* Should use () */
            mb_check(mb_attempt_open_bracket(bas,arg));
        }
        else
        {
            mb_check(mb_attempt_func_begin(bas,arg));
        }

        ExeArgsCount=keyword->second.ArgCount;
        ExeArgs=new struct ScriptArgValue[ExeArgsCount];

        /* Default all the args (include optional ones) */
        for(a=0;a<ExeArgsCount;a++)
        {
            curarg=&(keyword->second.Args[a]);
            ExeArgs[a].ArgName=curarg->ArgName;
            ExeArgs[a].Value=NULL;
        }

        /* Grab args */
        for(a=0;a<ExeArgsCount;a++)
        {
            curarg=&(keyword->second.Args[a]);

            if(!mb_has_arg(bas,arg))
            {
                if(keyword->second.OptionalArgStart>=0 &&
                        a>=keyword->second.OptionalArgStart)
                {
                    /* This arg was optional, so we are done */
                    break;
                }
                else
                {
                    mb_raise_error(bas,arg,SE_RN_VAR_EXPECTED,
                            MB_FUNC_ERR);
                    throw(0);
                }
            }
            switch(curarg->ArgType)
            {
                case e_ScriptDataArg_String:
                    ArgRet=mb_pop_string(bas,arg,&ValueStr);
                    if(ArgRet!=MB_FUNC_OK)
                    {
                        mb_raise_error(bas,arg,SE_RN_STRING_EXPECTED,
                                MB_FUNC_ERR);
                        throw(0);
                    }
                break;
                case e_ScriptDataArg_Bool:
                    ArgRet=mb_pop_int(bas,arg,&ArgInt);
                    if(ArgRet!=MB_FUNC_OK)
                    {
                        mb_raise_error(bas,arg,SE_RN_INTEGER_EXPECTED,
                                MB_FUNC_ERR);
                        throw(0);
                    }
                    ValueStr=TmpBuff;
                    sprintf(ValueStr,"%d",ArgInt);
                break;
                case e_ScriptDataArg_Int:
                    ArgRet=mb_pop_int(bas,arg,&ArgInt);
                    if(ArgRet!=MB_FUNC_OK)
                    {
                        mb_raise_error(bas,arg,SE_RN_INTEGER_EXPECTED,
                                MB_FUNC_ERR);
                        throw(0);
                    }
                    ValueStr=TmpBuff;
                    sprintf(ValueStr,"%d",ArgInt);
                break;
                case e_ScriptDataArg_Float:
                    ArgRet=mb_pop_real(bas,arg,&ArgReal);
                    if(ArgRet!=MB_FUNC_OK)
                    {
                        mb_raise_error(bas,arg,SE_RN_NUMBER_EXPECTED,
                                MB_FUNC_ERR);
                        throw(0);
                    }
                    ValueStr=TmpBuff;
                    sprintf(ValueStr,"%g",ArgReal);
                break;
                case e_ScriptDataArg_None:
                case e_ScriptDataArgMAX:
                    /* Unknown arg type */
                    throw(SE_CM_NOT_SUPPORTED);
                break;
            }
            ExeArgs[a].Value=new char[strlen(ValueStr)+1];
            strcpy(ExeArgs[a].Value,ValueStr);
        }

        if(keyword->second.RetType!=e_ScriptDataArg_None)
        {
            mb_check(mb_attempt_close_bracket(bas,arg));
        }
        else
        {
            mb_check(mb_attempt_func_end(bas,arg));
        }

        Success=g_WTB_Scripting->ExeRegisteredKeyword(Data->Inst,
                keyword->second.Namespace.c_str(),
                keyword->second.Keyword.c_str(),
                &RetStr,ExeArgs,ExeArgsCount);
        if(!Success)
        {
            mb_raise_error(bas,arg,SE_CM_NOT_SUPPORTED,MB_FUNC_ERR);
            throw(SE_RN_FAILED_TO_OPERATE);
        }

        switch(keyword->second.RetType)
        {
            case e_ScriptDataArg_String:
    //        mb_check(mb_push_string(bas,arg,mb_memdup("\15\12",3)));
            break;
            case e_ScriptDataArg_Bool:
    //        mb_check(mb_push_string(bas,arg,mb_memdup("\15\12",3)));
            break;
            case e_ScriptDataArg_Int:
    //        mb_check(mb_push_string(bas,arg,mb_memdup("\15\12",3)));
            break;
            case e_ScriptDataArg_Float:
    //        mb_check(mb_push_string(bas,arg,mb_memdup("\15\12",3)));
            break;
            case e_ScriptDataArg_None:
            case e_ScriptDataArgMAX:
            break;
        }
        RetValue=MB_FUNC_OK;
    }
    catch(mb_error_e ErrCode)
    {
        mb_raise_error(bas,arg,ErrCode,MB_FUNC_ERR);
        RetValue=MB_FUNC_ERR;
    }
    catch(...)
    {
        /* Assume out of memory */
        mb_raise_error(bas,arg,SE_RN_OUT_OF_MEMORY,MB_FUNC_ERR);
        RetValue=MB_FUNC_ERR;
    }

    if(ExeArgs!=NULL)
    {
        for(a=0;a<ExeArgsCount;a++)
        {
            if(ExeArgs[a].Value!=NULL)
                delete[] ExeArgs[a].Value;
        }
    }

    if(RetStr!=NULL)
        g_WTB_Scripting->FreeExeRegisteredKeywordRetStr(Data->Inst,&RetStr);

    if(ExeArgs!=NULL)
        delete[] ExeArgs;

    return RetValue;
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
    char *NewPrintBuff;

    mb_get_userdata(bas,(void **)&Data);

    if(Data->PrintBuff==NULL)
    {
        Data->PrintBuff=(char *)malloc(100);
        if(Data->PrintBuff==NULL)
            return 0;
        Data->PrintBuffSize=100;
    }

    va_start(arg,fmt);
    ret=vsnprintf(NULL,0,fmt,arg);
    va_end(arg);
    if(ret>(int)Data->PrintBuffSize)
    {
        /* Resize */
        NewPrintBuff=(char *)realloc(Data->PrintBuff,ret+1);
        if(NewPrintBuff==NULL)
            return 0;
        Data->PrintBuff=NewPrintBuff;
    }
    va_start(arg,fmt);
    ret=vsnprintf(Data->PrintBuff,Data->PrintBuffSize,fmt,arg);
    va_end(arg);

    Start=Data->PrintBuff;
    p=Data->PrintBuff;
    while(*p!=0)
    {
        if(*p=='\n')
        {
            /* Write what we have already */
            if(Data->StdioGoes2Com)
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
            if(Data->StdioGoes2Com)
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
    if(Data->StdioGoes2Com)
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

    if(Data->StdioGoes2Com)
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


int SPrintFn(struct mb_interpreter_t *bas, void **arg)
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

int CPrintFn(struct mb_interpreter_t *bas, void **arg)
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

int CMDFn(struct mb_interpreter_t *bas, void **arg)
{
    struct WTBasicContext *Data;
    int Local;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_pop_int(bas,arg,&Local));
    mb_check(mb_attempt_func_end(bas,arg));

    Data->StdioGoes2Com=!Local;

    return MB_FUNC_OK;
}

static int OpenFn(struct mb_interpreter_t* s, void** l)
{
    int result = MB_FUNC_OK;
    FILE* fp = 0;
    char* fn = 0;
    char* fm = 0;

    mb_check(mb_attempt_open_bracket(s, l));
    mb_check(mb_pop_string(s, l, &fn));
    mb_check(mb_pop_string(s, l, &fm));
    mb_check(mb_attempt_close_bracket(s, l));

    if(!fn)
    {
        result = MB_FUNC_ERR;

        goto _exit;
    }

    fp = fopen(fn, fm);
    if(!fp)
    {
        result = MB_FUNC_ERR;

        goto _exit;
    }

_exit:
    mb_check(mb_push_usertype(s, l, (void*)fp));

    return result;
}

static int CloseFn(struct mb_interpreter_t* s, void** l)
{
    int result = MB_FUNC_OK;
    FILE* fp = 0;
    void* up = 0;

    mb_check(mb_attempt_open_bracket(s, l));
    mb_check(mb_pop_usertype(s, l, &up));
    mb_check(mb_attempt_close_bracket(s, l));

    if(!up)
        return MB_FUNC_ERR;

    fp = (FILE*)up;
    fclose(fp);

    return result;
}

static int TellFn(struct mb_interpreter_t* s, void** l)
{
    int result = MB_FUNC_OK;
    FILE* fp = 0;
    void* up = 0;
    long ft = -1; /* Push -1 for error */

    mb_check(mb_attempt_open_bracket(s, l));
    mb_check(mb_pop_usertype(s, l, &up));
    mb_check(mb_attempt_close_bracket(s, l));

    if(!up) {
        result = MB_FUNC_ERR;

        goto _exit;
    }

    fp = (FILE*)up;
    ft = ftell(fp);

_exit:
    mb_check(mb_push_int(s, l, ft));

    return result;
}

static int SeekFn(struct mb_interpreter_t* s, void** l)
{
    int result = MB_FUNC_OK;
    FILE* fp = 0;
    void* up = 0;
    int fo = 0;
    int set = 0;
    int What;

    mb_check(mb_attempt_open_bracket(s, l));
    mb_check(mb_pop_usertype(s, l, &up));
    mb_check(mb_pop_int(s, l, &fo));
    mb_check(mb_pop_int(s, l, &set));
    mb_check(mb_attempt_close_bracket(s, l));

    if(!up)
        return MB_FUNC_ERR;

    switch(set)
    {
        case 0:
            What=SEEK_SET;
        break;
        case 1:
            What=SEEK_CUR;
        break;
        case 2:
            What=SEEK_END;
        break;
        default:
            return MB_FUNC_ERR;
    }

    fp = (FILE*)up;
    fseek(fp, fo, What);

    return result;
}

static int ReadFn(struct mb_interpreter_t* s, void** l)
{
    int result = MB_FUNC_OK;
    FILE* fp = 0;
    void* up = 0;
    int ln = 0;

    mb_check(mb_attempt_open_bracket(s, l));
    mb_check(mb_pop_usertype(s, l, &up));
    if(mb_has_arg(s, l))
    {
        mb_check(mb_pop_int(s, l, &ln));
    }
    mb_check(mb_attempt_close_bracket(s, l));

    if(!up)
    {
        result = MB_FUNC_ERR;
        goto _exit;
    }

    fp = (FILE*)up;
    if(ln)
    {
        char* buf = (char*)malloc(ln + 1);
        fgets(buf, ln + 1, fp);
        buf[ln] = '\0';
        mb_check(mb_push_string(s, l, mb_memdup(buf, ln + 1)));
        free(buf);
    }
    else
    {
        int ret = fgetc(fp);
        mb_check(mb_push_int(s, l, ret));
    }

_exit:
    return result;
}

static int WriteFn(struct mb_interpreter_t* s, void** l)
{
    int result = MB_FUNC_OK;
    FILE* fp = 0;
    void* up = 0;
    mb_value_t val;

    mb_check(mb_attempt_open_bracket(s, l));
    mb_check(mb_pop_usertype(s, l, &up));
    if(!up)
    {
        result = MB_FUNC_ERR;
        goto _exit;
    }

    fp = (FILE*)up;

    while(mb_has_arg(s, l))
    { /* Support variadic */
        mb_check(mb_pop_value(s, l, &val));
        switch(val.type)
        {
            case MB_DT_INT:
                fputc(val.value.integer, fp);
            break;
            case MB_DT_REAL:
                fputc((int_t)val.value.float_point, fp);
            break;
            case MB_DT_STRING:
                fputs(val.value.string, fp);
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
                result = MB_FUNC_ERR;
            break;
        }
    }

_exit:
    mb_check(mb_attempt_close_bracket(s, l));

    return result;
}

int MkdirFn(struct mb_interpreter_t *bas, void **arg)
{
    struct WTBasicContext *Data;
    char *DirName;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_pop_string(bas,arg,&DirName));
    mb_check(mb_attempt_func_end(bas,arg));

    WTB_Mkdir(DirName);

    return MB_FUNC_OK;
}

int RmdirFn(struct mb_interpreter_t *bas, void **arg)
{
    struct WTBasicContext *Data;
    char *DirName;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_pop_string(bas,arg,&DirName));
    mb_check(mb_attempt_func_end(bas,arg));

    if(DirName[0]!=0)
    {
        WTB_Rmdir(DirName);
    }
    else
    {
        return MB_FUNC_ERR;
    }

    return MB_FUNC_OK;
}

int DelFileFn(struct mb_interpreter_t *bas, void **arg)
{
    struct WTBasicContext *Data;
    char *DirFilename;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_pop_string(bas,arg,&DirFilename));
    mb_check(mb_attempt_func_end(bas,arg));

    if(DirFilename[0]!=0)
    {
        remove(DirFilename);
    }
    else
    {
        mb_raise_error(bas,arg,SE_RN_STRING_EXPECTED,MB_FUNC_ERR);
        return MB_FUNC_ERR;
    }

    return MB_FUNC_OK;
}

int DirFn(struct mb_interpreter_t *bas, void **arg)
{
    struct WTBasicContext *Data;
    char *DirPath;
    void *array;
    int dim;
    mb_value_t val;
    void *Dir;
    int Count;
    const char *FileName;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_open_bracket(bas,arg));
    mb_check(mb_pop_string(bas,arg,&DirPath));
    mb_check(mb_attempt_close_bracket(bas,arg));

    if(DirPath[0]==0)
    {
        mb_raise_error(bas,arg,SE_RN_STRING_EXPECTED,MB_FUNC_ERR);
        return MB_FUNC_ERR;
    }

    Dir=WTB_OpenDir(DirPath);
    if(Dir==NULL)
    {
        mb_raise_error(bas,arg,SE_PS_FAILED_TO_OPEN_FILE,MB_FUNC_ERR);
        return MB_FUNC_ERR;
    }
    Count=0;
    while(WTB_NextDirEntry(Dir)!=NULL)
        Count++;
    WTB_CloseDir(Dir);

    dim=Count;
    mb_init_array(bas,arg,MB_DT_STRING,&dim,1,&array);

    /* Walk it again (I know, I know, it could have changed between scans,
       don't care...) */
    Dir=WTB_OpenDir(DirPath);
    if(Dir==NULL)
    {
        mb_raise_error(bas,arg,SE_PS_FAILED_TO_OPEN_FILE,MB_FUNC_ERR);
        return MB_FUNC_ERR;
    }

    for(dim=0;dim<Count;dim++)
    {
        FileName=WTB_NextDirEntry(Dir);
        if(FileName==NULL)
        {
            /* Unexpected. */
            FileName="";
        }

        val.type=MB_DT_STRING;
        val.value.string = (char *)FileName;
        mb_set_array_elem(bas,arg,array,&dim,1,val);
    }

    WTB_CloseDir(Dir);

    val.type=MB_DT_ARRAY;
    val.value.array=array;
    mb_check(mb_push_value(bas,arg,val));

    return MB_FUNC_OK;
}

int IsDirFn(struct mb_interpreter_t *bas, void **arg)
{
    struct WTBasicContext *Data;
    char *FileName;
    bool IsDir;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_open_bracket(bas,arg));
    mb_check(mb_pop_string(bas,arg,&FileName));
    mb_check(mb_attempt_close_bracket(bas,arg));

    if(FileName[0]==0)
    {
        mb_raise_error(bas,arg,SE_RN_STRING_EXPECTED,MB_FUNC_ERR);
        return MB_FUNC_ERR;
    }

    IsDir=WTB_IsDir(FileName);
    mb_check(mb_push_int(bas,arg,IsDir));

    return MB_FUNC_OK;
}

static int NlFn(struct mb_interpreter_t *bas, void **arg)
{
    struct WTBasicContext *Data;
//    int Type;

    mb_get_userdata(bas,(void **)&Data);

    mb_check(mb_attempt_func_begin(bas,arg));
    mb_check(mb_attempt_func_end(bas,arg));
//    mb_check(mb_attempt_open_bracket(bas,arg));
//    mb_check(mb_attempt_close_bracket(bas,arg));

    mb_check(mb_push_string(bas,arg,mb_memdup("\15\12",3)));

    return MB_FUNC_OK;
}

static int WaitFn(struct mb_interpreter_t *bas, void **arg)
{
    return WaitFnCommon(bas,arg,false);
}

static int WaitComFn(struct mb_interpreter_t *bas, void **arg)
{
    return WaitFnCommon(bas,arg,true);
}

static int WaitFnCommon(struct mb_interpreter_t *bas, void **arg,bool ForceCom)
{
    struct WTBasicContext *Data;
    int result;
    mb_value_t val;
    list<string> ListOfStrings;
    list<string>::iterator i;
    vector<unsigned int> MatchCounts;
    int FoundIndex;
    unsigned int l;
    unsigned int r;
    char TmpBuff[10];
    struct PluginKeyPress key;
    unsigned int o;
    bool DoingFirst;
    int Timeout;
    int TimeoutCount;

    try
    {
        result=MB_FUNC_OK;

        mb_get_userdata(bas,(void **)&Data);

        Timeout=-1;
        DoingFirst=true;
        mb_check(mb_attempt_func_begin(bas,arg));
        while(mb_has_arg(bas,arg))
        {
            mb_check(mb_pop_value(bas,arg,&val));
            if(val.type==MB_DT_INT && DoingFirst)
            {
                /* The first arg can be a number for timeout in ms */
                Timeout=val.value.integer;
                DoingFirst=false;
                continue;
            }
            DoingFirst=false;
            if(val.type!=MB_DT_STRING)
            {
                /* We can only wait on strings */
                throw(SE_RN_STRING_EXPECTED);
            }
            ListOfStrings.push_back(val.value.string);
            MatchCounts.push_back(0);
        }
        mb_check(mb_attempt_func_end(bas,arg));

        /* Now we wait until we see one of these strings */
        TimeoutCount=0;
        FoundIndex=-1;
        while(FoundIndex<0)
        {
            if(Data->StdioGoes2Com || ForceCom)
            {
                l=g_WTB_Scripting->ReadCom(Data->Inst,(uint8_t *)TmpBuff,
                        sizeof(TmpBuff));
            }
            else
            {
                l=0;
                if(g_WTB_Scripting->ReadKeyboard(Data->Inst,&key,1)>0)
                {
                    ConvertKey2String(&key,TmpBuff,sizeof(TmpBuff)-1);
                    if(TmpBuff[0]<32)
                    {
                        /* Ignore control codes */
                        continue;
                    }
                    l=strlen(TmpBuff);
                }
            }
            for(r=0;r<l;r++)
            {
                for(o=0,i=ListOfStrings.begin();i!=ListOfStrings.end();i++,o++)
                {
                    if((*i)[MatchCounts[o]]!=TmpBuff[r])
                        MatchCounts[o]=0;

                    if((*i)[MatchCounts[o]]==TmpBuff[r])
                    {
                        MatchCounts[o]++;
                        if(MatchCounts[o]==i->length())
                        {
                            FoundIndex=o;
                            break;
                        }
                    }
                }
                if(i!=ListOfStrings.end())
                    break;
            }

            WTB_Sleep(1);

            if(Timeout>=0)
            {
                TimeoutCount++;
                if(TimeoutCount>=Timeout)
                    break;
            }
        }

        mb_check(mb_push_int(bas,arg,FoundIndex));
    }
    catch(mb_error_e err)
    {
        mb_raise_error(bas,arg,err,MB_FUNC_ERR);
        result=MB_FUNC_ERR;
    }
    catch(...)
    {
        mb_raise_error(bas,arg,SE_RN_OUT_OF_MEMORY,MB_FUNC_ERR);
        result=MB_FUNC_ERR;
    }

    return result;
}

