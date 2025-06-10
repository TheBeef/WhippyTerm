/*******************************************************************************
 * FILENAME: DisplayBinary.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This is the binary version of the display base.
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
#ifndef __DISPLAYBINARY_H_
#define __DISPLAYBINARY_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/Display/DisplayBase.h"
#include "UI/UITextMainArea.h"
#include "UI/UITimers.h"
#include <string>
#include <stdint.h>

/***  DEFINES                          ***/
#define MAX_BINARY_HEX_BYTES_PER_LINE                   64  // The max number of bytes we can have on a line

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
struct DisBin_SelectionBlock;

/***  CLASS DEFINITIONS                ***/
class DisplayBinary : public DisplayBase
{
    friend bool DisplayBinary_EventHandlerCB(const struct TextDisplayEvent *Event);
    friend void DisplayBin_ScrollTimer_Timeout(uintptr_t UserData);

    public:
        DisplayBinary();
        ~DisplayBinary();

        bool Init(void *ParentWidget,class ConSettings *SettingsPtr,bool (*EventCallback)(const struct DBEvent *Event),uintptr_t UserData);
        void Reparent(void *NewParentWidget);
        void WriteChar(uint8_t *Chr);
        void ClearScreen(e_ScreenClearType Type);
        void SetCursorStyle(e_TextCursorStyleType Style);
        void SetInFocus(void);
        void ClearScrollBackBuffer(void);
        void ResetTerm(void);
        t_UIContextMenuCtrl *GetContextMenuHandle(e_UITD_ContextMenuType UIObj);
        t_UIContextSubMenuCtrl *GetContextSubMenuHandle(e_UITD_ContextSubMenuType UIObj);
        void ClearSelection(void);
        bool GetSelectionString(std::string &Clip);
        void SelectAll(void);
        bool IsThereASelection(void);
        void ApplySettings(void);
        void SetCursorBlinking(bool Blinking);
        void ToggleAttribs2Selection(uint32_t Attribs);
        void ApplyBGColor2Selection(uint32_t RGB);
        bool IsAttribSetInSelection(uint32_t Attribs);
        uint8_t *GetSelectionRAW(unsigned int *Bytes);

    private:
        bool InitCalled;
        e_TextCursorStyleType CursorStyle;
        t_UITextDisplayCtrl *TextDisplayCtrl;
        struct UITimer *ScrollTimer;
        bool LeftMouseDown;         // Is the left mouse button down

        uint8_t *HexBuffer;             // This is a circular buffer
        uint8_t *EndOfHexBuffer;
        int HexBufferSize;
        struct CharStyling *ColorBuffer;    // This is a circular buffer with the styling info for 'HexBuffer' in it

        struct CharStyling *ColorBottomOfBufferLine;
        struct CharStyling *ColorTopOfBufferLine;
        struct CharStyling *ColorTopLine;
        uint8_t *TopOfBufferLine;       // Where we read data from 'HexBuffer'.  This is the oldest data
        uint8_t *BottomOfBufferLine;    // Where we insert new data in 'HexBuffer'.  This points to the start of the line
        uint8_t InsertPoint;            // The insert offset from 'BottomOfBufferLine' (BottomOfBufferLine[InsertPoint])
        uint8_t *TopLine;               // The first line of the display window (where we are scrolled to).  This is relitive to 'Top of Buffer'

        int ScreenWidthPx;
        int ScreenHeightPx;
        int CharWidthPx;
        int CharHeightPx;
        int DisplayLines;
        int WindowXOffsetPx;

        /* Selection */
        bool SelectionActive;       // Is there an active selection
        bool SelectionInAscII;
        uint8_t *SelectionLine;
        int SelectionLineOffset;
        uint8_t *SelectionAnchorLine;
        int SelectionLineAnchorOffset;
        int AutoSelectionScrolldx;
        int AutoSelectionScrolldy;

        bool DoTextDisplayCtrlEvent(const struct TextDisplayEvent *Event);
        void RedrawCurrentLine(void);
        void ScreenResize(void);
        void SetupCanvas(void);
        void SetDrawMask(uint16_t Mask);
        void RethinkYScrollBar(void);
        void RethinkWindowSize(void);
        void RedrawScreen(void);
        void DrawLine(const uint8_t *Line,const struct CharStyling *ColorLine,
                int ScreenLine,unsigned int Bytes);
        bool ScrollBarAtBottom(void);
        void RethinkCursor(void);
        bool ConvertScreenXY2BufferLinePtr(int x,int y,uint8_t **Ptr,int *Offset,bool *InAscII);
        void HandleLeftMousePress(bool Down,int x,int y);
        void HandleMouseMove(int x,int y);
        void DoScrollTimerTimeout(void);
        void ScrollScreen(int dxpx,int dy);
        bool GetNormalizedSelectionBlocks(struct DisBin_SelectionBlock *Blocks);
        void BuildSelOutputAndAppendData(std::string &Dest,const uint8_t *Src,int Bytes,bool AscII);
        bool DoHexInputEvent(const struct HDEvent *Event);
        t_UITextInputCtrl *GetSendPanel_HexPosInput(void);
        t_UIRadioBttnCtrl *GetSendPanel_HexRadioBttn(void);
        t_UIRadioBttnCtrl *GetSendPanel_TextRadioBttn(void);
        t_UIMuliLineTextInputCtrl *GetSendPanel_TextInput(void);
        t_UIComboBoxCtrl *GetSendPanel_LineEndInput(void);
        void SendPanel_ShowHexOrText(bool Text);
        struct CharStyling *GetColorPtrFromLinePtr(const uint8_t *Line);
        void FillAttrib(struct DisBin_SelectionBlock *SelBlock,uint32_t Attribs,bool Set);
        bool CheckIfAttribSet(struct DisBin_SelectionBlock *SelBlock,uint32_t Attribs);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
