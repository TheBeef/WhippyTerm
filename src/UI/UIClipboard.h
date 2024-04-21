/*******************************************************************************
 * FILENAME: UIClipboard.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This has the clip board API in it.
 *
 * COPYRIGHT:
 *    Copyright 2020 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (10 Jul 2020)
 *       Created
 *
 *******************************************************************************/
#ifndef __UICLIPBOARD_H_
#define __UICLIPBOARD_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_Clipboard_Selection,  // Linux only.  Always paste when the selection changes
    e_Clipboard_Clipboard,  // All.  Copy on CTRL-C
    e_ClipboardMAX
} e_ClipboardType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void UI_SetClipboardText(std::string Text,e_ClipboardType Clip);
void UI_SetClipboardTextCStr(const char *Text,e_ClipboardType Clip);
void UI_GetClipboardText(std::string &Text,e_ClipboardType Clip);

#endif
