/*******************************************************************************
 * FILENAME: QTKeyMappings.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2020 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (12 Jul 2020)
 *       Created
 *
 *******************************************************************************/
#ifndef __QTKEYMAPPINGS_H_
#define __QTKEYMAPPINGS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/KeyDefines.h"
#include <Qt>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
Qt::Key ConvertUIKey2QTKey(e_UIKeys UIKey);
e_UIKeys ConvertQTKey2UIKey(Qt::Key QKey);

#endif
