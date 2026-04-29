/*******************************************************************************
 * FILENAME: UINewVersionCheck.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the UI API to the new version check dialog.
 *
 * COPYRIGHT:
 *    Copyright 21 Apr 2026 Paul Hutchinson.
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
 *    Paul Hutchinson (21 Apr 2026)
 *       Created
 *
 *******************************************************************************/
#ifndef __UINEWVERSIONCHECK_H_
#define __UINEWVERSIONCHECK_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIControl.h"

/***  DEFINES                          ***/
typedef enum
{
    e_VersionCheckStep_Connect,
    e_VersionCheckStep_Reading,
    e_VersionCheckStep_Closing,
    e_VersionCheckStep_Check,
    e_VersionCheckStepMAX
} e_VersionCheckStepType;

typedef enum
{
    e_VersionCheckMode_Clear,
    e_VersionCheckMode_Doing,
    e_VersionCheckMode_Done,
    e_VersionCheckModeMAX
} e_VersionCheckModeType;

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_NVC_Button_GotoWebPage,
    e_NVC_ButtonMAX
} e_NVC_Button;

typedef enum
{
    e_NVC_Label_Result,
    e_NVC_LabelMAX
} e_NVC_Label;

typedef enum
{
    e_NVCEvent_BttnTriggered,
    e_NVCEvent_Timer,
    e_NVCEventMAX
} e_NVCEventType;

struct NVCEventDataBttn
{
    e_NVC_Button BttnID;
};

union NVCEventData
{
    struct NVCEventDataBttn Bttn;
};

struct NVCEvent
{
    e_NVCEventType EventType;
    union NVCEventData Info;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UIAlloc_NewVersionCheck(void);
bool UIShow_NewVersionCheck(void);
void UIFree_NewVersionCheck(void);

void NVC_SetStepMode(e_VersionCheckStepType Step,e_VersionCheckModeType Mode);
void NVC_ShowHideNewVersionFound(bool Show);
void NVC_EnableOkBttn(bool Enable);

t_UIButtonCtrl *UINVC_GetButtonHandle(e_NVC_Button UIObj);
t_UILabelCtrl *UINVC_GetLabelHandle(e_NVC_Label UIObj);

bool NVC_Event(const struct NVCEvent *Event);

#endif   /* end of "#ifndef __UINEWVERSIONCHECK_H_" */
