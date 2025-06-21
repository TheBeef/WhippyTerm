/*******************************************************************************
 * FILENAME: CodePage437Decoder.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
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
#include "CodePage437Decoder.h"
#include "PluginSDK/Plugin.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*** DEFINES                  ***/
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      CodePage437Decode // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x01000000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
const uint16_t m_OEM2Unicode_LookupTable[256]=
{
    0x0000,0x263A,0x263B,0x2665,0x2666,0x2663,0x2660,0x2022,0x25D8,0x25CB,
    0x25D9,0x2642,0x2640,0x266A,0x266B,0x263C,0x25BA,0x25C4,0x2195,0x203C,
    0x00B6,0x00A7,0x25AC,0x21A8,0x2191,0x2193,0x2192,0x2190,0x221F,0x2194,
    0x25B2,0x25BC,0x0020,0x0021,0x0022,0x0023,0x0024,0x0025,0x0026,0x0027,
    0x0028,0x0029,0x002A,0x002B,0x002C,0x002D,0x002E,0x002F,0x0030,0x0031,
    0x0032,0x0033,0x0034,0x0035,0x0036,0x0037,0x0038,0x0039,0x003A,0x003B,
    0x003C,0x003D,0x003E,0x003F,0x0040,0x0041,0x0042,0x0043,0x0044,0x0045,
    0x0046,0x0047,0x0048,0x0049,0x004A,0x004B,0x004C,0x004D,0x004E,0x004F,
    0x0050,0x0051,0x0052,0x0053,0x0054,0x0055,0x0056,0x0057,0x0058,0x0059,
    0x005A,0x005B,0x005C,0x005D,0x005E,0x005F,0x0060,0x0061,0x0062,0x0063,
    0x0064,0x0065,0x0066,0x0067,0x0068,0x0069,0x006A,0x006B,0x006C,0x006D,
    0x006E,0x006F,0x0070,0x0071,0x0072,0x0073,0x0074,0x0075,0x0076,0x0077,
    0x0078,0x0079,0x007A,0x007B,0x007C,0x007D,0x007E,0x2302,0x00C7,0x00FC,
    0x00E9,0x00E2,0x00E4,0x00E0,0x00E5,0x00E7,0x00EA,0x00EB,0x00E8,0x00EF,
    0x00EE,0x00EC,0x00C4,0x00C5,0x00C9,0x00E6,0x00C6,0x00F4,0x00F6,0x00F2,
    0x00FB,0x00F9,0x00FF,0x00D6,0x00DC,0x00A2,0x00A3,0x00A5,0x20A7,0x0192,
    0x00E1,0x00ED,0x00F3,0x00FA,0x00F1,0x00D1,0x00AA,0x00BA,0x00BF,0x2310,
    0x00AC,0x00BD,0x00BC,0x00A1,0x00AB,0x00BB,0x2591,0x2592,0x2593,0x2502,
    0x2524,0x2561,0x2562,0x2556,0x2555,0x2563,0x2551,0x2557,0x255D,0x255C,
    0x255B,0x2510,0x2514,0x2534,0x252C,0x251C,0x2500,0x253C,0x255E,0x255F,
    0x255A,0x2554,0x2569,0x2566,0x2560,0x2550,0x256C,0x2567,0x2568,0x2564,
    0x2565,0x2559,0x2558,0x2552,0x2553,0x256B,0x256A,0x2518,0x250C,0x2588,
    0x2584,0x258C,0x2590,0x2580,0x03B1,0x00DF,0x0393,0x03C0,0x03A3,0x03C3,
    0x00B5,0x03C4,0x03A6,0x0398,0x03A9,0x03B4,0x221E,0x03C6,0x03B5,0x2229,
    0x2261,0x00B1,0x2265,0x2264,0x2320,0x2321,0x00F7,0x2248,0x00B0,0x2219,
    0x00B7,0x221A,0x207F,0x00B2,0x25A0,0x00A0
};

/*** FUNCTION PROTOTYPES      ***/
const struct DataProcessorInfo *CodePage437Decode_GetProcessorInfo(
        unsigned int *SizeOfInfo);
void CodePage437Decode_ProcessByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed);

/*** VARIABLE DEFINITIONS     ***/
struct DataProcessorAPI m_CodePage437DecodeCBs=
{
    NULL,
    NULL,
    CodePage437Decode_GetProcessorInfo,
    NULL,
    CodePage437Decode_ProcessByte,
    NULL, // ProcessIncomingBinaryByte
    /* V2 */
    NULL,       // ProcessOutGoingData
    NULL,       // AllocSettingsWidgets
    NULL,       // FreeSettingsWidgets
    NULL,       // StoreSettings
};

struct DataProcessorInfo m_CodePage437Decode_Info=
{
    "DOS Characters (IBM CP437)",
    "Original IBM PC (OEM) charactor set (code page 437)",
    "The IBM PC used extended ASCII with extra charactors above 127.  This"
        " converts bytes in the extended range (128-255) to the symbols used"
        " by the IBM PC.",
    e_DataProcessorType_Text,
    e_TextDataProcessorClass_CharEncoding,
    e_BinaryDataProcessorModeMAX,
};

static const struct PI_UIAPI *m_UIAPI;
static const struct PI_SystemAPI *m_System;
static const struct DPS_API *m_DPS;

/*******************************************************************************
 * NAME:
 *    CodePage437Decode_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int CodePage437Decode_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
 *          unsigned int Version);
 *
 * PARAMETERS:
 *    SysAPI [I] -- The main API to WhippyTerm
 *    Version [I] -- What version of WhippyTerm is running.  This is used
 *                   to make sure we are compatible.  This is in the
 *                   Major<<24 | Minor<<16 | Rev<<8 | Patch
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

        m_DPS->RegisterDataProcessor("CodePage437Decode",
                &m_CodePage437DecodeCBs,sizeof(m_CodePage437DecodeCBs));

        return 0;
    }
}
/*******************************************************************************
 * NAME:
 *    CodePage437Decode_GetProcessorInfo
 *
 * SYNOPSIS:
 *    const struct DataProcessorInfo *CodePage437Decode_GetProcessorInfo(
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
const struct DataProcessorInfo *CodePage437Decode_GetProcessorInfo(
        unsigned int *SizeOfInfo)
{
    *SizeOfInfo=sizeof(struct DataProcessorInfo);
    return &m_CodePage437Decode_Info;
}

/*******************************************************************************
 *  NAME:
 *    CodePage437Decode_ProcessByte
 *
 *  SYNOPSIS:
 *    void CodePage437Decode_ProcessByte(
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
void CodePage437Decode_ProcessByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed)
{
    uint16_t CodePoint;

    CodePoint=m_OEM2Unicode_LookupTable[RawByte];

    /* Convert to UTF-8 */
    if(CodePoint<=0x7F)
    {
        ProcessedChar[0]=CodePoint;
        *CharLen=1;
    }
    else if(CodePoint<=0x7FF)
    {
        ProcessedChar[0]=0xC0|((CodePoint>>6)&0x1F);
        ProcessedChar[1]=0x80|((CodePoint)&0x3F);
        *CharLen=2;
    }
    else
    {
        ProcessedChar[0]=0xE0|((CodePoint>>12)&0x0F);
        ProcessedChar[1]=0x80|((CodePoint>>6)&0x3F);
        ProcessedChar[2]=0x80|((CodePoint)&0x3F);
        *CharLen=3;
    }
}
