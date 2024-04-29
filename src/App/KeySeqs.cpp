/*******************************************************************************
 * FILENAME: KeySeqs.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2020 Paul Hutchinson.
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
 *    Paul Hutchinson (16 Jul 2020)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "KeySeqs.h"
#include <string.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
const char *m_KeyNames[e_UIKeysMAX]=
{
    "Escape",                                   // e_UIKeys_Escape
    "Tab",                                      // e_UIKeys_Tab
    "Backtab",                                  // e_UIKeys_Backtab
    "Backspace",                                // e_UIKeys_Backspace
    "Return",                                   // e_UIKeys_Return
    "Enter",                                    // e_UIKeys_Enter
    "Insert",                                   // e_UIKeys_Insert
    "Delete",                                   // e_UIKeys_Delete
    "Pause",                                    // e_UIKeys_Pause
    "Print",                                    // e_UIKeys_Print
    "SysReq",                                   // e_UIKeys_SysReq
    "Clear",                                    // e_UIKeys_Clear
    "Home",                                     // e_UIKeys_Home
    "End",                                      // e_UIKeys_End
    "Left",                                     // e_UIKeys_Left
    "Up",                                       // e_UIKeys_Up
    "Right",                                    // e_UIKeys_Right
    "Down",                                     // e_UIKeys_Down
    "PageUp",                                   // e_UIKeys_PageUp
    "PageDown",                                 // e_UIKeys_PageDown
    "Shift",                                    // e_UIKeys_Shift
    "Control",                                  // e_UIKeys_Control
    "Meta",                                     // e_UIKeys_Meta
    "Alt",                                      // e_UIKeys_Alt
    "AltGr",                                    // e_UIKeys_AltGr
    "CapsLock",                                 // e_UIKeys_CapsLock
    "NumLock",                                  // e_UIKeys_NumLock
    "ScrollLock",                               // e_UIKeys_ScrollLock
    "F1",                                       // e_UIKeys_F1
    "F2",                                       // e_UIKeys_F2
    "F3",                                       // e_UIKeys_F3
    "F4",                                       // e_UIKeys_F4
    "F5",                                       // e_UIKeys_F5
    "F6",                                       // e_UIKeys_F6
    "F7",                                       // e_UIKeys_F7
    "F8",                                       // e_UIKeys_F8
    "F9",                                       // e_UIKeys_F9
    "F10",                                      // e_UIKeys_F10
    "F11",                                      // e_UIKeys_F11
    "F12",                                      // e_UIKeys_F12
    "F13",                                      // e_UIKeys_F13
    "F14",                                      // e_UIKeys_F14
    "F15",                                      // e_UIKeys_F15
    "F16",                                      // e_UIKeys_F16
    "F17",                                      // e_UIKeys_F17
    "F18",                                      // e_UIKeys_F18
    "F19",                                      // e_UIKeys_F19
    "F20",                                      // e_UIKeys_F20
    "F21",                                      // e_UIKeys_F21
    "F22",                                      // e_UIKeys_F22
    "F23",                                      // e_UIKeys_F23
    "F24",                                      // e_UIKeys_F24
    "F25",                                      // e_UIKeys_F25
    "F26",                                      // e_UIKeys_F26
    "F27",                                      // e_UIKeys_F27
    "F28",                                      // e_UIKeys_F28
    "F29",                                      // e_UIKeys_F29
    "F30",                                      // e_UIKeys_F30
    "F31",                                      // e_UIKeys_F31
    "F32",                                      // e_UIKeys_F32
    "F33",                                      // e_UIKeys_F33
    "F34",                                      // e_UIKeys_F34
    "F35",                                      // e_UIKeys_F35
    "Super_L",                                  // e_UIKeys_Super_L
    "Super_R",                                  // e_UIKeys_Super_R
    "Menu",                                     // e_UIKeys_Menu
    "Hyper_L",                                  // e_UIKeys_Hyper_L
    "Hyper_R",                                  // e_UIKeys_Hyper_R
    "Help",                                     // e_UIKeys_Help
    "Direction_L",                              // e_UIKeys_Direction_L
    "Direction_R",                              // e_UIKeys_Direction_R
    "Back",                                     // e_UIKeys_Back
    "Forward",                                  // e_UIKeys_Forward
    "Stop",                                     // e_UIKeys_Stop
    "Refresh",                                  // e_UIKeys_Refresh
    "VolumeDown",                               // e_UIKeys_VolumeDown
    "VolumeMute",                               // e_UIKeys_VolumeMute
    "VolumeUp",                                 // e_UIKeys_VolumeUp
    "BassBoost",                                // e_UIKeys_BassBoost
    "BassUp",                                   // e_UIKeys_BassUp
    "BassDown",                                 // e_UIKeys_BassDown
    "TrebleUp",                                 // e_UIKeys_TrebleUp
    "TrebleDown",                               // e_UIKeys_TrebleDown
    "MediaPlay",                                // e_UIKeys_MediaPlay
    "MediaStop",                                // e_UIKeys_MediaStop
    "MediaPrevious",                            // e_UIKeys_MediaPrevious
    "MediaNext",                                // e_UIKeys_MediaNext
    "MediaRecord",                              // e_UIKeys_MediaRecord
    "MediaPause",                               // e_UIKeys_MediaPause
    "MediaTogglePlayPause",                     // e_UIKeys_MediaTogglePlayPause
    "HomePage",                                 // e_UIKeys_HomePage
    "Favorites",                                // e_UIKeys_Favorites
    "Search",                                   // e_UIKeys_Search
    "Standby",                                  // e_UIKeys_Standby
    "OpenUrl",                                  // e_UIKeys_OpenUrl
    "LaunchMail",                               // e_UIKeys_LaunchMail
    "LaunchMedia",                              // e_UIKeys_LaunchMedia
    "Launch0",                                  // e_UIKeys_Launch0
    "Launch1",                                  // e_UIKeys_Launch1
    "Launch2",                                  // e_UIKeys_Launch2
    "Launch3",                                  // e_UIKeys_Launch3
    "Launch4",                                  // e_UIKeys_Launch4
    "Launch5",                                  // e_UIKeys_Launch5
    "Launch6",                                  // e_UIKeys_Launch6
    "Launch7",                                  // e_UIKeys_Launch7
    "Launch8",                                  // e_UIKeys_Launch8
    "Launch9",                                  // e_UIKeys_Launch9
    "LaunchA",                                  // e_UIKeys_LaunchA
    "LaunchB",                                  // e_UIKeys_LaunchB
    "LaunchC",                                  // e_UIKeys_LaunchC
    "LaunchD",                                  // e_UIKeys_LaunchD
    "LaunchE",                                  // e_UIKeys_LaunchE
    "LaunchF",                                  // e_UIKeys_LaunchF
    "LaunchG",                                  // e_UIKeys_LaunchG
    "LaunchH",                                  // e_UIKeys_LaunchH
    "MonBrightnessUp",                          // e_UIKeys_MonBrightnessUp
    "MonBrightnessDown",                        // e_UIKeys_MonBrightnessDown
    "KeyboardLightOnOff",                       // e_UIKeys_KeyboardLightOnOff
    "KeyboardBrightnessUp",                     // e_UIKeys_KeyboardBrightnessUp
    "KeyboardBrightnessDown",                   // e_UIKeys_KeyboardBrightnessDown
    "PowerOff",                                 // e_UIKeys_PowerOff
    "WakeUp",                                   // e_UIKeys_WakeUp
    "Eject",                                    // e_UIKeys_Eject
    "ScreenSaver",                              // e_UIKeys_ScreenSaver
    "WWW",                                      // e_UIKeys_WWW
    "Memo",                                     // e_UIKeys_Memo
    "LightBulb",                                // e_UIKeys_LightBulb
    "Shop",                                     // e_UIKeys_Shop
    "History",                                  // e_UIKeys_History
    "AddFavorite",                              // e_UIKeys_AddFavorite
    "HotLinks",                                 // e_UIKeys_HotLinks
    "BrightnessAdjust",                         // e_UIKeys_BrightnessAdjust
    "Finance",                                  // e_UIKeys_Finance
    "Community",                                // e_UIKeys_Community
    "AudioRewind",                              // e_UIKeys_AudioRewind
    "BackForward",                              // e_UIKeys_BackForward
    "ApplicationLeft",                          // e_UIKeys_ApplicationLeft
    "ApplicationRight",                         // e_UIKeys_ApplicationRight
    "Book",                                     // e_UIKeys_Book
    "CD",                                       // e_UIKeys_CD
    "Calculator",                               // e_UIKeys_Calculator
    "ToDoList",                                 // e_UIKeys_ToDoList
    "ClearGrab",                                // e_UIKeys_ClearGrab
    "Close",                                    // e_UIKeys_Close
    "Copy",                                     // e_UIKeys_Copy
    "Cut",                                      // e_UIKeys_Cut
    "Display",                                  // e_UIKeys_Display
    "DOS",                                      // e_UIKeys_DOS
    "Documents",                                // e_UIKeys_Documents
    "Excel",                                    // e_UIKeys_Excel
    "Explorer",                                 // e_UIKeys_Explorer
    "Game",                                     // e_UIKeys_Game
    "Go",                                       // e_UIKeys_Go
    "iTouch",                                   // e_UIKeys_iTouch
    "LogOff",                                   // e_UIKeys_LogOff
    "Market",                                   // e_UIKeys_Market
    "Meeting",                                  // e_UIKeys_Meeting
    "MenuKB",                                   // e_UIKeys_MenuKB
    "MenuPB",                                   // e_UIKeys_MenuPB
    "MySites",                                  // e_UIKeys_MySites
    "News",                                     // e_UIKeys_News
    "OfficeHome",                               // e_UIKeys_OfficeHome
    "Option",                                   // e_UIKeys_Option
    "Paste",                                    // e_UIKeys_Paste
    "Phone",                                    // e_UIKeys_Phone
    "Calendar",                                 // e_UIKeys_Calendar
    "Reply",                                    // e_UIKeys_Reply
    "Reload",                                   // e_UIKeys_Reload
    "RotateWindows",                            // e_UIKeys_RotateWindows
    "RotationPB",                               // e_UIKeys_RotationPB
    "RotationKB",                               // e_UIKeys_RotationKB
    "Save",                                     // e_UIKeys_Save
    "Send",                                     // e_UIKeys_Send
    "Spell",                                    // e_UIKeys_Spell
    "SplitScreen",                              // e_UIKeys_SplitScreen
    "Support",                                  // e_UIKeys_Support
    "TaskPane",                                 // e_UIKeys_TaskPane
    "Terminal",                                 // e_UIKeys_Terminal
    "Tools",                                    // e_UIKeys_Tools
    "Travel",                                   // e_UIKeys_Travel
    "Video",                                    // e_UIKeys_Video
    "Word",                                     // e_UIKeys_Word
    "Xfer",                                     // e_UIKeys_Xfer
    "ZoomIn",                                   // e_UIKeys_ZoomIn
    "ZoomOut",                                  // e_UIKeys_ZoomOut
    "Away",                                     // e_UIKeys_Away
    "Messenger",                                // e_UIKeys_Messenger
    "WebCam",                                   // e_UIKeys_WebCam
    "MailForward",                              // e_UIKeys_MailForward
    "Pictures",                                 // e_UIKeys_Pictures
    "Music",                                    // e_UIKeys_Music
    "Battery",                                  // e_UIKeys_Battery
    "Bluetooth",                                // e_UIKeys_Bluetooth
    "WLAN",                                     // e_UIKeys_WLAN
    "UWB",                                      // e_UIKeys_UWB
    "AudioForward",                             // e_UIKeys_AudioForward
    "AudioRepeat",                              // e_UIKeys_AudioRepeat
    "AudioRandomPlay",                          // e_UIKeys_AudioRandomPlay
    "Subtitle",                                 // e_UIKeys_Subtitle
    "AudioCycleTrack",                          // e_UIKeys_AudioCycleTrack
    "Time",                                     // e_UIKeys_Time
    "Hibernate",                                // e_UIKeys_Hibernate
    "View",                                     // e_UIKeys_View
    "TopMenu",                                  // e_UIKeys_TopMenu
    "PowerDown",                                // e_UIKeys_PowerDown
    "Suspend",                                  // e_UIKeys_Suspend
    "ContrastAdjust",                           // e_UIKeys_ContrastAdjust
    "MediaLast",                                // e_UIKeys_MediaLast
    "unknown",                                  // e_UIKeys_unknown
    "Call",                                     // e_UIKeys_Call
    "Camera",                                   // e_UIKeys_Camera
    "CameraFocus",                              // e_UIKeys_CameraFocus
    "Context1",                                 // e_UIKeys_Context1
    "Context2",                                 // e_UIKeys_Context2
    "Context3",                                 // e_UIKeys_Context3
    "Context4",                                 // e_UIKeys_Context4
    "Flip",                                     // e_UIKeys_Flip
    "Hangup",                                   // e_UIKeys_Hangup
    "No",                                       // e_UIKeys_No
    "Select",                                   // e_UIKeys_Select
    "Yes",                                      // e_UIKeys_Yes
    "ToggleCallHangup",                         // e_UIKeys_ToggleCallHangup
    "VoiceDial",                                // e_UIKeys_VoiceDial
    "LastNumberRedial",                         // e_UIKeys_LastNumberRedial
    "Execute",                                  // e_UIKeys_Execute
    "Printer",                                  // e_UIKeys_Printer
    "Play",                                     // e_UIKeys_Play
    "Sleep",                                    // e_UIKeys_Sleep
    "Zoom",                                     // e_UIKeys_Zoom
    "Cancel"                                    // e_UIKeys_Cancel
};

/*******************************************************************************
 * NAME:
 *    ConvertKeyName2ENum
 *
 * SYNOPSIS:
 *    e_UIKeys ConvertKeyName2ENum(const char *Name);
 *
 * PARAMETERS:
 *    Name [I] -- The name of the key to convert to the enum
 *
 * FUNCTION:
 *    This function converts a string of a key name to the keys enum.
 *
 * RETURNS:
 *    The key that this string matches or 'e_UIKeysMAX' if not found.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
e_UIKeys ConvertKeyName2ENum(const char *Name)
{
    int r;

    for(r=0;r<e_UIKeysMAX;r++)
    {
        if(strcmp(m_KeyNames[r],Name)==0)
            return (e_UIKeys)r;
    }
    return e_UIKeysMAX;
}

/*******************************************************************************
 * NAME:
 *    ConvertENum2KeyName
 *
 * SYNOPSIS:
 *    const char *ConvertENum2KeyName(e_UIKeys Key);
 *
 * PARAMETERS:
 *    Key [I] -- The key to convert
 *
 * FUNCTION:
 *    This function converts a key enum to a string.
 *
 * RETURNS:
 *    A static string of the key name.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
const char *ConvertENum2KeyName(e_UIKeys Key)
{
    if(Key>=e_UIKeysMAX)
        return "";
    return m_KeyNames[Key];
}

/*******************************************************************************
 * NAME:
 *    ConvertKeySeq2String
 *
 * SYNOPSIS:
 *    const char *ConvertKeySeq2String(struct CommandKeySeq *KeySeq);
 *
 * PARAMETERS:
 *    KeySeq [I] -- The key seq to convert to a string
 *
 * FUNCTION:
 *    This function converts a key seq to a string.
 *
 * RETURNS:
 *    A pointer to a static buffer with the key seq string in it.  This will
 *    be overwritten the next time this function is called.
 *
 * SEE ALSO:
 *    ConvertString2KeySeq()
 ******************************************************************************/
const char *ConvertKeySeq2String(struct CommandKeySeq *KeySeq)
{
    static char KeyStr[200];

    KeyStr[0]=0;
    if(KeySeq->Mod&KEYMOD_SHIFT)
        strcat(KeyStr,"SHIFT+");
    if(KeySeq->Mod&KEYMOD_CONTROL)
        strcat(KeyStr,"CTRL+");
    if(KeySeq->Mod&KEYMOD_ALT)
        strcat(KeyStr,"ALT+");
    if(KeySeq->Mod&KEYMOD_LOGO)
        strcat(KeyStr,"LOGO+");

    if(KeySeq->Key==e_UIKeysMAX)
    {
        /* Use the letter */
        if(KeySeq->Letter==0)
        {
            strcat(KeyStr,"None");
        }
        else if(KeySeq->Letter<' ' || KeySeq->Letter>=127)
        {
            strcat(KeyStr,"Unknown");
        }
        else
        {
            KeyStr[strlen(KeyStr)+1]=0;
            KeyStr[strlen(KeyStr)]=KeySeq->Letter;
        }
    }
    else
    {
        strcat(KeyStr,ConvertENum2KeyName(KeySeq->Key));
    }
    return KeyStr;
}

/*******************************************************************************
 * NAME:
 *    ConvertString2KeySeq
 *
 * SYNOPSIS:
 *    bool ConvertString2KeySeq(struct CommandKeySeq *KeySeq,const char *Str);
 *
 * PARAMETERS:
 *    KeySeq [O] -- The key seq that this string goes with.
 *    Str [I] -- The string to convert
 *
 * FUNCTION:
 *    This function takes a string and converts it to a key seq.  If the
 *    string could not be converted then 'KeqSeq' will be set to:
 *          Key=e_UIKeysMAX;
 *          Letter=0;
 *
 * RETURNS:
 *    true -- This function was able to figure out the key seq
 *    false -- Invalid key seq
 *
 * SEE ALSO:
 *    ConvertKeySeq2String()
 ******************************************************************************/
bool ConvertString2KeySeq(struct CommandKeySeq *KeySeq,const char *Str)
{
    const char *Point;
    const char *LastPoint;

    KeySeq->Mod=0;
    KeySeq->Key=e_UIKeysMAX;
    KeySeq->Letter=0;

    if(strcasecmp(Str,"None")==0 || *Str==0)
        return true;

    if(Str[0]=='+' && Str[1]=='\0')
    {
        /* Special case where user has sent in "+" as the key to assign */
        Point=Str;
        LastPoint=Str;
    }
    else
    {
        Point=Str;
        LastPoint=Point;
        do
        {
            Point=strchr(Point,'+');
            if(Point!=NULL)
            {
                if(strncasecmp(LastPoint,"SHIFT",5)==0)
                    KeySeq->Mod|=KEYMOD_SHIFT;
                else if(strncasecmp(LastPoint,"CTRL",4)==0)
                    KeySeq->Mod|=KEYMOD_CONTROL;
                else if(strncasecmp(LastPoint,"ALT",3)==0)
                    KeySeq->Mod|=KEYMOD_ALT;
                else if(strncasecmp(LastPoint,"LOGO",4)==0)
                    KeySeq->Mod|=KEYMOD_LOGO;

                Point++;    // Move past the +
                LastPoint=Point;

                /* Special case where we have something like CTRL++ */
                if(*Point=='+')
                    break;
            }
        } while(Point!=NULL);
    }

    /* Find this key name */
    KeySeq->Key=ConvertKeyName2ENum(LastPoint);
    if(KeySeq->Key==e_UIKeysMAX)
    {
        /* Use the letter */
        KeySeq->Letter=*LastPoint;
        if(*LastPoint==0 || *LastPoint==' ')
            return false;
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    KeySeqMapped
 *
 * SYNOPSIS:
 *    bool KeySeqMapped(struct CommandKeySeq *KeySeq);
 *
 * PARAMETERS:
 *    KeySeq [I] -- The key seq to look at
 *
 * FUNCTION:
 *    This function checks to see if the key seq is assigned to a key or
 *    none.
 *
 * RETURNS:
 *    true -- Key seq is a valid key
 *    false -- key seq maps to none.  This key seq can not be used to send
 *             commands.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool KeySeqMapped(struct CommandKeySeq *KeySeq)
{
    if(KeySeq->Key==e_UIKeysMAX)
    {
        if(KeySeq->Letter==0)
            return false;
        if(KeySeq->Letter<' ' || KeySeq->Letter>=127)
            return false;
    }

    return true;
}
