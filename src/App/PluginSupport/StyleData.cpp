/*******************************************************************************
 * FILENAME: StyleData.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has support functions for style data.
 *
 * COPYRIGHT:
 *    Copyright 02 Feb 2026 Paul Hutchinson.
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
 * CREATED BY:
 *    Paul Hutchinson (02 Feb 2026)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Settings.h"
#include "App/PluginSupport/StyleData.h"
#include "PluginSDK/PluginTypes.h"
#include <string.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    DefaultStyleDataStructure
 *
 * SYNOPSIS:
 *    void DefaultStyleDataStructure(struct StyleData *Struct2Default);
 *
 * PARAMETERS:
 *    Struct2Default [O] -- The structure to default.
 *
 * FUNCTION:
 *    This function defaults the StyleData structure.  This is needed so that
 *    plugins that use an older version of the structure will get sane defaults
 *    in the fields they do not know about.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DefaultStyleDataStructure(struct StyleData *Struct2Default)
{
    memset(Struct2Default,0x00,sizeof(struct StyleData));

    Struct2Default->FGColor=g_Settings.DefaultConSettings.
            DefaultColors[e_DefaultColors_FG];
    Struct2Default->BGColor=g_Settings.DefaultConSettings.
            DefaultColors[e_DefaultColors_BG];
    Struct2Default->Attribs=0;
    Struct2Default->ULineColor=Struct2Default->FGColor;
}
