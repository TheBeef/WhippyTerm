/*******************************************************************************
 * FILENAME: HTTPClient_Main.h
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
 *    Paul Hutchinson (10 Jul 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __HTTPCLIENT_MAIN_H_
#define __HTTPCLIENT_MAIN_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/Plugin.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/
extern const struct IOS_API *g_HC_IOSystem;
extern const struct PI_UIAPI *g_HC_UI;
extern const struct PI_SystemAPI *g_HC_System;

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void HTTPClient_RegisterPlugin(const struct PI_SystemAPI *SysAPI);

#endif
