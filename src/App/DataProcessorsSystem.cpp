void DebugMsg(const char *fmt,...);
/*******************************************************************************
 * FILENAME: DataProcessorsSystem.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the data processors system it in.  It lets data processors
 *    register them selfs and then calls each one for each incoming byte.
 *
 *    The data processors operate on each byte of data that come in from a
 *    device and on each byte going to a device, converting the data as it
 *    flows though them.  An example is the ANSI escape codes processor,
 *    it takes key presses and converts them into ANSI escape seq's.  It also
 *    takes and interpretes the escape seq's as they come in converts them
 *    to colors.
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

/*
NOTES:
 * Doing 1 char at a time (one unicode char)
 * Should use a unicode plugin to make bytes in to unicode chars


Binary filters:
 * Only one filter active at a time (no flowing from one to the next).
 * Can replace a char with a string

Text filters:
 * Input flows from filter to filter
 * Processor can mark char as 'consumed'
 * Processor can only replace char was a different char not a string
 * TODO: Add a priority system to input filters

Example plugins:
 * "HEX", is binary, replaces all incoming chars with a hex dump
 * "Test ANSI", is text, takes ! makes it cursor up, @ cursor down, # cursor left, $ cursor right, % color 1, ^ color 2, & attrib change
 * "http highlighter", is text, takes http://xxxxx.com and under lines it (will have to back up and change the inserted text attribs)

*/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Connections.h"
#include "App/ConnectionsGlobal.h"
#include "App/DataProcessorsSystem.h"
#include "App/Settings.h"
#include "App/PluginSupport/PluginUISupport.h"
#include "App/PluginSupport/KeyValueSupport.h"
#include "App/PluginSupport/PluginSystem.h"
#include "PluginSDK/Plugin.h"
#include "UI/UIAsk.h"
#include "UI/UIDebug.h"
#include <string.h>
#include <stdlib.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
PG_BOOL DPS_RegisterDataProcessor(const char *ProID,const struct DataProcessorAPI *ProAPI,int SizeOfProAPI);
void DPS_SetFGColor(uint32_t FGColor);
uint32_t DPS_GetFGColor(void);
void DPS_SetBGColor(uint32_t BGColor);
uint32_t DPS_GetBGColor(void);
void DPS_SetULineColor(uint32_t ULineColor);
uint32_t DPS_GetULineColor(void);
void DPS_SetAttribs(uint32_t Attribs);
uint32_t DPS_GetAttribs(void);
void DPS_DoNewLine(void);
void DPS_DoReturn(void);
void DPS_DoBackspace(void);
void DPS_SetCursorXY(uint32_t X,uint32_t Y);
void DPS_DoClearScreen(void);
void DPS_DoClearScreenAndBackBuffer(void);
void DPS_DoClearArea(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2);
void DPS_GetCursorXY(int32_t *RetCursorX,int32_t *RetCursorY);
void DPS_InsertString(uint8_t *Str,uint32_t Len);
void DPS_GetScreenSize(int32_t *RetRows,int32_t *RetColumns);
uint32_t DPS_GetSysColor(uint32_t SysColShade,uint32_t SysColor);
uint32_t DPS_GetSysDefaultColor(uint32_t DefaultColor);
void DPS_NoteNonPrintable(const char *CodeStr);
void DPS_DoTab(void);
void DPS_DoPrevTab(void);
static void DPS_SetTitle(const char *Title);
static void DPS_SetCurrentSettingsTabName(const char *Name);
static t_WidgetSysHandle *DPS_AddNewSettingsTab(const char *Name);
void DPS_SendBackspace(void);
void DPS_SendEnter(void);
void DPS_BinaryAddText(const char *Str);
void DPS_BinaryAddHex(uint8_t Byte);
void DPS_DoSystemBell(int VisualOnly);
void DPS_DoScrollArea(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2,
        int32_t DeltaX,int32_t DeltaY);
static struct PluginSettings *DPS_FindPluginSetting(const char *IDStr,
        class ConSettings *Settings);
void DPS_DoProcessIncomingTextByteCallbacks(struct ProcessorConData *FData,e_TextDataProcessorClassType CallClass,uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,PG_BOOL *Consumed);

static t_DataProMark *DPS_AllocateMark(void);
static void DPS_FreeMark(t_DataProMark *Mark);
static PG_BOOL DPS_IsMarkValid(t_DataProMark *Mark);
static void DPS_SetMark2CursorPos(t_DataProMark *Mark);
static void DPS_ApplyAttrib2Mark(t_DataProMark *Mark,uint32_t Attrib,uint32_t Offset,uint32_t Len);
static void DPS_RemoveAttribFromMark(t_DataProMark *Mark,uint32_t Attrib,uint32_t Offset,uint32_t Len);
static void DPS_ApplyFGColor2Mark(t_DataProMark *Mark,uint32_t FGColor,uint32_t Offset,uint32_t Len);
static void DPS_ApplyBGColor2Mark(t_DataProMark *Mark,uint32_t BGColor,uint32_t Offset,uint32_t Len);
static void DPS_MoveMark(t_DataProMark *Mark,int Amount);
static const uint8_t *DPS_GetMarkString(t_DataProMark *Mark,uint32_t *Size,uint32_t Offset,uint32_t Len);
static void DPS_FreezeStream(void);
static void DPS_ClearFrozenStream(void);
static void DPS_ReleaseFrozenStream(void);
static const uint8_t *DPS_GetFrozenString(uint32_t *Size);

/*** VARIABLE DEFINITIONS     ***/
static struct DataProcessor *m_ActiveDataProcessor;

struct DPS_API g_DPSAPI=
{
    DPS_RegisterDataProcessor,
    PIUSDefault_GetDefaultAPI,
    Con_WriteData,
    DPS_GetSysColor,
    DPS_GetSysDefaultColor,
    DPS_SetFGColor,
    DPS_GetFGColor,
    DPS_SetBGColor,
    DPS_GetBGColor,
    DPS_SetULineColor,
    DPS_GetULineColor,
    DPS_SetAttribs,
    DPS_GetAttribs,
    DPS_SetTitle,
    DPS_DoNewLine,
    DPS_DoReturn,
    DPS_DoBackspace,
    DPS_DoClearScreen,
    DPS_DoClearArea,
    DPS_DoTab,
    DPS_DoPrevTab,
    DPS_DoSystemBell,
    DPS_DoScrollArea,
    DPS_DoClearScreenAndBackBuffer,
    DPS_GetCursorXY,
    DPS_SetCursorXY,
    DPS_GetScreenSize,
    DPS_NoteNonPrintable,
    DPS_SendBackspace,
    DPS_SendEnter,
    DPS_BinaryAddText,
    DPS_BinaryAddHex,
    DPS_InsertString,
    /* V2 */
    DPS_SetCurrentSettingsTabName,
    DPS_AddNewSettingsTab,
    DPS_AllocateMark,
    DPS_FreeMark,
    DPS_IsMarkValid,
    DPS_SetMark2CursorPos,
    DPS_ApplyAttrib2Mark,
    DPS_RemoveAttribFromMark,
    DPS_ApplyFGColor2Mark,
    DPS_ApplyBGColor2Mark,
    DPS_MoveMark,
    DPS_GetMarkString,
    DPS_FreezeStream,
    DPS_ClearFrozenStream,
    DPS_ReleaseFrozenStream,
    DPS_GetFrozenString,
};
t_DPSDataProcessorsType m_DataProcessors;     // All available data processors

static void *(*DPS_PS_GuiCtrlFn)(e_DataProPlugSettingsFnType Fn,void *Arg1,void *Arg2);

/*******************************************************************************
 * NAME:
 *    DPS_Init
 *
 * SYNOPSIS:
 *    void DPS_Init(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function init's the Data Processing System.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DPS_Init(void)
{
}

/*******************************************************************************
 * NAME:
 *    DPS_RegisterDataProcessor
 *
 * SYNOPSIS:
 *    PG_BOOL DPS_RegisterDataProcessor(const char *ProID,
 *              const struct DataProcessorAPI *ProAPI,int SizeOfProAPI);
 *
 * PARAMETERS:
 *    ProID [I] -- The ID name of the data processor.  This is a name (without
 *                   spaces) that the system uses to find this input processor
 *                   between sessions.  It needs to be unique for this
 *                   processor.
 *    ProAPI [I] -- The callbacks for this data processor.  See below.
 *    SizeOfProAPI [I] -- The sizeof the 'ProAPI' structure the plugin
 *                        is using.  This is used for forward / backward
 *                        compatibility.
 *
 * FUNCTION:
 *    This function registers a new data processor.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error.  The user has been informed.
 *
 * CALLBACKS:
 *==============================================================================
 * NAME:
 *    AllocateData
 *
 * SYNOPSIS:
 *    t_DataProcessorHandleType *AllocateData(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates any needed data for this data processor.
 *
 * NOTES:
 *    You can not use most of the API 'DPS_API' because there is no connection
 *    when AllocateData() is called.
 *
 * RETURNS:
 *   A pointer to the data, NULL if there was an error.
 *==============================================================================
 * NAME:
 *    FreeData
 *
 * SYNOPSIS:
 *    void FreeData(t_DataProcessorHandleType *DataHandle);
 *
 * PARAMETERS:
 *    DataHandle [I] -- The data handle to free.  This will need to be
 *                      case to your internal data type before you use it.
 *
 * FUNCTION:
 *    This function frees the memory allocated with AllocateData().
 *
 * RETURNS:
 *    NONE
 *==============================================================================
 *    NAME:
 *      GetProcessorInfo
 *
 *    SYNOPSIS:
 *      const struct DataProcessorInfo *GetProcessorInfo(
 *              unsigned int *SizeOfInfo);
 *
 *    PARAMETERS:
 *      SizeOfInfo [O] -- The size of 'struct DataProcessorInfo'.  This is used
 *                        for forward / backward compatibility.
 *
 *    FUNCTION:
 *      This function gets info about the plugin.  'DataProcessorInfo' has
 *      the following fields:
 *          DisplayName -- The name we show the user
 *          Tip -- A tool tip (for when you hover the mouse over this plugin)
 *          Help -- A help string for this plugin.
 *          ProType -- The type of process.  Supported values:
 *                  e_DataProcessorType_Text -- This is a text processor.
 *                      This is the more clasic type of processor (like VT100).
 *                  e_DataProcessorType_Binary -- This is a binary processor.
 *                      These are processors for binary protocol.  This may
 *                      be something as simple as a hex dump.
 *          TxtClass -- This only applies to 'e_DataProcessorType_Text' type
 *              processors. This is what class of text processor is
 *              this.  Supported classes:
 *                      e_TextDataProcessorClass_Other -- This is a generic
 *                          class more than one of these processors can be
 *                          active at a time but no other requirements exist.
 *                      e_TextDataProcessorClass_CharEncoding -- This is a
 *                          class that converts the raw stream into some kind
 *                          of char encoding.  For example unicode is converted
 *                          from a number of bytes to chars in the system.
 *                      e_TextDataProcessorClass_TermEmulation -- This is a
 *                          type of terminal emulator.  An example of a
 *                          terminal emulator is VT100.
 *                      e_TextDataProcessorClass_Highlighter -- This is a
 *                          processor that highlights strings as they come in
 *                          the input stream.  For example a processor that
 *                          underlines URL's.
 *                      e_TextDataProcessorClass_Logger -- This is a processor
 *                          that saves the input.  It may save to a file or
 *                          send out a debugging service.  And example is
 *                          a processor that saves all the raw bytes to a file.
 *
 *    RETURNS:
 *      NONE
 *==============================================================================
 *    NAME:
 *      ProcessIncomingTextByte
 *
 *    SYNOPSIS:
 *      void ProcessIncomingTextByte(t_DataProcessorHandleType *DataHandle,
 *          const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
 *          PG_BOOL *Consumed);
 *
 *    PARAMETERS:
 *      DataHandle [I] -- The data handle to work on.  This is your internal
 *                        data.
 *      RawByte [I] -- This is the byte that came in.
 *      ProcessedChar [I/O] -- This is a unicode char that has already been
 *                           processed by some of the other input filters.  You
 *                           can change this as you need.  It must remain only
 *                           one unicode char.
 *      CharLen [I/O] -- This number of bytes in 'ProcessedChar'
 *      Consumed [I/O] -- This tells the system (and other filters) if the
 *                        char has been used up and will not be added to the
 *                        screen.
 *
 *    FUNCTION:
 *      This function is called for each byte that comes in if you are a
 *      'e_DataProcessorType_Text' type of processor.  You work on the
 *      'ProcessedChar' to change the byte as needed.
 *
 *      If you set 'Consumed' to true then the 'ProcessedChar' will not be added
 *      to the display (or passed to other processors).  If it is set to
 *      false then it will be added to the screen.
 *
 *    RETURNS:
 *      NONE
 *
 *==============================================================================
 * NAME:
 *    ProcessIncomingBinaryByte
 *
 * SYNOPSIS:
 *    void ProcessIncomingBinaryByte(t_DataProcessorHandleType *DataHandle,
 *          const uint8_t Byte);
 *
 * PARAMETERS:
 *      DataHandle [I] -- The data handle to work on.  This is your internal
 *                        data.
 *      Byte [I] -- This is the byte that came in.
 *
 * FUNCTION:
 *      This function is called for each byte that comes in if you are a
 *      'e_DataProcessorType_Binary' type of processor.
 *
 *      You process this byte and call one of the add to screen functions (or
 *      all of them if you like).  See BinaryAddText()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    BinaryAddText()
 *==============================================================================
 *    NAME:
 *      ProcessKeyPress
 *
 *    SYNKPPSIS:
 *      bool ProcessKeyPress(t_DataProcessorHandleType *DataHandle,
 *          const uint8_t *KeyChar,int KeyCharLen,e_UIKeys ExtendedKey,
 *          uint8_t Mod);
 *
 *    PARAMETERS:
 *      DataHandle [I] -- The data handle to work on.  This is your internal
 *                        data.
 *      KeyChar [I] -- The key that was pressed.  In a UTF-8 string.  This is
 *                     a single char but maybe any bytes.  This len will be in
 *                     'KeyCharLen'.
 *      KeyCharLen [I] -- The number of bytes in 'KeyChar'.  If this is 0
 *                        then 'ExtendedKey' is used for what key was pressed.
 *      ExtendedKey [I] -- The non text key that was pressed.  This includes
 *                         things like the arrow keys, function keys, etc.
 *                         This is only used if 'KeyCharLen' is 0.
 *      Mod [I] -- What modifier keys where held when this key was pressed.
 *                 This is a bit field (so more than one key can be held.
 *                 Supported bits:
 *                      KEYMOD_SHIFT -- Shift key
 *                      KEYMOD_CONTROL -- Control hey
 *                      KEYMOD_ALT -- Alt key
 *                      KEYMOD_LOGO -- Logo key (Windows, Amiga, Command, etc)
 *
 *    FUNCTION:
 *      This function is called for each key pressed that would normally be
 *      sent out.  This includes extended keys like the arrows.  This is only
 *      called for data processors that are
 *      e_TextDataProcessorClass_TermEmulation only.
 *
 *    RETURNS:
 *      true -- This key was used up (do not pass to other key processors)
 *      false -- This key should continue on it's way though the key press
 *               processor list.
 *==============================================================================
 * NAME:
 *    ProcessOutGoingData
 *
 * SYNOPSIS:
 *    void ProcessOutGoingData(t_DataProcessorHandleType *DataHandle,
 *          const uint8_t *TxData,int Bytes);
 *
 * PARAMETERS:
 *    DataHandle [I] -- The data handle to work on.  This is your internal
 *                      data.
 *    TxData [I] -- This is the block of data that is about to be sent
 *    Bytes [I] -- The number of bytes in the 'Data' block.
 *
 * FUNCTION:
 *    This function is called for block of data being send to a IO driver.
 *    This is in addition to ProcessKeyPress(), this is called for every
 *    byte being sent, where as ProcessKeyPress() is only called for
 *    key presses.  You will be called 2 times for key presses, one for
 *    ProcessKeyPress(), and then this function.  For things like Send Buffers
 *    no ProcessKeyPress() will be called only this function.
 *    So if you want to see all bytes going out use this function.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    ProcessKeyPress()
 *==============================================================================
 * NAME:
 *    AllocSettingsWidgets
 *
 * SYNOPSIS:
 *    t_DataProSettingsWidgetsType *AllocSettingsWidgets(
 *              t_WidgetSysHandle *WidgetHandle,t_PIKVList *Settings);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to add new widgets to
 *    Settings [I] -- The current settings.  This is a standard key/value
 *                    list.
 *
 * FUNCTION:
 *    This function is called when the user presses the "Settings" button
 *    to change any settings for this plugin (in the settings dialog).  If
 *    this is NULL then the user can not press the settings button.
 *
 *    When the plugin settings dialog is open it will have a tab control in
 *    it with a "Generic" tab opened.  Your widgets will be added to this
 *    tab.  If you want to add a new tab use can call the DPS_API
 *    function AddNewSettingsTab().  If you want to change the name of the
 *    first tab call SetCurrentSettingsTabName() before you add a new tab.
 *
 * RETURNS:
 *    The private settings data that you want to use.  This is a private
 *    structure that you allocate and then cast to
 *    (t_DataProSettingsWidgetsType *) when you return.  It's up to you what
 *    you want to do with this data (if you do not want to use it return
 *    a fixed int set to 1, and ignore it in FreeSettingsWidgets.  If you
 *    return NULL it is considered an error.
 *
 * SEE ALSO:
 *    FreeSettingsWidgets(), SetCurrentSettingsTabName(), AddNewSettingsTab()
 *==============================================================================
 * NAME:
 *    FreeSettingsWidgets
 *
 * SYNOPSIS:
 *    void FreeSettingsWidgets(t_DataProSettingsWidgetsType *PrivData);
 *
 * PARAMETERS:
 *    PrivData [I] -- The private data to free
 *
 * FUNCTION:
 *      This function is called when the system frees the settings widets.
 *      It should free any private data you allocated in AllocSettingsWidgets().
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    AllocSettingsWidgets()
 *==============================================================================
 * NAME:
 *    SetSettingsFromWidgets
 *
 * SYNOPSIS:
 *    void SetSettingsFromWidgets(t_DataProSettingsWidgetsType *PrivData,
 *              t_PIKVList *Settings);
 *
 * PARAMETERS:
 *    PrivData [I] -- Your private data allocated in AllocSettingsWidgets()
 *    Settings [O] -- This is where you store the settings.
 *
 * FUNCTION:
 *    This function takes the widgets added with AllocSettingsWidgets() and
 *    stores them is a key/value pair list in 'Settings'.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    AllocSettingsWidgets()
 *==============================================================================
 * NAME:
 *    ApplySettings
 *
 * SYNOPSIS:
 *    void ApplySettings(t_DataProcessorHandleType *DataHandle,
 *              t_PIKVList *Settings);
 *
 * PARAMETERS:
 *    DataHandle [I] -- The data handle to work on.  This is your internal
 *                      data.
 *    Settings [I] -- This is where you get your settings from.
 *
 * FUNCTION:
 *    This function takes the settings from 'Settings' and setups the
 *    plugin to use them when new bytes come in or out.  It will normally
 *    copy the settings from key/value pairs to internal data structures.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 *==============================================================================
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
PG_BOOL DPS_RegisterDataProcessor(const char *ProID,
        const struct DataProcessorAPI *ProAPI,int SizeOfProAPI)
{
    struct DataProcessor NewPro;
    unsigned int Size2Copy;
    i_DPSDataProcessorsType pro;
    const struct DataProcessorInfo *Info;
    unsigned int InfoSize;

    try
    {
        /* Check if this is already registered */
        for(pro=m_DataProcessors.begin();pro!=m_DataProcessors.end();pro++)
            if(strcmp(pro->ProID.c_str(),ProID)==0)
                break;
        if(pro!=m_DataProcessors.end())
            throw(0);

        if(ProAPI->GetProcessorInfo==NULL)
            throw(0);
        InfoSize=0;
        Info=ProAPI->GetProcessorInfo(&InfoSize);
        if(InfoSize==0)
        {
            /* We can't register a plugin with 0 info */
            throw(0);
        }

        NewPro.ProID=ProID;
        memset(&NewPro.Info,0x00,sizeof(NewPro.Info));
        Size2Copy=InfoSize;
        if(Size2Copy>sizeof(NewPro.Info))
            Size2Copy=sizeof(NewPro.Info);
        memcpy(&NewPro.Info,Info,Size2Copy);

        memset(&NewPro.API,0x00,sizeof(NewPro.API));
        Size2Copy=SizeOfProAPI;
        if(Size2Copy>sizeof(NewPro.API))
            Size2Copy=sizeof(NewPro.API);
        memcpy(&NewPro.API,ProAPI,Size2Copy);

        m_DataProcessors.push_back(NewPro);

        /* Register this plugin with system */
        RegisterPluginWithSystem(ProID);
    }
    catch(...)
    {
        string ErrorMsg;
        ErrorMsg="Failed to register ";
        ErrorMsg+=ProID;
        UIAsk("Failed",ErrorMsg,e_AskBox_Error,e_AskBttns_Ok);
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    DPS_AllocProcessorConData
 *
 * SYNOPSIS:
 *    bool DPS_AllocProcessorConData(struct ProcessorConData *FData,
 *          class ConSettings *CustomSettings);
 *
 * PARAMETERS:
 *    FData [O] -- The filter connection data.
 *    CustomSettings [I] -- The custom settings to use from this connection.
 *
 * FUNCTION:
 *    This function init's the data processors con data when a new
 *    connection is started.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DPS_AllocProcessorConData(struct ProcessorConData *FData,
        class ConSettings *CustomSettings)
{
    i_DPSDataProcessorsType Processor;
    t_DataProcessorHandleType *NewProcessorData;
    i_DPSDataProcessorsType CurProcessor;
    i_StringListType CurStr;
    struct PluginSettings *PlugSettings;
    t_KVList BlankKVList;
    t_KVList *SettingsKVList;

    FData->Settings=CustomSettings;

    /* Copy the data processors list (based on settings) for this connection */
    if(CustomSettings->DataProcessorType==e_DataProcessorType_Text)
    {
        for(CurStr=CustomSettings->EnabledTextDataProcessors.begin();
                CurStr!=CustomSettings->EnabledTextDataProcessors.end();
                CurStr++)
        {
            /* Find this input processor */
            for(Processor=m_DataProcessors.begin();
                    Processor!=m_DataProcessors.end();Processor++)
            {
                if(strcmp(Processor->ProID.c_str(),CurStr->c_str())==0)
                {
                    /* Found it */
                    FData->DataProcessorsList.push_back(*Processor);
                    break;
                }
            }
        }

        /* Handle the Term Emu */
        for(CurStr=CustomSettings->EnabledTermEmuDataProcessors.begin();
                CurStr!=CustomSettings->EnabledTermEmuDataProcessors.end();
                CurStr++)
        {
            /* Find this input processor */
            for(Processor=m_DataProcessors.begin();
                    Processor!=m_DataProcessors.end();Processor++)
            {
                if(strcmp(Processor->ProID.c_str(),CurStr->c_str())==0)
                {
                    /* Found it */
                    FData->DataProcessorsList.push_back(*Processor);
                    break;
                }
            }
        }
    }
    else
    {
        for(CurStr=CustomSettings->EnabledBinaryDataProcessors.begin();
                CurStr!=CustomSettings->EnabledBinaryDataProcessors.end();
                CurStr++)
        {
            /* Find this input processor */
            for(Processor=m_DataProcessors.begin();
                    Processor!=m_DataProcessors.end();Processor++)
            {
                if(strcmp(Processor->ProID.c_str(),CurStr->c_str())==0)
                {
                    /* Found it */
                    FData->DataProcessorsList.push_back(*Processor);
                    break;
                }
            }
        }
    }

    /* Allocate data for all the data processors */
    FData->ProcessorsData.reserve(FData->DataProcessorsList.size());

    for(CurProcessor=FData->DataProcessorsList.begin();
            CurProcessor!=FData->DataProcessorsList.end();CurProcessor++)
    {
        NewProcessorData=NULL;
        if(CurProcessor->API.AllocateData!=NULL)
        {
            NewProcessorData=CurProcessor->API.AllocateData();
            if(NewProcessorData==NULL)
                return false;

            if(CurProcessor->API.ApplySettings!=NULL)
            {
                PlugSettings=DPS_FindPluginSetting(CurProcessor->ProID.c_str(),
                        CustomSettings);

                if(PlugSettings!=NULL)
                    SettingsKVList=&PlugSettings->Settings;
                else
                    SettingsKVList=&BlankKVList;

                CurProcessor->API.ApplySettings(NewProcessorData,
                        PIS_ConvertKVList2PIKVList(*SettingsKVList));
            }
        }
        FData->ProcessorsData.push_back(NewProcessorData);

        /* Tell the system we are using this plugin */
        NotePluginInUse(CurProcessor->ProID.c_str());
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    DPS_FreeProcessorConData
 *
 * SYNOPSIS:
 *    void DPS_FreeProcessorConData(struct ProcessorConData *FData);
 *
 * PARAMETERS:
 *    FData [I/O] -- The processor connection data.
 *
 * FUNCTION:
 *    This function frees any memory allocated in DPS_AllocProcessorConData()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_AllocProcessorConData()
 ******************************************************************************/
void DPS_FreeProcessorConData(struct ProcessorConData *FData)
{
    i_DPSDataProcessorsType CurProcessor;
    unsigned int Index;

    for(CurProcessor=FData->DataProcessorsList.begin(),Index=0;
            CurProcessor!=FData->DataProcessorsList.end();
            CurProcessor++,Index++)
    {
        if(Index<FData->ProcessorsData.size())
        {
            if(CurProcessor->API.FreeData!=NULL)
                CurProcessor->API.FreeData(FData->ProcessorsData[Index]);
        }

        UnNotePluginInUse(CurProcessor->ProID.c_str());
    }

    FData->DataProcessorsList.clear();
    FData->ProcessorsData.clear();
}

/*******************************************************************************
 * NAME:
 *    DPS_ReapplyProcessor2Connection
 *
 * SYNOPSIS:
 *    bool DPS_ReapplyProcessor2Connection(struct ProcessorConData *FData,
 *              class ConSettings *CustomSettings);
 *
 * PARAMETERS:
 *    FData [I/O] -- The filter connection data.
 *    CustomSettings [I] -- The new custom settings to apply to this connection.
 *
 * FUNCTION:
 *    This function goes thought the processor list and free's / allocates
 *    any new processors.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error, this connection is no longer able to process
 *             data.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DPS_ReapplyProcessor2Connection(struct ProcessorConData *FData,
        class ConSettings *CustomSettings)
{
    /* DEBUG PAUL: This could be made smarter by looking at what processors
       have changed */
    DPS_FreeProcessorConData(FData);
    return DPS_AllocProcessorConData(FData,CustomSettings);
}

/*******************************************************************************
 * NAME:
 *    DPS_ProcessorIncomingBytes
 *
 * SYNOPSIS:
 *    void DPS_ProcessorIncomingBytes(struct ProcessorConData *FData,
 *          const uint8_t *inbuff,int bytes,bool DoAutoLF,bool DoAutoCR);
 *
 * PARAMETERS:
 *    FData [I] -- The connection processor data for this connection.
 *    inbuff [I] -- The buffer with the data that was read.  This is raw data
 *    bytes [I] -- The number of bytes that was read.
 *    DoAutoLF [I] -- If this is true then if a \n is found then DoReturn()
 *                    is called on the active connection (only for text
 *                    connections).
 *    DoAutoCR [I] -- If this is true then if a \r is found then DoNewLine()
 *                    is called on the active connection (only for text
 *                    connections).
 *
 * FUNCTION:
 *    This function takes bytes that have come into a connection and passes
 *    them into the filters.  As a last step it adds them to the main window.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DPS_ProcessorIncomingBytes(struct ProcessorConData *FData,
        const uint8_t *inbuff,int bytes,bool DoAutoLF,bool DoAutoCR)
{
    uint8_t ProcessedChar[MAX_BYTES_PER_CHAR+1];  // Buffer for the char we will eventually output.  UTF8 seems to be limited to 4 maybe 6 bytes so 10 should be good (it's up the plugin not to go over 6)
    PG_BOOL Consumed;
    int32_t byte;
    int CharLen;
    i_DPSDataProcessorsType CurProcessor;
    unsigned int Index;

    if(FData->Settings->DataProcessorType==e_DataProcessorType_Text)
    {
        /* Text mode data processors */
        for(byte=0;byte<bytes;byte++)
        {
            Consumed=false;
            CharLen=1;
            ProcessedChar[0]=inbuff[byte];

            /* We do all the different types of plugins, but we do then
               an known order */
            DPS_DoProcessIncomingTextByteCallbacks(FData,
                    e_TextDataProcessorClass_Logger,inbuff[byte],ProcessedChar,
                    &CharLen,&Consumed);
            DPS_DoProcessIncomingTextByteCallbacks(FData,
                    e_TextDataProcessorClass_CharEncoding,inbuff[byte],
                    ProcessedChar,&CharLen,&Consumed);
            DPS_DoProcessIncomingTextByteCallbacks(FData,
                    e_TextDataProcessorClass_TermEmulation,inbuff[byte],
                    ProcessedChar,&CharLen,&Consumed);
            DPS_DoProcessIncomingTextByteCallbacks(FData,
                    e_TextDataProcessorClass_Highlighter,inbuff[byte],
                    ProcessedChar,&CharLen,&Consumed);
            DPS_DoProcessIncomingTextByteCallbacks(FData,
                    e_TextDataProcessorClass_Other,inbuff[byte],ProcessedChar,
                    &CharLen,&Consumed);

            if(!Consumed)
            {
                ProcessedChar[CharLen]=0;   // Make it a string
DB_StartTimer(e_DBT_AddChar2Display);
                Con_WriteChar2Display(ProcessedChar);
DB_StopTimer(e_DBT_AddChar2Display);
            }

            if(DoAutoLF)
            {
                if(inbuff[byte]=='\n')
                    DPS_DoReturn();
            }

            if(DoAutoCR)
            {
                if(inbuff[byte]=='\r')
                    DPS_DoNewLine();
            }
        }
    }
    else
    {
        /* Binary data processors */
        for(byte=0;byte<bytes;byte++)
        {
            /* Decoders first */
            for(CurProcessor=FData->DataProcessorsList.begin(),Index=0;
                    CurProcessor!=FData->DataProcessorsList.end();
                    CurProcessor++,Index++)
            {
                m_ActiveDataProcessor=&*CurProcessor;

                if(CurProcessor->Info.BinClass==
                        e_BinaryDataProcessorClass_Decoder)
                {
                    if(CurProcessor->API.ProcessIncomingBinaryByte!=NULL)
                    {
                        CurProcessor->API.ProcessIncomingBinaryByte(
                                FData->ProcessorsData[Index],inbuff[byte]);
                    }
                }
            }

            /* Everything that isn't a decoder */
            for(CurProcessor=FData->DataProcessorsList.begin(),Index=0;
                    CurProcessor!=FData->DataProcessorsList.end();
                    CurProcessor++,Index++)
            {
                m_ActiveDataProcessor=&*CurProcessor;

                if(CurProcessor->Info.BinClass!=
                        e_BinaryDataProcessorClass_Decoder)
                {
                    if(CurProcessor->API.ProcessIncomingBinaryByte!=NULL)
                    {
                        CurProcessor->API.ProcessIncomingBinaryByte(
                                FData->ProcessorsData[Index],inbuff[byte]);
                    }
                }
            }
        }
        m_ActiveDataProcessor=NULL;
    }
}

/*******************************************************************************
 * NAME:
 *    DPS_DoProcessIncomingTextByteCallbacks
 *
 * SYNOPSIS:
 *    void DPS_DoProcessIncomingTextByteCallbacks(struct ProcessorConData *FData,
 *          e_TextDataProcessorClassType CallClass);
 *
 * PARAMETERS:
 *    FData [I] -- The processor connection data to work with
 *    CallClass [I] -- Call plugins that have this TxtClass.
 *
 * FUNCTION:
 *    This is a helper function for DPS_ProcessorIncomingBytes() it loops
 *    through all the plugins and if it's a text plugin and matches 'CallClass'
 *    then the ProcessIncomingTextByte() function is called.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_ProcessorIncomingBytes()
 ******************************************************************************/
void DPS_DoProcessIncomingTextByteCallbacks(struct ProcessorConData *FData,
        e_TextDataProcessorClassType CallClass,uint8_t RawByte,
        uint8_t *ProcessedChar,int *CharLen,PG_BOOL *Consumed)
{
    i_DPSDataProcessorsType CurProcessor;
    unsigned int Index;

    for(CurProcessor=FData->DataProcessorsList.begin(),Index=0;
            CurProcessor!=FData->DataProcessorsList.end();
            CurProcessor++,Index++)
    {
        m_ActiveDataProcessor=&*CurProcessor;
        if(CurProcessor->Info.TxtClass==CallClass)
        {
            if(CurProcessor->API.ProcessIncomingTextByte!=NULL)
            {
                CurProcessor->API.ProcessIncomingTextByte(FData->
                        ProcessorsData[Index],RawByte,ProcessedChar,
                        CharLen,Consumed);
            }
        }
    }
    m_ActiveDataProcessor=NULL;
}

/*******************************************************************************
 * NAME:
 *    DPS_ProcessorKeyPress
 *
 * SYNOPSIS:
 *    bool DPS_ProcessorKeyPress(struct ProcessorConData *FData,
 *              const uint8_t *KeyChar,int KeyCharLen,e_UIKeys ExtendedKey,
 *              uint8_t Mod);
 *
 * PARAMETERS:
 *    FData [I] -- The connection processor data for this connection.
 *    KeyChar [I] -- A string with the key in it (UTF8).  If the key can be
 *                   converted to a char this will have it.
 *    KeyCharLen [I] -- The number of bytes in 'KeyChar'
 *    ExtendedKey [I] -- What "special" key was pressed (things like arrow keys)
 *    Mods [I] -- What keys where held when this key event happended
 *
 * FUNCTION:
 *    This function .
 *
 * RETURNS:
 *    true -- Key event was used up
 *    false -- Key event should continue on it's way.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DPS_ProcessorKeyPress(struct ProcessorConData *FData,
        const uint8_t *KeyChar,int KeyCharLen,e_UIKeys ExtendedKey,uint8_t Mod)
{
    i_DPSDataProcessorsType CurProcessor;
    unsigned int Index;
    bool Consumed;

    Consumed=false;

    /* Send it to all the term emulations */
    for(CurProcessor=FData->DataProcessorsList.begin(),Index=0;
            CurProcessor!=FData->DataProcessorsList.end();
            CurProcessor++,Index++)
    {
        if((CurProcessor->Info.ProType==e_DataProcessorType_Text &&
                CurProcessor->Info.TxtClass==e_TextDataProcessorClass_TermEmulation) ||
                CurProcessor->Info.ProType==e_DataProcessorType_Binary)
        {
            if(CurProcessor->API.ProcessKeyPress!=NULL)
            {
                Consumed=CurProcessor->API.ProcessKeyPress(FData->
                        ProcessorsData[Index],KeyChar,KeyCharLen,
                        ExtendedKey,Mod);
                if(Consumed)
                    break;
            }
        }
    }

    return Consumed;
}

/*******************************************************************************
 * NAME:
 *    DPS_ProcessorOutGoingBytes
 *
 * SYNOPSIS:
 *    void DPS_ProcessorOutGoingBytes(struct ProcessorConData *FData,
 *              const uint8_t *outbuff,int bytes);
 *
 * PARAMETERS:
 *    FData [I] -- The connection processor data for this connection.
 *    outbuff [I] -- The buffer with outgoing bytes in it
 *    bytes [I] -- The number of bytes being sent
 *
 * FUNCTION:
 *    This function is called just before bytes are sent out to the IO driver.
 *    It sends the bytes to data processor plugin so it can know what is 
 *    being sent (for styling?).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DPS_ProcessorOutGoingBytes(struct ProcessorConData *FData,
        const uint8_t *outbuff,int bytes)
{
    i_DPSDataProcessorsType CurProcessor;
    unsigned int Index;

    /* Send it to all the term emulations */
    if(FData->Settings->DataProcessorType==e_DataProcessorType_Text)
    {
        /* Decoder's first */
        for(CurProcessor=FData->DataProcessorsList.begin(),Index=0;
                CurProcessor!=FData->DataProcessorsList.end();
                CurProcessor++,Index++)
        {
            m_ActiveDataProcessor=&*CurProcessor;
            if(CurProcessor->API.ProcessOutGoingData!=NULL)
            {
                CurProcessor->API.ProcessOutGoingData(FData->
                        ProcessorsData[Index],outbuff,bytes);
            }
            m_ActiveDataProcessor=NULL;
        }
    }
    else
    {
        /* Decoder's first */
        for(CurProcessor=FData->DataProcessorsList.begin(),Index=0;
                CurProcessor!=FData->DataProcessorsList.end();
                CurProcessor++,Index++)
        {
            m_ActiveDataProcessor=&*CurProcessor;
            if(CurProcessor->Info.BinClass==e_BinaryDataProcessorClass_Decoder)
            {
                if(CurProcessor->API.ProcessOutGoingData!=NULL)
                {
                    CurProcessor->API.ProcessOutGoingData(FData->ProcessorsData[Index],
                            outbuff,bytes);
                }
            }
            m_ActiveDataProcessor=NULL;
        }

        /* Everything that's not a decoder */
        for(CurProcessor=FData->DataProcessorsList.begin(),Index=0;
                CurProcessor!=FData->DataProcessorsList.end();
                CurProcessor++,Index++)
        {
            m_ActiveDataProcessor=&*CurProcessor;
            if(CurProcessor->Info.BinClass!=e_BinaryDataProcessorClass_Decoder)
            {
                if(CurProcessor->API.ProcessOutGoingData!=NULL)
                {
                    CurProcessor->API.ProcessOutGoingData(FData->
                            ProcessorsData[Index],outbuff,bytes);
                }
            }
            m_ActiveDataProcessor=NULL;
        }
    }
}

/*******************************************************************************
 * NAME:
 *    DPS_GetListOfTextProcessors
 *
 * SYNOPSIS:
 *    void DPS_GetListOfTextProcessors(e_TextDataProcessorClassType TxtClass,
 *          t_DPS_ProInfoType &RetData);
 *
 * PARAMETERS:
 *    TxtClass [I] -- What class of input processor to return a list of
 *    RetData [O] -- This is filled with info about the input processors.
 *                   The data return will be a vector of a structure with
 *                   the following fields:
 *                      IDStr -- The identifier string for this input
 *                               processor.  This will remain static until
 *                               a plugin is removed (currently not
 *                               possible, so it's static, but that may
 *                               change in the future).
 *                      DisplayName -- The name to display to the user
 *                      Tip -- A tool tip for this input processor
 *                      Help -- Help for this input processor.  This is
 *                              written in a text markup.
 *
 * FUNCTION:
 *    This function gets a list of text data processors of a requested
 *    class.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    The order of the returned data will change from time to time so you
 *    must use the 'IDStr' to identify the text data processor not the
 *    index.
 *
 * SEE ALSO:
 *    DPS_GetListOfBinaryProcessors(), DPS_GetDataProcessorPluginCount(),
 *    DPS_GetDataProcessorPluginList()
 ******************************************************************************/
void DPS_GetListOfTextProcessors(e_TextDataProcessorClassType TxtClass,
        t_DPS_ProInfoType &RetData)
{
    i_DPSDataProcessorsType CurProcessor;
    struct DPS_ProInfo NewEntry;

    RetData.clear();

    for(CurProcessor=m_DataProcessors.begin();
            CurProcessor!=m_DataProcessors.end();CurProcessor++)
    {
        if(CurProcessor->Info.ProType==e_DataProcessorType_Text &&
                CurProcessor->Info.TxtClass==TxtClass)
        {
            NewEntry.IDStr=CurProcessor->ProID.c_str();
            NewEntry.DisplayName=CurProcessor->Info.DisplayName;
            NewEntry.Tip=CurProcessor->Info.Tip;
            NewEntry.Help=CurProcessor->Info.Help;
            NewEntry.ProcessorData=&*CurProcessor;

            RetData.push_back(NewEntry);
        }
    }
}

/*******************************************************************************
 * NAME:
 *    DPS_GetListOfBinaryProcessors
 *
 * SYNOPSIS:
 *    void DPS_GetListOfBinaryProcessors(e_BinaryDataProcessorClassType BinClass,
 *          t_DPS_ProInfoType &RetData);
 *
 * PARAMETERS:
 *    BinClass [I] -- What type of binary processors to get
 *    RetData [O] -- This is filled with info about the input processors.
 *                   The data return will be a vector of a structure with
 *                   the following fields:
 *                      IDStr -- The identifier string for this input
 *                               processor.
 *                      DisplayName -- The name to display to the user
 *                      Tip -- A tool tip for this input processor
 *                      Help -- Help for this input processor.  This is
 *                              written in a text markup.
 *
 * FUNCTION:
 *    This function gets a list of binary data processors.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    The order of the returned data will change from time to time so you
 *    must use the 'IDStr' to identify the text data processor not the
 *    index.
 *
 * SEE ALSO:
 *    DPS_GetListOfTextProcessors()
 ******************************************************************************/
void DPS_GetListOfBinaryProcessors(e_BinaryDataProcessorClassType BinClass,
        t_DPS_ProInfoType &RetData)
{
    i_DPSDataProcessorsType CurProcessor;
    struct DPS_ProInfo NewEntry;

    RetData.clear();

    for(CurProcessor=m_DataProcessors.begin();
            CurProcessor!=m_DataProcessors.end();CurProcessor++)
    {
        if(CurProcessor->Info.ProType==e_DataProcessorType_Binary &&
                CurProcessor->Info.BinClass==BinClass)
        {
            NewEntry.IDStr=CurProcessor->ProID.c_str();
            NewEntry.DisplayName=CurProcessor->Info.DisplayName;
            NewEntry.Tip=CurProcessor->Info.Tip;
            NewEntry.Help=CurProcessor->Info.Help;
            NewEntry.ProcessorData=&*CurProcessor;

            RetData.push_back(NewEntry);
        }
    }
}

/*******************************************************************************
 * NAME:
 *    DPS_GetDataProcessorPluginCount
 *
 * SYNOPSIS:
 *    uint_fast32_t DPS_GetDataProcessorPluginCount(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This funciton gets the total number of plugins that have registered
 *    the data processors system.
 *
 * RETURNS:
 *    The number of plugins currently registered.
 *
 * NOTES:
 *    This count can go down if a plugin is unregister and removed.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
uint_fast32_t DPS_GetDataProcessorPluginCount(void)
{
    return m_DataProcessors.size();
}

/*******************************************************************************
 * NAME:
 *    DPS_GetDataProcessorPluginList
 *
 * SYNOPSIS:
 *    void DPS_GetDataProcessorPluginList(t_DPS_ProInfoType &RetData);
 *
 * PARAMETERS:
 *    RetData [O] -- This is filled with info about the input processors.
 *                   The data return will be a vector of a structure with
 *                   the following fields:
 *                      IDStr -- The identifier string for this input
 *                               processor.
 *                      DisplayName -- The name to display to the user
 *                      Tip -- A tool tip for this input processor
 *                      Help -- Help for this input processor.  This is
 *                              written in a text markup.
 *
 * FUNCTION:
 *    This function gets a list of all registered data processors.  The list
 *    will be ordered by when they where registered.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    The order of the returned data will change from time to time so you
 *    must use the 'IDStr' to identify the text data processor not the
 *    index.
 *
 * SEE ALSO:
 *    DPS_GetListOfTextProcessors()
 ******************************************************************************/
void DPS_GetDataProcessorPluginList(t_DPS_ProInfoType &RetData)
{
    i_DPSDataProcessorsType CurProcessor;
    struct DPS_ProInfo NewEntry;

    RetData.clear();
    RetData.reserve(m_DataProcessors.size());

    for(CurProcessor=m_DataProcessors.begin();
            CurProcessor!=m_DataProcessors.end();CurProcessor++)
    {
        NewEntry.IDStr=CurProcessor->ProID.c_str();
        NewEntry.DisplayName=CurProcessor->Info.DisplayName;
        NewEntry.Tip=CurProcessor->Info.Tip;
        NewEntry.Help=CurProcessor->Info.Help;
        NewEntry.ProcessorData=&*CurProcessor;

        RetData.push_back(NewEntry);
    }
}

/*******************************************************************************
 * NAME:
 *    DPS_GetProcessorsInfo
 *
 * SYNOPSIS:
 *    const struct DataProcessor *DPS_GetProcessorsInfo(const char *IDStr);
 *
 * PARAMETERS:
 *    IDStr [I] -- The ID string for this processor to lookup
 *
 * FUNCTION:
 *    This function finds a data processor from the list of available data
 *    processors.
 *
 * RETURNS:
 *    A pointer to the data process or NULL if it was not found.  This will
 *    be valid until another call to a data processor system call.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
const struct DataProcessor *DPS_GetProcessorsInfo(const char *IDStr)
{
    i_DPSDataProcessorsType CurProcessor;

    /* See if we can find this data processor */
    for(CurProcessor=m_DataProcessors.begin();
            CurProcessor!=m_DataProcessors.end();CurProcessor++)
    {
        if(strcmp(CurProcessor->ProID.c_str(),IDStr)==0)
        {
            /* Found it */
            return &*CurProcessor;
        }
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    DPS_DoesPluginHaveSettings
 *
 * SYNOPSIS:
 *    bool DPS_DoesPluginHaveSettings(const char *IDStr);
 *
 * PARAMETERS:
 *    IDStr [I] -- The ID string for this processor to lookup
 *
 * FUNCTION:
 *    This function checks if a plugin has a settings dialog.
 *
 * RETURNS:
 *    true -- It does use a settings dialog
 *    false -- There are no settings for this plugin.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DPS_DoesPluginHaveSettings(const char *IDStr)
{
    const struct DataProcessor *ProData;

    ProData=DPS_GetProcessorsInfo(IDStr);
    if(ProData!=NULL)
        if(ProData->API.AllocSettingsWidgets!=NULL)
            return true;

    return false;
}

/*******************************************************************************
 * NAME:
 *    DPS_PluginSettings_SetActiveCtrls
 *
 * SYNOPSIS:
 *    void DPS_PluginSettings_SetActiveCtrls(void *(*GuiCtrl)(e_DataProPlugSettingsFnType Fn,void *Arg1,void *Arg2))
 *
 * PARAMETERS:
 *    GuiCtrl [I] -- The function to call when the plugin makes a GUI request
 *                   for the settings.  See below.
 *
 * FUNCTION:
 *    This function sets the callback function when the plugin makes a
 *    settings GUI request.  When you are done with the settings you should
 *    call this will NULL (so you don't get unexpected callbacks).
 *
 * CALLBACKS:
 * =============================================================================
 * NAME:
 *    GuiCtrl
 *
 * SYNOPSIS:
 *    void *GuiCtrl(e_DataProPlugSettingsFnType Fn,void *Arg1,void *Arg2);
 *
 * PARAMETERS:
 *    Fn [I] -- What function is the plugin trying to do.  Supported fns:
 *                  e_DataProPlugSettingsFn_SetCurrentTabName -- Changes the
 *                          name of the current tab that widgets are being
 *                          added to.
 *                              Arg1 -- "const char *" with the new name in it.
 *                              Arg2 -- ignored.
 *                              Return value: NULL
 *                  e_DataProPlugSettingsFn_AddNewTab -- A new tab should
 *                          be allocated and the t_UIContainerFrameCtrl for
 *                          this tab should be returned.
 *                              Arg1 -- "const char *" with the tab name in it.
 *                              Arg2 -- ignored.
 *                              Return value: "t_UIContainerFrameCtrl *" with
 *                                      the handle to the container for the
 *                                      new tab in it.
 *
 * FUNCTION:
 *    This function is called when a plugin is in its settings and wants
 *    to preform an action.  See above for the actions.
 *
 * RETURNS:
 *    Depends on the 'Fn'  See above.
 *
 * SEE ALSO:
 *    
 * =============================================================================
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DPS_PluginSettings_SetActiveCtrls(void *(*GuiCtrl)(e_DataProPlugSettingsFnType Fn,void *Arg1,void *Arg2))
{
    DPS_PS_GuiCtrlFn=GuiCtrl;
}

/*******************************************************************************
 * NAME:
 *    DPS_PluginSettings_AddWidgets
 *
 * SYNOPSIS:
 *    t_DataProSettingsWidgetsType *DPS_PluginSettings_AddWidgets(class ConSettings *Settings,
 *              const char *IDStr,t_UIContainerFrameCtrl *Cont);
 *
 * PARAMETERS:
 *    Settings [I] -- The settings with the plugin settings in it.
 *    IDStr [I] -- The ID string for this processor to lookup
 *    Cont [I] -- This is the GUI container that the plugin will add it's
 *                widgets to.
 *
 * FUNCTION:
 *    This function tells the plugin to add it's settings widgets to a
 *    container and allocate it's private data.
 *
 * RETURNS:
 *    A pointer to the private data the plugin allocated or NULL if there was
 *    an error.
 *
 * NOTES:
 *    The plugin can call DPS_AddNewSettingsTab() to add a new tab to the
 *    current GUI for more settings.  You need to support this (you need
 *    to have container in a tab input) and you need to support the callbacks
 *    from the plugin using the DPS_PluginSettings_SetActiveCtrls() function.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_DataProSettingsWidgetsType *DPS_PluginSettings_AddWidgets(class ConSettings *Settings,
        const char *IDStr,t_UIContainerFrameCtrl *Cont)
{
    const struct DataProcessor *ProData;
    t_DataProSettingsWidgetsType *PrivData;
    struct PluginSettings *PlugSettings;
    t_KVList BlankKVList;
    t_KVList *UseSettings;

    ProData=DPS_GetProcessorsInfo(IDStr);
    if(ProData==NULL || ProData->API.AllocSettingsWidgets==NULL)
        return NULL;

    /* We need to find the settings for this plugin */
    PlugSettings=DPS_FindPluginSetting(IDStr,Settings);
    if(PlugSettings==NULL)
    {
        /* Ok, we don't have settings for this plugin yet use a blank copy */
        UseSettings=&BlankKVList;
    }
    else
    {
        UseSettings=&PlugSettings->Settings;
    }

    PrivData=ProData->API.AllocSettingsWidgets((t_WidgetSysHandle *)Cont,
            PIS_ConvertKVList2PIKVList(*UseSettings));

    return PrivData;
}

/*******************************************************************************
 * NAME:
 *    DPS_FindPluginSetting
 *
 * SYNOPSIS:
 *    static struct PluginSettings *DPS_FindPluginSetting(const char *IDStr,
 *              class ConSettings *Settings);
 *
 * PARAMETERS:
 *    IDStr [I] -- The ID string for this processor to lookup
 *    Settings [I] -- The connection settings to search.
 *
 * FUNCTION:
 *    This function finds a plugins settings in connection settings.
 *
 * RETURNS:
 *    A pointer to the connection settings or NULL if it was not found.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static struct PluginSettings *DPS_FindPluginSetting(const char *IDStr,
        class ConSettings *Settings)
{
    i_PluginSettings plug;
    for(plug=Settings->PluginsSettings.begin();
            plug!=Settings->PluginsSettings.end();plug++)
    {
        if(strcmp(IDStr,plug->IDStr.c_str())==0)
        {
            /* Found */
            return &*plug;
        }
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    DPS_PluginSettings_FreeWidgets
 *
 * SYNOPSIS:
 *    void DPS_PluginSettings_FreeWidgets(const char *IDStr,
 *              t_DataProSettingsWidgetsType *PrivData);
 *
 * PARAMETERS:
 *    IDStr [I] -- The ID string for this processor to lookup
 *    PrivData [I] -- The plugins private data that was allocated with
 *                    DPS_PluginSettings_AddWidgets()
 *
 * FUNCTION:
 *    This function tells the plugin to free the widgets it allocated with
 *    DPS_PluginSettings_AddWidgets()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_PluginSettings_AddWidgets()
 ******************************************************************************/
void DPS_PluginSettings_FreeWidgets(const char *IDStr,t_DataProSettingsWidgetsType *PrivData)
{
    const struct DataProcessor *ProData;

    ProData=DPS_GetProcessorsInfo(IDStr);
    if(ProData==NULL || ProData->API.FreeSettingsWidgets==NULL)
        return;

    ProData->API.FreeSettingsWidgets(PrivData);
}

/*******************************************************************************
 * NAME:
 *    DPS_PluginSettings_SetSettingsFromWidgets
 *
 * SYNOPSIS:
 *    void DPS_PluginSettings_SetSettingsFromWidgets(
 *          class ConSettings *Settings,const char *IDStr,
 *          t_DataProSettingsWidgetsType *PrivData);
 *
 * PARAMETERS:
 *    Settings [I] -- The settings with the plugin settings in it.
 *    IDStr [I] -- The ID string for this processor to lookup
 *    PrivData [I] -- The plugins private data that was allocated with
 *                    DPS_PluginSettings_AddWidgets()
 *
 * FUNCTION:
 *    This function tells the plugin to grab it's settings from the GUI into
 *    it's settings.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_PluginSettings_AddWidgets()
 ******************************************************************************/
void DPS_PluginSettings_SetSettingsFromWidgets(class ConSettings *Settings,
        const char *IDStr,t_DataProSettingsWidgetsType *PrivData)
{
    const struct DataProcessor *ProData;
    struct PluginSettings *PlugSettings;
    struct PluginSettings NewPluginSettings;

    try
    {
        ProData=DPS_GetProcessorsInfo(IDStr);
        if(ProData==NULL || ProData->API.SetSettingsFromWidgets==NULL)
            return;

        /* We need to find the settings for this plugin */
        PlugSettings=DPS_FindPluginSetting(IDStr,Settings);
        if(PlugSettings==NULL)
        {
            /* Ok, we don't have settings for this plugin yet add one */
            NewPluginSettings.IDStr=IDStr;
            Settings->PluginsSettings.push_back(NewPluginSettings);
            PlugSettings=&Settings->PluginsSettings.back();
        }

        ProData->API.SetSettingsFromWidgets(PrivData,
                PIS_ConvertKVList2PIKVList(PlugSettings->Settings));
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    DPS_PrunePluginSettings
 *
 * SYNOPSIS:
 *    void DPS_PrunePluginSettings(class ConSettings *Settings);
 *
 * PARAMETERS:
 *    Settings [I] -- The settings to prune.
 *
 * FUNCTION:
 *    This function will go though the list of plugin settings and delete
 *    any plugins that are no longer installed.  This is to keep the
 *    settings from just continuously growing.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DPS_PrunePluginSettings(class ConSettings *Settings)
{
    i_PluginSettings plug;
    i_PluginSettings delme;
    i_DPSDataProcessorsType CurProcessor;

    /* Go though all the settings and see if we can find the plugin */
    for(plug=Settings->PluginsSettings.begin();
            plug!=Settings->PluginsSettings.end();)
    {
        for(CurProcessor=m_DataProcessors.begin();
                CurProcessor!=m_DataProcessors.end();CurProcessor++)
        {
            if(CurProcessor->ProID==plug->IDStr)
                break;
        }
        if(CurProcessor==m_DataProcessors.end())
        {
            /* This plugin wasn't found, delete it */
            delme=plug;
            plug++;
            Settings->PluginsSettings.erase(delme);
            continue;
        }
        plug++;
    }
}

void DPS_PluginSettings_Load(class ConSettings *Settings,const char *IDStr)
{
//    const struct DataProcessor *ProData;
//    struct PluginSettings *PlugSettings;
//    struct PluginSettings NewPluginSettings;
//
//    try
//    {
//        ProData=DPS_GetProcessorsInfo(IDStr);
//        if(ProData==NULL || ProData->API.LoadSettings==NULL)
//            return;
//
//        /* We need to find the settings for this plugin */
//        PlugSettings=DPS_FindPluginSetting(IDStr,Settings);
//        if(PlugSettings==NULL)
//            return;
//
//        ProData->API.LoadSettings(FData->ProcessorsData[Index],PIS_ConvertKVList2PIKVList(PlugSettings->
//                Settings));
//    }
//    catch(...)
//    {
//    }
}

/*******************************************************************************
 * NAME:
 *    DPS_InformOfNewPluginInstalled
 *
 * SYNOPSIS:
 *    void DPS_InformOfNewPluginInstalled(const char *PluginIDStr);
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
void DPS_InformOfNewPluginInstalled(const char *PluginIDStr)
{
    /* We apply the settings so that if there are any open connections that
       had a plugin in listed but it wasn't installed / enabled it will
       reeval it and enable it */
    ApplySettings();
}

/*******************************************************************************
 * NAME:
 *    DPS_InformOfPluginUninstalled
 *
 * SYNOPSIS:
 *    void DPS_InformOfPluginUninstalled(const char *PluginIDStr);
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
void DPS_InformOfPluginUninstalled(const char *PluginIDStr)
{
    i_DPSDataProcessorsType pro;

    /* Remove from the list of available data processors */
    for(pro=m_DataProcessors.begin();pro!=m_DataProcessors.end();pro++)
        if(pro->ProID==PluginIDStr)
            break;
    if(pro!=m_DataProcessors.end())
    {
        m_DataProcessors.erase(pro);
        UnRegisterPluginWithSystem(PluginIDStr);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*******************************************************************************
 * NAME:
 *    DPS_SetCurrentSettingsTabName
 *
 * SYNOPSIS:
 *    static void DPS_SetCurrentSettingsTabName(const char *Name);
 *
 * PARAMETERS:
 *    Name [I] -- The new name for the tab.
 *
 * FUNCTION:
 *    This function is used as part of the settings for the plugin.  It changes
 *    the settings dialogs current tabs name.  When the settings dialog is
 *    first allocated the default tab will have a generic name and this
 *    function lets the plugin change it's name.
 *
 * RETURNS:
 *    NONE
 *
 * LIMITATIONS:
 *    It is only valid to call this when in AllocSettingsWidgets() or a call
 *    back from one of the widgets that was allocated in AllocSettingsWidgets().
 *
 * SEE ALSO:
 *    DPS_AddNewSettingsTab()
 ******************************************************************************/
static void DPS_SetCurrentSettingsTabName(const char *Name)
{
    if(DPS_PS_GuiCtrlFn!=NULL)
    {
        DPS_PS_GuiCtrlFn(e_DataProPlugSettingsFn_SetCurrentTabName,(void *)Name,
                NULL);
    }
}

/*******************************************************************************
 * NAME:
 *    DPS_AddNewSettingsTab
 *
 * SYNOPSIS:
 *    static t_WidgetSysHandle *DPS_AddNewSettingsTab(const char *Name);
 *
 * PARAMETERS:
 *    Name [I] -- The name for the new tab.
 *
 * FUNCTION:
 *    This function adds a new tab to the plugin settings dialog.
 *
 * RETURNS:
 *    A new handle for adding widgets or NULL if there was an error.  This is
 *    no need to free this handle.
 *
 * NOTES:
 *    You must use the handle returned from this function with any widgets
 *    added from this point until we add a new tab.  That is to says when you
 *    call a UI function to access a widget you must pass the t_WidgetSysHandle
 *    that was active when you added the widget.
 *
 *    For example:
 *      AllocSettingsWidgets(t_WidgetSysHandle *StartingHandle)
 *      {
 *          t_WidgetSysHandle *NewHandle;
 *
 *          Check1=UI->AddCheckbox(StartingHandle,"One",NULL,NULL);
 *          NewHandle=API->AddNewSettingsTab("Tab2");
 *          Check2=UI->AddCheckbox(NewHandle,"Two",NULL,NULL);
 *
 *          // You must use 'StartingHandle' when accessing Check1
 *          UI->SetCheckboxChecked(StartingHandle,Check1,true);
 *
 *          // You must use 'NewHandle' when accessing Check2
 *          UI->SetCheckboxChecked(NewHandle,Check2,true);
 *      }
 *
 * SEE ALSO:
 *    DPS_SetCurrentSettingsTabName()
 ******************************************************************************/
static t_WidgetSysHandle *DPS_AddNewSettingsTab(const char *Name)
{
    if(DPS_PS_GuiCtrlFn==NULL)
        return NULL;

    return (t_WidgetSysHandle *)DPS_PS_GuiCtrlFn(
            e_DataProPlugSettingsFn_AddNewTab,(void *)Name,NULL);
}

/*******************************************************************************
 * NAME:
 *    DPS_SetFGColor
 *
 * SYNOPSIS:
 *    void DPS_SetFGColor(uint32_t FGColor);
 *
 * PARAMETERS:
 *    FGColor [I] -- The new forground color in the format 0xRRGGBB
 *
 * FUNCTION:
 *    This function sets the forground color.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_GetFGColor()
 ******************************************************************************/
void DPS_SetFGColor(uint32_t FGColor)
{
    Con_SetFGColor(FGColor);
}

/*******************************************************************************
 * NAME:
 *    DPS_GetFGColor
 *
 * SYNOPSIS:
 *    uint32_t DPS_GetFGColor(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the forground color.
 *
 * RETURNS:
 *    The forgound color in 0xRRGGBB format.
 *
 * SEE ALSO:
 *    DPS_SetFGColor()
 ******************************************************************************/
uint32_t DPS_GetFGColor(void)
{
    return Con_GetFGColor();
}

/*******************************************************************************
 * NAME:
 *    DPS_SetBGColor
 *
 * SYNOPSIS:
 *    void DPS_SetBGColor(uint32_t BGColor);
 *
 * PARAMETERS:
 *    BGColor [I] -- The new background color in the format 0xRRGGBB
 *
 * FUNCTION:
 *    This function sets the background color.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_GetBGColor()
 ******************************************************************************/
void DPS_SetBGColor(uint32_t BGColor)
{
    Con_SetBGColor(BGColor);
}

/*******************************************************************************
 * NAME:
 *    DPS_GetBGColor
 *
 * SYNOPSIS:
 *    uint32_t DPS_GetBGColor(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the background color.
 *
 * RETURNS:
 *    The background color in 0xRRGGBB format.
 *
 * SEE ALSO:
 *    DPS_SetBGColor()
 ******************************************************************************/
uint32_t DPS_GetBGColor(void)
{
    return Con_GetBGColor();
}

/*******************************************************************************
 * NAME:
 *    DPS_SetULineColor
 *
 * SYNOPSIS:
 *    void DPS_SetULineColor(uint32_t ULineColor);
 *
 * PARAMETERS:
 *    ULineColor [I] -- The new underline color in the format 0xRRGGBB
 *
 * FUNCTION:
 *    This function sets the underline color.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_GetULineColor()
 ******************************************************************************/
void DPS_SetULineColor(uint32_t ULineColor)
{
    Con_SetULineColor(ULineColor);
}

/*******************************************************************************
 * NAME:
 *    DPS_GetULineColor
 *
 * SYNOPSIS:
 *    uint32_t DPS_GetULineColor(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the underline color.
 *
 * RETURNS:
 *    The underline color in 0xRRGGBB format.
 *
 * SEE ALSO:
 *    DPS_SetULineColor()
 ******************************************************************************/
uint32_t DPS_GetULineColor(void)
{
    return Con_GetULineColor();
}

/*******************************************************************************
 * NAME:
 *    DPS_SetAttribs
 *
 * SYNOPSIS:
 *    void DPS_SetAttribs(uint32_t Attribs);
 *
 * PARAMETERS:
 *    Attribs [I] -- The new attribs to use.  These are bit values where
 *                   the following bits are supported:
 *                      TXT_ATTRIB_UNDERLINE -- Underline the text
 *                      TXT_ATTRIB_UNDERLINE_DOUBLE -- Double underline the text
 *                      TXT_ATTRIB_UNDERLINE_DOTTED -- Dotted underline
 *                      TXT_ATTRIB_UNDERLINE_DASHED -- Dashed underline.  This
 *                              is not effected by turning off general
 *                              underlining.
 *                      TXT_ATTRIB_UNDERLINE_WAVY -- A wavy underline.  This
 *                              is not effected by turning off general
 *                              underlining.
 *                      TXT_ATTRIB_OVERLINE -- Put a line over the text
 *                      TXT_ATTRIB_LINETHROUGHT -- Put a line though the text
 *                      TXT_ATTRIB_BOLD -- Bold Text
 *                      TXT_ATTRIB_ITALIC -- Italic Text
 *                      TXT_ATTRIB_OUTLINE -- Draw an outline around the leters.
 *                      TXT_ATTRIB_REVERSE -- Reverse video
 *
 * FUNCTION:
 *    This function sets the underline color.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_GetAttribs()
 ******************************************************************************/
void DPS_SetAttribs(uint32_t Attribs)
{
    Con_SetAttribs(Attribs);
}

/*******************************************************************************
 * NAME:
 *    DPS_GetAttribs
 *
 * SYNOPSIS:
 *    uint32_t DPS_GetAttribs(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the currently applied attribs.  These are bit values.
 *
 * RETURNS:
 *    The attribs that are currently turned on.
 *
 * SEE ALSO:
 *    DPS_SetAttribs()
 ******************************************************************************/
uint32_t DPS_GetAttribs(void)
{
    return Con_GetAttribs();
}

/*******************************************************************************
 * NAME:
 *    DPS_DoNewLine
 *
 * SYNOPSIS:
 *    void DPS_DoNewLine(void);
 *
 * PARAMETERS:
 *    NONE
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
void DPS_DoNewLine(void)
{
    Con_DoFunction(e_ConFunc_NewLine);
}

/*******************************************************************************
 * NAME:
 *    DPS_DoReturn
 *
 * SYNOPSIS:
 *    void DPS_DoReturn(void);
 *
 * PARAMETERS:
 *    NONE
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
void DPS_DoReturn(void)
{
    Con_DoFunction(e_ConFunc_Return);
}

/*******************************************************************************
 * NAME:
 *    DPS_DoBackspace
 *
 * SYNOPSIS:
 *    void DPS_DoBackspace(void);
 *
 * PARAMETERS:
 *    NONE
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
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_SendBackspace()
 ******************************************************************************/
void DPS_DoBackspace(void)
{
    Con_DoFunction(e_ConFunc_Backspace);
}

/*******************************************************************************
 * NAME:
 *    DPS_SetCursorXY
 *
 * SYNOPSIS:
 *    void DPS_SetCursorXY(uint32_t X,uint32_t Y);
 *
 * PARAMETERS:
 *    X [I] -- The new X pos.  DEBUG PAUL: Add doc about if this is from 0 or 1
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
void DPS_SetCursorXY(uint32_t X,uint32_t Y)
{
    Con_DoFunction(e_ConFunc_MoveCursor,X,Y);
}

/*******************************************************************************
 * NAME:
 *    DPS_DoClearScreen
 *
 * SYNOPSIS:
 *    void DPS_DoClearScreen(void);
 *
 * PARAMETERS:
 *    NONE
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
void DPS_DoClearScreen(void)
{
    Con_DoFunction(e_ConFunc_ClearScreen);
}

/*******************************************************************************
 * NAME:
 *    DPS_DoClearScreenAndBackBuffer
 *
 * SYNOPSIS:
 *    void DPS_DoClearScreenAndBackBuffer(void);
 *
 * PARAMETERS:
 *    NONE
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
void DPS_DoClearScreenAndBackBuffer(void)
{
    Con_DoFunction(e_ConFunc_ClearScreenAndBackBuffer);
}

/*******************************************************************************
 * NAME:
 *    DPS_DoClearArea
 *
 * SYNOPSIS:
 *    void DPS_DoClearArea(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2);
 *
 * PARAMETERS:
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
void DPS_DoClearArea(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2)
{
    Con_DoFunction(e_ConFunc_ClearArea,X1,Y1,X2,Y2);
}

/*******************************************************************************
 * NAME:
 *    DPS_GetCursorXY
 *
 * SYNOPSIS:
 *    void DPS_GetCursorXY(int32_t *RetCursorX,int32_t *RetCursorY);
 *
 * PARAMETERS:
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
void DPS_GetCursorXY(int32_t *RetCursorX,int32_t *RetCursorY)
{
    Con_GetCursorXY(RetCursorX,RetCursorY);
}

/*******************************************************************************
 * NAME:
 *    DPS_InsertString
 *
 * SYNOPSIS:
 *    void DPS_InsertString(uint8_t *Str,uint32_t Len);
 *
 * PARAMETERS:
 *    Str [I] -- The string to add (UTF8)
 *    Len [I] -- The number of chars in 'Str'.  Note chars are utf8 chars not
 *               bytes.  So if you have "0xE2 0x98 0x82 0x33" in 'str' that is
 *               only 2 chars but 4 bytes, so the correct value for 'Len' is 2.
 *
 * FUNCTION:
 *    This function adds a utf8 string to the display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DPS_InsertString(uint8_t *Str,uint32_t Len)
{
    Con_InsertString(Str,Len);
}

void DPS_GetScreenSize(int32_t *RetRows,int32_t *RetColumns)
{
    Con_GetScreenSize(RetRows,RetColumns);
}

/*******************************************************************************
 * NAME:
 *    DPS_GetSysColor
 *
 * SYNOPSIS:
 *    uint32_t DPS_GetSysColor(int32_t SysColShade,int32_t SysColor);
 *
 * PARAMETERS:
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
uint32_t DPS_GetSysColor(uint32_t SysColShade,uint32_t SysColor)
{
//    if(SysColShade>=e_SysColShadeMAX)
//        return 0;
//    if(SysColor>=e_SysColMAX)
//        return 0;
//
//    return g_Settings.DefaultConSettings.SysColors[SysColShade][SysColor];
    return Con_GetSysColor((e_SysColShadeType)SysColShade,(e_SysColType)SysColor);
}

/*******************************************************************************
 * NAME:
 *    DPS_GetSysDefaultColor
 *
 * SYNOPSIS:
 *    uint32_t DPS_GetSysDefaultColor(uint32_t DefaultColor);
 *
 * PARAMETERS:
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
uint32_t DPS_GetSysDefaultColor(uint32_t DefaultColor)
{
    return Con_GetSysDefaultColor((e_DefaultColorsType)DefaultColor);
//    if(DefaultColor>=e_DefaultColorsMAX)
//        return 0;
////`
//    return g_Settings.DefaultConSettings.DefaultColors[DefaultColor];
}

/*******************************************************************************
 * NAME:
 *    DPS_NoteNonPrintable
 *
 * SYNOPSIS:
 *    void DPS_NoteNonPrintable(const char *CodeStr);
 *
 * PARAMETERS:
 *    CodeStr [I] -- The string to add to the display when the user has
 *                   show non-printable control codes turned on.  This should
 *                   be kept short as it will be drawen inline with the
 *                   normal text.  (for example in AscII the BEL char would
 *                   show <bel>)
 *
 * FUNCTION:
 *    This function adds a decoded non-printable char.  When something is
 *    decoded that does not result in a printable char this function can be
 *    used to add a note about the char (which the user can show or hide)
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DPS_NoteNonPrintable(const char *CodeStr)
{
    Con_DoFunction(e_ConFunc_NoteNonPrintable,(uintptr_t)CodeStr);
}

/*******************************************************************************
 * NAME:
 *    DPS_DoTab
 *
 * SYNOPSIS:
 *    void DPS_DoTab(void);
 *
 * PARAMETERS:
 *    NONE
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
void DPS_DoTab(void)
{
    Con_DoFunction(e_ConFunc_Tab);
}

/*******************************************************************************
 * NAME:
 *    DPS_DoPrevTab
 *
 * SYNOPSIS:
 *    void DPS_DoPrevTab(void);
 *
 * PARAMETERS:
 *    NONE
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
void DPS_DoPrevTab(void)
{
    Con_DoFunction(e_ConFunc_PrevTab);
}

/*******************************************************************************
 * NAME:
 *    DPS_SendBackspace
 *
 * SYNOPSIS:
 *    void DPS_SendBackspace(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function send the backspace char.  What the backspace char is
 *    depends on what the user has selected.
 *
 *    This writes a backspace char (as if it was typed on the keyboard),
 *    unlike DPS_DoBackspace() which does (acks) the backspace.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_DoBackspace()
 ******************************************************************************/
void DPS_SendBackspace(void)
{
    Con_DoFunction(e_ConFunc_SendBackspace);
}

/*******************************************************************************
 * NAME:
 *    DPS_SendEnter
 *
 * SYNOPSIS:
 *    void DPS_SendEnter(void);
 *
 * PARAMETERS:
 *    NONE
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
void DPS_SendEnter(void)
{
    Con_DoFunction(e_ConFunc_SendEnter);
}

/*******************************************************************************
 * NAME:
 *    DPS_BinaryAddText
 *
 * SYNOPSIS:
 *    void DPS_BinaryAddText(const char *Str);
 *
 * PARAMETERS:
 *    Str [I] -- The string to add to the text display
 *
 * FUNCTION:
 *    This function is called from ProcessIncomingBinaryByte() to add
 *    a text to the text display.  If this plugin is not a
 *    'e_BinaryDataProcessorMode_Text' then calls to this function are ignored.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_BinaryAddHex()
 ******************************************************************************/
void DPS_BinaryAddText(const char *Str)
{
    uint8_t buff[2];

    if(m_ActiveDataProcessor==NULL)
        return;

    if(m_ActiveDataProcessor->Info.BinMode!=e_BinaryDataProcessorMode_Text)
        return;

    /* Only decoders can add text (or hex) */
    if(m_ActiveDataProcessor->Info.BinClass!=e_BinaryDataProcessorClass_Decoder)
        return;

    buff[1]=0;
    while(*Str!=0)
    {
        buff[0]=*Str;
        Str++;
        Con_WriteChar2Display(buff);
    }
}

/*******************************************************************************
 * NAME:
 *    DPS_BinaryAddHex
 *
 * SYNOPSIS:
 *    void DPS_BinaryAddHex(uint8_t Byte);
 *
 * PARAMETERS:
 *    Byte [I] -- The byte to add to the hex display.
 *
 * FUNCTION:
 *    This function is called from ProcessIncomingBinaryByte() to add
 *    a hex value to the display.  If this plugin is not a
 *    'e_BinaryDataProcessorMode_Hex' then calls to this function are ignored.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_BinaryAddText()
 ******************************************************************************/
void DPS_BinaryAddHex(uint8_t Byte)
{
    uint8_t buff[2];

    if(m_ActiveDataProcessor==NULL)
        return;

    if(m_ActiveDataProcessor->Info.BinMode!=e_BinaryDataProcessorMode_Hex)
        return;

    /* Only decoders can add hex (or text) */
    if(m_ActiveDataProcessor->Info.BinClass!=e_BinaryDataProcessorClass_Decoder)
        return;

    buff[0]=Byte;
    buff[1]=0;

    Con_WriteChar2Display(buff);
}

/*******************************************************************************
 * NAME:
 *    DPS_DoSystemBell
 *
 * SYNOPSIS:
 *    void DPS_DoSystemBell(int VisualOnly);
 *
 * PARAMETERS:
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
void DPS_DoSystemBell(int VisualOnly)
{
    Con_DoBell(VisualOnly);
}

/*******************************************************************************
 * NAME:
 *    DPS_DoScrollArea
 *
 * SYNOPSIS:
 *    void DPS_DoScrollArea(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2,
 *              int32_t DeltaX,int32_t DeltaY);
 *
 * PARAMETERS:
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
void DPS_DoScrollArea(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2,
        int32_t DeltaX,int32_t DeltaY)
{
    Con_DoFunction(e_ConFunc_ScrollArea,X1,Y1,X2,Y2,DeltaX,DeltaY);
}

/*******************************************************************************
 * NAME:
 *    DPS_SetTitle
 *
 * SYNOPSIS:
 *    void DPS_SetTitle(const char *Title);
 *
 * PARAMETERS:
 *    Title [I] -- The new title for the window.
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
void DPS_SetTitle(const char *Title)
{
    Con_SetTitle(Title);
}

/*******************************************************************************
 * NAME:
 *    DPS_AllocateMark
 *
 * SYNOPSIS:
 *    t_DataProMark *DPS_AllocateMark(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates a new mark.  A mark is a point in the data that
 *    the system is currently inserting bytes.  You can use it to remember
 *    the current point in the stream and apply changes after.
 *
 *    Because this mark is a point of older data it may be overwritten,
 *    destroyed or removed, in this case then the mark becomes invalid and
 *    you can no longer use it.  You still need to free the mark even if
 *    it becomes invalid.
 *
 *    The new marker will be set to the current cursor position.
 *
 * RETURNS:
 *    A pointer to the mark in the data.
 *
 * SEE ALSO:
 *    DPS_FreeMark(), DPS_IsMarkValid(), DPS_SetMark2CursorPos(),
 *    DPS_ApplyAttrib2Mark(), DPS_RemoveAttribFromMark(),
 *    DPS_ApplyFGColor2Mark(), DPS_ApplyBGColor2Mark(), DPS_MoveMark()
 ******************************************************************************/
t_DataProMark *DPS_AllocateMark(void)
{
    return Con_AllocateMark();
}

/*******************************************************************************
 * NAME:
 *    DPS_FreeMark
 *
 * SYNOPSIS:
 *    void DPS_FreeMark(t_DataProMark *Mark);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to free
 *
 * FUNCTION:
 *    This function frees the mark allocated with DPS_AllocateMark().  You
 *    do not need to call this on your mark's when your plugin is shutting
 *    down.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_AllocateMark()
 ******************************************************************************/
void DPS_FreeMark(t_DataProMark *Mark)
{
    Con_FreeMark(Mark);
}

/*******************************************************************************
 * NAME:
 *    DPS_IsMarkValid
 *
 * SYNOPSIS:
 *    PG_BOOL DPS_IsMarkValid(t_DataProMark *Mark);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *
 * FUNCTION:
 *    This function checks if a mark is still valid.  The system can move
 *    things around after you set a mark and it will make a mark invalid.
 *    For example if the user clears the screen any marks you have will be
 *    marked invalid.
 *
 * RETURNS:
 *    true -- Mark is still value
 *    false -- The mark is no longer valid
 *
 * SEE ALSO:
 *    DPS_AllocateMark()
 ******************************************************************************/
PG_BOOL DPS_IsMarkValid(t_DataProMark *Mark)
{
    return Con_IsMarkValid(Mark);
}

/*******************************************************************************
 * NAME:
 *    DPS_SetMark2CursorPos
 *
 * SYNOPSIS:
 *    void DPS_SetMark2CursorPos(t_DataProMark *Mark);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *
 * FUNCTION:
 *    This function will take a mark and move it to the current cursor position.
 *    It will also set this mark to valid.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_AllocateMark()
 ******************************************************************************/
void DPS_SetMark2CursorPos(t_DataProMark *Mark)
{
    return Con_SetMark2CursorPos(Mark);
}

/*******************************************************************************
 * NAME:
 *    DPS_ApplyAttrib2Mark
 *
 * SYNOPSIS:
 *    void DPS_ApplyAttrib2Mark(t_DataProMark *Mark,uint32_t Attrib,
 *              uint32_t Offset,uint32_t Len);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *    Attrib [I] -- The new attrib(s) to set
 *    Offset [I] -- The number of chars from the mark to skip before starting
 *                  to apply the attribs.
 *    Len [I] -- The number of chars to apply these new attributes to.  Pass
 *               0 to apply until the cursor.
 *
 * FUNCTION:
 *    This function takes and sets a attrib (or more than one) between the
 *    mark and the cursor.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_AllocateMark()
 ******************************************************************************/
void DPS_ApplyAttrib2Mark(t_DataProMark *Mark,uint32_t Attrib,uint32_t Offset,
        uint32_t Len)
{
    Con_ApplyAttrib2Mark(Mark,Attrib,Offset,Len);
}

/*******************************************************************************
 * NAME:
 *    DPS_RemoveAttribFromMark
 *
 * SYNOPSIS:
 *    void DPS_RemoveAttribFromMark(t_DataProMark *Mark,uint32_t Attrib,
 *              uint32_t Offset,uint32_t Len);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *    Attrib [I] -- The new attrib(s) to clear
 *    Offset [I] -- The number of chars from the mark to skip before starting
 *                  to remove the attribs.
 *    Len [I] -- The number of chars to remove these attributes from.  Pass
 *               0 to apply until the cursor.
 *
 * FUNCTION:
 *    This function takes and clears a attrib (or more than one) between the
 *    mark and the cursor.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_AllocateMark()
 ******************************************************************************/
void DPS_RemoveAttribFromMark(t_DataProMark *Mark,uint32_t Attrib,
        uint32_t Offset,uint32_t Len)
{
    Con_RemoveAttribFromMark(Mark,Attrib,Offset,Len);
}

/*******************************************************************************
 * NAME:
 *    DPS_ApplyFGColor2Mark
 *
 * SYNOPSIS:
 *    void DPS_ApplyFGColor2Mark(t_DataProMark *Mark,uint32_t FGColor,
 *              uint32_t Offset,uint32_t Len);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *    FGColor [I] -- The colors to apply
 *    Offset [I] -- The number of chars from the mark to skip before starting
 *                  to apply the color.
 *    Len [I] -- The number of chars to apply this color to.  Pass
 *               0 to apply until the cursor.
 *
 * FUNCTION:
 *    This function takes and colors between the mark and the cursor.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_AllocateMark()
 ******************************************************************************/
void DPS_ApplyFGColor2Mark(t_DataProMark *Mark,uint32_t FGColor,
        uint32_t Offset,uint32_t Len)
{
    Con_ApplyFGColor2Mark(Mark,FGColor,Offset,Len);
}

/*******************************************************************************
 * NAME:
 *    DPS_ApplyBGColor2Mark
 *
 * SYNOPSIS:
 *    void DPS_ApplyBGColor2Mark(t_DataProMark *Mark,uint32_t FGColor,
 *              uint32_t Offset,uint32_t Len);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *    FGColor [I] -- The colors to apply
 *    Offset [I] -- The number of chars from the mark to skip before starting
 *                  to apply the color.
 *    Len [I] -- The number of chars to apply this color to.  Pass
 *               0 to apply until the cursor.
 *
 * FUNCTION:
 *    This function takes and colors between the mark and the cursor.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_AllocateMark()
 ******************************************************************************/
void DPS_ApplyBGColor2Mark(t_DataProMark *Mark,uint32_t BGColor,uint32_t Offset,
        uint32_t Len)
{
    Con_ApplyBGColor2Mark(Mark,BGColor,Offset,Len);
}

/*******************************************************************************
 * NAME:
 *    DPS_MoveMark
 *
 * SYNOPSIS:
 *    void DPS_MoveMark(t_DataProMark *Mark,int Amount);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *    Amount [I] -- How much to move the mark by (plus for toward the cursor,
 *                  neg to move toward the start of the buffer).
 *
 * FUNCTION:
 *    This function moves the point that a mark points to.  This is moved
 *    by char's (or bytes if binary) and it moves in the post processing
 *    text (after all the other plugin have modified the text).
 *
 *    The mark is not be moved past the cursor (or insert point) or back before
 *    the start of the screen / data.  It will just clip the move.  So for
 *    example if you told the system to move the mark 1000000 chars (and there
 *    aren't over 1000000 chars) then the mark will be moved to the cursor
 *    and stop moving.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_AllocateMark()
 ******************************************************************************/
void DPS_MoveMark(t_DataProMark *Mark,int Amount)
{
    Con_MoveMark(Mark,Amount);
}

/*******************************************************************************
 * NAME:
 *    DPS_GetMarkString
 *
 * SYNOPSIS:
 *    static const uint8_t *DPS_GetMarkString(t_DataProMark *Mark,
 *              uint32_t *Size,uint32_t Offset,uint32_t Len);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *    Size [O] -- The number of bytes in the returned buffer.
 *    Offset [I] -- The number of chars from the mark to skip before starting
 *                  to copy the string.
 *    Len [I] -- The number of chars to copy from this string.  Pass 0 for
 *               all the bytes between Mark+Offset to the cursor.
 *
 * FUNCTION:
 *    Gets a copy of the bytes between a mark and the cursor.
 *
 * RETURNS:
 *    A buffer with the data from the mark to the cursor in it.  This buffer
 *    is valid until the connection is modified.  So you should consider this
 *    buffer invalid after you call another function that works on the
 *    connection or when your function goes out of scope.  This function will
 *    also return NULL if there was an error (or the mark is invalid).
 *
 * NOTES:
 *    The "string" that is returned is not really a string, but instead of
 *    buffer with the string in it.  This means that the "string" can have
 *    \0's in it (if this is a binary connection for example).
 *
 *    Although the returned buffer is not a real c-string, the buffer will
 *    actually be (*Size)+1, and a \0 will be added to the end.  This is so
 *    when you are text based plugin you can just grab the result and
 *    do string operations on it.  The binary system will also add this zero
 *    past the end but you shouldn't use it.
 *
 *    Also note that if the stream is gets frozen then this will not include
 *    any of the frozen data until the stream is unfrozen (and will not include
 *    any of the frozen data if it is cleared).
 *
 * SEE ALSO:
 *    DPS_GetFrozenString()
 ******************************************************************************/
static const uint8_t *DPS_GetMarkString(t_DataProMark *Mark,uint32_t *Size,
        uint32_t Offset,uint32_t Len)
{
    return Con_GetMarkString(Mark,Size,Offset,Len);
}

/*******************************************************************************
 * NAME:
 *    DPS_FreezeStream
 *
 * SYNOPSIS:
 *    void DPS_FreezeStream(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function freezes the display stream.  When a connection has a
 *    frozen stream any writes to the screen are stopped and the text
 *    that would be added is queued instead.  When you release the
 *    freeze then all the chars are added to the display at that point.
 *
 *    This is usefull if you want to erase text from the incoming stream
 *    but don't want the text to flash infront of the user (as it's drawn
 *    and then erased later).
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    You must have a matching call to DPS_ReleaseFrozenStream() or the
 *    connection will not be able to display any text.
 *
 * SEE ALSO:
 *    DPS_ReleaseFrozenStream(), DPS_ClearFrozenStream()
 ******************************************************************************/
void DPS_FreezeStream(void)
{
    Con_FreezeStream();
}

/*******************************************************************************
 * NAME:
 *    DPS_ReleaseFrozenStream
 *
 * SYNOPSIS:
 *    void DPS_ReleaseFrozenStream(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function releases a lock added to a display stream that was
 *    added to DPS_FreezeStream()
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    Calls to DPS_FreezeStream() must match the calls to DPS_FreezeStream().
 *    If you call more times than you called DPS_FreezeStream() you will mess
 *    with other plugins.
 *
 * SEE ALSO:
 *    DPS_FreezeStream()
 ******************************************************************************/
void DPS_ReleaseFrozenStream(void)
{
    Con_ReleaseFrozenStream();
}

/*******************************************************************************
 * NAME:
 *    DPS_ClearFrozenStream
 *
 * SYNOPSIS:
 *    void DPS_ClearFrozenStream(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clears the queue that stores the frozen chars that would
 *    have been added when DPS_ReleaseFrozenStream() is called.
 *
 * RETURNS:
 *    NONE
 *
 * LIMITATIONS:
 *    This function currently clears all chars in the pending queue.  What this
 *    means is that if plugin A freezes the stream some chars are added and
 *    plugin B freezes the stream and then clears the stream then all the
 *    chars in the queue are erased including the chars that plugin A froze.
 *
 *    If this becomes an issue then the frozen system will need to reworked
 *    to note who locked the stream and let them only effect the queued
 *    chars from that plugin.
 *
 * SEE ALSO:
 *    DPS_FreezeStream()
 ******************************************************************************/
void DPS_ClearFrozenStream(void)
{
    Con_ClearFrozenStream();
}

/*******************************************************************************
 * NAME:
 *    DPS_GetFrozenString
 *
 * SYNOPSIS:
 *    const uint8_t *DPS_GetFrozenString(uint32_t *Size);
 *
 * PARAMETERS:
 *    Size [O] -- The number of bytes in the buffer returned.  This does not
 *                include the convenience \0.
 *
 * FUNCTION:
 *    This function gets all bytes that have been collected since the stream
 *    was frozen.
 *
 * RETURNS:
 *    A buffer with all the chars in it that have been frozen.  This includes
 *    a \0 at the end for your convenience.  This will be a UTF-8 string
 *    not the raw bytes.
 *
 *    If the stream is not frozen then this function will return NULL.  You
 *    must always check for NULL as a reset of the connection can unfreeze
 *    the connection.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
const uint8_t *DPS_GetFrozenString(uint32_t *Size)
{
    return Con_GetFrozenString(Size);
}
