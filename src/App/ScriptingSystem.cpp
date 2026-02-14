/*******************************************************************************
 * FILENAME: ScriptingSystem.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the scripting system in it.
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
#include "App/Connections.h"
#include "App/PluginSupport/StyleData.h"
#include "App/ScriptingSystem.h"
#include "App/Session.h"
#include "OS/OSTime.h"
#include "PluginSDK/ScriptingEngine.h"
#include "UI/UIAsk.h"
#include "UI/UISystem.h"
#include "UI/UIFileReq.h"
#include <list>
#include <string>
#include <queue>
#include <string.h>
#include <stdio.h>
#include <threads.h>

using namespace std;

/*** DEFINES                  ***/
#define MAX_SCRIPT_SIZE         1000000000  // Max file size is 1G (what are they thinking if they have a script that big?)
#define MAX_KEYPRESS_QUEUE_SIZE         16  // The max number of key press queue size per script instance

#define INCOMING_QUEUE_GROW_SIZE            1024
#define INCOMING_QUEUE_POLL_RATE_TIMEOUT    1500 // 1.5s
#define INCOMING_QUEUE_MAX_SIZE             10000000    // 1M

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct ScriptEngine
{
    string ID;
    string DisplayName;
    string LanguageGroup;
    string LanguageVariant;
    string FileExt;
    struct ScriptingEngineAPI API;
    bool InitBeenCalled;
    mtx_t UseCountMutex;
    int UseCount;   // How many scripts are running (++ on start, -- on end)
};
typedef list<struct ScriptEngine> t_ScriptEngineType;
typedef t_ScriptEngineType::iterator i_ScriptEngineType;

typedef queue<struct PluginKeyPress> t_KeyQueueType;

struct ScriptInComingQueue
{
    unsigned int Head;
    unsigned int Tail;
    uint8_t *Queue;
    unsigned int QueueSize;
    uint32_t LastPoll4BytesTime;
};

/* Belongs to the thread and the main thread, to free it you need to use MainThreadFreed & ThreadFreed flags */
struct ScriptEngineInstance
{
    /* Thread access only */
    thrd_t Thread;
    char *FileContents;
    char *StartOfScript;
    bool AbortedScript;

    /* Main thread access only */
    class Connection *ConnectedCon;
    class TheMainWindow *ConnectedMW;
    t_KeyQueueType KeyboardQueue;
    struct ScriptInComingQueue InComingQueue;

    /* Shared */
    mtx_t SharedMutex;
    volatile bool MainThreadFreed;   // If both of these are set to true then we should free the instance (thread or main)
    volatile bool ThreadFreed;
    /* Non-Mutexed Shared */
    struct ScriptEngine *ScriptEngine;
    t_ScriptingEngineContextType *Context;
    volatile bool ThreadWaiting2Run;
};

struct RPCAskData
{
    const char *Title;
    const char *Msg;
    e_AskBoxType BoxType;
    e_AskBttnsType Buttons;
};

struct RPCScriptDoneData
{
    struct ScriptEngineInstance *SEInstance;
};

struct RPCWriteScreenData
{
    struct ScriptEngineInstance *SEInstance;
    const uint8_t *Str;
    uint32_t Len;
};

struct RPCReadKeyboardData
{
    struct ScriptEngineInstance *SEInstance;
    struct PluginKeyPress *KeyPresses;
    uint32_t MaxCount;
    unsigned int RetValue;
};

struct RPCWriteComData
{
    struct ScriptEngineInstance *SEInstance;
    const uint8_t *Str;
    uint32_t Len;
};

struct RPCReadComData
{
    struct ScriptEngineInstance *SEInstance;
    uint8_t *Buffer;
    uint32_t BufferSize;
    unsigned int RetValue;
};

struct RPCDoConFunctionData
{
    struct ScriptEngineInstance *SEInstance;
    e_ConFuncType Fn;
    uintptr_t Arg1;
    uintptr_t Arg2;
    uintptr_t Arg3;
    uintptr_t Arg4;
    uintptr_t Arg5;
    uintptr_t Arg6;
};

struct RPCDoSystemBellData
{
    struct ScriptEngineInstance *SEInstance;
    int VisualOnly;
};

struct RPCGetCursorXYData
{
    struct ScriptEngineInstance *SEInstance;
    int32_t *RetCursorX;
    int32_t *RetCursorY;
};

struct RPCGetScreenSizeData
{
    struct ScriptEngineInstance *SEInstance;
    int32_t *RetWidth;
    int32_t *RetHeight;
};

struct RPCGetSysColorData
{
    struct ScriptEngineInstance *SEInstance;
    e_SysColShadeType SysColShade;
    e_SysColType SysColor;
    uint32_t RetColor;
};

struct RPCGetSysDefaultColorData
{
    struct ScriptEngineInstance *SEInstance;
    e_DefaultColorsType DefaultColor;
    uint32_t RetColor;
};

struct RPCSetStyleData
{
    struct ScriptEngineInstance *SEInstance;
    struct StyleData *Style;
};

struct RPCGetStyleData
{
    struct ScriptEngineInstance *SEInstance;
    struct StyleData *Style;
};

struct RPCDefaultStyleDataStructData
{
    struct ScriptEngineInstance *SEInstance;
    struct StyleData *Struct2Default;
};

struct RPCSetTitleData
{
    struct ScriptEngineInstance *SEInstance;
    const char *Title;
};

struct RPCGetTitleData
{
    struct ScriptEngineInstance *SEInstance;
    char *Title;
    unsigned int MaxLen;
    unsigned int RetLen;
};

struct RPCDisableKeyboardSendData
{
    struct ScriptEngineInstance *SEInstance;
    bool Enabled;
};

struct RPCDisableScreenDisplayData
{
    struct ScriptEngineInstance *SEInstance;
    bool Enabled;
};

/*** FUNCTION PROTOTYPES      ***/
static PG_BOOL Scripting_RegisterScriptingEngine(const struct ScriptingEngineInfo *Info,unsigned int SizeOfScriptingEngineInfo);
static uint32_t Scripting_GetSysColor(t_ScriptingEngineInstType *Inst,uint32_t SysColShade,uint32_t SysColor);
static uint32_t Scripting_GetSysDefaultColor(t_ScriptingEngineInstType *Inst,uint32_t DefaultColor);
static void Scripting_DoNewLine(t_ScriptingEngineInstType *Inst);
static void Scripting_DoReturn(t_ScriptingEngineInstType *Inst);
static void Scripting_DoBackspace(t_ScriptingEngineInstType *Inst);
static void Scripting_DoClearScreen(t_ScriptingEngineInstType *Inst);
static void Scripting_DoTab(t_ScriptingEngineInstType *Inst);
static void Scripting_DoPrevTab(t_ScriptingEngineInstType *Inst);
static void Scripting_DoSystemBell(t_ScriptingEngineInstType *Inst,int VisualOnly);
static void Scripting_DoClearScreenAndBackBuffer(t_ScriptingEngineInstType *Inst);
static void Scripting_GetCursorXY(t_ScriptingEngineInstType *Inst,int32_t *RetCursorX,int32_t *RetCursorY);
static void Scripting_SetCursorXY(t_ScriptingEngineInstType *Inst,uint32_t X,uint32_t Y);
static void Scripting_GetScreenSize(t_ScriptingEngineInstType *Inst,int32_t *RetRows,int32_t *RetColumns);
static void Scripting_SendBackspace(t_ScriptingEngineInstType *Inst);
static void Scripting_SendEnter(t_ScriptingEngineInstType *Inst);
static void Scripting_SetStyle(t_ScriptingEngineInstType *Inst,struct StyleData *NewStyle);
static void Scripting_GetStyle(t_ScriptingEngineInstType *Inst,struct StyleData *CurStyle);
static void Scripting_DefaultStyleDataStruct(t_ScriptingEngineInstType *Inst,struct StyleData *Struct2Default);
static void Scripting_SetTitle(t_ScriptingEngineInstType *Inst,const char *Title);
static unsigned int Scripting_GetTitle(t_ScriptingEngineInstType *Inst,char *Title,unsigned int MaxLen);
static void Scripting_DoClearArea(t_ScriptingEngineInstType *Inst,uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2);
static void Scripting_DoScrollArea(t_ScriptingEngineInstType *Inst,uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2,int32_t DeltaX,int32_t DeltaY);
static void Scripting_WriteScreen(t_ScriptingEngineInstType *Inst,const uint8_t *Str,uint32_t Len);
static unsigned int Scripting_ReadKeyboard(t_ScriptingEngineInstType *Inst,struct PluginKeyPress *KeyPresses,uint32_t MaxCount);
static void Scripting_WriteCom(t_ScriptingEngineInstType *Inst,const uint8_t *Str,uint32_t Len);
static unsigned int Scripting_ReadCom(t_ScriptingEngineInstType *Inst,uint8_t *Buffer,uint32_t BufferSize);
static void Scripting_DisableKeyboardSend(t_ScriptingEngineInstType *Inst,PG_BOOL Enabled);
static void Scripting_DisableScreenDisplay(t_ScriptingEngineInstType *Inst,PG_BOOL Enabled);

static int Scripting_MainThreadAskCB(struct UI_RPCData *RPCData);
static e_AskRetType Scripting_ThreadAsk(const char *Title,const char *Msg,e_AskBoxType BoxType=e_AskBox_Info,e_AskBttnsType Buttons=e_AskBttns_Default);
static int Scripting_MainThreadScriptDoneCB(struct UI_RPCData *RPCData);
static int Scripting_DoConFunctionCB(struct UI_RPCData *RPCData);
static int Scripting_DoSystemBellCB(struct UI_RPCData *RPCData);
static int Scripting_GetCursorXYCB(struct UI_RPCData *RPCData);
static int Scripting_GetScreenSizeCB(struct UI_RPCData *RPCData);
static int Scripting_GetSysColorCB(struct UI_RPCData *RPCData);
static int Scripting_GetSysDefaultColorCB(struct UI_RPCData *RPCData);
static int Scripting_SetStyleCB(struct UI_RPCData *RPCData);
static int Scripting_GetStyleCB(struct UI_RPCData *RPCData);
static int Scripting_DefaultStyleDataStructCB(struct UI_RPCData *RPCData);
static int Scripting_SetTitleCB(struct UI_RPCData *RPCData);
static int Scripting_GetTitleCB(struct UI_RPCData *RPCData);
static int Scripting_WriteScreenCB(struct UI_RPCData *RPCData);
static int Scripting_ReadKeyboardCB(struct UI_RPCData *RPCData);
static int Scripting_WriteComCB(struct UI_RPCData *RPCData);
static int Scripting_ReadComCB(struct UI_RPCData *RPCData);
static int Scripting_DisableKeyboardSendCB(struct UI_RPCData *RPCData);
static int Scripting_DisableScreenDisplayCB(struct UI_RPCData *RPCData);

static int RunScriptThread(void *data);

/*** VARIABLE DEFINITIONS     ***/
struct ScriptingSystem_API g_ScriptingAPI=
{
    /* V1 */
    Scripting_RegisterScriptingEngine,
    NULL, // const struct PI_UIAPI *(*Scripting_GetAPI_UI)(void);
    Scripting_GetSysColor,
    Scripting_GetSysDefaultColor,
    Scripting_DefaultStyleDataStruct,
    Scripting_SetStyle,
    Scripting_GetStyle,
    Scripting_GetTitle,
    Scripting_SetTitle,
    Scripting_DoNewLine,
    Scripting_DoReturn,
    Scripting_DoBackspace,
    Scripting_DoClearScreen,
    Scripting_DoClearArea,
    Scripting_DoTab,
    Scripting_DoPrevTab,
    Scripting_DoSystemBell,
    Scripting_DoScrollArea,
    Scripting_DoClearScreenAndBackBuffer,
    Scripting_GetCursorXY,
    Scripting_SetCursorXY,
    Scripting_GetScreenSize,
    Scripting_SendBackspace,
    Scripting_SendEnter,
    Scripting_WriteScreen,
    Scripting_ReadKeyboard,
    Scripting_WriteCom,
    Scripting_ReadCom,
    Scripting_DisableScreenDisplay,
    Scripting_DisableKeyboardSend,
    /* V2 */
};
static t_ScriptEngineType m_ScriptEngineList;

/*******************************************************************************
 * NAME:
 *    Scripting_RegisterScriptingEngine
 *
 * SYNOPSIS:
 *    PG_BOOL Scripting_RegisterScriptingEngine(const struct ScriptingEngineInfo *Info,
 *              unsigned int SizeOfScriptingEngineInfo);
 *
 * PARAMETERS:
 *    Info [I] -- Info about the new scripting plugin you are registering with
 *                the system.
 *    SizeOfScriptingEngineInfo [I] -- The sizeof the 'Info' structure.
 *
 * FUNCTION:
 *    This function registers a new scripting plugin with the system.  This
 *    should be called in your Register function.
 *
 * RETURNS:
 *    true -- The plugin was successfully registered
 *    false -- There was an error registering the plugin and it hasn't been.
 *
 * CALLBACKS:
 *==============================================================================
 * NAME:
 *    Init
 *
 * SYNOPSIS:
 *    PG_BOOL Init(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    
 *
 * RETURNS:
 *    
 *
 * SEE ALSO:
 *    
 *==============================================================================
 * NAME:
 *    AllocateContext
 *
 * SYNOPSIS:
 *    t_ScriptingEngineContextType *AllocateContext(t_ScriptingEngineInstType *Inst);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                You will need to keep this to send to scripting functions
 *                you want to call.
 *
 * FUNCTION:
 *    
 *
 * RETURNS:
 *    
 *
 * NOTES:
 *    This runs in the thread context.
 *
 * SEE ALSO:
 *    
 *==============================================================================
 * NAME:
 *    FreeContext
 *
 * SYNOPSIS:
 *    void FreeContext(t_ScriptingEngineContextType *Context);
 *
 * PARAMETERS:
 *    Context [I] -- The script to work on
 *
 * FUNCTION:
 *    
 *
 * RETURNS:
 *    
 *
 * NOTES:
 *    This runs in the thread context.
 *
 * SEE ALSO:
 *    
 *==============================================================================
 * NAME:
 *    GetLastError
 *
 * SYNOPSIS:
 *    const char *GetLastError(t_ScriptingEngineContextType *Context);
 *
 * PARAMETERS:
 *    Context [I] -- The script to work on
 *    
 *
 * FUNCTION:
 *    
 *
 * RETURNS:
 *    
 *
 * NOTES:
 *    This runs in the thread context.
 *
 * SEE ALSO:
 *    
 *==============================================================================
 * NAME:
 *    LoadScriptFromString
 *
 * SYNOPSIS:
 *    PG_BOOL LoadScriptFromString(t_ScriptingEngineContextType *Context,
 *              const char *Str);
 *
 * PARAMETERS:
 *    Context [I] -- The script to work on
 *    Str [I] -- 
 *
 * FUNCTION:
 *    
 *
 * RETURNS:
 *    
 *
 * NOTES:
 *    This runs in the thread context.
 *
 *
 * SEE ALSO:
 *    
 *==============================================================================
 * NAME:
 *    RunLoadedScript
 *
 * SYNOPSIS:
 *    PG_BOOL RunLoadedScript(t_ScriptingEngineContextType *Context);
 *
 * PARAMETERS:
 *    Context [I] -- The script to work on
 *    
 *
 * FUNCTION:
 *    
 *
 * RETURNS:
 *    
 *
 * NOTES:
 *    This runs in the thread context.
 *
 * SEE ALSO:
 *    
 *==============================================================================
 * NAME:
 *    AbortScript
 *
 * SYNOPSIS:
 *    void AbortScript(t_ScriptingEngineContextType *Context);
 *
 * PARAMETERS:
 *    Context [I] -- The script to work on
 *
 * FUNCTION:
 *    This function tells the scripting engine to abort the currently running
 *    script.  If there isn't one then this does nothing.  If it hasn't
 *    actually started the script yet, then the script will need to be aborted
 *    as soon as it starts (or the start up will need to be abandoned)
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    WARNING!!! This runs in the MAIN thread context.
 *
 * SEE ALSO:
 *    
 *==============================================================================
 * NAME:
 *    NewKeyPressDetected
 *
 * SYNOPSIS:
 *    void NewKeyPressDetected(t_ScriptingEngineContextType *Context);
 *
 * PARAMETERS:
 *    Context [I] -- The script to work on
 *
 * FUNCTION:
 *    This function is called to inform the script engine that a new key press
 *    has been queued.  This can be NULL if you don't care.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    WARNING!!! This runs in the MAIN thread context so you can't access
 *    your context var's without some kind of mutex.
 *
 * SEE ALSO:
 *    
 *==============================================================================
 * SEE ALSO:
 *    
 ******************************************************************************/
PG_BOOL Scripting_RegisterScriptingEngine(const struct ScriptingEngineInfo *Info,
        unsigned int SizeOfScriptingEngineInfo)
{
    struct ScriptEngine *NewEntry;
    struct ScriptingEngineInfo FullCopyOfInfo;
    unsigned int UseSize;
    bool MutexAllocated;
    string Error;

    MutexAllocated=false;
    NewEntry=NULL;
    try
    {
        UseSize=SizeOfScriptingEngineInfo;
        if(UseSize>sizeof(struct ScriptingEngineInfo))
            UseSize=sizeof(struct ScriptingEngineInfo);
        memset((void *)&FullCopyOfInfo,0x00,sizeof(FullCopyOfInfo));
        memcpy((void *)&FullCopyOfInfo,(void *)Info,UseSize);

        /* Make sure we have all of the needed API */
        if(Info->API->AllocateContext==NULL ||
                Info->API->FreeContext==NULL ||
                Info->API->GetLastError==NULL ||
                Info->API->LoadScriptFromString==NULL ||
                Info->API->AbortScript==NULL ||
                Info->API->RunLoadedScript==NULL)
        {
            Error="Failed to init the scripting engine:\n";
            Error+=FullCopyOfInfo.DisplayName;
            UIAsk("Error",Error.c_str(),e_AskBox_Error,e_AskBttns_Ok);
            throw(0);
        }

        m_ScriptEngineList.emplace_back();
        NewEntry=&m_ScriptEngineList.back();

        NewEntry->InitBeenCalled=false;

        NewEntry->ID=Info->IDStr;
        NewEntry->DisplayName=Info->DisplayName;
        NewEntry->LanguageGroup=Info->LanguageGroup;
        NewEntry->LanguageVariant=Info->LanguageVariant;
        NewEntry->FileExt=Info->FileExt;

        NewEntry->UseCount=0;
        if(mtx_init(&NewEntry->UseCountMutex,mtx_plain)!=thrd_success)
            throw(0);
        MutexAllocated=true;

        UseSize=Info->ScriptingEngineAPISize;
        if(UseSize>sizeof(struct ScriptingEngineAPI))
            UseSize=sizeof(struct ScriptingEngineAPI);
        memset((void *)&NewEntry->API,0x00,sizeof(struct ScriptingEngineAPI));
        memcpy((void *)&NewEntry->API,(void *)Info->API,UseSize);
    }
    catch(...)
    {
        if(MutexAllocated)
        {
            mtx_destroy(&NewEntry->UseCountMutex);
        }
        if(NewEntry!=NULL)
        {
            /* Remove this from the list of script engines */
            m_ScriptEngineList.pop_back();
        }
        return false;
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    Scripting_KeyPress
 *
 * SYNOPSIS:
 *    void Scripting_KeyPress(struct ScriptHandle *Handle,class Connection *Con,
 *              uint8_t Mods,e_UIKeys Key,const uint8_t *TextPtr,
 *              unsigned int TextLen);
 *
 * PARAMETERS:
 *    Handle [I] -- The handle of the script
 *    Con [I] -- The connection that this key was pressed into
 *    Mods [I] -- What keys where held when this key event happended
 *    Key [I] -- What "special" key was pressed (things like arrow keys)
 *    TextPtr [I] -- A string with the key in it (UTF8).  If the key can be
 *                   converted to a char this will have it.
 *    TextLen [I] -- The number of bytes in 'TextPtr'
 *
 * FUNCTION:
 *    This function is called when a key is pressed from a connection.  It
 *    queues the key press for any scripts that are running.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_KeyPress(struct ScriptHandle *Handle,class Connection *Con,
        uint8_t Mods,e_UIKeys Key,const uint8_t *TextPtr,unsigned int TextLen)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Handle;
    struct PluginKeyPress NewKeyEntry;

    /* Queue the key press for this script */
    NewKeyEntry.Mod=Mods;
    NewKeyEntry.ExtendedKey=Key;
    strncpy((char *)&NewKeyEntry.KeyChar,(char *)TextPtr,
            sizeof(NewKeyEntry.KeyChar)-1);
    NewKeyEntry.KeyChar[sizeof(NewKeyEntry.KeyChar)-1]=0;

    if(SEInstance->KeyboardQueue.size()<MAX_KEYPRESS_QUEUE_SIZE)
        SEInstance->KeyboardQueue.push(NewKeyEntry);

    /* Now tell the script engine that there's a key */
    if(SEInstance->ScriptEngine->API.NewKeyPressDetected!=NULL)
        SEInstance->ScriptEngine->API.NewKeyPressDetected(SEInstance->Context);
}

/*******************************************************************************
 * NAME:
 *    Scripting_RecvBytes
 *
 * SYNOPSIS:
 *    void Scripting_RecvBytes(struct ScriptHandle *Handle,
 *              class Connection *Con,const uint8_t *inbuff,unsigned int bytes);
 *
 * PARAMETERS:
 *    Handle [I] -- The handle of the script
 *    Con [I] -- The connection that these bytes came into
 *    inbuff [I] -- A buffer with the bytes that came in are placed
 *    bytes [I] -- The number of bytes in the 'inbuff' buffer.
 *
 * FUNCTION:
 *    This function is called when bytes come into a connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_RecvBytes(struct ScriptHandle *Handle,class Connection *Con,
        const uint8_t *inbuff,unsigned int bytes)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Handle;
    struct ScriptInComingQueue *q;
    unsigned int BytesLeft;
    unsigned int NextHead;
    unsigned int NewQueueSize;
    uint8_t *NewQueue;
    const uint8_t *CopyPos;

    q=&SEInstance->InComingQueue;

    /* Queue these bytes */
    CopyPos=inbuff;
    BytesLeft=bytes;
    while(BytesLeft>0)
    {
        NextHead=q->Head;
        NextHead++;
        if(NextHead>=q->QueueSize)
            NextHead=0;
        if(NextHead==q->Tail)
        {
            /* We are full, see the script engine is trying to read the queue */
            if(GetElapsedTime_ms()-(q->LastPoll4BytesTime)>=
                    INCOMING_QUEUE_POLL_RATE_TIMEOUT ||
                    q->QueueSize>=INCOMING_QUEUE_MAX_SIZE)
            {
                /* Ok, the script engine hasn't tried to read the data fast
                   enough or we have hit max size, abort */
                return;
            }
            NewQueueSize=q->QueueSize+INCOMING_QUEUE_GROW_SIZE;
            NewQueue=(uint8_t *)realloc(q->Queue,NewQueueSize);
            if(NewQueue==NULL)
            {
                /* Out of memory */
                return;
            }
            q->Queue=NewQueue;

            if(q->Tail<q->Head)
            {
                /* We are just expanding, nothing to do */
            }
            else
            {
                /* We need to move the tail to the end */
                memcpy(&q->Queue[q->Tail+INCOMING_QUEUE_GROW_SIZE],
                        &q->Queue[q->Tail],q->QueueSize-(q->Tail));

                q->Tail+=INCOMING_QUEUE_GROW_SIZE;
            }

            /* Fix the next head (in case it had wrapped) */
            if(NextHead==0)
                NextHead=q->Head+1;

            q->QueueSize=NewQueueSize;
        }
        q->Queue[q->Head]=*CopyPos++;
        q->Head=NextHead;
        BytesLeft--;
    }

//    /* Now tell the script engine that there's a key */
//    if(SEInstance->ScriptEngine->API.NewKeyPressDetected!=NULL)
//        SEInstance->ScriptEngine->API.NewKeyPressDetected(SEInstance->Context);
}

/*******************************************************************************
 * NAME:
 *    Scripting_WriteScreen
 *
 * SYNOPSIS:
 *    static void Scripting_WriteScreen(t_ScriptingEngineInstType *Inst,
 *          const uint8_t *Str,uint32_t Len);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *    Str [I] -- The string to add (UTF8)
 *    Len [I] -- The number of chars in 'Str'.  Note chars are utf8 chars not
 *               bytes.  So if you have "0xE2 0x98 0x82 0x33" in 'str' that is
 *               only 2 chars but 4 bytes, so the correct value for 'Len' is 2.
 *
 * FUNCTION:
 *    This function writes to the screen only (it doesn't not end up in the hex
 *    displays or any other place than the screen).  This only works when
 *    a text display is active (it is ignored with binary connections).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Scripting_ReadKeyboard()
 ******************************************************************************/
static void Scripting_WriteScreen(t_ScriptingEngineInstType *Inst,
        const uint8_t *Str,uint32_t Len)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Inst;
    struct RPCWriteScreenData RPC;

    RPC.SEInstance=SEInstance;
    RPC.Str=Str;
    RPC.Len=Len;

    DoGenericRPC2MainThread(Scripting_WriteScreenCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_ReadKeyboard
 *
 * SYNOPSIS:
 *    unsigned int Scripting_ReadKeyboard(t_ScriptingEngineInstType *Inst,
 *              struct PluginKeyPress *KeyPresses,uint32_t MaxCount);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *    KeyPresses [O] -- Where to store the key presses that are read
 *    MaxCount [I] -- The number of entries in 'KeyPresses'
 *
 * FUNCTION:
 *    This function reads from the local keyboard queue.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Scripting_WriteScreen()
 ******************************************************************************/
unsigned int Scripting_ReadKeyboard(t_ScriptingEngineInstType *Inst,
        struct PluginKeyPress *KeyPresses,uint32_t MaxCount)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Inst;
    struct RPCReadKeyboardData RPC;

    RPC.SEInstance=SEInstance;
    RPC.KeyPresses=KeyPresses;
    RPC.MaxCount=MaxCount;

    DoGenericRPC2MainThread(Scripting_ReadKeyboardCB,(void *)&RPC);

    return RPC.RetValue;
}

/*******************************************************************************
 * NAME:
 *    Scripting_WriteCom
 *
 * SYNOPSIS:
 *    static void Scripting_WriteCom(t_ScriptingEngineInstType *Inst,
 *          const uint8_t *Str,uint32_t Len);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *    Str [I] -- The string to add (UTF8)
 *    Len [I] -- The number of chars in 'Str'.  Note chars are utf8 chars not
 *               bytes.  So if you have "0xE2 0x98 0x82 0x33" in 'str' that is
 *               only 2 chars but 4 bytes, so the correct value for 'Len' is 2.
 *
 * FUNCTION:
 *    This function sends bytes out the connection, the bytes are not echoed
 *    to the screen.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Scripting_WriteScreen()
 ******************************************************************************/
static void Scripting_WriteCom(t_ScriptingEngineInstType *Inst,
        const uint8_t *Str,uint32_t Len)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Inst;
    struct RPCWriteComData RPC;

    RPC.SEInstance=SEInstance;
    RPC.Str=Str;
    RPC.Len=Len;

    DoGenericRPC2MainThread(Scripting_WriteComCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_ReadCom
 *
 * SYNOPSIS:
 *    unsigned int Scripting_ReadCom(t_ScriptingEngineInstType *Inst,
 *              uint8_t *Buffer,uint32_t BufferSize);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *    Buffer [O] -- The buffer to fill with bytes that where read from the com
 *    BufferSize [I] -- The max number of bytes 'Buffer' can hold.
 *
 * FUNCTION:
 *    This function reads bytes out the incoming connection queue.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Scripting_WriteCom()
 ******************************************************************************/
unsigned int Scripting_ReadCom(t_ScriptingEngineInstType *Inst,
        uint8_t *Buffer,uint32_t BufferSize)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Inst;
    struct RPCReadComData RPC;

    RPC.SEInstance=SEInstance;
    RPC.Buffer=Buffer;
    RPC.BufferSize=BufferSize;

    DoGenericRPC2MainThread(Scripting_ReadComCB,(void *)&RPC);

    return RPC.RetValue;
}

/*******************************************************************************
 * NAME:
 *    Scripting_GetSysColor
 *
 * SYNOPSIS:
 *    static uint32_t Scripting_GetSysColor(t_ScriptingEngineInstType *Inst,
 *              uint32_t SysColShade,uint32_t SysColor);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *    SysColShade [I] -- What shade of the color to get.  Supported values:
 *                          e_SysColShade_Normal -- The normal shade
 *                          e_SysColShade_Bright -- A bright version of this
 *                                                  color.
 *                          e_SysColShade_Dark -- A dark version of this color.
 *    SysColor [I] -- The color to get from the system.  Supported values:
 *                      e_SysCol_Black
 *                      e_SysCol_Red
 *                      e_SysCol_Green
 *                      e_SysCol_Yellow
 *                      e_SysCol_Blue
 *                      e_SysCol_Magenta
 *                      e_SysCol_Cyan
 *                      e_SysCol_White
 *
 * FUNCTION:
 *    This function gets a system color from the settings.
 *
 * RETURNS:
 *    The color value in the format 0xRRGGBB.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static uint32_t Scripting_GetSysColor(t_ScriptingEngineInstType *Inst,
        uint32_t SysColShade,uint32_t SysColor)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Inst;
    struct RPCGetSysColorData RPC;

    RPC.SEInstance=SEInstance;
    RPC.SysColShade=(e_SysColShadeType)SysColShade;
    RPC.SysColor=(e_SysColType)SysColor;

    DoGenericRPC2MainThread(Scripting_GetSysColorCB,(void *)&RPC);

    return RPC.RetColor;
}

/*******************************************************************************
 * NAME:
 *    Scripting_GetSysDefaultColor
 *
 * SYNOPSIS:
 *    uint32_t Scripting_GetSysDefaultColor(t_ScriptingEngineInstType *Inst,
 *              uint32_t DefaultColor);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *    DefaultColor [I] -- The color to get the default for.  Supported values:
 *                          e_DefaultColors_BG -- The background color
 *                          e_DefaultColors_FG -- The forground color
 *
 * FUNCTION:
 *    This function gets the default background or forground color.  These
 *    colors may not be in the colors available with DPS_GetSysColor()
 *
 * RETURNS:
 *    The color value in the format 0xRRGGBB.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
uint32_t Scripting_GetSysDefaultColor(t_ScriptingEngineInstType *Inst,
        uint32_t DefaultColor)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Inst;
    struct RPCGetSysDefaultColorData RPC;

    RPC.SEInstance=SEInstance;
    RPC.DefaultColor=(e_DefaultColorsType)DefaultColor;

    DoGenericRPC2MainThread(Scripting_GetSysDefaultColorCB,(void *)&RPC);

    return RPC.RetColor;
}

/*******************************************************************************
 * NAME:
 *    Scripting_SetStyle
 *
 * SYNOPSIS:
 *    void Scripting_SetStyle(t_ScriptingEngineInstType *Inst,
 *              struct StyleData *NewStyle);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *    NewStyle [I] -- The new style set to apply.
 *
 * FUNCTION:
 *    This function sets the current style.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_SetStyle(t_ScriptingEngineInstType *Inst,
        struct StyleData *NewStyle)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Inst;
    struct RPCSetStyleData RPC;

    RPC.SEInstance=SEInstance;
    RPC.Style=NewStyle;

    DoGenericRPC2MainThread(Scripting_SetStyleCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_GetStyle
 *
 * SYNOPSIS:
 *    void Scripting_GetStyle(t_ScriptingEngineInstType *Inst,
 *              struct StyleData *CurStyle);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *    CurStyle [O] -- The current style is copied here
 *
 * FUNCTION:
 *    This function gets the current style.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_GetStyle(t_ScriptingEngineInstType *Inst,
        struct StyleData *CurStyle)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Inst;
    struct RPCGetStyleData RPC;

    RPC.SEInstance=SEInstance;
    RPC.Style=CurStyle;

    DoGenericRPC2MainThread(Scripting_GetStyleCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_DefaultStyleDataStruct
 *
 * SYNOPSIS:
 *    void Scripting_DefaultStyleDataStruct(t_ScriptingEngineInstType *Inst,
 *              struct StyleData *Struct2Default);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *    Struct2Default [O] -- This is the struct to default
 *
 * FUNCTION:
 *    This function defaults a StyleData structure with the system defaults.
 *    You need to do this before you use structure because you might be using
 *    an older version of the structure and this will fill in good defaults
 *    for fields you do not know about.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_DefaultStyleDataStruct(t_ScriptingEngineInstType *Inst,
        struct StyleData *Struct2Default)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Inst;
    struct RPCDefaultStyleDataStructData RPC;

    RPC.SEInstance=SEInstance;
    RPC.Struct2Default=Struct2Default;

    DoGenericRPC2MainThread(Scripting_DefaultStyleDataStructCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_SetTitle
 *
 * SYNOPSIS:
 *    void Scripting_SetTitle(t_ScriptingEngineInstType *Inst,
 *              const char *Title);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *    Title [I] -- The title for the tab/window
 *
 * FUNCTION:
 *    This function changes the title of this connections title.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_SetTitle(t_ScriptingEngineInstType *Inst,const char *Title)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Inst;
    struct RPCSetTitleData RPC;

    RPC.SEInstance=SEInstance;
    RPC.Title=Title;

    DoGenericRPC2MainThread(Scripting_SetTitleCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_GetTitle
 *
 * SYNOPSIS:
 *    unsigned int Scripting_GetTitle(t_ScriptingEngineInstType *Inst,
 *              const char *Title,unsigned int MaxLen);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *    Title [O] -- A buffer to store the title in
 *    MaxLen [I] -- The max number of bytes that can be stored in 'Title'
 *
 * FUNCTION:
 *    This function changes the title of this connections title.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
unsigned int Scripting_GetTitle(t_ScriptingEngineInstType *Inst,
        char *Title,unsigned int MaxLen)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Inst;
    struct RPCGetTitleData RPC;

    RPC.SEInstance=SEInstance;
    RPC.Title=Title;
    RPC.MaxLen=MaxLen;

    DoGenericRPC2MainThread(Scripting_GetTitleCB,(void *)&RPC);

    return RPC.RetLen;
}

/*******************************************************************************
 * NAME:
 *    Scripting_DoClearArea
 *
 * SYNOPSIS:
 *    void Scripting_DoClearArea(t_ScriptingEngineInstType *Inst,uint32_t X1,
 *              uint32_t Y1,uint32_t X2,uint32_t Y2);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *    X1 [I] -- The left edge
 *    Y1 [I] -- The top edge
 *    X2 [I] -- The right edge +1
 *    Y2 [I] -- The bottom edge +1
 *
 * FUNCTION:
 *    This function clears a box on the screen.  The box may be only one line
 *    high or one row wide.
 *
 *    The line with Y1 to Y2-1 will be cleared.  The column X1 to X2-1 will
 *    be cleared.
 *
 *    So if you send in:
 *          DoClearArea(10,1,12,3);
 *    Then it will clear line 1, and 2 and column 10, and 11.
 *
 *           012345678901234567890
 *          0xxxxxxxxxxxxxxxxxxxxx
 *          1xxxxxxxxxx  xxxxxxxxx
 *          2xxxxxxxxxx  xxxxxxxxx
 *          3xxxxxxxxxxxxxxxxxxxxx
 *          4xxxxxxxxxxxxxxxxxxxxx
 *
 *    The background color will be taken from the current styling info.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_DoClearArea(t_ScriptingEngineInstType *Inst,uint32_t X1,
        uint32_t Y1,uint32_t X2,uint32_t Y2)
{
    struct RPCDoConFunctionData RPC;

    RPC.SEInstance=(struct ScriptEngineInstance *)Inst;
    RPC.Fn=e_ConFunc_ClearArea;
    RPC.Arg1=X1;
    RPC.Arg2=Y1;
    RPC.Arg3=X2;
    RPC.Arg4=Y2;
    RPC.Arg5=0;
    RPC.Arg6=0;

    DoGenericRPC2MainThread(Scripting_DoConFunctionCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_DoScrollArea
 *
 * SYNOPSIS:
 *    void Scripting_DoScrollArea(t_ScriptingEngineInstType *Inst,uint32_t X1,
 *              uint32_t Y1,uint32_t X2,uint32_t Y2,int32_t DeltaX,
 *              int32_t DeltaY);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *    X1 [I] -- The left edge
 *    Y1 [I] -- The top edge
 *    X2 [I] -- The right edge
 *    Y2 [I] -- The bottom edge
 *    DeltaX [I] -- The amount to scroll in the X direction
 *    DeltaY [I] -- The amount to scroll in the Y direction
 *
 * FUNCTION:
 *    This function scrolls an area of the screen.  Nothing is moved to the
 *    scroll back buffer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_DoScrollArea(t_ScriptingEngineInstType *Inst,uint32_t X1,
        uint32_t Y1,uint32_t X2,uint32_t Y2,int32_t DeltaX,int32_t DeltaY)
{
    struct RPCDoConFunctionData RPC;

    RPC.SEInstance=(struct ScriptEngineInstance *)Inst;
    RPC.Fn=e_ConFunc_ScrollArea;
    RPC.Arg1=X1;
    RPC.Arg2=Y1;
    RPC.Arg3=X2;
    RPC.Arg4=Y2;
    RPC.Arg5=DeltaX;
    RPC.Arg6=DeltaY;

    DoGenericRPC2MainThread(Scripting_DoConFunctionCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    DPS_DoNewLine
 *
 * SYNOPSIS:
 *    void DPS_DoNewLine(t_ScriptingEngineInstType *Inst);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *
 * FUNCTION:
 *    This function moves the cursor down one line.  When it gets to the bottom
 *    it will scroll the screen.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_DoReturn()
 ******************************************************************************/
void Scripting_DoNewLine(t_ScriptingEngineInstType *Inst)
{
    struct RPCDoConFunctionData RPC;

    RPC.SEInstance=(struct ScriptEngineInstance *)Inst;
    RPC.Fn=e_ConFunc_NewLine;
    RPC.Arg1=0;
    RPC.Arg2=0;
    RPC.Arg3=0;
    RPC.Arg4=0;
    RPC.Arg5=0;
    RPC.Arg6=0;

    DoGenericRPC2MainThread(Scripting_DoConFunctionCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_DoReturn
 *
 * SYNOPSIS:
 *    void Scripting_DoReturn(t_ScriptingEngineInstType *Inst);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *
 * FUNCTION:
 *    This function moves the cursor to the start of the current line.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_DoReturn(t_ScriptingEngineInstType *Inst)
{
    struct RPCDoConFunctionData RPC;

    RPC.SEInstance=(struct ScriptEngineInstance *)Inst;
    RPC.Fn=e_ConFunc_Return;
    RPC.Arg1=0;
    RPC.Arg2=0;
    RPC.Arg3=0;
    RPC.Arg4=0;
    RPC.Arg5=0;
    RPC.Arg6=0;

    DoGenericRPC2MainThread(Scripting_DoConFunctionCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_DoBackspace
 *
 * SYNOPSIS:
 *    void Scripting_DoBackspace(t_ScriptingEngineInstType *Inst);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *
 * FUNCTION:
 *    This function moves the cursor back one position.  When it hits the
 *    left edge it will move to the right edge up one line.
 *
 *    This acts on the backspace command, unlike DPS_SendBackspace() which
 *    just writes the backspace char.
 *
 *    Depending on the settings this may erase the char under the char.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_DoBackspace(t_ScriptingEngineInstType *Inst)
{
    struct RPCDoConFunctionData RPC;

    RPC.SEInstance=(struct ScriptEngineInstance *)Inst;
    RPC.Fn=e_ConFunc_Backspace;
    RPC.Arg1=0;
    RPC.Arg2=0;
    RPC.Arg3=0;
    RPC.Arg4=0;
    RPC.Arg5=0;
    RPC.Arg6=0;

    DoGenericRPC2MainThread(Scripting_DoConFunctionCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_DoClearScreen
 *
 * SYNOPSIS:
 *    void Scripting_DoClearScreen(t_ScriptingEngineInstType *Inst);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *
 * FUNCTION:
 *    This function clears the screen.  It will be filled with the current
 *    background color and spaces.
 *
 *    Depending on settings it may also insert a dividing line or some other
 *    action.
 *
 *    The cursor will also be moved to 0,0
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_DoClearScreen(t_ScriptingEngineInstType *Inst)
{
    struct RPCDoConFunctionData RPC;

    RPC.SEInstance=(struct ScriptEngineInstance *)Inst;
    RPC.Fn=e_ConFunc_ClearScreen;
    RPC.Arg1=0;
    RPC.Arg2=0;
    RPC.Arg3=0;
    RPC.Arg4=0;
    RPC.Arg5=0;
    RPC.Arg6=0;

    DoGenericRPC2MainThread(Scripting_DoConFunctionCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_DoTab
 *
 * SYNOPSIS:
 *    void Scripting_DoTab(t_ScriptingEngineInstType *Inst);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *
 * FUNCTION:
 *    This function moves to the next tab stop.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_DoTab(t_ScriptingEngineInstType *Inst)
{
    struct RPCDoConFunctionData RPC;

    RPC.SEInstance=(struct ScriptEngineInstance *)Inst;
    RPC.Fn=e_ConFunc_Tab;
    RPC.Arg1=0;
    RPC.Arg2=0;
    RPC.Arg3=0;
    RPC.Arg4=0;
    RPC.Arg5=0;
    RPC.Arg6=0;

    DoGenericRPC2MainThread(Scripting_DoConFunctionCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_DoPrevTab
 *
 * SYNOPSIS:
 *    void Scripting_DoPrevTab(t_ScriptingEngineInstType *Inst);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *
 * FUNCTION:
 *    This function moves to the previous tab stop.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_DoPrevTab(t_ScriptingEngineInstType *Inst)
{
    struct RPCDoConFunctionData RPC;

    RPC.SEInstance=(struct ScriptEngineInstance *)Inst;
    RPC.Fn=e_ConFunc_PrevTab;
    RPC.Arg1=0;
    RPC.Arg2=0;
    RPC.Arg3=0;
    RPC.Arg4=0;
    RPC.Arg5=0;
    RPC.Arg6=0;

    DoGenericRPC2MainThread(Scripting_DoConFunctionCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_DoSystemBell
 *
 * SYNOPSIS:
 *    void Scripting_DoSystemBell(t_ScriptingEngineInstType *Inst,int VisualOnly)
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *    VisualOnly [I] -- If this is true then we do not play a sound and only
 *                      do something on screen to "show" the bell.
 *
 * FUNCTION:
 *    This function rings the system bell.  The bell might be turned off in
 *    which case this does nothing.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_DoSystemBell(t_ScriptingEngineInstType *Inst,int VisualOnly)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Inst;
    struct RPCDoSystemBellData RPC;

    RPC.SEInstance=SEInstance;
    RPC.VisualOnly=VisualOnly;

    DoGenericRPC2MainThread(Scripting_DoSystemBellCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_DoClearScreenAndBackBuffer
 *
 * SYNOPSIS:
 *    void Scripting_DoClearScreenAndBackBuffer(t_ScriptingEngineInstType *Inst);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *
 * FUNCTION:
 *    This function clears the screen and the back buffer.  It will be filled
 *    with the current background color and spaces.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_DoClearScreenAndBackBuffer(t_ScriptingEngineInstType *Inst)
{
    struct RPCDoConFunctionData RPC;

    RPC.SEInstance=(struct ScriptEngineInstance *)Inst;
    RPC.Fn=e_ConFunc_ClearScreenAndBackBuffer;
    RPC.Arg1=0;
    RPC.Arg2=0;
    RPC.Arg3=0;
    RPC.Arg4=0;
    RPC.Arg5=0;
    RPC.Arg6=0;

    DoGenericRPC2MainThread(Scripting_DoConFunctionCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_GetCursorXY
 *
 * SYNOPSIS:
 *    void Scripting_GetCursorXY(t_ScriptingEngineInstType *Inst,
 *              int32_t *RetCursorX,int32_t *RetCursorY);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *    RetCursorX [O] -- The place to store the current X pos
 *    RetCursorY [O] -- The place to store the current Y pos
 *
 * FUNCTION:
 *    This function gets the current cursor X and Y pos from the display.
 *    This is relative to the screen (0,0 = top left).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_GetCursorXY(t_ScriptingEngineInstType *Inst,int32_t *RetCursorX,
        int32_t *RetCursorY)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Inst;
    struct RPCGetCursorXYData RPC;

    RPC.SEInstance=SEInstance;
    RPC.RetCursorX=RetCursorX;
    RPC.RetCursorY=RetCursorY;

    DoGenericRPC2MainThread(Scripting_GetCursorXYCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_SetCursorXY
 *
 * SYNOPSIS:
 *    void Scripting_SetCursorXY(t_ScriptingEngineInstType *Inst,
 *              int32_t *RetCursorX,int32_t *RetCursorY);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *    X [I] -- The new X pos.
 *    Y [I] -- The new Y pos
 *
 * FUNCTION:
 *    This function moves the cursor around on the Screen.  It can not be
 *    moved in to the history or off the edges.  It you try to move the
 *    cursor out side of the screen it will be clipped to the edge of the
 *    screen.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_SetCursorXY(t_ScriptingEngineInstType *Inst,uint32_t X,uint32_t Y)
{
    struct RPCDoConFunctionData RPC;

    RPC.SEInstance=(struct ScriptEngineInstance *)Inst;
    RPC.Fn=e_ConFunc_MoveCursor;
    RPC.Arg1=X;
    RPC.Arg2=Y;
    RPC.Arg3=0;
    RPC.Arg4=0;
    RPC.Arg5=0;
    RPC.Arg6=0;

    DoGenericRPC2MainThread(Scripting_DoConFunctionCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_GetScreenSize
 *
 * SYNOPSIS:
 *    void Scripting_GetScreenSize(t_ScriptingEngineInstType *Inst,
 *              int32_t *RetRows,int32_t *RetColumns);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *    RetCursorX [O] -- The place to store the current X pos
 *    RetCursorY [O] -- The place to store the current Y pos
 *
 * FUNCTION:
 *    This function gets the current size of the screen in chars.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_GetScreenSize(t_ScriptingEngineInstType *Inst,int32_t *RetRows,
        int32_t *RetColumns)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Inst;
    struct RPCGetScreenSizeData RPC;

    RPC.SEInstance=SEInstance;
    RPC.RetWidth=RetColumns;
    RPC.RetHeight=RetRows;

    DoGenericRPC2MainThread(Scripting_GetScreenSizeCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_SendBackspace
 *
 * SYNOPSIS:
 *    void Scripting_SendBackspace(t_ScriptingEngineInstType *Inst);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *
 * FUNCTION:
 *    This function moves the cursor back one position.  When it hits the
 *    left edge it will move to the right edge up one line.
 *
 *    This acts on the backspace command, unlike DPS_SendBackspace() which
 *    just writes the backspace char.
 *
 *    Depending on the settings this may erase the char under the char.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void Scripting_SendBackspace(t_ScriptingEngineInstType *Inst)
{
    struct RPCDoConFunctionData RPC;

    RPC.SEInstance=(struct ScriptEngineInstance *)Inst;
    RPC.Fn=e_ConFunc_SendBackspace;
    RPC.Arg1=0;
    RPC.Arg2=0;
    RPC.Arg3=0;
    RPC.Arg4=0;
    RPC.Arg5=0;
    RPC.Arg6=0;

    DoGenericRPC2MainThread(Scripting_DoConFunctionCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_SendEnter
 *
 * SYNOPSIS:
 *    void Scripting_SendEnter(t_ScriptingEngineInstType *Inst);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *
 * FUNCTION:
 *    This function send the enter char.  What the enter char is
 *    depends on what the user has selected.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void Scripting_SendEnter(t_ScriptingEngineInstType *Inst)
{
    struct RPCDoConFunctionData RPC;

    RPC.SEInstance=(struct ScriptEngineInstance *)Inst;
    RPC.Fn=e_ConFunc_SendEnter;
    RPC.Arg1=0;
    RPC.Arg2=0;
    RPC.Arg3=0;
    RPC.Arg4=0;
    RPC.Arg5=0;
    RPC.Arg6=0;

    DoGenericRPC2MainThread(Scripting_DoConFunctionCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_DisableKeyboardSend
 *
 * SYNOPSIS:
 *    static void Scripting_DisableKeyboardSend(t_ScriptingEngineInstType *Inst,
 *              PG_BOOL Enabled);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *    Enabled [I] -- is the keyboard send enabled or disabled.
 *
 * FUNCTION:
 *    This function enables/disables key presses from the keyboard being sent
 *    out the connection.  Key presses are still sent to the script.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void Scripting_DisableKeyboardSend(t_ScriptingEngineInstType *Inst,
        PG_BOOL Enabled)
{
    struct RPCDisableKeyboardSendData RPC;

    RPC.SEInstance=(struct ScriptEngineInstance *)Inst;
    RPC.Enabled=Enabled;

    DoGenericRPC2MainThread(Scripting_DisableKeyboardSendCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_DisableScreenDisplay
 *
 * SYNOPSIS:
 *    static void Scripting_DisableScreenDisplay(t_ScriptingEngineInstType *Inst,
 *              PG_BOOL Enabled);
 *
 * PARAMETERS:
 *    Inst [I] -- The scripting instance that this script is being run with.
 *                This was passed in when the context was allocated.
 *    Enabled [I] -- is drawing incoming bytes to the screen enabled or
 *                   disabled.
 *
 * FUNCTION:
 *    This function enables/disables the system echoing bytes that come in the
 *    connection to the screen.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void Scripting_DisableScreenDisplay(t_ScriptingEngineInstType *Inst,
        PG_BOOL Enabled)
{
    struct RPCDisableScreenDisplayData RPC;

    RPC.SEInstance=(struct ScriptEngineInstance *)Inst;
    RPC.Enabled=Enabled;

    DoGenericRPC2MainThread(Scripting_DisableScreenDisplayCB,(void *)&RPC);
}

/*******************************************************************************
 * NAME:
 *    Scripting_Init
 *
 * SYNOPSIS:
 *    void Scripting_Init(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function init's the scripting system it's self.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_Init(void)
{
}

/*******************************************************************************
 * NAME:
 *    Scripting_InitPlugins
 *
 * SYNOPSIS:
 *    void Scripting_InitPlugins(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function handles init'ing of stuff related to the plugin's.  This
 *    can't be done in normal init because the plugin's haven't been
 *    registered yet.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_InitPlugins(void)
{
    i_ScriptEngineType i;
    string Error;

    /* Call all the init's that haven't been called yet */
    for(i=m_ScriptEngineList.begin();i!=m_ScriptEngineList.end();)
    {
        if(!i->InitBeenCalled)
        {
            i->InitBeenCalled=true;
            if(i->API.Init!=NULL)
            {
                if(!i->API.Init())
                {
                    /* Tell user we failed to init this scripting engine */
                    Error="Failed to init the scripting engine:\n";
                    Error+=i->DisplayName;
                    UIAsk("Error",Error.c_str(),e_AskBox_Error,e_AskBttns_Ok);
                    /* Ok, we couldn't init this plugin, remove it from the
                       list */
                    m_ScriptEngineList.erase(i);
                    i=m_ScriptEngineList.begin();
                    continue;
                }
            }
        }
        i++;
    }
}

/*******************************************************************************
 * NAME:
 *    Scripting_CheckifPluginCanUninstall
 *
 * SYNOPSIS:
 *    bool Scripting_CheckifPluginCanUninstall(const char *PluginIDStr);
 *
 * PARAMETERS:
 *    PluginIDStr [I] -- The ID string for the plugin to check
 *
 * FUNCTION:
 *    This function checks to see if a script engine can be uninstalled
 *    (checks if it has a script running).
 *
 * RETURNS:
 *    true -- Ok to uninstall
 *    false -- Can't uninstall this plugin.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Scripting_CheckifPluginCanUninstall(const char *PluginIDStr)
{
    i_ScriptEngineType se;
    int UseCountCopy;

    UseCountCopy=0;
    for(se=m_ScriptEngineList.begin();se!=m_ScriptEngineList.end();se++)
        if(se->ID==PluginIDStr)
            break;
    if(se!=m_ScriptEngineList.end())
    {
        /* Make sure we don't have any scripts running */
        mtx_lock(&se->UseCountMutex);
        UseCountCopy=se->UseCount;
        mtx_unlock(&se->UseCountMutex);
    }
    if(UseCountCopy>0)
        return false;
    return true;
}

/*******************************************************************************
 * NAME:
 *    Scripting_InformOfNewPluginInstalled
 *
 * SYNOPSIS:
 *    void Scripting_InformOfNewPluginInstalled(const char *PluginIDStr);
 *
 * PARAMETERS:
 *    PluginIDStr [I] -- The ID string for the plugin that was installed
 *
 * FUNCTION:
 *    This function is called any time a new plugin is installed.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    This is not called at started when plugin are loaded, just when a new
 *    plugin is installed.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_InformOfNewPluginInstalled(const char *PluginIDStr)
{
    /* Ok the new plugin will have it's 'InitBeenCalled' set to false so
       we can call Scripting_InitPlugins() again to just call the new plugins
       init() */
    Scripting_InitPlugins();
}

/*******************************************************************************
 * NAME:
 *    Scripting_InformOfPluginUninstalled
 *
 * SYNOPSIS:
 *    void Scripting_InformOfPluginUninstalled(const char *PluginIDStr);
 *
 * PARAMETERS:
 *    PluginIDStr [I] -- The ID string for the plugin that was removed
 *
 * FUNCTION:
 *    This function is called when a plugin is removed from the system.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_InformOfPluginUninstalled(const char *PluginIDStr)
{
    i_ScriptEngineType se;
    int UseCountCopy;

    for(se=m_ScriptEngineList.begin();se!=m_ScriptEngineList.end();se++)
        if(se->ID==PluginIDStr)
            break;
    if(se!=m_ScriptEngineList.end())
    {
        /* Make sure we don't have any scripts running */
        mtx_lock(&se->UseCountMutex);
        UseCountCopy=se->UseCount;
        mtx_unlock(&se->UseCountMutex);
        if(UseCountCopy==0)
        {
            UnRegisterPluginWithSystem(PluginIDStr);
            mtx_destroy(&se->UseCountMutex);
            m_ScriptEngineList.erase(se);
        }
    }
}

/*******************************************************************************
 * NAME:
 *    Scripting_AppendListOfFileTypeFilters
 *
 * SYNOPSIS:
 *    void Scripting_AppendListOfFileTypeFilters(std::string &RetList);
 *
 * PARAMETERS:
 *    RetList [I/O] -- This is a string to append the list of available
 *                     scripting engines to.
 *
 * FUNCTION:
 *    This funciton adds a list of available scripting engines to a filter
 *    string for file requesters.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UI_LoadFileReq()
 ******************************************************************************/
void Scripting_AppendListOfFileTypeFilters(std::string &RetList)
{
    i_ScriptEngineType se;

    for(se=m_ScriptEngineList.begin();se!=m_ScriptEngineList.end();se++)
    {
        RetList+=se->DisplayName;
        RetList+="|";
        RetList+=se->FileExt;
        RetList+="\n";
    }
}

struct ScriptHandle *Scripting_LoadScript(const char *Filename)
{
    FILE *in;
    string Error;
    const char *Ext;
    char *FileContents;
    char *ScriptType;
    char *StartOfScript;
    unsigned long FileSize;
    struct ScriptHandle *RetValue;
    i_ScriptEngineType se;
    struct ScriptEngineInstance *NewSEInstance;
    bool MutexAllocated;

    FileContents=NULL;
    RetValue=NULL;
    in=NULL;
    NewSEInstance=NULL;
    MutexAllocated=false;
    try
    {
        /* Load the script in */
        in=fopen(Filename,"rb");
        if(in==NULL)
        {
            Error="Failed to open the script file:\n";
            Error+=Filename;
            throw(Error.c_str());
        }
        /* Read the whole file in */
        fseek(in,0,SEEK_END);
        FileSize=ftell(in);
        fseek(in,0,SEEK_SET);

        /* We limit the size of the script file */
        if(FileSize>MAX_SCRIPT_SIZE)
            throw("Script file too large");

        if(FileSize==0)
            throw("Script file empty");

        FileContents=(char *)malloc(FileSize+1);
        if(FileContents==NULL)
            throw("Failed to allocate needed memory to run script");

        if(fread(FileContents,FileSize,1,in)!=1)
            throw("Failed to load script file");
        FileContents[FileSize]=0;

        fclose(in);
        in=NULL;

        /* See if it starts with a #! */
        if(FileContents[0]=='#' && FileContents[1]=='!')
        {
            /* It's a #! figure out what language this script is for */
            ScriptType=FileContents;

            /* Find end of line */
            while(*ScriptType!='\n')
            {
                if(*ScriptType==0)
                    break;
                ScriptType++;
            }
            if(*ScriptType==0)
            {
                /* Missing end of line? */
                throw("Malformed script file");
            }
            StartOfScript=ScriptType+1;
            if(*StartOfScript=='\r')
                StartOfScript++;

            /* Strip off any spaces */
            while(ScriptType>FileContents &&
                    (*ScriptType==' ' || *ScriptType=='\n'))
            {
                ScriptType--;
            }
            *(ScriptType+1)=0;  // String it

            /* Ok, now we back up looking for a non alpha/num char */
            while(ScriptType>FileContents)
            {
                if(!isalnum(*ScriptType) && *ScriptType!='_')
                    break;
                ScriptType--;
            }
            ScriptType++;

            if(*ScriptType==0)
                throw("Malformed script file");

            /* Ok, we now have the script type, see if have an engine for this
               type. */
            for(se=m_ScriptEngineList.begin();se!=m_ScriptEngineList.end();se++)
                if(strcasecmp(se->ID.c_str(),ScriptType)==0)
                    break;
            if(se==m_ScriptEngineList.end())
            {
                Error="#! override found, but unknown script language:\n";
                Error+=ScriptType;
                throw(Error.c_str());
            }
        }
        else
        {
            /* Use the extention */
            Ext=&Filename[strlen(Filename)-1];
            while(Ext>Filename && *Ext!='.')
                Ext--;
            if(*Ext!='.')
            {
                throw("Filename does not have extention or #!.\n"
                        "Unknown script language");
            }
            Ext++;

            for(se=m_ScriptEngineList.begin();se!=m_ScriptEngineList.end();se++)
                if(strcasecmp(se->FileExt.c_str(),Ext)==0)
                    break;
            if(se==m_ScriptEngineList.end())
            {
                throw("Unknown script language (based on extention)");
            }
            StartOfScript=FileContents;
        }

        /* Allocate an instance for this new engine for this script */
        NewSEInstance=new struct ScriptEngineInstance;
        NewSEInstance->InComingQueue.Queue=NULL;

        /* Thread setup */
        NewSEInstance->Context=NULL;
        NewSEInstance->FileContents=FileContents;
        NewSEInstance->StartOfScript=StartOfScript;
        NewSEInstance->AbortedScript=false;

        /* Main Thread setup */
        NewSEInstance->InComingQueue.Head=0;
        NewSEInstance->InComingQueue.Tail=0;
        NewSEInstance->InComingQueue.QueueSize=INCOMING_QUEUE_GROW_SIZE;
        NewSEInstance->InComingQueue.LastPoll4BytesTime=0;
        NewSEInstance->InComingQueue.Queue=
                (uint8_t *)malloc(INCOMING_QUEUE_GROW_SIZE);
        if(NewSEInstance->InComingQueue.Queue==NULL)
            throw("Out of memory");

        /* Shared setup */
        NewSEInstance->ScriptEngine=&*se;
        if(mtx_init(&NewSEInstance->SharedMutex,mtx_plain)!=thrd_success)
            throw(0);
        MutexAllocated=true;
        NewSEInstance->MainThreadFreed=false;
        NewSEInstance->ThreadFreed=false;
        NewSEInstance->ThreadWaiting2Run=true; // Start in a paused state

        /* Start the thread for this instance and hand the instance over to
           it. */
        /* Up the UseCount, the thread will -- it */
        mtx_lock(&se->UseCountMutex);
        se->UseCount++;
        mtx_unlock(&se->UseCountMutex);

        if(thrd_create(&NewSEInstance->Thread,RunScriptThread,
                (void *)NewSEInstance))
        {
            /* Thread never started, so -- it our self */
            mtx_lock(&se->UseCountMutex);
            se->UseCount--;
            mtx_unlock(&se->UseCountMutex);

            throw("Failed to start scripting engine");
        }

        thrd_detach(NewSEInstance->Thread);

        RetValue=(struct ScriptHandle *)NewSEInstance;
    }
    catch(const char *Msg)
    {
        if(MutexAllocated)
            mtx_destroy(&NewSEInstance->SharedMutex);
        if(NewSEInstance!=NULL)
        {
            if(NewSEInstance->InComingQueue.Queue!=NULL)
                free(NewSEInstance->InComingQueue.Queue);
            delete NewSEInstance;
        }
        if(FileContents!=NULL)
            free(FileContents);
        if(in!=NULL)
            fclose(in);
        UIAsk("Error",Msg,e_AskBox_Error,e_AskBttns_Ok);
        RetValue=NULL;
    }
    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    Scripting_RunScript
 *
 * SYNOPSIS:
 *    void Scripting_RunScript(struct ScriptHandle *Handle);
 *
 * PARAMETERS:
 *    
 *
 * FUNCTION:
 *    
 *
 * RETURNS:
 *    
 *
 * NOTES:
 *    
 *
 * LIMITATIONS:
 *    
 *
 * EXAMPLE:
 *    
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_RunScript(struct ScriptHandle *Handle)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Handle;

    /* Thread should be waiting for this signal */
    SEInstance->ThreadWaiting2Run=false;
}

/*******************************************************************************
 * NAME:
 *    Scripting_SetConnectedWindow
 *
 * SYNOPSIS:
 *    void Scripting_SetConnectedWindow(struct ScriptHandle *Handle,
 *              class TheMainWindow *MW,class Connection *Con);
 *
 * PARAMETERS:
 *    
 *
 * FUNCTION:
 *    
 *
 * RETURNS:
 *    
 *
 * NOTES:
 *    
 *
 * LIMITATIONS:
 *    
 *
 * EXAMPLE:
 *    
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_SetConnectedWindow(struct ScriptHandle *Handle,
        class TheMainWindow *MW,class Connection *Con)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Handle;

    SEInstance->ConnectedCon=Con;
    SEInstance->ConnectedMW=MW;
}

/*******************************************************************************
 * NAME:
 *    
 *
 * SYNOPSIS:
 *    
 *
 * PARAMETERS:
 *    
 *
 * FUNCTION:
 *    
 *
 * RETURNS:
 *    
 *
 * NOTES:
 *    
 *
 * LIMITATIONS:
 *    
 *
 * EXAMPLE:
 *    
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_FreeScriptHandle(struct ScriptHandle *Handle)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Handle;
    bool FreeInstance;

    /* Mark the instance as freed */
    mtx_lock(&SEInstance->SharedMutex);
    FreeInstance=SEInstance->ThreadFreed;
    SEInstance->MainThreadFreed=true;
    mtx_unlock(&SEInstance->SharedMutex);

    /* See if main thread is also free (if so free the memory) */
    if(FreeInstance)
    {
        free(SEInstance->InComingQueue.Queue);
        delete SEInstance;
    }
}

/*******************************************************************************
 * NAME:
 *    Scripting_AbortRunningScript
 *
 * SYNOPSIS:
 *    void Scripting_AbortRunningScript(struct ScriptHandle *Handle);
 *
 * PARAMETERS:
 *    Handle [I] -- The script to abort.
 *
 * FUNCTION:
 *    
 *
 * RETURNS:
 *    
 *
 * NOTES:
 *    
 *
 * LIMITATIONS:
 *    
 *
 * EXAMPLE:
 *    
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Scripting_AbortRunningScript(struct ScriptHandle *Handle)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)Handle;

    /* Tell the script engine to abort */
    SEInstance->ScriptEngine->API.AbortScript(SEInstance->Context);
}

e_AskRetType Scripting_ThreadAsk(const char *Title,const char *Msg,
        e_AskBoxType BoxType,e_AskBttnsType Buttons)
{
    struct UI_RPCData RPCData;
    struct RPCAskData AskData;

    AskData.Title=Title;
    AskData.Msg=Msg;
    AskData.BoxType=BoxType;
    AskData.Buttons=Buttons;

    RPCData.MainThreadFunction=Scripting_MainThreadAskCB;
    RPCData.Data=(void *)&AskData;
    DoGenericRPC2MainThread(&RPCData);
    return (e_AskRetType)RPCData.RetValue;
}

int Scripting_MainThreadAskCB(struct UI_RPCData *RPCData)
{
    struct RPCAskData *AskData;

    AskData=(struct RPCAskData *)RPCData->Data;

    return UIAsk(AskData->Title,AskData->Msg,AskData->BoxType,AskData->Buttons);
}

/*******************************************************************************
 * NAME:
 *    Scripting_MainThreadScriptDoneCB
 *
 * SYNOPSIS:
 *    int Scripting_MainThreadScriptDoneCB(struct UI_RPCData *RPCData);
 *
 * PARAMETERS:
 *    RPCData [I] -- The Remote proc callback data from the thread.
 *
 * FUNCTION:
 *    This function is called from the thread to the main thread (runs in
 *    the main thread context) and informs the scripting system that a
 *    thread is quiting.
 *
 * RETURNS:
 *    0
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int Scripting_MainThreadScriptDoneCB(struct UI_RPCData *RPCData)
{
    struct RPCScriptDoneData *ScriptDoneData;
    struct ScriptEngineInstance *SEInstance;

    ScriptDoneData=(struct RPCScriptDoneData *)RPCData->Data;
    SEInstance=ScriptDoneData->SEInstance;

    /* If we have a connection tell it we are done, if not check the
       mainwindow and tell it */
    if(SEInstance->ConnectedCon!=NULL)
    {
        SEInstance->ConnectedCon->
                InformOfScriptDone((struct ScriptHandle *)SEInstance);
    }
    else if(SEInstance->ConnectedMW!=NULL)
    {
        /* Ok, we have a main window, tell it the script has finished */
        SEInstance->ConnectedMW->
                InformOf_ScriptDone((struct ScriptHandle *)SEInstance);
    }

    /* We are done with the script handle */
    Scripting_FreeScriptHandle((struct ScriptHandle *)SEInstance);

    return 0;
}

/*******************************************************************************
 * NAME:
 *    Scripting_WriteScreenCB
 *
 * SYNOPSIS:
 *    int Scripting_WriteScreenCB(struct UI_RPCData *RPCData);
 *
 * PARAMETERS:
 *    RPCData [I] -- The Remote proc callback data from the thread.
 *
 * FUNCTION:
 *    This function is called from the thread to the main thread from the
 *    Scripting_WriteScreen() function.  It does it's function.
 *
 * RETURNS:
 *    0
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int Scripting_WriteScreenCB(struct UI_RPCData *RPCData)
{
    struct RPCWriteScreenData *Data=(struct RPCWriteScreenData *)RPCData->Data;

    if(Data->SEInstance->ConnectedCon!=NULL)
    {
        if(!Data->SEInstance->ConnectedCon->IsConnectionBinary())
        {
            Data->SEInstance->ConnectedCon->SuppressFrozenStream(true);
            Data->SEInstance->ConnectedCon->InsertString(Data->Str,Data->Len);
            Data->SEInstance->ConnectedCon->SuppressFrozenStream(false);
        }
    }

    return 0;
}

/*******************************************************************************
 * NAME:
 *    Scripting_ReadKeyboardCB
 *
 * SYNOPSIS:
 *    int Scripting_ReadKeyboardCB(struct UI_RPCData *RPCData);
 *
 * PARAMETERS:
 *    RPCData [I] -- The Remote proc callback data from the thread.
 *
 * FUNCTION:
 *    This function is called from the thread to the main thread from the
 *    Scripting_ReadKeyboard() function.  It does it's function.
 *
 * RETURNS:
 *    0
 *
 * SEE ALSO:
 *    NewKeyPressDetected()
 ******************************************************************************/
int Scripting_ReadKeyboardCB(struct UI_RPCData *RPCData)
{
    struct RPCReadKeyboardData *Data=(struct RPCReadKeyboardData *)RPCData->Data;
    struct PluginKeyPress *Entry;
    unsigned int r;

    Data->RetValue=0;
    if(Data->SEInstance->ConnectedCon!=NULL)
    {
        if(!Data->SEInstance->ConnectedCon->IsConnectionBinary())
        {
            for(r=0;r<Data->MaxCount &&
                    !Data->SEInstance->KeyboardQueue.empty();r++)
            {
                Entry=&Data->SEInstance->KeyboardQueue.front();
                memcpy(&Data->KeyPresses[r],Entry,
                        sizeof(struct PluginKeyPress));
                Data->SEInstance->KeyboardQueue.pop();

                Data->RetValue++;
            }
        }
    }

    return 0;
}

/*******************************************************************************
 * NAME:
 *    Scripting_WriteComCB
 *
 * SYNOPSIS:
 *    int Scripting_WriteComCB(struct UI_RPCData *RPCData);
 *
 * PARAMETERS:
 *    RPCData [I] -- The Remote proc callback data from the thread.
 *
 * FUNCTION:
 *    This function is called from the thread to the main thread from the
 *    Scripting_WriteCom() function.  It does it's function.
 *
 * RETURNS:
 *    0
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int Scripting_WriteComCB(struct UI_RPCData *RPCData)
{
    struct RPCWriteComData *Data=(struct RPCWriteComData *)RPCData->Data;

    if(Data->SEInstance->ConnectedCon!=NULL)
    {
        if(!Data->SEInstance->ConnectedCon->IsConnectionBinary())
        {
            Data->SEInstance->ConnectedCon->WriteData(Data->Str,Data->Len,
                    e_ConWriteSource_Script);
        }
    }

    return 0;
}

/*******************************************************************************
 * NAME:
 *    Scripting_ReadComCB
 *
 * SYNOPSIS:
 *    int Scripting_ReadComCB(struct UI_RPCData *RPCData);
 *
 * PARAMETERS:
 *    RPCData [I] -- The Remote proc callback data from the thread.
 *
 * FUNCTION:
 *    This function is called from the thread to the main thread from the
 *    Scripting_ReadCom() function.  It does it's function.
 *
 * RETURNS:
 *    0
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int Scripting_ReadComCB(struct UI_RPCData *RPCData)
{
    struct RPCReadComData *Data=(struct RPCReadComData *)RPCData->Data;
    struct ScriptInComingQueue *q;
    unsigned int BuffSize;
    unsigned int BytesRead;

    q=&Data->SEInstance->InComingQueue;
    BuffSize=Data->BufferSize;

    Data->RetValue=0;
    if(Data->SEInstance->ConnectedCon!=NULL)
    {
        /* Note that we tried to read something (this is needed because we don't
           grow the queue if the script engine is not reading the data) */
        q->LastPoll4BytesTime=GetElapsedTime_ms();

        BytesRead=0;
        while(BytesRead<BuffSize)
        {
            if(q->Tail==q->Head)
                break;
            Data->Buffer[BytesRead++]=q->Queue[q->Tail++];
            if(q->Tail>=q->QueueSize)
                q->Tail=0;
        }
        Data->RetValue=BytesRead;
    }

    return 0;
}

/*******************************************************************************
 * NAME:
 *    Scripting_GetSysColorCB
 *
 * SYNOPSIS:
 *    int Scripting_GetSysColorCB(struct UI_RPCData *RPCData);
 *
 * PARAMETERS:
 *    RPCData [I] -- The Remote proc callback data from the thread.
 *
 * FUNCTION:
 *    This function is called from the thread to the main thread from the
 *    Scripting_GetSysColor() function.  It does it's function.
 *
 * RETURNS:
 *    0
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int Scripting_GetSysColorCB(struct UI_RPCData *RPCData)
{
    struct RPCGetSysColorData *Data=(struct RPCGetSysColorData *)RPCData->Data;

    if(Data->SysColShade>=e_SysColShadeMAX)
    {
        Data->RetColor=0x000000;
        return 0;
    }
    if(Data->SysColor>=e_SysColMAX)
    {
        Data->RetColor=0x000000;
        return 0;
    }

    if(Data->SEInstance->ConnectedCon!=NULL)
    {
        if(!Data->SEInstance->ConnectedCon->UsingCustomSettings)
        {
            Data->RetColor=g_Settings.DefaultConSettings.
                    SysColors[Data->SysColShade][Data->SysColor];
        }
        else
        {
            Data->RetColor=Data->SEInstance->ConnectedCon->CustomSettings.
                    SysColors[Data->SysColShade][Data->SysColor];
        }
    }
    else
    {
        Data->RetColor=g_Settings.DefaultConSettings.
                SysColors[Data->SysColShade][Data->SysColor];
    }

    return 0;
}

/*******************************************************************************
 * NAME:
 *    Scripting_GetSysDefaultColorCB
 *
 * SYNOPSIS:
 *    int Scripting_GetSysDefaultColorCB(struct UI_RPCData *RPCData);
 *
 * PARAMETERS:
 *    RPCData [I] -- The Remote proc callback data from the thread.
 *
 * FUNCTION:
 *    This function is called from the thread to the main thread from the
 *    Scripting_GetSysDefaultColor() function.  It does it's function.
 *
 * RETURNS:
 *    0
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int Scripting_GetSysDefaultColorCB(struct UI_RPCData *RPCData)
{
    struct RPCGetSysDefaultColorData *Data=(struct RPCGetSysDefaultColorData *)RPCData->Data;

    if(Data->DefaultColor>=e_DefaultColorsMAX)
    {
        Data->RetColor=0x000000;
        return 0;
    }

    if(Data->SEInstance->ConnectedCon!=NULL)
    {
        if(!Data->SEInstance->ConnectedCon->UsingCustomSettings)
        {
            Data->RetColor=g_Settings.DefaultConSettings.
                    DefaultColors[Data->DefaultColor];
        }
        else
        {
            Data->RetColor=Data->SEInstance->ConnectedCon->CustomSettings.
                    DefaultColors[Data->DefaultColor];
        }
    }
    else
    {
        Data->RetColor=g_Settings.DefaultConSettings.
                DefaultColors[Data->DefaultColor];
    }

    return 0;
}

/*******************************************************************************
 * NAME:
 *    Scripting_SetStyleCB
 *
 * SYNOPSIS:
 *    int Scripting_SetStyleCB(struct UI_RPCData *RPCData);
 *
 * PARAMETERS:
 *    RPCData [I] -- The Remote proc callback data from the thread.
 *
 * FUNCTION:
 *    This function is called from the thread to the main thread from the
 *    Scripting_SetStyle() function.  It does it's function.
 *
 * RETURNS:
 *    0
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int Scripting_SetStyleCB(struct UI_RPCData *RPCData)
{
    struct RPCSetStyleData *Data=(struct RPCSetStyleData *)RPCData->Data;

    if(Data->SEInstance->ConnectedCon!=NULL)
    {
        Data->SEInstance->ConnectedCon->SetFGColor(Data->Style->FGColor);
        Data->SEInstance->ConnectedCon->SetBGColor(Data->Style->BGColor);
        Data->SEInstance->ConnectedCon->SetULineColor(Data->Style->ULineColor);
        Data->SEInstance->ConnectedCon->SetAttribs(Data->Style->Attribs);
    }

    return 0;
}

/*******************************************************************************
 * NAME:
 *    Scripting_GetStyleCB
 *
 * SYNOPSIS:
 *    int Scripting_GetStyleCB(struct UI_RPCData *RPCData);
 *
 * PARAMETERS:
 *    RPCData [I] -- The Remote proc callback data from the thread.
 *
 * FUNCTION:
 *    This function is called from the thread to the main thread from the
 *    Scripting_GetStyle() function.  It does it's function.
 *
 * RETURNS:
 *    0
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int Scripting_GetStyleCB(struct UI_RPCData *RPCData)
{
    struct RPCGetStyleData *Data=(struct RPCGetStyleData *)RPCData->Data;

    if(Data->SEInstance->ConnectedCon!=NULL)
    {
        Data->Style->FGColor=Data->SEInstance->ConnectedCon->GetFGColor();
        Data->Style->BGColor=Data->SEInstance->ConnectedCon->GetBGColor();
        Data->Style->ULineColor=Data->SEInstance->ConnectedCon->GetULineColor();
        Data->Style->Attribs=Data->SEInstance->ConnectedCon->GetAttribs();
    }

    return 0;
}

/*******************************************************************************
 * NAME:
 *    Scripting_DefaultStyleDataStructCB
 *
 * SYNOPSIS:
 *    int Scripting_DefaultStyleDataStructCB(struct UI_RPCData *RPCData);
 *
 * PARAMETERS:
 *    RPCData [I] -- The Remote proc callback data from the thread.
 *
 * FUNCTION:
 *    This function is called from the thread to the main thread from the
 *    Scripting_DefaultStyleDataStruct() function.  It does it's function.
 *
 * RETURNS:
 *    0
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int Scripting_DefaultStyleDataStructCB(struct UI_RPCData *RPCData)
{
    struct RPCDefaultStyleDataStructData *Data=(struct RPCDefaultStyleDataStructData *)RPCData->Data;

    DefaultStyleDataStructure(Data->Struct2Default);

    return 0;
}

/*******************************************************************************
 * NAME:
 *    Scripting_SetTitleCB
 *
 * SYNOPSIS:
 *    int Scripting_SetTitleCB(struct UI_RPCData *RPCData);
 *
 * PARAMETERS:
 *    RPCData [I] -- The Remote proc callback data from the thread.
 *
 * FUNCTION:
 *    This function is called from the thread to the main thread from the
 *    Scripting_SetTitle() function.  It does it's function.
 *
 * RETURNS:
 *    0
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int Scripting_SetTitleCB(struct UI_RPCData *RPCData)
{
    struct RPCSetTitleData *Data=(struct RPCSetTitleData *)RPCData->Data;

    if(Data->SEInstance->ConnectedCon!=NULL)
    {
        Data->SEInstance->ConnectedCon->SetDisplayName(Data->Title);
    }

    return 0;
}

/*******************************************************************************
 * NAME:
 *    Scripting_GetTitleCB
 *
 * SYNOPSIS:
 *    int Scripting_GetTitleCB(struct UI_RPCData *RPCData);
 *
 * PARAMETERS:
 *    RPCData [I] -- The Remote proc callback data from the thread.
 *
 * FUNCTION:
 *    This function is called from the thread to the main thread from the
 *    Scripting_GetTitle() function.  It does it's function.
 *
 * RETURNS:
 *    0
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int Scripting_GetTitleCB(struct UI_RPCData *RPCData)
{
    struct RPCGetTitleData *Data=(struct RPCGetTitleData *)RPCData->Data;
    std::string TitleStr;

    Data->RetLen=0;

    if(Data->MaxLen>0)
        Data->Title[0]=0;
    if(Data->SEInstance->ConnectedCon!=NULL)
    {
        Data->SEInstance->ConnectedCon->GetDisplayName(TitleStr);
        Data->RetLen=TitleStr.length();
        strncpy(Data->Title,TitleStr.c_str(),Data->MaxLen-1);
        Data->Title[Data->MaxLen-1]=0;
    }

    return 0;
}

/*******************************************************************************
 * NAME:
 *    Scripting_DoConFunctionCB
 *
 * SYNOPSIS:
 *    int Scripting_DoConFunctionCB(struct UI_RPCData *RPCData);
 *
 * PARAMETERS:
 *    RPCData [I] -- The Remote proc callback data from the thread.
 *
 * FUNCTION:
 *    This function is called from the thread to the main thread do to the
 *    connection DoFunction() function.
 *
 * RETURNS:
 *    0
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int Scripting_DoConFunctionCB(struct UI_RPCData *RPCData)
{
    struct RPCDoConFunctionData *Data=(struct RPCDoConFunctionData *)RPCData->Data;

    if(Data->SEInstance->ConnectedCon!=NULL)
    {
        Data->SEInstance->ConnectedCon->SuppressFrozenStream(true);
        Data->SEInstance->ConnectedCon->DoFunction(Data->Fn,
                Data->Arg1,Data->Arg2,Data->Arg3,Data->Arg4,Data->Arg5,
                Data->Arg6);
        Data->SEInstance->ConnectedCon->SuppressFrozenStream(false);
    }

    return 0;
}

/*******************************************************************************
 * NAME:
 *    Scripting_DoSystemBellCB
 *
 * SYNOPSIS:
 *    int Scripting_DoSystemBellCB(struct UI_RPCData *RPCData);
 *
 * PARAMETERS:
 *    RPCData [I] -- The Remote proc callback data from the thread.
 *
 * FUNCTION:
 *    This function is called from the thread to the main thread from the
 *    Scripting_DoSystemBell() function.  It does it's function.
 *
 * RETURNS:
 *    0
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int Scripting_DoSystemBellCB(struct UI_RPCData *RPCData)
{
    struct RPCDoSystemBellData *Data=(struct RPCDoSystemBellData *)RPCData->Data;

    if(Data->SEInstance->ConnectedCon!=NULL)
    {
        Data->SEInstance->ConnectedCon->SuppressFrozenStream(true);
        Data->SEInstance->ConnectedCon->DoBell(Data->VisualOnly);
        Data->SEInstance->ConnectedCon->SuppressFrozenStream(false);
    }

    return 0;
}

/*******************************************************************************
 * NAME:
 *    Scripting_GetCursorXYCB
 *
 * SYNOPSIS:
 *    int Scripting_GetCursorXYCB(struct UI_RPCData *RPCData);
 *
 * PARAMETERS:
 *    RPCData [I] -- The Remote proc callback data from the thread.
 *
 * FUNCTION:
 *    This function is called from the thread to the main thread from the
 *    Scripting_GetCursorXY() function.  It does it's function.
 *
 * RETURNS:
 *    0
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int Scripting_GetCursorXYCB(struct UI_RPCData *RPCData)
{
    struct RPCGetCursorXYData *Data=(struct RPCGetCursorXYData *)RPCData->Data;

    if(Data->SEInstance->ConnectedCon!=NULL)
    {
        Data->SEInstance->ConnectedCon->GetCursorXY(Data->RetCursorX,
                Data->RetCursorY);
    }

    return 0;
}

/*******************************************************************************
 * NAME:
 *    Scripting_GetScreenSizeCB
 *
 * SYNOPSIS:
 *    int Scripting_GetScreenSizeCB(struct UI_RPCData *RPCData);
 *
 * PARAMETERS:
 *    RPCData [I] -- The Remote proc callback data from the thread.
 *
 * FUNCTION:
 *    This function is called from the thread to the main thread from the
 *    Scripting_GetScreenSize() function.  It does it's function.
 *
 * RETURNS:
 *    0
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int Scripting_GetScreenSizeCB(struct UI_RPCData *RPCData)
{
    struct RPCGetScreenSizeData *Data=(struct RPCGetScreenSizeData *)RPCData->Data;

    if(Data->SEInstance->ConnectedCon!=NULL)
    {
        Data->SEInstance->ConnectedCon->GetScreenSize(Data->RetWidth,
                Data->RetHeight);
    }

    return 0;
}

/*******************************************************************************
 * NAME:
 *    Scripting_DisableKeyboardSendCB
 *
 * SYNOPSIS:
 *    int Scripting_DisableKeyboardSendCB(struct UI_RPCData *RPCData);
 *
 * PARAMETERS:
 *    RPCData [I] -- The Remote proc callback data from the thread.
 *
 * FUNCTION:
 *    This function is called from the thread to the main thread from the
 *    Scripting_DisableKeyboardSend() function.  It does it's function.
 *
 * RETURNS:
 *    0
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int Scripting_DisableKeyboardSendCB(struct UI_RPCData *RPCData)
{
    struct RPCDisableKeyboardSendData *Data=(struct RPCDisableKeyboardSendData *)RPCData->Data;

    if(Data->SEInstance->ConnectedCon!=NULL)
    {
        Data->SEInstance->ConnectedCon->DisableTxKeyboard(Data->Enabled);
    }

    return 0;
}

/*******************************************************************************
 * NAME:
 *    Scripting_DisableScreenDisplayCB
 *
 * SYNOPSIS:
 *    int Scripting_DisableScreenDisplayCB(struct UI_RPCData *RPCData);
 *
 * PARAMETERS:
 *    RPCData [I] -- The Remote proc callback data from the thread.
 *
 * FUNCTION:
 *    This function is called from the thread to the main thread from the
 *    Scripting_DisableScreenDisplay() function.  It does it's function.
 *
 * RETURNS:
 *    0
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int Scripting_DisableScreenDisplayCB(struct UI_RPCData *RPCData)
{
    struct RPCDisableScreenDisplayData *Data=(struct RPCDisableScreenDisplayData *)RPCData->Data;

    if(Data->SEInstance->ConnectedCon!=NULL)
    {
        Data->SEInstance->ConnectedCon->DisableDisplayWrite(Data->Enabled);
    }

    return 0;
}

static int RunScriptThread(void *data)
{
    struct ScriptEngineInstance *SEInstance=(struct ScriptEngineInstance *)data;
    struct ScriptingEngineAPI *API;
    string Error;
    const char *LastError;
    bool FreeInstance;
    struct UI_RPCData RPCData;
    struct RPCScriptDoneData ScriptDoneData;

    try
    {
        API=&SEInstance->ScriptEngine->API;
        SEInstance->Context=API->
                AllocateContext((t_ScriptingEngineInstType *)SEInstance);
        if(SEInstance->Context==NULL)
            throw("Failed to allocate context for script");

        if(!API->LoadScriptFromString(SEInstance->Context,
                SEInstance->StartOfScript))
        {
            throw("Error loading the script:\n");
        }

        /* Ok, we wait for the main thread to give us the go ahead to run
           the script (we don't mutex these because we are polling) */
        while(SEInstance->ThreadWaiting2Run)
        {
            OS_Sleep(1);

            /* Make sure we exit if the main thread frees */
            if(SEInstance->MainThreadFreed)
                throw(nullptr);
        }

        if(!API->RunLoadedScript(SEInstance->Context))
        {
            throw("Error running the script:\n");
        }
    }
    catch(const char *Err)
    {
        if(Err!=NULL)
        {
            Error=Err;
            if(SEInstance->Context!=NULL)
            {
                LastError=API->GetLastError(SEInstance->Context);
                if(LastError!=NULL)
                Error+=LastError;
            }
            Scripting_ThreadAsk("Script error",Error.c_str(),e_AskBox_Error,
                    e_AskBttns_Ok);
        }
    }

    /* Ok, we need to tell the main thread that we are quiting this script */
    ScriptDoneData.SEInstance=SEInstance;
    RPCData.MainThreadFunction=Scripting_MainThreadScriptDoneCB;
    RPCData.Data=(void *)&ScriptDoneData;
    DoGenericRPC2MainThread(&RPCData);

    /* We are done, remove us from the use count */
    mtx_lock(&SEInstance->ScriptEngine->UseCountMutex);
    SEInstance->ScriptEngine->UseCount--;
    mtx_unlock(&SEInstance->ScriptEngine->UseCountMutex);

    if(SEInstance->Context!=NULL)
        API->FreeContext(SEInstance->Context);

    /* Mark the instance as freed */
    mtx_lock(&SEInstance->SharedMutex);
    SEInstance->ThreadFreed=true;
    FreeInstance=SEInstance->MainThreadFreed;
    mtx_unlock(&SEInstance->SharedMutex);

    /* See if main thread is also free (if so free the memory) */
    if(FreeInstance)
    {
        free(SEInstance->InComingQueue.Queue);
        delete SEInstance;
    }

    return 0;
}
