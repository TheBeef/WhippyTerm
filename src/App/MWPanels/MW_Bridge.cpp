/*******************************************************************************
 * FILENAME: MW_Bridge.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the main window's bridge panel tab in it.
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
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
 *    Paul Hutchinson (26 Aug 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/MWPanels/MW_Bridge.h"
#include "App/MWPanels/MWPanels.h"
#include "App/MainWindow.h"
#include "App/Connections.h"
#include "App/Settings.h"
#include "UI/UISystem.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

MWBridge::MWBridge()
{
    UIWin=NULL;
    MW=NULL;

    PanelActive=false;
}

MWBridge::~MWBridge()
{
}

/*******************************************************************************
 * NAME:
 *    MWBridge::Setup
 *
 * SYNOPSIS:
 *    void MWBridge::Setup(class TheMainWindow *Parent,
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
void MWBridge::Setup(class TheMainWindow *Parent,t_UIMainWindow *Win)
{
    t_UILabelCtrl *Con1Label;

    MW=Parent;
    UIWin=Win;

    Con1Label=UIMW_GetLabelHandle(UIWin,e_UIMWLabel_Bridge_Connection1);
    UISetLabelText(Con1Label,"");
}

/*******************************************************************************
 * NAME:
 *    MWBridge::ActivateCtrls
 *
 * SYNOPSIS:
 *    void MWBridge::ActivateCtrls(bool Active);
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
void MWBridge::ActivateCtrls(bool Active)
{
    PanelActive=Active;

    RethinkControls();
    RethinkLockNames();
}

/*******************************************************************************
 * NAME:
 *    MWBridge::RethinkControls
 *
 * SYNOPSIS:
 *    void MWBridge::RethinkControls(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rethinks all the control.  It enables/disables all the
 *    controls.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWBridge::RethinkControls(void)
{
    t_UIButtonCtrl *BridgeBttn;
    t_UIButtonCtrl *ReleaseBttn;
    t_UIComboBoxCtrl *Con2Combox;
    t_UICheckboxCtrl *Lock1Checkbox;
    t_UICheckboxCtrl *Lock2Checkbox;
    bool BridgeEnabled;
    bool ReleaseEnabled;
    bool Con1Enabled;
    bool Con2Enabled;
    bool Lock1Enabled;
    bool Lock2Enabled;
    bool CanBridge;
    bool AlreadyBridged;
    class Connection *BridgedCon1;

    if(UIWin==NULL || MW==NULL)
        return;

    BridgeBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_Bridge_Bridge);
    ReleaseBttn=UIMW_GetButtonHandle(UIWin,e_UIMWBttn_Bridge_Release);
    Con2Combox=UIMW_GetComboBoxHandle(UIWin,e_UIMWComboBox_Bridge_Connection2);
    Lock1Checkbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Bridge_Lock1);
    Lock2Checkbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Bridge_Lock2);

    BridgeEnabled=PanelActive;
    ReleaseEnabled=PanelActive;
    Con1Enabled=PanelActive;
    Con2Enabled=PanelActive;
    Lock1Enabled=PanelActive;
    Lock2Enabled=PanelActive;

    if(PanelActive)
    {
        if(MW->ActiveCon->GetBridgedConnection()!=NULL)
            AlreadyBridged=true;
        else
            AlreadyBridged=false;

        CanBridge=true;
        if(AlreadyBridged || UIGetComboBoxSelectedIndex(Con2Combox)<0)
            CanBridge=false;

        if(CanBridge)
            BridgeEnabled=true;
        else
            BridgeEnabled=false;

        ReleaseEnabled=false;
        if(AlreadyBridged)
            ReleaseEnabled=true;

        Con1Enabled=!AlreadyBridged;
        Con2Enabled=!AlreadyBridged;

        Lock1Enabled=AlreadyBridged;
        Lock2Enabled=AlreadyBridged;
    }

    UIEnableButton(BridgeBttn,BridgeEnabled);
    UIEnableButton(ReleaseBttn,ReleaseEnabled);
    UIEnableComboBox(Con2Combox,Con2Enabled);
    UIEnableCheckbox(Lock1Checkbox,Lock1Enabled);
    UIEnableCheckbox(Lock2Checkbox,Lock2Enabled);
}

/*******************************************************************************
 * NAME:
 *    MWBridge::BridgeConnections
 *
 * SYNOPSIS:
 *    void MWBridge::BridgeConnections(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function bridges to the 2 selected connections.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWBridge::BridgeConnections(void)
{
    t_UIComboBoxCtrl *Con2Combox;
    class Connection *Con1;
    class Connection *Con2;
    t_UICheckboxCtrl *Lock1Checkbox;
    t_UICheckboxCtrl *Lock2Checkbox;

    if(!PanelActive || MW->ActiveCon==NULL)
        return;

    Con2Combox=UIMW_GetComboBoxHandle(UIWin,e_UIMWComboBox_Bridge_Connection2);
    Lock1Checkbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Bridge_Lock1);
    Lock2Checkbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Bridge_Lock2);

    if(UIGetComboBoxSelectedIndex(Con2Combox)<0)
        return;

    Con1=MW->ActiveCon;
    Con2=(class Connection *)UIGetComboBoxSelectedEntry(Con2Combox);

    /* Make sure we can bridge these connections */
    if(MW->ActiveCon->GetBridgedConnection()!=NULL ||
            Con2->GetBridgedConnection()!=NULL)
    {
        return;
    }

    Con1->SetLockOutConnectionWhenBridged(
            UIGetCheckboxCheckStatus(Lock1Checkbox));
    Con2->SetLockOutConnectionWhenBridged(
            UIGetCheckboxCheckStatus(Lock2Checkbox));

    Con1->BridgeConnection(Con2);
    Con2->BridgeConnection(Con1);

    RethinkControls();
}

/*******************************************************************************
 * NAME:
 *    MWBridge::ReleaseConnections
 *
 * SYNOPSIS:
 *    void MWBridge::ReleaseConnections(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function unbridges the bridged connectsions.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWBridge::ReleaseConnections(void)
{
    t_UIComboBoxCtrl *Con2Combox;
    class Connection *Con1;
    class Connection *Con2;

    if(MW==NULL || MW->ActiveCon==NULL)
        return;

    Con2Combox=UIMW_GetComboBoxHandle(UIWin,e_UIMWComboBox_Bridge_Connection2);

    Con1=MW->ActiveCon;
    Con2=(class Connection *)UIGetComboBoxSelectedEntry(Con2Combox);

    Con1->BridgeConnection(NULL);
    Con2->BridgeConnection(NULL);

    RethinkControls();
}

/*******************************************************************************
 * NAME:
 *    MWBridge::ConnectionAbout2Changed
 *
 * SYNOPSIS:
 *    void MWBridge::ConnectionAbout2Changed(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called just before the active connection is able to
 *    change to a different connection.
 *
 *    It stores the current UI in to the connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWBridge::ConnectionAbout2Changed(void)
{
}

/*******************************************************************************
 * NAME:
 *    MWBridge::ConnectionChanged
 *
 * SYNOPSIS:
 *    void MWBridge::ConnectionChanged(void);
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
void MWBridge::ConnectionChanged(void)
{
    t_UILabelCtrl *Con1Label;
    std::string ConName;

    Con1Label=UIMW_GetLabelHandle(UIWin,e_UIMWLabel_Bridge_Connection1);

    if(MW==NULL || MW->ActiveCon==NULL)
    {
        UISetLabelText(Con1Label,"");
        return;
    }

    MW->ActiveCon->GetDisplayName(ConName);
    UISetLabelText(Con1Label,ConName.c_str());

    /* We need to rebuild the list of connections (because we need to remove
       invalid connections */
    ConnectionsChanged();
}

/*******************************************************************************
 * NAME:
 *    MWBridge::ConnectionsChanged
 *
 * SYNOPSIS:
 *    void MWBridge::ConnectionsChanged(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called to tell the bridge panel that a connection has
 *    been allocated or freed or changed in some way (like the bridge status
 *    being changed).
 *
 *    It takes an updates the list of connections available for bridging.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWBridge::ConnectionsChanged(void)
{
    t_ConnectionList ConList;
    i_ConnectionList CurrentCon;
    class Connection *Con;
    t_UIComboBoxCtrl *Con2Combox;
    uintptr_t SelectCon1;
    uintptr_t SelectCon2;
    std::string ConName;
    t_UICheckboxCtrl *Lock1Checkbox;
    t_UICheckboxCtrl *Lock2Checkbox;
    class Connection *Con1;
    class Connection *Con2;

    if(MW==NULL || MW->ActiveCon==NULL)
        return;

    Con2Combox=UIMW_GetComboBoxHandle(UIWin,e_UIMWComboBox_Bridge_Connection2);
    Lock1Checkbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Bridge_Lock1);
    Lock2Checkbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Bridge_Lock2);

    Con1=MW->ActiveCon;
    Con2=MW->ActiveCon->GetBridgedConnection();

    SelectCon2=UIGetComboBoxSelectedEntry(Con2Combox);

    UIClearComboBox(Con2Combox);

    Con_GetListOfConnections(ConList);
    for(CurrentCon=ConList.begin();CurrentCon!=ConList.end();CurrentCon++)
    {
        Con=*CurrentCon;

        /* We only add connections that are not already bridged (and are
           not the active connection) */
        if(Con!=MW->ActiveCon && (Con->GetBridgedConnection()==NULL ||
                Con->GetBridgedConnection()==MW->ActiveCon))
        {
            Con->GetDisplayName(ConName);
            UIAddItem2ComboBox(Con2Combox,ConName.c_str(),(uintptr_t)Con);
            if(Con->GetBridgedConnection()==MW->ActiveCon)
                SelectCon2=(uintptr_t)Con;
        }
    }

    UISetComboBoxSelectedEntry(Con2Combox,SelectCon2);
    UISortComboBox(Con2Combox);

    if(Con1!=NULL)
        UICheckCheckbox(Lock1Checkbox,Con1->GetLockOutConnectionWhenBridged());
    if(Con2!=NULL)
        UICheckCheckbox(Lock2Checkbox,Con2->GetLockOutConnectionWhenBridged());

    RethinkLockNames();
    RethinkControls();
}

/*******************************************************************************
 * NAME:
 *    MWBridge::SelectedConnectionChanged
 *
 * SYNOPSIS:
 *    void MWBridge::SelectedConnectionChanged(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when one of the selected connections to bridge
 *    changes.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWBridge::SelectedConnectionChanged(void)
{
    RethinkControls();
    RethinkLockNames();
}

/*******************************************************************************
 * NAME:
 *    MWBridge::LockConnectionChange
 *
 * SYNOPSIS:
 *    void MWBridge::LockConnectionChange(int Connection);
 *
 * PARAMETERS:
 *    Connection [I] -- What connected lock button was pressed.
 *
 * FUNCTION:
 *    This function is called when one of the lock buttons is clicked.  It
 *    locks / unlocks that connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWBridge::LockConnectionChange(int Connection)
{
    t_UIComboBoxCtrl *ConCombox;
    class Connection *Con;
    t_UICheckboxCtrl *LockCheckbox;

    if(MW->ActiveCon==NULL)
        return;

    if(Connection==0)
    {
        LockCheckbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Bridge_Lock1);
        Con=MW->ActiveCon;
    }
    else
    {
        ConCombox=UIMW_GetComboBoxHandle(UIWin,
                e_UIMWComboBox_Bridge_Connection2);
        LockCheckbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Bridge_Lock2);
        Con=(class Connection *)UIGetComboBoxSelectedEntry(ConCombox);
    }

    Con->SetLockOutConnectionWhenBridged(
            UIGetCheckboxCheckStatus(LockCheckbox));
}

/*******************************************************************************
 * NAME:
 *    MWBridge::ConnectionAttribChanged
 *
 * SYNOPSIS:
 *    void MWBridge::ConnectionAttribChanged(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when an attribute of a connection is changed.
 *    Attributes include things like the connection name.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWBridge::ConnectionAttribChanged(void)
{
    ConnectionsChanged();
}

/*******************************************************************************
 * NAME:
 *    MWBridge::RethinkLockNames
 *
 * SYNOPSIS:
 *    void MWBridge::RethinkLockNames(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function takes the currently selected connections and updates
 *    the lock names.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void MWBridge::RethinkLockNames(void)
{
    t_MainWindowConnectionList ConList;
    i_MainWindowConnectionList CurrentCon;
    t_UIComboBoxCtrl *Con2Combox;
    t_UICheckboxCtrl *Lock1Checkbox;
    t_UICheckboxCtrl *Lock2Checkbox;
    class Connection *SelectCon1;
    class Connection *SelectCon2;
    std::string ConName;
    std::string NewLabel;

    if(MW==NULL)
        return;

    Con2Combox=UIMW_GetComboBoxHandle(UIWin,e_UIMWComboBox_Bridge_Connection2);
    Lock1Checkbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Bridge_Lock1);
    Lock2Checkbox=UIMW_GetCheckboxHandle(UIWin,e_UIMWCheckbox_Bridge_Lock2);

    SelectCon1=MW->ActiveCon;
    SelectCon2=(class Connection *)UIGetComboBoxSelectedEntry(Con2Combox);

    if(SelectCon1!=NULL)
        SelectCon1->GetDisplayName(ConName);
    else
        ConName="1";
    NewLabel="Lock ";
    NewLabel+=ConName;
    UISetCheckboxLabel(Lock1Checkbox,NewLabel.c_str());

    if(SelectCon2!=NULL)
        SelectCon2->GetDisplayName(ConName);
    else
        ConName="2";
    NewLabel="Lock ";
    NewLabel+=ConName;
    UISetCheckboxLabel(Lock2Checkbox,NewLabel.c_str());
}
