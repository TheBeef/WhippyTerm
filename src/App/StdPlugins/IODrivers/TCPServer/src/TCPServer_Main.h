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
