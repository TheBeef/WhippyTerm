/*******************************************************************************
 * FILENAME: BasicCtrlCharsDecoder.cpp
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
#include "BasicCtrlCharsDecoder.h"
#include "PluginSDK/Plugin.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*** DEFINES                  ***/
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      BasicCtrlCharsDecoder // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x01000000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
const struct DataProcessorInfo *BasicCtrlCharsDecoder_GetProcessorInfo(unsigned int *SizeOfInfo);
void BasicCtrlCharsDecoder_ProcessByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed);
PG_BOOL BasicCtrlCharsDecoder_ProcessKeyPress(t_DataProcessorHandleType *DataHandle,
        const uint8_t *KeyChar,int KeyCharLen,e_UIKeys ExtendedKey,
        uint8_t Mod);

/*** VARIABLE DEFINITIONS     ***/
struct DataProcessorAPI m_BasicCtrlCharsDecoderCBs=
{
    NULL,
    NULL,
    BasicCtrlCharsDecoder_GetProcessorInfo,
    BasicCtrlCharsDecoder_ProcessKeyPress,
    BasicCtrlCharsDecoder_ProcessByte,
    NULL, // ProcessIncomingBinaryByte
    /* V2 */
    NULL,       // ProcessOutGoingData
    /* V3 */
    NULL,       // AllocSettingsWidgets
    NULL,       // FreeSettingsWidgets
    NULL,       // SetSettingsFromWidgets
    NULL,       // ApplySettings
};

struct DataProcessorInfo m_BasicCtrlCharsDecoder_Info=
{
    "Basic Control Characters",
    "Basic ASCII control characters like new line, backspace, and cartridge return",
    "This processes the following bytes:\n"
        " * Line feed (0x0A)\n"
        " * Cartridge return (0x0D)\n"
        " * Tab (0x09)\n"
        " * Back space (0x08)\n"
        " * Form Feed (0x0C)\n",
    e_DataProcessorType_Text,
    e_TextDataProcessorClass_TermEmulation,
    e_BinaryDataProcessorModeMAX,
};

static const struct PI_UIAPI *m_BasicCtrlChars_UIAPI;
static const struct PI_SystemAPI *m_BasicCtrlChars_SysAPI;
static const struct DPS_API *m_DPS;

/*******************************************************************************
 * NAME:
 *    BasicCtrlCharsDecoder_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int BasicCtrlCharsDecoder_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
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

        m_BasicCtrlChars_SysAPI=SysAPI;
        m_DPS=SysAPI->GetAPI_DataProcessors();
        m_BasicCtrlChars_UIAPI=m_DPS->GetAPI_UI();

        /* If we are have the correct experimental API */
        if(m_BasicCtrlChars_SysAPI->GetExperimentalID()>0 &&
                m_BasicCtrlChars_SysAPI->GetExperimentalID()<1)
        {
            return 0xFFFFFFFF;
        }

        m_DPS->RegisterDataProcessor("BasicCtrlCharsDecoder",
                &m_BasicCtrlCharsDecoderCBs,sizeof(m_BasicCtrlCharsDecoderCBs));

        return 0;
    }
}

/*******************************************************************************
 * NAME:
 *    BasicCtrlCharsDecoder_GetProcessorInfo
 *
 * SYNOPSIS:
 *    const struct DataProcessorInfo *BasicCtrlCharsDecoder_GetProcessorInfo(
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
const struct DataProcessorInfo *BasicCtrlCharsDecoder_GetProcessorInfo(
        unsigned int *SizeOfInfo)
{
    *SizeOfInfo=sizeof(struct DataProcessorInfo);
    return &m_BasicCtrlCharsDecoder_Info;
}

/*******************************************************************************
 *  NAME:
 *    BasicCtrlCharsDecoder_ProcessByte
 *
 *  SYNOPSIS:
 *    void BasicCtrlCharsDecoder_ProcessByte(
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
void BasicCtrlCharsDecoder_ProcessByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed)
{
    const char *CodeStr;

    if(RawByte=='\n')
    {
        /* New line */
        m_DPS->DoNewLine();
        *Consumed=true;
    }
    else if(RawByte=='\r')
    {
        /* Carriage return */
        m_DPS->DoReturn();
        *Consumed=true;
    }
    else if(RawByte=='\t')
    {
        /* Tab */
        m_DPS->DoTab();
        *Consumed=true;
    }
    else if(RawByte=='\b')
    {
        /* Backspace */
        m_DPS->DoBackspace();
        *Consumed=true;
    }
    else if(RawByte=='\f')
    {
        m_DPS->DoClearScreen();
        *Consumed=true;
    }
    if(RawByte<32)
    {
        /* DEBUG PAUL: Expand this */
        switch(RawByte)
        {
            case 0:
                CodeStr="NUL";
            break;
            case 1:
                CodeStr="SOH";
            break;
            case 2:
                CodeStr="STX";
            break;
            case 3:
                CodeStr="ETX";
            break;
            case 4:
                CodeStr="EOT";
            break;
            case 5:
                CodeStr="ENQ";
            break;
            case 6:
                CodeStr="ACK";
            break;
            case 7:
                CodeStr="BEL";
            break;
//            case 8:
//                CodeStr="BS";
//            break;
//                case 9:
//                    CodeStr="HT";
//                break;
//            case 10:
//                CodeStr="LF";
//            break;
            case 11:
                CodeStr="VT";
            break;
//            case 12:
//                CodeStr="FF";
//            break;
//            case 13:
//                CodeStr="CR";
//            break;
            case 14:
                CodeStr="SO";
            break;
            case 15:
                CodeStr="SI";
            break;
            case 16:
                CodeStr="DLE";
            break;
            case 17:
                CodeStr="DC1";
            break;
            case 18:
                CodeStr="DC2";
            break;
            case 19:
                CodeStr="DC3";
            break;
            case 20:
                CodeStr="DC4";
            break;
            case 21:
                CodeStr="NAK";
            break;
            case 22:
                CodeStr="SYN";
            break;
            case 23:
                CodeStr="ETB";
            break;
            case 24:
                CodeStr="CAN";
            break;
            case 25:
                CodeStr="EM";
            break;
            case 26:
                CodeStr="SUB";
            break;
            case 27:
                CodeStr="ESC";
            break;
            case 28:
                CodeStr="FS";
            break;
            case 29:
                CodeStr="GS";
            break;
            case 30:
                CodeStr="RS";
            break;
            case 31:
                CodeStr="US";
            break;
            case 127:
                CodeStr="DEL";
            break;
            default:
                CodeStr=NULL;
            break;
        }
        if(CodeStr!=NULL)
        {
            m_DPS->NoteNonPrintable(CodeStr);
            *Consumed=true;
        }
    }
}

/*******************************************************************************
 * NAME:
 *   BasicCtrlCharsDecoder_ProcessKeyPress
 *
 * SYNOPSIS:
 *   PG_BOOL BasicCtrlCharsDecoder_ProcessKeyPress(
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
PG_BOOL BasicCtrlCharsDecoder_ProcessKeyPress(
        t_DataProcessorHandleType *DataHandle,const uint8_t *KeyChar,
        int KeyCharLen,e_UIKeys ExtendedKey,uint8_t Mod)
{
    uint8_t c; // The char we are going to send

    c=*KeyChar;

    if(Mod&KEYMOD_CONTROL)
    {
        if(c>='A' && c<='Z')
        {
            c-='@';
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
                c=27;
            break;
            case e_UIKeys_Tab:
                c=9;
            break;
            case e_UIKeys_Backspace:
                m_DPS->SendBackspace();
            break;
            case e_UIKeys_Return:
            case e_UIKeys_Enter:
                m_DPS->SendEnter();
            break;
            case e_UIKeys_Delete:
                c=127;
            break;
            case e_UIKeys_Up:
            case e_UIKeys_Down:
            case e_UIKeys_Left:
            case e_UIKeys_Right:
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

    m_DPS->WriteData(&c,1);

    return true;
}
