/*******************************************************************************
 * FILENAME: UIEditSendBuffer.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the access functions for the edit send buffer dialog in it.
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (16 Aug 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __UIEDITSENDBUFFER_H_
#define __UIEDITSENDBUFFER_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
enum e_ESB_Button
{
    e_ESB_Button_Clear,
    e_ESB_Button_Fill,
    e_ESB_Button_EndianSwap,
    e_ESB_Button_CRCType,
    e_ESB_Button_InsertCRC,
    e_ESB_Button_InsertAsNumber,
    e_ESB_Button_InsertAsText,
    e_ESB_Button_InsertProperties,
    e_ESB_ButtonMAX
};

enum e_ESB_TextInput
{
    e_ESB_TextInput_SelectedBytes,
    e_ESB_TextInput_Offset,
    e_ESB_TextInput_CRC,
    e_ESB_TextInput_BufferSize,
    e_ESB_TextInput_InsertTxt,
    e_ESB_TextInput_BufferName,
    e_ESB_TextInputMAX
};

enum e_ESB_ContextMenu
{
    e_ESB_ContextMenu_LoadBuffer,
    e_ESB_ContextMenu_SaveBuffer,
    e_ESB_ContextMenu_ExportData,
    e_ESB_ContextMenu_ImportData,
    e_ESB_ContextMenu_InsertFromDisk,
    e_ESB_ContextMenu_InsertFromClipboard,
    e_ESB_ContextMenuMAX
};

typedef enum
{
    e_ESBEvent_BttnTriggered,
    e_ESBEvent_TextEditDone,
    e_ESBEvent_ContextMenuClicked,
    e_ESBEventMAX
} e_ESBEventType;

struct ESBEventDataBttn
{
    e_ESB_Button BttnID;
};
struct ESBEventDataText
{
    e_ESB_TextInput TxtID;
};
struct ESBEventDataContextMenu
{
    e_ESB_ContextMenu MenuID;
};

union ESBEventData
{
    struct ESBEventDataBttn Bttn;
    struct ESBEventDataText Txt;
    struct ESBEventDataContextMenu Context;
};

struct ESBEvent
{
    e_ESBEventType EventType;
    uintptr_t ID;
    union ESBEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_EditSendBuffer(void);
void UIFree_EditSendBuffer(void);
bool UIShow_EditSendBuffer(void);
t_UIButtonCtrl *UIESB_GetButton(e_ESB_Button bttn);
t_UIContainerFrameCtrl *UIESB_GetHexContainerFrame(void);
t_UITextInputCtrl *UIESB_GetTextInput(e_ESB_TextInput UIObj);
t_UIContextMenuCtrl *UIESB_GetContextMenu(e_ESB_ContextMenu UIObj);

bool ESB_Event(const struct ESBEvent *Event);

#endif
