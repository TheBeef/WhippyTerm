/*******************************************************************************
 * FILENAME: Dialog_IODriverSettings.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the dialog for the IO Driver settings.
 *
 * COPYRIGHT:
 *    Copyright 12 Nov 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (12 Nov 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/IOSystem.h"
#include "App/Settings.h"
#include "Dialog_IODriverSettings.h"
#include "UI/UIIODriverSettings.h"
#include "UI/UIAsk.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static void *DIODS_IODriverFnCallback(e_IODriverSettingsFnType Fn,void *Arg1,void *Arg2);

/*** VARIABLE DEFINITIONS     ***/
static t_IODriverSettingsWidgets *m_DIODS_PluginPrivData;

/*******************************************************************************
 * NAME:
 *    RunIODriverSettingsDialog
 *
 * SYNOPSIS:
 *    void RunIODriverSettingsDialog(t_PluginSettings *Settings,
 *              const char *BaseURI);
 *
 * PARAMETERS:
 *    Settings [I] -- Where the settings are stored.
 *    BaseURI [I] -- The Base URI for the driver we want to do setting on
 *
 * FUNCTION:
 *    This function shows the IO Driver settings dialog for a driver.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RunIODriverSettingsDialog(t_PluginSettings *Settings,
        const char *BaseURI)
{
    t_UITabCtrl *TabCtrl;
    t_UITab *FirstTab;
    t_UILayoutContainerCtrl *FirstTabContainer;

    if(!UIAlloc_IODriverSettings())
        return;

    TabCtrl=UIIODS_GetTabControl();
    FirstTab=UITabCtrlGetTabFromIndex(TabCtrl,0);
    FirstTabContainer=UIIODS_GetTabContainerFrame(FirstTab);

    IOS_DriverSettings_SetActiveCtrls(DIODS_IODriverFnCallback);

    m_DIODS_PluginPrivData=IOS_DriverSettings_AddWidgets(Settings,
            BaseURI,FirstTabContainer);
    if(m_DIODS_PluginPrivData==NULL)
    {
        UIAsk("Error","Failed to have the IODriver add it's widgets.",
                e_AskBox_Error);
        UIFree_IODriverSettings();
        return;
    }

    if(UIShow_IODriverSettings())
    {
        IOS_DriverSettings_SetSettingsFromWidgets(Settings,BaseURI,
                m_DIODS_PluginPrivData);
    }

    IOS_DriverSettings_FreeWidgets(BaseURI,m_DIODS_PluginPrivData);

    IOS_DriverSettings_SetActiveCtrls(NULL);

    UIFree_IODriverSettings();
}

///*******************************************************************************
// * NAME:
// *    DIODS_IODriverFnCallback
// *
// * SYNOPSIS:
// *    void *DIODS_IODriverFnCallback(e_IODriverSettingsFnType Fn,void *Arg1,void *Arg2);
// *
// * PARAMETERS:
// *    Fn [I] -- What function is the plugin trying to do.  Supported fns:
// *                  e_DataProPlugSettingsFn_SetCurrentTabName -- Changes the
// *                          name of the current tab that widgets are being
// *                          added to.
// *                              Arg1 -- "const char *" with the new name in it.
// *                              Arg2 -- ignored.
// *                              Return value: NULL
// *                  e_DataProPlugSettingsFn_AddNewTab -- A new tab should
// *                          be allocated and the t_UILayoutContainerCtrl for
// *                          this tab should be returned.
// *                              Arg1 -- "const char *" with the tab name in it.
// *                              Arg2 -- ignored.
// *                              Return value: "t_UILayoutContainerCtrl *" with
// *                                      the handle to the container for the
// *                                      new tab in it.
// *
// * FUNCTION:
// *    This function is called when a plugin is in its settings and wants
// *    to preform an action.  See above for the actions.
// *
// * RETURNS:
// *    Depends on the 'Fn'  See above.
// *
// * SEE ALSO:
// *    
// ******************************************************************************/
static void *DIODS_IODriverFnCallback(e_IODriverSettingsFnType Fn,void *Arg1,
        void *Arg2)
{
    t_UITabCtrl *TabCtrl;
    int TabCount;
    t_UITab *ActiveTab;
    t_UILayoutContainerCtrl *ActiveTabContainer;
    t_UITab *NewTab;

    TabCtrl=UIIODS_GetTabControl();
    TabCount=UITabCtrlGetTabCount(TabCtrl);
    ActiveTab=UITabCtrlGetTabFromIndex(TabCtrl,TabCount-1);
    ActiveTabContainer=UIIODS_GetTabContainerFrame(ActiveTab);

    switch(Fn)
    {
        case e_IODriverSettingsFn_SetCurrentTabName:
            UITabCtrlSetTabLabel(TabCtrl,ActiveTab,(const char *)Arg1);
        break;
        case e_IODriverSettingsFn_AddNewTab:
            NewTab=UIIODS_AddNewTab((const char *)Arg1);
            ActiveTabContainer=UIIODS_GetTabContainerFrame(NewTab);
            return (void *)ActiveTabContainer;
        break;
        case e_IODriverSettingsFnMAX:
        default:
        break;
    }
    return NULL;
}

