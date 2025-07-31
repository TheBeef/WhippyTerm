/*
TODO:
 - Binary blinks when no focus
*/

/*******************************************************************************
 * FILENAME: DisplayBase.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This is the display base class.
 *
 * COPYRIGHT:
 *    Copyright 2023 Paul Hutchinson.
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
 *    Paul Hutchinson (03 Aug 2023)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "DisplayBase.h"
#include "App/Dialogs/Dialog_EditSendBuffer.h"
#include "App/Settings.h"
#include "App/Settings.h"
#include "UI/UIAsk.h"
#include <string>
#include <string.h>

using namespace std;

/*** DEFINES                  ***/
#define LOCAL_SEND_BUFFER_STARTING_SIZE         100

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
bool DisplayBase_HexInputEvent(const struct HDEvent *Event);

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    DisplayBase_HexInputEvent
 *
 * SYNOPSIS:
 *    static bool DisplayBase_HexInputEvent(const struct HDEvent *
 *              Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event from the hex display buffer.
 *
 * FUNCTION:
 *    This function handles events from the hex display.
 *
 * RETURNS:
 *    true -- Accept this event
 *    false -- Cancel this event
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayBase_HexInputEvent(const struct HDEvent *Event)
{
    class DisplayBase *DT=(class DisplayBase *)Event->ID;

    return DT->DoHexInputEvent(Event);
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::DisplayBase
 *
 * SYNOPSIS:
 *    DisplayBase::DisplayBase();
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This is the constructor for this class.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    InitBase();
 ******************************************************************************/
DisplayBase::DisplayBase()
{
    Settings=&g_Settings.DefaultConSettings;
    HasFocus=false;

    HexInput=NULL;
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::~DisplayBase
 *
 * SYNOPSIS:
 *    DisplayBase::~DisplayBase();
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This is the destructor for this class.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
DisplayBase::~DisplayBase()
{
}

/*******************************************************************************
 * NAME:
 *    DisplayBuffer::InitBase
 *
 * SYNOPSIS:
 *    bool DisplayBuffer::InitBase(bool (*EventCallback)(const struct
 *          DBEvent *Event),uintptr_t UserData);
 *
 * PARAMETERS:
 *    EventCallback [I] -- The event callback.  See below.
 *    UserData [I] -- The user data that will be sent to the event callback.
 *
 * FUNCTION:
 *    This funciton init's the display.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error.  User has NOT been informed
 *
 * CALLBACKS:
 * =============================================================================
 * NAME:
 *    EventCallback
 *
 * SYNOPSIS:
 *    bool EventCallback(const struct DBEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event
 *
 * FUNCTION:
 *    This function is called when there is an event from the display.
 *
 * RETURNS:
 *    true -- The event should act normally
 *    false -- There event should be canceled
 *
 * SEE ALSO:
 *    
 * =============================================================================
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayBase::InitBase(bool (*EventCallback)(const struct DBEvent *Event),
        uintptr_t UserData)
{
    DBEventHandler=EventCallback;
    EventHandlerUserData=UserData;

    return true;
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::SetCustomSettings
 *
 * SYNOPSIS:
 *    void DisplayBase::SetCustomSettings(class ConSettings *NewSettingsPtr);
 *
 * PARAMETERS:
 *    NewSettingsPtr [I] -- A pointer to the settings you want to use with
 *                          this display.  If NULL then the global one will
 *                          be used.
 *
 * FUNCTION:
 *    This function changes where the settings are being read from for this
 *    display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::SetCustomSettings(class ConSettings *NewSettingsPtr)
{
    Settings=NewSettingsPtr;
    if(Settings==nullptr)
        Settings=&g_Settings.DefaultConSettings;
    ApplySettings();
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::GetCustomSettings
 *
 * SYNOPSIS:
 *    class ConSettings *DisplayBase::GetCustomSettings(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the connection settings that have been applied to
 *    this display.
 *
 * RETURNS:
 *    A pointer to the connection settings being used by this display.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
class ConSettings *DisplayBase::GetCustomSettings(void)
{
    return Settings;
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::SendEvent
 *
 * SYNOPSIS:
 *    void DisplayBase::SendEvent(e_DBEventType EventID,
 *          const union DBEventData *Info);
 *
 * PARAMETERS:
 *    EventID [I] -- The event we are doing
 *    Info [I] -- The extra info.  This can be NULL.
 *
 * FUNCTION:
 *    This function sends an event to the registered event callback.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::SendEvent(e_DBEventType EventID,const union DBEventData *Info)
{
    struct DBEvent Event;

    if(DBEventHandler==nullptr)
        return;

    Event.EventType=EventID;
    Event.UserData=EventHandlerUserData;
    Event.Info=Info;

    DBEventHandler(&Event);
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::SetBlockDeviceMode
 *
 * SYNOPSIS:
 *    void DisplayBase::SetBlockDeviceMode(bool On);
 *
 * PARAMETERS:
 *    On [I] -- This device is a block device (true), or a steam device (false)
 *
 * FUNCTION:
 *    This function changes if the device this display is connected to is a
 *    block or steam device.
 *
 *    This is the default version and does nothing.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::SetBlockDeviceMode(bool On)
{
    /* We do nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::NoteNonPrintable
 *
 * SYNOPSIS:
 *    void DisplayBase::NoteNonPrintable(const char *NoteStr);
 *
 * PARAMETERS:
 *    NoteStr [I] -- The string to add as a non-printable note.
 *
 * FUNCTION:
 *    This function adds a decoded non-printable char.  When something is
 *    decoded that does not result in a printable char this function can be
 *    used to add a note about the char (which the user can show or hide).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::NoteNonPrintable(const char *NoteStr)
{
    /* Do nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::SetShowNonPrintable
 *
 * SYNOPSIS:
 *    void DisplayBase::SetShowNonPrintable(bool Show);
 *
 * PARAMETERS:
 *    Show [I] -- true = show the non-printable chars, false = hide them.
 *
 * FUNCTION:
 *    This function sets if this display should show non-printable chars.
 *    Not all displays support this (they may always show them, or always
 *    hide them).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::SetShowNonPrintable(bool Show)
{
    /* Do nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::SetShowEndOfLines
 *
 * SYNOPSIS:
 *    void DisplayBase::SetShowEndOfLines(bool Show);
 *
 * PARAMETERS:
 *    Show [I] -- true = show the end of lines, false = hide them.
 *
 * FUNCTION:
 *    This function sets if this display should show end of lines.
 *    Not all displays support this (they may always show them, or always
 *    hide them).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::SetShowEndOfLines(bool Show)
{
    /* Do nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::ApplySettings
 *
 * SYNOPSIS:
 *    void DisplayBase::ApplySettings(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function needs to be called after there is a change to the con
 *    settings that this display has been connected to.
 *
 *    It will look things again and apply them to this display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::ApplySettings(void)
{
    /* Do nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::SetCursorBlinking
 *
 * SYNOPSIS:
 *    void DisplayBase::SetCursorBlinking(bool Blinking);
 *
 * PARAMETERS:
 *    Blinking [I] -- Is the cursor blinking (true), or on solid (false)
 *
 * FUNCTION:
 *    This function changes if the cursor is blinking or not.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::SetCursorBlinking(bool Blinking)
{
    /* Do nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::SetCursorStyle
 *
 * SYNOPSIS:
 *    void DisplayBase::SetCursorStyle(e_TextCursorStyleType Style)
 *
 * PARAMETERS:
 *    Style [I] -- What style to draw the cursor in
 *
 * FUNCTION:
 *    This function changes what style the cursor is drawen as in this display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::SetCursorStyle(e_TextCursorStyleType Style)
{
    /* Do nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::GetInFocus
 *
 * SYNOPSIS:
 *    bool DisplayBase::GetInFocus(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets if this display has focus or not.
 *
 * RETURNS:
 *    true -- Has focus
 *    false -- Does not have focus
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayBase::GetInFocus(void)
{
    return HasFocus;
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::SetInFocus
 *
 * SYNOPSIS:
 *    void DisplayBase::SetInFocus(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gives focus to this display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    GetInFocus()
 ******************************************************************************/
void DisplayBase::SetInFocus(void)
{
    HasFocus=true;
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::SetCursorXY
 *
 * SYNOPSIS:
 *    void DisplayBase::SetCursorXY(unsigned int x,unsigned y);
 *
 * PARAMETERS:
 *    x [I] -- The new x position of the cursor
 *    y [I] -- The new y position of the cursor
 *
 * FUNCTION:
 *    This function moves the cursor in the display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::SetCursorXY(unsigned int x,unsigned y)
{
    /* Do nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::GetCursorXY
 *
 * SYNOPSIS:
 *    void DisplayBase::GetCursorXY(unsigned int *x,unsigned int *y);
 *
 * PARAMETERS:
 *    x [O] -- The X pos of the cursor
 *    y [O] -- The Y pos of the cursor
 *
 * FUNCTION:
 *    This function gets the current pos of the cursor on the display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::GetCursorXY(unsigned int *x,unsigned int *y)
{
    /* Do nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::AddTab
 *
 * SYNOPSIS:
 *    void DisplayBase::AddTab(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function handles when a driver send a tab command.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::AddTab(void)
{
    /* Do nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::AddReverseTab
 *
 * SYNOPSIS:
 *    void DisplayBase::AddReverseTab(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function handles when a driver send a goto previous tab stop command.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::AddReverseTab(void)
{
    /* Do nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::DoBackspace
 *
 * SYNOPSIS:
 *    void DisplayBase::DoBackspace(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function handles when a driver send a backspace command.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::DoBackspace(void)
{
    /* Do nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::DoReturn
 *
 * SYNOPSIS:
 *    void DisplayBase::DoReturn(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function handles when a driver send a cartridge return command.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::DoReturn(void)
{
    /* Do nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::DoLineFeed
 *
 * SYNOPSIS:
 *    void DisplayBase::DoLineFeed(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function handles when a driver send a line feed command.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::DoLineFeed(void)
{
    /* Do nothing */
}

///*******************************************************************************
// * NAME:
// *    DisplayBase::GetSelectionString
// *
// * SYNOPSIS:
// *    bool DisplayBase::GetSelectionString(std::string &Clip);
// *
// * PARAMETERS:
// *    Clip [O] -- The text from the selection
// *
// * FUNCTION:
// *    This function gets a copy of the selection text.
// *
// * RETURNS:
// *    true -- The selection was valid and 'Clip' has been set
// *    false --- There was no selection and 'Clip' has been set to ""
// *
// * SEE ALSO:
// *    IsThereASelection(), SelectAll(), ClearSelection()
// ******************************************************************************/
//bool DisplayBase::GetSelectionString(std::string &Clip)
//{
//    /* Do nothing */
//    Clip="";
//    return false;
//}
//
///*******************************************************************************
// * NAME:
// *    DisplayBase::IsThereASelection
// *
// * SYNOPSIS:
// *    bool DisplayBase::IsThereASelection(void);
// *
// * PARAMETERS:
// *    NONE
// *
// * FUNCTION:
// *    This function checks to see if there is selected text.
// *
// * RETURNS:
// *    true -- There is text that can be copied to the clip board
// *    false -- There is not selection (nothing to copy).
// *
// * SEE ALSO:
// *    DisplayBase::GetSelectionString()
// ******************************************************************************/
//bool DisplayBase::IsThereASelection(void)
//{
//    /* Do nothing */
//    return false;
//}
//
///*******************************************************************************
// * NAME:
// *    DisplayBase::SelectAll
// *
// * SYNOPSIS:
// *    void DisplayBase::SelectAll(void);
// *
// * PARAMETERS:
// *    NONE
// *
// * FUNCTION:
// *    This function sets the selection to select everything.
// *
// * RETURNS:
// *    NONE
// *
// * SEE ALSO:
// *    DisplayBase::GetSelectionString()
// ******************************************************************************/
//void DisplayBase::SelectAll(void)
//{
//    /* Do nothing */
//}
//
///*******************************************************************************
// * NAME:
// *    DisplayBase::ClearSelection
// *
// * SYNOPSIS:
// *    void DisplayBase::ClearSelection(void);
// *
// * PARAMETERS:
// *    NONE
// *
// * FUNCTION:
// *    This function sets the selection to nothing.
// *
// * RETURNS:
// *    NONE
// *
// * SEE ALSO:
// *    GetSelectionString()
// ******************************************************************************/
//void DisplayBase::ClearSelection(void)
//{
//    /* Do nothing */
//}

/*******************************************************************************
 * NAME:
 *    DisplayBase::SetOverrideMessage
 *
 * SYNOPSIS:
 *    void DisplayBase::SetOverrideMessage(const char *Msg);
 *
 * PARAMETERS:
 *    Msg [I] -- The message to display or NULL to clear it.
 *
 * FUNCTION:
 *    This function locks the display with a message telling the user why the
 *    connection is locked out.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::SetOverrideMessage(const char *Msg)
{
    /* Do nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::ClearScreen
 *
 * SYNOPSIS:
 *    void DisplayBase::ClearScreen(e_ScreenClearType Type);
 *
 * PARAMETERS:
 *    Type [I] -- The type of clearing we want to do.  Supported types:
 *                  e_ScreenClear_Clear -- Normal clearing.  Throw away
 *                          anything that's on the screen area.
 *                  e_ScreenClear_Scroll -- Move any non blank lines to the
 *                          scroll back buffer before clearing.
 *                  e_ScreenClear_ScrollAll -- Move all the screen area lines
 *                          to the scroll back buffer.
 *                  e_ScreenClear_ScrollWithHR -- Move any non blank lines to
 *                          the scroll back buffer and then add a marker to
 *                          show that's where the new screen starts.
 *
 * FUNCTION:
 *    This function clears the screen area.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::ClearScreen(e_ScreenClearType Type)
{
    /* Do nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::ClearScrollBackBuffer
 *
 * SYNOPSIS:
 *    void DisplayBase::ClearScrollBackBuffer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clears the scroll back buffer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::ClearScrollBackBuffer(void)
{
    /* Do nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::InsertHorizontalRule
 *
 * SYNOPSIS:
 *    void DisplayBase::InsertHorizontalRule(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function does an insert of a horizontal rule on the current line
 *    with the cursor.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::InsertHorizontalRule(void)
{
    /* Do nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::ResetTerm
 *
 * SYNOPSIS:
 *    void DisplayBase::ResetTerm(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function does an reset term on the display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::ResetTerm(void)
{
    /* Do nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::SetFont
 *
 * SYNOPSIS:
 *    bool DisplayBase::SetFont(const std::string &NewFontName,int NewFontSize,
 *              bool NewFontBold,bool NewFontItalic);
 *
 * PARAMETERS:
 *    NewFontName [I] -- The font to use
 *    NewFontSize [I] -- The size to use
 *    NewFontBold [I] -- Use the bold font
 *    NewFontItalic [I] -- Use the italic font
 *
 * FUNCTION:
 *    This function sets what font to use on this display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::SetFont(const std::string &NewFontName,int NewFontSize,
        bool NewFontBold,bool NewFontItalic)
{
    FontName=NewFontName;
    FontSize=NewFontSize;
    FontBold=NewFontBold;
    FontItalic=NewFontItalic;

    SetupCanvas();
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::GetFont
 *
 * SYNOPSIS:
 *    void DisplayBase::GetFont(std::string &CurFontName,int &CurFontSize,
 *              bool &CurFontBold,bool &CurFontItalic);
 *
 * PARAMETERS:
 *    CurFontName [O] -- The font in use
 *    CurFontSize [O] -- The size in use
 *    CurFontBold [O] -- Are we using the bold version
 *    CurFontItalic [O] -- Are we using the italic version
 *
 * FUNCTION:
 *    This function sets what font to use on this display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::GetFont(std::string &CurFontName,int &CurFontSize,
        bool &CurFontBold,bool &CurFontItalic)
{
    CurFontName=FontName;
    CurFontSize=FontSize;
    CurFontBold=FontBold;
    CurFontItalic=FontItalic;
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::SetupCanvas
 *
 * SYNOPSIS:
 *    void DisplayBase::SetupCanvas(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function sets up the display canvas with the correct font and
 *    other settings.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::SetupCanvas(void)
{
    /* Does nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::SetupCanvas
 *
 * SYNOPSIS:
 *    void DisplayBase::SetupCanvas(uint16_t Mask);
 *
 * PARAMETERS:
 *    Mask [I] -- What mask to apply to drawing attributes.  If the bit is
 *                set then this is drawen.  These are the bits from
 *                UITC_SetDrawMask() UI control.
 *
 * FUNCTION:
 *    This function sets the draw mask for this display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::SetDrawMask(uint16_t Mask)
{
    /* Does nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::GetContextMenuHandle
 *
 * SYNOPSIS:
 *    t_UIContextMenuCtrl *DisplayBase::GetContextMenuHandle(
 *              e_UITD_ContextMenuType UIObj)
 *
 * PARAMETERS:
 *    UIObj [I] -- The context menu item to get the handle for.
 *
 * FUNCTION:
 *    This function gets a context menu item's handle.
 *
 * RETURNS:
 *    The context menu item's handle or NULL if it was not found.  If this
 *    is not supported (because there is no menu for example) then this will
 *    return NULL.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIContextMenuCtrl *DisplayBase::GetContextMenuHandle(e_UITD_ContextMenuType UIObj)
{
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::GetContextSubMenuHandle
 *
 * SYNOPSIS:
 *    t_UIContextMenuCtrl *DisplayBase::GetContextSubMenuHandle(
 *              e_UITD_ContextMenuType UIObj)
 *
 * PARAMETERS:
 *    UIObj [I] -- The context menu item to get the handle for.
 *
 * FUNCTION:
 *    This function gets a context menu item's handle.
 *
 * RETURNS:
 *    The context sub menu item's handle or NULL if it was not found.  If this
 *    is not supported (because there is no menu for example) then this will
 *    return NULL.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIContextSubMenuCtrl *DisplayBase::GetContextSubMenuHandle(e_UITD_ContextSubMenuType UIObj)
{
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::ClearArea
 *
 * SYNOPSIS:
 *    void DisplayBase::ClearArea(uint32_t X1,uint32_t Y1,uint32_t X2,
 *              uint32_t Y2);
 *
 * PARAMETERS:
 *    X1 [I] -- The left edge
 *    Y1 [I] -- The top edge
 *    X2 [I] -- The right edge
 *    Y2 [I] -- The bottom edge
 *
 * FUNCTION:
 *    This function clears part of the screen.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    ScrollArea()
 ******************************************************************************/
void DisplayBase::ClearArea(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2)
{
    /* Does nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::ScrollArea
 *
 * SYNOPSIS:
 *    void DisplayBase::ScrollArea(uint32_t X1,uint32_t Y1,uint32_t X2,
 *              uint32_t Y2,int32_t dx,int32_t dy);
 *
 * PARAMETERS:
 *    X1 [I] -- The left edge
 *    Y1 [I] -- The top edge
 *    X2 [I] -- The right edge
 *    Y2 [I] -- The bottom edge
 *    dx [I] -- The amount to scroll in the x dir
 *    dy [I] -- The amount to scroll in the y dir
 *
 * FUNCTION:
 *    This function scrolls an area on the screen.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    ClearArea()
 ******************************************************************************/
void DisplayBase::ScrollArea(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2,
        int32_t dx,int32_t dy)
{
    /* Does nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::ShowBell
 *
 * SYNOPSIS:
 *    void DisplayBase::ShowBell(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function tells the display to show the bell graphic / do a visible
 *    bell.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::ShowBell(void)
{
    /* Do nothing */
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::DoHexInputEvent
 *
 * SYNOPSIS:
 *    bool DisplayBase::DoHexInputEvent(const struct HDEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event from the hex input.
 *
 * FUNCTION:
 *    This function is called when there is an event in the hex input control.
 *
 * RETURNS:
 *    true -- Accept this event
 *    false -- Cancel this event
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayBase::DoHexInputEvent(const struct HDEvent *Event)
{
    t_UITextInputCtrl *Offset;
    char buff[100];
    switch(Event->EventType)
    {
        case e_HDEvent_SelectionEvent:
        case e_HDEvent_MouseMove:
        case e_HDEvent_BufferResize:
        break;
        case e_HDEvent_CursorMove:
            Offset=GetSendPanel_HexPosInput();
            if(Offset==NULL)
                return true;

            sprintf(buff,"%d",Event->Info.Cursor.Pos);
            UISetTextCtrlText(Offset,buff);
        break;
        case e_HDEvent_ContextMenu:
            switch(Event->Info.Context.Menu)
            {
                case e_UICTW_ContextMenu_Copy:
                    HexInput->SendSelection2Clipboard(e_Clipboard_Clipboard,
                            e_HDBCFormat_Default);
                break;
                case e_UICTW_ContextMenu_Paste:
                    HexInput->DoInsertFromClipboard(e_HDBCFormat_Default);
                break;
                case e_UICTW_ContextMenu_ClearScreen:
                    DoBlock_ClearHexInput();
                break;
                case e_UICTW_ContextMenu_Edit:
                    DoBlock_EditHex();
                break;
                case e_UICTW_ContextMenu_FindCRCAlgorithm:
                case e_UICTW_ContextMenu_EndianSwap:
                case e_UICTW_ContextMenu_ZoomIn:
                case e_UICTW_ContextMenu_ZoomOut:
                case e_UICTW_ContextMenuMAX:
                default:
                break;
            }
        break;
        case e_HDEventMAX:
        default:
        break;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::GetSendPanel_HexPosInput
 *
 * SYNOPSIS:
 *    t_UITextInputCtrl *DisplayBase::GetSendPanel_HexPosInput(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    You must override this function for use with the send panel below the
 *    display input.  It gets the handle the text input for cursor position.
 *
 * RETURNS:
 *    A handle to the hex cursor position text input in the send panel or NULL
 *    if it is not supported.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UITextInputCtrl *DisplayBase::GetSendPanel_HexPosInput(void)
{
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::GetSendPanel_HexRadioBttn
 *
 * SYNOPSIS:
 *    t_UIRadioBttnCtrl *DisplayBase::GetSendPanel_HexRadioBttn(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    You must override this function for use with the send panel below the
 *    display input.  It gets the handle the hex mode radio button.
 *
 * RETURNS:
 *    A handle to the widget or NULL if it is not supported.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIRadioBttnCtrl *DisplayBase::GetSendPanel_HexRadioBttn(void)
{
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::GetSendPanel_TextRadioBttn
 *
 * SYNOPSIS:
 *    t_UIRadioBttnCtrl *DisplayBase::GetSendPanel_TextRadioBttn(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    You must override this function for use with the send panel below the
 *    display input.  It gets the handle the text mode radio button.
 *
 * RETURNS:
 *    A handle to the widget or NULL if it is not supported.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIRadioBttnCtrl *DisplayBase::GetSendPanel_TextRadioBttn(void)
{
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::GetSendPanel_TextInput
 *
 * SYNOPSIS:
 *    t_UIMuliLineTextInputCtrl *DisplayBase::GetSendPanel_TextInput(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    You must override this function for use with the send panel below the
 *    display input.  It gets the handle the muliline text to send input.
 *
 * RETURNS:
 *    A handle to the widget or NULL if it is not supported.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIMuliLineTextInputCtrl *DisplayBase::GetSendPanel_TextInput(void)
{
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::GetSendPanel_LineEndInput
 *
 * SYNOPSIS:
 *    t_UIComboBoxCtrl *DisplayBase::GetSendPanel_LineEndInput(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    You must override this function for use with the send panel below the
 *    display input.  It gets the handle the line end input.
 *
 * RETURNS:
 *    A handle to the widget or NULL if it is not supported.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIComboBoxCtrl *DisplayBase::GetSendPanel_LineEndInput(void)
{
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::SetupHexInput
 *
 * SYNOPSIS:
 *    void DisplayBase::SetupHexInput(t_UIContainerFrameCtrl *ParentWid);
 *
 * PARAMETERS:
 *    ParentWid [I] -- The widget that this hex input will live
 *
 * FUNCTION:
 *    This function adds a hex input widget and sets it up.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    Throws text if there was an error.
 *
 * SEE ALSO:
 *    DisplayBase::FreeHexInput()
 ******************************************************************************/
void DisplayBase::SetupHexInput(t_UIContainerFrameCtrl *ParentWid)
{
    t_UIContextMenuCtrl *ContextMenu_ClearScreen;
    t_UIContextMenuCtrl *ContextMenu_EndianSwap;
    t_UIContextMenuCtrl *ContextMenu_Edit;

    /* Connect in the hex control */
    HexInput=new HexDisplayBuffer();
    if(!HexInput->Init(ParentWid,DisplayBase_HexInputEvent,(uintptr_t)this))
        throw("Failed to connect hex input to UI");

    HexInput->SetEditMode();
    HexInput->SetLossOfFocusBehavior(true);
    HexInput->SetFont(g_Settings.HexDisplaysFontName.c_str(),
            g_Settings.HexDisplaysFontSize,g_Settings.HexDisplaysFontBold,
            g_Settings.HexDisplaysFontItalic);
    HexInput->SetColors(g_Settings.HexDisplaysFGColor,
            g_Settings.HexDisplaysBGColor,g_Settings.HexDisplaysSelBGColor);

    ContextMenu_ClearScreen=HexInput->GetContextMenuHandle(e_UICTW_ContextMenu_ClearScreen);
    ContextMenu_EndianSwap=HexInput->GetContextMenuHandle(e_UICTW_ContextMenu_EndianSwap);
    ContextMenu_Edit=HexInput->GetContextMenuHandle(e_UICTW_ContextMenu_Edit);

    UISetContextMenuVisible(ContextMenu_ClearScreen,true);
    UISetContextMenuVisible(ContextMenu_EndianSwap,false);
    UISetContextMenuVisible(ContextMenu_Edit,true);

    HexInput->RebuildDisplay();
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::FreeHexInput
 *
 * SYNOPSIS:
 *    void DisplayBase::FreeHexInput(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the stuff that was allocated with SetupHexInput().
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DisplayBase::SetupHexInput()
 ******************************************************************************/
void DisplayBase::FreeHexInput(void)
{
    if(HexInput!=NULL)
        delete HexInput;
    HexInput=NULL;
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::DoBlock_ClearHexInput
 *
 * SYNOPSIS:
 *    void DisplayBase::DoBlock_ClearHexInput(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clears the hex input (after asking the user if they are
 *    sure)
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::DoBlock_ClearHexInput(void)
{
    if(UIAsk("Are you sure?","This will erase all the data.\n"
            "Are you sure?",e_AskBox_Question,
            e_AskBttns_YesNo)==e_AskRet_Yes)
    {
        HexInput->SetBufferSize(0);
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::DoBlock_Send
 *
 * SYNOPSIS:
 *    void DisplayBase::DoBlock_Send(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when the user presses the send button in the
 *    block area at the bottom.  It sends the currently entered data and the
 *    line ending.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::DoBlock_Send(void)
{
    union DBEventData Info;
    t_UIRadioBttnCtrl *HexBttn;
    t_UIMuliLineTextInputCtrl *Text2Send;
    t_UIComboBoxCtrl *LineEnd;
    string String2Send;
    string EndOfLine;
    const uint8_t *Buffer;
    int BufferSize;

    HexBttn=GetSendPanel_HexRadioBttn();
    Text2Send=GetSendPanel_TextInput();
    LineEnd=GetSendPanel_LineEndInput();

    if(HexBttn==NULL || Text2Send==NULL || LineEnd==NULL)
        return;

    if(UIIsRadioBttnSelected(HexBttn))
    {
        /* We are sending the hex buffer */
        if(HexInput->GetBufferInfo(&Buffer,&BufferSize))
        {
            Info.BlockSend.Buffer=Buffer;
            Info.BlockSend.Len=BufferSize;
            SendEvent(e_DBEvent_SendBlockData,&Info);
        }
    }
    else
    {
        /* We are sending the text */
        UIGetMuliLineTextCtrlText(Text2Send,String2Send);
        switch(UIGetComboBoxSelectedIndex(LineEnd))
        {
             case e_Block_LineEnd_CRLF:
                EndOfLine="\r\n";
            break;
            case e_Block_LineEnd_CR:
                EndOfLine="\r";
            break;
            case e_Block_LineEnd_LF:
                EndOfLine="\n";
            break;
            case e_Block_LineEnd_TAB:
                EndOfLine="\t";
            break;
            case e_Block_LineEnd_ESC:
                EndOfLine="\033";
            break;
            case e_Block_LineEnd_NULL:
                EndOfLine="";
                EndOfLine.append(1,0);
            break;
            case e_Block_LineEnd_None:
            case e_Block_LineEndMAX:
            default:
                EndOfLine="";
            break;
        }

        /* We also want to change all \n's to the end of line char */
        size_t pos = 0;
        while((pos = String2Send.find('\n', pos)) != std::string::npos)
        {
            String2Send.replace(pos,1,EndOfLine);
            pos+=EndOfLine.length();
        }
        String2Send+=EndOfLine;

        Info.BlockSend.Buffer=(uint8_t *)String2Send.c_str();
        Info.BlockSend.Len=String2Send.length();
        SendEvent(e_DBEvent_SendBlockData,&Info);
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::DoBlock_EditHex
 *
 * SYNOPSIS:
 *    void DisplayBase::DoBlock_EditHex(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when the user presses the edit button in the hex
 *    area of the block area at the bottom.  It opens the edit dialog.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::DoBlock_EditHex(void)
{
    const uint8_t *EditBuffer;
    uint8_t *EditBufferCopy;
    int BufferSize;

    EditBufferCopy=NULL;
    try
    {
        if(!HexInput->GetBufferInfo(&EditBuffer,&BufferSize))
            throw("Failed to get buffer");

        /* We need to copy the buffer because we don't want hex input freeing
           it on us */
        EditBufferCopy=(uint8_t *)malloc(BufferSize);
        if(EditBufferCopy==NULL)
            throw("Failed to copy the buffer for editing");
        memcpy(EditBufferCopy,EditBuffer,BufferSize);

        if(RunEditSendBufferDialog(0,&EditBufferCopy,&BufferSize))
        {
            HexInput->SetBuffer(EditBufferCopy,BufferSize);
            EditBufferCopy=NULL;
        }
    }
    catch(const char *Msg)
    {
        UIAsk("Error",Msg,e_AskBox_Error,e_AskBttns_Ok);
    }
    catch(...)
    {
    }

    if(EditBufferCopy!=NULL)
        free(EditBufferCopy);
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::DoBlock_RadioBttnChange
 *
 * SYNOPSIS:
 *    void DisplayBase::DoBlock_RadioBttnChange(void)
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when the user changes the mode of block area
 *    at the bottom.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::DoBlock_RadioBttnChange(void)
{
    t_UIRadioBttnCtrl *HexBttn;

    HexBttn=GetSendPanel_HexRadioBttn();

    if(HexBttn==NULL)
        return;

    if(UIIsRadioBttnSelected(HexBttn))
    {
        /* We are sending the hex buffer */
        Block_SetHexOrTextMode(false);
    }
    else
    {
        Block_SetHexOrTextMode(true);
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::Block_SetHexOrTextMode
 *
 * SYNOPSIS:
 *    void DisplayBase::Block_SetHexOrTextMode(bool TextMode);
 *
 * PARAMETERS:
 *    TextMode [I] -- Set things for text mode (true), or hex mode (false)
 *
 * FUNCTION:
 *    This function changes the send block panel to text or hex mode.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::Block_SetHexOrTextMode(bool TextMode)
{
    t_UIRadioBttnCtrl *HexBttn;
    t_UIRadioBttnCtrl *TxtBttn;
    t_UIComboBoxCtrl *LineEndInput;

    HexBttn=GetSendPanel_HexRadioBttn();
    TxtBttn=GetSendPanel_TextRadioBttn();
    LineEndInput=GetSendPanel_LineEndInput();

    if(HexBttn==NULL || TxtBttn==NULL || LineEndInput==NULL)
        return;

    if(TextMode)
    {
        UIUnselectRadioBttn(HexBttn);
        UISelectRadioBttn(TxtBttn);
        UIEnableComboBox(LineEndInput,true);
    }
    else
    {
        UISelectRadioBttn(HexBttn);
        UIUnselectRadioBttn(TxtBttn);
        UIEnableComboBox(LineEndInput,false);
    }

    SendPanel_ShowHexOrText(TextMode);
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::SendPanel_ShowHexOrText
 *
 * SYNOPSIS:
 *    void DisplayBase::SendPanel_ShowHexOrText(bool Text);
 *
 * PARAMETERS:
 *    Text [I] -- Show the text input (true) or the hex input (false).
 *
 * FUNCTION:
 *    This function tells the GUI to show the text input or the hex input in
 *    the send panel.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::SendPanel_ShowHexOrText(bool Text)
{
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::ToggleAttribs2Selection
 *
 * SYNOPSIS:
 *    void DisplayBase::ToggleAttribs2Selection(uint32_t Attribs);
 *
 * PARAMETERS:
 *    Attribs [I] -- The TXT_ATTRIB_ attribs to toggle.
 *
 * FUNCTION:
 *    This function takes the current selection and check if any of the
 *    selected bytes has these attribs turned on.  If they do it removes that
 *    attrib, if not it turns the attribs on.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::ToggleAttribs2Selection(uint32_t Attribs)
{
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::ApplyBGColor2Selection
 *
 * SYNOPSIS:
 *    void DisplayBase::ApplyBGColor2Selection(uint32_t RGB);
 *
 * PARAMETERS:
 *    RGB [I] -- The color to apply
 *
 * FUNCTION:
 *    This function takes the current selection and changes the background
 *    color for all the text selected.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::ApplyBGColor2Selection(uint32_t RGB)
{
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::IsAttribSetInSelection
 *
 * SYNOPSIS:
 *    bool DisplayBase::IsAttribSetInSelection(uint32_t Attribs);
 *
 * PARAMETERS:
 *    Attribs [I] -- The attrib to check
 *
 * FUNCTION:
 *    This function checks the bytes in the selection and return true if
 *    any of them have this attrib set.
 *
 * RETURNS:
 *    true -- At least 1 byte of the selection has this attrib
 *    false -- There is no selection or the selection does not have this
 *             attrib in it.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayBase::IsAttribSetInSelection(uint32_t Attribs)
{
    return false;
}

uint8_t *DisplayBase::GetSelectionRAW(unsigned int *Bytes)
{
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::AllocateMark
 *
 * SYNOPSIS:
 *    t_DataProMark *DisplayBase::AllocateMark(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates a mark for this display.  A mark is a point in
 *    the display that we are currently adding chars to.  This mark is
 *    used to reference this insert point again in the future.  The mark
 *    can become invalid if the insert point goes away.  The mark remains
 *    allocated and can be used but will not do anything.
 *
 *    The display keeps a list of all the marks that have been allocated.
 *    This is so it can go through the list and invalid any that become
 *    invalid because of a change to the display.
 *
 * RETURNS:
 *    A pointer to the mark.
 *
 * SEE ALSO:
 *    DPS_AllocateMark()
 ******************************************************************************/
t_DataProMark *DisplayBase::AllocateMark(void)
{
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::FreeMark
 *
 * SYNOPSIS:
 *    void DisplayBase::FreeMark(t_DataProMark *Mark);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to free
 *
 * FUNCTION:
 *    This function frees a mark that was allocated with AllocateMark()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBase::FreeMark(t_DataProMark *Mark)
{
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::IsMarkValid
 *
 * SYNOPSIS:
 *    bool DisplayBase::IsMarkValid(t_DataProMark *Mark);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *
 * FUNCTION:
 *    This function checks if a marker is valid or not.
 *
 * RETURNS:
 *    true -- Mark is still valid
 *    false -- Mark is invalid
 *
 * SEE ALSO:
 *    DPS_IsMarkValid()
 ******************************************************************************/
bool DisplayBase::IsMarkValid(t_DataProMark *Mark)
{
    return false;
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::SetMark2CursorPos
 *
 * SYNOPSIS:
 *    void DisplayBase::SetMark2CursorPos(t_DataProMark *Mark);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *
 * FUNCTION:
 *    This function will take a mark and move it to the current cursor position.
 *    It will also set this mark to valid.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_SetMark2CursorPos()
 ******************************************************************************/
void DisplayBase::SetMark2CursorPos(t_DataProMark *Mark)
{
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::ApplyAttrib2Mark
 *
 * SYNOPSIS:
 *    void DisplayBase::ApplyAttrib2Mark(t_DataProMark *Mark,uint32_t Attrib,
 *              uint32_t Offset,uint32_t Len);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *    Attrib [I] -- The new attrib(s) to set
 *    Offset [I] -- The number of chars from the mark to skip before starting
 *                  to apply the attribs.
 *    Len [I] -- The number of chars to apply these new attributes to.
 *
 * FUNCTION:
 *    This function does the DPS_ApplyAttrib2Mark() function to the a display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_ApplyAttrib2Mark()
 ******************************************************************************/
void DisplayBase::ApplyAttrib2Mark(t_DataProMark *Mark,uint32_t Attrib,
        uint32_t Offset,uint32_t Len)
{
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::RemoveAttribFromMark
 *
 * SYNOPSIS:
 *    void DisplayBase::RemoveAttribFromMark(t_DataProMark *Mark,uint32_t Attrib,
 *          uint32_t Offset,uint32_t Len);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *    Attrib [I] -- The new attrib(s) to clear
 *    Offset [I] -- The number of chars from the mark to skip before starting
 *                  to remove the attribs.
 *    Len [I] -- The number of chars to remove these new attributes from.
 *
 * FUNCTION:
 *    This function does the DPS_RemoveAttribFromMark() function to the active
 *    connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_RemoveAttribFromMark()
 ******************************************************************************/
void DisplayBase::RemoveAttribFromMark(t_DataProMark *Mark,uint32_t Attrib,
        uint32_t Offset,uint32_t Len)
{
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::ApplyFGColor2Mark
 *
 * SYNOPSIS:
 *    void DPS_ApplyFGColor2Mark(t_DataProMark *Mark,uint32_t FGColor,
 *              uint32_t Offset,uint32_t Len);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *    FGColor [I] -- The colors to apply
 *    Offset [I] -- The number of chars from the mark to skip before starting
 *                  to apply the color.
 *    Len [I] -- The number of chars to apply this color to
 *
 * FUNCTION:
 *    This function does the DPS_ApplyFGColor2Mark() function to the display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_ApplyFGColor2Mark()
 ******************************************************************************/
void DisplayBase::ApplyFGColor2Mark(t_DataProMark *Mark,uint32_t FGColor,
        uint32_t Offset,uint32_t Len)
{
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::ApplyBGColor2Mark
 *
 * SYNOPSIS:
 *    void DisplayBase::ApplyBGColor2Mark(t_DataProMark *Mark,uint32_t BGColor,
 *              uint32_t Offset,uint32_t Len);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *    BGColor [I] -- The colors to apply
 *    Offset [I] -- The number of chars from the mark to skip before starting
 *                  to apply the color.
 *    Len [I] -- The number of chars to apply this color to
 *
 * FUNCTION:
 *    This function does the DPS_ApplyBGColor2Mark() function to the display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_ApplyBGColor2Mark()
 ******************************************************************************/
void DisplayBase::ApplyBGColor2Mark(t_DataProMark *Mark,uint32_t BGColor,
        uint32_t Offset,uint32_t Len)
{
}

/*******************************************************************************
 * NAME:
 *    DisplayBase::MoveMark
 *
 * SYNOPSIS:
 *    void DisplayBase::MoveMark(t_DataProMark *Mark,int Amount);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *    Amount [I] -- How much to move the mark by (plus for toward the cursor,
 *                  neg to move toward the start of the buffer).
 *
 * FUNCTION:
 *    This function does the DPS_MoveMark() function to the display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_MoveMark()
 ******************************************************************************/
void DisplayBase::MoveMark(t_DataProMark *Mark,int Amount)
{
}
