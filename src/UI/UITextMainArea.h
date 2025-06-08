/*******************************************************************************
 * FILENAME: UITextMainArea.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the access functions and defines for the main frame that
 *    has the text display control in it (it is a custom control and big
 *    enough that it gets it's own file outside of UIControl.h) as well as
 *    any controls for that text area (basicly the contents of a main window
 *    tab).
 *
 *    See also: UICustomTextInput.h for just the text input area without the
 *    send panel and other things that go in a tab.
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
 *    Paul Hutchinson (07 Aug 2023)
 *       Created
 *
 *******************************************************************************/
#ifndef __UITEXTDISPLAY_H_
#define __UITEXTDISPLAY_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/KeyDefines.h"
#include "UI/UIMouse.h"
#include "UI/UITextDefs.h"
#include "UI/UIControl.h"
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
struct UITextDisplay {int x;};
typedef struct UITextDisplay t_UITextDisplayCtrl;

typedef enum
{
    e_UITD_ContextMenu_SendBuffers,
    e_UITD_ContextMenu_FindCRCAlgorithm,
    e_UITD_ContextMenu_Copy,
    e_UITD_ContextMenu_Paste,
    e_UITD_ContextMenu_ClearScreen,
    e_UITD_ContextMenu_ZoomIn,
    e_UITD_ContextMenu_ZoomOut,
    e_UITD_ContextMenu_Edit,
    e_UITD_ContextMenu_EndianSwap,
    e_UITD_ContextMenu_Bold,
    e_UITD_ContextMenu_Italics,
    e_UITD_ContextMenu_Underline,
    e_UITD_ContextMenu_StrikeThrough,
    e_UITD_ContextMenu_StyleBGColor_Black,
    e_UITD_ContextMenu_StyleBGColor_Blue,
    e_UITD_ContextMenu_StyleBGColor_Green,
    e_UITD_ContextMenu_StyleBGColor_Cyan,
    e_UITD_ContextMenu_StyleBGColor_Red,
    e_UITD_ContextMenu_StyleBGColor_Magenta,
    e_UITD_ContextMenu_StyleBGColor_Brown,
    e_UITD_ContextMenu_StyleBGColor_White,
    e_UITD_ContextMenu_StyleBGColor_Gray,
    e_UITD_ContextMenu_StyleBGColor_LightBlue,
    e_UITD_ContextMenu_StyleBGColor_LightGreen,
    e_UITD_ContextMenu_StyleBGColor_LightCyan,
    e_UITD_ContextMenu_StyleBGColor_LightRed,
    e_UITD_ContextMenu_StyleBGColor_LightMagenta,
    e_UITD_ContextMenu_StyleBGColor_Yellow,
    e_UITD_ContextMenu_StyleBGColor_BrightWhite,
    e_UITD_ContextMenuMAX
} e_UITD_ContextMenuType;

typedef enum
{
    e_UITD_ContextSubMenu_BGColor,
    e_UITD_ContextSubMenuMAX
} e_UITD_ContextSubMenuType;

typedef enum
{
    e_UITC_Bttn_Send,
    e_UITC_Bttn_HexEdit,
    e_UITC_Bttn_Clear,
    e_UITC_BttnMAX
} e_UITC_BttnType;

typedef enum
{
    e_UITC_Txt_Pos,
    e_UITC_TxtMAX
} e_UITC_TxtType;

typedef enum
{
    e_UITC_RadioButton_Text,
    e_UITC_RadioButton_Hex,
    e_UITC_RadioButtonMAX
} e_UITC_RadioButtonType;

typedef enum
{
    e_UITC_Combox_LineEnd,
    e_UITC_ComboxMAX
} e_UITC_ComboxType;

typedef enum
{
    e_UITC_MuliTxt_TextInput,
    e_UITC_MuliTxtMAX
} e_UITC_MuliTxtType;

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
    e_TextDisplayEvent_ContextMenu,
    e_TextDisplayEvent_ButtonPress,
    e_TextDisplayEvent_ComboxChange,
    e_TextDisplayEvent_RadioButtonPress,
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

struct TextDisplayEventButtonPress
{
    e_UITC_BttnType Bttn;
};

struct TextDisplayEventCombox
{
    uintptr_t ID;
    e_UITC_ComboxType BoxID;
};

struct TextDisplayEventRadioBttn
{
    e_UITC_RadioButtonType BttnID;
};

struct TextDisplayEventContextMenu
{
    e_UITD_ContextMenuType Menu;
};

union TextDisplayEventData
{
    struct TextDisplayEventFrameScroll Scroll;
    struct TextDisplayEventDataXY Mouse;
    struct TextDisplayEventDataWheel MouseWheel;
    struct TextDisplayEventDataSize NewSize;
    struct TextDisplayEventKeyPress Key;
    struct TextDisplayEventContextMenu Context;
    struct TextDisplayEventButtonPress ButtonPress;
    struct TextDisplayEventCombox Combox;
    struct TextDisplayEventRadioBttn RadioButton;
};

struct TextDisplayEvent
{
    e_TextDisplayEventType EventType;
    uintptr_t ID;
    t_UITextDisplayCtrl *Ctrl;
    union TextDisplayEventData Info;
};

typedef enum
{
    e_UITCSetFocus_Main,
    e_UITCSetFocus_SendPanel,
    e_UITCSetFocusMAX
} e_UITCSetFocusType;

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
void UITC_SendPanelShowHexOrTextInput(t_UITextDisplayCtrl *ctrl,bool ShowText);
void UITC_SetCursorBlinking(t_UITextDisplayCtrl *ctrl,bool Blinking);
void UITC_SetCursorPos(t_UITextDisplayCtrl *ctrl,unsigned int x,unsigned int y);
void UITC_SetFocus(t_UITextDisplayCtrl *ctrl,e_UITCSetFocusType What);
void UITC_SetSelectionAvailable(t_UITextDisplayCtrl *ctrl,bool Available);

/* Sub-Widgets */
t_UIScrollBarCtrl *UITC_GetHorzSlider(t_UITextDisplayCtrl *ctrl);
t_UIScrollBarCtrl *UITC_GetVertSlider(t_UITextDisplayCtrl *ctrl);
t_UIContextMenuCtrl *UITC_GetContextMenuHandle(t_UITextDisplayCtrl *ctrl,e_UITD_ContextMenuType UIObj);
t_UIContextSubMenuCtrl *UITC_GetContextSubMenuHandle(t_UITextDisplayCtrl *ctrl,e_UITD_ContextSubMenuType UIObj);
t_UIContainerFrameCtrl *UITC_GetSendHexDisplayContainerFrameCtrlHandle(t_UITextDisplayCtrl *ctrl);
t_UIButtonCtrl *UITC_GetButtonHandle(t_UITextDisplayCtrl *ctrl,e_UITC_BttnType Bttn);
t_UITextInputCtrl *UITC_GetTextInputHandle(t_UITextDisplayCtrl *ctrl,e_UITC_TxtType Txt);
t_UIRadioBttnCtrl *UITC_GetRadioButton(t_UITextDisplayCtrl *ctrl,e_UITC_RadioButtonType bttn);
t_UIComboBoxCtrl *UITC_GetComboBoxHandle(t_UITextDisplayCtrl *ctrl,e_UITC_ComboxType UIObj);
t_UIMuliLineTextInputCtrl *UITC_GetMuliLineTextInputHandle(t_UITextDisplayCtrl *ctrl,e_UITC_MuliTxtType Txt);

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
void UITC_SetTextDefaultColor(t_UITextDisplayCtrl *ctrl,uint32_t FgColor);
void UITC_SetBorderBackgroundColor(t_UITextDisplayCtrl *ctrl,uint32_t BgColor,bool Fill);
void UITC_SetOverrideMsg(t_UITextDisplayCtrl *ctrl,const char *Msg,bool OnOff);

/* Rendering */
void UITC_ClearAllLines(t_UITextDisplayCtrl *ctrl);
void UITC_Begin(t_UITextDisplayCtrl *ctrl,int Line);
void UITC_End(t_UITextDisplayCtrl *ctrl);
void UITC_ClearLine(t_UITextDisplayCtrl *ctrl,uint32_t BGColor);
void UITC_AddFragment(t_UITextDisplayCtrl *ctrl,const struct TextCanvasFrag *Frag);
void UITC_SetXOffset(t_UITextDisplayCtrl *ctrl,int XOffsetPx);
void UITC_SetMaxLines(t_UITextDisplayCtrl *ctrl,int MaxLines,uint32_t BGColor);
void UITC_RedrawScreen(t_UITextDisplayCtrl *ctrl);
void UITC_SetDrawMask(t_UITextDisplayCtrl *ctrl,uint16_t Mask);

void UITC_ShowBellIcon(t_UITextDisplayCtrl *ctrl);
void UITC_SetMouseCursor(t_UITextDisplayCtrl *ctrl,e_UIMouse_CursorType Cursor);


#endif
