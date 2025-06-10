/*******************************************************************************
 * FILENAME: DisplayColors.h
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2018 Paul Hutchinson.
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
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __DISPLAYCOLORS_H_
#define __DISPLAYCOLORS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/DataProcessors.h"
#include "PluginSDK/Plugin.h"
#include <stdint.h>

/***  DEFINES                          ***/
#define SELECTION_BG_COLOR_DEFAULT          0x5d81a9
#define SELECTION_FG_COLOR_DEFAULT          0xFFFFFF

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_SysColPreset_WhippyTerm,
    e_SysColPreset_VGA,
    e_SysColPreset_CMD,
    e_SysColPreset_TerminalApp,
    e_SysColPreset_PuTTY,
    e_SysColPreset_mIRC,
    e_SysColPreset_xterm,
    e_SysColPreset_Ubuntu,
    e_SysColPresetMAX
} e_SysColPresetType;

typedef enum
{
    e_Color_BG,
    e_Color_FG,
    e_ColorMAX
} e_ColorType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void GetPresetSysColors(e_SysColPresetType Preset,uint32_t Colors[e_SysColShadeMAX][e_SysColMAX]);
void GetPresetSelectionColors(uint32_t Colors[e_ColorMAX]);

#endif
