/*******************************************************************************
 * FILENAME: Dialog_ESB_CRCType.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This is a sub dialog for the Edit Send Buffer dialog.  It does the
 *    crc type dialog.
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (30 Aug 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_ESB_CRCType.h"
#include "App/Dialogs/Dialog_ESB_ViewSource.h"
#include "UI/UI_ESB_CRCType.h"
#include "UI/UIAsk.h"
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    RunESB_CRCTypeDialog
 *
 * SYNOPSIS:
 *    bool RunESB_CRCTypeDialog(e_CRCType &CRCType);
 *
 * PARAMETERS:
 *    CRCType [I/O] -- The CRC to select.  This will be set to the selected
 *                     value if the user presses Ok.
 *
 * FUNCTION:
 *    This function shows the edit send buffer crc type dialog.
 *
 * RETURNS:
 *    true -- User selected Ok
 *    false -- User selected Cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool RunESB_CRCTypeDialog(e_CRCType &CRCType)
{
    bool RetValue;
    t_UIListViewCtrl *CRCList;
    t_ListViewItemListType ListOfCRCs;

    try
    {
        if(!UIAlloc_ESB_CRCType())
            return false;

        CRCList=UIESBCR_GetListView(e_ESBCR_ListView_CRCList);

        if(!CRC_GetListOfAvailableCRCs(ListOfCRCs))
            throw("Failed to build the list of CRC's (out of memory)");

        UIAddList2ListView(CRCList,ListOfCRCs);
        UISetListViewSelectedEntry(CRCList,CRCType);

        RetValue=UIShow_ESB_CRCType();

        if(RetValue)
            CRCType=(e_CRCType)UIGetListViewSelectedEntry(CRCList);
    }
    catch(const char *Msg)
    {
        UIAsk("Error",Msg,e_AskBox_Error,e_AskBttns_Ok);
        RetValue=false;
    }
    catch(...)
    {
        RetValue=false;
    }

    UIFree_ESB_CRCType();

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    ESBCT_Event
 *
 * SYNOPSIS:
 *    bool ESBCT_Event(const struct ESBEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event from the UI
 *
 * FUNCTION:
 *    This function is called from the UI when there is an event for the
 *    edit send buffer crc type dialog.
 *
 * RETURNS:
 *    true -- Handle the event noramlly
 *    false -- Cancel the event
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool ESBCT_Event(const struct ESBCTEvent *Event)
{
    string Source;
    e_CRCType CRCType;
    t_UIListViewCtrl *CRCList;

    CRCList=UIESBCR_GetListView(e_ESBCR_ListView_CRCList);

    switch(Event->EventType)
    {
        case e_ESBCT_Button_ShowSource:
            switch(Event->Info.Bttn.BttnID)
            {
                case e_ESBCT_Button_ShowSource:
                    CRCType=(e_CRCType)UIGetListViewSelectedEntry(CRCList);
                    if(CRCType>=e_CRCMAX)
                    {
                        RunESB_ViewSourceDialog("Error.  Invalid CRC selection");
                    }
                    else
                    {
                        if(CRC_BuildSource4CRC(CRCType,Source))
                            RunESB_ViewSourceDialog(Source.c_str());
                    }
                break;
                case e_ESBCT_ButtonMAX:
                default:
                break;
            }
        break;
        case e_ESBCTEventMAX:
        default:
        break;
    }
    return true;
}
