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
#include "UI/UITextMainArea.h"
#include "App/Display/DisplayBase.h"
#include "UI/UITimers.h"
#include <stdint.h>
#include <string>
#include <list>

/***  DEFINES                          ***/
#define DTXT_APPLY_SET_ATTRIB   0x0001
#define DTXT_APPLY_CLR_ATTRIB   0x0002
#define DTXT_APPLY_FOREGROUND   0x0004
#define DTXT_APPLY_BACKGROUND   0x0008
#define DTXT_APPLY_ULINE_COLOR  0x0010

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

struct TextPointMarker
{
    bool Valid;
    int X;      // The marker left pos (in chars)
    int Y;      // The marker top pos (in lines)
    struct TextPointMarker *Prev;
    struct TextPointMarker *Next;
};

struct DTPoint
{
    i_TextLines Line;               // The line in 'Lines'
    int LineY;                      // The number of lines from the start of the buffer (where 'Line' lives)
    i_TextLineFrags Frag;           // The frag in 'Line'
    int_fast32_t StrPos;            // The offset into 'Frag' string
};

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

        bool Init(void *ParentWidget,class ConSettings *SettingsPtr,bool (*EventCallback)(const struct DBEvent *Event),uintptr_t UserData);
        void Reparent(void *NewParentWidget);
        void WriteChar(uint8_t *Chr);
        void WriteCharWithOptions(uint8_t *Chr,bool AdvCursor);
        void NoteNonPrintable(const char *NoteStr);
        void SetShowNonPrintable(bool Show);
        void SetShowEndOfLines(bool Show);
        void ApplySettings(void);
        void SetCursorBlinking(bool Blinking);
        void SetCursorStyle(e_TextCursorStyleType Style);
        void SetInFocus(void);
        void SetCursorXY(unsigned int x,unsigned y);
        void GetCursorXY(unsigned int *x,unsigned int *y);
        void AddTab(void);
        void AddReverseTab(void);
        void DoBackspace(void);
        void DoReturn(void);
        void DoLineFeed(void);
        bool GetSelectionString(std::string &Clip);
        bool IsThereASelection(void);
        void SelectAll(void);
        void ClearSelection(void);
        void SetOverrideMessage(const char *Msg);
        void ClearScreen(e_ScreenClearType Type);
        void ClearArea(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2);
        void ScrollArea(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2,int32_t dx,int32_t dy);
        void ClearScrollBackBuffer(void);
        void InsertHorizontalRule(void);
        void ResetTerm(void);
        void SetupCanvas(void);
        void SetDrawMask(uint16_t Mask);
        t_UIContextMenuCtrl *GetContextMenuHandle(e_UITD_ContextMenuType UIObj);
        t_UIContextSubMenuCtrl *GetContextSubMenuHandle(e_UITD_ContextSubMenuType UIObj);
        void ShowBell(void);
        void ToggleAttribs2Selection(uint32_t Attribs);
        void ApplyBGColor2Selection(uint32_t RGB);
        bool IsAttribSetInSelection(uint32_t Attribs);
        uint8_t *GetSelectionRAW(unsigned int *Bytes);

        t_DataProMark *AllocateMark(void);
        void FreeMark(t_DataProMark *Mark);
        bool IsMarkValid(t_DataProMark *Mark);
        void SetMark2CursorPos(t_DataProMark *Mark);
        void ApplyAttrib2Mark(t_DataProMark *Mark,uint32_t Attrib,uint32_t Offset,uint32_t Len);
        void RemoveAttribFromMark(t_DataProMark *Mark,uint32_t Attrib,uint32_t Offset,uint32_t Len);
        void ApplyFGColor2Mark(t_DataProMark *Mark,uint32_t FGColor,uint32_t Offset,uint32_t Len);
        void ApplyBGColor2Mark(t_DataProMark *Mark,uint32_t BGColor,uint32_t Offset,uint32_t Len);
        void MoveMark(t_DataProMark *Mark,int Amount);
        const uint8_t *GetMarkString(t_DataProMark *Mark,uint32_t *Size,uint32_t Offset,uint32_t Len);

        void SetBlockDeviceMode(bool On);

    private:
        t_UITextDisplayCtrl *TextDisplayCtrl;

        bool InitCalled;
        std::string TmpStr; // A temp string (so we don't have keep allocating it)
        std::string TmpStr2; // A temp string (so we don't have keep allocating it)
        struct UITimer *ScrollTimer;

        bool ShowNonPrintables;
        bool ShowEndOfLines;
        bool LeftMouseDown;         // Is the left mouse button down
        int AutoSelectionScrolldx;
        int AutoSelectionScrolldy;

        int CharWidthPx;
        int CharHeightPx;

        /* Cursor */
        int CursorX;                // In chars
        int CursorY;
        e_TextCursorStyleType CursorStyle;
        e_TextCursorStyleType LastCursorStyle;
        bool LastCursorVisible;
        int CursorXPx;              // In pixels
        int MaxCursorX;             // The max value we have been moved to for this line (reset every time Y changes)
        int MaxCursorY;             // The max value we have been moved to for this screen
        int LastCursorY;            // The value that we set the cursor to when the cursor was last moved

        /* Text Area (the total size of the text area in the widget) in chars */
        int TextAreaWidthPx;
        int TextAreaHeightPx;

        /* Screen Area (the area the cursor can move in) in chars */
        int_fast32_t ScreenWidthChars; // The number of chars wide is the screen area
        int_fast32_t ScreenHeightChars;// The number of chars height is the screen area
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
        int InsertPos;                  // The offset into the current string frag's 'Text' (also used as a flag see DisplayText::RethinkInsertFrag())

        /* Selection */
        bool SelectionActive;       // Is there an active selection (may not be valid, but we are selecting something)
        int Selection_X;            // The selection left pos (in chars)
        int Selection_Y;            // The selection top pos (in lines)
        int Selection_AnchorX;      // The selection end left pos (in chars)
        int Selection_AnchorY;      // The selection end top pos (in lines)

        /* Markers */
        struct TextPointMarker *MarkerList;
        std::string GetMarkTextBuffer;

        bool DoTextDisplayCtrlEvent(const struct TextDisplayEvent *Event);
        void DoScrollTimerTimeout(void);
        void RedrawActiveLine(void);
        void AppendChar(uint8_t *Chr);
        void DoOverwriteInsertPos(uint8_t *Chr);
        void PadOutCurrentLine2Cursor(void);
        void PadOutLine(struct TextLine *Line,int Pos);
        void PadOutScreen(int Y);
        void AdjustCursorAfterWriteChar(uint8_t *Chr);
        void RethinkTextAreaSize(void);
        void RethinkWindowSize(void);
        void RethinkLineLengths(void);
        void ReFindLongestLineLength(void);
        void MoveCursor(unsigned int x,unsigned y,bool CursorXPxPrecaled);
        void RethinkScrollBars(void);
        int GetLineEndSize(struct TextLine *Line);
        int DrawLine(int LineY,int ScreenLine,struct TextLine *Line);

        void HandleDeletingNPAfterOverwrite(void);

        void RethinkCursorHidden(void);
        int CalcCorrectedCursorPos(void);
        void MoveToNextLine(int &NewCursorY);
        void HandleLeftMousePress(bool Down,int x,int y);
        void HandleMouseMove(int x,int y);
        int CharUnderCursorWidthPx(void);
        void ScrollVertAreaDown(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2,int32_t dy);
        void ScrollVertAreaUp(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2,int32_t dy);
        void DEBUG_ForceRedrawOfScreen(void);
        int CalcCursorXPx(void);

        /* NOTE PAUL: These comment headings where added long after so they might
           be missing functions from the grouping */

        /* Screen handling */
        bool CursorLineVisible(void);
        bool ScrollBarAtBottom(void);
        void ScrollScreen(int dx,int dy);
        void ScrollScreen2MakeCursorVisible(void);
        bool ConvertScreenXY2Chars(int x,int y,int *CharX,int *CharY);
        void PadOutScreenWithBlankLines(void);
        void ScrollScreenByXLines(int Lines2Scroll);
        void RedrawFullScreen(void);

        /* Selection handling */
        void GetNormalizedSelection(int &X1,int &Y1,int &X2,int &Y2);
        bool FindPointsOfSelection(struct DTPoint &Start,struct DTPoint &End);

        /* Points (X,Y stuff) */
        bool FindPoint(int PX,int PY,struct DTPoint &Pos,i_TextLines HintLine,int HintStartY);
        void FindPointHelper_FindDelta(int PY,i_TextLines StartLine,int StartingY,unsigned int *LastMinDelta,i_TextLines *RetStartLine,int *RetStartingY);
        void AdvancePoint(int &PX,int &PY,int Amount,int MinX,int MinY,int MaxX,int MaxY);

        /* Line handling */
        bool IsLineBlank(i_TextLines Line);
        bool TextLine_FindFragAndPos(i_TextLines Line,int_fast32_t Offset,i_TextLineFrags *FoundFrag,int_fast32_t *FoundPos);
        void TextLine_SplitFrag(i_TextLines Line,int_fast32_t Offset,bool SplitEvenIfResultEmpty=false,i_TextLineFrags *Frag1=NULL,i_TextLineFrags *Frag2=NULL);
        void TextLine_ErasePos2Pos(i_TextLines Line,int_fast32_t StartOffset,int_fast32_t EndOffset);
        void TextLine_Insert(i_TextLines Line,int_fast32_t Offset,const struct CharStyling *Style,const char *Str);
        void TextLine_Fill(i_TextLines Line,int_fast32_t Offset,uint_fast32_t Count,const struct CharStyling *Style,const char *Chr);
        void TextLine_Clear(i_TextLines Line);
        int TextLine_FindLineLen(i_TextLines Line);

        /* Frag handling */
        void TxtFrag_SplitFrag(i_TextLines Line,i_TextLineFrags Frag,int_fast32_t StrPos,bool SplitEvenIfResultEmpty,i_TextLineFrags *Frag1,i_TextLineFrags *Frag2);
        i_TextLineFrags FindLastTextFragOnLine(const i_TextLines &Line);
        i_TextLineFrags AddSpecialFrag(struct TextLineFrag &SpecialFrag);
        i_TextLineFrags InsertSpecialFragInMiddleOfString(struct TextLineFrag &SpecialFrag);
        void RethinkFragWidth(i_TextLineFrags Frag);
        bool RethinkInsertFrag(void);
        i_TextLineFrags AddNewEmptyFragToLine(struct TextLine *Line,i_TextLineFrags InsertPoint);

        /* Blocks */
        void ChangeAttribsBetweenPoints(int P1X,int P1Y,int P2X,int P2Y,uint32_t Attribs,uint32_t FGColor,uint32_t BGColor,uint32_t ULineColor,uint32_t What);
        bool GetStringBetweenPoints(int P1X,int P1Y,int P2X,int P2Y,std::string &Clip);

        /* Send panel */
        t_UITextInputCtrl *GetSendPanel_HexPosInput(void);
        t_UIRadioBttnCtrl *GetSendPanel_HexRadioBttn(void);
        t_UIRadioBttnCtrl *GetSendPanel_TextRadioBttn(void);
        t_UIMuliLineTextInputCtrl *GetSendPanel_TextInput(void);
        t_UIComboBoxCtrl *GetSendPanel_LineEndInput(void);
        void SendPanel_ShowHexOrText(bool Text);

        /* Markers */
        void InvalidateAllMarks(void);
        void InvalidateOutOfRangeMarks(void);
        void DoApplyToMark(t_DataProMark *Mark,uint32_t Attrib,uint32_t Offset,uint32_t Len,uint32_t What);
        void GetMarkMinMaxPoints(struct TextPointMarker *Marker,int &PX,int &PY,int &StopX,int &StopY,uint32_t Offset,uint32_t Len);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
