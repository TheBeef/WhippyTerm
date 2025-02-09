// https://vt100.net/docs/vt510-rm/chapter4.html

// Also see:https://stackoverflow.com/questions/44116977/get-mouse-position-in-pixels-using-escape-sequences
// http://invisible-island.net/xterm/ctlseqs/ctlseqs.html
// https://godoc.org/github.com/pborman/ansi
// https://ecma-international.org/publications-and-standards/standards/ecma-48/

// Note: Seems that ANSI-X3.64 was withdrawn because it's basicly ECMA-48, but
// everyone seems to use ANSI and no one knows about ECMA-48.

/*******************************************************************************
 * FILENAME: ANSIX3_64.c
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    == C0 ==
 *    NUL     00/00           0x00    NULL
 *    SOH     00/01           0x01    START OF HEADING
 *    STX     00/02           0x02    START OF TEXT
 *    ETX     00/03           0x03    END OF TEXT
 *    EOT     00/04           0x04    END OF TRANSMISSION
 *    ENQ     00/05           0x05    ENQUIRY
 *    ACK     00/06           0x06    ACKNOWLEDGE
 *    BEL     00/07           0x07    BELL
 *    BS      00/08           0x08    BACKSPACE
 *    HT      00/09           0x09    CHARACTER TABULATION
 *    LF      00/10           0x0A    LINE FEED
 *    VT      00/11           0x0B    LINE TABULATION
 *    FF      00/12           0x0C    FORM FEED
 *    CR      00/13           0x0D    CARRIAGE RETURN
 *    SO/LS1  00/14           0x0E    SHIFT-OUT / LOCKING-SHIFT ONE
 *    SI/LS0  00/15           0x0F    SHIFT-IN / LOCKING-SHIFT ZERO
 *    DLE     01/00           0x10    DATA LINK ESCAPE
 *    DC1     01/01           0x11    DEVICE CONTROL ONE
 *    DC2     01/02           0x12    DEVICE CONTROL TWO
 *    DC3     01/03           0x13    DEVICE CONTROL THREE
 *    DC4     01/04           0x14    DEVICE CONTROL FOUR
 *    NAK     01/05           0x15    NEGATIVE ACKNOWLEDGE
 *    SYN     01/06           0x16    SYNCHRONOUS IDLE
 *    ETB     01/07           0x17    END OF TRANSMISSION BLOCK
 *    CAN     01/08           0x18    CANCEL
 *    EM      01/09           0x19    END OF MEDIUM
 *    SUB     01/10           0x1A    SUBSTITUTE
 *    ESC     01/11           0x1B    ESCAPE
 *    IS4     01/12           0x1C    INFORMATION SEPARATOR FOUR (FS - FILE SEPARATOR)
 *    IS3     01/13           0x1D    INFORMATION SEPARATOR THREE (GS - GROUP SEPARATOR)
 *    IS2     01/14           0x1E    INFORMATION SEPARATOR TWO (RS - RECORD SEPARATOR)
 *    IS1     01/15           0x1F    INFORMATION SEPARATOR ONE (US - UNIT SEPARATOR)
 *
 *    == C1 ==
 *    BPH     ESC 04/02       0x1B 0x42   B   BREAK PERMITTED HERE
 *    NBH     ESC 04/03       0x1B 0x43   C   NO BREAK HERE
 *    NEL     ESC 04/05       0x1B 0x45   E   NEXT LINE
 *    SSA     ESC 04/06       0x1B 0x46   F   START OF SELECTED AREA
 *    ESA     ESC 04/07       0x1B 0x47   G   END OF SELECTED AREA
 *    HTS     ESC 04/08       0x1B 0x48   H   CHARACTER TABULATION SET
 *    HTJ     ESC 04/09       0x1B 0x49   I   CHARACTER TABULATION WITH JUSTIFICATION
 *    VTS     ESC 04/10       0x1B 0x4A   J   LINE TABULATION SET
 *    PLD     ESC 04/11       0x1B 0x4B   K   PARTIAL LINE FORWARD
 *    PLU     ESC 04/12       0x1B 0x4C   L   PARTIAL LINE BACKWARD
 *    RI      ESC 04/13       0x1B 0x4D   M   REVERSE LINE FEED
 *    SS2     ESC 04/14       0x1B 0x4E   N   SINGLE-SHIFT TWO
 *    SS3     ESC 04/15       0x1B 0x4F   O   SINGLE-SHIFT THREE
 *
 *    DCS     ESC 05/00       0x1B 0x50   P   DEVICE CONTROL STRING
 *    PU1     ESC 05/01       0x1B 0x51   Q   PRIVATE USE ONE
 *    PU2     ESC 05/02       0x1B 0x52   R   PRIVATE USE TWO
 *    STS     ESC 05/03       0x1B 0x53   S   SET TRANSMIT STATE
 *    CCH     ESC 05/04       0x1B 0x54   T   CANCEL CHARACTER
 *    MW      ESC 05/05       0x1B 0x55   U   MESSAGE WAITING
 *    SPA     ESC 05/06       0x1B 0x56   V   START OF GUARDED AREA
 *    EPA     ESC 05/07       0x1B 0x57   W   END OF GUARDED AREA
 *    SOS     ESC 05/08       0x1B 0x58   X   START OF STRING
 *    SCI     ESC 05/10       0x1B 0x5A   Z   SINGLE CHARACTER INTRODUCER
 *    CSI     ESC 05/11       0x1B 0x5B   [   CONTROL SEQUENCE INTRODUCER
 *    ST      ESC 05/12       0x1B 0x5C   \   STRING TERMINATOR
 *    OSC     ESC 05/13       0x1B 0x5D   ]   OPERATING SYSTEM COMMAND
 *    PM      ESC 05/14       0x1B 0x5E   ^   PRIVACY MESSAGE
 *    APC     ESC 05/15       0x1B 0x5F   _   APPLICATION PROGRAM COMMAND
 *
 *    == NP ==
 *
 *    == Ps ==
 *    ICH     CSI 04/00       0x1B 0x5B 0x40  @   INSERT CHARACTER
 *    CUU     CSI 04/01       0x1B 0x5B 0x41  A   CURSOR UP
 *    CUD     CSI 04/02       0x1B 0x5B 0x42  B   CURSOR DOWN
 *    CUF     CSI 04/03       0x1B 0x5B 0x43  C   CURSOR RIGHT
 *    CUB     CSI 04/04       0x1B 0x5B 0x44  D   CURSOR LEFT
 *    CNL     CSI 04/05       0x1B 0x5B 0x45  E   CURSOR NEXT LINE
 *    CPL     CSI 04/06       0x1B 0x5B 0x46  F   CURSOR PRECEDING LINE
 *    CHA     CSI 04/07       0x1B 0x5B 0x47  G   CURSOR CHARACTER ABSOLUTE
 *    CUP     CSI 04/08       0x1B 0x5B 0x48  H   CURSOR POSITION
 *    CHT     CSI 04/09       0x1B 0x5B 0x49  I   CURSOR FORWARD TABULATION
 *    ED      CSI 04/10       0x1B 0x5B 0x4A  J   ERASE IN PAGE
 *    EL      CSI 04/11       0x1B 0x5B 0x4B  K   ERASE IN LINE
 *    IL      CSI 04/12       0x1B 0x5B 0x4C  L   INSERT LINE
 *    DL      CSI 04/13       0x1B 0x5B 0x4D  M   DELETE LINE
 *    EF      CSI 04/14       0x1B 0x5B 0x4E  N   ERASE IN FIELD
 *    EA      CSI 04/15       0x1B 0x5B 0x4F  O   ERASE IN AREA
 *
 *    DCH     CSI 05/00       0x1B 0x5B 0x50  @   DELETE CHARACTER
 *    SEE     CSI 05/01       0x1B 0x5B 0x51  Q   SELECT EDITING EXTENT (miss labled SSE)
 *    CPR     CSI 05/02       0x1B 0x5B 0x52  R   ACTIVE POSITION REPORT
 *    SU      CSI 05/03       0x1B 0x5B 0x53  S   SCROLL UP
 *    SD      CSI 05/04       0x1B 0x5B 0x54  T   SCROLL DOWN
 *    NP      CSI 05/05       0x1B 0x5B 0x55  U   NEXT PAGE
 *    PP      CSI 05/06       0x1B 0x5B 0x56  V   PRECEDING PAGE
 *    CTC     CSI 05/07       0x1B 0x5B 0x57  W   CURSOR TABULATION CONTROL
 *    ECH     CSI 05/08       0x1B 0x5B 0x58  X   ERASE CHARACTER
 *    CVT     CSI 05/09       0x1B 0x5B 0x59  Y   CURSOR LINE TABULATION
 *    CBT     CSI 05/10       0x1B 0x5B 0x5A  Z   CURSOR BACKWARD TABULATION
 *    SRS     CSI 05/11       0x1B 0x5B 0x5B  [   START REVERSED STRING
 *    PTX     CSI 05/12       0x1B 0x5B 0x5C  \   PARALLEL TEXTS
 *    SDS     CSI 05/13       0x1B 0x5B 0x5D  ]   START DIRECTED STRING
 *    SIMD    CSI 05/14       0x1B 0x5B 0x5E  ^   SELECT IMPLICIT MOVEMENT DIRECTION
 *    ---     CSI 05/15       0x1B 0x5B 0x5F  _   
 *
 *    HPA     CSI 06/00       0x1B 0x5B 0x60  `   CHARACTER POSITION ABSOLUTE
 *    HPR     CSI 06/01       0x1B 0x5B 0x61  a   CHARACTER POSITION FORWARD
 *    REP     CSI 06/02       0x1B 0x5B 0x62  b   REPEAT
 *    DA      CSI 06/03       0x1B 0x5B 0x63  c   DEVICE ATTRIBUTES
 *    VPA     CSI 06/04       0x1B 0x5B 0x64  d   LINE POSITION ABSOLUTE
 *    VPR     CSI 06/05       0x1B 0x5B 0x65  e   LINE POSITION FORWARD
 *    HVP     CSI 06/06       0x1B 0x5B 0x66  f   CHARACTER AND LINE POSITION
 *    TBC     CSI 06/07       0x1B 0x5B 0x67  g   TABULATION CLEAR
 *    SM      CSI 06/08       0x1B 0x5B 0x68  h   SET MODE
 *    MC      CSI 06/09       0x1B 0x5B 0x69  i   MEDIA COPY
 *    HPB     CSI 06/10       0x1B 0x5B 0x6A  j   CHARACTER POSITION BACKWARD
 *    VPB     CSI 06/11       0x1B 0x5B 0x6B  k   LINE POSITION BACKWARD
 *    RM      CSI 06/12       0x1B 0x5B 0x6C  l   RESET MODE
 *    SGR     CSI 06/13       0x1B 0x5B 0x6D  m   SELECT GRAPHIC RENDITION
 *    DSR     CSI 06/14       0x1B 0x5B 0x6E  n   DEVICE STATUS REPORT
 *    DAQ     CSI 06/15       0x1B 0x5B 0x6F  o   DEFINE AREA QUALIFICATION
 *
 *    == Pn ==
 *    SL      CSI 04/00       0x1B 0x5B 0x20 @    SCROLL LEFT
 *    SR      CSI 04/01       0x1B 0x5B 0x20 A    SCROLL RIGHT
 *    GSM     CSI 04/02       0x1B 0x5B 0x20 B    GRAPHIC SIZE MODIFICATION
 *    GSS     CSI 04/03       0x1B 0x5B 0x20 C    GRAPHIC SIZE SELECTION
 *    FNT     CSI 04/04       0x1B 0x5B 0x20 D    FONT SELECTION
 *    TSS     CSI 04/05       0x1B 0x5B 0x20 E    THIN SPACE SPECIFICATION
 *    JFY     CSI 04/06       0x1B 0x5B 0x20 F    JUSTIFY
 *    SPI     CSI 04/07       0x1B 0x5B 0x20 G    SPACING INCREMENT
 *    QUAD    CSI 04/08       0x1B 0x5B 0x20 H    QUAD
 *    SSU     CSI 04/09       0x1B 0x5B 0x20 I    SELECT SIZE UNIT
 *    PFS     CSI 04/10       0x1B 0x5B 0x20 J    PAGE FORMAT SELECTION
 *    SHS     CSI 04/11       0x1B 0x5B 0x20 K    SELECT CHARACTER SPACING
 *    SVS     CSI 04/12       0x1B 0x5B 0x20 L    SELECT LINE SPACING
 *    IGS     CSI 04/13       0x1B 0x5B 0x20 M    IDENTIFY GRAPHIC SUBREPERTOIRE
 *    --      CSI 04/14       0x1B 0x5B 0x20 N    
 *    IDCS    CSI 04/15       0x1B 0x5B 0x20 O    IDENTIFY DEVICE CONTROL STRING
 *
 *    PPA     CSI 05/00       0x1B 0x5B 0x20 P    PAGE POSITION ABSOLUTE
 *    PPR     CSI 05/01       0x1B 0x5B 0x20 Q    PAGE POSITION FORWARD
 *    PPB     CSI 05/02       0x1B 0x5B 0x20 R    PAGE POSITION BACKWARD
 *    SPD     CSI 05/03       0x1B 0x5B 0x20 S    SELECT PRESENTATION DIRECTIONS
 *    DTA     CSI 05/04       0x1B 0x5B 0x20 T    DIMENSION TEXT AREA
 *    SHL/SLH CSI 05/05       0x1B 0x5B 0x20 U    SET LINE HOME
 *    SLL     CSI 05/06       0x1B 0x5B 0x20 V    SET LINE LIMIT
 *    FNK     CSI 05/07       0x1B 0x5B 0x20 W    FUNCTION KEY
 *    SPQR    CSI 05/08       0x1B 0x5B 0x20 X    SELECT PRINT QUALITY AND RAPIDITY
 *    SEF     CSI 05/09       0x1B 0x5B 0x20 Y    SHEET EJECT AND FEED
 *    PEC     CSI 05/10       0x1B 0x5B 0x20 Z    PRESENTATION EXPAND OR CONTRACT
 *    SSW     CSI 05/11       0x1B 0x5B 0x20 [    SET SPACE WIDTH
 *    SACS    CSI 05/12       0x1B 0x5B 0x20 \    SET ADDITIONAL CHARACTER SEPARATION
 *    SAPV    CSI 05/13       0x1B 0x5B 0x20 ]    SELECT ALTERNATIVE PRESENTATION VARIANTS
 *    STAB    CSI 05/14       0x1B 0x5B 0x20 ^    SELECTIVE TABULATION
 *    GCC     CSI 05/15       0x1B 0x5B 0x20 _    GRAPHIC CHARACTER COMBINATION
 *
 *    TATE    CSI 06/00       0x1B 0x5B 0x20 `    TABULATION ALIGNED TRAILING EDGE
 *    TALE    CSI 06/01       0x1B 0x5B 0x20 a    TABULATION ALIGNED LEADING EDGE
 *    TAC     CSI 06/02       0x1B 0x5B 0x20 b    TABULATION ALIGNED CENTRED
 *    TCC     CSI 06/03       0x1B 0x5B 0x20 c    TABULATION CENTRED ON CHARACTER
 *    TSR     CSI 06/04       0x1B 0x5B 0x20 d    TABULATION STOP REMOVE
 *    SCO     CSI 06/05       0x1B 0x5B 0x20 e    SELECT CHARACTER ORIENTATION
 *    SRCS    CSI 06/06       0x1B 0x5B 0x20 f    SET REDUCED CHARACTER SEPARATION
 *    SCS     CSI 06/07       0x1B 0x5B 0x20 g    SET CHARACTER SPACING
 *    SLS     CSI 06/08       0x1B 0x5B 0x20 h    SET LINE SPACING
 *    --      CSI 06/09       0x1B 0x5B 0x20 i    
 *    --      CSI 06/10       0x1B 0x5B 0x20 j    
 *    SCP     CSI 06/11       0x1B 0x5B 0x20 k    SELECT CHARACTER PATH
 *    --      CSI 06/12       0x1B 0x5B 0x20 l    
 *    --      CSI 06/13       0x1B 0x5B 0x20 m    
 *    --      CSI 06/14       0x1B 0x5B 0x20 n    
 *    --      CSI 06/15       0x1B 0x5B 0x20 o    
 *
 *    == Fs ==
 *    DMI     ESC 06/00       0x1B 0x60   `   DISABLE MANUAL INPUT
 *    INT     ESC 06/01       0x1B 0x61   a   INTERRUPT
 *    EMI     ESC 06/02       0x1B 0x62   b   ENABLE MANUAL INPUT
 *    RIS     ESC 06/03       0x1B 0x63   c   RESET TO INITIAL STATE
 *    CMD     ESC 06/04       0x1B 0x64   d   CODING METHOD DELIMITER
 *    --      ESC 06/05       0x1B 0x65   e   
 *    --      ESC 06/06       0x1B 0x66   f   
 *    --      ESC 06/07       0x1B 0x67   g   
 *    --      ESC 06/08       0x1B 0x68   h   
 *    --      ESC 06/09       0x1B 0x69   i   
 *    --      ESC 06/10       0x1B 0x6A   j   
 *    --      ESC 06/11       0x1B 0x6B   k   
 *    --      ESC 06/12       0x1B 0x6C   l   
 *    --      ESC 06/13       0x1B 0x6D   m   
 *    LS2     ESC 06/14       0x1B 0x6E   n   LOCKING-SHIFT TWO
 *    LS3     ESC 06/15       0x1B 0x6F   o   LOCKING-SHIFT THREE
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
#include "ANSIX3_64.h"
#include "PluginSDK/Plugin.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*** DEFINES                  ***/
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      ANSIX3_64 // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x000B0000

#define MAX_SEARCH_ABORT_COUNT  128

#define LOG_UNKNOWN_CODES                   // Write a log when we see an unknown code
#define LOG_UNKNOWN_CODES_FILENAME          "/ram/unknowncodes.txt"

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
typedef enum
{
    e_ESCState_Normal,
    e_ESCState_Search4Exit,
    e_ESCState_Search4ST,
    e_ESCState_ESC,
    e_ESCState_CSI,
    e_ESCState_CSIQuest,
    e_ESCStateMAX
} e_ESCStateType;

struct ANSIX364DecoderSavedCursorAttribs
{
    int32_t SavedCursorX;
    int32_t SavedCursorY;
    uint32_t SavedAttribs;
    uint32_t SavedFGColor;
    uint32_t SavedBGColor;
    uint32_t SavedULineColor;
};

struct ANSIX364DecoderData
{
    e_ESCStateType CurrentMode;
    int CSIArg[10];
    unsigned int CSIArgCount;
    unsigned int SearchAbortCount;
    unsigned int SearchIndex;
    int CurrentNum;
    bool DoingDim;
    bool DoingBright;
    uint8_t *LastProcessedChar;
    int LastProcessedCharLen;
    int LastProcessedCharBuffSize;
    struct ANSIX364DecoderSavedCursorAttribs SavedCursorAttribs;
};

/*** FUNCTION PROTOTYPES      ***/
t_DataProcessorHandleType *ANSIX364Decoder_AllocateData(void);
void ANSIX364Decoder_FreeData(t_DataProcessorHandleType *DataHandle);
const struct DataProcessorInfo *ANSIX364Decoder_GetProcessorInfo(unsigned int *SizeOfInfo);
void ANSIX364Decoder_ProcessIncomingTextByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed);
void ANSIX364Decoder_HandleSGR(struct ANSIX364DecoderData *Data);
void ANSIX364Decoder_ProcessCSI(struct ANSIX364DecoderData *Data,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed);
void ANSIX364Decoder_DoCSICommand(struct ANSIX364DecoderData *Data,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed);
void ANSIX364Decoder_ResetSGR(struct ANSIX364DecoderData *Data);
PG_BOOL ANSIX364Decoder_ProcessKeyPress(t_DataProcessorHandleType *DataHandle,
            const uint8_t *KeyChar,int KeyCharLen,e_UIKeys ExtendedKey,
            uint8_t Mod);
void ANSIX364Decoder_ProcessESC(struct ANSIX364DecoderData *Data,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed);
void ANSIX364Decoder_ProcessNormalChar(struct ANSIX364DecoderData *Data,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed);
void ANSIX364Decoder_ProcessCSIQuest(struct ANSIX364DecoderData *Data,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed);
void ANSIX364Decoder_DoCSIQuestCommand(struct ANSIX364DecoderData *Data,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed);
static void ANSIX364Decoder_DefaultData(struct ANSIX364DecoderData *Data);

/*** VARIABLE DEFINITIONS     ***/
static const struct PI_UIAPI *m_UIAPI;
static const struct PI_SystemAPI *m_System;
static const struct DPS_API *m_DPS;

struct DataProcessorAPI m_ANSIX364DecoderAPI=
{
    ANSIX364Decoder_AllocateData,
    ANSIX364Decoder_FreeData,
    ANSIX364Decoder_GetProcessorInfo,
    ANSIX364Decoder_ProcessKeyPress,
    ANSIX364Decoder_ProcessIncomingTextByte,
    NULL, // ProcessIncomingBinaryByte
};
struct DataProcessorInfo m_ANSIX364Decoder_Info=
{
    "ANSI",
    "ANSI escape sequences",
    "ANSI X3.64, ISO 6429, and Digital VT100 escape sequences", // NOTE: ISO/IEC 6429:1992 is the latest
    e_DataProcessorType_Text,
    e_TextDataProcessorClass_TermEmulation,
    e_BinaryDataProcessorModeMAX,
};

/*******************************************************************************
 * NAME:
 *    ANSIX3_64_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int ANSIX3_64_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
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

        m_DPS->RegisterDataProcessor("ANSIX364Decoder",&m_ANSIX364DecoderAPI,
                sizeof(m_ANSIX364DecoderAPI));

        return 0;
    }
}

/*******************************************************************************
 * NAME:
 *    ANSIX364Decoder_AllocateData
 *
 * SYNOPSIS:
 *    t_DataProcessorHandleType *ANSIX364Decoder_AllocateData(void);
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
t_DataProcessorHandleType *ANSIX364Decoder_AllocateData(void)
{
    struct ANSIX364DecoderData *Data;
    Data=(struct ANSIX364DecoderData *)malloc(sizeof(struct ANSIX364DecoderData));
    if(Data==NULL)
        return NULL;

    Data->LastProcessedCharBuffSize=1;
    Data->LastProcessedChar=(uint8_t *)malloc(Data->LastProcessedCharBuffSize);
    if(Data->LastProcessedChar==NULL)
    {
        free(Data);
        return NULL;
    }

    ANSIX364Decoder_DefaultData(Data);

    return (t_DataProcessorHandleType *)Data;
}

/*******************************************************************************
 *  NAME:
 *    ANSIX364Decoder_FreeData
 *
 *  SYNOPSIS:
 *    void ANSIX364Decoder_FreeData(t_DataProcessorHandleType *DataHandle);
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
void ANSIX364Decoder_FreeData(t_DataProcessorHandleType *DataHandle)
{
    struct ANSIX364DecoderData *Data=(struct ANSIX364DecoderData *)DataHandle;

    if(Data->LastProcessedChar!=NULL)
        free(Data->LastProcessedChar);

    free(Data);
}

/*******************************************************************************
 * NAME:
 *    ANSIX364Decoder_GetProcessorInfo
 *
 * SYNOPSIS:
 *    const struct DataProcessorInfo *ANSIX364Decoder_GetProcessorInfo(
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
const struct DataProcessorInfo *ANSIX364Decoder_GetProcessorInfo(
        unsigned int *SizeOfInfo)
{
    *SizeOfInfo=sizeof(struct DataProcessorInfo);
    return &m_ANSIX364Decoder_Info;
}

/*******************************************************************************
 * NAME:
 *    ANSIX364Decoder_DefaultData
 *
 * SYNOPSIS:
 *    static void ANSIX364Decoder_DefaultData(struct ANSIX364DecoderData *Data);
 *
 * PARAMETERS:
 *    Data [I] -- The data to default
 *
 * FUNCTION:
 *    This function defaults the data for this plugin.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void ANSIX364Decoder_DefaultData(struct ANSIX364DecoderData *Data)
{
    Data->CurrentMode=e_ESCState_Normal;
    Data->CSIArgCount=0;
    Data->SearchAbortCount=0;
    Data->CurrentNum=0;
    Data->LastProcessedCharLen=0;

    Data->SavedCursorAttribs.SavedCursorX=0;
    Data->SavedCursorAttribs.SavedCursorY=0;
    Data->SavedCursorAttribs.SavedAttribs=0;
    Data->SavedCursorAttribs.SavedFGColor=
            m_DPS->GetSysDefaultColor(e_DefaultColors_FG);
    Data->SavedCursorAttribs.SavedBGColor=
            m_DPS->GetSysDefaultColor(e_DefaultColors_BG);
    Data->SavedCursorAttribs.SavedULineColor=
            m_DPS->GetSysDefaultColor(e_DefaultColors_FG);

    ANSIX364Decoder_ResetSGR(Data);
}

/*******************************************************************************
 * NAME:
 *   ANSIX364Decoder_ProcessKeyPress
 *
 * SYNOPSIS:
 *   PG_BOOL ANSIX364Decoder_ProcessKeyPress(
 *          t_DataProcessorHandleType *DataHandle,const uint8_t *KeyChar,
 *          int KeyCharLen,e_UIKeys ExtendedKey,uint8_t Mod)
 *
 * PARAMETERS:
 *   DataHandle [I] -- The data handle to work on.  This is your internal
 *                     data.
 *   KeyChar [I] -- The key that was pressed.  In a UTF-8 string.  This is
 *                  a single char but maybe any bytes.  This len will be in
 *                  'KeyCharLen'.
 *   KeyCharLen [I] -- The number of bytes in 'KeyChar'.  If this is 0
 *                     then 'ExtendedKey' is used for what key was pressed.
 *   ExtendedKey [I] -- The non text key that was pressed.  This includes
 *                      things like the arrow keys, function keys, etc.
 *                      This is only used if 'KeyCharLen' is 0.
 *   Mod [I] -- What modifier keys where held when this key was pressed.
 *              This is a bit field (so more than one key can be held.
 *              Supported bits:
 *                   KEYMOD_SHIFT -- Shift key
 *                   KEYMOD_CONTROL -- Control hey
 *                   KEYMOD_ALT -- Alt key
 *                   KEYMOD_LOGO -- Logo key (Windows, Amiga, Command, etc)
 *
 * FUNCTION:
 *   This function is called for each key pressed that would normally be
 *   sent out.  This includes extended keys like the arrows.  This is only
 *   called for data processors that are
 *   e_InputProcessorClass_TermEmulation only.
 *
 * RETURNS:
 *   true -- This key was used up (do not pass to other key processors)
 *   false -- This key should continue on it's way though the key press
 *            processor list.
 ******************************************************************************/
PG_BOOL ANSIX364Decoder_ProcessKeyPress(t_DataProcessorHandleType *DataHandle,
            const uint8_t *KeyChar,int KeyCharLen,e_UIKeys ExtendedKey,
            uint8_t Mod)
{
    uint8_t c; // The char we got
    uint8_t SendStr[10];    // The string we are going to send
    uint_fast8_t SendLen;

    c=*KeyChar;

    /* UTF8 char */
    if(KeyCharLen>1)
        return false;

    if(Mod&KEYMOD_CONTROL)
    {
        if(c>='A' && c<='Z')
        {
            SendStr[0]=c-'@';
            SendLen=1;
        }
        else
        {
            return false;
        }
    }
    else
    {
        if(Mod!=KEYMOD_NONE)
            return false;

        if(KeyCharLen>0)
            return false;

        switch(ExtendedKey)
        {
            case e_UIKeys_Escape:
                SendStr[0]=27;
                SendLen=1;
            break;
            case e_UIKeys_Tab:
                SendStr[0]=9;
                SendLen=1;
            break;
            case e_UIKeys_Backspace:
                m_DPS->SendBackspace();
            return true;
            case e_UIKeys_Return:
            case e_UIKeys_Enter:
                m_DPS->SendEnter();
            return true;
            case e_UIKeys_Delete:
                SendStr[0]=127;
                SendLen=1;
            break;
            case e_UIKeys_Up:
                SendStr[0]=27;
                SendStr[1]='[';
                SendStr[2]='A';
                SendLen=3;
            break;
            case e_UIKeys_Down:
                SendStr[0]=27;
                SendStr[1]='[';
                SendStr[2]='B';
                SendLen=3;
            break;
            case e_UIKeys_Left:
                SendStr[0]=27;
                SendStr[1]='[';
                SendStr[2]='D';
                SendLen=3;
            break;
            case e_UIKeys_Right:
                SendStr[0]=27;
                SendStr[1]='[';
                SendStr[2]='C';
                SendLen=3;
            break;
            case e_UIKeys_Backtab:
            case e_UIKeys_Insert:
            case e_UIKeys_Pause:
            case e_UIKeys_Print:
            case e_UIKeys_SysReq:
            case e_UIKeys_Clear:
            case e_UIKeys_Home:
            case e_UIKeys_End:
            case e_UIKeys_PageUp:
            case e_UIKeys_PageDown:
            case e_UIKeys_Shift:
            case e_UIKeys_Control:
            case e_UIKeys_Meta:
            case e_UIKeys_Alt:
            case e_UIKeys_AltGr:
            case e_UIKeys_CapsLock:
            case e_UIKeys_NumLock:
            case e_UIKeys_ScrollLock:
            case e_UIKeys_F1:
            case e_UIKeys_F2:
            case e_UIKeys_F3:
            case e_UIKeys_F4:
            case e_UIKeys_F5:
            case e_UIKeys_F6:
            case e_UIKeys_F7:
            case e_UIKeys_F8:
            case e_UIKeys_F9:
            case e_UIKeys_F10:
            case e_UIKeys_F11:
            case e_UIKeys_F12:
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
            case e_UIKeysMAX:
            default:
                return false;
        }
    }

    m_System->WriteData(SendStr,SendLen);

    return true;
}

/*******************************************************************************
 *  NAME:
 *    ANSIX364Decoder_ProcessByte
 *
 *  SYNOPSIS:
 *    void ANSIX364Decoder_ProcessIncomingTextByte(
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
void ANSIX364Decoder_ProcessIncomingTextByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed)
{
    struct ANSIX364DecoderData *Data=(struct ANSIX364DecoderData *)DataHandle;

    switch(Data->CurrentMode)
    {
        case e_ESCState_Normal:
            ANSIX364Decoder_ProcessNormalChar(Data,RawByte,ProcessedChar,
                    CharLen,Consumed);
        break;
        case e_ESCState_Search4Exit:
            /* We are searching for a char that will make us exit (and ignore)
               the current mode */
            if(RawByte!=':' && RawByte!=';' && (RawByte<'0' || RawByte>'9'))
            {
                Data->CurrentMode=e_ESCState_Normal;
            }
            *Consumed=true;
        break;
        case e_ESCState_Search4ST:
            /* Search until we find a ST (String Terminator) or we have to many
               chars (we don't want to get locked into this) */
            if((RawByte<0x08 || RawByte>0x0D) &&
                    (RawByte<0x20 || RawByte>0x7E) &&
                    RawByte!=0x1B)
            {
                /* This char is not in the valid range, abort */
                Data->CurrentMode=e_ESCState_Normal;
                break;
            }
            switch(Data->SearchIndex)
            {
                case 0: // First byte
                    if(RawByte==0x1B)   // ESC
                    {
                        /* Ok, we need to check the new char */
                        Data->SearchIndex++;
                    }
                break;
                case 1: // Second byte
                    if(RawByte=='\\')   // STRING TERMINATOR
                    {
                        /* Ok we are done */
                        Data->CurrentMode=e_ESCState_Normal;
                    }
                    else
                    {
                        Data->SearchIndex=0;
                    }
                break;
                default:
                break;
            }
            Data->SearchAbortCount++;
            if(Data->SearchAbortCount>MAX_SEARCH_ABORT_COUNT)
            {
                /* The string is too long so we abort (so we don't get stuck) */
                Data->CurrentMode=e_ESCState_Normal;
            }
            *Consumed=true;
        break;
        case e_ESCState_ESC:
            ANSIX364Decoder_ProcessESC(Data,RawByte,ProcessedChar,
                    CharLen,Consumed);
        break;
        case e_ESCState_CSI:
            ANSIX364Decoder_ProcessCSI(Data,RawByte,ProcessedChar,
                    CharLen,Consumed);
        break;
        case e_ESCState_CSIQuest:
            ANSIX364Decoder_ProcessCSIQuest(Data,RawByte,ProcessedChar,
                    CharLen,Consumed);
        break;
        case e_ESCStateMAX:
        default:
            /* Invalid mode switch back to normal */
            Data->CurrentMode=e_ESCState_Normal;
        break;
    }
}

void ANSIX364Decoder_ProcessNormalChar(struct ANSIX364DecoderData *Data,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed)
{
    const char *CodeStr;

    // https://en.wikipedia.org/wiki/C0_and_C1_control_codes
    CodeStr=NULL;
    switch(RawByte)
    {
        case 0: // NUL  NULL
            CodeStr="NUL";
        break;
        case 1: // SOH  Start of Heading
            CodeStr="SOH";
        break;
        case 2: // STX  Start of Text
            CodeStr="STX";
        break;
        case 3: // ETX  End of Text
            CodeStr="ETX";
        break;
        case 4: // EOT  End of Transmissionc
            CodeStr="EOT";
        break;
        case 5: // ENQ  Enquiry (Do we want an auto answer)
            CodeStr="ENQ";
        break;
        case 6: // ACK  Acknowledge
            CodeStr="ACK";
        break;
        case 7: // BEL  Bell
            CodeStr="BEL";
            m_DPS->DoSystemBell(false);
            *Consumed=true;
        break;
        case 8: // BS  Backspace
            /* Backspace */
//                CodeStr="BS";
            m_DPS->DoBackspace();
            *Consumed=true;
        break;
        case 9: // HT  Character Tabulation, Horizontal Tabulation
            m_DPS->DoTab();
            *Consumed=true;
        break;
        case 10:    // LF  Line Feed
            /* New line */
            m_DPS->DoNewLine();
//                CodeStr="LF";
            *Consumed=true;
        break;
        case 11:    // VT  Line Tabulation, Vertical Tabulation
            CodeStr="VT";
        break;
        case 12:    // FF  Form Feed
            m_DPS->DoClearScreen();
//                CodeStr="FF";
            *Consumed=true;
        break;
        case 13:    // CR  Carriage Return
            m_DPS->DoReturn();
//                CodeStr="CR";
            *Consumed=true;
        break;
        case 14:    // SO  Shift Out
            CodeStr="SO";
        break;
        case 15:    // SI  Shift In
            CodeStr="SI";
        break;
        case 16:    // DLE  Data Link Escape
            CodeStr="DLE";
        break;
        case 17:    // DC1  Device Control One (XON)
            CodeStr="DC1";
        break;
        case 18:    // DC2  Device Control Two
            CodeStr="DC2";
        break;
        case 19:    // DC3  Device Control Three (XOFF)
            CodeStr="DC3";
        break;
        case 20:    // DC4  Device Control Four
            CodeStr="DC4";
        break;
        case 21:    // NAK  Negative Acknowledge
            CodeStr="NAK";
        break;
        case 22:    // SYN  Synchronous Idle
            CodeStr="SYN";
        break;
        case 23:    // ETB  End of Transmission Block
            CodeStr="ETB";
        break;
        case 24:    // CAN  Cancel
            CodeStr="CAN";
        break;
        case 25:    // EM  End of medium
            CodeStr="EM";
        break;
        case 26:    // SUB  Substitute
            CodeStr="SUB";
        break;
        case 27:    // ESC  Escape
//                CodeStr="ESC";
            Data->CSIArgCount=0;
            Data->CurrentNum=0;
            Data->CSIArg[0]=0;
            Data->CurrentMode=e_ESCState_ESC;
            *Consumed=true;
        break;
        case 28:    // FS  File Separator
            CodeStr="FS";
        break;
        case 29:    // GS  Group Separator
            CodeStr="GS";
        break;
        case 30:    // RS  Record Separator
            CodeStr="RS";
        break;
        case 31:    // US  Unit Separator
            CodeStr="US";
        break;
        case 127:   // DEL  Delete
            CodeStr="DEL";
        break;
        default:
            /* Note what char this was */
            Data->LastProcessedCharLen=*CharLen;
            if(*CharLen>Data->LastProcessedCharBuffSize)
            {
                uint8_t *NewBuffer;

                /* We need a bigger buffer */
                NewBuffer=(uint8_t *)realloc(Data->LastProcessedChar,*CharLen);
                if(NewBuffer!=NULL)
                {
                    Data->LastProcessedChar=NewBuffer;
                    Data->LastProcessedCharBuffSize=*CharLen;
                }
                else
                {
                    Data->LastProcessedCharLen=0;
                }
            }
            if(Data->LastProcessedCharLen>0)
                memcpy(Data->LastProcessedChar,ProcessedChar,*CharLen);
        break;
    }
    if(CodeStr!=NULL)
    {
        m_DPS->NoteNonPrintable(CodeStr);
        *Consumed=true;
    }
}

void ANSIX364Decoder_ProcessCSI(struct ANSIX364DecoderData *Data,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed)
{
    /* We keep going until we see something that isn't a "0-9:;<=>?" */
    *Consumed=true;

    switch(RawByte)
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            /* Add to the number */
            Data->CurrentNum*=10;
            Data->CurrentNum+=RawByte-'0';
        break;
        case ':':   // Sub arg...
        break;
        case ';':
            /* Next arg */
            if(Data->CSIArgCount<sizeof(Data->CSIArg)/sizeof(int))
                Data->CSIArg[Data->CSIArgCount++]=Data->CurrentNum;
            Data->CurrentNum=0;
        break;
        case '?':   // Private parameter strings
            Data->CSIArgCount=0;
            Data->CurrentNum=0;
            Data->CSIArg[0]=0;
            Data->CurrentMode=e_ESCState_CSIQuest;
        break;
        case '<':   // Private parameter strings
        case '=':   // Private parameter strings
        case '>':   // Private parameter strings
        break;
        default:
            /* This is the end of the command */
            if(Data->CSIArgCount<sizeof(Data->CSIArg)/sizeof(int))
                Data->CSIArg[Data->CSIArgCount++]=Data->CurrentNum;

            ANSIX364Decoder_DoCSICommand(Data,RawByte,ProcessedChar,CharLen,
                    Consumed);
            Data->CurrentMode=e_ESCState_Normal;
        break;
    }
}

void ANSIX364Decoder_DoCSICommand(struct ANSIX364DecoderData *Data,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed)
{
    int32_t CursorX,CursorY;
    int p1;
    int p2;
    int r;
    int32_t NewPos;
    char buff[100];

    /* Setup defaults args for the first 2 args (defaulting to 1) */
    p1=1;
    p2=1;
    if(Data->CSIArgCount>=1)
        p1=Data->CSIArg[0];
    if(Data->CSIArgCount>=2)
        p2=Data->CSIArg[1];

    if(p1<1)
        p1=1;
    if(p2<1)
        p2=1;

    /* Get the cursor x,y because a lot of stuff uses them */
    m_DPS->GetCursorXY(&CursorX,&CursorY);

    switch(RawByte)
    {
        case 'N':   // EF
        case 'O':   // EA
        case 'Q':   // SSE
        case 'R':   // CPR -- Active Position Report (reply to cursor pos)
        case 'U':   // NP
        case 'V':   // PP
        case 'W':   // CTC
        case 'Y':   // CVT
        case '[':   // SRS
        case '\\':  // PTX
        case ']':   // SDS
        case '^':   // SIMD
        case 'c':   // DA
        case 'h':   // SM
        case 'i':   // MC - MEDIA COPY
        case 'o':   // DAQ
        case 'p':   // Private Use
        case 'q':   // Private Use
        case 'r':   // Private Use
        case 'v':   // Private Use
        case 'w':   // Private Use
        case 'x':   // Private Use
        case 'y':   // Private Use
        case 'z':   // Private Use
        case 't':   // Private Use
            /* Unsupported */
#ifdef LOG_UNKNOWN_CODES
            {
                FILE *out;
                unsigned int r;
                out=fopen(LOG_UNKNOWN_CODES_FILENAME,"a");
                if(out!=NULL)
                {
                    fprintf(out,"\\e[");
                    for(r=0;r<Data->CSIArgCount;r++)
                    {
                        fprintf(out,"%d",Data->CSIArg[r]);
                        if(r!=Data->CSIArgCount-1)
                            fprintf(out,";");
                    }
                    fprintf(out,"%c\n",RawByte);
                    fclose(out);
                }
            }
#endif
        break;
        case '@':   // ICH
            /* We need to push all the chars to the right at the cursor
               (insert x spaces) */
            m_DPS->DoScrollArea(CursorX,CursorY,-1,CursorY+1,p1,0);
        break;
        case 'A':   // CUU - Cursor Up
            NewPos=CursorY-p1;
            if(NewPos<0)
                NewPos=0;
            m_DPS->DoMoveCursor(CursorX,NewPos);
        break;
        case 'B':   // CUD - Cursor Down
            NewPos=CursorY+p1;
            m_DPS->DoMoveCursor(CursorX,NewPos);
        break;
        case 'a':   // HPR - Horizontal Position Relative
        case 'C':   // CUF - Cursor Forward
            NewPos=CursorX+p1;
            m_DPS->DoMoveCursor(NewPos,CursorY);
        break;
        case 'D':   // CUB - Cursor Back
            NewPos=CursorX-p1;
            m_DPS->DoMoveCursor(NewPos,CursorY);
        break;
        case 'E':   // CNL - Cursor Next Line (Move to right edge and down)
            NewPos=CursorY+p1;
            m_DPS->DoMoveCursor(0,NewPos);
        break;
        case 'F':   // CPL - Cursor Previous Line (Move to right edge and up)
            NewPos=CursorY-p1;
            if(NewPos<0)
                NewPos=0;
            m_DPS->DoMoveCursor(0,NewPos);
        break;
        case '`':   // HPA - Horizontal Position Absolute
        case 'G':   // CHA - Cursor Horizontal Absolute
            m_DPS->DoMoveCursor(p1-1,CursorY);
        break;
        case 'f':   // HVP - Horizontal Vertical Position
        case 'H':   // CUP - Cursor Position
            m_DPS->DoMoveCursor(p2-1,p1-1);
        break;
        case 'I':   // CHT - Do x tabs
            for(r=0;r<p2;r++)
                m_DPS->DoTab();
        break;
        case 'J':   // ED - Erase in Display
            p1=0;
            if(Data->CSIArgCount>=1)
                p1=Data->CSIArg[0];
            switch(p1)
            {
                case 0: // From the cursor through the end of the display
                    m_DPS->GetCursorXY(&CursorX,&CursorY);

                    if(CursorX==0 && CursorY==0)
                    {
                        /* This is really a clear screen */
                        m_DPS->DoClearScreen();
                    }
                    else
                    {
                        /* We can only clear rectangles so we have to do this
                           in 2 steps, part of the line and everything else */
                        m_DPS->DoClearArea(CursorX,CursorY,-1,CursorY);
                        m_DPS->DoClearArea(0,CursorY+1,-1,-1);
                    }
                break;
                case 1: // From the beginning of the display through the cursor
                    m_DPS->GetCursorXY(&CursorX,&CursorY);
                    if(CursorY!=0)
                        m_DPS->DoClearArea(0,0,-1,CursorY-1);
                    m_DPS->DoClearArea(0,CursorY,CursorX+1,CursorY);
                break;
                case 2: // The complete display
                    m_DPS->GetCursorXY(&CursorX,&CursorY);
                    m_DPS->DoClearScreen();
                    m_DPS->DoMoveCursor(CursorX,CursorY);
                break;
                case 3: // clear entire screen and delete all lines saved in the scrollback buffer (from xterm)
                    m_DPS->DoClearScreenAndBackBuffer();
                break;
                default:
                break;
            }
        break;
        case 'K':   // EL - Erase in Line
            p1=0;
            if(Data->CSIArgCount>=1)
                p1=Data->CSIArg[0];

            switch(p1)
            {
                case 0: // Erase to end
                    m_DPS->DoClearArea(CursorX,CursorY,-1,CursorY);
                break;
                case 1: // Erase to start
                    m_DPS->DoClearArea(0,CursorY,CursorX,CursorY);
                break;
                case 2: // Erase line
                    m_DPS->DoClearArea(0,CursorY,-1,CursorY);
                break;
                default:
                break;
            }
        break;
        case 'L':   // IL - Insert lines
            m_DPS->DoScrollArea(0,CursorY,-1,-1,0,p1);
        break;
        case 'M':   // DL - Delete Line
            m_DPS->DoScrollArea(0,CursorY,-1,-1,0,-p1);
        break;
        case 'P':   // DCH - Delete Character
            m_DPS->DoScrollArea(CursorX,CursorY,-1,CursorY+1,-p1,0);
        break;
        case 'S':   // SU - Scroll Up
            m_DPS->DoScrollArea(0,0,-1,-1,0,-p1);
        break;
        case 'T':   // SD - Scroll Down
            m_DPS->DoScrollArea(0,0,-1,-1,0,p1);
        break;
        case 'X':   // ECH - Erase Character
            m_DPS->DoClearArea(CursorX,CursorY,CursorX+p1,CursorY+1);
        break;
        case 'Z':   // CBT - Cursor Backward Tabulation
            for(r=0;r<p1;r++)
                m_DPS->DoPrevTab();
        break;
        case 'b':   // REP - Repeat Preceding Character
            for(r=0;r<p1;r++)
                m_DPS->InsertString((uint8_t *)Data->LastProcessedChar,1);
        break;
        case 'd':   // VPA - Vertical Position Absolute
            m_DPS->DoMoveCursor(CursorX,p1-1);
        break;
        case 'e':   // VPR - Vertical Position Relative
            NewPos=CursorY+p1;
            m_DPS->DoMoveCursor(CursorX,NewPos);
        break;
        case 'g':   // TBC - TABULATION CLEAR
            /* Not supported */
        break;
        case 'j':   // HPB - CHARACTER POSITION BACKWARD
            /* Not supported */
        break;
        case 'k':   // VPB - LINE POSITION BACKWARD
            /* Not supported */
        break;
        case 'l':   // RM - RESET MODE
            /* Not supported */
        break;
        case 'm':   // SGR - Select Graphic Rendition
            ANSIX364Decoder_HandleSGR(Data);
            *Consumed=true;
        break;
        case 'n':   // DSR - Device Status Report
            p1=0;
            if(Data->CSIArgCount>=1)
                p1=Data->CSIArg[0];
            switch(p1)
            {
                case 0: // ready, no malfunction detected
                case 1: // busy, another DSR must be requested later
                case 2: // busy, another DSR will be sent later
                case 3: // some malfunction detected, another DSR must be requested later
                case 4: // some malfunction detected, another DSR will be sent later
                break;
                case 5: // a DSR is requested
                    /* Always report that we are ready */
                    m_System->WriteData((uint8_t *)"\33[0n",4);
                break;
                case 6: // Cursor Position Report
                    m_DPS->GetCursorXY(&CursorX,&CursorY);
                    sprintf(buff,"\33[%d;%dR",CursorY+1,CursorX+1);   // CPR
                    m_System->WriteData((uint8_t *)buff,
                            strlen(buff));
                break;
                default:
                break;
            }
        break;
        case 's':   // SCP - Save Cursor Position
            m_DPS->GetCursorXY(&Data->SavedCursorAttribs.SavedCursorX,
                    &Data->SavedCursorAttribs.SavedCursorY);
        break;
        case 'u':   // RCP - Restore Cursor Position
            m_DPS->DoMoveCursor(Data->SavedCursorAttribs.SavedCursorX,
                    Data->SavedCursorAttribs.SavedCursorY);
        break;
        default:
        break;
    }
}

void ANSIX364Decoder_HandleSGR(struct ANSIX364DecoderData *Data)
{
    unsigned int r;
    uint32_t TmpCol;
    e_SysColShadeType Shade;
    uint32_t Attribs;
    uint32_t FGColor;
    uint32_t BGColor;
    uint32_t ULineColor;

    FGColor=m_DPS->GetFGColor();
    BGColor=m_DPS->GetBGColor();
    Attribs=m_DPS->GetAttribs();
    ULineColor=m_DPS->GetULineColor();

    for(r=0;r<Data->CSIArgCount;r++)
    {
        switch(Data->CSIArg[r])
        {
            case 0: // 0 default rendition (implementation-defined), cancels the effect of any preceding occurrence of SGR in the data stream regardless of the setting of the GRAPHIC RENDITION COMBINATION MODE (GRCM)
                ANSIX364Decoder_ResetSGR(Data);
                FGColor=m_DPS->GetSysDefaultColor(e_DefaultColors_FG);
                BGColor=m_DPS->GetSysDefaultColor(e_DefaultColors_BG);
                Attribs=0;
                ULineColor=FGColor;
            break;
            case 1: // bold or increased intensity
                Attribs|=TXT_ATTRIB_BOLD;
                Data->DoingBright=true;
            break;
            case 2: // faint, decreased intensity or second colour
                Data->DoingDim=true;
            break;
            case 3: // italicized
                Attribs|=TXT_ATTRIB_ITALIC;
            break;
            case 4: // singly underlined
                Attribs|=TXT_ATTRIB_UNDERLINE;
                ULineColor=FGColor;
            break;
            case 5: // slowly blinking (less then 150 per minute)
            break;
            case 6: // rapidly blinking (150 per minute or more)
            break;
            case 7: // negative image
                Attribs|=TXT_ATTRIB_REVERSE;
            break;
            case 8: // concealed characters
            break;
            case 9: // crossed-out (characters still legible but marked as to be deleted)
                Attribs|=TXT_ATTRIB_LINETHROUGHT;
            break;
            case 10: // primary (default) font
            case 11: // first alternative font
            case 12: // second alternative font
            case 13: // third alternative font
            case 14: // fourth alternative font
            case 15: // fifth alternative font
            case 16: // sixth alternative font
            case 17: // seventh alternative font
            case 18: // eighth alternative font
            case 19: // ninth alternative font
            case 20: // Fraktur (Gothic)
            break;
            case 21: // doubly underlined
                Attribs|=TXT_ATTRIB_UNDERLINE_DOUBLE;
                ULineColor=FGColor;
            break;
            case 22: // normal colour or normal intensity (neither bold nor faint)
                Attribs&=~TXT_ATTRIB_BOLD;
                Data->DoingDim=false;
                Data->DoingBright=false;
            break;
            case 23: // not italicized, not fraktur
                Attribs&=~TXT_ATTRIB_ITALIC;
            break;
            case 24: // not underlined (neither singly nor doubly)
                Attribs&=~(TXT_ATTRIB_UNDERLINE|TXT_ATTRIB_UNDERLINE_DOUBLE);
            break;
            case 25: // steady (not blinking)
            case 26: // (reserved for proportional spacing as specified in CCITT Recommendation T.61)
            break;
            case 27: // positive image
                Attribs&=~TXT_ATTRIB_REVERSE;
            break;
            case 28: // revealed characters
            break;
            case 29: // not crossed out
                Attribs&=~TXT_ATTRIB_LINETHROUGHT;
            break;
            case 30: // black display
            case 31: // red display
            case 32: // green display
            case 33: // yellow display
            case 34: // blue display
            case 35: // magenta display
            case 36: // cyan display
            case 37: // white display
                Shade=e_SysColShade_Normal;
                if(Data->DoingDim)
                    Shade=e_SysColShade_Dark;
                if(Data->DoingBright)
                    Shade=e_SysColShade_Bright;
                TmpCol=m_DPS->GetSysColor(Shade,Data->CSIArg[r]-30);
                FGColor=TmpCol;
            break;
            case 38: // (reserved for future standardization; intended for setting character foreground colour as specified in ISO 8613-6 [CCITT Recommendation T.416])
            break;
            case 39: // default display colour (implementation-defined)
                FGColor=m_DPS->GetSysDefaultColor(e_DefaultColors_FG);
            break;
            case 40: // black background
            case 41: // red background
            case 42: // green background
            case 43: // yellow background
            case 44: // blue background
            case 45: // magenta background
            case 46: // cyan background
            case 47: // whitebackground
                TmpCol=m_DPS->GetSysColor(e_SysColShade_Normal,Data->CSIArg[r]-40);
                BGColor=TmpCol;
            break;
            case 48: // (reserved for future standardization; intended for setting character background colour as specified in ISO 8613-6 [CCITT Recommendation T.416])
            break;
            case 49: // default background colour (implementation-defined)
                BGColor=m_DPS->GetSysDefaultColor(e_DefaultColors_BG);
            break;
            case 50: // (reserved for cancelling the effect of the rendering aspect established by parameter value 26)
            break;
            case 51: // framed
            case 52: // encircled
            break;
            case 53: // overlined
                Attribs|=TXT_ATTRIB_OVERLINE;
                ULineColor=FGColor;
            break;
            case 54: // not framed, not encircled
            break;
            case 55: // not overlined
                Attribs&=~TXT_ATTRIB_OVERLINE;
            break;
            case 56: // (reserved for future standardization)
            case 57: // (reserved for future standardization)
            case 58: // (reserved for future standardization)
            case 59: // (reserved for future standardization)
            case 60: // ideogram underline or right side line
            case 61: // ideogram double underline or double line on the right side
            case 62: // ideogram overline or left side line
            case 63: // ideogram double overline or double line on the left side
            case 64: // ideogram stress marking
            case 65: // cancels the effect of the rendition aspects established by parameter values 60 to 64
            break;
            case 90: // bright black display
            case 91: // bright red display
            case 92: // bright green display
            case 93: // bright yellow display
            case 94: // bright blue display
            case 95: // bright magenta display
            case 96: // bright cyan display
            case 97: // bright white display
                TmpCol=m_DPS->GetSysColor(e_SysColShade_Bright,Data->CSIArg[r]-90);
                FGColor=TmpCol;
            break;
            case 100: // bright background black display
            case 101: // bright background red display
            case 102: // bright background green display
            case 103: // bright background yellow display
            case 104: // bright background blue display
            case 105: // bright background magenta display
            case 106: // bright background cyan display
            case 107: // bright background white display
                TmpCol=m_DPS->GetSysColor(e_SysColShade_Bright,Data->CSIArg[r]-100);
                BGColor=TmpCol;
            break;
            default:
            break;
        }
    }
    m_DPS->SetFGColor(FGColor);
    m_DPS->SetBGColor(BGColor);
    m_DPS->SetAttribs(Attribs);
    m_DPS->SetULineColor(ULineColor);
}

void ANSIX364Decoder_ResetSGR(struct ANSIX364DecoderData *Data)
{
    Data->DoingDim=false;
    Data->DoingBright=false;
}

/*******************************************************************************
 * NAME:
 *    ANSIX364Decoder_ProcessESC
 *
 * SYNOPSIS:
 *    void ANSIX364Decoder_ProcessESC(struct ANSIX364DecoderData *Data,
 *          const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
 *          PG_BOOL *Consumed);
 *
 * PARAMETERS:
 *    Data [I] -- Our internal data.
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
 * FUNCTION:
 *    This function handles when we are 'ESC' mode.  We have just seen an
 *    ESC and we are trying to decide what to with the first byte after it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    ANSIX364Decoder_ProcessIncomingTextByte()
 ******************************************************************************/
void ANSIX364Decoder_ProcessESC(struct ANSIX364DecoderData *Data,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed)
{
    int32_t NewPos;
    int32_t CursorX,CursorY;

    *Consumed=true;

    // https://en.wikipedia.org/wiki/C0_and_C1_control_codes
    // https://www.gnu.org/software/screen/manual/html_node/Control-Sequences.html
    switch(RawByte)
    {
        case 27:    // ESC
            /* Pass it along */
            *Consumed=false;
        break;

        /* We don't support these (or they are not defined) */
        case '0':   // 
        case '1':   // 
        case '2':   // 
        case '3':   // 
        case '4':   // 
        case '5':   // 
        case '6':   // Back Index (VT510)
        case '9':   // Forward Index (VT510)
        case '<':   // 
        case '=':   // DECNKM  Numeric Keypad Mode (extention?)
        case '>':   // DECNKM  Numeric Keypad Mode (extention?)
        case '@':   // Padding Character
        case 'A':   // High Octet Preset
        case 'B':   // BPH  Break Permitted Here
        case 'C':   // NBH  No Break Here
        case 'F':   // SSA  Start of Selected Area
        case 'G':   // ESA  End of Selected Area
        case 'I':   // HTJ  Character Tabulation With Justification Horizontal Tabulation With Justification
        case 'J':   // VTS  Line Tabulation Set Vertical Tabulation Set
        case 'K':   // PLD  Partial Line Forward Partial Line Down
        case 'L':   // PLU  Partial Line Backward Partial Line Up
        case 'N':   // SS2  Single-Shift 2
        case 'O':   // SS3  Single-Shift 3
        case 'Q':   // PU1  Private Use 1
        case 'R':   // PU2  Private Use 2
        case 'S':   // STS  Set Transmit State
        case 'T':   // CCH  Cancel character
        case 'U':   // MW  Message Waiting
        case 'V':   // SPA  Start of Protected Area
        case 'W':   // EPA  End of Protected Area
        case 'Y':   // Single Graphic Character Introducer
        case 'Z':   // SCI  Single Character Introducer
        case '\\':  // ST  String Terminator
        case '`':   // DMI  DISABLE MANUAL INPUT
        case 'a':   // INT  INTERRUPT
        case 'b':   // EMI  ENABLE MANUAL INPUT
        case 'd':   // CMD  CODING METHOD DELIMITER
        case 'e':   // 
        case 'f':   // 
        case 'h':   // 
        case 'i':   // 
        case 'j':   // 
        case 'l':   // 
        case 'm':   // 
        case 'n':   // LOCKING-SHIFT TWO
        case 'o':   // LOCKING-SHIFT THREE
        case 'p':   // 
        case 'q':   // 
        case 'r':   // 
        case 's':   // 
        case 't':   // 
        case 'u':   // 
        case 'v':   // 
        case 'w':   // 
        case 'x':   // 
        case 'y':   // 
        case 'z':   // 
        case '{':   // 
        case '|':   // LS3R
        case '}':   // LS2R
        case '~':   // LS1R
#ifdef LOG_UNKNOWN_CODES
            {
                FILE *out;
                out=fopen(LOG_UNKNOWN_CODES_FILENAME,"a");
                if(out!=NULL)
                {
                    fprintf(out,"\\e%c\n",RawByte);
                    fclose(out);
                }
            }
#endif
        break;
        case '7':   // Save Cursor and Attributes
            m_DPS->GetCursorXY(&Data->SavedCursorAttribs.SavedCursorX,
                    &Data->SavedCursorAttribs.SavedCursorY);
            Data->SavedCursorAttribs.SavedFGColor=m_DPS->GetFGColor();
            Data->SavedCursorAttribs.SavedBGColor=m_DPS->GetBGColor();
            Data->SavedCursorAttribs.SavedAttribs=m_DPS->GetAttribs();
            Data->SavedCursorAttribs.SavedULineColor=m_DPS->GetULineColor();
        break;
        case '8':   // Restore Cursor and Attributes
            m_DPS->DoMoveCursor(Data->SavedCursorAttribs.SavedCursorX,
                    Data->SavedCursorAttribs.SavedCursorY);
            m_DPS->SetFGColor(Data->SavedCursorAttribs.SavedFGColor);
            m_DPS->SetBGColor(Data->SavedCursorAttribs.SavedBGColor);
            m_DPS->SetAttribs(Data->SavedCursorAttribs.SavedAttribs);
            m_DPS->SetULineColor(Data->SavedCursorAttribs.SavedULineColor);
        break;
        case '?':   // Question mark mode (private commands)
            Data->CurrentMode=e_ESCState_Search4Exit;
        break;
        case 'D':   // Index
            m_DPS->DoNewLine();
        break;
        case 'E':   // NEL  Next Line
            m_DPS->DoReturn();
            m_DPS->DoNewLine();
        break;
        case 'H':   // HTS  Character Tabulation Set Horizontal Tabulation Set
            /* Not supported */
        break;
        case 'M':   // RI  Reverse Line Feed Reverse Index
            m_DPS->GetCursorXY(&CursorX,&CursorY);
            NewPos=CursorY-1;
            if(NewPos<0)
                NewPos=0;
            m_DPS->DoMoveCursor(CursorX,NewPos);
        break;
        case 'P':   // DCS  Device Control String
            Data->SearchAbortCount=0;
            Data->SearchIndex=0;
            Data->CurrentMode=e_ESCState_Search4ST;
        break;
        case 'X':   // SOS  Start of String
            Data->SearchAbortCount=0;
            Data->SearchIndex=0;
            Data->CurrentMode=e_ESCState_Search4ST;
        break;
        case '[':   // CSI  Control Sequence Introducer (CSI)
            Data->CurrentMode=e_ESCState_CSI;
        break;
        case ']':   // OSC  Operating System Command
        case '^':   // PM  Privacy Message
        case '_':   // APC  Application Program Command
            Data->SearchAbortCount=0;
            Data->SearchIndex=0;
            Data->CurrentMode=e_ESCState_Search4ST;
        break;
        case 'c':   // RIS  RESET TO INITIAL STATE
            ANSIX364Decoder_DefaultData(Data);

            m_DPS->SetFGColor(m_DPS->GetSysDefaultColor(e_DefaultColors_FG));
            m_DPS->SetBGColor(m_DPS->GetSysDefaultColor(e_DefaultColors_BG));
            m_DPS->SetAttribs(0);
            m_DPS->SetULineColor(m_DPS->GetSysDefaultColor(e_DefaultColors_FG));

            m_DPS->DoClearScreen();
            m_DPS->DoClearScreenAndBackBuffer();
        break;
        case 'g':   // Visual Bell
            m_DPS->DoSystemBell(true);
        break;
        case 'k':   // Title Definition String
            /* Not supported */
        break;

        default:
        break;
    }

    /* If we are still in the ESC mode then switch back to normal */
    if(Data->CurrentMode==e_ESCState_ESC)
        Data->CurrentMode=e_ESCState_Normal;
}

/*******************************************************************************
 * NAME:
 *    ANSIX364Decoder_ProcessCSIQuest
 *
 * SYNOPSIS:
 *    void ANSIX364Decoder_ProcessCSIQuest(struct ANSIX364DecoderData *Data,
 *          const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
 *          PG_BOOL *Consumed);
 *
 * PARAMETERS:
 *    Data [I] -- Our internal data.
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
 * FUNCTION:
 *    This function handles when we are 'CSI' question mark mode (private
 *    parameter string).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    ANSIX364Decoder_ProcessIncomingTextByte()
 ******************************************************************************/
void ANSIX364Decoder_ProcessCSIQuest(struct ANSIX364DecoderData *Data,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed)
{
    *Consumed=true;

    switch(RawByte)
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            /* Add to the number */
            Data->CurrentNum*=10;
            Data->CurrentNum+=RawByte-'0';
        break;
        case ':':   // Sub arg...
        case ';':   // Next arg
        break;
        default:
            /* This is the end of the command */
            if(Data->CSIArgCount<sizeof(Data->CSIArg)/sizeof(int))
                Data->CSIArg[Data->CSIArgCount++]=Data->CurrentNum;

            ANSIX364Decoder_DoCSIQuestCommand(Data,RawByte,ProcessedChar,
                    CharLen,Consumed);
            Data->CurrentMode=e_ESCState_Normal;
        break;
    }
}

void ANSIX364Decoder_DoCSIQuestCommand(struct ANSIX364DecoderData *Data,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed)
{
    switch(RawByte)
    {
        case 'l':   // Reset
//            switch(Data->CurrentNum)
//            {
//                case 
//            }
        break;
        case 'h':   // Set
        break;
        default:
#ifdef LOG_UNKNOWN_CODES
            {
                FILE *out;
                out=fopen(LOG_UNKNOWN_CODES_FILENAME,"a");
                if(out!=NULL)
                {
                    fprintf(out,"\\e[?%d%c\n",Data->CurrentNum,RawByte);
                    fclose(out);
                }
            }
#endif
        break;
    }
}
