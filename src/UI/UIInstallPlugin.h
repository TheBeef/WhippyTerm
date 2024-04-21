/*******************************************************************************
 * FILENAME: UIInstallPlugin.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This has the UI definitions for the install plugin dialog.
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
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
 *    Paul Hutchinson (06 Mar 2022)
 *       Created
 *
 *******************************************************************************/
#ifndef __UIINSTALLPLUGIN_H_
#define __UIINSTALLPLUGIN_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
enum e_UIIP_TextInput
{
    e_UIIP_TextInput_Name,
    e_UIIP_TextInput_Copyright,
    e_UIIP_TextInput_ReleaseDate,
    e_UIIP_TextInput_Contrib,
    e_UIIP_TextInput_PluginType,
    e_UIIP_TextInput_Version,
    e_UIIP_TextInputMAX
};

enum e_UIIP_MultiLineTextInput
{
    e_UIIP_MultiLineTextInput_Description,
    e_UIIP_MultiLineTextInputMAX
};

/* Events */
typedef enum
{
//    e_DIPEvent_BttnTriggered,
//    e_DIPEvent_ListViewChange,
    e_DIPEvent_DialogOk,
    e_DIPEventMAX
} e_DIPEventType;

//struct DIPEventDataBttn
//{
//    enum e_UIIP_Button InputID;
//};
//
//struct DIPEventDataListView
//{
//    enum e_UIIP_ListView InputID;
//};

//union DIPEventData
//{
//    struct DIPEventDataBttn Bttn;
//    struct DIPEventDataListView ListView;
//};

struct DIPEvent
{
    e_DIPEventType EventType;
    uintptr_t ID;
//    union DIPEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_InstallPlugin(void);
void UIFree_InstallPlugin(void);
bool UIShow_InstallPlugin(void);

t_UITextInputCtrl *UIIP_GetTextInputHandle(e_UIIP_TextInput UIObj);
t_UIMuliLineTextInputCtrl *UIIP_GetMultiLineTextInputHandle(e_UIIP_MultiLineTextInput UIObj);

bool DIP_Event(const struct DIPEvent *Event);

#endif
