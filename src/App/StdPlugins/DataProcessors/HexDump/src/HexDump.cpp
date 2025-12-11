/*******************************************************************************
 * FILENAME: HexDump.cpp
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
 *    Paul Hutchinson (17 May 2024)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "HexDump.h"
#include "PluginSDK/Plugin.h"
#include "ColorStream.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string>
extern "C"
{
 #include "BPDS.h"
}

using namespace std;

/*** DEFINES                  ***/
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      HexDumpDecoder // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x02010000  //` DEBUG PAUL: TBD 2.2 likely

#define NUMBER_OF_SETS                          5
#define MAX_NUMBER_OF_FIELDS                    20

/*** MACROS                   ***/
/* All this to make REGISTER_PLUGIN_FUNCTION_PRIV_NAME a string */
#define QUOTE(name)     #name
#define STR(macro)      QUOTE(macro)
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME_STR STR(REGISTER_PLUGIN_FUNCTION_PRIV_NAME)

/*** TYPE DEFINITIONS         ***/
struct HexDumpDecoderData
{
    struct ColorStreamData *CSD[NUMBER_OF_SETS];

    string BPDSDefStr[NUMBER_OF_SETS];
    struct StyleData Styles[NUMBER_OF_SETS][MAX_NUMBER_OF_FIELDS];
};

struct HexDumpDecoder_BPDSDefCallbackInfo
{
    struct HexDumpDecoder_SettingsWidgets *WData;
    unsigned int Set;
};

struct HexDumpDecoderSet
{
    /* Tab Handles */
    t_WidgetSysHandle *SetTabs;

    /* BPDS Tab */
    struct PI_GroupBox *BPDSGroupBox;
    struct PI_TextInput *BPDSStr;
    struct PI_Checkbox *BigEndian;

    /* Set tab */
    struct PI_StylePick *FieldStyle[MAX_NUMBER_OF_FIELDS];
    struct PI_ButtonInput *ResetButton;

    struct HexDumpDecoder_BPDSDefCallbackInfo BPDSDefCallbackInfo;
};

struct HexDumpDecoder_SettingsWidgets
{
//    t_WidgetSysHandle *OptionsTabHandle;
    t_WidgetSysHandle *BPDSTab;

    /* BPDS Sets */
    struct HexDumpDecoderSet Sets[NUMBER_OF_SETS];
    struct PI_GroupBox *ErrorsGroupBox;
    struct PI_TextBox *ErrorsTextBox;

    struct PI_WebLink *HelpLink;
};

/*** FUNCTION PROTOTYPES      ***/
t_DataProcessorHandleType *HexDumpDecoder_AllocateData(void);
void HexDumpDecoder_FreeData(t_DataProcessorHandleType *DataHandle);
const struct DataProcessorInfo *HexDumpDecoder_GetProcessorInfo(unsigned int *SizeOfInfo);
void HexDumpDecoder_ProcessIncomingBinaryByte(t_DataProcessorHandleType *DataHandle,const uint8_t Byte);
void HexDumpDecoder_HandleSGR(struct HexDumpDecoderData *Data);
void HexDumpDecoder_ProcessCSI(struct HexDumpDecoderData *Data,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed);
PG_BOOL HexDumpDecoder_ProcessKeyPress(t_DataProcessorHandleType *DataHandle,
            const uint8_t *KeyChar,int KeyCharLen,e_UIKeys ExtendedKey,
            uint8_t Mod);
static t_DataProSettingsWidgetsType *HexDumpDecoder_AllocSettingsWidgets(t_WidgetSysHandle *WidgetHandle,t_PIKVList *Settings);
static void HexDumpDecoder_FreeSettingsWidgets(t_DataProSettingsWidgetsType *PrivData);
static void HexDumpDecoder_SetSettingsFromWidgets(t_DataProSettingsWidgetsType *PrivData,t_PIKVList *Settings);
static void HexDumpDecoder_ApplySettings(t_DataProcessorHandleType *DataHandle,t_PIKVList *Settings);
static void BPDSDefStrEdit_EventCB(const struct PICBEvent *Event,void *UserData);
static void HexDumpDecoder_AppendBPDSError(struct HexDumpDecoder_SettingsWidgets *WData,
        string &Msg,struct BPDSDef *BPDSDef,unsigned int SetNumber);
static void HexDumpDecoder_CheckAllBPDSDefsForErrors(struct HexDumpDecoder_SettingsWidgets *WData);
static void HexDumpDecoder_RethinkFieldLabels(struct HexDumpDecoder_SettingsWidgets *WData,unsigned int Set);
static void HexDumpDecoder_Reset2DefaultBttnCB(const struct PIButtonEvent *Event,void *UserData);

/*** VARIABLE DEFINITIONS     ***/
static const struct PI_UIAPI *m_UIAPI;
static const struct PI_SystemAPI *m_System;
const struct DPS_API *g_DPS;

struct DataProcessorAPI m_HexDumpDecoderAPI=
{
    HexDumpDecoder_AllocateData,
    HexDumpDecoder_FreeData,
    HexDumpDecoder_GetProcessorInfo,
    HexDumpDecoder_ProcessKeyPress,
    NULL,   // ProcessIncomingByte,
    HexDumpDecoder_ProcessIncomingBinaryByte,
    /* V2 */
    NULL,       // ProcessOutGoingData
    /* V3 */
    HexDumpDecoder_AllocSettingsWidgets,
    HexDumpDecoder_FreeSettingsWidgets,
    HexDumpDecoder_SetSettingsFromWidgets,
    HexDumpDecoder_ApplySettings,
};

struct DataProcessorInfo m_HexDumpDecoder_Info=
{
    "Hexdump",
    "Hexdump of binary data",
    "Prints out hex values of incoming data",
    e_DataProcessorType_Binary,
    .BinClass=e_BinaryDataProcessorClass_Decoder,
    e_BinaryDataProcessorMode_Hex,
};

static struct StyleData m_DefaultStyleData[MAX_NUMBER_OF_FIELDS]=
{
    {0xFF0000,-1U     ,0,0,0},   // 1
    {0x00FF00,-1U     ,0,0,0},   // 2
    {0x0000FF,-1U     ,0,0,0},   // 3
    {0xFF00FF,-1U     ,0,0,0},   // 4
    {0xFFFF00,-1U     ,0,0,0},   // 5
    {0x000000,0xFF0000,0,0,0},   // 6
    {0x000000,0x00FF00,0,0,0},   // 7
    {0x000000,0x0000FF,0,0,0},   // 8
    {0x000000,0xFF00FF,0,0,0},   // 9
    {0x000000,0xFFFF00,0,0,0},   // 10
    {0x000000,0xFFFFFF,0,0,0},   // 11
    {0xFF0000,0xFFFFFF,0,0,0},   // 12
    {0x00FF00,0xFFFFFF,0,0,0},   // 13
    {0x0000FF,0xFFFFFF,0,0,0},   // 14
    {0xFF00FF,0xFFFFFF,0,0,0},   // 15
    {0xE5A50A,0xFFFFFF,0,0,0},   // 16
    {0x000000,0x777777,0,0,0},   // 17
    {-1U,-1U          ,TXT_ATTRIB_UNDERLINE,0,0},   // 18
    {-1U,-1U          ,TXT_ATTRIB_BOLD,0,0},        // 19
    {-1U,-1U          ,TXT_ATTRIB_ITALIC,0,0},      // 20
};

/*******************************************************************************
 * NAME:
 *    HexDumpDecoder_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int HexDumpDecoder_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
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
        g_DPS=SysAPI->GetAPI_DataProcessors();
        m_UIAPI=g_DPS->GetAPI_UI();

        /* If we are have the correct experimental API */
        if(m_System->GetExperimentalID()>0 &&
                m_System->GetExperimentalID()<1)
        {
            return 0xFFFFFFFF;
        }

        g_DPS->RegisterDataProcessor("BasicHexDecoder", // We keep the name BasicHexDecoder because the older version used that name and we want any bookmarks to switch to the new one
                &m_HexDumpDecoderAPI,sizeof(m_HexDumpDecoderAPI));

        return 0;
    }
}

/*******************************************************************************
 * NAME:
 *    HexDumpDecoder_AllocateData
 *
 * SYNOPSIS:
 *    t_DataProcessorHandleType *HexDumpDecoder_AllocateData(void);
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
t_DataProcessorHandleType *HexDumpDecoder_AllocateData(void)
{
    struct HexDumpDecoderData *Data;
    unsigned int r;

    Data=NULL;
    try
    {
        Data=new struct HexDumpDecoderData;

        for(r=0;r<NUMBER_OF_SETS;r++)
            Data->CSD[r]=NULL;
    }
    catch(...)
    {
        if(Data!=NULL)
            delete Data;
        Data=NULL;
    }

    return (t_DataProcessorHandleType *)Data;
}

/*******************************************************************************
 *  NAME:
 *    HexDumpDecoder_FreeData
 *
 *  SYNOPSIS:
 *    void HexDumpDecoder_FreeData(t_DataProcessorHandleType *DataHandle);
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
void HexDumpDecoder_FreeData(t_DataProcessorHandleType *DataHandle)
{
    struct HexDumpDecoderData *Data=(struct HexDumpDecoderData *)DataHandle;
    unsigned int r;

    for(r=0;r<NUMBER_OF_SETS;r++)
    {
        if(Data->CSD[r]!=NULL)
            FreeColorStream(Data->CSD[r]);
    }

    delete Data;
}

/*******************************************************************************
 * NAME:
 *    HexDumpDecoder_GetProcessorInfo
 *
 * SYNOPSIS:
 *    const struct DataProcessorInfo *HexDumpDecoder_GetProcessorInfo(
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
const struct DataProcessorInfo *HexDumpDecoder_GetProcessorInfo(
        unsigned int *SizeOfInfo)
{
    *SizeOfInfo=sizeof(struct DataProcessorInfo);
    return &m_HexDumpDecoder_Info;
}

/*******************************************************************************
 * NAME:
 *   HexDumpDecoder_ProcessKeyPress
 *
 * SYNOPSIS:
 *   PG_BOOL HexDumpDecoder_ProcessKeyPress(
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
PG_BOOL HexDumpDecoder_ProcessKeyPress(t_DataProcessorHandleType *DataHandle,
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
                g_DPS->SendBackspace();
            return true;
            case e_UIKeys_Return:
            case e_UIKeys_Enter:
                g_DPS->SendEnter();
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

    g_DPS->WriteData(SendStr,SendLen);

    return true;
}

void HexDumpDecoder_ProcessIncomingByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed)
{
}

void HexDumpDecoder_ProcessIncomingBinaryByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t Byte)
{
    struct HexDumpDecoderData *Data=(struct HexDumpDecoderData *)DataHandle;
    unsigned int r;
    char buff[100];
    bool DidStyling;

    DidStyling=false;
    for(r=0;r<NUMBER_OF_SETS;r++)
    {
        if(Data->CSD[r]!=NULL)
            if(ColorStreamProcessIncomingByte(Data->CSD[r],Byte,DidStyling))
                DidStyling=true;
    }

    sprintf(buff,"%02X ",Byte);
    g_DPS->BinaryAddText(buff);
    g_DPS->BinaryAddHex(Byte);

    for(r=0;r<NUMBER_OF_SETS;r++)
    {
        if(Data->CSD[r]!=NULL)
            ColorStreamProcessIncomingByteFinish(Data->CSD[r],Byte,DidStyling);
    }
}

/*******************************************************************************
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
 ******************************************************************************/
t_DataProSettingsWidgetsType *HexDumpDecoder_AllocSettingsWidgets(t_WidgetSysHandle *WidgetHandle,t_PIKVList *Settings)
{
    struct HexDumpDecoder_SettingsWidgets *WData;
    const char *Str;
    unsigned int r;
    unsigned int rr;
    char buff[100];
    struct StyleData TmpStyleData;

    try
    {
        /* Fix the defaults */
        for(r=0;r<MAX_NUMBER_OF_FIELDS;r++)
        {
            if(m_DefaultStyleData[r].FGColor==-1U)
                m_DefaultStyleData[r].FGColor=g_DPS->GetSysDefaultColor(e_DefaultColors_FG);
            if(m_DefaultStyleData[r].BGColor==-1U)
                m_DefaultStyleData[r].BGColor=g_DPS->GetSysDefaultColor(e_DefaultColors_BG);
            m_DefaultStyleData[r].ULineColor=m_DefaultStyleData[r].FGColor;
        }

        WData=new HexDumpDecoder_SettingsWidgets;

        /* Zero everything */
//        WData->OptionsTabHandle=NULL;
        WData->BPDSTab=NULL;
        for(r=0;r<NUMBER_OF_SETS;r++)
        {
            WData->Sets[r].SetTabs=NULL;
            WData->Sets[r].BPDSGroupBox=NULL;
            WData->Sets[r].BPDSStr=NULL;
            WData->Sets[r].BigEndian=NULL;
            for(rr=0;rr<MAX_NUMBER_OF_FIELDS;rr++)
                WData->Sets[r].FieldStyle[rr]=NULL;
            WData->Sets[r].ResetButton=NULL;
        }
        WData->ErrorsGroupBox=NULL;
        WData->ErrorsTextBox=NULL;
        WData->HelpLink=NULL;

//        /* Options tab */
//        WData->OptionsTabHandle=WidgetHandle;
//        g_DPS->SetCurrentSettingsTabName("Options");

        /* BPDS tab */
//        WData->BPDSTab=g_DPS->AddNewSettingsTab("BPDS");
        WData->BPDSTab=WidgetHandle;
        g_DPS->SetCurrentSettingsTabName("Definitions");

        for(r=0;r<NUMBER_OF_SETS;r++)
        {
            sprintf(buff,"Set %d Styles",r+1);
            WData->Sets[r].SetTabs=g_DPS->AddNewSettingsTab(buff);
            if(WData->Sets[r].SetTabs==NULL)
                throw(0);

            /********************/
            /* Add BPDS widgets */
            /********************/
            sprintf(buff,"Set %d",r+1);
            WData->Sets[r].BPDSGroupBox=m_UIAPI->AddGroupBox(WData->BPDSTab,
                    buff);
            if(WData->Sets[r].BPDSGroupBox==NULL)
                throw(0);

            WData->Sets[r].BPDSDefCallbackInfo.WData=WData;
            WData->Sets[r].BPDSDefCallbackInfo.Set=r;

            WData->Sets[r].BPDSStr=m_UIAPI->AddTextInput(WData->Sets[r].
                    BPDSGroupBox->GroupWidgetHandle,"BPDS Definition",
                    BPDSDefStrEdit_EventCB,
                    (void *)&WData->Sets[r].BPDSDefCallbackInfo);
            if(WData->Sets[r].BPDSStr==NULL)
                throw(0);
            sprintf(buff,"BPDSDef_%d",r);
            Str=m_System->KVGetItem(Settings,buff);
            if(Str==NULL)
                Str="";
            m_UIAPI->SetTextInputText(WData->Sets[r].BPDSGroupBox->
                    GroupWidgetHandle,WData->Sets[r].BPDSStr->Ctrl,Str);

            WData->Sets[r].BigEndian=m_UIAPI->AddCheckbox(WData->Sets[r].BPDSGroupBox->
                    GroupWidgetHandle,"Big Endian",NULL,NULL);
            if(WData->Sets[r].BigEndian==NULL)
                throw(0);
            sprintf(buff,"BigEndian_%d",r);
            Str=m_System->KVGetItem(Settings,buff);
            if(Str==NULL)
                Str="1";
            m_UIAPI->SetCheckboxChecked(WData->Sets[r].BPDSGroupBox->
                    GroupWidgetHandle,WData->Sets[r].BigEndian->Ctrl,atoi(Str));

            for(rr=0;rr<MAX_NUMBER_OF_FIELDS;rr++)
            {
                sprintf(buff,"FieldStyle_%d_%d",r,rr);
                Str=m_System->KVGetItem(Settings,buff);
                if(Str!=NULL)
                {
                    m_UIAPI->Str2StyleHelper(&TmpStyleData,Str);
                }
                else
                {
                    /* Use defaults */
                    TmpStyleData=m_DefaultStyleData[rr];
                }

                sprintf(buff,"Field %d",rr+1);
                WData->Sets[r].FieldStyle[rr]=m_UIAPI->AddStylePick(WData->
                        Sets[r].SetTabs,buff,&TmpStyleData,NULL,NULL);
                if(WData->Sets[r].FieldStyle[rr]==NULL)
                    throw(0);
            }

            WData->Sets[r].ResetButton=m_UIAPI->AddButtonInput(WData->
                        Sets[r].SetTabs,"Reset to defaults",
                        HexDumpDecoder_Reset2DefaultBttnCB,
                        (void *)&WData->Sets[r].BPDSDefCallbackInfo);
            if(WData->Sets[r].ResetButton==NULL)
                throw(0);

            HexDumpDecoder_RethinkFieldLabels(WData,r);
        }

        /* Errors */
        WData->ErrorsGroupBox=m_UIAPI->AddGroupBox(WData->BPDSTab,"Errors");
        if(WData->ErrorsGroupBox==NULL)
            throw(0);

        WData->ErrorsTextBox=m_UIAPI->AddTextBox(WData->
                ErrorsGroupBox->GroupWidgetHandle,"","");
        if(WData->ErrorsTextBox==NULL)
            throw(0);

        m_UIAPI->ChangeTextBoxProp(WData->ErrorsGroupBox->GroupWidgetHandle,
                WData->ErrorsTextBox->Ctrl,e_TextBoxProp_FontMode,true,NULL);

        WData->HelpLink=m_UIAPI->AddWebLink(WData->BPDSTab,"","Click here for information on the Binary Protocol Documentation Standard","https://whippyterm.com/BinaryProtocolDocumentationStandard");
        if(WData->HelpLink==NULL)
            throw(0);

        HexDumpDecoder_CheckAllBPDSDefsForErrors(WData);
    }
    catch(...)
    {
        if(WData!=NULL)
        {
            HexDumpDecoder_FreeSettingsWidgets(
                    (t_DataProSettingsWidgetsType *)WData);
        }
        return NULL;
    }

    return (t_DataProSettingsWidgetsType *)WData;
}

/*******************************************************************************
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
 ******************************************************************************/
void HexDumpDecoder_FreeSettingsWidgets(t_DataProSettingsWidgetsType *PrivData)
{
    struct HexDumpDecoder_SettingsWidgets *WData=(struct HexDumpDecoder_SettingsWidgets *)PrivData;
    unsigned int r;
    unsigned int rr;

    /* Free everything in reverse order */
    if(WData->HelpLink!=NULL)
        m_UIAPI->FreeWebLink(WData->BPDSTab,WData->HelpLink);

    if(WData->ErrorsTextBox!=NULL)
    {
        m_UIAPI->FreeTextBox(WData->ErrorsGroupBox->GroupWidgetHandle,
                WData->ErrorsTextBox);
    }

    if(WData->ErrorsGroupBox!=NULL)
        m_UIAPI->FreeGroupBox(WData->BPDSTab,WData->ErrorsGroupBox);

    for(r=0;r<NUMBER_OF_SETS;r++)
    {
        if(WData->Sets[r].ResetButton!=NULL)
        {
            m_UIAPI->FreeButtonInput(WData->Sets[r].SetTabs,
                    WData->Sets[r].ResetButton);
        }

        for(rr=0;rr<MAX_NUMBER_OF_FIELDS;rr++)
        {
            if(WData->Sets[r].FieldStyle[rr]!=NULL)
            {
                m_UIAPI->FreeStylePick(WData->Sets[r].SetTabs,
                        WData->Sets[r].FieldStyle[rr]);
            }
        }

        if(WData->Sets[r].BigEndian!=NULL)
        {
            m_UIAPI->FreeCheckbox(WData->Sets[r].BPDSGroupBox->
                    GroupWidgetHandle,WData->Sets[r].BigEndian);
        }

        if(WData->Sets[r].BPDSStr!=NULL)
        {
            m_UIAPI->FreeTextInput(WData->Sets[r].BPDSGroupBox->
                    GroupWidgetHandle,WData->Sets[r].BPDSStr);
        }

        m_UIAPI->FreeGroupBox(WData->BPDSTab,WData->Sets[r].BPDSGroupBox);
    }

    delete WData;
}

/*******************************************************************************
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
 ******************************************************************************/
void HexDumpDecoder_SetSettingsFromWidgets(t_DataProSettingsWidgetsType *PrivData,t_PIKVList *Settings)
{
    struct HexDumpDecoder_SettingsWidgets *WData=(struct HexDumpDecoder_SettingsWidgets *)PrivData;
    string Str;
    unsigned int r;
    unsigned int rr;
    char buff[100];
    struct StyleData SD;
    char SDStr[SUGGESTED_STYLE_DATA_STR_BUFFER_LEN];

    for(r=0;r<NUMBER_OF_SETS;r++)
    {
        Str=m_UIAPI->GetTextInputText(WData->Sets[r].BPDSGroupBox->
                GroupWidgetHandle,WData->Sets[r].BPDSStr->Ctrl);
        sprintf(buff,"BPDSDef_%d",r);
        m_System->KVAddItem(Settings,buff,Str.c_str());

        Str=m_UIAPI->IsCheckboxChecked(WData->Sets[r].BPDSGroupBox->
                GroupWidgetHandle,WData->Sets[r].BigEndian->Ctrl)?"1":"0";
        sprintf(buff,"BigEndian_%d",r);
        m_System->KVAddItem(Settings,buff,Str.c_str());

        for(rr=0;rr<MAX_NUMBER_OF_FIELDS;rr++)
        {
            sprintf(buff,"Field %d",rr+1);
            m_UIAPI->GetStylePickValue(WData->Sets[r].SetTabs,
                    WData->Sets[r].FieldStyle[rr]->Ctrl,&SD);

            if(m_UIAPI->Style2StrHelper(&SD,SDStr,sizeof(SDStr)))
            {
                sprintf(buff,"FieldStyle_%d_%d",r,rr);
                Str=m_System->KVAddItem(Settings,buff,SDStr);
            }
        }
    }
}

/*******************************************************************************
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
 ******************************************************************************/
static void HexDumpDecoder_ApplySettings(t_DataProcessorHandleType *DataHandle,
        t_PIKVList *Settings)
{
    struct HexDumpDecoderData *Data=(struct HexDumpDecoderData *)DataHandle;
    const char *Str;
    unsigned int r;
    unsigned int rr;
    char buff[100];
    bool UseBigEndian;

    for(r=0;r<NUMBER_OF_SETS;r++)
    {
        sprintf(buff,"BPDSDef_%d",r);
        Str=m_System->KVGetItem(Settings,buff);
        if(Str==NULL)
            Str="";
        Data->BPDSDefStr[r]=Str;

        if(Data->CSD[r]!=NULL)
        {
            FreeColorStream(Data->CSD[r]);
            Data->CSD[r]=NULL;
        }

        sprintf(buff,"BigEndian_%d",r);
        Str=m_System->KVGetItem(Settings,buff);
        if(Str==NULL)
            Str="1";
        UseBigEndian=atoi(Str);

        if(Data->BPDSDefStr[r]!="")
        {
            Data->CSD[r]=AllocColorStream(Data->BPDSDefStr[r].c_str());
            if(Data->CSD[r]!=NULL)
            {
                SetColorStreamEndian(Data->CSD[r],UseBigEndian);

                /* Style */
                for(rr=0;rr<MAX_NUMBER_OF_FIELDS;rr++)
                {
                    sprintf(buff,"FieldStyle_%d_%d",r,rr);
                    Str=m_System->KVGetItem(Settings,buff);
                    if(Str!=NULL)
                        m_UIAPI->Str2StyleHelper(&Data->Styles[r][rr],Str);
                    else
                        Data->Styles[r][rr]=m_DefaultStyleData[rr];

                    /* Apply the styling */
                    SetColorStreamFieldStyling(Data->CSD[r],rr,&Data->Styles[r][rr]);
                }
            }
        }
    }
}

void BPDSDefStrEdit_EventCB(const struct PICBEvent *Event,void *UserData)
{
    struct HexDumpDecoder_BPDSDefCallbackInfo *Info=(struct HexDumpDecoder_BPDSDefCallbackInfo *)UserData;
    struct HexDumpDecoder_SettingsWidgets *WData=Info->WData;

    if(Event->EventType==e_PIECB_TextInputEditFinished)
    {
        HexDumpDecoder_RethinkFieldLabels(WData,Info->Set);

        HexDumpDecoder_CheckAllBPDSDefsForErrors(WData);
    }
}

static void HexDumpDecoder_CheckAllBPDSDefsForErrors(struct HexDumpDecoder_SettingsWidgets *WData)
{
    struct HexDumpDecoderSet *UseSet;
    const char *Str;
    struct BPDSDef *TmpBPDSDef;
    string ErrorMsgs;
    unsigned int r;
    char buff[100];

    ErrorMsgs="";
    for(r=0;r<NUMBER_OF_SETS;r++)
    {
        UseSet=&WData->Sets[r];

        Str=m_UIAPI->GetTextInputText(UseSet->BPDSGroupBox->GroupWidgetHandle,
                UseSet->BPDSStr->Ctrl);

        TmpBPDSDef=BPDS_Parse(Str);
        if(TmpBPDSDef==NULL)
        {
            if(ErrorMsgs!="")
                ErrorMsgs+="-------------\n";
            sprintf(buff,"Set %d has an error:\n",r+1);
            ErrorMsgs+=buff;
            ErrorMsgs+="Failed to allocate the BPDS parser\n";
        }
        else
        {
            if(TmpBPDSDef->HadError)
            {
                HexDumpDecoder_AppendBPDSError(WData,ErrorMsgs,TmpBPDSDef,r);
            }
            BPDS_FreeBPDSDef(TmpBPDSDef);
        }
    }
    m_UIAPI->SetTextBox(WData->ErrorsGroupBox->GroupWidgetHandle,
            WData->ErrorsTextBox->Ctrl,ErrorMsgs.c_str());
}

static void HexDumpDecoder_AppendBPDSError(struct HexDumpDecoder_SettingsWidgets *WData,
        string &Msg,struct BPDSDef *BPDSDef,unsigned int SetNumber)
{
    char buff[100];

    if(Msg!="")
        Msg+="-------------\n";
    sprintf(buff,"Set %d has an error:\n",SetNumber+1);
    Msg+=buff;

    Msg+="   ";
    Msg+=BPDSDef->ErrorStr;
    sprintf(buff," at offset %u.\n",BPDSDef->ErrorOffset);
    Msg+=buff;

    Msg+="   ";
    Msg+=BPDSDef->DPDSDefStr;
    Msg+="\n";

    Msg+="   ";
    Msg.append(BPDSDef->ErrorOffset,' ');
    Msg+="^";
    Msg+="\n";
}

static void HexDumpDecoder_RethinkFieldLabels(struct HexDumpDecoder_SettingsWidgets *WData,unsigned int Set)
{
    char buff[100];
    struct BPDSDef *TmpBPDSDef;
    struct HexDumpDecoderSet *UseSet;
    const char *Str;
    string LabelStr;
    struct BPDSField *Field;
    unsigned int rr;
    unsigned int i;

    UseSet=&WData->Sets[Set];

    for(rr=0;rr<MAX_NUMBER_OF_FIELDS;rr++)
    {
        sprintf(buff,"Field %d",rr+1);
        LabelStr=buff;

        Str=m_UIAPI->GetTextInputText(UseSet->BPDSGroupBox->GroupWidgetHandle,
                UseSet->BPDSStr->Ctrl);
        TmpBPDSDef=BPDS_Parse(Str);
        if(TmpBPDSDef!=NULL)
        {
            if(!TmpBPDSDef->HadError)
            {
                for(i=0,Field=TmpBPDSDef->FieldList;Field!=NULL && i<rr;
                        Field=Field->Next,i++)
                {
                }
                if(Field!=NULL)
                {
                    if(*Field->Name!=0)
                    {
                        LabelStr+=" (";
                        LabelStr+=Field->Name;
                        LabelStr+=")";
                    }
                }
            }
            BPDS_FreeBPDSDef(TmpBPDSDef);
        }

        m_UIAPI->SetLabelText(WData->Sets[Set].SetTabs,
                WData->Sets[Set].FieldStyle[rr]->Label,LabelStr.c_str());
    }
}

static void HexDumpDecoder_Reset2DefaultBttnCB(const struct PIButtonEvent *Event,
        void *UserData)
{
    struct HexDumpDecoder_BPDSDefCallbackInfo *Info=(struct HexDumpDecoder_BPDSDefCallbackInfo *)UserData;
    struct HexDumpDecoder_SettingsWidgets *WData=Info->WData;
    struct HexDumpDecoderSet *UseSet=&WData->Sets[Info->Set];
    unsigned int rr;

    if(m_UIAPI->Ask("Are you sure you want to reset this set styles to the defaults?",
            PIUI_ASK_YESNO)==PIUI_ASK_YES_BTTN)
    {
        for(rr=0;rr<MAX_NUMBER_OF_FIELDS;rr++)
        {
            m_UIAPI->SetStylePickValue(UseSet->SetTabs,
                    UseSet->FieldStyle[rr]->Ctrl,&m_DefaultStyleData[rr]);
        }
    }
}
