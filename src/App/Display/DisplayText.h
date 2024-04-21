/*******************************************************************************
 * FILENAME: DisplayText.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This is the text version of the display base.
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
 * HISTORY:
 *    Paul Hutchinson (06 Aug 2023)
 *       Created
 *
 *******************************************************************************/
#ifndef __DISPLAYTEXT_H_
#define __DISPLAYTEXT_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UITextDisplay.h"
#include "App/Display/DisplayBase.h"
#include "UI/UITimers.h"
#include <stdint.h>
#include <string>
#include <list>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_DTEOL_Soft,
    e_DTEOL_Hard,
    e_DTEOLMAX
} e_DTEOLType;

struct TextLineFrag
{
    e_TextCanvasFragType FragType;
    std::string Text;
    struct CharStyling Styling;
    int WidthPx;
    int Value;
    void *Data;
};

typedef std::list<struct TextLineFrag> t_TextLineFrags;
typedef t_TextLineFrags::iterator i_TextLineFrags;

struct TextLine
{
    t_TextLineFrags Frags;
    int LineWidthPx;
    uint32_t LineBackgroundColor;
    e_DTEOLType EOL;
};

typedef std::list<struct TextLine> t_TextLines;
typedef t_TextLines::iterator i_TextLines;

/***  CLASS DEFINITIONS                ***/

/*


           / +-------------------+
          /  |                   |
         /   |    Back Buffer    |
        /    |                   |
       /     |  +----------------|---+   <- TopLine
      /      |  |                |   |
     /       |  | Display Window |   |
Lines        |  |                |   |
     \       |  +----------------|---+
      \      |                   |
       \     |-------------------|       <- ScreenFirstLine
        \    |                   |
         \   |    Screen Area    |
          \  | X                 |       <- CursorY
           \ +-------------------+

Back Buffer: The history of the screen area
Screen Area: The area the cursor lives

CursorY is relative to 'ScreenFirstLine'
ActiveLine is relative to 'ScreenFirstLine'

*/



class DisplayText : public DisplayBase
{
    friend bool DisplayText_EventHandlerCB(const struct TextDisplayEvent *Event);
    friend void DisplayText_ScrollTimer_Timeout(uintptr_t UserData);

    public:
        DisplayText();
        ~DisplayText();

        bool Init(void *ParentWidget,bool (*EventCallback)(const struct DBEvent *Event),uintptr_t UserData);
        void Reparent(void *NewParentWidget);
        void WriteChar(uint8_t *Chr);
        void NoteNonPrintable(const char *NoteStr);
        void SetShowNonPrintable(bool Show);
        void SetShowEndOfLines(bool Show);
        void ApplySettings(void);
        void SetCursorBlinking(bool Blinking);
        void SetCursorStyle(e_TextCursorStyleType Style);
        void SetCursorXY(unsigned int x,unsigned y);
        void GetCursorXY(unsigned int *x,unsigned int *y);
        void AddTab(void);
        void DoBackspace(void);
        void DoReturn(void);
        void DoLineFeed(void);
        bool GetSelectionString(std::string &Clip);
        bool IsThereASelection(void);
        void SetOverrideMessage(const char *Msg);

        void SetBlockDeviceMode(bool On);

    private:
        t_UITextDisplayCtrl *TextDisplayCtrl;

        bool InitCalled;
        std::string TmpStr; // A temp string (so we don't have keep allocating it)
        std::string TmpStr2; // A temp string (so we don't have keep allocating it)
        struct UITimer *ScrollTimer;

        bool ShowNonPrintables;
        bool ShowEndOfLines;
        int CursorX;
        int CursorY;
        bool LeftMouseDown;         // Is the left mouse button down
        int AutoSelectionScrolldx;
        int AutoSelectionScrolldy;
        int CursorXPx;

        int CharWidthPx;
        int CharHeightPx;

        e_TextCursorStyleType CursorStyle;
        e_TextCursorStyleType LastCursorStyle;
        bool LastCursorVisible;

        /* Text Area (the total size of the text area in the widget) in chars */
        int TextAreaWidthPx;
        int TextAreaHeightPx;

        /* Screen Area (the area the cursor can move in) in chars */
        int ScreenWidthChars;
        int ScreenHeightChars;
        i_TextLines ScreenFirstLine;    // The line the cursor lives in below
        struct TextLine *ActiveLine;    // The line with the cursor on it
        int ActiveLineY;                // What line count (from 'ScreenFirstLine') is 'ActiveLine'

        /* Window (the area the user is looking at) */
        int WindowHeightChars;
        int WindowXOffsetPx;
        i_TextLines TopLine;            // The top line we are displaying
        int TopLineY;                   // The number of lines from the top of 'Lines' that 'TopLine' is on

        /* Lines */
        int LongestLinePx;
        t_TextLines Lines;
        int LinesCount;                 // Lines.size(), but tracked (faster)
        i_TextLineFrags InsertFrag;
        int InsertPos;

        /* Selection */
        bool SelectionValid;        // Is the selection valid
        int Selection_X;            // The selection left pos (in chars)
        int Selection_Y;            // The selection top pos (in lines)
        int Selection_AnchorX;      // The selection end left pos (in chars)
        int Selection_AnchorY;      // The selection end top pos (in lines)

        void ScrollScreenBy1Line(void);
        bool DoTextDisplayCtrlEvent(const struct TextDisplayEvent *Event);
        void DoScrollTimerTimeout(void);
        void SetupCanvas(void);
        void RedrawActiveLine(void);
        void RedrawFullScreen(void);
        bool RethinkInsertFrag(void);
        void AppendChar(uint8_t *Chr);
        i_TextLineFrags AddNewEmptyFragToActiveLine(i_TextLineFrags InsertPoint);
        void DoOverwriteInsertPos(uint8_t *Chr);
        void PadOutCurrentLine2Cursor(void);
        void AdjustCursorAfterWriteChar(uint8_t *Chr);
        void RethinkTextAreaSize(void);
        void RethinkWindowSize(void);
        void RethinkLineLengths(void);
        void ReFindLongestLineLength(void);
        void MoveCursor(unsigned int x,unsigned y,bool CursorXPxPrecaled);
        void RethinkScrollBars(void);
        int GetLineEndSize(struct TextLine *Line);
        int DrawLine(int LineY,int ScreenLine,struct TextLine *Line);

        void RethinkFragWidth(i_TextLineFrags Frag);
        void HandleDeletingNPAfterOverwrite(void);

        i_TextLineFrags AddSpecialFrag(struct TextLineFrag &SpecialFrag);
        i_TextLineFrags InsertSpecialFragInMiddleOfString(struct TextLineFrag &SpecialFrag);

        bool CursorLineVisible(void);
        void RethinkCursorHidden(void);
        int CalcCorrectedCursorPos(void);
        void MoveToNextLine(int &NewCursorY);
        bool ScrollBarAtBottom(void);
        void ScrollScreen(int dx,int dy);

        void HandleLeftMousePress(bool Down,int x,int y);
        void HandleMouseMove(int x,int y);
        void CalSelectionPoint(int x,int y,int *SelX,int *SelY);
        void GetNormalizedSelection(int &X1,int &Y1,int &X2,int &Y2);
        bool ConvertScreenXY2Chars(int x,int y,int *CharX,int *CharY);
        void ScrollScreen2MakeCursorVisible(void);
        int CalcCursorXPx(void);
        int CharUnderCursorWidthPx(void);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
