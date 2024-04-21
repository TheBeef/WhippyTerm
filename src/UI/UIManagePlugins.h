/*******************************************************************************
 * FILENAME: UIManagePlugins.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This has the UI definitions for the manage plugins dialog.
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
 *    Paul Hutchinson (04 Mar 2022)
 *       Created
 *
 *******************************************************************************/
#ifndef __UIMANAGEPLUGINS_H_
#define __UIMANAGEPLUGINS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
enum e_UIMP_ListView
{
    e_UIMP_ListView_PluginList=0,
    e_UIMP_ListViewMAX
};

enum e_UIMP_Button
{
    e_UIMP_Button_Enable,
    e_UIMP_Button_Install,
    e_UIMP_Button_Uninstall,
    e_UIMP_ButtonMAX
};

enum e_UIMP_TextInput
{
    e_UIMP_TextInput_Name,
    e_UIMP_TextInput_Copyright,
    e_UIMP_TextInput_ReleaseDate,
    e_UIMP_TextInput_Contrib,
    e_UIMP_TextInput_PluginType,
    e_UIMP_TextInput_Version,
    e_UIMP_TextInputMAX
};

enum e_UIMP_MultiLineTextInput
{
    e_UIMP_MultiLineTextInput_Description,
    e_UIMP_MultiLineTextInputMAX
};

enum e_UIMP_LabelInput
{
    e_UIMP_LabelInput_MustRestart,
    e_UIMP_LabelInputMAX
};

/* Events */
typedef enum
{
    e_DMPEvent_BttnTriggered,
    e_DMPEvent_ListViewChange,
    e_DMPEvent_DialogOk,
    e_DMPEventMAX
} e_DMPEventType;

struct DMPEventDataBttn
{
    enum e_UIMP_Button InputID;
};

struct DMPEventDataListView
{
    enum e_UIMP_ListView InputID;
};

union DMPEventData
{
    struct DMPEventDataBttn Bttn;
    struct DMPEventDataListView ListView;
};

struct DMPEvent
{
    e_DMPEventType EventType;
    uintptr_t ID;
    union DMPEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_ManagePlugins(void);
void UIFree_ManagePlugins(void);
bool UIShow_ManagePlugins(void);

t_UIListViewCtrl *UIMP_GetListViewHandle(e_UIMP_ListView UIObj);
t_UIButtonCtrl *UIMP_GetButtonHandle(e_UIMP_Button UIObj);
t_UITextInputCtrl *UIMP_GetTextInputHandle(e_UIMP_TextInput UIObj);
t_UIMuliLineTextInputCtrl *UIMP_GetMultiLineTextInputHandle(e_UIMP_MultiLineTextInput UIObj);
t_UILabelCtrl *UIMP_GetLabelInputHandle(e_UIMP_LabelInput UIObj);

bool DMP_Event(const struct DMPEvent *Event);

#endif
