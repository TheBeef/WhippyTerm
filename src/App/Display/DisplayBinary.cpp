/*******************************************************************************
 * FILENAME: DisplayBinary.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the display for binary in it.  This handles the back buffer,
 *    scroll bars, word wrap, etc in it.
 *
 * COPYRIGHT:
 *    Copyright 2023 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (06 Aug 2023)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "DisplayBinary.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

DisplayBinary::DisplayBinary()
{
}

DisplayBinary::~DisplayBinary()
{
}

bool DisplayBinary::Init(void *ParentWidget,bool (*EventCallback)(const struct DBEvent *Event),uintptr_t UserData)
{
    if(!InitBase(EventCallback,UserData))
        return false;

    return true;
}

void DisplayBinary::Reparent(void *NewParentWidget)
{
    
}

/*******************************************************************************
 * NAME:
 *    DisplayBinary::WriteChar
 *
 * SYNOPSIS:
 *    void DisplayBinary::WriteChar(uint8_t *Chr);
 *
 * PARAMETERS:
 *    Chr [I] -- The char to add.  This is a UTF8 char, and is 0 term'ed.
 *
 * FUNCTION:
 *    This function adds a char to the display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DisplayBinary::WriteChar(uint8_t *Chr)
{
}
