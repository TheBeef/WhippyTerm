/*******************************************************************************
 * FILENAME: StandardTypes.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has standard base types used by WhippyTerm
 *
 * COPYRIGHT:
 *    Copyright 03 Apr 2026 Paul Hutchinson.
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
 *    Paul Hutchinson (03 Apr 2026)
 *       Created
 *
 *******************************************************************************/
#ifndef __STANDARDTYPES_H_
#define __STANDARDTYPES_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <list>
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef std::list<std::string> t_StringList;
typedef t_StringList::iterator i_StringList;

typedef enum
{
    e_DirectSendPanel_LineEnd_CRLF,
    e_DirectSendPanel_LineEnd_CR,
    e_DirectSendPanel_LineEnd_LF,
    e_DirectSendPanel_LineEnd_TAB,
    e_DirectSendPanel_LineEnd_ESC,
    e_DirectSendPanel_LineEnd_NULL,
    e_DirectSendPanel_LineEnd_None,
    e_DirectSendPanel_LineEndMAX
} e_DirectSendPanel_LineEndType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif   /* end of "#ifndef __STANDARDTYPES_H_" */
