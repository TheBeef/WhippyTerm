/*******************************************************************************
 * FILENAME: TCPClient_Main.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (20 Jun 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __TCPCLIENT_MAIN_H_
#define __TCPCLIENT_MAIN_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/Plugin.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/
extern const struct IOS_API *g_TCPC_IOSystem;
extern const struct PI_UIAPI *g_TCPC_UI;
extern const struct PI_SystemAPI *g_TCPC_System;

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void TCPClient_RegisterPlugin(const struct PI_SystemAPI *SysAPI);

#endif
