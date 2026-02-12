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
#include "App/Display/HexDisplayBuffers.h"
#include "UI/UIMainWindow.h"
#include "UI/UITextMainArea.h"
#include "UI/UITextDefs.h"
#include <stdint.h>
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

typedef enum
{
    e_DBEvent_MouseMiddlePress,
    e_DBEvent_LeftMousePress,
    e_DBEvent_KeyEvent,
    e_DBEvent_SelectionChanged,
    e_DBEvent_SendBlockData,
    e_DBEvent_FocusChange,
    e_DBEvent_MouseMouseWheel,
    e_DBEvent_ContextMenu,
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

typedef enum
{
    e_Block_LineEnd_CRLF,
    e_Block_LineEnd_CR,
    e_Block_LineEnd_LF,
    e_Block_LineEnd_TAB,
    e_Block_LineEnd_ESC,
    e_Block_LineEnd_NULL,
    e_Block_LineEnd_None,
    e_Block_LineEndMAX
} e_Block_LineEndType;

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

struct DBEventContextMenu
{
    e_UITD_ContextMenuType Menu;
};

union DBEventData
{
    struct DBEventKeyPress Key;
    struct DBEventMouse Mouse;
    struct DBEventBlockSend BlockSend;
    struct DBEventFocusInfo Focus;
    struct DBEventMouseWheel MouseWheel;
    struct DBEventContextMenu Context;
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
    friend bool DisplayBase_HexInputEvent(const struct HDEvent *Event);

    public:
        DisplayBase();
        virtual ~DisplayBase();
        virtual bool Init(void *ParentWidget,class ConSettings *SettingsPtr,bool (*EventCallback)(const struct DBEvent *Event),uintptr_t UserData)=0;
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
        virtual void SetInFocus(void);
        virtual void SetCursorXY(unsigned int x,unsigned y);
        virtual void GetCursorXY(unsigned int *x,unsigned int *y);
        virtual void AddTab(void);
        virtual void AddReverseTab(void);
        virtual void DoBackspace(void);
        virtual void DoReturn(void);
        virtual void DoLineFeed(void);
        virtual bool GetSelectionString(std::string &Clip)=0;
        virtual bool IsThereASelection(void)=0;
        virtual void SelectAll(void)=0;
        virtual void ClearSelection(void)=0;
        virtual void SetOverrideMessage(const char *Msg);
        virtual void ClearScreen(e_ScreenClearType Type);
        virtual void ClearArea(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2);
        virtual void ScrollArea(uint32_t X1,uint32_t Y1,uint32_t X2,uint32_t Y2,int32_t dx,int32_t dy);
        virtual void ClearScrollBackBuffer(void);
        virtual void InsertHorizontalRule(void);
        virtual void ResetTerm(void);
        virtual void SetupCanvas(void);
        virtual void SetDrawMask(uint16_t Mask);
        virtual t_UIContextMenuCtrl *GetContextMenuHandle(e_UITD_ContextMenuType UIObj);
        virtual t_UIContextSubMenuCtrl *GetContextSubMenuHandle(e_UITD_ContextSubMenuType UIObj);
        virtual void ShowBell(void);
        virtual void ToggleAttribs2Selection(uint32_t Attribs);
        virtual void ApplyBGColor2Selection(uint32_t RGB);
        virtual bool IsAttribSetInSelection(uint32_t Attribs);
        virtual uint8_t *GetSelectionRAW(unsigned int *Bytes);
        virtual t_DataProMark *AllocateMark(void);
        virtual void FreeMark(t_DataProMark *Mark);
        virtual bool IsMarkValid(t_DataProMark *Mark);
        virtual void SetMark2CursorPos(t_DataProMark *Mark);
        virtual void ApplyAttrib2Mark(t_DataProMark *Mark,uint32_t Attrib,uint32_t Offset,uint32_t Len);
        virtual void RemoveAttribFromMark(t_DataProMark *Mark,uint32_t Attrib,uint32_t Offset,uint32_t Len);
        virtual void ApplyFGColor2Mark(t_DataProMark *Mark,uint32_t FGColor,uint32_t Offset,uint32_t Len);
        virtual void ApplyBGColor2Mark(t_DataProMark *Mark,uint32_t BGColor,uint32_t Offset,uint32_t Len);
        virtual void MoveMark(t_DataProMark *Mark,int Amount);
        virtual const uint8_t *GetMarkString(t_DataProMark *Mark,uint32_t *Size,uint32_t Offset,uint32_t Len);
        virtual void GetScreenSize(uint32_t *Width,uint32_t *Height);

        virtual void MoveViewTop(void);
        virtual void MoveViewBottom(void);
        virtual void MoveViewUpDown(int Delta);
        virtual void MoveViewLeftRight(int Delta);
        virtual void MoveViewHome(void);
        virtual void MoveViewEnd(void);
        virtual void MovePageUp(void);
        virtual void MovePageDown(void);

        void SetCustomSettings(class ConSettings *NewSettingsPtr);
        class ConSettings *GetCustomSettings(void);
        void GetFont(std::string &CurFontName,int &CurFontSize,bool &CurFontBold,bool &CurFontItalic);
        void SetFont(const std::string &NewFontName,int NewFontSize,bool NewFontBold,bool NewFontItalic);
        void SetupHexInput(t_UIFrameContainerCtrl *ParentWid);
        void FreeHexInput(void);

        struct CharStyling CurrentStyle;

    protected:
        bool InitBase(bool (*EventCallback)(const struct DBEvent *Event),uintptr_t UserData);
        bool (*DBEventHandler)(const struct DBEvent *Event);
        uintptr_t EventHandlerUserData;
        void SendEvent(e_DBEventType EventID,const union DBEventData *Info);
        bool DoHexInputEvent(const struct HDEvent *Event);
        virtual t_UITextInputCtrl *GetSendPanel_HexPosInput(void);
        virtual t_UIRadioBttnCtrl *GetSendPanel_HexRadioBttn(void);
        virtual t_UIRadioBttnCtrl *GetSendPanel_TextRadioBttn(void);
        virtual t_UIMuliLineTextInputCtrl *GetSendPanel_TextInput(void);
        virtual t_UIComboBoxCtrl *GetSendPanel_LineEndInput(void);
        virtual void SendPanel_ShowHexOrText(bool Text);

        void DoBlock_ClearHexInput(void);
        void DoBlock_Send(void);
        void DoBlock_EditHex(void);
        void DoBlock_RadioBttnChange(void);
        void Block_SetHexOrTextMode(bool TextMode);

        class HexDisplayBuffer *HexInput;

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
