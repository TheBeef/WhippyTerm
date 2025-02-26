/*******************************************************************************
 * FILENAME: NewLineProcessor.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This has the new line processor that converts end of lines to always
 *    move the cursor to the new line.
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
 *    Paul Hutchinson (02 Jul 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "NewLineProcessor.h"
#include "PluginSDK/Plugin.h"
#include <string.h>

/*** DEFINES                  ***/
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      NewLineProcessor // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x01000000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
const struct DataProcessorInfo *NewLineProcessor_GetProcessorInfo(
        unsigned int *SizeOfInfo);
void NewLineProcessor_ProcessByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed);

/*** VARIABLE DEFINITIONS     ***/
struct DataProcessorAPI m_NewLineProcessorCBs=
{
    NULL,
    NULL,
    NewLineProcessor_GetProcessorInfo,
    NULL,
    NewLineProcessor_ProcessByte,
};
struct DataProcessorInfo m_NewLineProcessor_Info=
{
    "New Line Processor",
    "Added a '\\r' when it sees a '\\n'",
    "Does a cartridge return when a new line character is read",
    e_DataProcessorType_Text,
    e_TextDataProcessorClass_Other
};

static const struct DPS_API *m_NLP_DPS;

/*******************************************************************************
 * NAME:
 *    NewLineProcessor_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int NewLineProcessor_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
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

        m_NLP_DPS=SysAPI->GetAPI_DataProcessors();

        /* If we are have the correct experimental API */
        if(SysAPI->GetExperimentalID()>0 &&
                SysAPI->GetExperimentalID()<1)
        {
            return 0xFFFFFFFF;
        }

        m_NLP_DPS->RegisterDataProcessor("NewLineProcessor",
                &m_NewLineProcessorCBs,sizeof(m_NewLineProcessorCBs));

        return 0;
    }
}

/*******************************************************************************
 * NAME:
 *    NewLineProcessor_GetProcessorInfo
 *
 * SYNOPSIS:
 *    const struct DataProcessorInfo *NewLineProcessor_GetProcessorInfo(
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
const struct DataProcessorInfo *NewLineProcessor_GetProcessorInfo(
        unsigned int *SizeOfInfo)
{
    *SizeOfInfo=sizeof(struct DataProcessorInfo);
    return &m_NewLineProcessor_Info;
}

/*******************************************************************************
 *  NAME:
 *    NewLineProcessor_ProcessByte
 *
 *  SYNOPSIS:
 *    void NewLineProcessor_ProcessByte(t_DataProcessorHandleType *DataHandle,
 *          const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
 *          PG_BOOL *Consumed)
 *
 *  PARAMETERS:
 *    DataHandle [I] -- The data handle to free.  This will need to be
 *                      case to your internal data type before you use it.
 *    ConnectionID [I] -- The connection to work on
 *    RawByte [I] -- The raw byte to process.  This is the byte that came in.
 *    ProcessedChar [I/O] -- This is a unicode char that has already been
 *                         processed by some of the other input filters.  You
 *                         can change this as you need.  It must remain only
 *                         one unicode char.
 *    CharLen [I/O] -- This number of bytes in 'ProcessedChar'
 *    Consumed [I/O] -- This tells the system (and other filters) if the
 *                      char has been used up and will not be added to the
 *                      screen.
 *    Style [I/O] -- This is the current style that the char will be added
 *                   with.  You can change this to change how the char will
 *                   be added.
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
void NewLineProcessor_ProcessByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed)
{
    /* If it's a new line char add a char return */
    if(*CharLen==1 && *ProcessedChar=='\n')
        m_NLP_DPS->DoReturn();
}
