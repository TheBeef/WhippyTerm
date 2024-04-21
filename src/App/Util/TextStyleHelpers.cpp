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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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

