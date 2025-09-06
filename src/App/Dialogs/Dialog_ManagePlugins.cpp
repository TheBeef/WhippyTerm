/*******************************************************************************
 * FILENAME: Dialog_ManagePlugins.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the manage plugin dialog in it.  It lets you
 *    enable / disable plugins, install new plugins, and uninstall plugins.
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
 *    Paul Hutchinson (04 Mar 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_ManagePlugins.h"
#include "App/PluginSupport/ExternPluginsSystem.h"
#include "App/PluginSupport/PluginSystem.h"
#include "UI/UIManagePlugins.h"
#include "UI/UIAsk.h"
#include <string.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static void DMP_SelectPlugin(int PluginIndex);
static void DMP_ToggleEnable(void);
static void DMP_RethinkEnableButton(bool BttnEnabled);
static void DMP_UninstallPlugin(void);
static void DMP_BuildPluginList(void);
static void DMP_UpgradePlugin(void);

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    RunManagePluginsDialog
 *
 * SYNOPSIS:
 *    void RunManagePluginsDialog(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function shows the manage plugins dialog.  It lets the user
 *    enable/disable, install / uninstall plugins.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RunManagePluginsDialog(void)
{
    if(!UIAlloc_ManagePlugins())
    {
        UIAsk("Error","Failed to allocate manage plugins dialog",
                e_AskBox_Error,e_AskBttns_Ok);
        return;
    }

    DMP_BuildPluginList();
    DMP_RethinkEnableButton(false);

    UIShow_ManagePlugins();
    UIFree_ManagePlugins();
}

/*******************************************************************************
 * NAME:
 *    DMP_Event
 *
 * SYNOPSIS:
 *    bool DMP_Event(const struct DMPEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event
 *
 * FUNCTION:
 *    This function is called from the UI to send a UI event to the manage
 *    plugins dialog.
 *
 * RETURNS:
 *    true -- Accept this event
 *    false -- Cancel this event
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DMP_Event(const struct DMPEvent *Event)
{
    t_UIListViewCtrl *PluginList;

    switch(Event->EventType)
    {
        case e_DMPEvent_BttnTriggered:
            switch(Event->Info.Bttn.InputID)
            {
                case e_UIMP_Button_Enable:
                    DMP_ToggleEnable();
                break;
                case e_UIMP_Button_Install:
                    PromptAndInstallPlugin();
                    PluginList=
                            UIMP_GetListViewHandle(e_UIMP_ListView_PluginList);
                    UIClearListView(PluginList);
                    DMP_BuildPluginList();
                break;
                case e_UIMP_Button_Uninstall:
                    DMP_UninstallPlugin();
                break;
                case e_UIMP_Button_Upgrade:
                    DMP_UpgradePlugin();
                break;
                case e_UIMP_ButtonMAX:
                default:
                break;
            }
        break;
        case e_DMPEvent_ListViewChange:
            switch(Event->Info.ListView.InputID)
            {
                case e_UIMP_ListView_PluginList:
                    DMP_SelectPlugin(Event->ID);
                break;
                case e_UIMP_ListViewMAX:
                default:
                break;
            }
        break;
        case e_DMPEvent_DialogOk:
        break;
        case e_DMPEventMAX:
        default:
        break;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    DMP_BuildPluginList
 *
 * SYNOPSIS:
 *    static void DMP_BuildPluginList(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function builds the list of plugins and fills in the list view.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DMP_BuildPluginList(void)
{
    t_UIListViewCtrl *PluginList;
    int Index;
    struct ExternPluginInfo PluginInfo;

    PluginList=UIMP_GetListViewHandle(e_UIMP_ListView_PluginList);

    Index=0;
    while(GetExternPluginInfo(Index,PluginInfo)!=false)
    {
        UIAddItem2ListView(PluginList,PluginInfo.PluginName.c_str(),Index);

        if(!PluginInfo.Enabled)
            UIStyleListViewItem(PluginList,Index,UISTYLE_STRIKETHROUGH);

        Index++;
    }
}

/*******************************************************************************
 * NAME:
 *    DMP_SelectPlugin
 *
 * SYNOPSIS:
 *    static void DMP_SelectPlugin(int PluginIndex);
 *
 * PARAMETERS:
 *    PluginIndex [I] -- The index into the plugin that has been selected
 *
 * FUNCTION:
 *    This function updates the UI with the info from the plugin when
 *    it is selected in the UI
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DMP_SelectPlugin(int PluginIndex)
{
    struct ExternPluginInfo PluginInfo;
    t_UITextInputCtrl *Name;
    t_UITextInputCtrl *Copyright;
    t_UITextInputCtrl *ReleaseDate;
    t_UITextInputCtrl *Contrib;
    t_UITextInputCtrl *PluginType;
    t_UITextInputCtrl *Version;
    t_UIMuliLineTextInputCtrl *Description;
    t_UIButtonCtrl *Enable;
    t_UIButtonCtrl *Uninstall;
    t_UIButtonCtrl *Upgrade;
    char buff[100];
    const char *PluginTypeStr;
    uint8_t VMaj;
    uint8_t VMin;
    uint8_t VPat;
    uint8_t VLet;

    Name=UIMP_GetTextInputHandle(e_UIMP_TextInput_Name);
    Copyright=UIMP_GetTextInputHandle(e_UIMP_TextInput_Copyright);
    ReleaseDate=UIMP_GetTextInputHandle(e_UIMP_TextInput_ReleaseDate);
    Contrib=UIMP_GetTextInputHandle(e_UIMP_TextInput_Contrib);
    PluginType=UIMP_GetTextInputHandle(e_UIMP_TextInput_PluginType);
    Version=UIMP_GetTextInputHandle(e_UIMP_TextInput_Version);
    Description=UIMP_GetMultiLineTextInputHandle(e_UIMP_MultiLineTextInput_Description);

    Enable=UIMP_GetButtonHandle(e_UIMP_Button_Enable);
    Uninstall=UIMP_GetButtonHandle(e_UIMP_Button_Uninstall);
    Upgrade=UIMP_GetButtonHandle(e_UIMP_Button_Upgrade);

    if(!GetExternPluginInfo(PluginIndex,PluginInfo))
    {
        PluginInfo.Enabled=false;
        PluginInfo.PluginName="";
        PluginInfo.Copyright="";
        PluginInfo.ReleaseDate="";
        PluginInfo.Contributors="";
        PluginInfo.Description="";
        PluginInfo.PluginClass=e_ExtPluginClassMAX;
        PluginInfo.Version=0;
        UIEnableButton(Enable,false);
        UIEnableButton(Uninstall,false);
        UIEnableButton(Upgrade,false);
    }
    else
    {
        UIEnableButton(Enable,true);
        UIEnableButton(Uninstall,true);
        UIEnableButton(Upgrade,true);
    }

    DMP_RethinkEnableButton(PluginInfo.Enabled);

    UISetTextCtrlText(Name,PluginInfo.PluginName.c_str());
    UISetTextCtrlText(Copyright,PluginInfo.Copyright.c_str());
    UISetTextCtrlText(ReleaseDate,PluginInfo.ReleaseDate.c_str());
    UISetTextCtrlText(Contrib,PluginInfo.Contributors.c_str());
    UISetMuliLineTextCtrlText(Description,PluginInfo.Description.c_str());

    PluginTypeStr="";
    switch(PluginInfo.PluginClass)
    {
        case e_ExtPluginClass_Unknown:
            PluginTypeStr="Unknown";
        break;
        case e_ExtPluginClass_DataProcessor:
            PluginTypeStr=buff;
            strcpy(buff,"Data Processor");
            switch(PluginInfo.PluginSubClass)
            {
                case e_ExtPluginSubClass_DataProTxt_CharEncoding:
                    strcat(buff," (Char Encoding)");
                break;
                case e_ExtPluginSubClass_DataProTxt_TermEmulation:
                    strcat(buff," (Terminal Emulation)");
                break;
                case e_ExtPluginSubClass_DataProTxt_Highlighter:
                    strcat(buff," (Highlighter)");
                break;
                case e_ExtPluginSubClass_DataProTxt_Logger:
                    strcat(buff," (Logger)");
                break;
                case e_ExtPluginSubClass_DataProTxt_Other:
                    strcat(buff," (Other)");
                break;
                case e_ExtPluginSubClass_DataProTxtMAX:
                default:
                break;
            }
        break;
        case e_ExtPluginClass_IODriver:
            PluginTypeStr="IO Driver";
        break;
        case e_ExtPluginClass_FileTransfer:
            PluginTypeStr="File Transfer";
        break;
        case e_ExtPluginClass_Tool:
            PluginTypeStr="Tool";
        break;
        case e_ExtPluginClassMAX:
        default:
        break;
    }
    UISetTextCtrlText(PluginType,PluginTypeStr);

    VMaj=(PluginInfo.Version&0xFF000000)>>24;
    VMin=(PluginInfo.Version&0x00FF0000)>>16;
    VPat=(PluginInfo.Version&0x0000FF00)>>8;
    VLet=(PluginInfo.Version&0x000000FF);

    if(VPat==0 && VLet==0)
        sprintf(buff,"%d.%d\n",VMaj,VMin);
    else if(VLet==0)
        sprintf(buff,"%d.%d.%d\n",VMaj,VMin,VPat);
    else
        sprintf(buff,"%d.%d.%d%c\n",VMaj,VMin,VPat,VLet+'A'-1);
    UISetTextCtrlText(Version,buff);
}

/*******************************************************************************
 * NAME:
 *    DMP_RethinkEnableButton
 *
 * SYNOPSIS:
 *    static void DMP_RethinkEnableButton(bool BttnEnabled);
 *
 * PARAMETERS:
 *    BttnEnabled [I] -- Is the button a enabled or disable button
 *
 * FUNCTION:
 *    This function rethinks the enable/disable button.  It will change the
 *    label depending on if it is enabled or disabled.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DMP_RethinkEnableButton(bool BttnEnabled)
{
    t_UIButtonCtrl *Enable;

    Enable=UIMP_GetButtonHandle(e_UIMP_Button_Enable);

    if(BttnEnabled)
        UISetButtonLabel(Enable,"Disable plugin");
    else
        UISetButtonLabel(Enable,"Enable plugin");
}

/*******************************************************************************
 * NAME:
 *    DMP_ToggleEnable
 *
 * SYNOPSIS:
 *    static void DMP_ToggleEnable(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function toggles the currently selected plugins enable / disable
 *    flag.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DMP_ToggleEnable(void)
{
    t_UIListViewCtrl *PluginList;
    struct ExternPluginInfo PluginInfo;
    int Index;

    PluginList=UIMP_GetListViewHandle(e_UIMP_ListView_PluginList);

    Index=UIGetListViewSelectedEntry(PluginList);
    if(GetExternPluginInfo(Index,PluginInfo))
    {
        /* See if it's currently in use */
        if(PluginInfo.Enabled && IsPluginInUse(&PluginInfo))
        {
            UIAsk("Disable plugin","This plugin is currently in use and "
                    "can not be disabled.\n\nClose any connections that "
                    "are using this plugin and try again.",
                    e_AskBox_Error,e_AskBttns_Ok);
            return;
        }

        PluginInfo.Enabled=!PluginInfo.Enabled;

        SetExternPluginEnabled(Index,PluginInfo.Enabled);

        if(PluginInfo.Enabled)
            UIStyleListViewItem(PluginList,Index,0);
        else
            UIStyleListViewItem(PluginList,Index,UISTYLE_STRIKETHROUGH);

        DMP_RethinkEnableButton(PluginInfo.Enabled);
    }
}

/*******************************************************************************
 * NAME:
 *    DMP_UninstallPlugin
 *
 * SYNOPSIS:
 *    static void DMP_UninstallPlugin(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    Prompts the user if they are sure and if so uninstalles the selected
 *    plugin.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DMP_UninstallPlugin(void)
{
    t_UIListViewCtrl *PluginList;
    struct ExternPluginInfo PluginInfo;
    int Index;
    string Msg;

    PluginList=UIMP_GetListViewHandle(e_UIMP_ListView_PluginList);

    Index=UIGetListViewSelectedEntry(PluginList);
    if(GetExternPluginInfo(Index,PluginInfo))
    {
        /* See if it's currently in use */
        if(IsPluginInUse(&PluginInfo))
        {
            UIAsk("Uninstall plugin","This plugin is currently in use and "
                    "can not be uninstalled.\n\nClose any connections that "
                    "are using this plugin and try again.",
                    e_AskBox_Error,e_AskBttns_Ok);
            return;
        }

        Msg="Are you sure you want to uninstall:\n";
        Msg+=PluginInfo.PluginName;

        if(UIAsk("Uninstall plugin",Msg,e_AskBox_Question,
                e_AskBttns_YesNoCancel)==e_AskRet_Yes)
        {
            UninstallExternPlugin(Index);
            UIClearListView(PluginList);
            DMP_BuildPluginList();
        }
    }
}

/*******************************************************************************
 * NAME:
 *    DMP_UpgradePlugin
 *
 * SYNOPSIS:
 *    static void DMP_UpgradePlugin(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function lets the user select a plugin to upgrade.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DMP_UpgradePlugin(void)
{
    t_UIListViewCtrl *PluginList;
    int Index;

    PluginList=UIMP_GetListViewHandle(e_UIMP_ListView_PluginList);

    Index=UIGetListViewSelectedEntry(PluginList);

    PromptAndUpgradePlugin(Index);

    /* Rebuild the plugin list (so it refreshs the version numbers) */
    UIClearListView(PluginList);
    DMP_BuildPluginList();
    DMP_SelectPlugin(Index);    // Assume it will have the same index
    UISetListViewSelectedEntry(PluginList,Index);
}
