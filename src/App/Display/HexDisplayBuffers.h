/*******************************************************************************
 * FILENAME: HexDisplayBuffers.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (15 May 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __HEXDISPLAYBUFFERS_H_
#define __HEXDISPLAYBUFFERS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/Display/DisplayBase.h"
#include "UI/UITextDisplay.h"
#include "UI/UIClipboard.h"
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_HDBCFormat_Default,
    e_HDBCFormat_HexDump,
    e_HDBCFormat_Hex,
    e_HDBCFormat_AscII,
    e_HDBCFormat_RAW,
    e_HDBCFormatMAX
} e_HDBCFormatType;

/* Event */
typedef enum
{
    e_HDEvent_SelectionEvent,
    e_HDEvent_MouseMove,
    e_HDEvent_CursorMove,
    e_HDEvent_BufferResize,
    e_HDEventMAX
} e_HDEventType;

struct HDEventKeyPress
{
    uint8_t Mods;
    e_UIKeys Key;
    const uint8_t *TextPtr;
    unsigned int TextLen;
};

struct HDEventSelectionChange
{
    bool SelectionValid;
    const uint8_t *StartOfSel;
    const uint8_t *EndOfSel;
};

struct HDEventCursorMove
{
    int Pos;
};

struct HDEventBufferInfo
{
    int Size;
};

union HDEventData
{
    struct HDEventKeyPress Key;
    struct HDEventSelectionChange Select;
    struct HDEventCursorMove Cursor;
    struct HDEventBufferInfo Buffer;
};

struct HDEvent
{
    e_HDEventType EventType;
    uintptr_t ID;
    union HDEventData Info;
};

typedef enum
{
    e_HDB_EditState_FirstNib,
    e_HDB_EditState_SecondNib,
    e_HDB_EditStateMAX
} e_HDB_EditStateType;

struct HDB_RBD_LineInfo;

/***  CLASS DEFINITIONS                ***/
class HexDisplayBuffer
{
    public:
        HexDisplayBuffer();
        ~HexDisplayBuffer();
        bool Init(void *ParentWidget,bool (*EventHandler)(const struct HDEvent *Event),uintptr_t ID);
        bool SetBuffer(uint8_t *Data,int Size);
        bool SetBuffer(const uint8_t *Data,int Size);
        void SetBufferSize(int Size);
        void RebuildDisplay(void);

        void GiveFocus(void);
        void SetCanvasSize(int Width,int Height);
        void ApplySettingsChange(void);
        bool EventHandler(const struct TextDisplayEvent *Event);
        void SetFont(const char *FontName,int Size,bool Bold,bool Italic);
        void SetColors(uint32_t NewFGColor,uint32_t NewBGColor,uint32_t NewSelBGColor);
        void SetDisplayParms(uint8_t *InsertPos,bool BufferIsCircular);
        void SetDisplayParms(const uint8_t *InsertPos,bool BufferIsCircular);
        void Enable(bool Enable);
        void ClearSelection(void);
        bool GetSelectionBounds(const uint8_t **StartPtr,const uint8_t **EndPtr);
        void SetSelectionBounds(const uint8_t *StartPtr,const uint8_t *EndPtr);
        bool GetBufferInfo(const uint8_t **StartOfBuffer,int *Size);
        int GetCursorPos(void);
        void SetCursorPos(int NewPos);
        int GetAnchorPos(void);
        int GetSizeOfSelection(e_HDBCFormatType ClipFormat);
        void CopySelection2Buffer(uint8_t *OutBuff,e_HDBCFormatType ClipFormat);
        void SendSelection2Clipboard(e_ClipboardType Clip,e_HDBCFormatType Format);
        void SetLossOfFocusBehavior(bool HideSelection);
        bool SetEditMode(void);
        void FillSelectionWithValue(uint8_t Value);
        void FillWithValue(int InsertOffset,const uint8_t *Data,int Bytes,bool Replace);

    private:
        t_UITextDisplayCtrl *TextDisplayCtrl;
        uintptr_t HDID;
        bool (*HDEventHandler)(const struct HDEvent *Event);
        bool FirstResize;   // Is this the resize call we have gotten since we where constructed
        bool ResizeNeeded;
        bool HasFocus;      // Does this control have focus
        bool HideSelectionOnLossOfFocus;
        bool LeftMouseDown; // Is the left mouse button down
        uint8_t *Buffer;          // The start of the memory with the data in it
        uint8_t *StartOfData;     // The point in 'Buffer' where we start drawing data from.  This is an adjusted 'InsertPos'
        uint8_t *InsertPos;       // The point in 'Buffer' where the next byte of data will be inserted.  When the buffer is circular this also is the oldest data
        bool BufferIsCircular;          // Is 'Buffer' flat (and data starts at 'Buffer' or is it circular data starts at 'InsertPos'
        int BufferSize;             // The number of bytes in 'Buffer'
        int BufferAllocatedSize;    // This is the number of bytes 'Buffer' has been allocated with.  This is used because when we grow/shrink 'Buffer' we allocate more bytes than is actually in 'BufferSize'.  This is so we don't realloc the buffer everytime a byte is added.
        int BufferBytes2Draw;
        int TopLine;
        std::string FontName;
        int FontSize;
        bool FontBold;
        bool FontItalic;
        uint32_t FGColor;
        uint32_t BGColor;
        uint32_t SelBGColor;
        bool DisplayEnabled;
        int WindowXOffsetPx;

        /* Selection / Cursor */
        bool InAscIIArea;
        int Selection_OrgAnchor;
        int Selection_Anchor;
        int Cursor_Pos;                 // Offset from the start of the buffer
        int Last_Cursor_Pos;
        bool SelectionMouseDown;
        bool SelectionValid;
        e_HDB_EditStateType EditMode;

        /* Edit vars */
        bool WeAllocBuffer;
        bool InEditMode;
        bool InInsertMode;
        uint8_t EditByteValue;
        bool DoingDotInputChar;
        bool DoingCycleInputChar;
        uint8_t NibCycleValue;

        /* View */
        int View_WidthPx;   // The width of the view in pixels
        int View_HeightPx;  // The height of the view in pixels
        int View_CharsX;    // The number of char's are are visible on the screen
        int View_CharsY;    // The number of char's are are visible on the screen
        int CharWidthPx;  // The width (px) of a char in the display font
        int CharHeightPx; // The height (px) of a char in the display font
        int LastTotalLines; // The total number of lines last time we updated the scroll bars
        int LastView_CharsY;    // The number of display chars we had the last time we updated the scroll bars

        /* Members */
        void SetupCanvas(void);
        void RethinkYScrollBar(void);
        void RethinkXScrollBar(void);
        bool KeyPress(uint8_t Mods,e_UIKeys Key,const uint8_t *TextPtr,unsigned int TextLen);
        void MouseWheelMoved(int Steps,uint8_t Mods);
        void HandleSelectionMouseUpDown(bool BttnDown,int x,int y);
        int CalSelectionFromMouse(int x,int y,bool &InAscII,bool &OutOfBounds);
        void HandleSelectionMouseUpDownMove(int x,int y);
        void MakeOffsetVisable(int Offset,bool ShowInAscII,bool RedrawScreen);
        void HandleSelectionScrollFromMouse(int x,int y,bool RedrawScreen);
        void SendSelectionEvent(void);
        void RethinkCursorLook(void);
        void RethinkCursorPos(void);
        void ClipCursorPos(bool SelectionRules);
        void EditHandleNibbleInput(int Nib);
        void AbortEdit(void);
        bool GrowBufferAsNeeded(int NewBufferSize);
        void EditHandleNibbleCycle(int Dir);
        bool ReadyForAddingChar(void);
        void AcceptCycleInput(void);
        void AbortDotInput(void);
        void SetNewBufferSize(int NewSize);
        void RebuildDisplay_ClearStyleHelper(struct CharStyling *style);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
