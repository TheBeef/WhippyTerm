/*******************************************************************************
 * FILENAME: Dialog_InstallPlugin.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the install plugin dialog in it.  It lets you see info
 *    about the plugin you are about to install and install the plugin.
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (06 Mar 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_InstallPlugin.h"
#include "App/PluginSupport/ExternPluginsSystem.h"
#include "UI/UIInstallPlugin.h"
#include "UI/UIAsk.h"
#include <string.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static void DIP_FillInUI(const struct ExternPluginInfo &PluginInfo);

/*** VARIABLE DEFINITIONS     ***/
static const char *DIP_WTPFilename;

/*******************************************************************************
 * NAME:
 *    RunInstallPluginDialog
 *
 * SYNOPSIS:
 *    void RunInstallPluginDialog(const char *PluginFilename);
 *
 * PARAMETERS:
 *    PluginFilename [I] -- The path to the plugin file to install.
 *
 * FUNCTION:
 *    This function shows the install plugins dialog.  It lets the user
 *    install plugins.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RunInstallPluginDialog(const char *PluginFilename)
{
    struct ExternPluginInfo Info;

    DIP_WTPFilename=PluginFilename;

    if(!UIAlloc_InstallPlugin())
    {
        UIAsk("Error","Failed to allocate manage plugins dialog",
                e_AskBox_Error,e_AskBttns_Ok);
        return;
    }

    /* Get info about the plugin */
    if(!GetNewExternPluginInfo(PluginFilename,Info))
    {
        UIFree_InstallPlugin();
        return;
    }

    /* Fill in UI */
    DIP_FillInUI(Info);

    UIShow_InstallPlugin();
    UIFree_InstallPlugin();
}

/*******************************************************************************
 * NAME:
 *    DIP_Event
 *
 * SYNOPSIS:
 *    bool DIP_Event(const struct DIPEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event
 *
 * FUNCTION:
 *    This function is called from the UI to send a UI event to the install
 *    plugin dialog.
 *
 * RETURNS:
 *    true -- Accept this event
 *    false -- Cancel this event
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool DIP_Event(const struct DIPEvent *Event)
{
    switch(Event->EventType)
    {
        case e_DIPEvent_DialogOk:
            InstallNewExternPlugin(DIP_WTPFilename);
        break;
        case e_DIPEventMAX:
        default:
        break;
    }
    return true;
}

static void DIP_FillInUI(const struct ExternPluginInfo &PluginInfo)
{
    t_UITextInputCtrl *Name;
    t_UITextInputCtrl *Copyright;
    t_UITextInputCtrl *ReleaseDate;
    t_UITextInputCtrl *Contrib;
    t_UITextInputCtrl *PluginType;
    t_UITextInputCtrl *Version;
    t_UIMuliLineTextInputCtrl *Description;
    char buff[100];
    const char *PluginTypeStr;
    uint8_t VMaj;
    uint8_t VMin;
    uint8_t VPat;
    uint8_t VLet;

    Name=UIIP_GetTextInputHandle(e_UIIP_TextInput_Name);
    Copyright=UIIP_GetTextInputHandle(e_UIIP_TextInput_Copyright);
    ReleaseDate=UIIP_GetTextInputHandle(e_UIIP_TextInput_ReleaseDate);
    Contrib=UIIP_GetTextInputHandle(e_UIIP_TextInput_Contrib);
    PluginType=UIIP_GetTextInputHandle(e_UIIP_TextInput_PluginType);
    Version=UIIP_GetTextInputHandle(e_UIIP_TextInput_Version);
    Description=UIIP_GetMultiLineTextInputHandle(e_UIIP_MultiLineTextInput_Description);

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
