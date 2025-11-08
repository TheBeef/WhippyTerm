/*******************************************************************************
 * FILENAME: BasicHex.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 17 May 2024 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (17 May 2024)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "BasicHex.h"
#include "PluginSDK/Plugin.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*** DEFINES                  ***/
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      BASIC_HEX // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x01000000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct BasicHexDecoderData
{
    int junk;
};

/*** FUNCTION PROTOTYPES      ***/
t_DataProcessorHandleType *BasicHexDecoder_AllocateData(void);
void BasicHexDecoder_FreeData(t_DataProcessorHandleType *DataHandle);
const struct DataProcessorInfo *BasicHexDecoder_GetProcessorInfo(unsigned int *SizeOfInfo);
void BasicHexDecoder_ProcessIncomingBinaryByte(t_DataProcessorHandleType *DataHandle,const uint8_t Byte);
void BasicHexDecoder_HandleSGR(struct BasicHexDecoderData *Data);
void BasicHexDecoder_ProcessCSI(struct BasicHexDecoderData *Data,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed);
PG_BOOL BasicHexDecoder_ProcessKeyPress(t_DataProcessorHandleType *DataHandle,
            const uint8_t *KeyChar,int KeyCharLen,e_UIKeys ExtendedKey,
            uint8_t Mod);

/*** VARIABLE DEFINITIONS     ***/
static const struct PI_UIAPI *m_UIAPI;
static const struct PI_SystemAPI *m_System;
static const struct DPS_API *m_DPS;

struct DataProcessorAPI m_BasicHexDecoderAPI=
{
    BasicHexDecoder_AllocateData,
    BasicHexDecoder_FreeData,
    BasicHexDecoder_GetProcessorInfo,
    BasicHexDecoder_ProcessKeyPress,
    NULL,   // ProcessIncomingByte,
    BasicHexDecoder_ProcessIncomingBinaryByte,
    /* V2 */
    NULL,       // ProcessOutGoingData
    /* V3 */
    NULL,       // AllocSettingsWidgets
    NULL,       // FreeSettingsWidgets
    NULL,       // SetSettingsFromWidgets
    NULL,       // ApplySettings
};

struct DataProcessorInfo m_BasicHexDecoder_Info=
{
    "Hexdump",
    "Hexdump of binary data",
    "Prints out hex values of incoming data",
    e_DataProcessorType_Binary,
    .BinClass=e_BinaryDataProcessorClass_Decoder,
    e_BinaryDataProcessorMode_Hex,
};

/*******************************************************************************
 * NAME:
 *    BASIC_HEX_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int BASIC_HEX_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
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

        m_DPS->RegisterDataProcessor("BasicHexDecoder",&m_BasicHexDecoderAPI,
                sizeof(m_BasicHexDecoderAPI));

        return 0;
    }
}

/*******************************************************************************
 * NAME:
 *    BasicHexDecoder_AllocateData
 *
 * SYNOPSIS:
 *    t_DataProcessorHandleType *BasicHexDecoder_AllocateData(void);
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
t_DataProcessorHandleType *BasicHexDecoder_AllocateData(void)
{
    struct BasicHexDecoderData *Data;
    Data=(struct BasicHexDecoderData *)malloc(sizeof(struct BasicHexDecoderData));
    if(Data==NULL)
        return NULL;

    return (t_DataProcessorHandleType *)Data;
}

/*******************************************************************************
 *  NAME:
 *    BasicHexDecoder_FreeData
 *
 *  SYNOPSIS:
 *    void BasicHexDecoder_FreeData(t_DataProcessorHandleType *DataHandle);
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
void BasicHexDecoder_FreeData(t_DataProcessorHandleType *DataHandle)
{
    struct BasicHexDecoderData *Data=(struct BasicHexDecoderData *)DataHandle;

    free(Data);
}

/*******************************************************************************
 * NAME:
 *    BasicHexDecoder_GetProcessorInfo
 *
 * SYNOPSIS:
 *    const struct DataProcessorInfo *BasicHexDecoder_GetProcessorInfo(
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
const struct DataProcessorInfo *BasicHexDecoder_GetProcessorInfo(
        unsigned int *SizeOfInfo)
{
    *SizeOfInfo=sizeof(struct DataProcessorInfo);
    return &m_BasicHexDecoder_Info;
}

/*******************************************************************************
 * NAME:
 *   BasicHexDecoder_ProcessKeyPress
 *
 * SYNOPSIS:
 *   PG_BOOL BasicHexDecoder_ProcessKeyPress(
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
PG_BOOL BasicHexDecoder_ProcessKeyPress(t_DataProcessorHandleType *DataHandle,
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

    m_DPS->WriteData(SendStr,SendLen);

    return true;
}

void BasicHexDecoder_ProcessIncomingByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed)
{
}

void BasicHexDecoder_ProcessIncomingBinaryByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t Byte)
{
    char buff[100];

    sprintf(buff,"%02X ",Byte);

//    if(Byte=='A')
//        m_DPS->SetFGColor(0xFF0000);
//    else if(Byte=='1')
//        m_DPS->SetFGColor(0x00FF00);
//    else if(Byte=='.')
//        m_DPS->SetFGColor(0x0000FF);
//    else
//        m_DPS->SetFGColor(0xFFFFFF);

    m_DPS->BinaryAddText(buff);
    m_DPS->BinaryAddHex(Byte);
}
