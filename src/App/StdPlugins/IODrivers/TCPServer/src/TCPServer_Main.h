/*******************************************************************************
 * FILENAME: TCPServer_Main.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    .h file
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (01 Jul 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __TCPSERVER_MAIN_H_
#define __TCPSERVER_MAIN_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/
extern const struct IOS_API *g_TCPS_IOSystem;
extern const struct PI_UIAPI *g_TCPS_UI;
extern const struct PI_SystemAPI *g_TCPS_System;

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void TCPServer_RegisterPlugin(const struct PI_SystemAPI *SysAPI);

#endif
