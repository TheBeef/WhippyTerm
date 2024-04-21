// https://vt100.net/docs/vt510-rm/chapter4.html

// Also see:https://stackoverflow.com/questions/44116977/get-mouse-position-in-pixels-using-escape-sequences
// http://invisible-island.net/xterm/ctlseqs/ctlseqs.html
// https://godoc.org/github.com/pborman/ansi

/*******************************************************************************
 * FILENAME: ANSIX3_64.c
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
#include "ANSIX3_64.h"
#include "PluginSDK/Plugin.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*** DEFINES                  ***/
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      ANSIX3_64 // The name to append on the RegisterPlugin() function for built in version

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct ANSIX364DecoderData
{
    bool ProcessingESC;
    bool ProcessingCSI;
    int CSIArg[10];
    unsigned int CSIArgCount;
    int CurrentNum;
    bool DoingReverseVideo;
    bool DoingDim;
    bool DoingBright;
    int32_t SavedCursorX;
    int32_t SavedCursorY;
};

/*** FUNCTION PROTOTYPES      ***/
t_DataProcessorHandleType *ANSIX364Decoder_AllocateData(void);
void ANSIX364Decoder_FreeData(t_DataProcessorHandleType *DataHandle);
const struct DataProcessorInfo *ANSIX364Decoder_GetProcessorInfo(unsigned int *SizeOfInfo);
void ANSIX364Decoder_ProcessIncomingByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed);
void ANSIX364Decoder_HandleSGR(struct ANSIX364DecoderData *Data);
void ANSIX364Decoder_ProcessCSI(struct ANSIX364DecoderData *Data,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed);
void ANSIX364Decoder_ResetSGR(struct ANSIX364DecoderData *Data);
PG_BOOL ANSIX364Decoder_ProcessKeyPress(t_DataProcessorHandleType *DataHandle,
            const uint8_t *KeyChar,int KeyCharLen,e_UIKeys ExtendedKey,
            uint8_t Mod);

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
    ANSIX364Decoder_ProcessIncomingByte,
};
struct DataProcessorInfo m_ANSIX364Decoder_Info=
{
    "ANSI",
    "ANSI escape sequences",
    "ANSI X3.64, ISO 6429, and Digital VT100 escape sequences", // NOTE: ISO/IEC 6429:1992 is the latest
    e_DataProcessorType_Text,
    e_DataProcessorClass_TermEmulation
};

/*******************************************************************************
 * NAME:
 *    ANSIX3_64_RegisterPlugin
 *
 * SYNOPSIS:
 *    void ANSIX3_64_RegisterPlugin(const struct PI_SystemAPI *SysAPI);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function registers this plugin with the system.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    This function name comes from PluginSDK/Plugin.h.  It will make it
 *    a built in plugin or an extern plugin depending on the build env.
 *    The name comes from 'REGISTER_PLUGIN_FUNCTION_PRIV_NAME' if it's
 *    build in, or is RegisterPlugin() if it's extern.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
/* This needs to be extern "C" because it is the main entry point for the
   plugin system */
extern "C"
{
    void REGISTER_PLUGIN_FUNCTION(const struct PI_SystemAPI *SysAPI)
    {
        m_System=SysAPI;
        m_DPS=SysAPI->GetAPI_DataProcessors();
        m_UIAPI=m_DPS->GetAPI_UI();

        m_DPS->RegisterDataProcessor("ANSIX364Decoder",&m_ANSIX364DecoderAPI,
                sizeof(m_ANSIX364DecoderAPI));
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

    Data->ProcessingESC=false;
    Data->ProcessingCSI=false;
    Data->CSIArgCount=0;
    Data->CurrentNum=0;
    Data->SavedCursorX=0;
    Data->SavedCursorY=0;

    ANSIX364Decoder_ResetSGR(Data);

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
                SendStr[0]=8;
                SendLen=1;
            break;
            case e_UIKeys_Return:
                SendStr[0]=13;
                SendLen=1;
            break;
            case e_UIKeys_Enter:
                SendStr[0]=10;
                SendLen=1;
            break;
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
 *    void ANSIX364Decoder_ProcessIncomingByte(
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
void ANSIX364Decoder_ProcessIncomingByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed)
{
    struct ANSIX364DecoderData *Data=(struct ANSIX364DecoderData *)DataHandle;
    const char *CodeStr;

    if(Data->ProcessingCSI)
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
            case ':':
            break;
            case ';':
                /* Next arg */
                if(Data->CSIArgCount<sizeof(Data->CSIArg)/sizeof(int))
                    Data->CSIArg[Data->CSIArgCount++]=Data->CurrentNum;
                Data->CurrentNum=0;
            break;
            case '<':
            case '=':
            case '>':
            default:
                /* This is the end of the command */
                if(Data->CSIArgCount<sizeof(Data->CSIArg)/sizeof(int))
                    Data->CSIArg[Data->CSIArgCount++]=Data->CurrentNum;

                ANSIX364Decoder_ProcessCSI(Data,RawByte,ProcessedChar,CharLen,
                        Consumed);
                Data->ProcessingCSI=false;
            break;
        }
    }
    else if(Data->ProcessingESC)
    {
        *Consumed=true;
        Data->ProcessingESC=false;
        // https://en.wikipedia.org/wiki/C0_and_C1_control_codes
        // https://www.gnu.org/software/screen/manual/html_node/Control-Sequences.html
        switch(RawByte)
        {
            case 27:    // ESC
                /* Pass it along */
                *Consumed=false;
            break;
            case '0':   // 
            case '1':   // 
            case '2':   // 
            case '3':   // 
            case '4':   // 
            case '5':   // 
            case '6':   // 
            case '7':   // DEBUG PAUL: Save Cursor and Attributes
            case '8':   // DEBUG PAUL: Restore Cursor and Attributes
            case '9':   // 
            break;
            case '@':   // Padding Character
            case 'A':   // High Octet Preset
            case 'B':   // Break Permitted Here
            case 'C':   // No Break Here
            case 'D':   // Index
            break;
            case 'E':   // Next Line
                m_DPS->DoReturn();
                m_DPS->DoNewLine();
            break;
            case 'F':   // Start of Selected Area
            case 'G':   // End of Selected Area
            case 'H':   // Character Tabulation Set Horizontal Tabulation Set
            case 'I':   // Character Tabulation With Justification Horizontal Tabulation With Justification
            case 'J':   // Line Tabulation Set Vertical Tabulation Set
            case 'K':   // Partial Line Forward Partial Line Down
            case 'L':   // Partial Line Backward Partial Line Up
            case 'M':   // Reverse Line Feed Reverse Index
            case 'N':   // Single-Shift 2
            case 'O':   // Single-Shift 3
            case 'P':   // Device Control String
            case 'Q':   // PU1 -- Private Use 1
            case 'R':   // PU2 -- Private Use 2
            case 'S':   // Set Transmit State
            break;
            case 'T':   // Cancel character
                /* Backspace */
                m_DPS->DoBackspace();
                m_DPS->InsertString((uint8_t *)" ",1);
                m_DPS->DoBackspace();
            break;
            case 'U':   // Message Waiting
            case 'V':   // Start of Protected Area
            case 'W':   // End of Protected Area
            case 'X':   // Start of String
            case 'Y':   // Single Graphic Character Introducer
            case 'Z':   // Single Character Introducer
            break;
            case '[':   // Control Sequence Introducer (CSI)
                Data->ProcessingCSI=true;
            break;
            case '\\':  // String Terminator
            case ']':   // Operating System Command
            case '^':   // Privacy Message
            case '_':   // Application Program Command
            break;
            default:
            break;
        }
    }
    else
    {
        // https://en.wikipedia.org/wiki/C0_and_C1_control_codes
        CodeStr=NULL;
        switch(RawByte)
        {
            case 0: // NULL
                CodeStr="NUL";
            break;
            case 1: // Start of Heading
                CodeStr="SOH";
            break;
            case 2: // Start of Text
                CodeStr="STX";
            break;
            case 3: // End of Text
                CodeStr="ETX";
            break;
            case 4: // End of Transmissionc
                CodeStr="EOT";
            break;
            case 5: // Enquiry
                CodeStr="ENQ";
            break;
            case 6: // Acknowledge
                CodeStr="ACK";
            break;
            case 7: // Bell
                CodeStr="BEL";
//                MW_SystemBeep();
                *Consumed=true;
            break;
            case 8: // Backspace
                /* Backspace */
//                CodeStr="BS";
                m_DPS->DoBackspace();
                *Consumed=true;
            break;
            case 9: // Character Tabulation, Horizontal Tabulation
                m_DPS->DoTab();
                *Consumed=true;
            break;
            case 10:    // Line Feed
                /* New line */
                m_DPS->DoNewLine();
//                CodeStr="LF";
                *Consumed=true;
            break;
            case 11:    // Line Tabulation, Vertical Tabulation
                CodeStr="VT";
            break;
            case 12:    // Form Feed
                m_DPS->DoClearScreen();
//                CodeStr="FF";
                *Consumed=true;
            break;
            case 13:    // Carriage Return
                m_DPS->DoReturn();
//                CodeStr="CR";
                *Consumed=true;
            break;
            case 14:    // Shift Out
                CodeStr="SO";
            break;
            case 15:    // Shift In
                CodeStr="SI";
            break;
            case 16:    // Data Link Escape
                CodeStr="DLE";
            break;
            case 17:    // Device Control One (XON)
                CodeStr="DC1";
            break;
            case 18:    // Device Control Two
                CodeStr="DC2";
            break;
            case 19:    // Device Control Three (XOFF)
                CodeStr="DC3";
            break;
            case 20:    // Device Control Four
                CodeStr="DC4";
            break;
            case 21:    // Negative Acknowledge
                CodeStr="NAK";
            break;
            case 22:    // Synchronous Idle
                CodeStr="SYN";
            break;
            case 23:    // End of Transmission Block
                CodeStr="ETB";
            break;
            case 24:    // Cancel
                CodeStr="CAN";
            break;
            case 25:    // End of medium
                CodeStr="EM";
            break;
            case 26:    // Substitute
                CodeStr="SUB";
            break;
            case 27:    // Escape
//                CodeStr="ESC";
                Data->CSIArgCount=0;
                Data->CurrentNum=0;
                Data->CSIArg[0]=0;
                Data->ProcessingESC=true;
                *Consumed=true;
            break;
            case 28:    // File Separator
                CodeStr="FS";
            break;
            case 29:    // Group Separator
                CodeStr="GS";
            break;
            case 30:    // Record Separator
                CodeStr="RS";
            break;
            case 31:    // Unit Separator
                CodeStr="US";
            break;
            case 127:   // Delete
                CodeStr="DEL";
            break;
            default:
            break;
        }
        if(CodeStr!=NULL)
        {
            m_DPS->NoteNonPrintable(CodeStr);
            *Consumed=true;
        }
    }
}

void ANSIX364Decoder_ProcessCSI(struct ANSIX364DecoderData *Data,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed)
{
    int32_t CursorX,CursorY;
    int i;
    int i2;
    int32_t NewPos;
    char buff[100];

    switch(RawByte)
    {
        case '@':   // ICH
        case 'A':   // CUU - Cursor Up
            m_DPS->GetCursorXY(&CursorX,&CursorY);
            i=Data->CSIArg[0];
            if(i<1)
                i=1;
            NewPos=CursorY-i;
            if(NewPos<0)
                NewPos=0;
            m_DPS->DoMoveCursor(CursorX,NewPos);
        break;
        case 'B':   // CUD - Cursor Down
            m_DPS->GetCursorXY(&CursorX,&CursorY);
            i=Data->CSIArg[0];
            if(i<1)
                i=1;
            NewPos=CursorY+i;
            m_DPS->DoMoveCursor(CursorX,NewPos);
        break;
        case 'C':   // CUF - Cursor Forward
            m_DPS->GetCursorXY(&CursorX,&CursorY);
            i=Data->CSIArg[0];
            if(i<1)
                i=1;
            NewPos=CursorX+i;
            m_DPS->DoMoveCursor(NewPos,CursorY);
        break;
        case 'D':   // CUB - Cursor Back
            m_DPS->GetCursorXY(&CursorX,&CursorY);
            i=Data->CSIArg[0];
            if(i<1)
                i=1;
            NewPos=CursorX-i;
            m_DPS->DoMoveCursor(NewPos,CursorY);
        break;
        case 'E':   // CNL - Cursor Next Line
        case 'F':   // CPL - Cursor Previous Line
        case 'G':   // CHA - Cursor Horizontal Absolute
        break;
        case 'H':   // CUP - Cursor Position
            i=1;
            i2=1;
            if(Data->CSIArgCount>=1)
                i=Data->CSIArg[0];
            if(Data->CSIArgCount>=2)
                i2=Data->CSIArg[1];

            if(i<1)
                i=1;
            if(i2<1)
                i2=1;

            m_DPS->DoMoveCursor(i2-1,i-1);
        break;
        case 'I':   // CHT
        break;
        case 'J':   // ED - Erase in Display
            i=Data->CSIArg[0];
            switch(i)
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
                        m_DPS->DoClearArea(CursorX,CursorY,-1,-1);
                    }
                break;
                case 1: // From the beginning of the display through the cursor
                    m_DPS->GetCursorXY(&CursorX,&CursorY);
                    m_DPS->DoClearArea(0,0,CursorX,CursorY);
                break;
                case 2: // The complete display
                    m_DPS->DoClearScreen();
//                    m_DPS->DoMoveCursor(0,0);
                break;
                case 3: // clear entire screen and delete all lines saved in the scrollback buffer (from xterm)
                break;
                default:
                break;
            }
        break;
        case 'K':   // EL - Erase in Line
            m_DPS->GetCursorXY(&CursorX,&CursorY);

            switch(Data->CSIArg[0])
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
        case 'L':   // IL
        case 'M':   // DL
        case 'N':   // EF
        case 'O':   // EA
        case 'P':   // DCH
        case 'Q':   // SSE
        case 'R':   // CPR -- Active Position Report (reply to cursor pos)
        case 'S':   // SU - Scroll Up
        case 'T':   // SD - Scroll Down
        case 'U':   // NP
        case 'V':   // PP
        case 'W':   // CTC
        case 'X':   // ECH
        case 'Y':   // CVT
        case 'Z':   // CBT
        case '[':   // SRS
        case '\\':  // PTX
        case ']':   // SDS
        case '^':   // SIMD
        case '`':   // HPA
        case 'a':   // HPR
        case 'b':   // REP
        case 'c':   // DA
        case 'd':   // VPA
        case 'e':   // VPR
        case 'f':   // HVP - Horizontal Vertical Position
        case 'g':   // TBC
        case 'h':   // SM
        case 'i':   // MC - MEDIA COPY
        case 'j':   // HPB
        case 'k':   // VPB
        case 'l':   // RM
        break;
        case 'm':   // SGR - Select Graphic Rendition
            ANSIX364Decoder_HandleSGR(Data);
            *Consumed=true;
        break;
        case 'n':   // DSR - Device Status Report
            i=Data->CSIArg[0];
            switch(i)
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
        case 'o':   // DAQ
        case 'p':   // Private Use
        case 'q':   // Private Use
        case 'r':   // Private Use
        break;
        case 's':   // SCP - Save Cursor Position
            m_DPS->GetCursorXY(&Data->SavedCursorX,
                    &Data->SavedCursorY);
        break;
        case 't':   // Private Use
        break;
        case 'u':   // RCP - Restore Cursor Position
            m_DPS->DoMoveCursor(Data->SavedCursorX,
                    Data->SavedCursorY);
        break;
        case 'v':   // Private Use
        case 'w':   // Private Use
        case 'x':   // Private Use
        case 'y':   // Private Use
        case 'z':   // Private Use
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
    uint16_t Attribs;
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
                if(Data->DoingReverseVideo)
                    ULineColor=FGColor;
                else
                    ULineColor=BGColor;
            break;
            case 5: // slowly blinking (less then 150 per minute)
            break;
            case 6: // rapidly blinking (150 per minute or more)
            break;
            case 7: // negative image
                if(!Data->DoingReverseVideo)
                {
                    Data->DoingReverseVideo=true;
                    TmpCol=FGColor;
                    FGColor=BGColor;
                    BGColor=TmpCol;
                    ULineColor=FGColor;
                }
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
                if(Data->DoingReverseVideo)
                    ULineColor=FGColor;
                else
                    ULineColor=BGColor;
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
                if(Data->DoingReverseVideo)
                {
                    Data->DoingReverseVideo=false;
                    TmpCol=FGColor;
                    FGColor=BGColor;
                    BGColor=TmpCol;
                    ULineColor=FGColor;
                }
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
                if(Data->DoingReverseVideo)
                    BGColor=TmpCol;
                else
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
                if(Data->DoingReverseVideo)
                    FGColor=TmpCol;
                else
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
                if(Data->DoingReverseVideo)
                    ULineColor=FGColor;
                else
                    ULineColor=BGColor;
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
    Data->DoingReverseVideo=false;
    Data->DoingDim=false;
    Data->DoingBright=false;
}

