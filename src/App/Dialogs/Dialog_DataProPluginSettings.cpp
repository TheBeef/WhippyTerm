/*******************************************************************************
 * FILENAME: Dialog_DataProPluginSettings.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the dialog for the data processor plugin settings.
 *
 * COPYRIGHT:
 *    Copyright 15 Jun 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (15 Jun 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/DataProcessorsSystem.h"
#include "Dialog_DataProPluginSettings.h"
#include "UI/UIDataProPluginSettings.h"
#include "UI/UIAsk.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static void *DDPPS_PluginFnCallback(e_DataProPlugSettingsFnType Fn,void *Arg1,void *Arg2);

/*** VARIABLE DEFINITIONS     ***/
static t_DataProSettingsWidgetsType *m_DDPPS_PluginPrivData;

/*******************************************************************************
 * NAME:
 *    RunDataProPluginSettingsDialog
 *
 * SYNOPSIS:
 *    void RunDataProPluginSettingsDialog(class ConSettings *Settings,
 *              const char *DataProIDStr);
 *
 * PARAMETERS:
 *    Settings [I] -- Where the settings are stored.
 *    DataProIDStr [I] -- The data processor we are going to do the dialog
 *                        for.  This is the IDStr from 'struct DPS_ProInfo'
 *
 * FUNCTION:
 *    This function shows the data pro plugin settings dialog.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RunDataProPluginSettingsDialog(class ConSettings *Settings,
        const char *DataProIDStr)
{
    t_UITabCtrl *TabCtrl;
    t_UITab *FirstTab;
    t_UILayoutContainerCtrl *FirstTabContainer;

    if(!UIAlloc_DataProPluginSettings())
        return;

    TabCtrl=UIDPPS_GetTabControl();
    FirstTab=UITabCtrlGetTabFromIndex(TabCtrl,0);
    FirstTabContainer=UIDPPS_GetTabContainerFrame(FirstTab);

    DPS_PluginSettings_SetActiveCtrls(DDPPS_PluginFnCallback);

    m_DDPPS_PluginPrivData=DPS_PluginSettings_AddWidgets(Settings,DataProIDStr,
            FirstTabContainer);
    if(m_DDPPS_PluginPrivData==NULL)
    {
        UIAsk("Error","Failed to have the plugin add it's widgets.",e_AskBox_Error);
        UIFree_DataProPluginSettings();
        return;
    }

    if(UIShow_DataProPluginSettings())
    {
        DPS_PluginSettings_SetSettingsFromWidgets(Settings,DataProIDStr,
                m_DDPPS_PluginPrivData);
    }

    DPS_PluginSettings_FreeWidgets(DataProIDStr,m_DDPPS_PluginPrivData);

    DPS_PluginSettings_SetActiveCtrls(NULL);

    UIFree_DataProPluginSettings();
}

/*******************************************************************************
 * NAME:
 *    DDPPS_PluginFnCallback
 *
 * SYNOPSIS:
 *    void *DDPPS_PluginFnCallback(e_DataProPlugSettingsFnType Fn,void *Arg1,void *Arg2);
 *
 * PARAMETERS:
 *    Fn [I] -- What function is the plugin trying to do.  Supported fns:
 *                  e_DataProPlugSettingsFn_SetCurrentTabName -- Changes the
 *                          name of the current tab that widgets are being
 *                          added to.
 *                              Arg1 -- "const char *" with the new name in it.
 *                              Arg2 -- ignored.
 *                              Return value: NULL
 *                  e_DataProPlugSettingsFn_AddNewTab -- A new tab should
 *                          be allocated and the t_UILayoutContainerCtrl for
 *                          this tab should be returned.
 *                              Arg1 -- "const char *" with the tab name in it.
 *                              Arg2 -- ignored.
 *                              Return value: "t_UILayoutContainerCtrl *" with
 *                                      the handle to the container for the
 *                                      new tab in it.
 *
 * FUNCTION:
 *    This function is called when a plugin is in its settings and wants
 *    to preform an action.  See above for the actions.
 *
 * RETURNS:
 *    Depends on the 'Fn'  See above.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void *DDPPS_PluginFnCallback(e_DataProPlugSettingsFnType Fn,void *Arg1,
        void *Arg2)
{
    t_UITabCtrl *TabCtrl;
    int TabCount;
    t_UITab *ActiveTab;
    t_UILayoutContainerCtrl *ActiveTabContainer;
    t_UITab *NewTab;

    TabCtrl=UIDPPS_GetTabControl();
    TabCount=UITabCtrlGetTabCount(TabCtrl);
    ActiveTab=UITabCtrlGetTabFromIndex(TabCtrl,TabCount-1);
    ActiveTabContainer=UIDPPS_GetTabContainerFrame(ActiveTab);

    switch(Fn)
    {
        case e_DataProPlugSettingsFn_SetCurrentTabName:
            UITabCtrlSetTabLabel(TabCtrl,ActiveTab,(const char *)Arg1);
        break;
        case e_DataProPlugSettingsFn_AddNewTab:
            NewTab=UIDPPS_AddNewTab((const char *)Arg1);
            ActiveTabContainer=UIDPPS_GetTabContainerFrame(NewTab);
            return (void *)ActiveTabContainer;
        break;
        case e_DataProPlugSettingsFnMAX:
        default:
        break;
    }
    return NULL;
}

