/*******************************************************************************
 * FILENAME: Dialog_SendByte.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the send byte dialog in it.
 *
 * COPYRIGHT:
 *    Copyright 2024 Paul Hutchinson.
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
 *    Paul Hutchinson (02 May 2024)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Dialog_SendByte.h"
#include "UI/UISendByte.h"
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
static uint8_t m_SendValue;

/*******************************************************************************
 * NAME:
 *    RunSendByteDialog
 *
 * SYNOPSIS:
 *    bool RunSendByteDialog(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function shows the send byte dialog.
 *
 * RETURNS:
 *    true -- User selected Ok
 *    false -- User selected Cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool RunSendByteDialog(TheMainWindow *MW)
{
    bool RetValue;

    m_SendValue=0;

    if(!UIAlloc_SendByte())
        return false;

    RetValue=UIShow_SendByte();

    if(RetValue)
        MW->DoSendByte(m_SendValue);

    UIFree_SendByte();

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    SBD_Event
 *
 * SYNOPSIS:
 *    bool SBD_Event(const struct SBDEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event
 *
 * FUNCTION:
 *    This function is called from the UI to tell us that there was a
 *    UI event.
 *
 * RETURNS:
 *    true -- Handle the event noramlly
 *    false -- Cancel the event
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool SBD_Event(const struct SBDEvent *Event)
{
    t_UITextInputCtrl *DecCtrl;
    t_UITextInputCtrl *HexCtrl;
    t_UITextInputCtrl *OctCtrl;
    char buff[100];
    string TextValue;
    int NewValue;

    DecCtrl=UISBD_GetTextInput(e_SBD_TextInput_Dec);
    HexCtrl=UISBD_GetTextInput(e_SBD_TextInput_Hex);
    OctCtrl=UISBD_GetTextInput(e_SBD_TextInput_Oct);

    switch(Event->EventType)
    {
        case e_SBDEvent_AscIISelected:
            m_SendValue=Event->Info.AscII.ByteValue;
        break;
        case e_SBDEvent_TextEditDone:
            switch(Event->Info.Txt.TxtID)
            {
                case e_SBD_TextInput_Dec:
                    UIGetTextCtrlText(DecCtrl,TextValue);
                    NewValue=strtol(TextValue.c_str(),NULL,10);
                    if(NewValue>=0 && NewValue<=255)
                        m_SendValue=NewValue;
                break;
                case e_SBD_TextInput_Hex:
                    UIGetTextCtrlText(HexCtrl,TextValue);
                    NewValue=strtol(TextValue.c_str(),NULL,16);
                    if(NewValue>=0 && NewValue<=255)
                        m_SendValue=NewValue;
                break;
                case e_SBD_TextInput_Oct:
                    UIGetTextCtrlText(OctCtrl,TextValue);
                    NewValue=strtol(TextValue.c_str(),NULL,8);
                    if(NewValue>=0 && NewValue<=255)
                        m_SendValue=NewValue;
                break;
                case e_SBD_TextInputMAX:
                default:
                break;
            }
        break;
        case e_SBDEventMAX:
        default:
        break;
    }

    sprintf(buff,"%d",m_SendValue);
    UISetTextCtrlText(DecCtrl,buff);
    sprintf(buff,"%X",m_SendValue);
    UISetTextCtrlText(HexCtrl,buff);
    sprintf(buff,"%o",m_SendValue);
    UISetTextCtrlText(OctCtrl,buff);

    return true;
}

