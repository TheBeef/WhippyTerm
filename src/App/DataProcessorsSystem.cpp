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
void DPS_SetAttribs(uint16_t Attribs);
uint16_t DPS_GetAttribs(void);
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

/*** VARIABLE DEFINITIONS     ***/

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
    DPS_DoMoveCursor,
    DPS_DoClearScreen,
    DPS_DoClearArea,
    DPS_GetCursorXY,
    DPS_InsertString,
    DPS_GetScreenSize,
    DPS_GetSysColor,
    DPS_GetSysDefaultColor,
    DPS_NoteNonPrintable,
    DPS_DoTab,
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
 *          ProClass -- This only applies to 'e_DataProcessorType_Text' type
 *              processors. This is what class of text processor is
 *              this.  Supported classes:
 *                      e_DataProcessorClass_Other -- This is a generic class
 *                          more than one of these processors can be active
 *                          at a time but no other requirements exist.
 *                      e_DataProcessorClass_CharEncoding -- This is a
 *                          class that converts the raw stream into some kind
 *                          of char encoding.  For example unicode is converted
 *                          from a number of bytes to chars in the system.
 *                      e_DataProcessorClass_TermEmulation -- This is a
 *                          type of terminal emulator.  An example of a
 *                          terminal emulator is VT100.
 *                      e_DataProcessorClass_Highlighter -- This is a processor
 *                          that highlights strings as they come in the input
 *                          stream.  For example a processor that underlines
 *                          URL's.
 *                      e_DataProcessorClass_Logger -- This is a processor
 *                          that saves the input.  It may save to a file or
 *                          send out a debugging service.  And example is
 *                          a processor that saves all the raw bytes to a file.
 *
 *    RETURNS:
 *      NONE
 *==============================================================================
// *    NAME:
// *      ProcessByte
// *
// *    SYNOPSIS:
// *      void ProcessByte(t_DataProcessorHandleType *DataHandle,t_ConID ConnectionID,
// *              const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
// *              bool *Consumed,struct CharStyling *Style);
// *
// *    PARAMETERS:
// *      DataHandle [I] -- The data handle to work on.  This is your internal
// *                        data.
// *      ConnectionID [I] -- The connection to work on
// *      RawByte [I] -- The raw byte to process.  This is the byte that came in.
// *      ProcessedChar [I/O] -- This is a unicode char that has already been
// *                           processed by some of the other input filters.  You
// *                           can change this as you need.  It must remain only
// *                           one unicode char.
// *      CharLen [I/O] -- This number of bytes in 'ProcessedChar'
// *      Consumed [I/O] -- This tells the system (and other filters) if the
// *                        char has been used up and will not be added to the
// *                        screen.
// *      Style [I/O] -- This is the current style that the char will be added
// *                     with.  You can change this to change how the char will
// *                     be added.
// *
// *    FUNCTION:
// *      This function is called for each byte that comes in.
//DEBUG PAUL: Fill in a lot more info.
// *
// *    RETURNS:
// *      NONE
// *
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
 *      e_DataProcessorClass_TermEmulation only.
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
        {
            if(strcmp(pro->ProID.c_str(),ProID)==0)
                break;
        }
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

    /* Copy the data processors list (based on settings) for this connection */
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

    for(byte=0;byte<bytes;byte++)
    {
        Consumed=false;
        CharLen=1;
        ProcessedChar[0]=inbuff[byte];

        /* Text mode data processors */
        for(CurProcessor=FData->DataProcessorsList.begin(),Index=0;
                CurProcessor!=FData->DataProcessorsList.end();
                CurProcessor++,Index++)
        {
            if(CurProcessor->API.ProcessIncomingByte!=NULL)
            {
                CurProcessor->API.ProcessIncomingByte(FData->
                        ProcessorsData[Index],inbuff[byte],ProcessedChar,
                        &CharLen,&Consumed);
            }
        }

        if(!Consumed)
        {
            ProcessedChar[CharLen]=0;   // Make it a string
DB_StartTimer(e_DBT_AddChar2Display);
            Con_WriteChar2Display(ProcessedChar);
DB_StopTimer(e_DBT_AddChar2Display);
        }
    }
}
//            MW_AddString2Con(ConnectionID,ProcessedChar,CharLen,&FData->Style);


///*******************************************************************************
// * NAME:
// *    DPS_ProcessorIncomingBytes
// *
// * SYNOPSIS:
// *    void DPS_ProcessorIncomingBytes(t_ConID ConnectionID,const uint8_t *inbuff,
// *          int bytes,struct ProcessorConData *FData);
// *
// * PARAMETERS:
// *    ConnectionID [I] -- The connection this data is for
// *    inbuff [I] -- The buffer with the data that was read.  This is raw data
// *    bytes [I] -- The number of bytes that was read.
// *    FData [I/O] -- The filter connection data for this connection.
// *
// * FUNCTION:
// *    This function takes bytes that have come into a connection and passes
// *    them into the filters.  As a last step it adds them to the main window.
// *
// * RETURNS:
// *    NONE
// *
// * SEE ALSO:
// *    
// ******************************************************************************/
//void DPS_ProcessorIncomingBytes(t_ConID ConnectionID,const uint8_t *inbuff,
//        int bytes,struct ProcessorConData *FData)
//{
//    uint8_t ProcessedChar[10];  // Buffer for the char we will eventually output.  UTF8 seems to be limited to 4 maybe 6 bytes so 10 should be good (it's up the plugin not to go over 6)
//    bool Consumed;
//    int32_t byte;
//    int CharLen;
//    i_DPSDataProcessorsType CurProcessor;
//    unsigned int Index;
//
//    for(byte=0;byte<bytes;byte++)
//    {
//        Consumed=false;
//        CharLen=1;
//        ProcessedChar[0]=inbuff[byte];
//
//        /* Text mode filters */
//        for(CurProcessor=FData->DataProcessorsList.begin(),Index=0;
//                CurProcessor!=FData->DataProcessorsList.end();
//                CurProcessor++,Index++)
//        {
//            (*CurProcessor)->API->ProcessByte(FData->ProcessorsData[Index],
//                    ConnectionID,inbuff[byte],ProcessedChar,&CharLen,
//                    &Consumed,&FData->Style);
//        }
//
////        if(!Consumed)
////            MW_AddString2Con(ConnectionID,ProcessedChar,CharLen,&FData->Style);
//    }
//}

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
        if(CurProcessor->Info.ProType==e_DataProcessorType_Text &&
                CurProcessor->Info.ProClass==e_DataProcessorClass_TermEmulation)
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
 *    void DPS_GetListOfTextProcessors(e_DataProcessorClassType ProClass,
 *          t_DPS_TextProInfoType &RetData);
 *
 * PARAMETERS:
 *    ProClass [I] -- What class of input processor to return a list of
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
 *    
 ******************************************************************************/
void DPS_GetListOfTextProcessors(e_DataProcessorClassType ProClass,
        t_DPS_TextProInfoType &RetData)
{
    i_DPSDataProcessorsType CurProcessor;
    struct DPS_TextProInfo NewEntry;

    RetData.clear();

    for(CurProcessor=m_DataProcessors.begin();
            CurProcessor!=m_DataProcessors.end();CurProcessor++)
    {
        if(CurProcessor->Info.ProType==e_DataProcessorType_Text &&
                CurProcessor->Info.ProClass==ProClass)
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
 *    void DPS_SetAttribs(uint16_t Attribs);
 *
 * PARAMETERS:
 *    Attribs [I] -- The new attribs to use.  These are bit values where
 *                   the following bits are supported:
 *                      TXT_ATTRIB_UNDERLINE -- Underline the text
 *                      TXT_ATTRIB_UNDERLINE_DOUBLE -- Double underline the text
 *                      TXT_ATTRIB_UNDERLINE_DOTTED -- Dotted underline
 *                      TXT_ATTRIB_UNDERLINE_DASHED -- Dashed underline
 *                      TXT_ATTRIB_UNDERLINE_WAVY -- A wavy underline
 *                      TXT_ATTRIB_OVERLINE -- Put a line over the text
 *                      TXT_ATTRIB_LINETHROUGHT -- Put a line though the text
 *                      TXT_ATTRIB_BOLD -- Bold Text
 *                      TXT_ATTRIB_ITALIC -- Italic Text
 *                      TXT_ATTRIB_OUTLINE -- Draw an outline around the leters.
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
void DPS_SetAttribs(uint16_t Attribs)
{
    Con_SetAttribs(Attribs);
}

/*******************************************************************************
 * NAME:
 *    DPS_GetAttribs
 *
 * SYNOPSIS:
 *    uint16_t DPS_GetAttribs(void);
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
uint16_t DPS_GetAttribs(void)
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
 *    Depending on the settings this may erase the char under the char.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
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
 *    X [I] -- The new X pos
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

