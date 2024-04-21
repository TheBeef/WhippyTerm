/*******************************************************************************
 * FILENAME: Comport_Main.h
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
#ifndef __COMPORT_MAIN_H_
#define __COMPORT_MAIN_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "OS/Comport_Serial.h"
#include <stdbool.h>

/***  DEFINES                          ***/
#define COMPORT_URI_PREFIX              "COM"

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
enum e_ComportDataBitsType
{
    e_ComportDataBits_7,
    e_ComportDataBits_8,
    e_ComportDataBitsMAX
};

enum e_ComportParityType
{
    e_ComportParity_none,
    e_ComportParity_odd,
    e_ComportParity_even,
    e_ComportParity_mark,
    e_ComportParity_space,
    e_ComportParityMAX
};

enum e_ComportStopBitsType
{
    e_ComportStopBits_1,
    e_ComportStopBits_2,
    e_ComportStopBitsMAX
};

enum e_ComportFlowControlType
{
    e_ComportFlowControl_None,
    e_ComportFlowControl_XONXOFF,
    e_ComportFlowControl_Hardware,
    e_ComportFlowControlMAX
};

struct ComportPortOptions
{
    uint32_t BitRate;
    e_ComportDataBitsType DataBits;
    e_ComportParityType Parity;
    e_ComportStopBitsType StopBits;
    e_ComportFlowControlType FlowControl;
};

/* DEBUG PAUL: Die */
struct ComportDetectData
{
    t_OSComportListType DevList;
    i_OSComportListType CurrentDev;
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/
extern const struct PI_UIAPI *g_CP_UI;
extern const struct IOS_API *g_CP_IOSystem;

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void Comport_DefaultPortOptions(struct ComportPortOptions *Options);
void Comport_ConvertFromKVList(struct ComportPortOptions *Options,
        const t_PIKVList *KVList);
void Comport_Convert2KVList(const struct ComportPortOptions *Options,
        t_PIKVList *KVList);
bool Comport_SetOptionsFromURI(const char *OptionsURIStart,t_PIKVList *KVList,
        bool Update);

#endif
