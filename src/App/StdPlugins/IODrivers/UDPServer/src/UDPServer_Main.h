/*******************************************************************************
 * FILENAME: UDPServer_Main.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    .h file
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (15 May 2022)
 *       Created
 *
 *******************************************************************************/
#ifndef __UDPSERVER_MAIN_H_
#define __UDPSERVER_MAIN_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/
extern const struct IOS_API *g_UDPS_IOSystem;
extern const struct PI_UIAPI *g_UDPS_UI;
extern const struct PI_SystemAPI *g_UDPS_System;

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void UDPServer_RegisterPlugin(const struct PI_SystemAPI *SysAPI);

#endif
