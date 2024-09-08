/*******************************************************************************
 * FILENAME: MW_AuxControls.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the main window's connection aux controls panel tab in it.
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
 *    Paul Hutchinson (16 Aug 2024)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/IOSystem.h"
#include "App/Connections.h"
#include "App/MWPanels/MW_AuxControls.h"

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
//static void MWAuxControlsEvent(void *UserData);

/*** VARIABLE DEFINITIONS     ***/

MWAuxControls::MWAuxControls()
{
//    AuxControlsWidgets=NULL;
    UIWin=NULL;
    MW=NULL;
}

MWAuxControls::~MWAuxControls()
{
//    if(AuxControlsWidgets!=NULL)
//    {
//        IOS_FreeConnectionAuxCtrls(AuxControlsWidgets);
//        AuxControlsWidgets=NULL;
//    }
}

/*******************************************************************************
 * NAME:
 *    MWAuxControls::Setup
 *
 * SYNOPSIS:
 *    void MWAuxControls::Setup(class TheMainWindow *Parent,
 *              t_UIMainWindow *Win);
 *
 * PARAMETERS:
 *    Parent [I] -- The main window that this function lives in
 *    Win [I] -- The UI window handle.
 *
 * FUNCTION:
 *    This function setups things needed by this class.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWAuxControls::Setup(class TheMainWindow *Parent,t_UIMainWindow *Win)
{
    MW=Parent;
    UIWin=Win;
}

/*******************************************************************************
 * NAME:
 *    MWAuxControls::ActivateCtrls
 *
 * SYNOPSIS:
 *    void MWAuxControls::ActivateCtrls(bool Active);
 *
 * PARAMETERS:
 *    Active [I] -- Should we activate the controls (true), or deactivate them
 *                  (false).
 *
 * FUNCTION:
 *    This function activates/deactivates the controls for this panel.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWAuxControls::ActivateCtrls(bool Active)
{
    if(Active)
    {
    }
    else
    {
    }
}

/*******************************************************************************
 * NAME:
 *    MWAuxControls::NewConnection
 *
 * SYNOPSIS:
 *    bool MWAuxControls::NewConnection(class Connection *Con);
 *
 * PARAMETERS:
 *    Con [I] -- The connection that we will setup the aux controls panel
 *               for.
 *
 * FUNCTION:
 *    This function adds aux controls when a new connection is allocated.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    MWAuxControls::RemoveConnection()
 ******************************************************************************/
bool MWAuxControls::NewConnection(class Connection *Con)
{
    t_IOSystemHandle *IOHandle;
    t_ConnectionAuxCtrlsDataType *AuxControlsWidgets;

    if(Con==NULL || UIWin==NULL)
        return false;

    AuxControlsWidgets=NULL;
    try
    {
        IOHandle=Con->GetIOHandle();
        AuxControlsWidgets=IOS_AllocConnectionAuxCtrls(IOHandle,
                UIMW_GetConAuxControlsFrameContainer(UIWin));
        if(AuxControlsWidgets==NULL)
            throw(0);

        ConWidgets.insert(make_pair(Con,AuxControlsWidgets));
    }
    catch(...)
    {
        if(AuxControlsWidgets==NULL)
            IOS_FreeConnectionAuxCtrls(AuxControlsWidgets);
        return false;
    }

//    ActivateCtrls(true);

    return true;
}

/*******************************************************************************
 * NAME:
 *    MWAuxControls::RemoveConnection
 *
 * SYNOPSIS:
 *    void MWAuxControls::RemoveConnection(class Connection *Con);
 *
 * PARAMETERS:
 *    Con [I] -- The connection that is about to be removed.
 *
 * FUNCTION:
 *    This function frees the aux controls widgets for a connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWAuxControls::RemoveConnection(class Connection *Con)
{
    i_MWAuxControlsConWidgets ConWids;

    if(Con==NULL || UIWin==NULL)
        return;

    /* We need to find this connection in the list of allocated con's */
    ConWids=ConWidgets.find(Con);
    if(ConWids==ConWidgets.end())
        return;

    IOS_FreeConnectionAuxCtrls(ConWids->second);

    ConWidgets.erase(ConWids);
}

///*******************************************************************************
// * NAME:
// *    MWAuxControlsEvent
// *
// * SYNOPSIS:
// *    static void MWAuxControlsEvent(void *UserData);
// *
// * PARAMETERS:
// *    UserData [I] -- A pointer to the 'MWAuxControls' class that this
// *                    event goes with.
// *
// * FUNCTION:
// *    This is called when there is a change event for a option widget.  It
// *    just passes it on to the correct class OptionChanged() function.
// *
// * RETURNS:
// *    NONE
// *
// * SEE ALSO:
// *    
// ******************************************************************************/
//static void MWAuxControlsEvent(void *UserData)
//{
//    class MWAuxControls *MWCO;
//
//    MWCO=(class MWAuxControls *)UserData;
//    MWCO->OptionChanged();
//}

/*******************************************************************************
 * NAME:
 *    MWAuxControls::ConnectionChanged
 *
 * SYNOPSIS:
 *    void MWAuxControls::ConnectionChanged(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function updates the UI after the active connection has changed
 *    to a new connection (MW->ActiveCon).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWAuxControls::ConnectionChanged(void)
{
    i_MWAuxControlsConWidgets ConWids;

    if(UIWin==NULL)
        return;

    /* Hide all controls that are not the active connection */
    for(ConWids=ConWidgets.begin();ConWids!=ConWidgets.end();ConWids++)
    {
        IOS_ConnectionAuxCtrlsShow(ConWids->second,
                ConWids->first==MW->ActiveCon);
    }
}

/*******************************************************************************
 * NAME:
 *    MWAuxControls::ConnectionAbout2Changed
 *
 * SYNOPSIS:
 *    void MWAuxControls::ConnectionAbout2Changed(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called just before the active connection is able to
 *    change to a different connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWAuxControls::ConnectionAbout2Changed(void)
{
}
