/*******************************************************************************
 * FILENAME: CursorKeyMode.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the cursor key mode system in it.  The cursor key mode
 *    controls if the cursor keys (or other movement keys like page up) function
 *    locally or send the key presses out the connection.
 *
 * COPYRIGHT:
 *    Copyright 13 Oct 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (13 Oct 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/CursorKeyMode.h"
#include "App/MainWindow.h"
#include "App/Settings.h"
#include "UI/UIKeyboard.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
bool m_CursorKeyModeLocal=false;
e_CursorKeyToggleModeType m_CursorKeyModeLast=e_CursorKeyToggleModeMAX;

/*******************************************************************************
 * NAME:
 *    SetCursorKeyModeLocal
 *
 * SYNOPSIS:
 *    void SetCursorKeyModeLocal(bool Local);
 *
 * PARAMETERS:
 *    Local [I] -- If true then switch to local control mode
 *
 * FUNCTION:
 *    This function changes the current mode that the cursor keys work in.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    GetCurrentCursorKeyModeIsLocal()
 ******************************************************************************/
void SetCursorKeyModeLocal(bool Local)
{
    switch(g_Settings.CursorKeyToggleMode)
    {
        case e_CursorKeyToggleMode_None:
        case e_CursorKeyToggleMode_Esc:
        case e_CursorKeyToggleMode_Insert:
        case e_CursorKeyToggleModeMAX:
        default:
            m_CursorKeyModeLocal=Local;
        break;
        case e_CursorKeyToggleMode_ScrollLock:
            /* Scroll lock is controled by the LED so we can't toggle */
        break;
    }

    MW_InformOfCursorKeyModeChange();
}

/*******************************************************************************
 * NAME:
 *    GetCurrentCursorKeyModeIsLocal
 *
 * SYNOPSIS:
 *    bool GetCurrentCursorKeyModeIsLocal(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function returns the current cursor key mode.
 *
 * RETURNS:
 *    true -- We are in local mode
 *    false -- We are in send mode
 *
 * SEE ALSO:
 *    SetCursorKeyModeLocal()
 ******************************************************************************/
bool GetCurrentCursorKeyModeIsLocal(void)
{
    switch(g_Settings.CursorKeyToggleMode)
    {
        case e_CursorKeyToggleMode_ScrollLock:
            /* Scroll lock is controled by the LED */
            return IsScrollLockOn();
        break;
        case e_CursorKeyToggleMode_None:
        case e_CursorKeyToggleMode_Esc:
        case e_CursorKeyToggleMode_Insert:
        case e_CursorKeyToggleModeMAX:
        default:
        break;
    }
    return m_CursorKeyModeLocal;
}

/*******************************************************************************
 * NAME:
 *    CursorKeyMode_ApplySettings
 *
 * SYNOPSIS:
 *    void CursorKeyMode_ApplySettings(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when new settings are applied.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void CursorKeyMode_ApplySettings(void)
{
    bool UsingScrollLock;

    UsingScrollLock=false;
    switch(g_Settings.CursorKeyToggleMode)
    {
        case e_CursorKeyToggleMode_None:
            if(g_Settings.CursorKeyToggleMode!=m_CursorKeyModeLast)
            {
                /* If the user changed the toggle mode, then we turn it off */
                m_CursorKeyModeLocal=false;
            }
        break;
        case e_CursorKeyToggleMode_ScrollLock:
            /* The scroll lock has be handled special because on Linux it's a
               mess, so we need to tell the UI if we are going to be using it */
            UsingScrollLock=true;
            m_CursorKeyModeLocal=IsScrollLockOn();
        break;
        case e_CursorKeyToggleMode_Esc:
        case e_CursorKeyToggleMode_Insert:
        case e_CursorKeyToggleModeMAX:
        default:
        break;
    }

    EnableHandlingOfScrollLock(UsingScrollLock);

    m_CursorKeyModeLast=g_Settings.CursorKeyToggleMode;
}
