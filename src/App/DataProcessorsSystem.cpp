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
void DPS_DoMoveCursor(uint32_t X,uint32_t Y);
void DPS_DoClearScreen(void);
void DPS_DoClearArea(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2);
void DPS_GetCursorXY(int32_t *RetCursorX,int32_t *RetCursorY);
void DPS_InsertString(uint8_t *Str,uint32_t Len);
void DPS_GetScreenSize(int32_t *RetRows,int32_t *RetColumns);
uint32_t DPS_GetSysColor(uint32_t SysColShade,uint32_t SysColor);
uint32_t DPS_GetSysDefaultColor(uint32_t DefaultColor);
void DPS_NoteNonPrintable(const char *CodeStr);
void DPS_DoTab(void);
void DPS_SendBackspace(void);
void DPS_SendEnter(void);
void DPS_BinaryAddText(const char *Str);
void DPS_BinaryAddHex(uint8_t Byte);
void DPS_DoSystemBell(void);
void DPS_DoScrollArea(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2,
        int32_t DeltaX,int32_t DeltaY);

/*** VARIABLE DEFINITIONS     ***/
static struct DataProcessor *m_ActiveDataProcessor;

struct DPS_API g_DPSAPI=
{
    DPS_RegisterDataProcessor,
    PIUSDefault_GetDefaultAPI,
    DPS_SetFGColor,
    DPS_GetFGColor,
    DPS_SetBGColor,
    DPS_GetBGColor,
    DPS_SetULineColor,
    DPS_GetULineColor,
    DPS_SetAttribs,
    DPS_GetAttribs,
    DPS_DoNewLine,
    DPS_DoReturn,
    DPS_DoBackspace,
    DPS_DoMoveCursor,           // DEBUG PAUL: Think about renaming this to DPS_SetCursorXY
    DPS_DoClearScreen,
    DPS_DoClearArea,
    DPS_GetCursorXY,
    DPS_InsertString,
    DPS_GetScreenSize,
    DPS_GetSysColor,
    DPS_GetSysDefaultColor,
    DPS_NoteNonPrintable,
    DPS_DoTab,
    DPS_SendBackspace,
    DPS_SendEnter,
    DPS_BinaryAddText,
    DPS_BinaryAddHex,
    DPS_DoSystemBell,
    DPS_DoScrollArea,
};
t_DPSDataProcessorsType m_DataProcessors;     // All available data processors

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
 *    NAME:
 *      AllocateData
 *
 *    SYNOPSIS:
 *      t_DataProcessorHandleType *AllocateData(void);
 *
 *    PARAMETERS:
 *      NONE
 *
 *    FUNCTION:
 *      This function allocates any needed data for this data processor.
 *
 *    RETURNS:
 *      A pointer to the data, NULL if there was an error.
 *==============================================================================
 *    NAME:
 *      FreeData
 *
 *    SYNOPSIS:
 *      void FreeData(t_DataProcessorHandleType *DataHandle);
 *
 *    PARAMETERS:
 *      DataHandle [I] -- The data handle to free.  This will need to be
 *                        case to your internal data type before you use it.
 *
 *    FUNCTION:
 *      This function frees the memory allocated with AllocateData().
 *
 *    RETURNS:
 *      NONE
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
        }
        FData->ProcessorsData.push_back(NewProcessorData);
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
 *    void DPS_ProcessorIncomingBytes(const uint8_t *inbuff,int bytes);
 *
 * PARAMETERS:
 *    inbuff [I] -- The buffer with the data that was read.  This is raw data
 *    bytes [I] -- The number of bytes that was read.
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
void DPS_ProcessorIncomingBytes(const uint8_t *inbuff,int bytes)
{
    uint8_t ProcessedChar[MAX_BYTES_PER_CHAR+1];  // Buffer for the char we will eventually output.  UTF8 seems to be limited to 4 maybe 6 bytes so 10 should be good (it's up the plugin not to go over 6)
    PG_BOOL Consumed;
    int32_t byte;
    int CharLen;
    i_DPSDataProcessorsType CurProcessor;
    unsigned int Index;
    struct ProcessorConData *FData;

    FData=Con_GetCurrentProcessorData();
    if(FData==NULL)
        return;

    if(FData->Settings->DataProcessorType==e_DataProcessorType_Text)
    {
        /* Text mode data processors */
        for(byte=0;byte<bytes;byte++)
        {
            Consumed=false;
            CharLen=1;
            ProcessedChar[0]=inbuff[byte];

            for(CurProcessor=FData->DataProcessorsList.begin(),Index=0;
                    CurProcessor!=FData->DataProcessorsList.end();
                    CurProcessor++,Index++)
            {
                m_ActiveDataProcessor=&*CurProcessor;
                if(CurProcessor->API.ProcessIncomingTextByte!=NULL)
                {
                    CurProcessor->API.ProcessIncomingTextByte(FData->
                            ProcessorsData[Index],inbuff[byte],ProcessedChar,
                            &CharLen,&Consumed);
                }
            }
            m_ActiveDataProcessor=NULL;

            if(!Consumed)
            {
                ProcessedChar[CharLen]=0;   // Make it a string
DB_StartTimer(e_DBT_AddChar2Display);
                Con_WriteChar2Display(ProcessedChar);
DB_StopTimer(e_DBT_AddChar2Display);
            }
        }
    }
    else
    {
        /* binary data processors */
        CurProcessor=FData->DataProcessorsList.begin();
        if(CurProcessor!=FData->DataProcessorsList.end())
        {
            m_ActiveDataProcessor=&*CurProcessor;
            CharLen=0;
            ProcessedChar[0]=0;
            Consumed=true;
            for(byte=0;byte<bytes;byte++)
            {
                /* Text mode data processors */
                if(CurProcessor->API.ProcessIncomingBinaryByte!=NULL)
                {
                    CurProcessor->API.ProcessIncomingBinaryByte(
                            FData->ProcessorsData[0],inbuff[byte]);
                }
            }
        }
        m_ActiveDataProcessor=NULL;
    }
}

/*******************************************************************************
 * NAME:
 *    DPS_ProcessorKeyPress
 *
 * SYNOPSIS:
 *    bool DPS_ProcessorKeyPress(const uint8_t *KeyChar,int KeyCharLen,
 *              e_UIKeys ExtendedKey,uint8_t Mod);
 *
 * PARAMETERS:
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
bool DPS_ProcessorKeyPress(const uint8_t *KeyChar,int KeyCharLen,
        e_UIKeys ExtendedKey,uint8_t Mod)
{
    i_DPSDataProcessorsType CurProcessor;
    struct ProcessorConData *FData;
    unsigned int Index;
    bool Consumed;

    FData=Con_GetCurrentProcessorData();
    if(FData==NULL)
        return false;

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
 *    DPS_GetListOfBinaryProcessors()
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

            RetData.push_back(NewEntry);
        }
    }
}

/*******************************************************************************
 * NAME:
 *    DPS_GetListOfBinaryProcessors
 *
 * SYNOPSIS:
 *    void DPS_GetListOfBinaryProcessors(t_DPS_ProInfoType &RetData);
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
void DPS_GetListOfBinaryProcessors(t_DPS_ProInfoType &RetData)
{
    i_DPSDataProcessorsType CurProcessor;
    struct DPS_ProInfo NewEntry;

    RetData.clear();

    for(CurProcessor=m_DataProcessors.begin();
            CurProcessor!=m_DataProcessors.end();CurProcessor++)
    {
        if(CurProcessor->Info.ProType==e_DataProcessorType_Binary)
        {
            NewEntry.IDStr=CurProcessor->ProID.c_str();
            NewEntry.DisplayName=CurProcessor->Info.DisplayName;
            NewEntry.Tip=CurProcessor->Info.Tip;
            NewEntry.Help=CurProcessor->Info.Help;

            RetData.push_back(NewEntry);
        }
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
 *                      TXT_ATTRIB_FORCE -- Ignore the user settings and apply
 *                                          all the attributes.
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
 *    DPS_DoMoveCursor
 *
 * SYNOPSIS:
 *    void DPS_DoMoveCursor(uint32_t X,uint32_t Y);
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
void DPS_DoMoveCursor(uint32_t X,uint32_t Y)
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
 *    DPS_DoClearArea
 *
 * SYNOPSIS:
 *    void DPS_DoClearArea(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2);
 *
 * PARAMETERS:
 *    X1 [I] -- The left edge
 *    Y1 [I] -- The top edge
 *    X2 [I] -- The right edge
 *    Y2 [I] -- The bottom edge
 *
 * FUNCTION:
 *    This function clears a box on the screen.  The box may be only one line
 *    high or one row wide.
 *
 *    The line with Y1 and Y2 on it will be cleared.  The column that X1 and
 *    X2 will also be cleared.
 *
 *    So if you send in:
 *          DoClearArea(10,1,12,3);
 *    Then it will clear line 1, 2, and 3 and column 10, 11 and 12.
 *
 *           012345678901234567890
 *          0xxxxxxxxxxxxxxxxxxxxx
 *          1xxxxxxxxxx   xxxxxxxx
 *          2xxxxxxxxxx   xxxxxxxx
 *          3xxxxxxxxxx   xxxxxxxx
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
 *    Len [I] -- The number of bytes in 'Str'
 *
 * FUNCTION:
 *    This function adds a string to the display.
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
 *    This function moves inserts a tab char.  The tab char moves to the
 *    next tab stop.
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

    buff[0]=Byte;
    buff[1]=0;

    Con_WriteChar2Display(buff);
}

/*******************************************************************************
 * NAME:
 *    DPS_DoSystemBell
 *
 * SYNOPSIS:
 *    void DPS_DoSystemBell(void);
 *
 * PARAMETERS:
 *    NONE
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
void DPS_DoSystemBell(void)
{
/* DEBUG PAUL: Do this */
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

