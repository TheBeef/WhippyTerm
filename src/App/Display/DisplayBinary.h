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
#include "UI/UITextDisplay.h"
#include <stdint.h>

/***  DEFINES                          ***/
#define MAX_BINARY_HEX_BYTES_PER_LINE                   64  // The max number of bytes we can have on a line

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/
class DisplayBinary : public DisplayBase
{
    friend bool DisplayBinary_EventHandlerCB(const struct TextDisplayEvent *Event);

    public:
        DisplayBinary();
        ~DisplayBinary();

        bool Init(void *ParentWidget,bool (*EventCallback)(const struct DBEvent *Event),uintptr_t UserData);
        void Reparent(void *NewParentWidget);
        void WriteChar(uint8_t *Chr);

    private:
        bool InitCalled;
        t_UITextDisplayCtrl *TextDisplayCtrl;

        uint8_t *HexBuffer;             // This is a circular buffer
        uint8_t *EndOfHexBuffer;
        int HexBufferSize;

        uint8_t *ProLine;               // Where we insert new data in 'HexBuffer'.  This points to the start of the line
        uint8_t InsertPoint;            // The insert offset from 'ProLine' (ProLine[InsertPoint])
        uint8_t *ConLine;               // Where we read data from 'HexBuffer'.  This is the oldest data
        uint8_t *TopLine;               // The first line of the display window (where we are scrolled to)

        int ScreenWidthPx;
        int ScreenHeightPx;
        int CharWidthPx;
        int CharHeightPx;

        bool DoTextDisplayCtrlEvent(const struct TextDisplayEvent *Event);
        void RedrawCurrentLine(void);
        void ScreenResize(void);
        void SetupCanvas(void);
        void RethinkScrollBars(void);
        void RethinkWindowSize(void);
        void RedrawScreen(void);
        void DrawLine(const uint8_t *Line,int ScreenLine,int Bytes);
        void MakeCurrentLineVisble(void);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
