/*******************************************************************************
 * FILENAME: QTSound.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 08 Feb 2025 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (08 Feb 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "UI/UISound.h"
#include "QTSound.h"
#include <QApplication>
#include <QSound>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

void UI_Beep(void)
{
    QApplication::beep();
}

void UIPlayWav(const char *Filename)
{
    QSound::play(Filename);
}

void UIPlayBuiltInBeep(void)
{
    QSound::play(":/S/Sounds/Computer_Error_Alert_Short.wav");
}

