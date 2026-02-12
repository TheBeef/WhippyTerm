/*******************************************************************************
 * FILENAME: DisplayText.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the display for text in it.  This handles the back buffer,
 *    scroll bars, word wrap, etc in it.
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
 *    Paul Hutchinson (06 Aug 2023)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Settings.h"
#include "App/Util/TextStyleHelpers.h"
#include "DisplayText.h"
#include "UI/UIDebug.h"
#include "UI/UISystem.h"
#include "UI/UITimers.h"
#include "ThirdParty/utf8.h"
#include <string.h>
#include <string>
#include <limits.h>
#include <stdio.h>

using namespace std;

/*** DEFINES                  ***/
//#define DEBUG_SHOW_BUFFER_POS               1       // Show the pointers to parts of the screen (only some)

#define SELECTION_SCROLL_SPEED_TIMER            50 // ms

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
bool DisplayText_EventHandlerCB(const struct TextDisplayEvent *Event);
void DisplayText_ScrollTimer_Timeout(uintptr_t UserData);

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    DisplayText_EventHandlerCB
 *
 * SYNOPSIS:
 *    static bool DisplayText_EventHandlerCB(
 *              const struct TextDisplayEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event from the TextDisplay widget.
 *
 * FUNCTION:
 *    This function takes an event from the TextDisplay widget and figures
 *    out the DisplayText class it belongs with and then calls the class
 *    handler method.
 *
 * RETURNS:
 *    true -- Let the event continue
 *    false -- Cancel the event
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayText_EventHandlerCB(const struct TextDisplayEvent *Event)
{
    class DisplayText *DT=(class DisplayText *)Event->ID;

    return DT->DoTextDisplayCtrlEvent(Event);
}

/*******************************************************************************
 * NAME:
 *    DisplayText_ScrollTimer_Timeout
 *
 * SYNOPSIS:
 *    void DisplayText_ScrollTimer_Timeout(uintptr_t UserData);
 *
 * PARAMETERS:
 *    UserData [I] -- A pointer to our display text class.
 *
 * FUNCTION:
 *    This is a callback from the scroll timer.  It just calls the class
 *    DoScrollTimerTimeout() function.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText_ScrollTimer_Timeout(uintptr_t UserData)
{
    class DisplayText *DT=(class DisplayText *)UserData;

    DT->DoScrollTimerTimeout();
}

DisplayText::DisplayText()
{
    TextDisplayCtrl=NULL;
    MarkerList=NULL;

    InitCalled=false;

    CursorX=0;
    CursorY=0;
    MaxCursorX=0;
    MaxCursorY=0;
    LastCursorY=0;

    TextAreaWidthPx=0;
    TextAreaHeightPx=0;
    ScreenWidthChars=0;
    ScreenHeightChars=0;
    WindowHeightChars=0;
    WindowXOffsetPx=0;
    LongestLinePx=0;
    CharWidthPx=1;
    CharHeightPx=1;

    ShowNonPrintables=false;
    ShowEndOfLines=false;

    LastCursorVisible=true;
    LastCursorStyle=e_TextCursorStyleMAX;
    CursorStyle=e_TextCursorStyle_Block;

    /* Selection */
    SelectionActive=false;
    Selection_X=0;
    Selection_Y=0;
    Selection_AnchorX=0;
    Selection_AnchorY=0;
    SelectMode=e_DTSelectModeMAX;

    ScrollTimer=NULL;

    LastSeenLF=false;
    LastSeenCR=false;
}

DisplayText::~DisplayText()
{
    struct TextPointMarker *Marker;

    FreeHexInput();

    if(ScrollTimer!=NULL)
        FreeUITimer(ScrollTimer);

    /* Free the marker list */
    while(MarkerList!=NULL)
    {
        Marker=MarkerList->Next;
        delete MarkerList;
        MarkerList=Marker;
    }

    if(TextDisplayCtrl!=NULL)
    {
        UITC_FreeTextDisplay(TextDisplayCtrl);
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::Init
 *
 * SYNOPSIS:
 *    bool DisplayText::Init(void *ParentWidget,class ConSettings *SettingsPtr,
 *          bool (*EventCallback)(const struct DBEvent *Event),
 *          uintptr_t UserData);
 *
 * PARAMETERS:
 *    ParentWidget [I] -- The parent UI widget that we will add our widgets to
 *    SettingsPtr [I] -- The setting to use for this display.  NULL for the
 *                       global settings.
 *    EventCallback [I] -- See DisplayBase::InitBase()
 *    UserData [I] -- The user data to send to the 'EventCallback' callback.
 *
 * FUNCTION:
 *    This function init's the display text.
 *
 * RETURNS:
 *    true -- Things worked
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayText::Init(void *ParentWidget,class ConSettings *SettingsPtr,
        bool (*EventCallback)(const struct DBEvent *Event),uintptr_t UserData)
{
    TextDisplayCtrl=NULL;
    try
    {
        struct TextLine FirstLine;

        if(!InitBase(EventCallback,UserData))
            throw(0);

        Settings=SettingsPtr;
        if(Settings==NULL)
            Settings=&g_Settings.DefaultConSettings;

        /* Allocate the text canvas */
        TextDisplayCtrl=UITC_AllocTextDisplay(ParentWidget,
                DisplayText_EventHandlerCB,(uintptr_t)this);
        if(TextDisplayCtrl==NULL)
            throw(0);

        SetupHexInput(UITC_GetSendHexDisplayContainerFrameCtrlHandle(TextDisplayCtrl));

        /* We show hex input because we are text */
        Block_SetHexOrTextMode(true);

        /* Current Style */
        CurrentStyle.FGColor=Settings->DefaultColors[e_DefaultColors_FG];
        CurrentStyle.BGColor=Settings->DefaultColors[e_DefaultColors_BG];
        CurrentStyle.Attribs=0;
        CurrentStyle.ULineColor=CurrentStyle.FGColor;

        FontName=Settings->FontName;
        FontSize=Settings->FontSize;
        FontBold=Settings->FontBold;
        FontItalic=Settings->FontItalic;

        Lines.clear();
        LinesCount=0;
        FirstLine.LineWidthPx=0;
        FirstLine.LineBackgroundColor=
                Settings->DefaultColors[e_DefaultColors_BG];
        FirstLine.EOL=e_DTEOL_Hard;
        Lines.push_back(FirstLine);
        LinesCount++;
        TopLine=Lines.begin();
        ScreenFirstLine=TopLine;
        TopLineY=0;
        ActiveLine=&*ScreenFirstLine;
        ActiveLineY=0;
        InsertFrag=ActiveLine->Frags.end();
        InsertPos=-1;

        SetupCanvas();

        ScrollTimer=AllocUITimer();
        if(ScrollTimer==NULL)
            throw(0);

        SetupUITimer(ScrollTimer,DisplayText_ScrollTimer_Timeout,
                (uintptr_t)this,true);

        UITimerSetTimeout(ScrollTimer,SELECTION_SCROLL_SPEED_TIMER);

        ApplySettings();

        InitCalled=true;
    }
    catch(...)
    {
        if(TextDisplayCtrl!=NULL)
        {
            UITC_FreeTextDisplay(TextDisplayCtrl);
            TextDisplayCtrl=NULL;
        }
        return false;
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::Reparent
 *
 * SYNOPSIS:
 *    void DisplayText::Reparent(void *NewParentWidget);
 *
 * PARAMETERS:
 *    NewParentWidget [I] -- The new parent
 *
 * FUNCTION:
 *    This function changes the widget that is the parent of the UI elements
 *    for this display text.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::Reparent(void *NewParentWidget)
{
    UITC_Reparent(TextDisplayCtrl,NewParentWidget);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::DoTextDisplayCtrlEvent
 *
 * SYNOPSIS:
 *    bool DisplayText::DoTextDisplayCtrlEvent(
 *              const struct TextDisplayEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event from the text display control.
 *
 * FUNCTION:
 *    This function is called when an event from our text display control
 *    sends an event.
 *
 * RETURNS:
 *    true -- Let the event continue
 *    false -- Cancel the event
 *
 * NOTES:
 *    Called from DisplayText_EventHandlerCB()
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayText::DoTextDisplayCtrlEvent(const struct TextDisplayEvent *Event)
{
    union DBEventData Info;
    int Delta;
    int r;

    if(!InitCalled)
        return false;

    switch(Event->EventType)
    {
        case e_TextDisplayEvent_DisplayFrameScrollX:
            WindowXOffsetPx=Event->Info.Scroll.Amount;

            if(TextDisplayCtrl!=NULL)
            {
                UITC_SetXOffset(UITC_GetTextDisplayPrimaryColumn(
                        TextDisplayCtrl),WindowXOffsetPx);
            }

            RedrawFullScreen();
        break;
        case e_TextDisplayEvent_DisplayFrameScrollY:
            /* Ok, we need to move TopLine to the new pos (we do this with
               a delta to speed things up */
            Delta=Event->Info.Scroll.Amount-TopLineY;

            if(Delta>0)
            {
                for(r=0;r<Delta;r++)
                    TopLine++;
            }
            else if(Delta<0)
            {
                for(r=0;r<-Delta;r++)
                    TopLine--;
            }

            TopLineY=Event->Info.Scroll.Amount;

            if(Delta!=0)
            {
                UITC_SetCursorPos(TextDisplayCtrl,CursorX,
                        CalcCorrectedCursorPos());
                RethinkCursorHidden();
                RedrawFullScreen();
            }
break;
        case e_TextDisplayEvent_MouseDown:
            Info.Mouse.x=Event->Info.Mouse.x;
            Info.Mouse.y=Event->Info.Mouse.y;
            SendEvent(e_DBEvent_LeftMousePress,&Info);

            HandleLeftMousePress(true,Event->Info.Mouse.x,
                    Event->Info.Mouse.y);
        break;
        case e_TextDisplayEvent_MouseUp:
            HandleLeftMousePress(false,Event->Info.Mouse.x,
                    Event->Info.Mouse.y);
        break;
        case e_TextDisplayEvent_MouseRightDown:
        break;
        case e_TextDisplayEvent_MouseRightUp:
        break;
        case e_TextDisplayEvent_MouseMiddleDown:
            Info.Mouse.x=Event->Info.Mouse.x;
            Info.Mouse.y=Event->Info.Mouse.y;
            SendEvent(e_DBEvent_MouseMiddlePress,&Info);
        break;
        case e_TextDisplayEvent_MouseMiddleUp:
        break;
        case e_TextDisplayEvent_MouseDoubleClick:
            HandleLeftMouseDoublePress(Event->Info.Mouse.x,Event->Info.Mouse.y);
        break;
        case e_TextDisplayEvent_MouseTripleClick:
            HandleLeftMouseTriplePress(Event->Info.Mouse.x,Event->Info.Mouse.y);
        break;
        case e_TextDisplayEvent_MouseWheel:
            if(TextDisplayCtrl==NULL)
                return false;

            if(Event->Info.MouseWheel.Mods)
            {
                Info.MouseWheel.Mods=Event->Info.MouseWheel.Mods;
                Info.MouseWheel.Steps=Event->Info.MouseWheel.Steps;
                SendEvent(e_DBEvent_MouseMouseWheel,&Info);
            }
            else
            {
                Delta=-Event->Info.MouseWheel.Steps;

                if(Delta!=0 && TopLineY+Delta>=0 &&
                            TopLineY+Delta<=LinesCount-ScreenHeightChars)
                {
                    if(Delta>0)
                    {
                        for(r=0;r<Delta;r++)
                            TopLine++;
                    }
                    else if(Delta<0)
                    {
                        for(r=0;r<-Delta;r++)
                            TopLine--;
                    }

                    TopLineY=TopLineY+Delta;

                    UITC_SetCursorPos(TextDisplayCtrl,CursorX,
                            CalcCorrectedCursorPos());
                    RethinkCursorHidden();
                    RedrawFullScreen();
                }
            }
        break;
        case e_TextDisplayEvent_MouseMove:
            HandleMouseMove(Event->Info.Mouse.x,Event->Info.Mouse.y);
        break;
        case e_TextDisplayEvent_Resize:
            RethinkTextAreaSize();
            RethinkWindowSize();
            RethinkScrollBars();
            RedrawFullScreen();
        break;
        case e_TextDisplayEvent_LostFocus:
            HasFocus=false;
            Info.Focus.HasFocus=HasFocus;
            SendEvent(e_DBEvent_FocusChange,&Info);
        break;
        case e_TextDisplayEvent_GotFocus:
            HasFocus=true;
            Info.Focus.HasFocus=HasFocus;
            SendEvent(e_DBEvent_FocusChange,&Info);
        break;
        case e_TextDisplayEvent_KeyEvent:
            Info.Key.Mods=Event->Info.Key.Mods;
            Info.Key.Key=Event->Info.Key.Key;
            Info.Key.TextPtr=Event->Info.Key.TextPtr;
            Info.Key.TextLen=Event->Info.Key.TextLen;
            SendEvent(e_DBEvent_KeyEvent,&Info);
        break;
        case e_TextDisplayEvent_ContextMenu:
            Info.Context.Menu=(e_UITD_ContextMenuType)Event->Info.Context.Menu;
            SendEvent(e_DBEvent_ContextMenu,&Info);
        break;
        case e_TextDisplayEvent_ButtonPress:
            switch(Event->Info.ButtonPress.Bttn)
            {
                case e_UITC_Bttn_Send:
                    DoBlock_Send();
                break;
                case e_UITC_Bttn_HexEdit:
                    DoBlock_EditHex();
                break;
                case e_UITC_Bttn_Clear:
                    DoBlock_ClearHexInput();
                break;
                case e_UITC_BttnMAX:
                default:
                break;
            }
        break;
        case e_TextDisplayEvent_RadioButtonPress:
            switch(Event->Info.RadioButton.BttnID)
            {
                case e_UITC_RadioButton_Text:
                case e_UITC_RadioButton_Hex:
                    DoBlock_RadioBttnChange();
                break;
                case e_UITC_RadioButtonMAX:
                default:
                break;
            }
        break;
        case e_TextDisplayEvent_HeadersRearranged:
        break;
        case e_TextDisplayEvent_ComboxChange:
        case e_TextDisplayEventMAX:
        default:
            return true;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::HandleLeftMousePress
 *
 * SYNOPSIS:
 *    void DisplayText::HandleLeftMousePress(bool Down,int x,int y);
 *
 * PARAMETERS:
 *    Down [I] -- The mouse button was pressed (true) or released (false).
 *    x [I] -- The x pos of the mouse
 *    y [I] -- The y pos of the mouse
 *
 * FUNCTION:
 *    This function is called when the left mouse button is pressed or released.
 *
 *    It used to track if we are moving the selection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::HandleLeftMousePress(bool Down,int x,int y)
{
    int XChars;
    int YChars;
    bool NeedRedraw;

    LeftMouseDown=Down;
    if(Down)
    {
        /* Clear the selection, but note where the user clicked */
        NeedRedraw=SelectionActive;
        SelectMode=e_DTSelectMode_Letter;
//SelectMode=e_DTSelectMode_Word;

        SelectionActive=false;

        if(x<CharWidthPx)
        {
            /* We select from the edge instead of the middle when we are on
               the left side */
            ConvertScreenXY2Chars(x,y,&XChars,&YChars);
        }
        else
        {
            /* We add 1/2 a char so we select the whole char when we are half way
               though instead of having to be all the way through */
            ConvertScreenXY2Chars(x+CharWidthPx/2,y,&XChars,&YChars);
        }
        Selection_X=XChars;
        Selection_AnchorX=XChars;
        Selection_Y=YChars;
        Selection_AnchorY=YChars;

///* DEBUG PAUL: This is for word select (double click) */
//SelectionActive=true;
//Selection_StartX1=Selection_X;
//Selection_StartY1=Selection_Y;
//FindWordStartEndPoints(Selection_StartX1,Selection_StartY1,Selection_StartX2,Selection_StartY2);
//Selection_X=Selection_StartX1;
//Selection_AnchorX=Selection_StartX2;
//Selection_Y=Selection_StartY1;
//Selection_AnchorY=Selection_StartY2;
//NeedRedraw=true;

        if(NeedRedraw)
        {
            RedrawFullScreen();
            SendEvent(e_DBEvent_SelectionChanged,NULL);
        }
    }
    else
    {
        /* If the mouse is coming up, cancel any selection scroll timers */
        if(UITimerRunning(ScrollTimer))
            UITimerStop(ScrollTimer);
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::HandleLeftMouseDoublePress
 *
 * SYNOPSIS:
 *    void DisplayText::HandleLeftMouseDoublePress(int x,int y);
 *
 * PARAMETERS:
 *    x [I] -- The x pos of the mouse
 *    y [I] -- The y pos of the mouse
 *
 * FUNCTION:
 *    This function is called when the left mouse button is double clicked.
 *
 *    It used to handle the selection of words.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::HandleLeftMouseDoublePress(int x,int y)
{
    int XChars;
    int YChars;

    LeftMouseDown=true;

    SelectMode=e_DTSelectMode_Word;

    if(x<CharWidthPx)
    {
        /* We select from the edge instead of the middle when we are on
           the left side */
        ConvertScreenXY2Chars(x,y,&XChars,&YChars);
    }
    else
    {
        /* We add 1/2 a char so we select the whole char when we are half way
           though instead of having to be all the way through */
        ConvertScreenXY2Chars(x+CharWidthPx/2,y,&XChars,&YChars);
    }
    Selection_X=XChars;
    Selection_AnchorX=XChars;
    Selection_Y=YChars;
    Selection_AnchorY=YChars;

    Selection_StartX1=Selection_X;
    Selection_StartY1=Selection_Y;
    FindWordStartEndPoints(Selection_StartX1,Selection_StartY1,
            Selection_StartX2,Selection_StartY2);

    Selection_X=Selection_StartX1;
    Selection_AnchorX=Selection_StartX2;
    Selection_Y=Selection_StartY1;
    Selection_AnchorY=Selection_StartY2;

    if(Selection_X==Selection_AnchorX && Selection_Y==Selection_AnchorY)
        SelectionActive=false;
    else
        SelectionActive=true;

    RedrawFullScreen();
    SendEvent(e_DBEvent_SelectionChanged,NULL);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::HandleLeftMouseTriplePress
 *
 * SYNOPSIS:
 *    void DisplayText::HandleLeftMouseTriplePress(int x,int y);
 *
 * PARAMETERS:
 *    x [I] -- The x pos of the mouse
 *    y [I] -- The y pos of the mouse
 *
 * FUNCTION:
 *    This function is called when the left mouse button is triple clicked.
 *
 *    It used to handle the selection of lines.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::HandleLeftMouseTriplePress(int x,int y)
{
    int XChars;
    int YChars;

    LeftMouseDown=true;

    SelectMode=e_DTSelectMode_Line;

    ConvertScreenXY2Chars(x,y,&XChars,&YChars);

    Selection_StartX1=XChars;
    Selection_StartY1=YChars;

    Selection_X=0;
    Selection_AnchorX=0;
    Selection_Y=YChars;
    Selection_AnchorY=YChars+1;

    SelectionActive=true;

    RedrawFullScreen();
    SendEvent(e_DBEvent_SelectionChanged,NULL);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::HandleMouseMove
 *
 * SYNOPSIS:
 *    void DisplayText::HandleMouseMove(int x,int y);
 *
 * PARAMETERS:
 *    x [I] -- The x pos of the mouse
 *    y [I] -- The y pos of the mouse
 *
 * FUNCTION:
 *    This function is called to inform us that the mouse has moved.  The
 *    event may only come if the mouse button is down.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::HandleMouseMove(int x,int y)
{
    int XChars;
    int YChars;
    int JunkX;
    int JunkY;
    int WordCmp;

    if(LeftMouseDown)
    {
        /* Clear the selection, but note where the user clicked */
        SelectionActive=true;

        AutoSelectionScrolldx=0;
        if(x<0)
            AutoSelectionScrolldx=-1;
        else if(x>TextAreaWidthPx)
            AutoSelectionScrolldx=1;

        AutoSelectionScrolldy=0;
        if(y<0)
            AutoSelectionScrolldy=-1;
        else if(y>TextAreaHeightPx)
            AutoSelectionScrolldy=1;

        if(AutoSelectionScrolldx!=0 || AutoSelectionScrolldy!=0)
        {
            /* Ok, we are scrolling the display */
            ScrollScreen(AutoSelectionScrolldx*CharWidthPx,
                    AutoSelectionScrolldy);

            /* Start the auto scroll timer */
            UITimerStart(ScrollTimer);
        }
        else
        {
            /* We don't need the timer */
            if(UITimerRunning(ScrollTimer))
                UITimerStop(ScrollTimer);
        }

        /* We add 1/2 a char so we select the whole char when we are half way
           though instead of having to be all the way through */
        ConvertScreenXY2Chars(x+CharWidthPx/2,y,&XChars,&YChars);

        switch(SelectMode)
        {
            case e_DTSelectMode_Letter:
                Selection_X=XChars;
                Selection_Y=YChars;
            break;
            case e_DTSelectMode_Word:
                /* Move to the start of the word */

                /* See if we are before or after the starting word */
                WordCmp=CmpXYPositions(XChars,YChars,Selection_StartX1,
                        Selection_StartY1);

                if(WordCmp<=0)
                {
                    /* Ok, we are selecting before the starting word start */
                    Selection_X=XChars;
                    Selection_AnchorX=Selection_StartX2;
                    Selection_Y=YChars;
                    Selection_AnchorY=Selection_StartY2;

                    FindWordStartEndPoints(Selection_X,Selection_Y,
                            JunkX,JunkY);
                }
                else
                {
                    /* We are selecting after the starting word */
                    Selection_X=Selection_StartX1;
                    Selection_Y=Selection_StartY1;

                    JunkX=XChars;
                    JunkY=YChars;
                    FindWordStartEndPoints(JunkX,JunkY,Selection_AnchorX,
                            Selection_AnchorY);
                }
            break;
            case e_DTSelectMode_Line:
                /* See if we are before or after the starting word */
                WordCmp=CmpXYPositions(XChars,YChars,Selection_StartX1,
                        Selection_StartY1);

                if(WordCmp<=0)
                {
                    /* Ok, we are selecting before the starting line */
                    Selection_X=0;
                    Selection_Y=YChars;
                    Selection_AnchorX=0;
                    Selection_AnchorY=Selection_StartY1+1;

                    FindWordStartEndPoints(Selection_X,Selection_Y,
                            JunkX,JunkY);
                }
                else
                {
                    /* We are selecting after the starting line */
                    Selection_X=0;
                    Selection_Y=Selection_StartY1;

                    Selection_AnchorX=0;
                    Selection_AnchorY=YChars+1;
                }
            break;
            case e_DTSelectModeMAX:
            break;
        }


        RedrawFullScreen();
        SendEvent(e_DBEvent_SelectionChanged,NULL);
    }
    else
    {
        /* If we have a scroll timer running kill it */
        if(UITimerRunning(ScrollTimer))
            UITimerStop(ScrollTimer);
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::SetBlockDeviceMode
 *
 * SYNOPSIS:
 *    void DisplayText::SetBlockDeviceMode(bool On);
 *
 * PARAMETERS:
 *    On [I] -- This device is a block device (true), or a steam device (false)
 *
 * FUNCTION:
 *    This function changes if the device this display is connected to is a
 *    block or steam device.  It will show / hide the send block panel.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::SetBlockDeviceMode(bool On)
{
    if(TextDisplayCtrl==NULL)
        return;

    UITC_ShowSendPanel(TextDisplayCtrl,On);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::SetupCanvas
 *
 * SYNOPSIS:
 *    void DisplayText::SetupCanvas(void);
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
 *    DisplayBuffer::ChangeCanvasID()
 ******************************************************************************/
void DisplayText::SetupCanvas(void)
{
//    UITC_SetFont(TextDisplayCtrl,FontName.c_str(),FontSize,FontBold,FontItalic);
    UITC_SetFont(UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),FontName.c_str(),FontSize,FontBold,FontItalic);

    UITC_SetCursorColor(TextDisplayCtrl,Settings->CursorColor);

    UITC_SetTextAreaBackgroundColor(UITC_GetTextDisplayPrimaryColumn(
            TextDisplayCtrl),Settings->DefaultColors[e_DefaultColors_BG]);
    UITC_SetTextDefaultColor(UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
            Settings->DefaultColors[e_DefaultColors_FG]);

    CharWidthPx=UITC_GetCharPxWidth(UITC_GetTextDisplayPrimaryColumn(
            TextDisplayCtrl));
    CharHeightPx=UITC_GetCharPxHeight(UITC_GetTextDisplayPrimaryColumn(
            TextDisplayCtrl));
    if(CharWidthPx<1)
        CharWidthPx=1;
    if(CharHeightPx<1)
        CharHeightPx=1;

    RethinkTextAreaSize();
    RethinkWindowSize();
    RethinkLineLengths();
    RethinkScrollBars();
}

/*******************************************************************************
 * NAME:
 *    DisplayText::RedrawActiveLine
 *
 * SYNOPSIS:
 *    void DisplayText::RedrawActiveLine(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function redraws the active line (the line with the cursor on it).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::RedrawActiveLine(void)
{
    int LineLenPx;
    bool LookupLongest;

    if(ActiveLine==NULL || TextDisplayCtrl==NULL)
        return;

    if(!CursorLineVisible())
        return;

    LineLenPx=DrawLine(ActiveLineY,CalcCorrectedCursorPos(),ActiveLine);

    /* Redo the line len for this line if it changed.  We do this because here
       instead of a function that just does this because we have the info
       already ready and we save having to recal the value. */
    if(ActiveLine->LineWidthPx!=LineLenPx)
    {
        /* If were the longest line and we got longer then just set us as
           the longest line.  Otherwize we need to refind the longest line */
        LookupLongest=false;
        if(LongestLinePx==ActiveLine->LineWidthPx && LineLenPx>=LongestLinePx)
            LongestLinePx=LineLenPx;
        else
            LookupLongest=true;

        ActiveLine->LineWidthPx=LineLenPx;

        if(LookupLongest)
            ReFindLongestLineLength();

        RethinkScrollBars();
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::DrawLine
 *
 * SYNOPSIS:
 *    int DisplayText::DrawLine(int LineY,int ScreenLine,
 *              struct TextLine *Line);
 *
 * PARAMETERS:
 *    LineY [I] -- What line from 'Lines' are we drawing
 *    ScreenLine [I] -- Where on the screen are we going to put this line.
 *                      (0 = top of the screen)
 *    Line [I] -- The data for this line.
 *
 * FUNCTION:
 *    This function draws a line.
 *
 * RETURNS:
 *    The number of px's that this line takes (length in pixels)
 *
 * NOTES:
 *    'Line' will have it's LineWidthPx updated.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int DisplayText::DrawLine(int LineY,int ScreenLine,struct TextLine *Line)
{
    i_TextLineFrags CurFrag;
    struct TextCanvasFrag DisplayFrag;
    int LineLenPx;
    int SelX1;
    int SelY1;
    int SelX2;
    int SelY2;
    bool FragSelected;
    int SelectFrag1;
    int SelectFrag2;
    int FragIndex;
    int CharsOver1;
    int CharsOver2;
    int Chars;
    int CharsLeft;
    uint32_t SavedFGColor;
    uint32_t SavedBGColor;
    uint32_t SavedAttribs;
    string::iterator StartOfStr;
#ifdef DEBUG_SHOW_BUFFER_POS
    int DebugX=0;
    string TmpStr;
#endif
    FragSelected=false;

    /* We use -1 and INT_MAX as our no selection so we can just use a
        if(FragIndex>SelectFrag1) and if(FragIndex<SelectFrag2)
       without needing extra code.  We where going to break if we ever got that
       many fragments on a line anyway because we wrap.  (BTW if we break
       then we end up highlighting everything) */
    SelectFrag1=INT_MAX;
    SelectFrag2=-1;

    /* See if we have to deal with the selection */
    if(IsThereASelection())
    {
        /* See if we are drawing the selection */
        GetNormalizedSelection(SelX1,SelY1,SelX2,SelY2);

        /* See if we are in the selection */
        if(LineY>SelY1 && LineY<SelY2)
        {
            /* The whole line is selected */
            FragSelected=true;
        }
        else
        {
            if(LineY==SelY1)
            {
                /* Find the fragment that we need to split for the selection */
                CharsOver1=0;
                for(CurFrag=Line->Frags.begin(),FragIndex=0;
                        CurFrag!=Line->Frags.end();CurFrag++,FragIndex++)
                {
                    /* Does this selection start in this fragment */
                    if(CurFrag->FragType!=e_TextCanvasFrag_String)
                        continue;
                    Chars=utf8::unchecked::distance(CurFrag->Text.begin(),
                            CurFrag->Text.end());
                    if(SelX1>=CharsOver1 && SelX1<CharsOver1+Chars)
                    {
                        /* This is the frag we need to split */
                        SelectFrag1=FragIndex;
                        break;
                    }
                    CharsOver1+=Chars;
                }
            }
            if(LineY==SelY2)
            {
                /* Find the fragment that we need to split for the selection */
                CharsOver2=0;
                for(CurFrag=Line->Frags.begin(),FragIndex=0;
                        CurFrag!=Line->Frags.end();CurFrag++,FragIndex++)
                {
                    /* Does this selection start in this fragment */
                    if(CurFrag->FragType!=e_TextCanvasFrag_String)
                        continue;
                    Chars=utf8::unchecked::distance(CurFrag->Text.begin(),
                            CurFrag->Text.end());
                    if(SelX2>=CharsOver2 && SelX2<=CharsOver2+Chars)
                    {
                        /* This is the frag we need to split */
                        SelectFrag2=FragIndex;
                        break;
                    }
                    CharsOver2+=Chars;
                }
            }
        }
    }

    /* Redraw the line */
    UITC_Begin(UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),ScreenLine);
    UITC_ClearLine(UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
            Line->LineBackgroundColor);

    LineLenPx=0;
    for(CurFrag=Line->Frags.begin(),FragIndex=0;CurFrag!=Line->Frags.end();
            CurFrag++,FragIndex++)
    {
        if(!ShowNonPrintables &&
                CurFrag->FragType==e_TextCanvasFrag_NonPrintableChar)
        {
            continue;
        }
        DisplayFrag.FragType=CurFrag->FragType;
        DisplayFrag.Text=CurFrag->Text.c_str();
        DisplayFrag.Styling=CurFrag->Styling;
        DisplayFrag.Value=CurFrag->Value;
        DisplayFrag.Data=CurFrag->Data;
        CharsLeft=utf8::unchecked::distance(CurFrag->Text.begin(),
                CurFrag->Text.end());

#ifdef DEBUG_SHOW_BUFFER_POS
        {
            struct TextPointMarker *Marker;

            /* Show markers */
            for(Marker=MarkerList;Marker!=NULL;Marker=Marker->Next)
            {
                if(Marker->Valid)
                {
                    if(LineY==Marker->Y)
                    {
                        if(Marker->X>=DebugX &&
                                Marker->X<=DebugX+(int)CurFrag->Text.length())
                        {
                            /* Marker in this frag */
                            TmpStr=CurFrag->Text;
                            TmpStr[Marker->X-DebugX]='$';
                            DisplayFrag.Text=TmpStr.c_str();
                        }
                    }
                }
            }
            DebugX+=CurFrag->Text.length();
        }
#endif

        if(FragIndex==SelectFrag1)
        {
            /* Ok, we need to split this fragment with the "normal" part on
               the left */

            /* Copy the current string and clip it */
            Chars=SelX1-CharsOver1;
            CharsLeft-=Chars;
            if(CharsLeft<0)
                CharsLeft=0;
            TmpStr=DisplayFrag.Text;

            if(CharsLeft>0)
            {
                StartOfStr=TmpStr.begin();
                utf8::unchecked::advance(StartOfStr,Chars);
                TmpStr2.assign(StartOfStr,TmpStr.end());
            }
            else
            {
                StartOfStr=TmpStr.end();
                TmpStr2="";
            }
            TmpStr.erase(StartOfStr,TmpStr.end());
            DisplayFrag.Text=TmpStr.c_str();
            UITC_AddFragment(UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
                    &DisplayFrag);

            /* Place things so the end of the string is in this fragment */
            DisplayFrag.Text=TmpStr2.c_str();

            /* Adjust the start of the overset for the end frag (but only
               if we are in the same fragment) */
            if(SelectFrag1==SelectFrag2)
                CharsOver2+=Chars;

            FragSelected=true;
        }
        else if(FragIndex>SelectFrag1 && SelY1!=SelY2)
        {
            /* Highlight the whole fragment */
            FragSelected=true;
        }

        if(FragIndex<SelectFrag2 && SelY1!=SelY2)
        {
            /* Highlight the whole fragment */
            FragSelected=true;
        }
        else if(FragIndex==SelectFrag2)
        {
            /* Ok, we need to split this fragment with the "normal" part on the
               right. */

            /* Copy the current string and clip it */
            Chars=SelX2-CharsOver2;
            CharsLeft-=Chars;
            if(CharsLeft<0)
                CharsLeft=0;
            TmpStr=DisplayFrag.Text;

            if(CharsLeft>0)
            {
                StartOfStr=TmpStr.begin();
                utf8::unchecked::advance(StartOfStr,Chars);
                TmpStr2.assign(StartOfStr,TmpStr.end());
            }
            else
            {
                StartOfStr=TmpStr.end();
                TmpStr2="";
            }
            TmpStr.erase(StartOfStr,TmpStr.end());
            DisplayFrag.Text=TmpStr.c_str();

            /* Make it the highlight color */
            SavedFGColor=DisplayFrag.Styling.FGColor;
            SavedBGColor=DisplayFrag.Styling.BGColor;
            SavedAttribs=DisplayFrag.Styling.Attribs;

            DisplayFrag.Styling.FGColor=Settings->SelectionColors[e_Color_FG];
            DisplayFrag.Styling.BGColor=Settings->SelectionColors[e_Color_BG];
            DisplayFrag.Styling.Attribs=TXT_ATTRIB_FORCE;
            UITC_AddFragment(UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
                    &DisplayFrag);

            /* Restore the color */
            DisplayFrag.Styling.FGColor=SavedFGColor;
            DisplayFrag.Styling.BGColor=SavedBGColor;
            DisplayFrag.Styling.Attribs=SavedAttribs;
            /* Place things so the end of the string is in this fragment */
            DisplayFrag.Text=TmpStr2.c_str();

            FragSelected=false;
        }

        if(FragSelected)
        {
            DisplayFrag.Styling.FGColor=Settings->SelectionColors[e_Color_FG];
            DisplayFrag.Styling.BGColor=Settings->SelectionColors[e_Color_BG];
            DisplayFrag.Styling.Attribs=TXT_ATTRIB_FORCE;
        }

        UITC_AddFragment(UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
                &DisplayFrag);

        LineLenPx+=CurFrag->WidthPx;
    }

    /* Only draw the end of line markers if they are shown and we are not
       drawing the last line (we don't draw the marker on the last line) */
    if(ShowEndOfLines)
    {
        if(Line!=&Lines.back())
        {
            DisplayFrag.FragType=e_TextCanvasFragMAX;
            DisplayFrag.Text="";
            DisplayFrag.Styling=CurrentStyle;
            DisplayFrag.Value=0;
            DisplayFrag.Data=NULL;

            switch(Line->EOL)
            {
                case e_DTEOL_Soft:
                    DisplayFrag.FragType=e_TextCanvasFrag_SoftRet;
                break;
                case e_DTEOL_Hard:
                    DisplayFrag.FragType=e_TextCanvasFrag_HardRet;
                break;
                case e_DTEOLMAX:
                default:
                break;
            }
            if(DisplayFrag.FragType!=e_TextCanvasFragMAX)
            {
                UITC_AddFragment(
                        UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
                        &DisplayFrag);
                LineLenPx+=UITC_GetFragWidth(
                        UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
                        &DisplayFrag);
            }
        }

        if(Line->EOL==e_DTEOL_Hard)
        {
            DisplayFrag.FragType=e_TextCanvasFragMAX;
            DisplayFrag.Text="";
            DisplayFrag.Styling=CurrentStyle;
            DisplayFrag.Value=0;
            DisplayFrag.Data=NULL;

            DisplayFrag.Styling.Attribs&=~(TXT_ATTRIB_UNDERLINE|
                    TXT_ATTRIB_UNDERLINE_DOUBLE|
                    TXT_ATTRIB_UNDERLINE_DOTTED|
                    TXT_ATTRIB_UNDERLINE_DASHED|
                    TXT_ATTRIB_UNDERLINE_WAVY|
                    TXT_ATTRIB_OVERLINE|
                    TXT_ATTRIB_LINETHROUGHT|
                    TXT_ATTRIB_OUTLINE|
                    TXT_ATTRIB_BOX);

            DisplayFrag.Styling.Attribs|=TXT_ATTRIB_ROUNDBOX;

            switch(Line->EOLGuess)
            {
                case e_DTEOLGuess_Unknown:
                break;
                case e_DTEOLGuess_LF:
                case e_DTEOLGuess_LFCR:
                    DisplayFrag.FragType=e_TextCanvasFrag_NonPrintableChar;
                    DisplayFrag.Text="LF";
                break;
                case e_DTEOLGuess_CR:
                case e_DTEOLGuess_CRLF:
                    DisplayFrag.FragType=e_TextCanvasFrag_NonPrintableChar;
                    DisplayFrag.Text="CR";
                break;
                case e_DTEOLGuessMAX:
                break;
            }

            if(DisplayFrag.FragType!=e_TextCanvasFragMAX)
            {
                UITC_AddFragment(
                        UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
                        &DisplayFrag);
                LineLenPx+=UITC_GetFragWidth(
                        UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
                        &DisplayFrag);
            }

            DisplayFrag.FragType=e_TextCanvasFragMAX;

            switch(Line->EOLGuess)
            {
                case e_DTEOLGuess_LFCR:
                    DisplayFrag.FragType=e_TextCanvasFrag_NonPrintableChar;
                    DisplayFrag.Text="CR";
                break;
                case e_DTEOLGuess_CRLF:
                    DisplayFrag.FragType=e_TextCanvasFrag_NonPrintableChar;
                    DisplayFrag.Text="LF";
                break;
                case e_DTEOLGuess_Unknown:
                case e_DTEOLGuess_LF:
                case e_DTEOLGuess_CR:
                case e_DTEOLGuessMAX:
                break;
            }

            if(DisplayFrag.FragType!=e_TextCanvasFragMAX)
            {
                UITC_AddFragment(
                        UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
                        &DisplayFrag);
                LineLenPx+=UITC_GetFragWidth(
                        UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
                        &DisplayFrag);
            }
        }

    }

    UITC_End(UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl));

    return LineLenPx;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::RedrawFullScreen
 *
 * SYNOPSIS:
 *    void DisplayText::RedrawFullScreen(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function redraws the display screen.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::RedrawFullScreen(void)
{
    i_TextLines CurLine;
    i_TextLineFrags CurFrag;
    bool LineLenChanged;
    int LineLenPx;
    int y;

    if(ActiveLine==NULL || TextDisplayCtrl==NULL)
        return;

    LineLenChanged=false;
    for(y=0,CurLine=TopLine;y<WindowHeightChars && CurLine!=Lines.end();
            CurLine++,y++)
    {
        LineLenPx=DrawLine(TopLineY+y,y,&*CurLine);
        if(CurLine->LineWidthPx!=LineLenPx)
        {
            LineLenChanged=true;
            CurLine->LineWidthPx=LineLenPx;
        }
    }

    UITC_SetMaxLines(UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),y,
            Settings->DefaultColors[e_DefaultColors_BG]);

    if(LineLenChanged)
        ReFindLongestLineLength();
    RethinkScrollBars();
}

/*******************************************************************************
 * NAME:
 *    DisplayText::NoteNonPrintable
 *
 * SYNOPSIS:
 *    void DisplayText::NoteNonPrintable(const char *NoteStr);
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
 *    WriteChar()
 ******************************************************************************/
void DisplayText::NoteNonPrintable(const char *NoteStr)
{
    i_TextLineFrags AddedFrag;
    struct TextLineFrag SpecialFrag;

    try
    {
        AddedFrag=AddSpecialFrag(SpecialFrag);

        AddedFrag->FragType=e_TextCanvasFrag_NonPrintableChar;
        AddedFrag->Text=NoteStr;
        AddedFrag->Styling=CurrentStyle;
        AddedFrag->Styling.Attribs&=~(TXT_ATTRIB_UNDERLINE|
                TXT_ATTRIB_UNDERLINE_DOUBLE|
                TXT_ATTRIB_UNDERLINE_DOTTED|
                TXT_ATTRIB_UNDERLINE_DASHED|
                TXT_ATTRIB_UNDERLINE_WAVY|
                TXT_ATTRIB_OVERLINE|
                TXT_ATTRIB_LINETHROUGHT|
                TXT_ATTRIB_OUTLINE|
                TXT_ATTRIB_BOX);

        AddedFrag->Styling.Attribs|=TXT_ATTRIB_ROUNDBOX;
        AddedFrag->Value=0;
        AddedFrag->Data=NULL;
        AddedFrag->WidthPx=0;
        RethinkFragWidth(AddedFrag);

        RethinkInsertFrag();
        RedrawActiveLine();
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::SetShowNonPrintable
 *
 * SYNOPSIS:
 *    void DisplayText::SetShowNonPrintable(bool Show);
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
void DisplayText::SetShowNonPrintable(bool Show)
{
    ShowNonPrintables=Show;

    RethinkLineLengths();
    RethinkScrollBars();
    RedrawFullScreen();
}

/*******************************************************************************
 * NAME:
 *    DisplayText::SetShowEndOfLines
 *
 * SYNOPSIS:
 *    void DisplayText::SetShowEndOfLines(bool Show);
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
void DisplayText::SetShowEndOfLines(bool Show)
{
    ShowEndOfLines=Show;

    RethinkLineLengths();
    RethinkScrollBars();

    RedrawFullScreen();
}

/*******************************************************************************
 * NAME:
 *    DisplayText::RethinkLineLengths
 *
 * SYNOPSIS:
 *    void DisplayText::RethinkLineLengths(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function takes and recal's the length of all the lines and figures
 *    out the size of the longest line.  Normally this is called if we change
 *    something that effects the rendering of lines (font change for example).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    ReFindLongestLineLength()
 ******************************************************************************/
void DisplayText::RethinkLineLengths(void)
{
    i_TextLines CurLine;
    i_TextLineFrags CurFrag;
    int LineLenPx;

    LongestLinePx=0;
    for(CurLine=Lines.begin();CurLine!=Lines.end();CurLine++)
    {
        LineLenPx=0;
        for(CurFrag=CurLine->Frags.begin();CurFrag!=CurLine->Frags.end();
                CurFrag++)
        {
            RethinkFragWidth(CurFrag);
            LineLenPx+=CurFrag->WidthPx;
        }

        /* Add in the size of the return char */
        LineLenPx+=GetLineEndSize(&*CurLine);

        CurLine->LineWidthPx=LineLenPx;

        if(LineLenPx>LongestLinePx)
            LongestLinePx=LineLenPx;
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ReFindLongestLineLength
 *
 * SYNOPSIS:
 *    void DisplayText::ReFindLongestLineLength(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function goes though all the lines and recal's the 'LongestLinePx'
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    This is like RethinkLineLengths() but does not figure out the line
 *    lengths but instead uses the precalculated 'LineWidthPx' for each line,
 *    there by making this much quicker.
 *
 * SEE ALSO:
 *    RethinkLineLengths()
 ******************************************************************************/
void DisplayText::ReFindLongestLineLength(void)
{
    i_TextLines CurLine;
    i_TextLineFrags CurFrag;

    LongestLinePx=0;
    for(CurLine=Lines.begin();CurLine!=Lines.end();CurLine++)
    {
        if(CurLine->LineWidthPx>LongestLinePx)
            LongestLinePx=CurLine->LineWidthPx;
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::AddSpecialFrag
 *
 * SYNOPSIS:
 *    i_TextLineFrags DisplayText::AddSpecialFrag(struct TextLineFrag &SpecialFrag);
 *
 * PARAMETERS:
 *    SpecialFrag [I] -- The frag to add.  This will be copied and returned.
 *
 * FUNCTION:
 *    This function adds frag.  It follows the rules for adding a char but this
 *    char can't be selected.  It also does not setup any extra info about the
 *    frag.
 *
 * RETURNS:
 *    The frag that was added.
 *
 * SEE ALSO:
 *    NoteNonPrintable()
 ******************************************************************************/
i_TextLineFrags DisplayText::AddSpecialFrag(struct TextLineFrag &SpecialFrag)
{
    i_TextLineFrags AddedFrag;

    if(ActiveLine==NULL)
        throw(0);

    if(ActiveLine->Frags.back().FragType==e_TextCanvasFrag_String &&
            ActiveLine->Frags.back().Text.empty())
    {
        /* Ok, we have a blank string, replace it with the special frag */
        ActiveLine->Frags.pop_back();
    }

    if(!ActiveLine->Frags.empty() &&
        ActiveLine->Frags.front().FragType==e_TextCanvasFrag_HR)
    {
        /* Ok, the first frag of this line is a HR, we delete that and
           setup for a virtual insert */
        ActiveLine->Frags.clear();
        InsertFrag=ActiveLine->Frags.end();
        /* We don't set 'InsertPos' so if the user was in virtual space
           we will still pad correctly */
    }

    if(InsertFrag==ActiveLine->Frags.end())
    {
        if(InsertPos>=0)
        {
            /* Cursor in virtual space, we need to pad to get to it before
               appending */
            PadOutCurrentLine2Cursor();
            RethinkFragWidth(InsertFrag);
        }
        else
        {
            /* Cursor in virtual space, 1 place past the end of the last,
               just append to the end */
        }
        AddedFrag=ActiveLine->Frags.insert(ActiveLine->Frags.end(),SpecialFrag);
    }
    else
    {
        if(InsertPos>=0)
        {
            AddedFrag=InsertSpecialFragInMiddleOfString(SpecialFrag);
        }
        else
        {
            /* Cursor is in virtual space, 1 place past the end of the last,
               but the last frag isn't a string frag (we just append) */
            AddedFrag=ActiveLine->Frags.insert(ActiveLine->Frags.end(),
                    SpecialFrag);
        }
    }

    return AddedFrag;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::InsertSpecialFragInMiddleOfString
 *
 * SYNOPSIS:
 *    i_TextLineFrags DisplayText::InsertSpecialFragInMiddleOfString(
 *          struct TextLineFrag &SpecialFrag);
 *
 * PARAMETERS:
 *    SpecialFrag [I] -- The frag to insert.  A copy of this will be made.
 *
 * FUNCTION:
 *    This function inserts a special frag in the middle of a string frag.
 *
 * RETURNS:
 *    The new frag that was added
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
i_TextLineFrags DisplayText::InsertSpecialFragInMiddleOfString(
        struct TextLineFrag &SpecialFrag)
{
    i_TextLineFrags NextFrag;
    i_TextLineFrags AddFrag;
    i_TextLineFrags NewFrag;

    NextFrag=InsertFrag;
    NextFrag++;

    /* Split the string (unless it would result in an empty string) */
    if(InsertPos==0)
    {
        /* We are at the start of the string, we would be moving all of the
           string frag, so just insert before the current frag */
        NewFrag=ActiveLine->Frags.insert(InsertFrag,SpecialFrag);
    }
    else
    {
        /* We are in the middle of a string and need to spilt it */
        NewFrag=ActiveLine->Frags.insert(NextFrag,SpecialFrag);

        /* Add a new string frag and split the current frag */
        NextFrag=InsertFrag;
        NextFrag++; // Move to the newly added special frag
        NextFrag++; // Move to the one after the new special frag

        AddFrag=AddNewEmptyFragToLine(ActiveLine,NextFrag);
        AddFrag->Styling=InsertFrag->Styling;
        AddFrag->Value=InsertFrag->Value;
        AddFrag->Data=InsertFrag->Data;
        AddFrag->Text.assign(InsertFrag->Text,InsertPos,string::npos);
        RethinkFragWidth(AddFrag);

        InsertFrag->Text.erase(InsertPos);
        RethinkFragWidth(InsertFrag);
    }

    return NewFrag;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ApplySettings
 *
 * SYNOPSIS:
 *    void DisplayText::ApplySettings(void);
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
void DisplayText::ApplySettings(void)
{
    CurrentStyle.FGColor=Settings->DefaultColors[e_DefaultColors_FG];
    CurrentStyle.BGColor=Settings->DefaultColors[e_DefaultColors_BG];
    CurrentStyle.Attribs=0;
    CurrentStyle.ULineColor=CurrentStyle.FGColor;

    FontName=Settings->FontName;
    FontSize=Settings->FontSize;
    FontBold=Settings->FontBold;
    FontItalic=Settings->FontItalic;

    /* DEBUG PAUL: Should we do something here when the user changes the
       defaults?  Maybe only change if we where already using the defaults,
       what about the lines that where already drawn?  What if the cursor
       was in the middle of the text??? */

    SetupCanvas();

    if(g_Settings.MouseCursorIBeam)
    {
        UITC_SetMouseCursor(UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
                e_UIMouse_Cursor_IBeam);
    }
    else
    {
        UITC_SetMouseCursor(UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
                e_UIMouse_Cursor_Default);
    }

    RedrawFullScreen();
}

/*******************************************************************************
 * NAME:
 *    DisplayText::SetCursorBlinking
 *
 * SYNOPSIS:
 *    void DisplayText::SetCursorBlinking(bool Blinking);
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
void DisplayText::SetCursorBlinking(bool Blinking)
{
    if(TextDisplayCtrl==NULL)
        return;

    UITC_SetCursorBlinking(TextDisplayCtrl,Blinking);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::SetCursorStyle
 *
 * SYNOPSIS:
 *    void DisplayText::SetCursorStyle(e_TextCursorStyleType Style)
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
void DisplayText::SetCursorStyle(e_TextCursorStyleType Style)
{
    CursorStyle=Style;
    RethinkCursorHidden();
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
void DisplayText::SetInFocus(void)
{
    union DBEventData Info;

    HasFocus=true;

    UITC_SetFocus(TextDisplayCtrl,e_UITCSetFocus_Main);

    Info.Focus.HasFocus=HasFocus;
    SendEvent(e_DBEvent_FocusChange,&Info);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::SetCursorXY
 *
 * SYNOPSIS:
 *    void DisplayText::SetCursorXY(unsigned int x,unsigned y);
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
void DisplayText::SetCursorXY(unsigned int x,unsigned y)
{
    if(ActiveLine!=NULL)
        ActiveLine->EOL=e_DTEOL_Hard;

    MoveCursor(x,y,false);

    InvalidateAllMarks();
}

/*******************************************************************************
 * NAME:
 *    DisplayText::GetCursorXY
 *
 * SYNOPSIS:
 *    void DisplayText::GetCursorXY(unsigned int *x,unsigned int *y);
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
void DisplayText::GetCursorXY(unsigned int *x,unsigned int *y)
{
    *x=CursorX;
    *y=CursorY;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::GetScreenSize
 *
 * SYNOPSIS:
 *    void DisplayText::GetScreenSize(uint32_t *Width,uint32_t *Height);
 *
 * PARAMETERS:
 *    Width [O] -- The width (in chars) of the screen.  This is the average.
 *    Height [O] -- The height (in chars) of the screen
 *
 * FUNCTION:
 *    This function gets the size of the screen.  This is basicly the size
 *    of the canvas divided by the current font height/width.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::GetScreenSize(uint32_t *Width,uint32_t *Height)
{
    *Width=ScreenWidthChars;
    *Height=ScreenHeightChars;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::RethinkInsertFrag
 *
 * SYNOPSIS:
 *    bool DisplayText::RethinkInsertFrag(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rethinks the point we are going to insert / replace chars
 *    when we write a new char to this display.  It updates the 'InsertFrag'
 *    and 'ActiveLine' to the correct points.
 *
 *    It sets up one of these modes:
 *      1) Cursor is in virtual space
 *          InsertFrag=ActiveLine->Frags.end()
 *          InsertPos>=0
 *              Pad out the current line so we can append a char
 *      2) Cursor is in virtual space 1 place past the end of the last
 *         frag, but the last frag is not a string type
 *          InsertFrag=ActiveLine->Frags.end()
 *          InsertPos=-1
 *              Append a new frag (of string type) and setup to append to it
 *      3) Cursor is in virtual space 1 place past the end of the last frag
 *          and we just need to append the char
 *          InsertFrag=Frag
 *          InsertPos=-1
 *              Append a char to 'Frag'
 *      4) Cursor is in the middle of a string frag, replace the char
 *          InsertFrag=Frag
 *          InsertPos>=0
 *              Replace the char at 'InsertPos':
 *                  * Get the number of bytes in the char being replaced
 *                  * Get the number of bytes in the new char
 *                  * delta=new_len-old_len
 *                  * if(delta<0) Remove 'delta' number from the string
 *                  * if(delta>0) Add 'delta' number to the string
 *                  * Overwrite the chars in the string
 *
 * RETURNS:
 *    true -- We needed to add lines to be able to make things valid
 *    false -- We just updated insert pos and did not change any lines.
 *
 * NOTES:
 *    This function will also add blank lines if the 'CursorY' to below the
 *    last line.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayText::RethinkInsertFrag(void)
{
    i_TextLineFrags Frag;
    unsigned int CharPos;
    unsigned int StrLength;
    const char *StartOfChar;
    const char *EndOfChar;
    unsigned int CharLen;    // How many bytes is this char
    int XPosInLine;
    i_TextLines CurLine;
    struct TextLine BlankLine;
    int y;
    bool Had2Change;

    Had2Change=false;
    if(CursorY!=ActiveLineY)
    {
        /* We need to figure out ActiveLine here again */
        CurLine=ScreenFirstLine;
        for(y=0;y<CursorY && CurLine!=Lines.end();y++)
            CurLine++;
        if(CurLine==Lines.end())
        {
            /* We didn't have enough lines, add blank lines */
            BlankLine.LineBackgroundColor=
                    Settings->DefaultColors[e_DefaultColors_BG];
            BlankLine.LineWidthPx=0;
            BlankLine.EOL=e_DTEOL_Hard;
            BlankLine.EOLGuess=e_DTEOLGuess_Unknown;

            while(y<=CursorY)
            {
                Lines.push_back(BlankLine);
                LinesCount++;
                y++;
            }
            CurLine=Lines.end();
            CurLine--;
        }
        ActiveLine=&*CurLine;
        ActiveLineY=CursorY;

        /* We need to do a full redraw (to draw the new lines on the screen) */
        Had2Change=true;
    }

    if(ActiveLine==NULL)
        return Had2Change;

    if(ActiveLine->Frags.empty())
    {
        /* We have no frags, just go into pad mode */
        InsertFrag=ActiveLine->Frags.end();
        InsertPos=0;
        return Had2Change;
    }

    XPosInLine=0;
    for(Frag=ActiveLine->Frags.begin();Frag!=ActiveLine->Frags.end();Frag++)
    {
        switch(Frag->FragType)
        {
            case e_TextCanvasFrag_String:
                StartOfChar=Frag->Text.c_str();
                StrLength=Frag->Text.length();
                for(CharPos=0;CharPos<StrLength;CharPos+=CharLen)
                {
                    if(XPosInLine==CursorX)
                    {
                        /* Ok, we have found our insert pos and fragment */
                        InsertFrag=Frag;
                        InsertPos=CharPos;
                        return Had2Change;
                    }

                    EndOfChar=StartOfChar;
                    utf8::unchecked::advance(EndOfChar,1);
                    CharLen=EndOfChar-StartOfChar;

                    XPosInLine++;
                    StartOfChar+=CharLen;
                }
            break;
            case e_TextCanvasFrag_NonPrintableChar:
            case e_TextCanvasFrag_SoftRet:
            case e_TextCanvasFrag_HardRet:
            case e_TextCanvasFrag_RetText:
            case e_TextCanvasFrag_HR:
            case e_TextCanvasFragMAX:
            default:
            break;
        }
    }

    /* Ok, we are in virtual space, see if it's one of the quick access
       points (XPosInLine is pointing to the end+1) */
    if(XPosInLine==CursorX)
    {
        /* We are just 1 past the cursor */
        if(ActiveLine->Frags.back().FragType!=e_TextCanvasFrag_String)
        {
            /* The last fragment is not a string type */
            InsertFrag=ActiveLine->Frags.end();
            InsertPos=-1;
            return Had2Change;
        }
        /* We are just past the last string pos, so we go with append mode */
        InsertFrag=ActiveLine->Frags.end();
        InsertFrag--;
        InsertPos=-1;
        return Had2Change;
    }

    /* Ok, we are way into virtual space */
    InsertFrag=ActiveLine->Frags.end();
    InsertPos=0;

    return Had2Change;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::WriteChar
 *
 * SYNOPSIS:
 *    void DisplayText::WriteChar(uint8_t *Chr);
 *
 * PARAMETERS:
 *    Chr [I] -- The char to add.  This is a UTF8 char that is 0 term'ed.
 *
 * FUNCTION:
 *    This function adds a char to the display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    NoteNonPrintable()
 ******************************************************************************/
void DisplayText::WriteChar(uint8_t *Chr)
{
    LastSeenLF=false;
    LastSeenCR=false;

    WriteCharWithOptions(Chr,true);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::WriteCharWithOptions
 *
 * SYNOPSIS:
 *    void DisplayText::WriteCharWithOptions(uint8_t *Chr,bool AdvCursor);
 *
 * PARAMETERS:
 *    Chr [I] -- The char to add.  This is a UTF8 char that is 0 term'ed.
 *    AdvCursor [I] -- If this is true then we move the cursor when we are done.
 *
 * FUNCTION:
 *    This is the real WriteChar().  It has more options than WriteChar()
 *    supports.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    WriteChar()
 ******************************************************************************/
void DisplayText::WriteCharWithOptions(uint8_t *Chr,bool AdvCursor)
{
    try
    {
        if(ActiveLine==NULL)
            return;

        if(!ActiveLine->Frags.empty() &&
            ActiveLine->Frags.front().FragType==e_TextCanvasFrag_HR)
        {
            /* Ok, the first frag of this line is a HR, we delete that and
               setup for a virtual insert */
            ActiveLine->Frags.clear();
            InsertFrag=ActiveLine->Frags.end();
            /* We don't set 'InsertPos' so if the user was in virtual space
               we will still pad correctly */
        }

        /* See how we are writing this char (see RethinkInsertFrag() for info on
           the 4 different modes) */
        if(InsertFrag==ActiveLine->Frags.end())
        {
            if(InsertPos>=0)
            {
                /* We need to pad out the current line to the cursor before an
                   append */
                PadOutCurrentLine2Cursor();
            }
            else
            {
                /* We need to start a new string frag and then append */
                InsertFrag=AddNewEmptyFragToLine(ActiveLine,
                        ActiveLine->Frags.end());
            }

            InsertFrag->Text.append((char *)Chr);
            RethinkFragWidth(InsertFrag);

            /* Append any new chars */
            InsertPos=-1;
        }
        else
        {
            if(InsertPos>=0)
            {
                /* We just need to overwrite the current char */
                DoOverwriteInsertPos(Chr);
            }
            else
            {
                if(!CmpCharStyle(&CurrentStyle,&InsertFrag->Styling))
                {
                    /* Style changed we need to add a new frag */
                    InsertFrag=AddNewEmptyFragToLine(ActiveLine,
                            ActiveLine->Frags.end());
                }

                /* We just append to the current frag */
                InsertFrag->Text.append((char *)Chr);
                RethinkFragWidth(InsertFrag);
            }
        }

        if(AdvCursor)
            AdjustCursorAfterWriteChar(Chr);
        RedrawActiveLine();
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::PadOutCurrentLine2Cursor
 *
 * SYNOPSIS:
 *    void DisplayText::PadOutCurrentLine2Cursor(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is a helper function that takes the current line and
 *    adds padding out to the cursor position.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::PadOutCurrentLine2Cursor(void)
{
    PadOutLine(ActiveLine,CursorX);

    /* Move Insert Frag to the (new) end of the line */
    /* See if we are a string frag and that the styles are the same */
    if(ActiveLine->Frags.back().FragType!=e_TextCanvasFrag_String ||
            !CmpCharStyle(&CurrentStyle,&ActiveLine->Frags.back().Styling))
    {
        InsertFrag=AddNewEmptyFragToLine(ActiveLine,ActiveLine->Frags.end());
    }
    else
    {
        InsertFrag=ActiveLine->Frags.end();
        InsertFrag--;
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::PadOutLine
 *
 * SYNOPSIS:
 *    void DisplayText::PadOutLine(struct TextLine *Line,int Pos);
 *
 * PARAMETERS:
 *    Line [I] -- The line to pad out
 *    Pos [I] -- The offset to pad the line out to.  This is the total number
 *               of chars that the line will have after we are done.
 *
 * FUNCTION:
 *    This function adds padding to the end of a line out to an offset.
 *    Padding is spaces in the default style.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::PadOutLine(struct TextLine *Line,int Pos)
{
    int LineLen;
    i_TextLineFrags Frag;
    const char *StartOfChar;
    unsigned int StrLength;
    struct CharStyling PaddingStyle;
    i_TextLineFrags AppendFrag;

    LineLen=0;
    for(Frag=Line->Frags.begin();Frag!=Line->Frags.end();Frag++)
    {
        switch(Frag->FragType)
        {
            case e_TextCanvasFrag_String:
                StartOfChar=Frag->Text.c_str();
                StrLength=Frag->Text.length();

                LineLen+=utf8::unchecked::distance(StartOfChar,
                        StartOfChar+StrLength);
            break;
            case e_TextCanvasFrag_NonPrintableChar:
            case e_TextCanvasFrag_SoftRet:
            case e_TextCanvasFrag_HardRet:
            case e_TextCanvasFrag_RetText:
            case e_TextCanvasFrag_HR:
            case e_TextCanvasFragMAX:
            default:
            break;
        }
    }

    if(Pos<LineLen)
        return;

    /* We need to pad out in the background color until the point we are
       going to insert at */
    PaddingStyle.FGColor=Settings->DefaultColors[e_DefaultColors_FG];
    PaddingStyle.BGColor=Line->LineBackgroundColor;
    PaddingStyle.Attribs=0;
    PaddingStyle.ULineColor=PaddingStyle.FGColor;

    /* See if we are string frag and that the styles are the same */
    if(Line->Frags.empty() ||
            Line->Frags.back().FragType!=e_TextCanvasFrag_String ||
            !CmpCharStyle(&PaddingStyle,&Line->Frags.back().Styling))
    {
        /* Nope, add a new frag with a padding style frag */
        AppendFrag=AddNewEmptyFragToLine(Line,Line->Frags.end());
        AppendFrag->Styling=PaddingStyle;
    }
    else
    {
        /* We can just extend the end frag */
        AppendFrag=Line->Frags.end();
        AppendFrag--;
    }

    /* Add the line background color padding */
    if(Pos-LineLen>0)
        AppendFrag->Text.append(Pos-LineLen,' ');
}

/*******************************************************************************
 * NAME:
 *    DisplayText::DoOverwriteInsertPos
 *
 * SYNOPSIS:
 *    void DisplayText::DoOverwriteInsertPos(uint8_t *Chr);
 *
 * PARAMETERS:
 *    Chr [I] -- The char to add.  This is a UTF8 char that is 0 term'ed.
 *
 * FUNCTION:
 *    This function overwrites a char at 'InsertFrag' by 'InsertPos'.  This is
 *    part of DisplayText::WriteChar().
 *
 *    It also handles erase things that should be after an overwrite.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DisplayText::WriteChar()
 ******************************************************************************/
void DisplayText::DoOverwriteInsertPos(uint8_t *Chr)
{
    unsigned int NewCharSize;
    unsigned int OldCharSize;
    const char *StartOfChar;
    const char *EndOfChar;
    int Delta;
    i_TextLineFrags FirstFrag;
    i_TextLineFrags LastFrag;
    i_TextLineFrags NextFrag;
    i_TextLineFrags PrevFrag;
    i_TextLineFrags NewFrag;
    i_TextLineFrags SplitFrag;
    i_TextLineFrags Frag;
    bool Overwrite;

    NewCharSize=strlen((char *)Chr);

    StartOfChar=InsertFrag->Text.c_str();
    StartOfChar+=InsertPos;
    EndOfChar=StartOfChar;
    utf8::unchecked::advance(EndOfChar,1);
    OldCharSize=EndOfChar-StartOfChar;

    Overwrite=true;

    /* We have a special case if we are at the start of the frag, if the
       prev frag has the new style then we need to move the char to the
       prev frag. */
    if(InsertPos==0 && !CmpCharStyle(&CurrentStyle,&InsertFrag->Styling))
    {
        /* Check if the frag before this one has the needed style */
        if(InsertFrag!=ActiveLine->Frags.begin())
        {
            PrevFrag=InsertFrag;
            PrevFrag--;

            if(PrevFrag->FragType==e_TextCanvasFrag_String &&
                    CmpCharStyle(&CurrentStyle,&PrevFrag->Styling))
            {
                /* They are the same, delete the char from this frag and
                   append the prev frag */
                InsertFrag->Text.erase(0,OldCharSize);
                RethinkFragWidth(InsertFrag);

                PrevFrag->Text.append((char *)Chr);
                RethinkFragWidth(PrevFrag);

                if(InsertFrag->Text.empty())
                {
                    /* We need to delete this empty frag */
                    ActiveLine->Frags.erase(InsertFrag);
                }
                InsertFrag=PrevFrag;
                InsertPos=InsertFrag->Text.length()-1;
                Overwrite=false;
            }
        }
    }

    if(Overwrite)
    {
        /* See if current style and the style of the char being overwritten are
           the same */
        if(!CmpCharStyle(&CurrentStyle,&InsertFrag->Styling))
        {
            /* We need to split this frag, and add a new one for this char */

            NextFrag=InsertFrag;
            NextFrag++;

            if(InsertFrag->Text.length()==InsertPos+OldCharSize)
            {
                /* We are replacing the last char, so erase it and just add
                   a new frag */
                InsertFrag->Text.erase(InsertPos);
                RethinkFragWidth(InsertFrag);

                InsertFrag=AddNewEmptyFragToLine(ActiveLine,NextFrag);
                InsertFrag->Text.assign((char *)Chr);
                InsertPos=0;
            }
            else
            {
                /* We need to split the string */
                NewFrag=AddNewEmptyFragToLine(ActiveLine,NextFrag);
                NewFrag->Text.assign((char *)Chr);
                RethinkFragWidth(NewFrag);

                SplitFrag=AddNewEmptyFragToLine(ActiveLine,NextFrag);
                SplitFrag->Text.assign(InsertFrag->Text,InsertPos+OldCharSize,
                        string::npos);
                SplitFrag->Styling=InsertFrag->Styling;
                SplitFrag->Value=InsertFrag->Value;
                SplitFrag->Data=InsertFrag->Data;

                RethinkFragWidth(SplitFrag);

                InsertFrag->Text.erase(InsertPos);
                RethinkFragWidth(InsertFrag);
                if(InsertFrag->Text.empty())
                {
                    /* We have an empty frag, delete it */
                    ActiveLine->Frags.erase(InsertFrag);
                }

                InsertFrag=NewFrag;
                InsertPos=0;
            }
        }
        else
        {
            Delta=NewCharSize-OldCharSize;
            if(Delta<0)
            {
                /* Remove some bytes from the string */
                InsertFrag->Text.erase(InsertPos,-Delta);
            }
            if(Delta>0)
            {
                /* Add some bytes to the string */
                InsertFrag->Text.insert(InsertPos,Delta,' ');
            }
            /* Over write the char */
            InsertFrag->Text.replace(InsertPos,NewCharSize,(char *)Chr);

            RethinkFragWidth(InsertFrag);
        }
    }

    HandleDeletingNPAfterOverwrite();
}

/*******************************************************************************
 * NAME:
 *    DisplayText::AddNewEmptyFragToLine
 *
 * SYNOPSIS:
 *    i_TextLineFrags DisplayText::AddNewEmptyFragToLine(struct TextLine *Line,
 *              i_TextLineFrags InsertPoint);
 *
 * PARAMETERS:
 *    Line [I] -- The line to add the empty frag to
 *    InsertPoint [I] -- The point to insert if front of (this will become
 *                       the frag after the newly added frag).  This must
 *                       be something on 'Line->Frags'
 *
 * FUNCTION:
 *    This function adds a new blank string fragment to 'Line'.
 *
 * RETURNS:
 *    A pointer to the new fragment that was added.
 *
 * NOTES:
 *    This function does not check if there is already a string frag there
 *    already or not, it just adds a new one.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
i_TextLineFrags DisplayText::AddNewEmptyFragToLine(struct TextLine *Line,
        i_TextLineFrags InsertPoint)
{
    /* We need to start a new string frag and then append */
    struct TextLineFrag NewFrag;

    NewFrag.FragType=e_TextCanvasFrag_String;
    NewFrag.Styling=CurrentStyle;
    NewFrag.Text="";
    NewFrag.WidthPx=0;

    return Line->Frags.insert(InsertPoint,NewFrag);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::AdjustCursorAfterWriteChar
 *
 * SYNOPSIS:
 *    void DisplayText::AdjustCursorAfterWriteChar(uint8_t *Chr);
 *
 * PARAMETERS:
 *    Chr [I] -- The char that was just added
 *
 * FUNCTION:
 *    This function moves the cursor to the next position after a new char has
 *    come in.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::AdjustCursorAfterWriteChar(uint8_t *Chr)
{
    int NewCursorPos;
    int NewCursorY;
    struct TextCanvasFrag DisplayFrag;

    NewCursorPos=CursorX+1;
    NewCursorY=CursorY;

    if(TextDisplayCtrl!=NULL)
    {
        /* See if we will move past the end of the line */
        if(NewCursorPos>=ScreenWidthChars)
        {
            /* Ok, we need to do a soft wrap here */
            ActiveLine->EOL=e_DTEOL_Soft;

            NewCursorPos=0;

            /* Redraw any changes to the current line before we move on */
            RedrawActiveLine();

            MoveToNextLine(NewCursorY);
            CursorXPx=0;
        }
        else
        {
            /* Move the Px by the width of the char we just added */
            DisplayFrag.FragType=e_TextCanvasFrag_String;
            DisplayFrag.Text=(const char *)Chr;
            if(InsertFrag!=ActiveLine->Frags.end())
            {
                DisplayFrag.Styling=InsertFrag->Styling;
                DisplayFrag.Value=InsertFrag->Value;
                DisplayFrag.Data=InsertFrag->Data;
            }
            else
            {
                DisplayFrag.Styling=CurrentStyle;
                DisplayFrag.Value=0;
                DisplayFrag.Data=NULL;
            }

            CursorXPx+=UITC_GetFragWidth(
                    UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
                    &DisplayFrag);
        }
    }

    MoveCursor(NewCursorPos,NewCursorY,true);   // We have already adjusted CursorXPx
}

/*******************************************************************************
 * NAME:
 *    DisplayText::RethinkFragWidth
 *
 * SYNOPSIS:
 *    void DisplayText::RethinkFragWidth(i_TextLineFrags Frag);
 *
 * PARAMETERS:
 *    Frag [I/O] -- The frag to rethink the px width of
 *
 * FUNCTION:
 *    This is a helper function that takes a frag and rethinks the width in
 *    pixels of that frag.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    if 'TextDisplayCtrl' is not valid then this sets the width to 0.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::RethinkFragWidth(i_TextLineFrags Frag)
{
    struct TextCanvasFrag DisplayFrag;

    Frag->WidthPx=0;

    if(TextDisplayCtrl==NULL)
        return;

    if(!ShowNonPrintables && Frag->FragType==e_TextCanvasFrag_NonPrintableChar)
        return;

    DisplayFrag.FragType=Frag->FragType;
    DisplayFrag.Text=Frag->Text.c_str();
    DisplayFrag.Styling=Frag->Styling;
    DisplayFrag.Value=Frag->Value;
    DisplayFrag.Data=Frag->Data;

    Frag->WidthPx=UITC_GetFragWidth(
            UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),&DisplayFrag);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::HandleDeletingNPAfterOverwrite
 *
 * SYNOPSIS:
 *    void DisplayText::HandleDeletingNPAfterOverwrite(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function handles deleting non-printable chars after an overwrite.
 *    'InsertFrag' and 'InsertPos' need to be setup to the new position before
 *    calling this function.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::HandleDeletingNPAfterOverwrite(void)
{
    i_TextLineFrags PrevFrag;
    i_TextLineFrags Frag;
    i_TextLineFrags FirstFrag;
    unsigned int NextCharSize;
    const char *StartOfChar;
    const char *EndOfChar;

    StartOfChar=InsertFrag->Text.c_str();
    StartOfChar+=InsertPos;
    EndOfChar=StartOfChar;
    utf8::unchecked::advance(EndOfChar,1);
    NextCharSize=EndOfChar-StartOfChar;

    /* Special case, if we only have NP frags before this frag and we are
       at InsertPos=0 then we need to delete the NP frags as well */
    if(InsertPos==0)
    {
        PrevFrag=InsertFrag;
        for(PrevFrag=ActiveLine->Frags.begin();
                PrevFrag!=ActiveLine->Frags.end();PrevFrag++)
        {
            if(PrevFrag->FragType!=e_TextCanvasFrag_NonPrintableChar &&
                    PrevFrag->FragType!=e_TextCanvasFrag_SoftRet &&
                    PrevFrag->FragType!=e_TextCanvasFrag_HardRet &&
                    PrevFrag->FragType!=e_TextCanvasFrag_HR)
            {
                break;
            }
        }
        if(PrevFrag==InsertFrag)
        {
            /* All the frag's before the insert point are NP, kill them */
            ActiveLine->Frags.erase(ActiveLine->Frags.begin(),InsertFrag);
        }
    }

    /* Now we need to remove any non-printable frags that follow
       this frag (if we are at the end of the frag) */
    if(InsertFrag->Text.length()==InsertPos+NextCharSize)
    {
        /* We start erasing at the frag after the current frag */
        Frag=InsertFrag;
        Frag++;
        FirstFrag=Frag;
        if(FirstFrag==ActiveLine->Frags.end())
        {
            /* Nothing to do as we are at the end of the list */
            return;
        }

        /* Find the group of NP chars (and any blank strings in there) */
        while(Frag!=ActiveLine->Frags.end())
        {
            switch(Frag->FragType)
            {
                case e_TextCanvasFrag_NonPrintableChar:
                case e_TextCanvasFrag_SoftRet:
                case e_TextCanvasFrag_HardRet:
                case e_TextCanvasFrag_RetText:
                case e_TextCanvasFrag_HR:
                break;
                case e_TextCanvasFrag_String:
                    if(!Frag->Text.empty())
                        goto BreakWhileLoop;
                break;
                case e_TextCanvasFragMAX:
                default:
                    goto BreakWhileLoop;
                break;
            }
            Frag++;
        }
BreakWhileLoop:
        ActiveLine->Frags.erase(FirstFrag,Frag);
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::AddTab
 *
 * SYNOPSIS:
 *    void DisplayText::AddTab(void);
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
void DisplayText::AddTab(void)
{
    int NewPos;

    if(Settings->TabSize==0)
        return;

    /* Move the cursor to the next tab pos */
    NewPos=CursorX+((Settings->TabSize)-CursorX%(Settings->TabSize));

    MoveCursor(NewPos,CursorY,false);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::AddReverseTab
 *
 * SYNOPSIS:
 *    void DisplayText::AddReverseTab(void);
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
void DisplayText::AddReverseTab(void)
{
    int NewPos;
    int Amount;

    if(Settings->TabSize==0)
        return;

    /* Move the cursor to the prev tab pos */
    Amount=CursorX%(Settings->TabSize);
    if(Amount==0)
        Amount=Settings->TabSize;
    NewPos=CursorX-Amount;

    MoveCursor(NewPos,CursorY,false);

    InvalidateAllMarks();
}

/*******************************************************************************
 * NAME:
 *    DisplayText::RethinkTextAreaSize
 *
 * SYNOPSIS:
 *    void DisplayText::RethinkTextAreaSize(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rethinks the number of chars in the text area.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::RethinkTextAreaSize(void)
{
    if(TextDisplayCtrl==NULL)
        return;

    TextAreaWidthPx=UITC_GetWidgetWidth(UITC_GetTextDisplayPrimaryColumn(
            TextDisplayCtrl));
    TextAreaHeightPx=UITC_GetWidgetHeight(TextDisplayCtrl);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::RethinkWindowSize
 *
 * SYNOPSIS:
 *    void DisplayText::RethinkWindowSize(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rethinks the size of the window area.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::RethinkWindowSize(void)
{
    int LeftEdge;
    int TopEdge;
    int Width;
    int Height;
    int OldWidth;
    int OldHeight;
    int r;
    int NewX;
    int NewY;
    t_UIScrollBarCtrl *VertScroll;
    bool Force2Bottom;
    bool RedrawNeeded;
    int CursorGlobalY;

    RedrawNeeded=false;

    Force2Bottom=ScrollBarAtBottom();

    OldWidth=ScreenWidthChars;
    OldHeight=ScreenHeightChars;

    if(Settings->TermSizeFixedWidth)
        ScreenWidthChars=Settings->TermSizeWidth;
    else
        ScreenWidthChars=TextAreaWidthPx/CharWidthPx;

    if(Settings->TermSizeFixedHeight)
        ScreenHeightChars=Settings->TermSizeHeight;
    else
        ScreenHeightChars=TextAreaHeightPx/CharHeightPx;

    WindowHeightChars=ScreenHeightChars;
    if(WindowHeightChars>TextAreaHeightPx/CharHeightPx)
        WindowHeightChars=TextAreaHeightPx/CharHeightPx;

    /* We keep the same cursor Y (unless we need to change it) */
    NewX=CursorX;
    NewY=CursorY;

    /* DEBUG PAUL: Should this be moved over to it's own function? */
    if(OldWidth!=ScreenWidthChars)
    {
        /* Rethink all the lines widths */
    }

    if(OldHeight!=ScreenHeightChars)
    {
        /* Move the cursor (because it's relative to topline) (screen can't be
           0 because then the cursor is in invalid space) */
        if(LinesCount<OldHeight || OldHeight==0)
            CursorGlobalY=CursorY;
        else
            CursorGlobalY=LinesCount-OldHeight+CursorY;

        if(LinesCount<ScreenHeightChars || ScreenHeightChars==0)
            NewY=CursorGlobalY;
        else
            NewY=CursorGlobalY+ScreenHeightChars-LinesCount;

        if(NewY<0)
            NewY=0;

        if(LinesCount>=ScreenHeightChars)
        {
            /* Ok, we need to adjust 'ScreenFirstLine' and the Cursor pos */
            ScreenFirstLine=Lines.end();
            for(r=0;r<ScreenHeightChars;r++)
                ScreenFirstLine--;

            /* If we are scrolled all the way at the bottom then keep it that way */
            RethinkScrollBars();
            if(Force2Bottom)
            {
                if(TextDisplayCtrl!=NULL)
                {
                    /* Force 'TopLine' to be 'ScreenFirstLine' */
                    TopLine=ScreenFirstLine;
                    TopLineY=LinesCount-ScreenHeightChars;
                    VertScroll=UITC_GetVertSlider(TextDisplayCtrl);
                    UISetScrollBarPos(VertScroll,TopLineY);

                    UITC_SetCursorPos(TextDisplayCtrl,CursorX,
                            CalcCorrectedCursorPos());
                    RethinkCursorHidden();
                    RedrawNeeded=true;
                }
            }
        }
        else
        {
            ScreenFirstLine=Lines.begin();
        }
    }

    ActiveLineY=-1; // Force a rethinking of active line
    MoveCursor(NewX,NewY,false);

    /* Set the clipping and offset from the edge of the widget */
    if(TextDisplayCtrl!=NULL)
    {
        LeftEdge=0;
        TopEdge=0;
        Width=TextAreaWidthPx;
        Height=TextAreaHeightPx;

        if(Settings->TermSizeFixedWidth)
        {
            Width=ScreenWidthChars*CharWidthPx;
            LeftEdge=TextAreaWidthPx/2-Width/2;
            if(LeftEdge<0)
                LeftEdge=0;

            if(!Settings->CenterTextInWindow)
                LeftEdge=0;
        }

        if(Settings->TermSizeFixedHeight)
        {
            Height=ScreenHeightChars*CharHeightPx;
            TopEdge=TextAreaHeightPx/2-Height/2;
            if(TopEdge<0)
                TopEdge=0;

            if(!Settings->CenterTextInWindow)
                TopEdge=0;
        }

        UITC_SetClippingWindow(UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
                LeftEdge,TopEdge,Width,Height);
    }

    /* We need to invalidate any markers that are off the top of the screen */
    InvalidateOutOfRangeMarks();

    if(RedrawNeeded)
    {
        RedrawFullScreen();
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::RethinkScrollBars
 *
 * SYNOPSIS:
 *    void DisplayText::RethinkScrollBars(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rethinks the size of the scroll bars.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::RethinkScrollBars(void)
{
    int MaxLength;
    int ScreenWidth;
    int ScreenWidthPx;
    t_UIScrollBarCtrl *HorzScroll;
    t_UIScrollBarCtrl *VertScroll;

    if(TextDisplayCtrl==NULL)
        return;

    /* Hozr */
    HorzScroll=UITC_GetHorzSlider(UITC_GetTextDisplayPrimaryColumn(
            TextDisplayCtrl));
    ScreenWidthPx=ScreenWidthChars*CharWidthPx;

    MaxLength=LongestLinePx;
    if(MaxLength<ScreenWidthChars*CharWidthPx)
        MaxLength=ScreenWidthChars*CharWidthPx;

    ScreenWidth=TextAreaWidthPx;
    if(Settings->TermSizeFixedWidth && TextAreaWidthPx>ScreenWidthPx)
        ScreenWidth=ScreenWidthPx;

    UISetScrollBarPageSizeAndMax(HorzScroll,ScreenWidth,MaxLength);

    WindowXOffsetPx=UIGetScrollBarPos(HorzScroll);

    UISetScrollBarStepSize(HorzScroll,CharWidthPx);

    /* Vert */
    VertScroll=UITC_GetVertSlider(TextDisplayCtrl);

    UISetScrollBarPageSizeAndMax(VertScroll,WindowHeightChars,LinesCount);
    UISetScrollBarPos(VertScroll,TopLineY);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::MoveCursor
 *
 * SYNOPSIS:
 *    void DisplayText::MoveCursor(unsigned int x,unsigned y,
 *          bool CursorXPxPrecaled);
 *
 * PARAMETERS:
 *    x [I] -- The new position of the cursor in the X direction
 *    y [I] -- The new position of the cursor in the Y direction
 *    CursorXPxPrecaled [I] -- If this is true then you have precalculated
 *          CursorXPx and this function won't.
 *
 * FUNCTION:
 *    This function in an internal function for moving the cursor.  It keeps
 *    the cursor in the window area.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::MoveCursor(unsigned int x,unsigned y,bool CursorXPxPrecaled)
{
    int UseX;
    int UseY;
    bool RedrawNeeded;

    UseX=x;
    UseY=y;

    if(UseX<0)
        UseX=0;
    if(UseY<0)
        UseY=0;

    if(UseX>=ScreenWidthChars)
        UseX=ScreenWidthChars-1;
    if(UseY>=ScreenHeightChars)
        UseY=ScreenHeightChars-1;

    CursorX=UseX;
    CursorY=UseY;

    /* Track the max X,Y we have moved the cursor */
    if(CursorY!=LastCursorY)
    {
        LastCursorY=CursorY;
        if(CursorY>MaxCursorY)
            MaxCursorY=CursorY;
        MaxCursorX=CursorX;   // Reset the cursor X max (we are on a new line)
    }
    if(CursorX>MaxCursorX)
        MaxCursorX=CursorX;

    /* Recalc the X px (if needed) */
    if(!CursorXPxPrecaled)
        CursorXPx=CalcCursorXPx();

    if(TextDisplayCtrl==NULL)
    {
        CursorXPx=-1;
        return;
    }

    RedrawNeeded=false;
    if(RethinkInsertFrag())
        RedrawNeeded=true;

    UITC_SetCursorPos(TextDisplayCtrl,CursorX,CalcCorrectedCursorPos());

    /* This line will make the screen always scroll to the bottom */
    /* ScrollScreen2MakeCursorVisible(); */

    RethinkCursorHidden();

    if(RedrawNeeded)
        RedrawFullScreen();

    InvalidateOutOfRangeMarks();
}

/*******************************************************************************
 * NAME:
 *    DisplayText::CalcCorrectedCursorPos()
 *
 * SYNOPSIS:
 *    int DisplayText::CalcCorrectedCursorPos(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function calculates where we are drawing the cursor.
 *    This function takes 'CursorY' and adjusts for the scroll bar and
 *    top line.
 *
 * RETURNS:
 *    The offset for the cursor from the 'TopLine'.
 *    If the cursor should be hidden then this function returns 0 (0 is also
 *    a valid position, you need to use a different function to figure out
 *    if the custor should be hidden).
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int DisplayText::CalcCorrectedCursorPos(void)
{
    int CursorGlobalY;
    int BottomLineY;
    int Y;

    if(LinesCount<WindowHeightChars)
    {
        /* We don't have enough chars so we always return the cursor pos */
        return CursorY;
    }

    if(LinesCount<ScreenHeightChars)
        CursorGlobalY=CursorY;
    else
        CursorGlobalY=LinesCount-ScreenHeightChars+CursorY;
    BottomLineY=TopLineY+WindowHeightChars;

    if(CursorGlobalY>=TopLineY && CursorGlobalY<BottomLineY)
    {
        Y=CursorGlobalY-TopLineY;
    }
    else
    {
        Y=0;
    }

    return Y;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::DoBackspace
 *
 * SYNOPSIS:
 *    void DisplayText::DoBackspace(void);
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
void DisplayText::DoBackspace(void)
{
    int NewPos;
    int NewCursorY;
    struct TextPointMarker *Marker;
    uint_fast32_t CursorOffset;
    uint_fast32_t MarkOffset;

    /* Move the cursor to the prev pos */
    NewCursorY=CursorY;
    NewPos=CursorX-1;
    if(NewPos<0)
    {
        /* Ok, we need to wrap to the prev line */
        NewCursorY--;
        if(NewCursorY<0)
        {
            NewCursorY=0;
            NewPos=0;
        }
        else
        {
            NewPos=ScreenWidthChars-1;

            if(Settings->DestructiveBackspace)
                ClearArea(NewPos,NewCursorY,~0,NewCursorY+1);
        }
    }

    MoveCursor(NewPos,NewCursorY,false);
    if(Settings->DestructiveBackspace)
        WriteCharWithOptions((uint8_t *)" ",false);

    /* We invalid the marks if we be less than the mark */
    CursorOffset=NewCursorY*ScreenWidthChars+NewPos;
    for(Marker=MarkerList;Marker!=NULL;Marker=Marker->Next)
    {
        if(Marker->Valid)
        {
            MarkOffset=Marker->Y*ScreenWidthChars+Marker->X;

            /* If we have moved to less than the mark invalid this marker */
            if(CursorOffset<MarkOffset)
                Marker->Valid=false;
        }
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::DoReturn
 *
 * SYNOPSIS:
 *    void DisplayText::DoReturn(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function handles the cartridge return.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::DoReturn(void)
{
    int y;
    i_TextLines PrevLine;
    int GlobalY;
    int BottomLineY;

    LastSeenCR=true;
    if(ActiveLine!=NULL)
    {
        ActiveLine->EOL=e_DTEOL_Hard;
        if(LastSeenLF)
        {
            /* This effects the line above (because we moved down 1 line) */
            PrevLine=ScreenFirstLine;
            for(y=0;y<ActiveLineY-1 && PrevLine!=Lines.end();y++)
                PrevLine++;

            if(PrevLine!=Lines.end())
            {
                PrevLine->EOLGuess=e_DTEOLGuess_LFCR;
                LastSeenCR=false;   // We just consumed the CR
            }

            if(LinesCount<ScreenHeightChars)
                GlobalY=y;
            else
                GlobalY=LinesCount-ScreenHeightChars+y;
            BottomLineY=TopLineY+WindowHeightChars;

            if(GlobalY>=TopLineY && GlobalY<BottomLineY &&
                    PrevLine!=Lines.end())
            {
                /* We need to redraw this line */
                DrawLine(GlobalY,y,&*PrevLine);
            }
        }
        else
        {
            ActiveLine->EOLGuess=e_DTEOLGuess_CR;
        }
    }

    LastSeenLF=false;

    /* Redraw any changes to the current line before we move on */
    RedrawActiveLine();

    MoveCursor(0,CursorY,false);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::DoLineFeed
 *
 * SYNOPSIS:
 *    void DisplayText::DoLineFeed(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function handles the cartridge return.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::DoLineFeed(void)
{
    int NewCursorY;

    LastSeenLF=true;
    if(ActiveLine!=NULL)
    {
        ActiveLine->EOL=e_DTEOL_Hard;
        if(LastSeenCR)
        {
            ActiveLine->EOLGuess=e_DTEOLGuess_CRLF;
            LastSeenLF=false;   // We just consumed the LF
        }
        else
        {
            ActiveLine->EOLGuess=e_DTEOLGuess_LF;
        }
    }
    LastSeenCR=false;

    /* Redraw any changes to the current line before we move on */
    RedrawActiveLine();

    NewCursorY=CursorY;

    MoveToNextLine(NewCursorY);

    MoveCursor(CursorX,NewCursorY,false);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::GetLineEndSize
 *
 * SYNOPSIS:
 *    int DisplayText::GetLineEndSize(struct TextLine *Line);
 *
 * PARAMETERS:
 *    Line [I] -- The line to get the size of the line of for
 *
 * FUNCTION:
 *    This function gets the size (in pixels) of the end of line symbol.
 *    If end of lines are hidden then this function returns 0.
 *
 * RETURNS:
 *    The number of pixels that the end of line symbol will take for this
 *    line.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int DisplayText::GetLineEndSize(struct TextLine *Line)
{
    struct TextCanvasFrag DisplayFrag;

    if(!ShowEndOfLines)
        return 0;

    /* DEBUG PAUL: This need to look up the type of line end for this line
       and then figure that one out */
    DisplayFrag.FragType=e_TextCanvasFrag_SoftRet;
    DisplayFrag.Text="";
    if(Line->Frags.empty())
        DisplayFrag.Styling=CurrentStyle;
    else
        DisplayFrag.Styling=Line->Frags.back().Styling;
    DisplayFrag.Value=0;
    DisplayFrag.Data=NULL;

    return UITC_GetFragWidth(UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
            &DisplayFrag);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ScrollScreenByXLines
 *
 * SYNOPSIS:
 *    void DisplayText::ScrollScreenByXLines(int Lines2Scroll);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function scrolls the lines by 'Lines2Scroll' lines.  It adds new
 *    lines to the 'Lines'.  It also removed old lines.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::ScrollScreenByXLines(int Lines2Scroll)
{
    struct TextLine BlankLine;
    i_TextLines CurLine;
    int y;
    int TotalLinesBeforeAdjust;
    int LinesScrolled;
    struct TextPointMarker *Marker;

    try
    {
        TotalLinesBeforeAdjust=LinesCount;

        BlankLine.LineBackgroundColor=CurrentStyle.BGColor;
        BlankLine.LineWidthPx=0;
        BlankLine.EOL=e_DTEOL_Hard;
        BlankLine.EOLGuess=e_DTEOLGuess_Unknown;

        for(LinesScrolled=0;LinesScrolled<Lines2Scroll;LinesScrolled++)
        {
            Lines.push_back(BlankLine);
            LinesCount++;

            if(LinesCount>ScreenHeightChars)
            {
                /* Move the screens top line down 1 */
                ScreenFirstLine++;
            }

            /* Remove any extra lines */
            while(LinesCount>(int)(Settings->ScrollBufferLines+
                    ScreenHeightChars))
            {
                if(TopLine==Lines.begin())
                {
                    /* We are about to remove this line so we need to move
                       top line as well */
                    TopLine++;
                    TopLineY++;
                }

                /* We are removing the oldest line so we need to move TopLineY
                   up */
                TopLineY--;

                /* Adjust the selection (if there is one) */
                if(SelectionActive)
                {
                    if(Selection_Y!=Selection_AnchorY ||
                            Selection_X!=Selection_AnchorX)
                    {
                        /* Ok, we have a valid selection, adjust it */
                        Selection_Y--;
                        Selection_AnchorY--;
                        if(Selection_Y<0 && Selection_AnchorY<0)
                        {
                            /* Ok, we are killing the selection */
                            SelectionActive=false;
                        }
                        if(Selection_Y<0)
                            Selection_Y=0;
                        if(Selection_AnchorY<0)
                            Selection_AnchorY=0;
                    }
                }

                /* Adjust all the markers */
                for(Marker=MarkerList;Marker!=NULL;Marker=Marker->Next)
                {
                    if(Marker->Valid)
                    {
                        Marker->Y--;
                        if(Marker->Y<0)
                            Marker->Y=0;
                    }
                }

                Lines.pop_front();
                LinesCount--;
            }
        }

        /* We go from the bottom of 'Lines' to the 'CursorY' pos (inverted) */
        CurLine=Lines.end();
        CurLine--;
        y=ScreenHeightChars-CursorY;
        while(CurLine!=Lines.begin())
        {
            y--;
            if(y==0)
                break;
            CurLine--;
        }

        ActiveLine=&*CurLine;

        RedrawFullScreen();

        if(TotalLinesBeforeAdjust!=LinesCount)
        {
            /* We had to adjust the total number of lines, rethink the
               scroll bars */
            RethinkScrollBars();
        }

        InvalidateOutOfRangeMarks();
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::CursorLineVisible
 *
 * SYNOPSIS:
 *    bool DisplayText::CursorLineVisible(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function check to see if the cursor line is visible or not.
 *
 * RETURNS:
 *    true -- Cursor line visible
 *    false -- The cursor is hidden
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayText::CursorLineVisible(void)
{
    int CursorGlobalY;
    int BottomLineY;

    if(LinesCount<WindowHeightChars)
    {
        /* Must be visible because all lines are */
        return true;
    }

    if(LinesCount<ScreenHeightChars)
        CursorGlobalY=CursorY;
    else
        CursorGlobalY=LinesCount-ScreenHeightChars+CursorY;
    BottomLineY=TopLineY+WindowHeightChars;

    if(CursorGlobalY>=TopLineY && CursorGlobalY<BottomLineY)
    {
        /* We can see the cursor */
        return true;
    }

    return false;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::RethinkCursorHidden
 *
 * SYNOPSIS:
 *    void DisplayText::RethinkCursorHidden(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rethinks if we should be showing the cursor or not
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::RethinkCursorHidden(void)
{
    bool IsCursorVisible;

    if(TextDisplayCtrl==NULL)
        return;

    IsCursorVisible=CursorLineVisible();

    if(LastCursorVisible!=IsCursorVisible || CursorStyle!=LastCursorStyle)
    {
        /* There has been a cursor visibility change */
        if(IsCursorVisible)
            UITC_SetCursorStyle(TextDisplayCtrl,CursorStyle);
        else
            UITC_SetCursorStyle(TextDisplayCtrl,e_TextCursorStyle_Hidden);

        LastCursorVisible=IsCursorVisible;
        LastCursorStyle=CursorStyle;
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::MoveToNextLine
 *
 * SYNOPSIS:
 *    void DisplayText::MoveToNextLine(int &NewCursorY);
 *
 * PARAMETERS:
 *    NewCursorY [I/O] -- The new CursorY you are going move from to
 *
 * FUNCTION:
 *    This function moves to the next line and scrolls the screen if needed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::MoveToNextLine(int &NewCursorY)
{
    int CursorGlobalY;
    int BottomLineY;

    NewCursorY++;

    if(LinesCount<ScreenHeightChars)
        CursorGlobalY=CursorY;
    else
        CursorGlobalY=LinesCount-ScreenHeightChars+CursorY;
    BottomLineY=TopLineY+WindowHeightChars-1;

    if(CursorGlobalY==BottomLineY)
    {
        if(TopLineY==LinesCount-WindowHeightChars)
        {
            /* Move 'TopLine' down 1 so it stays at the top */
            TopLine++;
            TopLineY++;
        }
    }

    if(NewCursorY>=ScreenHeightChars)
    {
        NewCursorY=ScreenHeightChars-1;
        ScrollScreenByXLines(1);
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ScrollBarAtBottom
 *
 * SYNOPSIS:
 *    bool DisplayText::ScrollBarAtBottom(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function figures out if the vert scroll bar is at the bottom (can't
 *    scroll any farther down).
 *
 * RETURNS:
 *    true -- Scroll bar is at the bottom
 *    false -- User could scroll down more
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayText::ScrollBarAtBottom(void)
{
    t_UIScrollBarCtrl *VertScroll;
    int CurrentPos;

    if(TextDisplayCtrl==NULL)
        return false;

    VertScroll=UITC_GetVertSlider(TextDisplayCtrl);

    CurrentPos=UIGetScrollBarPos(VertScroll);

    if(CurrentPos>=LinesCount-WindowHeightChars)
        return true;
    return false;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::GetNormalizedSelection
 *
 * SYNOPSIS:
 *    void DisplayText::GetNormalizedSelection(int &X1,int &Y1,int &X2,int &Y2);
 *
 * PARAMETERS:
 *    X1 [O] -- The Left edge of the selection
 *    Y1 [O] -- The Top edge of the selection
 *    X2 [O] -- The Right edge of the selection
 *    Y2 [O] -- The Bottom edge of the selection
 *
 * FUNCTION:
 *    Normalize the selection values so that X1,Y1 are always smaller than
 *    X2,Y2.  The Anchor is wherever the selection started, and X,Y can be
 *    before that or after.  This function returns things so they always the
 *    same.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::GetNormalizedSelection(int &X1,int &Y1,int &X2,int &Y2)
{
    int Tmp;

    /* See if we have to deal with the selection */
    if(!SelectionActive)
        return;

    X1=Selection_X;
    Y1=Selection_Y;
    X2=Selection_AnchorX;
    Y2=Selection_AnchorY;

    if(Y2<Y1)
    {
        Tmp=Y2;
        Y2=Y1;
        Y1=Tmp;

        Tmp=X2;
        X2=X1;
        X1=Tmp;
    }
    else if(Y1==Y2)
    {
        /* We also have to do the X's */
        if(X2<X1)
        {
            Tmp=X2;
            X2=X1;
            X1=Tmp;
        }
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ConvertScreenXY2Chars
 *
 * SYNOPSIS:
 *    bool DisplayText::ConvertScreenXY2Chars(int x,int y,int *CharX,
 *          int *CharY);
 *
 * PARAMETERS:
 *    x [I] -- The x pos on the screen
 *    y [I] -- The y pos on the screen
 *    CharX [O] -- The 'x' converted to the char offset on that line
 *    CharY [O] -- The 'y' converted to the char offset from 'Lines'
 *
 * FUNCTION:
 *    This function takes a screen x,y and converts it to what char and line
 *    that is on at this point on the screen.  The Y is a simple div by the
 *    char height.  The X has to find the line in 'Lines' and count over the
 *    number of chars.  This is because each char in a line might be different
 *    widths.
 *
 * RETURNS:
 *    true -- The x,y is in the text
 *    false -- The x,y is out of bounds.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayText::ConvertScreenXY2Chars(int x,int y,int *CharX,int *CharY)
{
    int UseX,UseY;
    int CalX,CalY;
    int r;
    i_TextLines Line;
    bool RetValue;
    i_TextLineFrags CurFrag;
    int StartPx;
    struct TextCanvasFrag DisplayFrag;
    string::iterator StartPos;
    string::iterator EndPos;
    int FragWidthPx;

    *CharX=0;
    *CharY=0;

    if(TextDisplayCtrl==NULL)
        return false;

    UseX=x+WindowXOffsetPx;
    UseY=y;

    RetValue=true;
    if(x<0 || y<0)
        RetValue=false;

    if(x>=TextAreaWidthPx || y>=TextAreaHeightPx)
        RetValue=false;

    if(UseX<0)
        UseX=0;
    if(UseY<0)
        UseY=0;

    if(CharHeightPx==0)
        CalY=0;
    else
        CalY=UseY/CharHeightPx;
    CalY+=TopLineY;

    if(CalY>=LinesCount)
    {
        CalY=LinesCount-1;
        RetValue=false;
    }

    /* Find this line */
    for(r=0,Line=TopLine;r+TopLineY<CalY && Line!=Lines.end();Line++,r++)
        ;

    CalX=0;
    if(Line!=Lines.end())
    {
        /* We need to walk this line looking for the px point */
        /* First find the frag that 'x' is in */
        CalX=0;
        StartPx=0;
        for(CurFrag=Line->Frags.begin();CurFrag!=Line->Frags.end();CurFrag++)
        {
            if(UseX>=StartPx && UseX<StartPx+CurFrag->WidthPx)
                break;

            if(CurFrag->FragType==e_TextCanvasFrag_String)
            {
                CalX+=utf8::unchecked::distance(CurFrag->Text.begin(),
                        CurFrag->Text.end());
            }
            StartPx+=CurFrag->WidthPx;
        }

        /* Ok, 'x' is somewhere in this frag */
        if(CurFrag!=Line->Frags.end() &&
                CurFrag->FragType==e_TextCanvasFrag_String)
        {
            DisplayFrag.FragType=CurFrag->FragType;
            DisplayFrag.Text=CurFrag->Text.c_str();
            DisplayFrag.Styling=CurFrag->Styling;
            DisplayFrag.Value=CurFrag->Value;
            DisplayFrag.Data=CurFrag->Data;

            StartPos=CurFrag->Text.begin();
            while(StartPos!=CurFrag->Text.end())
            {
                /* Move up by 1 char */
                EndPos=StartPos;
                utf8::unchecked::next(EndPos);

                TmpStr.assign(StartPos,EndPos);
                DisplayFrag.Text=TmpStr.c_str();
                FragWidthPx=UITC_GetFragWidth(
                        UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
                        &DisplayFrag);

                if(UseX<StartPx+FragWidthPx)
                    break;

                CalX++;
                StartPx+=FragWidthPx;
                StartPos=EndPos;
            }
        }
    }

    *CharX=CalX;
    *CharY=CalY;

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::GetSelectionString
 *
 * SYNOPSIS:
 *    bool DisplayText::GetSelectionString(std::string &Clip);
 *
 * PARAMETERS:
 *    Clip [O] -- The text from the selection
 *
 * FUNCTION:
 *    This function gets a copy of the selection text.
 *
 * RETURNS:
 *    true -- The selection was valid and 'Clip' has been set
 *    false --- There was no selection and 'Clip' has been set to ""
 *
 * SEE ALSO:
 *    IsThereASelection(), SelectAll(), ClearSelection()
 ******************************************************************************/
bool DisplayText::GetSelectionString(std::string &Clip)
{
    int SelX1;
    int SelY1;
    int SelX2;
    int SelY2;

    Clip="";

    /* Return false if there is no selection */
    if(!IsThereASelection())
        return false;

    GetNormalizedSelection(SelX1,SelY1,SelX2,SelY2);

    return GetStringBetweenPoints(SelX1,SelY1,SelX2,SelY2,Clip);

//    struct DTPoint Start;
//    struct DTPoint End;
//    i_TextLines CurLine;
//    i_TextLineFrags CurFrag;
//    string::iterator StartOfStr;
//    string::iterator EndOfStr;
//
//    Clip="";
//
//    if(!FindPointsOfSelection(Start,End))
//        return false;
//
//    if(Start.Line==End.Line && Start.Frag==End.Frag)
//    {
//        /* On the same line and frag */
//        Clip.assign(Start.Frag->Text,Start.StrPos,End.StrPos-Start.StrPos);
//    }
//    else
//    {
//        CurLine=Start.Line;
//        CurFrag=Start.Frag;
//
//        /* Take from Start to the end of frag */
//        if(Start.Frag!=Start.Line->Frags.end())
//        {
//            Clip.assign(Start.Frag->Text,Start.StrPos,string::npos);
//            CurFrag++;
//        }
//        else
//        {
//            /* The selection was past the end of the line (or it was a
//               blank line) so we add a blank line and then continue the
//               copy. */
//            Clip="\n";
//            CurLine++;  // Move to the end line
//            CurFrag=CurLine->Frags.begin(); // And the first fragment
//        }
//
//        /* Now copy all the frag from here to end frag */
//        while(CurFrag!=End.Frag)
//        {
//            if(CurFrag==CurLine->Frags.end())
//            {
//                /* End of the line move to the next */
//                CurLine++;
//                if(CurLine==Lines.end())
//                {
//                    /* Unexpected end */
//                    return false;
//                }
//                CurFrag=CurLine->Frags.begin();
//                Clip.append("\n");
//                continue;
//            }
//
//            if(CurFrag!=CurLine->Frags.end() &&
//                    CurFrag->FragType==e_TextCanvasFrag_String)
//            {
//                Clip.append(CurFrag->Text);
//            }
//
//            CurFrag++;
//        }
//
//        /* Now take the last part of the string */
//        if(CurFrag!=CurLine->Frags.end() &&
//                CurFrag->FragType==e_TextCanvasFrag_String)
//        {
//            Clip.append(End.Frag->Text,0,End.StrPos);
//        }
//    }
//
//    return true;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::IsThereASelection
 *
 * SYNOPSIS:
 *    bool DisplayText::IsThereASelection(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function checks to see if there is selected text.
 *
 * RETURNS:
 *    true -- There is text that can be copied to the clip board
 *    false -- There is not selection (nothing to copy).
 *
 * SEE ALSO:
 *    GetSelectionString()
 ******************************************************************************/
bool DisplayText::IsThereASelection(void)
{
    if(!SelectionActive)
        return false;

    if(Selection_Y==Selection_AnchorY && Selection_X==Selection_AnchorX)
        return false;

    return true;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::SelectAll
 *
 * SYNOPSIS:
 *    void DisplayText::SelectAll(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function sets the selection to select everything.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DisplayText::GetSelectionString()
 ******************************************************************************/
void DisplayText::SelectAll(void)
{
    i_TextLineFrags CurFrag;
    i_TextLines Line;

    if(Lines.empty())
    {
        SelectionActive=false;
    }
    else
    {
        Line=Lines.end();
        Line--;

        Selection_AnchorX=0;
        for(CurFrag=Line->Frags.begin();CurFrag!=Line->Frags.end();CurFrag++)
        {
            if(CurFrag->FragType==e_TextCanvasFrag_String)
            {
                Selection_AnchorX+=utf8::unchecked::distance(CurFrag->
                        Text.begin(),CurFrag->Text.end());
            }
        }

        SelectionActive=true;

        Selection_Y=0;
        Selection_X=0;
        Selection_AnchorY=LinesCount-1;
    }

    RedrawFullScreen();

    SendEvent(e_DBEvent_SelectionChanged,NULL);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ClearSelection
 *
 * SYNOPSIS:
 *    void DisplayText::ClearSelection(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function sets the selection to nothing.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DisplayText::GetSelectionString()
 ******************************************************************************/
void DisplayText::ClearSelection(void)
{
    SelectionActive=false;
    Selection_X=0;
    Selection_Y=0;
    Selection_AnchorX=0;
    Selection_AnchorY=0;

    RedrawFullScreen();

    SendEvent(e_DBEvent_SelectionChanged,NULL);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ScrollScreen
 *
 * SYNOPSIS:
 *    void DisplayText::ScrollScreen(int dxpx,int dy);
 *
 * PARAMETERS:
 *    dxpx [I] -- The amount to scroll the screen in the x dir.  This is pixels
 *    dy [I] -- The amount to scroll the screen in the y dir.  This is chars
 *
 * FUNCTION:
 *    This function moves the scroll bar(s) by an amount.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::ScrollScreen(int dxpx,int dy)
{
    int ScreenWidth;
    int ScreenWidthPx;
    t_UIScrollBarCtrl *HorzScroll;
    t_UIScrollBarCtrl *VertScroll;
    int MaxPos;
    int MaxLength;
    int NewPos;

    if(TextDisplayCtrl==NULL)
        return;

    HorzScroll=UITC_GetHorzSlider(UITC_GetTextDisplayPrimaryColumn(
            TextDisplayCtrl));
    VertScroll=UITC_GetVertSlider(TextDisplayCtrl);

    /* Hozr */
    ScreenWidthPx=ScreenWidthChars*CharWidthPx;

    MaxLength=LongestLinePx;
    if(MaxLength<ScreenWidthChars*CharWidthPx)
        MaxLength=ScreenWidthChars*CharWidthPx;

    ScreenWidth=TextAreaWidthPx;
    if(Settings->TermSizeFixedWidth && TextAreaWidthPx>ScreenWidthPx)
        ScreenWidth=ScreenWidthPx;

    MaxPos=MaxLength-ScreenWidth;
    WindowXOffsetPx+=dxpx;
    if(WindowXOffsetPx<0)
        WindowXOffsetPx=0;
    if(WindowXOffsetPx>MaxPos)
        WindowXOffsetPx=MaxPos;

    UISetScrollBarPos(HorzScroll,WindowXOffsetPx);

    /* Vert */
    MaxPos=LinesCount-WindowHeightChars;
    NewPos=TopLineY+dy;
    if(NewPos>MaxPos)
        NewPos=MaxPos;
    if(NewPos<0)
        NewPos=0;

    UISetScrollBarPos(VertScroll,NewPos);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::DoScrollTimerTimeout
 *
 * SYNOPSIS:
 *    void DisplayText::DoScrollTimerTimeout(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when the scroll timer goes off.  It scroll the
 *    screen if needed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::DoScrollTimerTimeout(void)
{
    if(!SelectionActive)
    {
        /* Hu? this is for scrolling when doing a selection.  No selection
           means we shouldn't be called */
        UITimerStop(ScrollTimer);
        return;
    }

    ScrollScreen(AutoSelectionScrolldx*CharWidthPx,AutoSelectionScrolldy);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ScrollScreen2MakeCursorVisible
 *
 * SYNOPSIS:
 *    void DisplayText::ScrollScreen2MakeCursorVisible(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function scroll the screen to make the cursor visible.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::ScrollScreen2MakeCursorVisible(void)
{
    t_UIScrollBarCtrl *HorzScroll;
    int CursorGlobalY;
    int BottomLineY;
    int LastTopLineY;
    int LastWindowXOffsetPx;
    int CharUnderCursorPx;

    if(TextDisplayCtrl==NULL)
        return;

    LastTopLineY=TopLineY;
    LastWindowXOffsetPx=WindowXOffsetPx;

    /* Deal with Y */
    /* We can't see the cursor, make it visible */
    if(LinesCount<ScreenHeightChars)
        CursorGlobalY=CursorY;
    else
        CursorGlobalY=LinesCount-ScreenHeightChars+CursorY;

    /* Scroll us into view if the cursor is above the topline */
    while(CursorGlobalY<TopLineY)
    {
        TopLineY--;
        TopLine--;
        if(TopLine==Lines.begin())
        {
            /* Shouldn't happen */
            break;
        }
    }
    /* Scroll us into view if we the cursor is below the bottom of the screen */
    BottomLineY=TopLineY+WindowHeightChars;
    while(CursorGlobalY>=BottomLineY)
    {
        TopLineY++;
        TopLine++;
        if(TopLine==Lines.end())
        {
            /* Shouldn't have happended */
            TopLine--;
            TopLineY--;
            break;
        }
        BottomLineY=TopLineY+WindowHeightChars;
    }

    /* Deal with X */
    HorzScroll=UITC_GetHorzSlider(UITC_GetTextDisplayPrimaryColumn(
            TextDisplayCtrl));

    if(CursorXPx<WindowXOffsetPx)
    {
        WindowXOffsetPx=CursorXPx;
        UISetScrollBarPos(HorzScroll,WindowXOffsetPx);
    }

    CharUnderCursorPx=CharUnderCursorWidthPx();
    if(CursorXPx+CharUnderCursorPx>WindowXOffsetPx+TextAreaWidthPx)
    {
        WindowXOffsetPx=(CursorXPx+CharUnderCursorPx)-TextAreaWidthPx;
        UISetScrollBarPos(HorzScroll,WindowXOffsetPx);
    }

    if(LastTopLineY!=TopLineY || LastWindowXOffsetPx)
    {
        UITC_SetCursorPos(TextDisplayCtrl,CursorX,CalcCorrectedCursorPos());
        RethinkCursorHidden();
        RedrawFullScreen();
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::CalcCursorXPx
 *
 * SYNOPSIS:
 *    int DisplayText::CalcCursorXPx(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function takes the 'CursorX' position and walks 'ActiveLine'
 *    to find the px position of cursor X.
 *
 * RETURNS:
 *    The number of pixels that 'CursorX' is over.  It does not take
 *    'WindowXOffsetPx' into account.
 *
 * LIMITATIONS:
 *    This is slow.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int DisplayText::CalcCursorXPx(void)
{
    int CalX;
    i_TextLines Line;
    i_TextLineFrags CurFrag;
    int Pixels;
    struct TextCanvasFrag DisplayFrag;
    string::iterator StartPos;
    string::iterator EndPos;
    int Chars;

    if(TextDisplayCtrl==NULL || ActiveLine==NULL)
        return 0;

    /* We need to walk this line looking for the px point */
    CalX=0;
    Pixels=0;
    for(CurFrag=ActiveLine->Frags.begin();CurFrag!=ActiveLine->Frags.end();
            CurFrag++)
    {
        Chars=utf8::unchecked::distance(CurFrag->Text.begin(),
                CurFrag->Text.end());
        if(CursorX>=CalX && CursorX<CalX+Chars)
            break;

        if(CurFrag->FragType==e_TextCanvasFrag_String)
            CalX+=Chars;
        Pixels+=CurFrag->WidthPx;
    }

    /* Ok, 'CursorX' is somewhere in this frag */
    if(CurFrag!=ActiveLine->Frags.end() &&
            CurFrag->FragType==e_TextCanvasFrag_String)
    {
        DisplayFrag.FragType=e_TextCanvasFrag_String;
        DisplayFrag.Text=CurFrag->Text.c_str();
        DisplayFrag.Styling=CurFrag->Styling;
        DisplayFrag.Value=CurFrag->Value;
        DisplayFrag.Data=CurFrag->Data;

        StartPos=CurFrag->Text.begin();
        EndPos=StartPos;
        while(StartPos!=CurFrag->Text.end())
        {
            if(CalX>=CursorX)
            {
                /* Ok, add in the first part of this fragment */
                TmpStr.assign(StartPos,EndPos);
                DisplayFrag.Text=TmpStr.c_str();
                Pixels+=UITC_GetFragWidth(
                        UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
                        &DisplayFrag);
                break;
            }

            /* Move up by 1 char */
            utf8::unchecked::next(EndPos);

            CalX++;
        }
    }

    if(CalX<CursorX)
    {
        /* Cursor in virtual space */
        Pixels+=(CursorX-CalX)*CharWidthPx;
    }

    return Pixels;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::CharUnderCursorWidthPx
 *
 * SYNOPSIS:
 *    int DisplayText::CharUnderCursorWidthPx(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function figures out the width of the char under the cursor.
 *
 * RETURNS:
 *    The width of the char under the cursor.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int DisplayText::CharUnderCursorWidthPx(void)
{
    int CalX;
    i_TextLineFrags CurFrag;
    struct TextCanvasFrag DisplayFrag;
    string::iterator StartPos;
    string::iterator EndPos;
    int Chars;
    int WidthPx;

    if(TextDisplayCtrl==NULL)
        return 0;

    WidthPx=0;

    /* We need to walk this line looking for the px point */
    CalX=0;
    for(CurFrag=ActiveLine->Frags.begin();CurFrag!=ActiveLine->Frags.end();
            CurFrag++)
    {
        Chars=utf8::unchecked::distance(CurFrag->Text.begin(),
                CurFrag->Text.end());
        if(CursorX>=CalX && CursorX<CalX+Chars)
            break;

        if(CurFrag->FragType==e_TextCanvasFrag_String)
            CalX+=Chars;
    }

    /* Ok, 'CursorX' is somewhere in this frag */
    if(CurFrag!=ActiveLine->Frags.end() &&
            CurFrag->FragType==e_TextCanvasFrag_String)
    {
        DisplayFrag.FragType=e_TextCanvasFrag_String;
        DisplayFrag.Text=CurFrag->Text.c_str();
        DisplayFrag.Styling=CurFrag->Styling;
        DisplayFrag.Value=CurFrag->Value;
        DisplayFrag.Data=CurFrag->Data;

        StartPos=CurFrag->Text.begin();
        EndPos=StartPos;
        while(StartPos!=CurFrag->Text.end())
        {
            /* Move up by 1 char */
            utf8::unchecked::next(EndPos);

            if(CalX>=CursorX)
            {
                /* Ok, we found the char */
                TmpStr.assign(StartPos,EndPos);
                DisplayFrag.Text=TmpStr.c_str();
                WidthPx=UITC_GetFragWidth(
                        UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
                        &DisplayFrag);
                break;
            }

            StartPos=EndPos;
            CalX++;
        }
    }

    if(CalX<CursorX || CurFrag==ActiveLine->Frags.end())
    {
        /* Cursor in virtual space */
        WidthPx=CharWidthPx;
    }

    return WidthPx;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::SetOverrideMessage
 *
 * SYNOPSIS:
 *    void DisplayText::SetOverrideMessage(const char *Msg);
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
void DisplayText::SetOverrideMessage(const char *Msg)
{
    if(TextDisplayCtrl==NULL)
        return;

    if(Msg==NULL)
        UITC_SetOverrideMsg(TextDisplayCtrl,Msg,false);
    else
        UITC_SetOverrideMsg(TextDisplayCtrl,Msg,true);

    if(TextDisplayCtrl!=NULL)
        UITC_RedrawScreen(UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl));
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ClearScreen
 *
 * SYNOPSIS:
 *    void DisplayText::ClearScreen(e_ScreenClearType Type);
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
void DisplayText::ClearScreen(e_ScreenClearType Type)
{
    i_TextLines CurLine;
    i_TextLines LastLine;
    int Lines2Scroll;
    i_TextLineFrags NewFrag;
    struct TextLineFrag NewHRFrag;

    try
    {
        switch(Type)
        {
            case e_ScreenClear_Clear:
            case e_ScreenClearMAX:
            default:
                /* Just set all the lines to blank */
                for(CurLine=ScreenFirstLine;CurLine!=Lines.end();CurLine++)
                {
                    CurLine->LineBackgroundColor=Settings->
                            DefaultColors[e_DefaultColors_BG];
                    CurLine->LineWidthPx=0;
                    CurLine->EOL=e_DTEOL_Hard;
                    CurLine->Frags.clear();
                    CurLine->EOLGuess=e_DTEOLGuess_Unknown;
                }
            break;
            case e_ScreenClear_Scroll:
            case e_ScreenClear_ScrollWithHR:
                Lines2Scroll=0;
                if(ScreenFirstLine!=Lines.end())
                {
                    /* Ok, scan for blank lines, starting at the bottom */
                    LastLine=Lines.end();
                    LastLine--;  // Goto the last line
                    while(LastLine!=ScreenFirstLine)
                    {
                        if(!IsLineBlank(LastLine))
                            break;

                        LastLine--;
                    }

                    /* Check if the last line is blank or not */
                    if(!IsLineBlank(LastLine))
                        LastLine++;

                    /* Ok, we need to scroll everything from 'ScreenFirstLine'
                       to 'LastLine' into the back buffer */
                    for(CurLine=ScreenFirstLine;CurLine!=LastLine;CurLine++)
                        Lines2Scroll++;

                    if(Lines2Scroll>0)
                    {
                        PadOutScreenWithBlankLines();
                        ScrollScreenByXLines(Lines2Scroll);
                    }

                    if(Type==e_ScreenClear_ScrollWithHR && Lines2Scroll>0)
                    {
                        if(ActiveLine!=NULL)
                        {
                            /* Move the cursor to the top/left before we insert
                               the HR */
                            MoveCursor(0,0,false);

                            /* Ok, we also need to a HR line to the end of the back
                               buffer */
                            NewHRFrag.FragType=e_TextCanvasFrag_HR;
                            NewHRFrag.Text="";
                            NewHRFrag.Styling.FGColor=CurrentStyle.FGColor;
                            NewHRFrag.Styling.BGColor=CurrentStyle.BGColor;
                            NewHRFrag.Styling.ULineColor=CurrentStyle.FGColor;
                            NewHRFrag.Styling.Attribs=0;
                            NewHRFrag.WidthPx=0;
                            NewHRFrag.Value=0;
                            NewHRFrag.Data=NULL;

                            ActiveLine->Frags.clear();
                            ActiveLine->Frags.push_back(NewHRFrag);
                            ActiveLine->EOL=e_DTEOL_Hard;
                            ActiveLine->LineBackgroundColor=
                                    CurrentStyle.BGColor;
                            ActiveLine->EOLGuess=e_DTEOLGuess_Unknown;
                            RethinkFragWidth(ActiveLine->Frags.begin());

                            InsertFrag=ActiveLine->Frags.end();
                            InsertPos=-1;

                            ScrollScreenByXLines(1);
                        }
                    }
                }
            break;
            case e_ScreenClear_ScrollAll:
                /* Make sure we have enough lines (we need to push a full screen
                   into the back buffer (which we can only do if we have a full
                   screen of lines to start with) */
                PadOutScreenWithBlankLines();
                ScrollScreenByXLines(ScreenHeightChars);
            break;
        }

        /* Scroll the window to be at the bottom */
        WindowXOffsetPx=0;
        TopLine=ScreenFirstLine;
        if(LinesCount>=ScreenHeightChars)
            TopLineY=LinesCount-ScreenHeightChars;
        else
            TopLineY=0;

        RethinkScrollBars();

        if(TextDisplayCtrl!=NULL)
        {
            UITC_SetXOffset(UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
                    WindowXOffsetPx);
        }

        /* Now we have to move the cursor to the top/left */
        /* Reset the max cursor */
        MaxCursorY=0;
        MaxCursorX=0;
        MoveCursor(0,0,false);

        /* Mark all markers as invalid */
        InvalidateAllMarks();

        RedrawFullScreen();
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ClearScrollBackBuffer
 *
 * SYNOPSIS:
 *    void DisplayText::ClearScrollBackBuffer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clears the search area.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::ClearScrollBackBuffer(void)
{
    i_TextLines Bottom;

    Bottom=ScreenFirstLine;

    if(Bottom==Lines.end() || Bottom==Lines.begin())
        return;

    Lines.erase(Lines.begin(),Bottom);
    LinesCount=Lines.size();

    TopLine=Lines.begin();
    TopLineY=0;
    SelectionActive=false;

    RethinkLineLengths();
    RethinkScrollBars();
    RethinkCursorHidden();
    UITC_SetCursorPos(TextDisplayCtrl,CursorX,CalcCorrectedCursorPos());
    RedrawFullScreen();
}

/*******************************************************************************
 * NAME:
 *    DisplayText::InsertHorizontalRule
 *
 * SYNOPSIS:
 *    void DisplayText::InsertHorizontalRule(void);
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
void DisplayText::InsertHorizontalRule(void)
{
    i_TextLines CurLine;
    struct TextLineFrag NewHRFrag;
    int NewCursorY;

    try
    {
        /* We replace the current line with a HR and then move the cursor to the
           next line */
        NewHRFrag.FragType=e_TextCanvasFrag_HR;
        NewHRFrag.Text="";
        NewHRFrag.Styling.FGColor=CurrentStyle.FGColor;
        NewHRFrag.Styling.BGColor=CurrentStyle.BGColor;
        NewHRFrag.Styling.ULineColor=CurrentStyle.FGColor;
        NewHRFrag.Styling.Attribs=0;
        NewHRFrag.WidthPx=0;
        NewHRFrag.Value=0;
        NewHRFrag.Data=NULL;

        if(Settings->OverrideHR)
            NewHRFrag.Styling.FGColor=Settings->HorizontalRuleColor;

        CurLine=GetActiveLineIterator();
        if(CurLine!=Lines.end() && !IsLineBlank(CurLine))
        {
            /* We are not on a blank line, move to the next line */
            /* Move to the start of the next line */
            NewCursorY=CursorY;
            MoveToNextLine(NewCursorY);
            MoveCursor(0,NewCursorY,false);
        }

        ActiveLine->Frags.clear();
        ActiveLine->Frags.push_back(NewHRFrag);
        ActiveLine->EOL=e_DTEOL_Hard;
        ActiveLine->LineBackgroundColor=CurrentStyle.BGColor;
        ActiveLine->EOLGuess=e_DTEOLGuess_Unknown;
        RethinkFragWidth(ActiveLine->Frags.begin());

        InsertFrag=ActiveLine->Frags.end();
        InsertPos=-1;

        RedrawActiveLine();

        /* Move to the start of the next line */
        NewCursorY=CursorY;
        MoveToNextLine(NewCursorY);
        MoveCursor(0,NewCursorY,false);
        InvalidateAllMarks();
    }
    catch(...)
    {
        
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::GetActiveLineIterator
 *
 * SYNOPSIS:
 *    i_TextLines DisplayText::GetActiveLineIterator(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets a line iterator that is the same as 'ActiveLine'
 *
 * RETURNS:
 *    An iterator to ActiveLine or Lines.end() if it is not valid.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
i_TextLines DisplayText::GetActiveLineIterator(void)
{
    i_TextLines CurLine;
    int y;
    if(ActiveLine==NULL)
        return Lines.end();

    for(y=0,CurLine=ScreenFirstLine;CurLine!=Lines.end() && y<ActiveLineY;
            CurLine++,y++)
    {
    }
    return CurLine;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ResetTerm
 *
 * SYNOPSIS:
 *    void DisplayText::ResetTerm(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function does an reset of the term.  It clears the scroll back
 *    buffer, clears the screen, and resets the colors.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::ResetTerm(void)
{
    struct TextLineFrag NewHRFrag;

    try
    {
        /* Current Style */
        CurrentStyle.FGColor=Settings->DefaultColors[e_DefaultColors_FG];
        CurrentStyle.BGColor=Settings->DefaultColors[e_DefaultColors_BG];
        CurrentStyle.Attribs=0;
        CurrentStyle.ULineColor=CurrentStyle.FGColor;

        ClearScrollBackBuffer();
        ClearScreen(e_ScreenClear_Clear);

        SelectionActive=false;
        Selection_X=0;
        Selection_Y=0;
        Selection_AnchorX=0;
        Selection_AnchorY=0;
    }
    catch(...)
    {
        
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::PadOutScreenWithBlankLines
 *
 * SYNOPSIS:
 *    void DisplayText::PadOutScreenWithBlankLines(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function adds blank lines to the end of the 'Lines' so we have a
 *    full screen worth of lines.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::PadOutScreenWithBlankLines(void)
{
    struct TextLine BlankLine;

    if(LinesCount<ScreenHeightChars)
    {
        /* We didn't have enough lines, add blank lines */
        BlankLine.LineBackgroundColor=Settings->
                DefaultColors[e_DefaultColors_BG];
        BlankLine.LineWidthPx=0;
        BlankLine.EOL=e_DTEOL_Hard;
        BlankLine.EOLGuess=e_DTEOLGuess_Unknown;

        while(LinesCount<ScreenHeightChars)
        {
            Lines.push_back(BlankLine);
            LinesCount++;
        }
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::IsLineBlank
 *
 * SYNOPSIS:
 *    bool DisplayText::IsLineBlank(i_TextLines Line);
 *
 * PARAMETERS:
 *    Line [I] -- The line to check
 *
 * FUNCTION:
 *    This function looks to see if a line is considered blank (empty).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayText::IsLineBlank(i_TextLines Line)
{
    i_TextLineFrags CurFrag;

    if(Line->Frags.empty())
        return true;

    for(CurFrag=Line->Frags.begin();CurFrag!=Line->Frags.end();CurFrag++)
    {
        /* Fragment must be all blank or all spaces */
        switch(CurFrag->FragType)
        {
            case e_TextCanvasFrag_String:
                if(CurFrag->Text.find_first_not_of(' ')!=
                        std::string::npos)
                {
                    return false;
                }
            break;
            case e_TextCanvasFrag_NonPrintableChar:
            case e_TextCanvasFrag_HR:
                return false;
            break;
            case e_TextCanvasFrag_SoftRet:
            case e_TextCanvasFrag_HardRet:
            case e_TextCanvasFrag_RetText:
            case e_TextCanvasFragMAX:
            default:
                /* These count as blank */
            break;
        }
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::SetDrawMask
 *
 * SYNOPSIS:
 *    void DisplayText::SetDrawMask(uint16_t Mask);
 *
 * PARAMETERS:
 *    Mask [I] -- What mask to apply to drawing attributes.  If the bit is
 *                set then this is drawen.  These are the bits from
 *                UITC_SetDrawMask() UI control.
 *
 * FUNCTION:
 *    This function sets the draw mask for this display.  The draw mask is
 *    what attributes / styles can be drawn (IE bold, color, etc).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::SetDrawMask(uint16_t Mask)
{
    if(TextDisplayCtrl==NULL)
        return;

    UITC_SetDrawMask(UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),Mask);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::GetContextMenuHandle
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
t_UIContextMenuCtrl *DisplayText::GetContextMenuHandle(e_UITD_ContextMenuType UIObj)
{
    if(TextDisplayCtrl==NULL)
        return NULL;
    return UITC_GetContextMenuHandle(TextDisplayCtrl,UIObj);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::GetContextSubMenuHandle
 *
 * SYNOPSIS:
 *    t_UIContextMenuCtrl *DisplayText::GetContextSubMenuHandle(
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
t_UIContextSubMenuCtrl *DisplayText::GetContextSubMenuHandle(e_UITD_ContextSubMenuType UIObj)
{
    if(TextDisplayCtrl==NULL)
        return NULL;

    return UITC_GetContextSubMenuHandle(TextDisplayCtrl,UIObj);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::DEBUG_ForceRedrawOfScreen
 *
 * SYNOPSIS:
 *    void DisplayText::DEBUG_ForceRedrawOfScreen(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This is a debug function that redraws the screen.  It is used when you
 *    want to single step through the code and see what's going on.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    REMOVE ALL CALLS TO THIS IN A RELEASE BUILD AS IT SLOWS THINGS DOWN A LOT!
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::DEBUG_ForceRedrawOfScreen(void)
{
    RedrawFullScreen();
    UI_ProcessAllPendingUIEvents();
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ClearArea
 *
 * SYNOPSIS:
 *    void DisplayText::ClearArea(uint32_t X1,uint32_t Y1,uint32_t X2,
 *              uint32_t Y2);
 *
 * PARAMETERS:
 *    X1 [I] -- The left edge
 *    Y1 [I] -- The top edge
 *    X2 [I] -- The right edge +1
 *    Y2 [I] -- The bottom edge +1
 *
 * FUNCTION:
 *    This function clears part of the screen.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::ClearArea(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2)
{
    i_TextLines CurLine;
    unsigned int y;
    i_TextLineFrags CurFrag;
    string::iterator StartPos;
    string::iterator EndPos;
    uint_fast32_t FillWidth;

    try
    {
        if(TextDisplayCtrl==NULL)
            return;

        if(X1>=X2 || Y1>=Y2)
            return;

        if(Y1>(unsigned)ScreenHeightChars || X1>(unsigned)ScreenWidthChars)
            return;

        /* Find the first line */
        CurLine=ScreenFirstLine;
        for(y=0;y<Y1 && CurLine!=Lines.end();y++)
            CurLine++;
        if(CurLine==Lines.end())
        {
            /* We don't have enough lines, we are clearing below the
               end of the lines, so we are done */
               return;
        }

        FillWidth=X2-X1;
        if(FillWidth>(uint_fast32_t)ScreenWidthChars)
            FillWidth=ScreenWidthChars-X1;

        while(y<Y2 && CurLine!=Lines.end())
        {
            /* Remove the char's we are clearing */
            TextLine_ErasePos2Pos(CurLine,X1,X2);
            /* Now insert spaces */
            TextLine_Fill(CurLine,X1,FillWidth,&CurrentStyle," ");

            y++;
            CurLine++;
        }

        RethinkInsertFrag();
        RedrawFullScreen();
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ScrollArea
 *
 * SYNOPSIS:
 *    void DisplayText::ScrollArea(uint32_t X1,uint32_t Y1,uint32_t X2,
 *              uint32_t Y2,int32_t dx,int32_t dy);
 *
 * PARAMETERS:
 *    X1 [I] -- The left edge
 *    Y1 [I] -- The top edge
 *    X2 [I] -- The right edge +1
 *    Y2 [I] -- The bottom edge +1
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
void DisplayText::ScrollArea(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2,
        int32_t dx,int32_t dy)
{
    i_TextLines CurLine;
    int_fast32_t y;
    i_TextLineFrags CurFrag;
    i_TextLineFrags Frag1;
    i_TextLineFrags Frag2;
    string::iterator StartPos;
    string::iterator EndPos;
    int_fast32_t ScrollWidth;
    int_fast32_t ScrollHeight;
    int32_t Use_dx;
    int32_t Use_dy;
    int32_t Delta;
    struct TextLine BlankLine;

    try
    {
        if(TextDisplayCtrl==NULL)
            return;

        if(X1>X2 || Y1>Y2)
            return;

        if(Y1>(unsigned)ScreenHeightChars || X1>(unsigned)ScreenWidthChars)
            return;

        ScrollWidth=X2-X1;
        if(ScrollWidth>ScreenWidthChars)
            ScrollWidth=ScreenWidthChars-X1;

        ScrollHeight=Y2-Y1;
        if(ScrollHeight>ScreenHeightChars)
            ScrollHeight=ScreenHeightChars-Y1;

        Use_dx=dx;
        if(Use_dx>0)
        {
            if(Use_dx>ScreenWidthChars)
                Use_dx=ScreenWidthChars;

            if(Use_dx>ScrollWidth)
                Use_dx=ScrollWidth;
        }
        else if(Use_dx<0)
        {
            if(Use_dx<-ScreenWidthChars)
                Use_dx=-ScreenWidthChars;
            if(Use_dx<-ScrollWidth)
                Use_dx=-ScrollWidth;
        }

        Use_dy=dy;
        if(Use_dy>0)
        {
            if(Use_dy>ScreenHeightChars)
                Use_dy=ScreenHeightChars;

            if(Use_dy>ScrollHeight)
                Use_dy=ScrollHeight;
        }
        else if(Use_dy<0)
        {
            if(Use_dy<-ScreenWidthChars)
                Use_dy=-ScreenWidthChars;
            if(Use_dy<-ScrollHeight)
                Use_dy=-ScrollHeight;
        }

        /* Scroll the X */
        /* Ok, walk each line scrolling things in the x dir */
        if(Use_dx!=0)
        {
            /* Find the first line */
            CurLine=ScreenFirstLine;
            for(y=0;y<Y1 && CurLine!=Lines.end();y++)
                CurLine++;
            if(CurLine==Lines.end())
            {
                /* We don't have enough lines, we are clearing below the
                   end of the lines, so we are done */
                   return;
            }

            /* Find the first line */
            while(y<Y2 && CurLine!=Lines.end())
            {
                if(Use_dx>0)
                {
                    /* Moving to the right */

                    /* Delete from the end and insert at the front */
                    TextLine_ErasePos2Pos(CurLine,X2-Use_dx,X2);
                    TextLine_Fill(CurLine,X1,Use_dx,&CurrentStyle," ");
                }
                else
                {
                    /* Moving to the left */
                    Delta=-Use_dx;  // Because dx is neg we need to invert

                    /* Delete from the start and insert at the end (dx is neg
                       so we have to reverse all the math (sub instead of add)) */
                    TextLine_ErasePos2Pos(CurLine,X1,X1+Delta);
                    TextLine_Fill(CurLine,X2-Delta,Delta,&CurrentStyle," ");
                }

                y++;
                CurLine++;
            }
        }

        /* Scroll the Y */
        if(Use_dy!=0)
        {
            /* Make sure we have enough lines */
            PadOutScreenWithBlankLines();

            if(Use_dy>0)
            {
                /* We are scrolling down */
                ScrollVertAreaDown(X1,Y1,X2,Y2,Use_dy);
            }
            else if(Use_dy<0)
            {
                /* We are scrolling up */
                ScrollVertAreaUp(X1,Y1,X2,Y2,-Use_dy);
            }
        }

        RethinkInsertFrag();
        RedrawFullScreen();
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ScrollVertAreaDown
 *
 * SYNOPSIS:
 *    void DisplayText::ScrollVertAreaDown(uint32_t X1,uint32_t Y1,uint32_t X2,
 *          uint32_t Y2,int32_t dy);
 *
 * PARAMETERS:
 *    X1 [I] -- The left edge
 *    Y1 [I] -- The top edge
 *    X2 [I] -- The right edge
 *    Y2 [I] -- The bottom edge
 *    dy [I] -- The amount to scroll
 *
 * FUNCTION:
 *    This function scrolls an area down (text moves down).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DisplayText::ScrollArea()
 ******************************************************************************/
void DisplayText::ScrollVertAreaDown(uint32_t X1,uint32_t Y1,uint32_t X2,
        uint32_t Y2,int32_t dy)
{
    i_TextLines TopLineOfArea;
    i_TextLines BottomLineOfArea;
    i_TextLines CurLine;
    i_TextLines CopyFromLine;
    i_TextLines CopyToLine;
    i_TextLineFrags InsertAfterFrag;
    i_TextLineFrags StartFrag;
    i_TextLineFrags EndFrag;
    int_fast32_t y;
    int_fast32_t r;
    int_fast32_t AreaHeight;
    int_fast32_t AreaWidth;

    if(Y2<=Y1)
    {
        /* Nothing to do */
        return;
    }

    AreaHeight=Y2-Y1;
    if(AreaHeight>ScreenHeightChars)
        AreaHeight=ScreenHeightChars-Y1;

    AreaWidth=X2-X1;
    if(AreaWidth>ScreenWidthChars)
        AreaWidth=ScreenWidthChars-X1;

    /* Find the top line */
    TopLineOfArea=ScreenFirstLine;
    for(y=0;y<Y1 && TopLineOfArea!=Lines.end();y++)
        TopLineOfArea++;
    if(TopLineOfArea==Lines.end())
    {
        /* We don't have enough lines, we are below the of the lines, so we
           are done */
        return;
    }

    /* Find the bottom line */
    BottomLineOfArea=TopLineOfArea;
    if(Y2>0)
    {
        for(;y<Y2-1 && BottomLineOfArea!=Lines.end();y++)
            BottomLineOfArea++;

        if(BottomLineOfArea==Lines.end())
            BottomLineOfArea--;
    }

    /************************************/
    /*** First delete from the bottom ***/
    /************************************/
    CurLine=BottomLineOfArea;
    for(r=0;r<dy;r++)
    {
        TextLine_ErasePos2Pos(CurLine,X1,X2);

        if(CurLine==Lines.begin())
        {
            /* We just did the top line, nothing above this, abort */
            break;
        }
        CurLine--;
    }

    /*************************************/
    /*** Shift the lines down (scroll) ***/
    /*************************************/
    /* Move the lines (working from the bottom) */
    CopyToLine=BottomLineOfArea;

    /* Move the copy from line up by the scroll amount */
    CopyFromLine=BottomLineOfArea;
    for(r=0;r<dy;r++)
    {
        if(CopyFromLine==Lines.begin())
        {
            /* We just did the top line, nothing above this, abort */
            break;
        }
        CopyFromLine--;
    }

    for(r=0;r<AreaHeight-dy;r++)
    {
        TextLine_SplitFrag(CopyFromLine,X2,false,NULL,&EndFrag);
        TextLine_SplitFrag(CopyFromLine,X1,false,NULL,&StartFrag);

        /* Make sure we have enough room on the line to insert the frag's */
        PadOutLine(&*CopyToLine,X1);

        TextLine_SplitFrag(CopyToLine,X1,false,NULL,&InsertAfterFrag);

        CopyToLine->Frags.insert(InsertAfterFrag,StartFrag,EndFrag);
        CopyFromLine->Frags.erase(StartFrag,EndFrag);

        if(CopyFromLine==Lines.begin())
        {
            /* We just did the top line, nothing above this, abort */
            break;
        }
        CopyFromLine--;
        CopyToLine--;
    }

    /***********************************************************/
    /*** Fill in blank lines to fill in the holes at the top ***/
    /***********************************************************/
    CurLine=TopLineOfArea;
    for(r=0;r<dy && CurLine!=Lines.end();r++)
    {
        TextLine_Fill(CurLine,X1,AreaWidth,&CurrentStyle," ");
        CurLine++;
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ScrollVertAreaUp
 *
 * SYNOPSIS:
 *    void DisplayText::ScrollVertAreaUp(uint32_t X1,uint32_t Y1,uint32_t X2,
 *          uint32_t Y2,int32_t dy);
 *
 * PARAMETERS:
 *    X1 [I] -- The left edge
 *    Y1 [I] -- The top edge
 *    X2 [I] -- The right edge
 *    Y2 [I] -- The bottom edge
 *    dy [I] -- The amount to scroll
 *
 * FUNCTION:
 *    This function scrolls an area up (text moves up, like a normal scroll
 *    of a screen).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DisplayText::ScrollArea()
 ******************************************************************************/
void DisplayText::ScrollVertAreaUp(uint32_t X1,uint32_t Y1,uint32_t X2,
        uint32_t Y2,int32_t dy)
{
    i_TextLines TopLineOfArea;
    i_TextLines BottomLineOfArea;
    i_TextLines CurLine;
    i_TextLines CopyFromLine;
    i_TextLines CopyToLine;
    i_TextLineFrags InsertAfterFrag;
    i_TextLineFrags StartFrag;
    i_TextLineFrags EndFrag;
    int_fast32_t y;
    int_fast32_t r;
    int_fast32_t AreaHeight;
    int_fast32_t AreaWidth;

    if(Y2<=Y1)
    {
        /* Nothing to do */
        return;
    }

    AreaHeight=Y2-Y1;
    if(AreaHeight>ScreenHeightChars)
        AreaHeight=ScreenHeightChars-Y1;

    AreaWidth=X2-X1;
    if(AreaWidth>ScreenWidthChars)
        AreaWidth=ScreenWidthChars-X1;

    /* Find the top line */
    TopLineOfArea=ScreenFirstLine;
    for(y=0;y<Y1 && TopLineOfArea!=Lines.end();y++)
        TopLineOfArea++;
    if(TopLineOfArea==Lines.end())
    {
        /* We don't have enough lines, we are below the of the lines, so we
           are done */
        return;
    }

    /* Find the bottom line */
    BottomLineOfArea=TopLineOfArea;
    if(Y2>0)
    {
        for(;y<Y2-1 && BottomLineOfArea!=Lines.end();y++)
            BottomLineOfArea++;

        if(BottomLineOfArea==Lines.end())
            BottomLineOfArea--;
    }

    /*********************************/
    /*** First delete from the top ***/
    /*********************************/
    CurLine=TopLineOfArea;
    for(r=0;r<dy && CurLine!=Lines.end();r++)
    {
        TextLine_ErasePos2Pos(CurLine,X1,X2);
        CurLine++;
    }

    /***********************************/
    /*** Shift the lines up (scroll) ***/
    /***********************************/
    /* Move the lines (working from the top) */
    CopyToLine=TopLineOfArea;

    /* Move the copy from line up by the scroll amount */
    CopyFromLine=TopLineOfArea;
    for(r=0;r<dy && CopyFromLine!=Lines.end();r++)
        CopyFromLine++;

    for(r=0;r<AreaHeight-dy && CopyFromLine!=Lines.end();r++)
    {
        TextLine_SplitFrag(CopyFromLine,X2,false,NULL,&EndFrag);
        TextLine_SplitFrag(CopyFromLine,X1,false,NULL,&StartFrag);

        /* Make sure we have enough room on the line to insert the frag's */
        PadOutLine(&*CopyToLine,X1);

        TextLine_SplitFrag(CopyToLine,X1,false,NULL,&InsertAfterFrag);

        CopyToLine->Frags.insert(InsertAfterFrag,StartFrag,EndFrag);
        CopyFromLine->Frags.erase(StartFrag,EndFrag);

        CopyFromLine++;
        CopyToLine++;
    }

    /**************************************************************/
    /*** Fill in blank lines to fill in the holes at the bottom ***/
    /**************************************************************/
    CurLine=BottomLineOfArea;
    for(r=0;r<dy;r++)
    {
        TextLine_Fill(CurLine,X1,AreaWidth,&CurrentStyle," ");

        if(CurLine==Lines.begin())
        {
            /* We just did the top line, nothing above this, abort */
            break;
        }
        CurLine--;
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::TextLine_FindFragAndPos
 *
 * SYNOPSIS:
 *    bool DisplayText::TextLine_FindFragAndPos(i_TextLines Line,
 *              int_fast32_t Offset,i_TextLineFrags *FoundFrag,
 *              int_fast32_t *FoundPos);
 *
 * PARAMETERS:
 *    Line [I] -- The line to search
 *    Offset [I] -- The string position on the line to find.  Only string
 *                  type are counted in the search.
 *    FoundFrag [O] -- This is set to the frag that 'Offset' was in
 *    FoundPos [O] -- This is set to the position from the start of the
 *                    string in 'FoundFrag'
 *    StrPos [O] -- This is a string iterator that will be set to the point
 *                  in the string.
 *
 * FUNCTION:
 *    This function finds a string offset on a line.  This function searchs
 *    all the string fragments and finds the char at 'Offset'
 *
 * RETURNS:
 *    true -- This offset was found on this line
 *    false -- The offset was not found.  'Offset' is beyond the end of the
 *             line.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayText::TextLine_FindFragAndPos(i_TextLines Line,
        int_fast32_t Offset,i_TextLineFrags *FoundFrag,int_fast32_t *FoundPos)
{
    i_TextLineFrags CurFrag;
    unsigned int CalX;
    string::iterator StartPos;
    string::iterator EndPos;
    int Chars;

    try
    {
        *FoundFrag=Line->Frags.end();
        *FoundPos=0;

        /* We need to walk the line until we find Offset */
        CalX=0;
        for(CurFrag=Line->Frags.begin();CurFrag!=Line->Frags.end();CurFrag++)
        {
            if(CurFrag->FragType==e_TextCanvasFrag_String)
            {
                Chars=utf8::unchecked::distance(CurFrag->Text.begin(),
                        CurFrag->Text.end());
                if(Offset>=CalX && Offset<CalX+Chars)
                    break;

                CalX+=Chars;
            }
        }

        /* Ok, 'Offset' is somewhere in this frag */
        if(CurFrag!=Line->Frags.end() &&
                CurFrag->FragType==e_TextCanvasFrag_String)
        {
            StartPos=CurFrag->Text.begin();
            EndPos=StartPos;
            while(StartPos!=CurFrag->Text.end())
            {
                /* Move up by 1 char */
                utf8::unchecked::next(EndPos);

                if(CalX>=Offset)
                {
                    /* Ok, we found the 'Offset' char */
                    *FoundFrag=CurFrag;
                    *FoundPos=EndPos-CurFrag->Text.begin()-1;
                    return true;
                }

                StartPos=EndPos;
                CalX++;
            }
        }
    }
    catch(...)
    {
    }

    return false;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::TextLine_SplitFrag
 *
 * SYNOPSIS:
 *    void DisplayText::TextLine_SplitFrag(i_TextLines Line,int_fast32_t Offset,
 *              bool SplitEvenIfResultEmpty,i_TextLineFrags *Frag1,
 *              i_TextLineFrags *Frag2);
 *
 * PARAMETERS:
 *    Line [I] -- The line to work on
 *    Offset [I] -- The string position on the line to break.
 *    SplitEvenIfResultEmpty [I] -- Normally if spliting the fragment will
 *                                  end up with empty fragment then this
 *                                  function will not split the fragment
 *                                  (Frag2 will be set to the existing
 *                                  fragment).  If this is true then the
 *                                  fragment is split anyway and an empty
 *                                  fragment is added.
 *    Frag1 [O] -- The first fragment.  This is the one that 'Offset' was in
 *                 (note that it maybe a copy).  This can be NULL.
 *    Frag2 [O] -- The second fragment.  This can be NULL.
 *
 * FUNCTION:
 *    This function takes a fragment and splits it into 2 fragments, 'Frag1'
 *    and 'Frag2'.
 *
 *    So for example if you have:
 *          AAAA->BBBB->CCCC
 *    and you tell this function to split BBBB in half (Offset=6) then the
 *    result will be:
 *          AAAA->BB->BB->CCCC
 *                ^   ^
 *            Frag1   Frag2
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    TextLine_FindFragAndPos(), TextLine_ErasePos2Pos(), TextLine_Insert(),
 *    TextLine_Fill(), TextLine_Clear()
 ******************************************************************************/
void DisplayText::TextLine_SplitFrag(i_TextLines Line,int_fast32_t Offset,
        bool SplitEvenIfResultEmpty,i_TextLineFrags *Frag1,i_TextLineFrags *Frag2)
{
    i_TextLineFrags NextFrag;
    i_TextLineFrags FirstFrag;
    i_TextLineFrags SecondFrag;
    i_TextLineFrags Frag;
    int_fast32_t Pos;
    struct TextLineFrag EmptyFrag;

    try
    {
        if(Frag1!=NULL)
            *Frag1=Line->Frags.end();
        if(Frag2!=NULL)
            *Frag2=Line->Frags.end();

        /* TextLine_FindFragAndPos() only returns String type frags so we don't
           need to worry about other frag types and we just need to break up
           the text in the current frag */
        if(!TextLine_FindFragAndPos(Line,Offset,&Frag,&Pos))
        {
            /* Nothing to do */
            return;
        }

/* DEBUG PAUL: This should work, but we need to fully test it, so for now
   we use the old code that we know works */
#if 0
        TxtFrag_SplitFrag(Line,Frag,Pos,SplitEvenIfResultEmpty,Frag1,Frag2);
#else
        if(Frag2!=NULL)
            *Frag2=Frag;

        /* Check if we are going to be adding blank strings */
        if(!SplitEvenIfResultEmpty &&
                (Pos==0 || Pos==(int_fast32_t)Frag->Text.length()))
        {
            return;
        }

        /* Add a copy of the frag */
        FirstFrag=Line->Frags.insert(Frag,*Frag);

        /* Remove the start of the copy of the string.  Because we insert
           BEFORE frag, the order is now:
                a -> b -> FirstFrag -> Frag -> c -> d) */
        FirstFrag->Text.erase(Pos);
        if(Frag1!=NULL)
            *Frag1=FirstFrag;

        /* Remove the end of the org string  */
        Frag->Text.erase(0,Pos);

        RethinkFragWidth(FirstFrag);
        RethinkFragWidth(Frag);
#endif
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::TxtFrag_SplitFrag
 *
 * SYNOPSIS:
 *    void DisplayText::TxtFrag_SplitFrag(i_TextLines Line,i_TextLineFrags Frag,
 *          int_fast32_t StrPos,bool SplitEvenIfResultEmpty,
 *          i_TextLineFrags *Frag1,i_TextLineFrags *Frag2);
 *
 * PARAMETERS:
 *    Line [I] -- The line to work on
 *    Frag [I] -- The frag to work on
 *    StrPos [I] -- The position to split this frag
 *    SplitEvenIfResultEmpty [I] -- Normally if spliting the fragment will
 *                                  end up with empty fragment then this
 *                                  function will not split the fragment
 *                                  (Frag2 will be set to the existing
 *                                  fragment).  If this is true then the
 *                                  fragment is split anyway and an empty
 *                                  fragment is added.
 *    Frag1 [O] -- The first fragment.  This is the one that 'Offset' was in
 *                 (note that it maybe a copy).  This can be NULL.
 *    Frag2 [O] -- The second fragment.  This can be NULL.
 *
 * FUNCTION:
 *    This function takes a fragment and splits it into 2 fragments, 'Frag1'
 *    and 'Frag2'.
 *
 *    So for example if you have:
 *          AAAA->BBBB->CCCC
 *    and you tell this function to split BBBB in half (Offset=6) then the
 *    result will be:
 *          AAAA->BB->BB->CCCC
 *                ^   ^
 *            Frag1   Frag2
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    This is like TextLine_SplitFrag() but works where you already have the
 *    frag you are interested in.
 *
 * SEE ALSO:
 *    TextLine_SplitFrag()
 ******************************************************************************/
void DisplayText::TxtFrag_SplitFrag(i_TextLines Line,i_TextLineFrags Frag,
        int_fast32_t StrPos,bool SplitEvenIfResultEmpty,i_TextLineFrags *Frag1,
        i_TextLineFrags *Frag2)
{
    i_TextLineFrags NextFrag;
    i_TextLineFrags FirstFrag;
    i_TextLineFrags SecondFrag;
    struct TextLineFrag EmptyFrag;

    try
    {
        if(Frag1!=NULL)
            *Frag1=Line->Frags.end();
        if(Frag2!=NULL)
            *Frag2=Line->Frags.end();

        /* We can only split text frags */
        if(Frag->FragType!=e_TextCanvasFrag_String)
            return;

        if(StrPos>=(int_fast32_t)Frag->Text.length())
            return;

        if(Frag2!=NULL)
            *Frag2=Frag;

        /* Check if we are going to be adding blank strings */
        if(!SplitEvenIfResultEmpty &&
                (StrPos==0 || StrPos==(int_fast32_t)Frag->Text.length()))
        {
            return;
        }

        /* Add a copy of the frag */
        FirstFrag=Line->Frags.insert(Frag,*Frag);

        /* Remove the start of the copy of the string.  Because we insert
           BEFORE frag, the order is now:
                a -> b -> FirstFrag -> Frag -> c -> d) */
        FirstFrag->Text.erase(StrPos);
        if(Frag1!=NULL)
            *Frag1=FirstFrag;

        /* Remove the end of the org string  */
        Frag->Text.erase(0,StrPos);

        RethinkFragWidth(FirstFrag);
        RethinkFragWidth(Frag);
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::TextLine_ErasePos2Pos
 *
 * SYNOPSIS:
 *    void DisplayText::TextLine_ErasePos2Pos(i_TextLines Line,
 *              int_fast32_t StartOffset,int_fast32_t EndOffset);
 *
 * PARAMETERS:
 *    Line [I] -- The line to work on
 *    StartOffset [I] -- The char to start erasing at
 *    EndOffset [I] -- The char to stop erasing at
 *
 * FUNCTION:
 *    This function erases all the char's and fragments between 'StartOffset'
 *    and 'EndOffset'.
 *
 *    For example if you have:
 *         AAAA->BBBB->CCCC
 *    and you delete from offset 2 to offset 10 you will end up with:
 *         AA->CC
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::TextLine_ErasePos2Pos(i_TextLines Line,
        int_fast32_t StartOffset,int_fast32_t EndOffset)
{
    i_TextLineFrags Frag1;
    i_TextLineFrags Frag2;

    try
    {
        TextLine_SplitFrag(Line,EndOffset,false,NULL,&Frag2);
        TextLine_SplitFrag(Line,StartOffset,false,NULL,&Frag1);

        /* Delete everything between Frag1 and Frag2 */
        Line->Frags.erase(Frag1,Frag2);
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::TextLine_Insert
 *
 * SYNOPSIS:
 *    void DisplayText::TextLine_Insert(i_TextLines Line,int_fast32_t Offset,
 *          const struct CharStyling *Style,const char *Str);
 *
 * PARAMETERS:
 *    Line [I] -- The line to work on
 *    Offset [I] -- The offset into 'Line' to insert this string
 *    Style [I] -- The style for this string
 *    Str [I] -- The string to insert (UTF8)
 *
 * FUNCTION:
 *    This function adds a string to a line.  No line wrap is done.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::TextLine_Insert(i_TextLines Line,int_fast32_t Offset,
        const struct CharStyling *Style,const char *Str)
{
    struct TextLineFrag NewFrag;
    i_TextLineFrags SplitFrag;
    i_TextLineFrags AddedFrag;
    i_TextLineFrags Frag;
    int_fast32_t Pos;

    try
    {
        if(!TextLine_FindFragAndPos(Line,Offset,&Frag,&Pos))
        {
            /* This means we couldn't find this Offset on this line, basicly
               Offset is past the end of the line.  We need to pad */
            PadOutLine(&*Line,Offset);

            /* We just added padding, so the last frag at the last pos is now
               our insert point */
            Frag=Line->Frags.end();
            Frag--;
            Pos=Frag->Text.length();
        }
        /* First check if we have the same attribs and if so just insert into
           the existing string */
        if(Frag->FragType==e_TextCanvasFrag_String &&
                CmpCharStyle(&Frag->Styling,Style))
        {
            /* Ok, we can just mod the string */
            Frag->Text.insert(Pos,Str);
        }
        else
        {
            /* Add a new frag with the new string */
            TextLine_SplitFrag(Line,Offset,false,NULL,&SplitFrag);

            NewFrag.FragType=e_TextCanvasFrag_String;
            NewFrag.Styling=*Style;
            NewFrag.Text=Str;
            NewFrag.WidthPx=0; /* We need to pass in an iterator so we do this later */

            Frag++;
            AddedFrag=Line->Frags.insert(SplitFrag,NewFrag);
            RethinkFragWidth(AddedFrag);
        }
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::TextLine_Fill
 *
 * SYNOPSIS:
 *    void DisplayText::TextLine_Fill(i_TextLines Line,int_fast32_t Offset,
 *              uint_fast32_t Count,const struct CharStyling *Style,
 *              const char *Chr);
 *
 * PARAMETERS:
 *    Line [I] -- The line to work on
 *    Offset [I] -- The offset into 'Line' to insert this string
 *    Count [I] -- The number of chars to add
 *    Style [I] -- The styling for this new string
 *    Chr [I] -- The UTF8 char to add
 *
 * FUNCTION:
 *    This function adds a string made up of 'Chr' repeated 'Count' times.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::TextLine_Fill(i_TextLines Line,int_fast32_t Offset,
        uint_fast32_t Count,const struct CharStyling *Style,const char *Chr)
{
    struct TextLineFrag NewFrag;
    i_TextLineFrags SplitFrag;
    i_TextLineFrags AddedFrag;
    i_TextLineFrags Frag;
    int_fast32_t Pos;
    uint_fast32_t r;

    try
    {
        if(!TextLine_FindFragAndPos(Line,Offset,&Frag,&Pos))
        {
            /* This means we couldn't find this Offset on this line, basicly
               Offset is past the end of the line.  We need to pad */
            PadOutLine(&*Line,Offset);

            /* We just added padding, so the last frag at the last pos is now
               our insert point */
            Frag=Line->Frags.end();
            Frag--;
            Pos=Frag->Text.length();
        }
        /* First check if we have the same attribs and if so just insert into
           the existing string */
        if(Frag->FragType==e_TextCanvasFrag_String &&
                CmpCharStyle(&Frag->Styling,Style))
        {
            /* Ok, we can just mod the string */
            for(r=0;r<Count;r++)
                Frag->Text.insert(Pos,Chr);
        }
        else
        {
            /* Add a new frag with the new string */
            TextLine_SplitFrag(Line,Offset,false,NULL,&SplitFrag);

            NewFrag.FragType=e_TextCanvasFrag_String;
            NewFrag.Styling=*Style;
            NewFrag.Text="";
            for(r=0;r<Count;r++)
                NewFrag.Text+=Chr;
            NewFrag.WidthPx=0; /* We need to pass in an iterator so we do this later */

            Frag++;
            AddedFrag=Line->Frags.insert(SplitFrag,NewFrag);
            RethinkFragWidth(AddedFrag);
        }
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::TextLine_Clear
 *
 * SYNOPSIS:
 *    void DisplayText::TextLine_Clear(i_TextLines Line);
 *
 * PARAMETERS:
 *    Line [I] -- The line to work on
 *
 * FUNCTION:
 *    This function erases all the fragments for a line.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::TextLine_Clear(i_TextLines Line)
{
    Line->Frags.clear();
    Line->LineWidthPx=0;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::TextLine_FindLineLen
 *
 * SYNOPSIS:
 *    int DisplayText::TextLine_FindLineLen(i_TextLines Line);
 *
 * PARAMETERS:
 *    Line [I] -- The line to work on
 *
 * FUNCTION:
 *    This function counts all the chars (not bytes) on a line in all the
 *    frags on the line.
 *
 * RETURNS:
 *    The number of chars on this line.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int DisplayText::TextLine_FindLineLen(i_TextLines Line)
{
    i_TextLineFrags CurFrag;
    int Chars;
    int TotalChars;

    TotalChars=0;

    /* We need to walk all the frags */
    for(CurFrag=Line->Frags.begin();CurFrag!=Line->Frags.end();CurFrag++)
    {
        if(CurFrag->FragType==e_TextCanvasFrag_String)
        {
            Chars=utf8::unchecked::distance(CurFrag->Text.begin(),
                    CurFrag->Text.end());

            TotalChars+=Chars;
        }
    }

    return TotalChars;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ShowBell
 *
 * SYNOPSIS:
 *    void DisplayText::ShowBell(void);
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
void DisplayText::ShowBell(void)
{
    UITC_ShowBellIcon(TextDisplayCtrl);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::GetSendPanel_HexPosInput
 *
 * SYNOPSIS:
 *    t_UITextInputCtrl *DisplayText::GetSendPanel_HexPosInput(void);
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
t_UITextInputCtrl *DisplayText::GetSendPanel_HexPosInput(void)
{
    return UITC_GetTextInputHandle(TextDisplayCtrl,e_UITC_Txt_Pos);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::GetSendPanel_HexRadioBttn
 *
 * SYNOPSIS:
 *    t_UIRadioBttnCtrl *DisplayText::GetSendPanel_HexRadioBttn(void);
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
t_UIRadioBttnCtrl *DisplayText::GetSendPanel_HexRadioBttn(void)
{
    return UITC_GetRadioButton(TextDisplayCtrl,e_UITC_RadioButton_Hex);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::GetSendPanel_TextRadioBttn
 *
 * SYNOPSIS:
 *    t_UIRadioBttnCtrl *DisplayText::GetSendPanel_TextRadioBttn(void);
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
t_UIRadioBttnCtrl *DisplayText::GetSendPanel_TextRadioBttn(void)
{
    return UITC_GetRadioButton(TextDisplayCtrl,e_UITC_RadioButton_Text);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::GetSendPanel_TextInput
 *
 * SYNOPSIS:
 *    t_UIMuliLineTextInputCtrl *DisplayText::GetSendPanel_TextInput(void);
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
t_UIMuliLineTextInputCtrl *DisplayText::GetSendPanel_TextInput(void)
{
    return UITC_GetMuliLineTextInputHandle(TextDisplayCtrl,e_UITC_MuliTxt_TextInput);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::GetSendPanel_LineEndInput
 *
 * SYNOPSIS:
 *    t_UIComboBoxCtrl *DisplayText::GetSendPanel_LineEndInput(void);
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
t_UIComboBoxCtrl *DisplayText::GetSendPanel_LineEndInput(void)
{
    return UITC_GetComboBoxHandle(TextDisplayCtrl,e_UITC_Combox_LineEnd);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::SendPanel_ShowHexOrText
 *
 * SYNOPSIS:
 *    void DisplayText::SendPanel_ShowHexOrText(bool Text);
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
void DisplayText::SendPanel_ShowHexOrText(bool Text)
{
    UITC_SendPanelShowHexOrTextInput(TextDisplayCtrl,Text);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ChangeAttribsBetweenPoints
 *
 * SYNOPSIS:
 *    void DisplayText::ChangeAttribsBetweenPoints(int P1X,int P1Y,int P2X,
 *      int P2Y,uint32_t Attribs,uint32_t FGColor,uint32_t BGColor,
 *      uint32_t ULineColor,uint32_t What);
 *
 * PARAMETERS:
 *    P1X [I] -- The X pos of the first point
 *    P1Y [I] -- The Y pos of the first point
 *    P2X [I] -- The X pos of the second point (this must come after P1X,P1Y)
 *    P2Y [I] -- The Y pos of the second point (this must come after P1X,P1Y)
 *    Attribs [I] -- What attribs to clear/set for this block
 *    FGColor [I] -- The foreground color to set
 *    BGColor [I] -- The background color to set
 *    ULineColor [I] -- The under line color to use
 *    What [I] -- What are we changing.  Supported values:
 *                  DTXT_APPLY_SET_ATTRIB -- Set 'Attribs'
 *                  DTXT_APPLY_CLR_ATTRIB -- Clear 'Attribs'
 *                  DTXT_APPLY_FOREGROUND -- Set 'FGColor'
 *                  DTXT_APPLY_BACKGROUND -- Set 'BGColor'
 *                  DTXT_APPLY_ULINE_COLOR -- Set 'ULineColor'
 *
 * FUNCTION:
 *    This function sets or clears the attributes/colors between to points.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::ChangeAttribsBetweenPoints(int P1X,int P1Y,int P2X,int P2Y,
        uint32_t Attribs,uint32_t FGColor,uint32_t BGColor,uint32_t ULineColor,
        uint32_t What)
{
    struct DTPoint Start;
    struct DTPoint End;
    i_TextLines CurLine;
    i_TextLineFrags CurFrag;
    string::iterator StartOfStr;
    string::iterator EndOfStr;
    i_TextLineFrags FirstFrag;
    i_TextLineFrags LastFrag;
    i_TextLineFrags StopFrag;

    if(!FindPoint(P1X,P1Y,Start,Lines.end(),0))
        return;

    if(!FindPoint(P2X,P2Y,End,Start.Line,Start.LineY))
        return;

    if(Start.Line==End.Line && Start.Frag==End.Frag)
    {
        /* On the same line and frag */

        /* Split the frags */
        TextLine_SplitFrag(Start.Line,P1X,false,NULL,NULL);
        TextLine_SplitFrag(End.Line,P2X,false,&LastFrag,NULL);

        if(What&DTXT_APPLY_SET_ATTRIB)
        {
            LastFrag->Styling.Attribs|=Attribs;
            /* Make sure to also set the uline color */
            LastFrag->Styling.ULineColor=LastFrag->Styling.FGColor;
        }
        if(What&DTXT_APPLY_CLR_ATTRIB)
            LastFrag->Styling.Attribs&=~Attribs;
        if(What&DTXT_APPLY_FOREGROUND)
            LastFrag->Styling.FGColor=FGColor;
        if(What&DTXT_APPLY_BACKGROUND)
            LastFrag->Styling.BGColor=BGColor;
        if(What&DTXT_APPLY_ULINE_COLOR)
            LastFrag->Styling.ULineColor=ULineColor;
    }
    else
    {
        /* First thing we need to do is split the first and last fragments */

        /* Split the end first */
        TextLine_SplitFrag(Start.Line,P1X,false,NULL,&FirstFrag);
        TextLine_SplitFrag(End.Line,P2X,false,NULL,&StopFrag);

        /* Look at all the frags between the start and end */
        CurLine=Start.Line;
        CurFrag=FirstFrag;
        while(CurFrag!=StopFrag)
        {
            if(CurFrag==CurLine->Frags.end())
            {
                /* End of the line move to the next */
                CurLine++;
                if(CurLine==Lines.end())
                {
                    /* Unexpected end */
                    return;
                }
                CurFrag=CurLine->Frags.begin();
                continue;
            }

            if(CurFrag!=CurLine->Frags.end() &&
                    CurFrag->FragType==e_TextCanvasFrag_String)
            {
                if(What&DTXT_APPLY_SET_ATTRIB)
                {
                    CurFrag->Styling.Attribs|=Attribs;
                    /* Make sure to also set the uline color */
                    CurFrag->Styling.ULineColor=CurFrag->Styling.FGColor;
                }
                if(What&DTXT_APPLY_CLR_ATTRIB)
                    CurFrag->Styling.Attribs&=~Attribs;
                if(What&DTXT_APPLY_FOREGROUND)
                    CurFrag->Styling.FGColor=FGColor;
                if(What&DTXT_APPLY_BACKGROUND)
                    CurFrag->Styling.BGColor=BGColor;
                if(What&DTXT_APPLY_ULINE_COLOR)
                    CurFrag->Styling.ULineColor=ULineColor;
            }

            CurFrag++;
        }
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::GetSelectionString
 *
 * SYNOPSIS:
 *    bool DisplayText::GetStringBetweenPoints(int P1X,int P1Y,int P2X,int P2Y,
 *          std::string &Clip);
 *
 * PARAMETERS:
 *    P1X [I] -- The X pos of the first point
 *    P1Y [I] -- The Y pos of the first point
 *    P2X [I] -- The X pos of the second point (this must come after P1X,P1Y)
 *    P2Y [I] -- The Y pos of the second point (this must come after P1X,P1Y)
 *    Clip [O] -- The text from the selection
 *
 * FUNCTION:
 *    This function gets a copy of the text between 2 points.
 *
 * RETURNS:
 *    true -- The block was valid and 'Clip' has been set
 *    false --- One of points we invalid and 'Clip' has been set to ""
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayText::GetStringBetweenPoints(int P1X,int P1Y,int P2X,int P2Y,
        std::string &Clip)
{
    struct DTPoint Start;
    struct DTPoint End;
    i_TextLines CurLine;
    i_TextLineFrags CurFrag;
    string::iterator StartOfStr;
    string::iterator EndOfStr;

    Clip="";

    if(!FindPoint(P1X,P1Y,Start,Lines.end(),0))
        return false;

    if(!FindPoint(P2X,P2Y,End,Start.Line,Start.LineY))
        return false;

    if(Start.Line==End.Line && Start.Frag==End.Frag)
    {
        /* On the same line and frag */
        Clip.assign(Start.Frag->Text,Start.StrPos,End.StrPos-Start.StrPos);
    }
    else
    {
        CurLine=Start.Line;
        CurFrag=Start.Frag;

        /* Take from Start to the end of frag */
        if(Start.Frag!=Start.Line->Frags.end())
        {
            Clip.assign(Start.Frag->Text,Start.StrPos,string::npos);
            CurFrag++;
        }
        else
        {
            /* The point was past the end of the line (or it was a
               blank line) so we add a blank line and then continue the
               copy. */
            Clip="\n";
            CurLine++;  // Move to the end line
            CurFrag=CurLine->Frags.begin(); // And the first fragment
        }

        /* Now copy all the frag from here to end frag */
        while(CurFrag!=End.Frag)
        {
            if(CurFrag==CurLine->Frags.end())
            {
                /* End of the line move to the next */
                CurLine++;
                if(CurLine==Lines.end())
                {
                    /* Unexpected end */
                    return false;
                }
                CurFrag=CurLine->Frags.begin();
                Clip.append("\n");
                continue;
            }

            if(CurFrag!=CurLine->Frags.end() &&
                    CurFrag->FragType==e_TextCanvasFrag_String)
            {
                Clip.append(CurFrag->Text);
            }

            CurFrag++;
        }

        /* Now take the last part of the string */
        if(CurFrag!=CurLine->Frags.end() &&
                CurFrag->FragType==e_TextCanvasFrag_String)
        {
            Clip.append(End.Frag->Text,0,End.StrPos);
        }
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ToggleAttribs2Selection
 *
 * SYNOPSIS:
 *    void DisplayText::ToggleAttribs2Selection(uint32_t Attribs);
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
void DisplayText::ToggleAttribs2Selection(uint32_t Attribs)
{
    int SelX1;
    int SelY1;
    int SelX2;
    int SelY2;
    uint32_t WhatFlags;

    WhatFlags=DTXT_APPLY_SET_ATTRIB;
    if(IsAttribSetInSelection(Attribs))
        WhatFlags=DTXT_APPLY_CLR_ATTRIB;

    GetNormalizedSelection(SelX1,SelY1,SelX2,SelY2);

    ChangeAttribsBetweenPoints(SelX1,SelY1,SelX2,SelY2,Attribs,0,0,0,WhatFlags);

    RedrawFullScreen();
}

/*******************************************************************************
 * NAME:
 *    DisplayText::IsAttribSetInSelection
 *
 * SYNOPSIS:
 *    bool DisplayText::IsAttribSetInSelection(uint32_t Attribs);
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
bool DisplayText::IsAttribSetInSelection(uint32_t Attribs)
{
    struct DTPoint Start;
    struct DTPoint End;
    i_TextLines CurLine;
    i_TextLineFrags CurFrag;
    string::iterator StartOfStr;
    string::iterator EndOfStr;

    if(!FindPointsOfSelection(Start,End))
        return false;

    if(Start.Line==End.Line && Start.Frag==End.Frag)
    {
        /* On the same line and frag */
        if((Start.Frag->Styling.Attribs&Attribs)==Attribs)
            return true;
    }
    else
    {
        /* Look at all the frags between the start and end */
        CurLine=Start.Line;
        CurFrag=Start.Frag;
        while(CurFrag!=End.Frag)
        {
            if(CurFrag==CurLine->Frags.end())
            {
                /* End of the line move to the next */
                CurLine++;
                if(CurLine==Lines.end())
                {
                    /* Unexpected end */
                    return false;
                }
                CurFrag=CurLine->Frags.begin();
                continue;
            }

            if(CurFrag!=CurLine->Frags.end() &&
                    CurFrag->FragType==e_TextCanvasFrag_String)
            {
                if((CurFrag->Styling.Attribs&Attribs)==Attribs)
                    return true;
            }

            CurFrag++;
        }

        /* Now take the last part of the string */
        if(CurFrag!=CurLine->Frags.end() && End.StrPos!=0 &&
                CurFrag->FragType==e_TextCanvasFrag_String)
        {
            if((CurFrag->Styling.Attribs&Attribs)==Attribs)
                return true;
        }
    }

    return false;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::FindPointsOfSelection
 *
 * SYNOPSIS:
 *    bool DisplayText::FindPointsOfSelection(struct DTPoint &Start,
 *              struct DTPoint &End);
 *
 * PARAMETERS:
 *    Start [O] -- Where in the buffer does the start of the selection points.
 *                 Start will always be earlier in the buffer (so you
 *                 can always begin at 'Start' and work increasing towards
 *                 'End').  The Start.Frag maybe set to Line->Frags.end(), if
 *                 that is the case that means the X was off the end of the line
 *                 (or the line was blank).
 *    End [O] -- Where in the buffer does the end of the selection points.
 *               Because the end point can be off the end of a line you have
 *               to handle where some parts are invalid.
 *                      Line -- Always valid
 *                      Frag -- Maybe set to Line->Frags.end()
 *                      StrPos -- If 'Frag' is at end then this is invalid.
 *
 * FUNCTION:
 *    This function takes the active selection and converts it to a start
 *    and end point in the data.
 *
 * RETURNS:
 *    true -- We converted the points
 *    false -- We couldn't convert the points (or there was no selection)
 *
 * SEE ALSO:
 *    FindPoint()
 ******************************************************************************/
bool DisplayText::FindPointsOfSelection(struct DTPoint &Start,
        struct DTPoint &End)
{
    int SelX1;
    int SelY1;
    int SelX2;
    int SelY2;

    Start.Line=Lines.end();
    End.Line=Lines.end();

    /* The select is also not valid if y=y and x=x */
    if(!IsThereASelection())
        return false;

    GetNormalizedSelection(SelX1,SelY1,SelX2,SelY2);

    if(!FindPoint(SelX1,SelY1,Start,Lines.end(),0))
        return false;
    if(!FindPoint(SelX2,SelY2,End,Start.Line,Start.LineY))
        return false;

    return true;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::CmpXYPositions
 *
 * SYNOPSIS:
 *    int DisplayText::CmpXYPositions(int X1,int Y1,int X2,int Y2);
 *
 * PARAMETERS:
 *    X1 [I] -- The first X pos to compare
 *    Y1 [I] -- The first Y pos to compare
 *    X2 [I] -- The second X pos to compare
 *    Y2 [I] -- The second Y pos to compare
 *
 * FUNCTION:
 *    This function compairs two x,y points to figure out which one is before
 *    the other in.
 *
 * RETURNS:
 *    - = 1 < 2
 *    0  = 1 = 2
 *    + = 1 > 2
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int DisplayText::CmpXYPositions(int X1,int Y1,int X2,int Y2)
{
    if(Y1==Y2)
    {
        if(X1==X2)
            return 0;

        return X1-X2;
    }

    return Y1-Y2;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::FindPoint
 *
 * SYNOPSIS:
 *    bool DisplayText::FindPoint(int PX,int PY,struct DTPoint &Pos,
 *              i_TextLines HintLine,int HintStartY);
 *
 * PARAMETERS:
 *    PX [I] -- The X pos to look up
 *    PY [I] -- The Y pos to look up
 *    End [O] -- Where in the buffer that this X,Y points.
 *               Because the point can be off the end of a line you have
 *               to handle where some parts are invalid.
 *                      Line -- Always valid
 *                      Frag -- Maybe set to Line->Frags.end()
 *                      StrPos -- If 'Frag' is at end then this is invalid.
 *    HintLine [I] -- A hint of where to start searching from.  Normally
 *                    you want to set this to the thing that is closest
 *                    to where the X,Y point will be found (just speeds things
 *                    up).  Set to Lines.end() to ignore.
 *    HintStartY [I] -- Used with 'HintLine'.
 *
 * FUNCTION:
 *    This function takes a x,y point in the buffer and converts it to a start
 *    and end point in the data.
 *
 * RETURNS:
 *    true -- We converted the point.  You can still get a return of true even
 *            if 'PX' is not on text.  See NOTES.
 *    false -- We could not convert the point.  Both 'PX' and 'PY' could not
 *             be found.
 *
 * NOTES:
 *    'PX' is still considered valid if it's at the end of the line.  In that
 *    case 'Pos.Frag' will be set to 'Pos.Line->Frags.end()'
 *
 * SEE ALSO:
 *    FindPointsOfSelection()
 ******************************************************************************/
bool DisplayText::FindPoint(int PX,int PY,struct DTPoint &Pos,
        i_TextLines HintLine,int HintStartY)
{
    i_TextLines CurLine;
    int Delta;
    unsigned int MinDelta;
    unsigned int TmpStart;
    i_TextLineFrags CurFrag;
    int r;
    int StartingY;
    i_TextLines StartLine;
    i_TextLineFrags StartFrag;
    int_fast32_t StartOfStr;
    int TargetLineY;

    /* Mark everything we don't support to .end() */
    Pos.Line=Lines.end();

    if(PY>=LinesCount)
    {
        /* We don't have this line */
        return false;
    }

    /* Find the quickest way to the line we are looking for */
    MinDelta=~0;

    if(HintLine!=Lines.end())
    {
        /* Try the hint line */
        FindPointHelper_FindDelta(PY,HintLine,HintStartY,&MinDelta,
                &StartLine,&StartingY);
    }
    /* ScreenFirstLine */
    if(ScreenHeightChars<LinesCount)
        TmpStart=LinesCount-ScreenHeightChars;
    else
        TmpStart=0;
    FindPointHelper_FindDelta(PY,ScreenFirstLine,TmpStart,&MinDelta,&StartLine,&StartingY);
    /* TopLine */
    FindPointHelper_FindDelta(PY,TopLine,TopLineY,&MinDelta,&StartLine,&StartingY);
    /* Top */
    FindPointHelper_FindDelta(PY,Lines.begin(),0,&MinDelta,&StartLine,&StartingY);
    /* Bottom */
    FindPointHelper_FindDelta(PY,Lines.end(),LinesCount,&MinDelta,&StartLine,&StartingY);

    if(StartingY<PY)
    {
        /* TopLine is above the selection (go forward) */
        Delta=PY-StartingY;
        for(r=0;r<Delta && StartLine!=Lines.end();r++)
            StartLine++;

        if(StartLine==Lines.end())
        {
            /* Hu? we don't have any lines to copy */
            return false;
        }
        TargetLineY=StartingY+Delta;
    }
    else
    {
        /* TopLine is below (or on) the point (go backward) */
        Delta=StartingY-PY;
        for(r=0;r<Delta && StartLine!=Lines.begin();r++)
            StartLine--;
        TargetLineY=StartingY-Delta;
        if(StartLine==Lines.begin())
            TargetLineY=0;
    }

    /* Find the starting and end frag and offsets */
    TextLine_FindFragAndPos(StartLine,PX,&StartFrag,&StartOfStr);

    Pos.Line=StartLine;
    Pos.LineY=TargetLineY;
    Pos.Frag=StartFrag;
    Pos.StrPos=StartOfStr;

    return true;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::FindPointHelper_FindDelta
 *
 * SYNOPSIS:
 *    void DisplayText::FindPointHelper_FindDelta(int PY,
 *          i_TextLines StartLine,int StartingY,unsigned int *LastMinDelta,
 *          i_TextLines *RetStartLine,int *RetStartingY);
 *
 * PARAMETERS:
 *    PY [I] -- The Y pos of the point we want to find (from top of buffer)
 *    StartLine [I] -- The start line to eval
 *    StartingY [I] -- The Y of 'StartLine' (from top of buffer)
 *    LastMinDelta [I/O] -- This will be used for the min delta that has
 *                          been found so far.  If the new delta is smaller
 *                          then this will be updated to the new min delta.
 *                          On the first call this should be set to ~0.
 *    RetStartLine [O] -- If this new delta is smaller than 'LastMinDelta' then
 *                        this will be set to the 'StartLine'
 *    RetStartingY [O] -- If this new delta is smaller than 'LastMinDelta' then
 *                        this will be set to the 'StartingY'
 *
 * FUNCTION:
 *    This is a helper function for FindPoint().  It checks if the number of
 *    lines between 'StartLine' is smaller than the last value
 *    checked ('LastMinDelta').
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    FindPoint()
 ******************************************************************************/
void DisplayText::FindPointHelper_FindDelta(int PY,
        i_TextLines StartLine,int StartingY,unsigned int *LastMinDelta,
        i_TextLines *RetStartLine,int *RetStartingY)
{
    unsigned int Delta;

    if(StartingY<PY)
        Delta=PY-StartingY;
    else
        Delta=StartingY-PY;

    if(Delta<*LastMinDelta)
    {
        *RetStartLine=StartLine;
        *RetStartingY=StartingY;
        *LastMinDelta=Delta;
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::FindLastTextFragOnLine
 *
 * SYNOPSIS:
 *    i_TextLineFrags DisplayText::FindLastTextFragOnLine(const i_TextLines &Line);
 *
 * PARAMETERS:
 *    Line [I] -- The line to find the frag on
 *
 * FUNCTION:
 *    This function returns the last frag on a line that is of
 *    'e_TextCanvasFrag_String' type.
 *
 * RETURNS:
 *    The last frag with text in it or Line->Frags.end() if there are no
 *    string fragments on this line.
 *
 * SEE ALSO:
 *    TextLine_FindFragAndPos()
 ******************************************************************************/
i_TextLineFrags DisplayText::FindLastTextFragOnLine(const i_TextLines &Line)
{
    i_TextLineFrags Frag;

    Frag=Line->Frags.end();
    if(!Line->Frags.empty())
    {
        while(Frag!=Line->Frags.begin())
        {
            Frag--;
            if(Frag->FragType==e_TextCanvasFrag_String)
                break;
        }
        if(Frag->FragType!=e_TextCanvasFrag_String)
            Frag=Line->Frags.end();
    }

    return Frag;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::AdvancePoint
 *
 * SYNOPSIS:
 *    void DisplayText::AdvancePoint(int &PX,int &PY,int Amount,int MinX,
 *              int MinY,int MaxX,int MaxY);
 *
 * PARAMETERS:
 *    PX [I/O] -- The X point to move
 *    PY [I/O] -- The Y point.  If X goes before the start of the line or
 *                past the end this will be changed.
 *    Amount [I] -- The amount to move the point
 *    MinX [I] -- The min X point we can move the point
 *    MinY [I] -- The min Y point we can move the point
 *    MaxX [I] -- The max X point we can move the point
 *    MaxY [I] -- The max Y point we can move the point
 *
 * FUNCTION:
 *    This function moves a point backward or forward on the line, moving to
 *    the next / prev line as needed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::AdvancePoint(int &PX,int &PY,int Amount,int MinX,
        int MinY,int MaxX,int MaxY)
{
    int AmountLeft;
    struct DTPoint Point;
    int CharsOnLine;

    /* First find this line */
    if(!FindPoint(PX,PY,Point,Lines.end(),0))
        return;

    AmountLeft=Amount;
    if(AmountLeft<0)
        AmountLeft=-AmountLeft;

    while(AmountLeft>0)
    {
        if(Amount<0)
        {
            /* Backwards */
            if(PX-AmountLeft<0)
            {
                /* Wrap to prev line */
                AmountLeft-=PX;
                if(PY==0 || Point.Line==Lines.begin() || PY==MinY)
                {
                    /* No place to go, done */
                    PX=MinX;
                    AmountLeft=0;
                }
                else
                {
                    PY--;
                    Point.Line--;
                    PX=TextLine_FindLineLen(Point.Line);
                }
            }
            else
            {
                PX-=AmountLeft;
                if(PY==MinY && PX<MinX)
                    PX=MinX;
                AmountLeft=0;
            }
        }
        else
        {
            /* Forward */
            CharsOnLine=TextLine_FindLineLen(Point.Line);
            if(PX+AmountLeft>=CharsOnLine)
            {
                /* We are moving more that the line has, wrap */
                AmountLeft-=CharsOnLine-PX+1;   // +1 for the new line char
                if(PY==MaxY)
                {
                    /* No place to go, done */
                    PX=MaxX;
                    AmountLeft=0;
                }
                else
                {
                    PY++;
                    Point.Line++;
                    if(Point.Line==Lines.end())
                    {
                        /* We are at the end, back up and we are done */
                        Point.Line--;
                        PX=MaxX;
                        AmountLeft=0;
                    }
                    PX=0;
                }
            }
            else
            {
                PX+=AmountLeft;
                if(PY==MaxY && PX>MaxX)
                    PX=MaxX;
                AmountLeft=0;
            }
        }
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::FindWordStartEndPoints
 *
 * SYNOPSIS:
 *    void DisplayText::FindWordStartEndPoints(int &PX,int &PY,int &PX2,
 *              int &PY2);
 *
 * PARAMETERS:
 *    PX [I/O] -- The X point to move.  This will be changed to the start of
 *                the word.
 *    PY [I/O] -- The Y point.  If X goes before the start of the line or
 *                past the end this will be changed.  This will be changed to
 *                the start of the word.
 *    PX2 [O] -- The end of the word.
 *    PY2 [O] -- The end of the word.
 *
 * FUNCTION:
 *    This function finds the start and end of the word under a point.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    If there is no valid word (PX,PY on a space) then the returned values
 *    will be PX=PX2 PY=PY2.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::FindWordStartEndPoints(int &PX,int &PY,int &PX2,int &PY2)
{
    string Letter;
    struct DTPoint Point;
    int TmpY;

    PX2=PX;
    PY2=PY;

    /* Find the start of the word */
    for(;;)
    {
        if(!GetStringBetweenPoints(PX,PY,PX+1,PY,Letter))
            break;
        if(CheckWordBreak(Letter))
        {
            PX++;   // We want to be just after the word break
            break;
        }

        if(PX==0)
        {
            /* We are done at the start of the line */
            /* We need to check to see if this was a wrapped line */
            TmpY=PY-1;
            if(TmpY<0)
                break;

            if(!FindPoint(PX,TmpY,Point,Lines.end(),0))
                break;

            /* Check if the line is a wrapped line */
            if(Point.Line->EOL!=e_DTEOL_Soft)
                break;

            /* Ok, it was a soft line break, move to the end of the prev line
               and keep looking */
            PY--;
            if(PY<0)
            {
                PY=0;
                break;
            }
            PX=TextLine_FindLineLen(Point.Line);
        }
        else
        {
            /* Back up by 1 */
            AdvancePoint(PX,PY,-1,0,0,ScreenWidthChars,LinesCount);
        }
    }

    /* Find the end of the word */
    for(;;)
    {
        if(!GetStringBetweenPoints(PX2,PY2,PX2+1,PY2,Letter))
            break;
        if(CheckWordBreak(Letter))
            break;

        /* Get info about this line */
        if(!FindPoint(PX2,PY2,Point,Lines.end(),0))
            break;

        /* Go forward by 1 spot */
        TmpY=PY2;
        AdvancePoint(PX2,PY2,+1,0,0,ScreenWidthChars,LinesCount);
        if(TmpY!=PY2)
        {
            /* We changed lines, check to see if the last line was a
               wrapped line */
            if(Point.Line->EOL!=e_DTEOL_Soft)
                break;
        }
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::CheckWordBreak
 *
 * SYNOPSIS:
 *    bool DisplayText::CheckWordBreak(string &Letter);
 *
 * PARAMETERS:
 *    Letter [I] -- The letter to check
 *
 * FUNCTION:
 *    This function checks if a letter is a word break char or not
 *
 * RETURNS:
 *    true -- It's a word break char
 *    false -- It is not a word break char
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DisplayText::CheckWordBreak(string &Letter)
{
    if(Letter==" " || Letter==":" || Letter==";" || Letter=="\"" ||
            Letter=="\'" || Letter=="," || Letter=="." || Letter=="?" ||
            Letter=="\\" || Letter=="/" || Letter=="`" || Letter=="~" ||
            Letter=="|" || Letter=="(" || Letter==")" || Letter=="[" ||
            Letter=="]" || Letter=="!" || Letter=="@" || Letter=="#" ||
            Letter=="$" || Letter=="%" || Letter=="^" || Letter=="&" ||
            Letter=="*" || Letter=="-" || Letter=="+" || Letter=="=" ||
            Letter=="<" || Letter==">")
    {
        return true;
    }
    return false;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ApplyBGColor2Selection
 *
 * SYNOPSIS:
 *    void DisplayText::ApplyBGColor2Selection(uint32_t RGB);
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
void DisplayText::ApplyBGColor2Selection(uint32_t RGB)
{
    int SelX1;
    int SelY1;
    int SelX2;
    int SelY2;

    GetNormalizedSelection(SelX1,SelY1,SelX2,SelY2);

    ChangeAttribsBetweenPoints(SelX1,SelY1,SelX2,SelY2,0,0,RGB,0,
            DTXT_APPLY_BACKGROUND);

    RedrawFullScreen();
}

/*******************************************************************************
 * NAME:
 *    DisplayText::InvalidateAllMarks
 *
 * SYNOPSIS:
 *    void DisplayText::InvalidateAllMarks(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function takes all the allocated marks and marks them as invalid.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::InvalidateAllMarks(void)
{
    struct TextPointMarker *Marker;

    for(Marker=MarkerList;Marker!=NULL;Marker=Marker->Next)
        Marker->Valid=false;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::InvalidateOutOfRangeMarks
 *
 * SYNOPSIS:
 *    void DisplayText::InvalidateOutOfRangeMarks(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function goes thought all the valid marks and checks if they should
 *    be vaild.  If not it marks them as invalid.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::InvalidateOutOfRangeMarks(void)
{
    struct TextPointMarker *Marker;
    int ScreenFirstLineY;

    /* Invalidate any markers that are out of range */
    if(LinesCount<ScreenHeightChars)
        ScreenFirstLineY=0;
    else
        ScreenFirstLineY=LinesCount-ScreenHeightChars;

    for(Marker=MarkerList;Marker!=NULL;Marker=Marker->Next)
    {
        if(Marker->Valid)
        {
            if(Marker->Y<ScreenFirstLineY)
                Marker->Valid=false;
            if(Marker->Y>=LinesCount)
                Marker->Valid=false;
            if(Marker->X>=ScreenWidthChars)
                Marker->Valid=false;
        }
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::DoApplyToMark
 *
 * SYNOPSIS:
 *    void DisplayText::DoApplyToMark(t_DataProMark *Mark,uint32_t Change2,
 *              uint32_t Offset,uint32_t Len,uint32_t What);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *    Change2 [I] -- Change to this value (what this is depends on 'What')
 *    Offset [I] -- The number of chars from the mark to skip before starting
 *                  to remove the attribs.
 *    Len [I] -- The number of chars to remove these new attributes from.
 *    What [I] -- What to pass to ChangeAttribsBetweenPoints()
 *
 * FUNCTION:
 *    This is helper function used for the marks that moves where the mark is
 *    going to be applied.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::DoApplyToMark(t_DataProMark *Mark,uint32_t Change2,
        uint32_t Offset,uint32_t Len,uint32_t What)
{
    struct TextPointMarker *Marker=(struct TextPointMarker *)Mark;
    int PX;
    int PY;
    int StopX;
    int StopY;

    if(!Marker->Valid)
        return;

    GetMarkMinMaxPoints(Marker,PX,PY,StopX,StopY,Offset,Len);

    ChangeAttribsBetweenPoints(PX,PY,StopX,StopY,Change2,Change2,Change2,
            Change2,What);

    RedrawFullScreen();
}

/*******************************************************************************
 * NAME:
 *    DisplayText::AllocateMark
 *
 * SYNOPSIS:
 *    t_DataProMark *DisplayText::AllocateMark(void);
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
t_DataProMark *DisplayText::AllocateMark(void)
{
    struct TextPointMarker *NewMarker;

    try
    {
        NewMarker=new struct TextPointMarker;
        NewMarker->Valid=false;

        /* Add it to the list of markers */
        NewMarker->Prev=NULL;
        NewMarker->Next=MarkerList;
        if(MarkerList!=NULL)
            MarkerList->Prev=NewMarker;
        MarkerList=NewMarker;

        /* Move this marker to the current cursor pos */
        SetMark2CursorPos((t_DataProMark *)NewMarker);
    }
    catch(...)
    {
        NewMarker=NULL;
    }

    return (t_DataProMark *)NewMarker;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::FreeMark
 *
 * SYNOPSIS:
 *    void DisplayText::FreeMark(t_DataProMark *Mark);
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
 * NOTES:
 *    When a display text is free'ed the users of Mark's can't call FreeMark()
 *    because the connection is no longer connected.  There for when a
 *    display text is free'ed it must free the list of marks.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::FreeMark(t_DataProMark *Mark)
{
    struct TextPointMarker *Marker=(struct TextPointMarker *)Mark;

    /* Unlink this item and free it */
    if(MarkerList==Marker)
        MarkerList=Marker->Next;

    if(Marker->Next!=NULL)
        Marker->Next->Prev=Marker->Prev;

    if(Marker->Prev!=NULL)
        Marker->Prev->Next=Marker->Next;

    delete Marker;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::IsMarkValid
 *
 * SYNOPSIS:
 *    bool DisplayText::IsMarkValid(t_DataProMark *Mark);
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
bool DisplayText::IsMarkValid(t_DataProMark *Mark)
{
    struct TextPointMarker *Marker=(struct TextPointMarker *)Mark;

    return Marker->Valid;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::SetMark2CursorPos
 *
 * SYNOPSIS:
 *    void DisplayText::SetMark2CursorPos(t_DataProMark *Mark);
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
void DisplayText::SetMark2CursorPos(t_DataProMark *Mark)
{
    struct TextPointMarker *Marker=(struct TextPointMarker *)Mark;
    int CursorGlobalY;

    if(LinesCount<ScreenHeightChars)
        CursorGlobalY=CursorY;
    else
        CursorGlobalY=LinesCount-ScreenHeightChars+CursorY;

    Marker->Valid=true;
    Marker->X=CursorX;
    Marker->Y=CursorGlobalY;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ApplyAttrib2Mark
 *
 * SYNOPSIS:
 *    void DisplayText::ApplyAttrib2Mark(t_DataProMark *Mark,uint32_t Attrib,
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
 *    This function does the DPS_ApplyAttrib2Mark() function to the display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_ApplyAttrib2Mark()
 ******************************************************************************/
void DisplayText::ApplyAttrib2Mark(t_DataProMark *Mark,uint32_t Attrib,
        uint32_t Offset,uint32_t Len)
{
    DoApplyToMark(Mark,Attrib,Offset,Len,DTXT_APPLY_SET_ATTRIB);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::RemoveAttribFromMark
 *
 * SYNOPSIS:
 *    void DisplayText::RemoveAttribFromMark(t_DataProMark *Mark,uint32_t Attrib,
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
void DisplayText::RemoveAttribFromMark(t_DataProMark *Mark,uint32_t Attrib,
        uint32_t Offset,uint32_t Len)
{
    DoApplyToMark(Mark,Attrib,Offset,Len,DTXT_APPLY_CLR_ATTRIB);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ApplyFGColor2Mark
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
 *    This function does the DPS_ApplyFGColor2Mark() function to the active
 *    connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_ApplyFGColor2Mark()
 ******************************************************************************/
void DisplayText::ApplyFGColor2Mark(t_DataProMark *Mark,uint32_t FGColor,
        uint32_t Offset,uint32_t Len)
{
    DoApplyToMark(Mark,FGColor,Offset,Len,DTXT_APPLY_FOREGROUND);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::ApplyBGColor2Mark
 *
 * SYNOPSIS:
 *    void DisplayText::ApplyBGColor2Mark(t_DataProMark *Mark,uint32_t BGColor,
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
void DisplayText::ApplyBGColor2Mark(t_DataProMark *Mark,uint32_t BGColor,
        uint32_t Offset,uint32_t Len)
{
    DoApplyToMark(Mark,BGColor,Offset,Len,DTXT_APPLY_BACKGROUND);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::MoveMark
 *
 * SYNOPSIS:
 *    void DisplayText::MoveMark(t_DataProMark *Mark,int Amount);
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
void DisplayText::MoveMark(t_DataProMark *Mark,int Amount)
{
    struct TextPointMarker *Marker=(struct TextPointMarker *)Mark;
    int PX;
    int PY;
    int StopX;
    int StopY;

    if(!Marker->Valid)
        return;

    GetMarkMinMaxPoints(Marker,PX,PY,StopX,StopY,Amount,0);
    Marker->X=PX;
    Marker->Y=PY;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::GetMarkString
 *
 * SYNOPSIS:
 *    const uint8_t *DisplayText::GetMarkString(t_DataProMark *Mark,
 *          uint32_t *Size,uint32_t Offset,uint32_t Len);
 *
 * PARAMETERS:
 *    Mark [I] -- The mark to work on
 *    Size [O] -- The number of bytes in the returned buffer.
 *    Offset [I] -- The number of chars from the mark to skip before starting
 *                  to copy the string.
 *    Len [I] -- The number of chars to copy from this string.
 *
 * FUNCTION:
 *    This function does the DPS_GetMarkString() function to the display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    DPS_GetMarkString()
 ******************************************************************************/
const uint8_t *DisplayText::GetMarkString(t_DataProMark *Mark,uint32_t *Size,
        uint32_t Offset,uint32_t Len)
{
    struct TextPointMarker *Marker=(struct TextPointMarker *)Mark;
    int PX;
    int PY;
    int StopX;
    int StopY;

    *Size=0;
    if(!Marker->Valid)
        return NULL;

    GetMarkMinMaxPoints(Marker,PX,PY,StopX,StopY,Offset,Len);
    if(!GetStringBetweenPoints(PX,PY,StopX,StopY,GetMarkTextBuffer))
        return NULL;

    *Size=GetMarkTextBuffer.length();
    return (const uint8_t *)GetMarkTextBuffer.c_str();
}

/*******************************************************************************
 * NAME:
 *    DisplayText::GetMarkMinMaxPoints
 *
 * SYNOPSIS:
 *    void DisplayText::GetMarkMinMaxPoints(struct TextPointMarker *Marker,
 *          int &PX,int &PY,int &StopX,int &StopY,uint32_t Offset,uint32_t Len);
 *
 * PARAMETERS:
 *    Marker [I] -- The marker to work on
 *    PX [O] -- The min X
 *    PY [O] -- The min Y
 *    StopX [O] -- The max X
 *    StopY [O] -- The max Y
 *    Offset [I] -- How many chars to move (_+)
 *    Len [I] -- The number of chars to return (not bytes).  0 = all the chars
 *               we can.
 *
 * FUNCTION:
 *    This is helper function that finds the min and max points a marker can
 *    apply changes to.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::GetMarkMinMaxPoints(struct TextPointMarker *Marker,
        int &PX,int &PY,int &StopX,int &StopY,uint32_t Offset,uint32_t Len)
{
    int MaxCursorGlobalY;
    int ScreenFirstLineY;

    if(LinesCount<ScreenHeightChars)
    {
        MaxCursorGlobalY=MaxCursorY;
        ScreenFirstLineY=0;
    }
    else
    {
        MaxCursorGlobalY=LinesCount-ScreenHeightChars+MaxCursorY;
        ScreenFirstLineY=LinesCount-ScreenHeightChars;
    }

    /* Move by offset */
    PX=Marker->X;
    PY=Marker->Y;
    AdvancePoint(PX,PY,Offset,0,ScreenFirstLineY,MaxCursorX,MaxCursorY);

    /* Figure out where to stop by adding 'Len' to the current point */
    if(Len>0)
    {
        StopX=PX;
        StopY=PY;
        AdvancePoint(StopX,StopY,Len,0,ScreenFirstLineY,MaxCursorX,MaxCursorY);
    }
    else
    {
        StopX=MaxCursorX;
        StopY=MaxCursorGlobalY;
    }
}

/*******************************************************************************
 * NAME:
 *    DisplayText::GetSelectionRAW
 *
 * SYNOPSIS:
 *    uint8_t *DisplayText::GetSelectionRAW(unsigned int *Bytes);
 *
 * PARAMETERS:
 *    Bytes [O] -- The number of bytes in the selection.
 *
 * FUNCTION:
 *    This function gets a copy of the selection as RAW bytes.
 *
 * RETURNS:
 *    A buffer with the selection copied into it or NULL if there was an error.
 *    You must free this buffer with free() (it will be allocated with
 *    malloc()).
 *
 * SEE ALSO:
 *    IsThereASelection(), SelectAll(), ClearSelection()
 ******************************************************************************/
uint8_t *DisplayText::GetSelectionRAW(unsigned int *Bytes)
{
    std::string Clip;
    uint8_t *RetBuff;

    *Bytes=0;

    if(!GetSelectionString(Clip))
        return NULL;

    *Bytes=Clip.length();

    RetBuff=(uint8_t *)malloc(*Bytes+1);
    if(RetBuff==NULL)
        return NULL;

    memcpy(RetBuff,Clip.c_str(),*Bytes+1);

    return RetBuff;
}

/*******************************************************************************
 * NAME:
 *    DisplayText::MoveViewTop
 *
 * SYNOPSIS:
 *    void DisplayText::MoveViewTop(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function move the display to the top of the scroll back buffer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::MoveViewTop(void)
{
    t_UIScrollBarCtrl *HorzScroll;

    if(TextDisplayCtrl==NULL)
        return;

    HorzScroll=UITC_GetHorzSlider(UITC_GetTextDisplayPrimaryColumn(
            TextDisplayCtrl));

    WindowXOffsetPx=0;
    TopLine=Lines.begin();
    TopLineY=0;

    UITC_SetXOffset(UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
            WindowXOffsetPx);
    UISetScrollBarPos(HorzScroll,WindowXOffsetPx);
    UITC_SetCursorPos(TextDisplayCtrl,CursorX,CalcCorrectedCursorPos());
    RethinkCursorHidden();
    RedrawFullScreen();
}

/*******************************************************************************
 * NAME:
 *    DisplayText::MoveViewBottom
 *
 * SYNOPSIS:
 *    void DisplayText::MoveViewBottom(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function move the display to the bottom of the screen.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::MoveViewBottom(void)
{
    t_UIScrollBarCtrl *HorzScroll;

    if(TextDisplayCtrl==NULL)
        return;

    HorzScroll=UITC_GetHorzSlider(UITC_GetTextDisplayPrimaryColumn(
            TextDisplayCtrl));

    /* Scroll the window to be at the bottom */
    WindowXOffsetPx=0;
    TopLine=ScreenFirstLine;
    if(LinesCount>=ScreenHeightChars)
        TopLineY=LinesCount-ScreenHeightChars;
    else
        TopLineY=0;

    UITC_SetXOffset(UITC_GetTextDisplayPrimaryColumn(TextDisplayCtrl),
            WindowXOffsetPx);
    UISetScrollBarPos(HorzScroll,WindowXOffsetPx);
    UITC_SetCursorPos(TextDisplayCtrl,CursorX,CalcCorrectedCursorPos());
    RethinkCursorHidden();
    RedrawFullScreen();
}

/*******************************************************************************
 * NAME:
 *    DisplayText::MoveViewUpDown
 *
 * SYNOPSIS:
 *    void DisplayText::MoveViewUpDown(int Delta);
 *
 * PARAMETERS:
 *    Delta [I] -- How mucht to move up (neg) or down (positive)
 *
 * FUNCTION:
 *    This function moves the display to up or down (scrolls)
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::MoveViewUpDown(int Delta)
{
    ScrollScreen(0,Delta);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::MoveViewLeftRight
 *
 * SYNOPSIS:
 *    void DisplayText::MoveViewLeftRight(int Delta);
 *
 * PARAMETERS:
 *    Delta [I] -- How much to move left (neg) or right (positive)
 *
 * FUNCTION:
 *    This function moves the display to left or right (scrolls)
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::MoveViewLeftRight(int Delta)
{
    ScrollScreen(Delta*CharWidthPx,0);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::MoveViewHome
 *
 * SYNOPSIS:
 *    void DisplayText::MoveViewHome(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function moves the display to the start of the left side.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::MoveViewHome(void)
{
    ScrollScreen(-WindowXOffsetPx,0);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::MoveViewEnd
 *
 * SYNOPSIS:
 *    void DisplayText::MoveViewEnd(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function moves the display to the end of the right side.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::MoveViewEnd(void)
{
    int MaxLength;
    int ScreenWidth;
    int ScreenWidthPx;

    MaxLength=LongestLinePx;
    if(MaxLength<ScreenWidthChars*CharWidthPx)
        MaxLength=ScreenWidthChars*CharWidthPx;

    ScreenWidthPx=ScreenWidthChars*CharWidthPx;

    ScreenWidth=TextAreaWidthPx;
    if(Settings->TermSizeFixedWidth && TextAreaWidthPx>ScreenWidthPx)
        ScreenWidth=ScreenWidthPx;

    MaxLength=LongestLinePx;
    if(MaxLength<ScreenWidthChars*CharWidthPx)
        MaxLength=ScreenWidthChars*CharWidthPx;

    ScrollScreen(MaxLength-ScreenWidth,0);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::MovePageUp
 *
 * SYNOPSIS:
 *    void DisplayText::MovePageUp(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function moves the display 1 page up
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::MovePageUp(void)
{
    ScrollScreen(0,-WindowHeightChars);
}

/*******************************************************************************
 * NAME:
 *    DisplayText::MovePageDown
 *
 * SYNOPSIS:
 *    void DisplayText::MovePageDown(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function moves the display 1 page down
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayText::MovePageDown(void)
{
    ScrollScreen(0,WindowHeightChars);
}
