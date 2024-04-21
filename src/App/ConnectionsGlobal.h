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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
struct ProcessorConData *Con_GetCurrentProcessorData(void);
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
        uintptr_t Arg3=0,uintptr_t Arg4=0);
void Con_GetCursorXY(int32_t *RetCursorX,int32_t *RetCursorY);
void Con_InsertString(uint8_t *Str,uint32_t Len);
void Con_GetScreenSize(int32_t *RetRows,int32_t *RetColumns);
uint32_t Con_GetSysColor(e_SysColShadeType SysColShade,e_SysColType SysColor);
uint32_t Con_GetSysDefaultColor(e_DefaultColorsType DefaultColor);

#endif
