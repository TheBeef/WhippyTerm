/*******************************************************************************
 * FILENAME: Dialog_SendBufferSelect.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 07 Sep 2024 Paul Hutchinson.
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
 *    Paul Hutchinson (07 Sep 2024)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_EditSendBuffer.h"
#include "App/SendBuffer.h"
#include "Dialog_SendBufferSelect.h"
#include "UI/UISendBufferSelect.h"
#include "UI/UIAsk.h"
#include "UI/UIControl.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static void SBSD_FillInBufferList(void);

/*** VARIABLE DEFINITIONS     ***/
static e_SBSDType m_SBSD_DialogType;
static class TheMainWindow *m_SBSD_MW;
static int m_SBSD_LastSelectedBufferIndex=-1;

/*******************************************************************************
 * NAME:
 *    RunSendBufferSelectDialog
 *
 * SYNOPSIS:
 *    void RunSendBufferSelectDialog(class TheMainWindow *MW,
 *          e_SBSDType DialogType);
 *
 * PARAMETERS:
 *    MW [I] -- The main window that this dialog is going to work with (where
 *              are we going to send the buffer).
 *    DialogType [I] -- What type of send buffer select to do.  Supported types:
 *                          e_SBSD_Send -- User and press "send" to send the
 *                                         selected buffer.
 *                          e_SBSD_Edit -- User and press "edit" to open the
 *                                         edit buffer of the selected entry.
 *
 * FUNCTION:
 *    This function shows the com test dialog.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RunSendBufferSelectDialog(class TheMainWindow *MW,e_SBSDType DialogType)
{
    bool AllocatedUI;
    t_UIButtonCtrl *GoBttn;

    AllocatedUI=false;
    try
    {
        m_SBSD_DialogType=DialogType;
        m_SBSD_MW=MW;

        if(!UIAlloc_SendBufferSelect())
            throw("Failed to open window");

        GoBttn=UISBS_GetButton(e_SBS_Button_GoButton);
        if(m_SBSD_LastSelectedBufferIndex<0)
            UIEnableButton(GoBttn,false);
        else
            UIEnableButton(GoBttn,true);

        switch(m_SBSD_DialogType)
        {
            case e_SBSD_Send:
                UISBS_SetDialogTitle("Send Buffer");
                UISetButtonLabel(GoBttn,"Send");
            break;
            case e_SBSD_Edit:
                UISBS_SetDialogTitle("Edit Send Buffer");
                UISetButtonLabel(GoBttn,"Edit");
            break;
            case e_SBSDMAX:
            default:
                throw("Internal error: unknown dialog type");
            break;
        }

        SBSD_FillInBufferList();

        if(UIShow_SendBufferSelect())
        {
        }
    }
    catch(const char *Error)
    {
        UIAsk("Error opening select send buffer dialog",Error,e_AskBox_Error,
                e_AskBttns_Ok);
    }
    catch(...)
    {
        UIAsk("Error opening select send buffer dialog",
                "Failed to open select send buffer dialog",
                e_AskBox_Error,e_AskBttns_Ok);
    }

    if(AllocatedUI)
        UIFree_SendBufferSelect();
}

/*******************************************************************************
 * NAME:
 *    SBS_Event
 *
 * SYNOPSIS:
 *    bool SBS_Event(const struct SBSEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event that got us here.
 *
 * FUNCTION:
 *    This function is called from the UI to tell us that something happend.
 *
 * RETURNS:
 *    true -- The event should act normally
 *    false -- There event should be canceled
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool SBS_Event(const struct SBSEvent *Event)
{
    t_UIButtonCtrl *GoBttn;
    t_UIColumnView *BufferList;
    int BufferIndex;

    BufferList=UISBS_GetColumnViewHandle(e_SBSColumnView_Buffers_List);
    GoBttn=UISBS_GetButton(e_SBS_Button_GoButton);

    switch(Event->EventType)
    {
        case e_SBSEvent_BttnTriggered:
            switch(Event->Info.Bttn.BttnID)
            {
                case e_SBS_Button_GoButton:
                    BufferIndex=UIGetColumnViewSelectedEntry(BufferList);
                    if(BufferIndex<0)
                        return true;

                    switch(m_SBSD_DialogType)
                    {
                        case e_SBSD_Send:
                            if(!g_SendBuffers.Send(m_SBSD_MW->ActiveCon,
                                    BufferIndex))
                            {
                                UIAsk("Error","Failed to send the buffer.",
                                        e_AskBox_Error,e_AskBttns_Ok);
                                return true;
                            }
                            m_SBSD_LastSelectedBufferIndex=BufferIndex;
                            UISBS_CloseDialog();
                        break;
                        case e_SBSD_Edit:
                            if(RunEditSendBufferDialog(BufferIndex,NULL,0))
                            {
                                /* Update the UI with any changes */
                                m_SBSD_MW->RethinkActiveConnectionUI();

                                m_SBSD_LastSelectedBufferIndex=BufferIndex;
                                UISBS_CloseDialog();
                            }
                        break;
                        case e_SBSDMAX:
                        default:
                        break;
                    }
                break;
                case e_SBS_Button_Cancel:
                    UISBS_CloseDialog();
                break;
                case e_SBS_ButtonMAX:
                default:
                break;
            }
        break;
        case e_SBSEvent_ColumnViewSelectionChanged:
            switch(Event->Info.ColumnView.InputID)
            {
                case e_SBSColumnView_Buffers_List:
                    if(Event->Info.ColumnView.NewRow>=0)
                        UIEnableButton(GoBttn,true);
                    else
                        UIEnableButton(GoBttn,false);
                break;
                case e_SBSColumnViewMAX:
                default:
                break;
            }
        break;
        case e_SBSEventMAX:
        default:
        break;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    SBSD_FillInBufferList
 *
 * SYNOPSIS:
 *    void SBSD_FillInBufferList(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function fills in the list of buffers with the buffers names.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void SBSD_FillInBufferList(void)
{
    int b;
    const char *Name;
    t_UIColumnView *BufferList;

    BufferList=UISBS_GetColumnViewHandle(e_SBSColumnView_Buffers_List);

    UIColumnViewClear(BufferList);
    for(b=0;b<MAX_SEND_BUFFERS;b++)
    {
        UIColumnViewAddRow(BufferList);
        Name=g_SendBuffers.GetBufferName(b);
        if(Name==NULL)
            Name="ERROR";
        UIColumnViewSetColumnText(BufferList,0,b,Name);
    }

    if(m_SBSD_LastSelectedBufferIndex>=0)
        UIColumnViewSelectRow(BufferList,m_SBSD_LastSelectedBufferIndex);
}

