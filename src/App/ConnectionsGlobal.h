/*******************************************************************************
 * FILENAME: ConnectionsGlobal.h
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
#ifndef __CONNECTIONSGLOBAL_H_
#define __CONNECTIONSGLOBAL_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/DataProcessorsSystem.h"
#include "App/Connections.h"
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void Con_SetActiveConnection(class Connection *ActiveCon);
void Con_WriteData(const uint8_t *Data,int Bytes);
void Con_InformOfConnected(uintptr_t ID);
void Con_InformOfDisconnected(uintptr_t ID);
bool Con_InformOfDataAvaiable(uintptr_t ID);
void Con_WriteChar2Display(uint8_t *Chr);
void Con_SetFGColor(uint32_t FGColor);
uint32_t Con_GetFGColor(void);
void Con_SetBGColor(uint32_t BGColor);
uint32_t Con_GetBGColor(void);
void Con_SetULineColor(uint32_t ULineColor);
uint32_t Con_GetULineColor(void);
void Con_SetAttribs(uint16_t Attribs);
uint16_t Con_GetAttribs(void);
void Con_DoFunction(e_ConFuncType Fn,uintptr_t Arg1=0,uintptr_t Arg2=0,
        uintptr_t Arg3=0,uintptr_t Arg4=0,uintptr_t Arg5=0,uintptr_t Arg6=0);
void Con_GetCursorXY(int32_t *RetCursorX,int32_t *RetCursorY);
void Con_InsertString(const uint8_t *Str,uint32_t Len);
void Con_GetScreenSize(int32_t *RetRows,int32_t *RetColumns);
uint32_t Con_GetSysColor(e_SysColShadeType SysColShade,e_SysColType SysColor);
uint32_t Con_GetSysDefaultColor(e_DefaultColorsType DefaultColor);
void Con_DoBell(bool VisualOnly);
void Con_SetTitle(const char *Title);
t_DataProMark *Con_AllocateMark(void);
void Con_FreeMark(t_DataProMark *Mark);
bool Con_IsMarkValid(t_DataProMark *Mark);
void Con_SetMark2CursorPos(t_DataProMark *Mark);
void Con_ApplyAttrib2Mark(t_DataProMark *Mark,uint32_t Attrib,uint32_t Offset,uint32_t Len);
void Con_RemoveAttribFromMark(t_DataProMark *Mark,uint32_t Attrib,uint32_t Offset,uint32_t Len);
void Con_ApplyFGColor2Mark(t_DataProMark *Mark,uint32_t FGColor,uint32_t Offset,uint32_t Len);
void Con_ApplyBGColor2Mark(t_DataProMark *Mark,uint32_t BGColor,uint32_t Offset,uint32_t Len);
void Con_MoveMark(t_DataProMark *Mark,int Amount);
const uint8_t *Con_GetMarkString(t_DataProMark *Mark,uint32_t *Size,uint32_t Offset,uint32_t Len);
void Con_FreezeStream(void);
void Con_ReleaseFrozenStream(void);
void Con_ClearFrozenStream(void);
const uint8_t *Con_GetFrozenString(uint32_t *Size);

#endif
