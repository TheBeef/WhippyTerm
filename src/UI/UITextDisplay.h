/*******************************************************************************
 * FILENAME: UITextDisplay.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the access functions and defines for the text display
 *    control in it (it is a custom control and big enough that it gets
 *    it's own file outside of UIControl.h)
 *
 * COPYRIGHT:
 *    Copyright 2023 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (07 Aug 2023)
 *       Created
 *
 *******************************************************************************/
#ifndef __UITEXTDISPLAY_H_
#define __UITEXTDISPLAY_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/KeyDefines.h"
#include "UI/UITextDefs.h"
#include "UI/UIControl.h"
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
struct UITextDisplay {int x;};
typedef struct UITextDisplay t_UITextDisplayCtrl;

/* Events */
typedef enum
{
    e_TextDisplayEvent_DisplayFrameScrollX,
    e_TextDisplayEvent_DisplayFrameScrollY,
    e_TextDisplayEvent_MouseDown,
    e_TextDisplayEvent_MouseUp,
    e_TextDisplayEvent_MouseRightDown,
    e_TextDisplayEvent_MouseRightUp,
    e_TextDisplayEvent_MouseMiddleDown,
    e_TextDisplayEvent_MouseMiddleUp,
    e_TextDisplayEvent_MouseWheel,
    e_TextDisplayEvent_MouseMove,
    e_TextDisplayEvent_Resize,
    e_TextDisplayEvent_LostFocus,
    e_TextDisplayEvent_GotFocus,
    e_TextDisplayEvent_KeyEvent,
    e_TextDisplayEvent_SendBttn,
    e_TextDisplayEventMAX
} e_TextDisplayEventType;

struct TextDisplayEventFrameScroll
{
    int Amount;
};

struct TextDisplayEventDataXY
{
    int x;
    int y;
};

struct TextDisplayEventDataSize
{
    int Width;
    int Height;
};

struct TextDisplayEventDataWheel
{
    int Steps;
    int Mods;
};

struct TextDisplayEventKeyPress
{
    uint8_t Mods;
    e_UIKeys Key;
    const uint8_t *TextPtr;
    unsigned int TextLen;
};

struct TextDisplayEventSendBttn
{
    const uint8_t *Buffer;
    int Len;
};

union TextDisplayEventData
{
    struct TextDisplayEventFrameScroll Scroll;
    struct TextDisplayEventDataXY Mouse;
    struct TextDisplayEventDataWheel MouseWheel;
    struct TextDisplayEventDataSize NewSize;
    struct TextDisplayEventKeyPress Key;
    struct TextDisplayEventSendBttn SendBttn;
};

struct TextDisplayEvent
{
    e_TextDisplayEventType EventType;
    uintptr_t ID;
    t_UITextDisplayCtrl *Ctrl;
    union TextDisplayEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

/* Allocate */
t_UITextDisplayCtrl *UITC_AllocTextDisplay(void *ParentWidget,
        bool (*EventHandler)(const struct TextDisplayEvent *Event),uintptr_t ID);
void UITC_FreeTextDisplay(t_UITextDisplayCtrl *ctrl);
void UITC_Reparent(t_UITextDisplayCtrl *ctrl,void *NewParentWidget);

/* Control */
void UITC_ShowSendPanel(t_UITextDisplayCtrl *ctrl,bool Visible);
void UITC_SetCursorBlinking(t_UITextDisplayCtrl *ctrl,bool Blinking);
void UITC_SetCursorPos(t_UITextDisplayCtrl *ctrl,unsigned int x,unsigned int y);
void UITC_SetFocus(t_UITextDisplayCtrl *ctrl);

/* Sub-Widgets */
t_UIScrollBarCtrl *UITC_GetHorzSlider(t_UITextDisplayCtrl *ctrl);
t_UIScrollBarCtrl *UITC_GetVertSlider(t_UITextDisplayCtrl *ctrl);

/* Info */
int UITC_GetFragWidth(t_UITextDisplayCtrl *ctrl,const struct TextCanvasFrag *Frag);
int UITC_GetWidgetWidth(t_UITextDisplayCtrl *ctrl);
int UITC_GetWidgetHeight(t_UITextDisplayCtrl *ctrl);
int UITC_GetCharPxHeight(t_UITextDisplayCtrl *ctrl);
int UITC_GetCharPxWidth(t_UITextDisplayCtrl *ctrl);

/* Setup */
void UITC_SetFont(t_UITextDisplayCtrl *ctrl,const char *FontName,int Size,bool Bold,bool Italic);
void UITC_SetCursorColor(t_UITextDisplayCtrl *ctrl,uint32_t Color);
void UITC_SetCursorStyle(t_UITextDisplayCtrl *ctrl,e_TextCursorStyleType Style);
void UITC_SetClippingWindow(t_UITextDisplayCtrl *ctrl,int LeftEdge,int TopEdge,
        int Width,int Height);
void UITC_SetTextAreaBackgroundColor(t_UITextDisplayCtrl *ctrl,uint32_t BgColor);
void UITC_SetBorderBackgroundColor(t_UITextDisplayCtrl *ctrl,uint32_t BgColor,bool Fill);
void UITC_SetOverrideMsg(t_UITextDisplayCtrl *ctrl,const char *Msg,bool OnOff);

/* Rendering */
void UITC_ClearAllLines(t_UITextDisplayCtrl *ctrl);
void UITC_Begin(t_UITextDisplayCtrl *ctrl,int Line);
void UITC_End(t_UITextDisplayCtrl *ctrl);
void UITC_ClearLine(t_UITextDisplayCtrl *ctrl,uint32_t BGColor);
void UITC_AddFragment(t_UITextDisplayCtrl *ctrl,const struct TextCanvasFrag *Frag);
void UITC_SetXOffset(t_UITextDisplayCtrl *ctrl,int XOffsetPx);
void UITC_SetMaxLines(t_UITextDisplayCtrl *ctrl,int MaxLines);
void UITC_RedrawScreen(t_UITextDisplayCtrl *ctrl);

#endif
