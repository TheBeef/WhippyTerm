/*******************************************************************************
 * FILENAME: UISettingsHexDumpAppearance.h
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the UI access for the hex dump appearance settings form
 *    in it.
 *
 * COPYRIGHT:
 *    Copyright 2026 Paul Hutchinson.
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
 *    Paul Hutchinson (04 Jul 2026)
 *       Created
 *
 ******************************************************************************/
#ifndef __UISETTINGSHEXDUMPAPPEARANCE_H_
#define __UISETTINGSHEXDUMPAPPEARANCE_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
enum e_UISHDA_Button
{
    e_UISHDA_Button_SelectDivLineColor,
    e_UISHDA_ButtonMAX
};

enum e_UISHDA_NumberInput
{
    e_UISHDA_NumberInput_DivLineWidth,
    e_UISHDA_NumberInput_BytesPerLine,
    e_UISHDA_NumberInput_DividerEvery,
    e_UISHDA_NumberInputMAX
};

enum e_UISHDA_ColorPreview
{
    e_UISHDA_ColorPreview_DivLineColor,
    e_UISHDA_ColorPreviewMAX
};

/* Events */
typedef enum
{
    e_SHDAEvent_BttnTriggered,
    e_SHDAEvent_NumberInputChanged,
    e_SHDAEventMAX
} e_SHDAEventType;

struct SHDAEventDataBttn
{
    e_UISHDA_Button BttnID;
};

struct SHDAEventDataNumber
{
    e_UISHDA_NumberInput InputID;
    int NewValue;
};

union SHDAEventData
{
    struct SHDAEventDataBttn Bttn;
    struct SHDAEventDataNumber Number;
};

struct SHDAEvent
{
    e_SHDAEventType EventType;
    uintptr_t ID;
    union SHDAEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_Settings_HexDumpAppearance(void);
bool UIShow_Settings_HexDumpAppearance(void);
void UIFree_Settings_HexDumpAppearance(void);

t_UIButtonCtrl *UISHDA_GetButton(e_UISHDA_Button Bttn);
t_UINumberInput *UISHDA_GetNumberInput(e_UISHDA_NumberInput Input);
t_UIColorPreviewCtrl *UISHDA_GetColorPreview(e_UISHDA_ColorPreview Preview);

bool SHDA_Event(const struct SHDAEvent *Event);

#endif
