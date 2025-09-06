/*******************************************************************************
 * FILENAME: MW_ConnectionOptions.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the main window's connection options panel tab in it.
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
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
 *    Paul Hutchinson (03 Mar 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Connections.h"
#include "App/IOSystem.h"
#include "App/MWPanels/MW_ConnectionOptions.h"
#include "UI/UIAsk.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static void MWConnectionOptionEvent(void *UserData);

/*** VARIABLE DEFINITIONS     ***/

MWConnectionOptions::MWConnectionOptions()
{
    ConnectionOptionsWidgets=NULL;
    UIWin=NULL;
    MW=NULL;
}

MWConnectionOptions::~MWConnectionOptions()
{
    if(ConnectionOptionsWidgets!=NULL)
    {
        IOS_FreeConnectionOptions(ConnectionOptionsWidgets);
        ConnectionOptionsWidgets=NULL;
    }
}

/*******************************************************************************
 * NAME:
 *    MWConnectionOptions::Setup
 *
 * SYNOPSIS:
 *    void MWConnectionOptions::Setup(class TheMainWindow *Parent,
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
void MWConnectionOptions::Setup(class TheMainWindow *Parent,t_UIMainWindow *Win)
{
    MW=Parent;
    UIWin=Win;
}

/*******************************************************************************
 * NAME:
 *    MWConnectionOptions::ActivateCtrls
 *
 * SYNOPSIS:
 *    void MWConnectionOptions::ActivateCtrls(bool Active);
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
void MWConnectionOptions::ActivateCtrls(bool Active)
{
    t_UIButtonCtrl *Bttn;

    Bttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_ConnectionOptionApply);

    if(Active)
    {
        UIEnableButton(Bttn,false);
    }
    else
    {
        if(ConnectionOptionsWidgets!=NULL)
        {
            IOS_FreeConnectionOptions(ConnectionOptionsWidgets);
            ConnectionOptionsWidgets=NULL;
        }

        UIEnableButton(Bttn,false);
    }
}

/*******************************************************************************
 * NAME:
 *    MWConnectionOptions::ApplyConnectionOptions
 *
 * SYNOPSIS:
 *    void MWConnectionOptions::ApplyConnectionOptions(
 *              class Connection *ApplyCon);
 *
 * PARAMETERS:
 *    ApplyCon [I] -- The connection to apply the options to.
 *
 * FUNCTION:
 *    This function grabs the connection options from the UI and applies them
 *    to a connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWConnectionOptions::ApplyConnectionOptions(class Connection *ApplyCon)
{
    t_KVList Options;

    if(ConnectionOptionsWidgets==NULL)
        return;

    /* Grap from UI */
    IOS_StoreConnectionOptions(ConnectionOptionsWidgets,Options);

    /* Send to connection */
    if(!ApplyCon->SetConnectionOptions(Options))
    {
        UIAsk("Error","There was an error applying connection options",
                e_AskBox_Error,e_AskBttns_Ok);
    }

    MW->HandlePanelAutoCloseLeft();
}

/*******************************************************************************
 * NAME:
 *    MWConnectionOptions::SetupConnectionOptionsPanel
 *
 * SYNOPSIS:
 *    void MWConnectionOptions::SetupConnectionOptionsPanel(
 *          const std::string &UniqueID,t_KVList &Options);
 *
 * PARAMETERS:
 *    UniqueID [I] -- The unique ID that identifies the connection.
 *    Options [I] -- The options for this connection.
 *
 * FUNCTION:
 *    This function setups the the connection options panel with info
 *    sent in.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *
 ******************************************************************************/
bool MWConnectionOptions::SetupConnectionOptionsPanel(const std::string &UniqueID,
        t_KVList &Options)
{
    if(UIWin==NULL)
        return false;

    if(ConnectionOptionsWidgets!=NULL)
    {
        IOS_FreeConnectionOptions(ConnectionOptionsWidgets);
        ConnectionOptionsWidgets=NULL;
    }

    ConnectionOptionsWidgets=IOS_AllocConnectionOptionsFromUniqueID(
            UniqueID.c_str(),UIMW_GetOptionsFrameContainer(UIWin),Options,
            MWConnectionOptionEvent,(void *)this);

    /* Make sure the apply button is clickable */
    ActivateCtrls(true);

    return true;
}

/*******************************************************************************
 * NAME:
 *    MWConnectionOptionEvent
 *
 * SYNOPSIS:
 *    static void MWConnectionOptionEvent(void *UserData);
 *
 * PARAMETERS:
 *    UserData [I] -- A pointer to the 'MWConnectionOptions' class that this
 *                    event goes with.
 *
 * FUNCTION:
 *    This is called when there is a change event for a option widget.  It
 *    just passes it on to the correct class OptionChanged() function.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void MWConnectionOptionEvent(void *UserData)
{
    class MWConnectionOptions *MWCO;

    MWCO=(class MWConnectionOptions *)UserData;
    MWCO->OptionChanged();
}

/*******************************************************************************
 * NAME:
 *    MWConnectionOptions::OptionChanged
 *
 * SYNOPSIS:
 *    void MWConnectionOptions::OptionChanged(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when there is a change to one of the options.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWConnectionOptions::OptionChanged(void)
{
    t_UIButtonCtrl *Bttn;

    Bttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_ConnectionOptionApply);

    /* No mater what the user did we just activate the button (we have no
       idea if the user put things back to there corrent options or
       not) */
    UIEnableButton(Bttn,true);
}

/*******************************************************************************
 * NAME:
 *    MWConnectionOptions::ConnectionChanged
 *
 * SYNOPSIS:
 *    void MWConnectionOptions::ConnectionChanged(void);
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
void MWConnectionOptions::ConnectionChanged(void)
{
    if(ConnectionOptionsWidgets!=NULL)
    {
        IOS_FreeConnectionOptions(ConnectionOptionsWidgets);
        ConnectionOptionsWidgets=NULL;
    }
}

/*******************************************************************************
 * NAME:
 *    MWConnectionOptions::ConnectionAbout2Changed
 *
 * SYNOPSIS:
 *    void MWConnectionOptions::ConnectionAbout2Changed(void);
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
void MWConnectionOptions::ConnectionAbout2Changed(void)
{
}
