/*******************************************************************************
 * FILENAME: TextStyleHelpers.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has helper functions for handling 
 *
 * COPYRIGHT:
 *    Copyright 2023 Paul Hutchinson.
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
 *    Paul Hutchinson (26 Aug 2023)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "TextStyleHelpers.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    CmpCharStyle
 *
 * SYNOPSIS:
 *    bool CmpCharStyle(const struct CharStyling *A,
 *              const struct CharStyling *B);
 *
 * PARAMETERS:
 *    A [I] -- The first style to compare
 *    B [I] -- The second style to compare
 *
 * FUNCTION:
 *    This function compares 2 char styles to see if they are the same or
 *    different.
 *
 * RETURNS:
 *    true -- They are the same
 *    false -- They are different
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool CmpCharStyle(const struct CharStyling *A,const struct CharStyling *B)
{
    if(A->FGColor==B->FGColor &&
            A->BGColor==B->BGColor &&
            A->ULineColor==B->ULineColor &&
            A->Attribs==B->Attribs)
    {
        return true;
    }
    return false;
}

