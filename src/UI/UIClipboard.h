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
