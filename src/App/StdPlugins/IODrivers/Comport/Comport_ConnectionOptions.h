/*******************************************************************************
 * FILENAME: Comport_ConnectionOptions.h
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
#ifndef __COMPORT_CONNECTIONOPTIONS_H_
#define __COMPORT_CONNECTIONOPTIONS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/Plugin.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
t_ConnectionOptionsWidgetsType *Comport_ConnectionOptionsWidgets_AllocWidgets(
        t_WidgetSysHandle *WidgetHandle);
void Comport_ConnectionOptionsWidgets_FreeWidgets(t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle);
void Comport_ConnectionOptionsWidgets_StoreUI(t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
        t_PIKVList *Options);
void Comport_ConnectionOptionsWidgets_UpdateUI(
        t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
        t_PIKVList *Options);

#endif
