/*******************************************************************************
 * FILENAME: Dialog_Bridge.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (27 Aug 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Connections.h"
//#include "App/IOSystem.h"
#include "App/Dialogs/Dialog_Bridge.h"
#include "UI/UIAsk.h"
#include "UI/UIBridgeConnections.h"
//#include "UI/UISystem.h"
//#include "UI/UITimers.h"
#include <string>
//#include <string.h>
//#include <inttypes.h>
//#include <time.h>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    RunBridgeConDialog
 *
 * SYNOPSIS:
 *    void RunBridgeConDialog(class TheMainWindow *MW);
 *
 * PARAMETERS:
 *    MW [I] -- The main window that this dialog was started from.
 *
 * FUNCTION:
 *    This function shows the bridge connection dialog.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RunBridgeConDialog(class TheMainWindow *MW)
{
    bool AllocatedUI;
    t_MainWindowConnectionList ConList;
    i_MainWindowConnectionList CurrentCon;
    class Connection *Con;
    std::string ConName;
    t_UILabelCtrl *Con1Label;
    t_UIComboBoxCtrl *Con2Combox;
    t_UICheckboxCtrl *LockCheckbox;

    AllocatedUI=false;
    try
    {
        if(MW->ActiveCon==NULL)
            throw("No active connection");

        if(!UIAlloc_BridgeCon())
            throw("Failed to open window");

        Con1Label=UIBC_GetLabelHandle(e_BC_Label_Connection1);
        Con2Combox=UIBC_GetComboBoxHandle(e_BC_Combox_Connection2);
        LockCheckbox=UIBC_GetCheckboxHandle(e_BC_Checkbox_Locked);

        MW->ActiveCon->GetDisplayName(ConName);
        UISetLabelText(Con1Label,ConName.c_str());

        /* Fill in the list of available connections */
        MW->GetListOfConnections(ConList);
        for(CurrentCon=ConList.begin();CurrentCon!=ConList.end();CurrentCon++)
        {
            Con=*CurrentCon;

            /* Skip the active connection */
            if(Con==MW->ActiveCon)
                continue;

            Con->GetDisplayName(ConName);
            UIAddItem2ComboBox(Con2Combox,ConName.c_str(),(uintptr_t)Con);
        }

        if(UIShow_BridgeCon())
        {
            Con=(class Connection *)UIGetComboBoxSelectedEntry(Con2Combox);

            Con->SetLockOutConnectionWhenBridged(
                    UIGetCheckboxCheckStatus(LockCheckbox));
            MW->ActiveCon->SetLockOutConnectionWhenBridged(
                    UIGetCheckboxCheckStatus(LockCheckbox));

            Con->BridgeConnection(MW->ActiveCon);
            MW->ActiveCon->BridgeConnection(Con);
        }
    }
    catch(const char *Error)
    {
        UIAsk("Error opening bridge connections dialog",Error,e_AskBox_Error,
                e_AskBttns_Ok);
    }
    catch(...)
    {
        UIAsk("Error opening bridge connections","Failed to bridge connections",
                e_AskBox_Error,e_AskBttns_Ok);
    }

    if(AllocatedUI)
        UIFree_BridgeCon();
}

/*******************************************************************************
 * NAME:
 *    BC_Event
 *
 * SYNOPSIS:
 *    bool BC_Event(const struct BCEvent *Event);
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
bool BC_Event(const struct BCEvent *Event)
{
    switch(Event->EventType)
    {
        case e_BCEvent_ComboxChange:
            switch(Event->Info.Combox.BoxID)
            {
                case e_BC_Combox_Connection2:
                break;
                case e_BC_ComboxMAX:
                default:
                break;
            }
        break;
        case e_BCEvent_CheckboxChange:
            switch(Event->Info.Checkbox.BoxID)
            {
                case e_BC_Checkbox_Locked:
                case e_BC_CheckboxMAX:
                default:
                break;
            }
        break;
        case e_BCEventMAX:
        default:
        break;
    }
    return true;
}
