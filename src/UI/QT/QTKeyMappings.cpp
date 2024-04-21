/*******************************************************************************
 * FILENAME: QTKeyMappings.cpp
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
 *    Paul Hutchinson (12 Jul 2020)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "QTKeyMappings.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

Qt::Key m_UIKeys2QTKeys[e_UIKeysMAX]=
{
    Qt::Key_Escape, // e_UIKeys_Escape
    Qt::Key_Tab, // e_UIKeys_Tab
    Qt::Key_Backtab, // e_UIKeys_Backtab
    Qt::Key_Backspace, // e_UIKeys_Backspace
    Qt::Key_Return, // e_UIKeys_Return
    Qt::Key_Enter, // e_UIKeys_Enter
    Qt::Key_Insert, // e_UIKeys_Insert
    Qt::Key_Delete, // e_UIKeys_Delete
    Qt::Key_Pause, // e_UIKeys_Pause
    Qt::Key_Print, // e_UIKeys_Print
    Qt::Key_SysReq, // e_UIKeys_SysReq
    Qt::Key_Clear, // e_UIKeys_Clear
    Qt::Key_Home, // e_UIKeys_Home
    Qt::Key_End, // e_UIKeys_End
    Qt::Key_Left, // e_UIKeys_Left
    Qt::Key_Up, // e_UIKeys_Up
    Qt::Key_Right, // e_UIKeys_Right
    Qt::Key_Down, // e_UIKeys_Down
    Qt::Key_PageUp, // e_UIKeys_PageUp
    Qt::Key_PageDown, // e_UIKeys_PageDown
    Qt::Key_Shift, // e_UIKeys_Shift
    Qt::Key_Control, // e_UIKeys_Control
    Qt::Key_Meta, // e_UIKeys_Meta
    Qt::Key_Alt, // e_UIKeys_Alt
    Qt::Key_AltGr, // e_UIKeys_AltGr
    Qt::Key_CapsLock, // e_UIKeys_CapsLock
    Qt::Key_NumLock, // e_UIKeys_NumLock
    Qt::Key_ScrollLock, // e_UIKeys_ScrollLock
    Qt::Key_F1, // e_UIKeys_F1
    Qt::Key_F2, // e_UIKeys_F2
    Qt::Key_F3, // e_UIKeys_F3
    Qt::Key_F4, // e_UIKeys_F4
    Qt::Key_F5, // e_UIKeys_F5
    Qt::Key_F6, // e_UIKeys_F6
    Qt::Key_F7, // e_UIKeys_F7
    Qt::Key_F8, // e_UIKeys_F8
    Qt::Key_F9, // e_UIKeys_F9
    Qt::Key_F10, // e_UIKeys_F10
    Qt::Key_F11, // e_UIKeys_F11
    Qt::Key_F12, // e_UIKeys_F12
    Qt::Key_F13, // e_UIKeys_F13
    Qt::Key_F14, // e_UIKeys_F14
    Qt::Key_F15, // e_UIKeys_F15
    Qt::Key_F16, // e_UIKeys_F16
    Qt::Key_F17, // e_UIKeys_F17
    Qt::Key_F18, // e_UIKeys_F18
    Qt::Key_F19, // e_UIKeys_F19
    Qt::Key_F20, // e_UIKeys_F20
    Qt::Key_F21, // e_UIKeys_F21
    Qt::Key_F22, // e_UIKeys_F22
    Qt::Key_F23, // e_UIKeys_F23
    Qt::Key_F24, // e_UIKeys_F24
    Qt::Key_F25, // e_UIKeys_F25
    Qt::Key_F26, // e_UIKeys_F26
    Qt::Key_F27, // e_UIKeys_F27
    Qt::Key_F28, // e_UIKeys_F28
    Qt::Key_F29, // e_UIKeys_F29
    Qt::Key_F30, // e_UIKeys_F30
    Qt::Key_F31, // e_UIKeys_F31
    Qt::Key_F32, // e_UIKeys_F32
    Qt::Key_F33, // e_UIKeys_F33
    Qt::Key_F34, // e_UIKeys_F34
    Qt::Key_F35, // e_UIKeys_F35
    Qt::Key_Super_L, // e_UIKeys_Super_L
    Qt::Key_Super_R, // e_UIKeys_Super_R
    Qt::Key_Menu, // e_UIKeys_Menu
    Qt::Key_Hyper_L, // e_UIKeys_Hyper_L
    Qt::Key_Hyper_R, // e_UIKeys_Hyper_R
    Qt::Key_Help, // e_UIKeys_Help
    Qt::Key_Direction_L, // e_UIKeys_Direction_L
    Qt::Key_Direction_R, // e_UIKeys_Direction_R
    Qt::Key_Back, // e_UIKeys_Back
    Qt::Key_Forward, // e_UIKeys_Forward
    Qt::Key_Stop, // e_UIKeys_Stop
    Qt::Key_Refresh, // e_UIKeys_Refresh
    Qt::Key_VolumeDown, // e_UIKeys_VolumeDown
    Qt::Key_VolumeMute, // e_UIKeys_VolumeMute
    Qt::Key_VolumeUp, // e_UIKeys_VolumeUp
    Qt::Key_BassBoost, // e_UIKeys_BassBoost
    Qt::Key_BassUp, // e_UIKeys_BassUp
    Qt::Key_BassDown, // e_UIKeys_BassDown
    Qt::Key_TrebleUp, // e_UIKeys_TrebleUp
    Qt::Key_TrebleDown, // e_UIKeys_TrebleDown
    Qt::Key_MediaPlay, // e_UIKeys_MediaPlay
    Qt::Key_MediaStop, // e_UIKeys_MediaStop
    Qt::Key_MediaPrevious, // e_UIKeys_MediaPrevious
    Qt::Key_MediaNext, // e_UIKeys_MediaNext
    Qt::Key_MediaRecord, // e_UIKeys_MediaRecord
    Qt::Key_MediaPause, // e_UIKeys_MediaPause
    Qt::Key_MediaTogglePlayPause, // e_UIKeys_MediaTogglePlayPause
    Qt::Key_HomePage, // e_UIKeys_HomePage
    Qt::Key_Favorites, // e_UIKeys_Favorites
    Qt::Key_Search, // e_UIKeys_Search
    Qt::Key_Standby, // e_UIKeys_Standby
    Qt::Key_OpenUrl, // e_UIKeys_OpenUrl
    Qt::Key_LaunchMail, // e_UIKeys_LaunchMail
    Qt::Key_LaunchMedia, // e_UIKeys_LaunchMedia
    Qt::Key_Launch0, // e_UIKeys_Launch0
    Qt::Key_Launch1, // e_UIKeys_Launch1
    Qt::Key_Launch2, // e_UIKeys_Launch2
    Qt::Key_Launch3, // e_UIKeys_Launch3
    Qt::Key_Launch4, // e_UIKeys_Launch4
    Qt::Key_Launch5, // e_UIKeys_Launch5
    Qt::Key_Launch6, // e_UIKeys_Launch6
    Qt::Key_Launch7, // e_UIKeys_Launch7
    Qt::Key_Launch8, // e_UIKeys_Launch8
    Qt::Key_Launch9, // e_UIKeys_Launch9
    Qt::Key_LaunchA, // e_UIKeys_LaunchA
    Qt::Key_LaunchB, // e_UIKeys_LaunchB
    Qt::Key_LaunchC, // e_UIKeys_LaunchC
    Qt::Key_LaunchD, // e_UIKeys_LaunchD
    Qt::Key_LaunchE, // e_UIKeys_LaunchE
    Qt::Key_LaunchF, // e_UIKeys_LaunchF
    Qt::Key_LaunchG, // e_UIKeys_LaunchG
    Qt::Key_LaunchH, // e_UIKeys_LaunchH
    Qt::Key_MonBrightnessUp, // e_UIKeys_MonBrightnessUp
    Qt::Key_MonBrightnessDown, // e_UIKeys_MonBrightnessDown
    Qt::Key_KeyboardLightOnOff, // e_UIKeys_KeyboardLightOnOff
    Qt::Key_KeyboardBrightnessUp, // e_UIKeys_KeyboardBrightnessUp
    Qt::Key_KeyboardBrightnessDown, // e_UIKeys_KeyboardBrightnessDown
    Qt::Key_PowerOff, // e_UIKeys_PowerOff
    Qt::Key_WakeUp, // e_UIKeys_WakeUp
    Qt::Key_Eject, // e_UIKeys_Eject
    Qt::Key_ScreenSaver, // e_UIKeys_ScreenSaver
    Qt::Key_WWW, // e_UIKeys_WWW
    Qt::Key_Memo, // e_UIKeys_Memo
    Qt::Key_LightBulb, // e_UIKeys_LightBulb
    Qt::Key_Shop, // e_UIKeys_Shop
    Qt::Key_History, // e_UIKeys_History
    Qt::Key_AddFavorite, // e_UIKeys_AddFavorite
    Qt::Key_HotLinks, // e_UIKeys_HotLinks
    Qt::Key_BrightnessAdjust, // e_UIKeys_BrightnessAdjust
    Qt::Key_Finance, // e_UIKeys_Finance
    Qt::Key_Community, // e_UIKeys_Community
    Qt::Key_AudioRewind, // e_UIKeys_AudioRewind
    Qt::Key_BackForward, // e_UIKeys_BackForward
    Qt::Key_ApplicationLeft, // e_UIKeys_ApplicationLeft
    Qt::Key_ApplicationRight, // e_UIKeys_ApplicationRight
    Qt::Key_Book, // e_UIKeys_Book
    Qt::Key_CD, // e_UIKeys_CD
    Qt::Key_Calculator, // e_UIKeys_Calculator
    Qt::Key_ToDoList, // e_UIKeys_ToDoList
    Qt::Key_ClearGrab, // e_UIKeys_ClearGrab
    Qt::Key_Close, // e_UIKeys_Close
    Qt::Key_Copy, // e_UIKeys_Copy
    Qt::Key_Cut, // e_UIKeys_Cut
    Qt::Key_Display, // e_UIKeys_Display
    Qt::Key_DOS, // e_UIKeys_DOS
    Qt::Key_Documents, // e_UIKeys_Documents
    Qt::Key_Excel, // e_UIKeys_Excel
    Qt::Key_Explorer, // e_UIKeys_Explorer
    Qt::Key_Game, // e_UIKeys_Game
    Qt::Key_Go, // e_UIKeys_Go
    Qt::Key_iTouch, // e_UIKeys_iTouch
    Qt::Key_LogOff, // e_UIKeys_LogOff
    Qt::Key_Market, // e_UIKeys_Market
    Qt::Key_Meeting, // e_UIKeys_Meeting
    Qt::Key_MenuKB, // e_UIKeys_MenuKB
    Qt::Key_MenuPB, // e_UIKeys_MenuPB
    Qt::Key_MySites, // e_UIKeys_MySites
    Qt::Key_News, // e_UIKeys_News
    Qt::Key_OfficeHome, // e_UIKeys_OfficeHome
    Qt::Key_Option, // e_UIKeys_Option
    Qt::Key_Paste, // e_UIKeys_Paste
    Qt::Key_Phone, // e_UIKeys_Phone
    Qt::Key_Calendar, // e_UIKeys_Calendar
    Qt::Key_Reply, // e_UIKeys_Reply
    Qt::Key_Reload, // e_UIKeys_Reload
    Qt::Key_RotateWindows, // e_UIKeys_RotateWindows
    Qt::Key_RotationPB, // e_UIKeys_RotationPB
    Qt::Key_RotationKB, // e_UIKeys_RotationKB
    Qt::Key_Save, // e_UIKeys_Save
    Qt::Key_Send, // e_UIKeys_Send
    Qt::Key_Spell, // e_UIKeys_Spell
    Qt::Key_SplitScreen, // e_UIKeys_SplitScreen
    Qt::Key_Support, // e_UIKeys_Support
    Qt::Key_TaskPane, // e_UIKeys_TaskPane
    Qt::Key_Terminal, // e_UIKeys_Terminal
    Qt::Key_Tools, // e_UIKeys_Tools
    Qt::Key_Travel, // e_UIKeys_Travel
    Qt::Key_Video, // e_UIKeys_Video
    Qt::Key_Word, // e_UIKeys_Word
    Qt::Key_Xfer, // e_UIKeys_Xfer
    Qt::Key_ZoomIn, // e_UIKeys_ZoomIn
    Qt::Key_ZoomOut, // e_UIKeys_ZoomOut
    Qt::Key_Away, // e_UIKeys_Away
    Qt::Key_Messenger, // e_UIKeys_Messenger
    Qt::Key_WebCam, // e_UIKeys_WebCam
    Qt::Key_MailForward, // e_UIKeys_MailForward
    Qt::Key_Pictures, // e_UIKeys_Pictures
    Qt::Key_Music, // e_UIKeys_Music
    Qt::Key_Battery, // e_UIKeys_Battery
    Qt::Key_Bluetooth, // e_UIKeys_Bluetooth
    Qt::Key_WLAN, // e_UIKeys_WLAN
    Qt::Key_UWB, // e_UIKeys_UWB
    Qt::Key_AudioForward, // e_UIKeys_AudioForward
    Qt::Key_AudioRepeat, // e_UIKeys_AudioRepeat
    Qt::Key_AudioRandomPlay, // e_UIKeys_AudioRandomPlay
    Qt::Key_Subtitle, // e_UIKeys_Subtitle
    Qt::Key_AudioCycleTrack, // e_UIKeys_AudioCycleTrack
    Qt::Key_Time, // e_UIKeys_Time
    Qt::Key_Hibernate, // e_UIKeys_Hibernate
    Qt::Key_View, // e_UIKeys_View
    Qt::Key_TopMenu, // e_UIKeys_TopMenu
    Qt::Key_PowerDown, // e_UIKeys_PowerDown
    Qt::Key_Suspend, // e_UIKeys_Suspend
    Qt::Key_ContrastAdjust, // e_UIKeys_ContrastAdjust
    Qt::Key_MediaLast, // e_UIKeys_MediaLast
    Qt::Key_unknown, // e_UIKeys_unknown
    Qt::Key_Call, // e_UIKeys_Call
    Qt::Key_Camera, // e_UIKeys_Camera
    Qt::Key_CameraFocus, // e_UIKeys_CameraFocus
    Qt::Key_Context1, // e_UIKeys_Context1
    Qt::Key_Context2, // e_UIKeys_Context2
    Qt::Key_Context3, // e_UIKeys_Context3
    Qt::Key_Context4, // e_UIKeys_Context4
    Qt::Key_Flip, // e_UIKeys_Flip
    Qt::Key_Hangup, // e_UIKeys_Hangup
    Qt::Key_No, // e_UIKeys_No
    Qt::Key_Select, // e_UIKeys_Select
    Qt::Key_Yes, // e_UIKeys_Yes
    Qt::Key_ToggleCallHangup, // e_UIKeys_ToggleCallHangup
    Qt::Key_VoiceDial, // e_UIKeys_VoiceDial
    Qt::Key_LastNumberRedial, // e_UIKeys_LastNumberRedial
    Qt::Key_Execute, // e_UIKeys_Execute
    Qt::Key_Printer, // e_UIKeys_Printer
    Qt::Key_Play, // e_UIKeys_Play
    Qt::Key_Sleep, // e_UIKeys_Sleep
    Qt::Key_Zoom, // e_UIKeys_Zoom
    Qt::Key_Cancel, // e_UIKeys_Cancel
};

Qt::Key ConvertUIKey2QTKey(e_UIKeys UIKey)
{
    if(UIKey>=sizeof(m_UIKeys2QTKeys)/sizeof(Qt::Key))
        return Qt::Key_unknown;
    return m_UIKeys2QTKeys[UIKey];
}

e_UIKeys ConvertQTKey2UIKey(Qt::Key QKey)
{
    int r;

    for(r=0;r<e_UIKeysMAX;r++)
        if(m_UIKeys2QTKeys[r]==QKey)
            return (e_UIKeys)r;
    return e_UIKeysMAX;
}
