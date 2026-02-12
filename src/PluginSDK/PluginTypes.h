/*******************************************************************************
 * FILENAME: PluginTypes.h
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
 *    Paul Hutchinson (10 Jul 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __PLUGINTYPES_H_
#define __PLUGINTYPES_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <stdint.h>

/***  DEFINES                          ***/
#define TXT_ATTRIB_UNDERLINE                0x0001
#define TXT_ATTRIB_UNDERLINE_DOUBLE         0x0002
#define TXT_ATTRIB_UNDERLINE_DOTTED         0x0004
#define TXT_ATTRIB_UNDERLINE_DASHED         0x0008
#define TXT_ATTRIB_UNDERLINE_WAVY           0x0010
#define TXT_ATTRIB_OVERLINE                 0x0020
#define TXT_ATTRIB_LINETHROUGH              0x0040
#define TXT_ATTRIB_BOLD                     0x0080
#define TXT_ATTRIB_ITALIC                   0x0100
#define TXT_ATTRIB_OUTLINE                  0x0200
#define TXT_ATTRIB_BOX                      0x0400  // Future
#define TXT_ATTRIB_ROUNDBOX                 0x0800
#define TXT_ATTRIB_REVERSE                  0x1000
#define TXT_ATTRIB_RESERVED                 0x8000  // Do not use

/* Goes back to version 1.0 (just here for compatibility) */
#define TXT_ATTRIB_LINETHROUGHT             TXT_ATTRIB_LINETHROUGH

/* When working with struct StyleData you need to send in a version so you know what fields are valid */
#define STYLEDATA_VERSION_1     1       /* Uses FGColor, BGColor, Attribs, ULineColor */

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef uint8_t PG_BOOL;
typedef struct PIKVList {int x;} t_PIKVList;

typedef enum
{
    e_SysCol_Black=0,
    e_SysCol_Red,
    e_SysCol_Green,
    e_SysCol_Yellow,
    e_SysCol_Blue,
    e_SysCol_Magenta,
    e_SysCol_Cyan,
    e_SysCol_White,
    e_SysColMAX
} e_SysColType;

typedef enum
{
    e_SysColShade_Normal,
    e_SysColShade_Bright,
    e_SysColShade_Dark,
    e_SysColShadeMAX,
} e_SysColShadeType;

typedef enum
{
    e_DefaultColors_BG,
    e_DefaultColors_FG,
    e_DefaultColorsMAX
} e_DefaultColorsType;

struct StyleData
{
    uint32_t FGColor;
    uint32_t BGColor;
    uint32_t Attribs;
    uint32_t ULineColor;
    uint8_t Reserved[512];
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
