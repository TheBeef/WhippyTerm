/*******************************************************************************
 * FILENAME: UICustomTextWidget.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the access functions and defines for our custom text
 *    display in it.
 *
 * COPYRIGHT:
 *    Copyright 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (29 Apr 2025)
 *       Created
 *
 *******************************************************************************/
#ifndef __UICUSTOMTEXTWIDGET_H_
#define __UICUSTOMTEXTWIDGET_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/KeyDefines.h"
#include "UI/UIMouse.h"
#include "UI/UITextDefs.h"
#include "UI/UIControl.h"
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
struct UICustomTextWidget {int x;};
typedef struct UICustomTextWidget t_UICustomTextWidgetCtrl;

typedef enum
{
    e_UICTW_ContextMenu_Copy,
    e_UICTW_ContextMenu_Paste,
    e_UICTW_ContextMenu_ClearScreen,
    e_UICTW_ContextMenu_ZoomIn,
    e_UICTW_ContextMenu_ZoomOut,
    e_UICTW_ContextMenu_Edit,
    e_UICTW_ContextMenu_EndianSwap,
    e_UICTW_ContextMenu_FindCRCAlgorithm,
    e_UICTW_ContextMenu_CopyToSendBuffer,
    e_UICTW_ContextMenuMAX
} e_UICTW_ContextMenuType;

/* Events */
typedef enum
{
    e_UICTWEvent_DisplayFrameScrollX,
    e_UICTWEvent_DisplayFrameScrollY,
    e_UICTWEvent_MouseDown,
    e_UICTWEvent_MouseUp,
    e_UICTWEvent_MouseRightDown,
    e_UICTWEvent_MouseRightUp,
    e_UICTWEvent_MouseMiddleDown,
    e_UICTWEvent_MouseMiddleUp,
    e_UICTWEvent_MouseWheel,
    e_UICTWEvent_MouseMove,
    e_UICTWEvent_Resize,
    e_UICTWEvent_LostFocus,
    e_UICTWEvent_GotFocus,
    e_UICTWEvent_KeyEvent,
    e_UICTWEvent_ContextMenu,
    e_UICTWEventMAX
} e_UICTWEventType;

struct UICTWEventFrameScroll
{
    int Amount;
};

struct UICTWEventDataXY
{
    int x;
    int y;
};

struct UICTWEventDataSize
{
    int Width;
    int Height;
};

struct UICTWEventDataWheel
{
    int Steps;
    int Mods;
};

struct UICTWEventKeyPress
{
    uint8_t Mods;
    e_UIKeys Key;
    const uint8_t *TextPtr;
    unsigned int TextLen;
};

struct UICTWEventContextMenu
{
    e_UICTW_ContextMenuType Menu;
};

union UICTWEventData
{
    struct UICTWEventFrameScroll Scroll;
    struct UICTWEventDataXY Mouse;
    struct UICTWEventDataWheel MouseWheel;
    struct UICTWEventDataSize NewSize;
    struct UICTWEventKeyPress Key;
    struct UICTWEventContextMenu Context;
};

struct UICTWEvent
{
    e_UICTWEventType EventType;
    uintptr_t ID;
    t_UICustomTextWidgetCtrl *Ctrl;
    union UICTWEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTWON PROTOTYPES     ***/

/* Allocate */
t_UICustomTextWidgetCtrl *UICTW_AllocCustomTextWidget(
        t_UIFrameContainerCtrl *ParentWidget,
        bool (*EventHandler)(const struct UICTWEvent *Event),uintptr_t ID);
void UICTW_FreeCustomTextWidget(t_UICustomTextWidgetCtrl *ctrl);
void UICTW_Reparent(t_UICustomTextWidgetCtrl *ctrl,void *NewParentWidget);

/* Control */
void UICTW_SetCursorBlinking(t_UICustomTextWidgetCtrl *ctrl,bool Blinking);
void UICTW_SetCursorPos(t_UICustomTextWidgetCtrl *ctrl,unsigned int x,unsigned int y);
void UICTW_SetFocus(t_UICustomTextWidgetCtrl *ctrl);

/* Sub-Widgets */
t_UIScrollBarCtrl *UICTW_GetHorzSlider(t_UICustomTextWidgetCtrl *ctrl);
t_UIScrollBarCtrl *UICTW_GetVertSlider(t_UICustomTextWidgetCtrl *ctrl);
t_UIContextMenuCtrl *UICTW_GetContextMenuHandle(t_UICustomTextWidgetCtrl *ctrl,e_UICTW_ContextMenuType UIObj);

/* Info */
int UICTW_GetFragWidth(t_UICustomTextWidgetCtrl *ctrl,const struct TextCanvasFrag *Frag);
int UICTW_GetWidgetWidth(t_UICustomTextWidgetCtrl *ctrl);
int UICTW_GetWidgetHeight(t_UICustomTextWidgetCtrl *ctrl);
int UICTW_GetCharPxHeight(t_UICustomTextWidgetCtrl *ctrl);
int UICTW_GetCharPxWidth(t_UICustomTextWidgetCtrl *ctrl);

/* Setup */
void UICTW_SetFont(t_UICustomTextWidgetCtrl *ctrl,const char *FontName,int Size,bool Bold,bool Italic);
void UICTW_SetCursorColor(t_UICustomTextWidgetCtrl *ctrl,uint32_t Color);
void UICTW_SetCursorStyle(t_UICustomTextWidgetCtrl *ctrl,e_TextCursorStyleType Style);
void UICTW_SetClippingWindow(t_UICustomTextWidgetCtrl *ctrl,int LeftEdge,int TopEdge,
        int Width,int Height);
void UICTW_SetTextAreaBackgroundColor(t_UICustomTextWidgetCtrl *ctrl,uint32_t BgColor);
void UICTW_SetTextDefaultColor(t_UICustomTextWidgetCtrl *ctrl,uint32_t FgColor);
void UICTW_SetBorderBackgroundColor(t_UICustomTextWidgetCtrl *ctrl,uint32_t BgColor,bool Fill);

/* Rendering */
void UICTW_ClearAllLines(t_UICustomTextWidgetCtrl *ctrl);
void UICTW_Begin(t_UICustomTextWidgetCtrl *ctrl,int Line);
void UICTW_End(t_UICustomTextWidgetCtrl *ctrl);
void UICTW_ClearLine(t_UICustomTextWidgetCtrl *ctrl,uint32_t BGColor);
void UICTW_AddFragment(t_UICustomTextWidgetCtrl *ctrl,const struct TextCanvasFrag *Frag);
void UICTW_SetXOffset(t_UICustomTextWidgetCtrl *ctrl,int XOffsetPx);
void UICTW_SetMaxLines(t_UICustomTextWidgetCtrl *ctrl,int MaxLines,uint32_t BGColor);
void UICTW_RedrawScreen(t_UICustomTextWidgetCtrl *ctrl);
void UICTW_SetDrawMask(t_UICustomTextWidgetCtrl *ctrl,uint16_t Mask);

void UICTW_SetMouseCursor(t_UICustomTextWidgetCtrl *ctrl,e_UIMouse_CursorType Cursor);

#endif   /* end of "#ifndef __UICUSTOMTEXTWIDGET_H_" */
