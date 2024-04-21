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
 *    This software is the property of Paul Hutchinson. and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void GetPresetSysColors(e_SysColPresetType Preset,uint32_t Colors[e_SysColShadeMAX][e_SysColMAX]);

#endif
