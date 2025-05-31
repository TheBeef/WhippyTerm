/*******************************************************************************
 * FILENAME: UINewConnectionFromURI.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the new connection from URI dialog interface in it.
 *
 * COPYRIGHT:
 *    Copyright 10 Feb 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (10 Feb 2025)
 *       Created
 *
 *******************************************************************************/
#ifndef __UINEWCONNECTIONFROMURI_H_
#define __UINEWCONNECTIONFROMURI_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <stdbool.h>
#include <stdint.h>
#include "UI/UIControl.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_PrivURIHelp_Style_Heading,
    e_PrivURIHelp_Style_Def,
    e_PrivURIHelp_Style_TextLine,
    e_PrivURIHelp_StyleMAX
} e_PrivURIHelp_StyleType;

enum e_UINCFU_ButtonInput
{
    e_UINCFU_ButtonInput_Ok,
    e_UINCFU_ButtonInputMAX
};

enum e_UINCFU_TxtInput
{
    e_UINCFU_TxtInput_URI,
    e_UINCFU_TxtInputMAX
};

typedef enum
{
    e_UINCFU_ListView_Driver,
    e_UINCFU_ListViewMAX
} e_UINCFU_ListViewType;

typedef enum
{
    e_UINCFU_HTMLView_Info,
    e_UINCFU_HTMLViewMAX
} e_UINCFU_HTMLViewType;

typedef enum
{
    e_DNCFUEvent_DriverChanged,
    e_DNCFUEvent_URIChanged,
    e_DNCFUEventMAX
} e_DNCFUEventType;

//struct DNCFUEventDataBttn
//{
//    e_UINCFU_Button BttnID;
//};
//
union DNCFUEventData
{
//    struct DNCFUEventDataBttn Bttn;
    int Junk;
};

struct DNCFUEvent
{
    e_DNCFUEventType EventType;
    uintptr_t ID;
    union DNCFUEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_NewConnectionFromURI(void);
bool UIShow_NewConnectionFromURI(void);
void UIFree_NewConnectionFromURI(void);

t_UIButtonCtrl *UINC_GetButtonInputHandle(e_UINCFU_ButtonInput UIObj);
t_UITextInputCtrl *UINC_GetTxtInputHandle(e_UINCFU_TxtInput UIObj);
t_UIListViewCtrl *UINC_GetListViewInputHandle(e_UINCFU_ListViewType UIObj);
t_UIHTMLViewCtrl *UINC_GetHTMLViewInputHandle(e_UINCFU_HTMLViewType UIObj);

bool DNCFU_Event(const struct DNCFUEvent *Event);

/*******************************************************************************
 *    The URI Help is different in that the UI knows what this control is used
 *    for.  This is to simplifly how this works.  It will be up to the UI
 *    to know how these parts will be rendered.
 *
 *    DON'T DO THIS!  This is a special case that actually makes the UI simpler
 *    because otherwize a whole styled text system would need to be made (which
 *    would be big and complex, so this actually makes it simpler).
 ******************************************************************************/
void UINC_PrivURIHelp_ClearCtrlText(void);
void UINC_PrivURIHelp_AppendText(const char *NewLine,e_PrivURIHelp_StyleType Style);

#endif   /* end of "#ifndef __UINEWCONNECTIONFROMURI_H_" */
