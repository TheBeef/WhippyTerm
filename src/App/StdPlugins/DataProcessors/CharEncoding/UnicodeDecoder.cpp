/*******************************************************************************
 * FILENAME: UnicodeDecoder.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the Unicode decoder input filter in it.
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

/*** HEADER FILES TO INCLUDE  ***/
#include "UnicodeDecoder.h"
#include "PluginSDK/Plugin.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*** DEFINES                  ***/
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      UnicodeDecoder // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x01000000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct UnicodeData
{
    uint8_t BufferedBytes[10];
    int InsertPos;
    int BytesLeft;
};

/*** FUNCTION PROTOTYPES      ***/
void UnicodeDecoder_Error(uint8_t *ProcessedChar,int &CharLen);
t_DataProcessorHandleType *UnicodeDecoder_AllocateData(void);
void UnicodeDecoder_FreeData(t_DataProcessorHandleType *DataHandle);
const struct DataProcessorInfo *UnicodeDecoder_GetProcessorInfo(unsigned int *SizeOfInfo);
void UnicodeDecoder_ProcessByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed);

/*** VARIABLE DEFINITIONS     ***/
struct DataProcessorAPI m_UnicodeDecoderCBs=
{
    UnicodeDecoder_AllocateData,
    UnicodeDecoder_FreeData,
    UnicodeDecoder_GetProcessorInfo,
    NULL,
    UnicodeDecoder_ProcessByte,
    NULL, // ProcessIncomingBinaryByte
    /* V2 */
    NULL,       // ProcessOutGoingData
    NULL,       // AllocSettingsWidgets
    NULL,       // FreeSettingsWidgets
    NULL,       // StoreSettings
};
struct DataProcessorInfo m_UnicodeDecoder_Info=
{
    "Unicode",
    "UTF-8 encoded bytes",
    "Takes incoming bytes and converts them to unicode using UTF-8 encoding.",
    e_DataProcessorType_Text,
    e_TextDataProcessorClass_CharEncoding,
    e_BinaryDataProcessorModeMAX,
};

static const struct PI_UIAPI *m_UIAPI;
static const struct PI_SystemAPI *m_System;
static const struct DPS_API *m_DPS;

/*******************************************************************************
 * NAME:
 *    URLHighlighter_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int URLHighlighter_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
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

        m_System=SysAPI;
        m_DPS=SysAPI->GetAPI_DataProcessors();
        m_UIAPI=m_DPS->GetAPI_UI();

        /* If we are have the correct experimental API */
        if(m_System->GetExperimentalID()>0 &&
                m_System->GetExperimentalID()<1)
        {
            return 0xFFFFFFFF;
        }

        m_DPS->RegisterDataProcessor("UnicodeDecoder",
                &m_UnicodeDecoderCBs,sizeof(m_UnicodeDecoderCBs));

        return 0;
    }
}

/*******************************************************************************
 * NAME:
 *    UnicodeDecoder_AllocateData
 *
 * SYNOPSIS:
 *    t_DataProcessorHandleType *UnicodeDecoder_AllocateData(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates any needed data for this input filter.
 *
 * RETURNS:
 *    A pointer to the data, NULL if there was an error.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_DataProcessorHandleType *UnicodeDecoder_AllocateData(void)
{
    struct UnicodeData *Data;
    Data=(struct UnicodeData *)malloc(sizeof(struct UnicodeData));
    if(Data==NULL)
        return NULL;

    Data->InsertPos=0;
    Data->BytesLeft=0;

    return (t_DataProcessorHandleType *)Data;
}

/*******************************************************************************
 *  NAME:
 *    UnicodeDecoder_FreeData
 *
 *  SYNOPSIS:
 *    void UnicodeDecoder_FreeData(t_DataProcessorHandleType *DataHandle);
 *
 *  PARAMETERS:
 *    DataHandle [I] -- The data handle to free.  This will need to be
 *                      case to your internal data type before you use it.
 *
 *  FUNCTION:
 *    This function frees the memory allocated with AllocateData().
 *
 *  RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UnicodeDecoder_FreeData(t_DataProcessorHandleType *DataHandle)
{
    struct UnicodeData *Data=(struct UnicodeData *)DataHandle;

    free(Data);
}
/*******************************************************************************
 * NAME:
 *    UnicodeDecoder_GetProcessorInfo
 *
 * SYNOPSIS:
 *    const struct DataProcessorInfo *UnicodeDecoder_GetProcessorInfo(
 *              unsigned int *SizeOfInfo);
 *
 * PARAMETERS:
 *    SizeOfInfo [O] -- The size of 'struct DataProcessorInfo'.  This is used
 *                        for forward / backward compatibility.
 *
 * FUNCTION:
 *    This function gets info about the plugin.  'DataProcessorInfo' has
 *    the following fields:
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
 *                      e_TextDataProcessorClass_Other -- This is a generic class
 *                          more than one of these processors can be active
 *                          at a time but no other requirements exist.
 *                      e_TextDataProcessorClass_CharEncoding -- This is a
 *                          class that converts the raw stream into some kind
 *                          of char encoding.  For example unicode is converted
 *                          from a number of bytes to chars in the system.
 *                      e_TextDataProcessorClass_TermEmulation -- This is a
 *                          type of terminal emulator.  An example of a
 *                          terminal emulator is VT100.
 *                      e_TextDataProcessorClass_Highlighter -- This is a processor
 *                          that highlights strings as they come in the input
 *                          stream.  For example a processor that underlines
 *                          URL's.
 *                      e_TextDataProcessorClass_Logger -- This is a processor
 *                          that saves the input.  It may save to a file or
 *                          send out a debugging service.  And example is
 *                          a processor that saves all the raw bytes to a file.
 * SEE ALSO:
 *    
 ******************************************************************************/
const struct DataProcessorInfo *UnicodeDecoder_GetProcessorInfo(
        unsigned int *SizeOfInfo)
{
    *SizeOfInfo=sizeof(struct DataProcessorInfo);
    return &m_UnicodeDecoder_Info;
}

/*******************************************************************************
 *  NAME:
 *    UnicodeDecoder_ProcessByte
 *
 *  SYNOPSIS:
 *    void UnicodeDecoder_ProcessByte(
 *              t_DataProcessorHandleType *DataHandle,const uint8_t RawByte,
 *              uint8_t *ProcessedChar,int *CharLen,PG_BOOL *Consumed);
 *
 *  PARAMETERS:
 *    DataHandle [I] -- The data handle to your internal data.
 *    RawByte [I] -- The raw byte to process.  This is the byte that came in.
 *    ProcessedChar [I/O] -- This is a unicode char that has already been
 *                         processed by some of the other input filters.  You
 *                         can change this as you need.  It must remain only
 *                         one unicode char.
 *    CharLen [I/O] -- This number of bytes in 'ProcessedChar'
 *    Consumed [I/O] -- This tells the system (and other filters) if the
 *                      char has been used up and will not be added to the
 *                      screen.
 *
 *  FUNCTION:
 *    This function is called for each byte that comes in.
 *
 *  RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UnicodeDecoder_ProcessByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed)
{
    struct UnicodeData *Data=(struct UnicodeData *)DataHandle;

    if((RawByte&0x80)==0x00)
    {
        /* AscII */
        Data->BytesLeft=0;
        return;
    }
    else if((RawByte&0xC0)==0x80)
    {
        /* Continued bytes */
        if(Data->BytesLeft>0)
        {
            Data->BufferedBytes[Data->InsertPos++]=RawByte;
            Data->BytesLeft--;
            if(Data->BytesLeft==0)
            {
                /* This was the last char needed */
                memcpy(ProcessedChar,Data->BufferedBytes,Data->InsertPos);
                *CharLen=Data->InsertPos;
            }
            else
            {
                *Consumed=true;
            }
        }
        else
        {
            /* Error */
            UnicodeDecoder_Error(ProcessedChar,*CharLen);
        }
        return;
    }

    Data->BufferedBytes[0]=RawByte;
    Data->InsertPos=1;
    if((RawByte&0xE0)==0xC0)
    {
        /* 2 byte */
        Data->BytesLeft=1;
    }
    else if((RawByte&0xF0)==0xE0)
    {
        /* 3 byte */
        Data->BytesLeft=2;
    }
    else if((RawByte&0xF8)==0xF0)
    {
        /* 4 byte */
        Data->BytesLeft=3;
    }
    else
    {
        /* Error */
        UnicodeDecoder_Error(ProcessedChar,*CharLen);
        Data->BytesLeft=0;
        return;
    }
    *Consumed=true;
}

void UnicodeDecoder_Error(uint8_t *ProcessedChar,int &CharLen)
{
//    /* Replacement character */
//    ProcessedChar[0]=0xEF;
//    ProcessedChar[1]=0xBF;
//    ProcessedChar[2]=0xBD;

    /* MEDIUM SHADE */
    ProcessedChar[0]=0xE2;
    ProcessedChar[1]=0x96;
    ProcessedChar[2]=0x92;

//    /* BALLOT BOX */
//    ProcessedChar[0]=0xE2;
//    ProcessedChar[1]=0x98;
//    ProcessedChar[2]=0x90;

    CharLen=3;
}
