/*******************************************************************************
 * FILENAME: DisplayBase.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the base defines and types.
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
 *    Paul Hutchinson (03 Aug 2023)
 *       Created
 *
 *******************************************************************************/
#ifndef __DISPLAYBASE_H_
#define __DISPLAYBASE_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIMainWindow.h"
#include "UI/UITextDefs.h"
#include <stdint.h>
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

typedef enum
{
    e_DBEvent_MouseMiddlePress,
    e_DBEvent_KeyEvent,
    e_DBEvent_SelectionChanged,
    e_DBEvent_SendBlockData,
    e_DBEvent_FocusChange,
    e_DBEvent_MouseMouseWheel,
    e_DBEventMAX
} e_DBEventType;

typedef enum
{
    e_ScreenClear_Clear,
    e_ScreenClear_Scroll,
    e_ScreenClear_ScrollAll,
    e_ScreenClear_ScrollWithHR,
    e_ScreenClearMAX
} e_ScreenClearType;

struct DBEventKeyPress
{
    uint8_t Mods;
    e_UIKeys Key;
    const uint8_t *TextPtr;
    unsigned int TextLen;
};

struct DBEventMouseWheel
{
    uint8_t Mods;
    int Steps;
};

struct DBEventMouse
{
    int x;
    int y;
};

struct DBEventBlockSend
{
    const uint8_t *Buffer;
    int Len;
};

struct DBEventFocusInfo
{
    bool HasFocus;
};

union DBEventData
{
    struct DBEventKeyPress Key;
    struct DBEventMouse Mouse;
    struct DBEventBlockSend BlockSend;
    struct DBEventFocusInfo Focus;
    struct DBEventMouseWheel MouseWheel;
};

struct DBEvent
{
    e_DBEventType EventType;
    uintptr_t UserData;
    const union DBEventData *Info;
};

/***  CLASS DEFINITIONS                ***/
class DisplayBase
{
    public:
        DisplayBase();
        virtual ~DisplayBase();
        virtual bool Init(void *ParentWidget,bool (*EventCallback)(const struct DBEvent *Event),uintptr_t UserData)=0;
        virtual void Reparent(void *NewParentWidget)=0;
        virtual void SetBlockDeviceMode(bool On);
        virtual void WriteChar(uint8_t *Chr)=0;
        virtual void NoteNonPrintable(const char *NoteStr);
        virtual void SetShowNonPrintable(bool Show);
        virtual void SetShowEndOfLines(bool Show);
        virtual void ApplySettings(void);
        virtual void SetCursorBlinking(bool Blinking);
        virtual void SetCursorStyle(e_TextCursorStyleType Style);
        virtual bool GetInFocus(void);
        virtual void SetCursorXY(unsigned int x,unsigned y);
        virtual void GetCursorXY(unsigned int *x,unsigned int *y);
        virtual void AddTab(void);
        virtual void DoBackspace(void);
        virtual void DoReturn(void);
        virtual void DoLineFeed(void);
        virtual bool GetSelectionString(std::string &Clip);
        virtual bool IsThereASelection(void);
        virtual void SelectAll(void);
        virtual void ClearSelection(void);
        virtual void SetOverrideMessage(const char *Msg);
        virtual void ClearScreen(e_ScreenClearType Type);
        virtual void ClearScrollBackBuffer(void);
        virtual void InsertHorizontalRule(void);
        virtual void ResetTerm(void);
        virtual void SetupCanvas(void);

        void SetCustomSettings(class ConSettings *NewSettingsPtr);
        class ConSettings *GetCustomSettings(void);
        void GetFont(std::string &CurFontName,int &CurFontSize,bool &CurFontBold,bool &CurFontItalic);
        void SetFont(const std::string &NewFontName,int NewFontSize,bool NewFontBold,bool NewFontItalic);

        struct CharStyling CurrentStyle;

    protected:
        bool InitBase(bool (*EventCallback)(const struct DBEvent *Event),uintptr_t UserData);
        bool (*DBEventHandler)(const struct DBEvent *Event);
        uintptr_t EventHandlerUserData;
        void SendEvent(e_DBEventType EventID,const union DBEventData *Info);

        class ConSettings *Settings;
        bool HasFocus;

        /* What font we are rendering in */
        std::string FontName;
        int FontSize;
        bool FontBold;
        bool FontItalic;
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
