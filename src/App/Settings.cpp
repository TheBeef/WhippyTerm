/*******************************************************************************
 * FILENAME: Settings.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the main settings structure in it.  It also handles the
 *    saving and loading of the main settings.
 *
 * COPYRIGHT:
 *    Copyright 2018 Paul Hutchinson.
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
 *    Paul Hutchinson (27 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Display/DisplayColors.h"
#include "App/Settings.h"
#include "App/Commands.h"
#include "App/Connections.h"
#include "OS/FilePaths.h"
#include "OS/Directorys.h"
#include "UI/UIFontReq.h"
#include <string>
#include <string.h>
#include <stdlib.h>

using namespace std;

/*** DEFINES                  ***/
#define SETTINGS_FILE       "Settings.cfg"

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
class WindowStartupPosCFG : public TinyCFGBaseData
{
   public:
      e_WindowStartupPosType *Ptr;
      bool LoadData(string &LoadedString);
      bool SaveData(string &StoreString);
};

class DataProcessorCFG : public TinyCFGBaseData
{
   public:
      e_DataProcessorTypeType *Ptr;
      bool LoadData(string &LoadedString);
      bool SaveData(string &StoreString);
};

class SysColorCFG : public TinyCFGBaseData
{
   public:
      uint32_t (*Ptr)[e_SysColMAX];
    bool LoadElement(class TinyCFG *CFG);
    bool SaveElement(class TinyCFG *CFG);
};

class DefaultColorCFG : public TinyCFGBaseData
{
   public:
      uint32_t *Ptr;
      bool LoadData(string &LoadedString);
      bool SaveData(string &StoreString);
};

class KeyCommandsCFG : public TinyCFGBaseData
{
    public:
        struct CommandKeySeq (*Ptr)[e_CmdMAX];
        bool LoadElement(class TinyCFG *CFG);
        bool SaveElement(class TinyCFG *CFG);
};

/*** FUNCTION PROTOTYPES      ***/
bool Settings_RegisterSysColorType(class TinyCFG &cfg,const char *XmlName,
        uint32_t (*Data)[e_SysColMAX]);
bool Settings_RegisterDefaultColorType(class TinyCFG &cfg,const char *XmlName,
      uint32_t &Data);
bool Settings_RegisterDataProcessorType(class TinyCFG &cfg,const char *XmlName,
      e_DataProcessorTypeType &Data);

/*** VARIABLE DEFINITIONS     ***/
class Settings g_Settings;
const char *m_SysColorNames[]=
{
    "Black",
    "Red",
    "Green",
    "Yellow",
    "Blue",
    "Magenta",
    "Cyan",
    "White"
};

/*******************************************************************************
 * NAME:
 *    LoadSettings
 *
 * SYNOPSIS:
 *    bool LoadSettings(const char *Filename=NULL);
 *
 * PARAMETERS:
 *    Filename [I] -- The file name to load settings from.  If this is
 *                    NULL then the default file / path will be used.
 *
 * FUNCTION:
 *    This function loads the settings from a file into the global settings
 *    'g_Settings'.  It defaults them first so if there is an error or missing
 *    settings everything will be defaulted.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    SaveSettings()
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 ******************************************************************************/
bool LoadSettings(const char *Filename)
{
    class TinyCFG cfg("Settings");
    const char *UseFilename;
    string Path;
    string SettingsFilename;

    try
    {
        UseFilename=Filename;
        if(Filename==NULL)
        {
            if(!g_Settings.GetDefaultSettingsPathAndName(Path,SettingsFilename))
                return false;

            /* See if this path exists */
            if(!PathExists(Path.c_str()))
            {
                /* Try making it */
                if(!MakePathDir(Path.c_str()))
                    return false;
            }

            SettingsFilename=Path;
            SettingsFilename+=SETTINGS_FILE;
            UseFilename=SettingsFilename.c_str();
        }

        g_Settings.RegisterAllMembers(cfg);

        g_Settings.DefaultSettings();;
        cfg.LoadCFGFile(UseFilename);

/* DEBUG PAUL: We need to ask each plugin to load it's settings data here */
    }
    catch(...)
    {
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    SaveSettings
 *
 * SYNOPSIS:
 *    bool SaveSettings(const char *Filename=NULL);
 *
 * PARAMETERS:
 *    Filename [I] -- The file name to save the settings under.  If this is
 *                    NULL then the default file / path will be used.
 *
 * FUNCTION:
 *    This function saves the global settings to a file.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    LoadSettings()
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 ******************************************************************************/
bool SaveSettings(const char *Filename)
{
    class TinyCFG cfg("Settings");
    const char *UseFilename;
    string Path;
    string SettingsFilename;

    try
    {
        UseFilename=Filename;
        if(Filename==NULL)
        {
            if(!g_Settings.GetDefaultSettingsPathAndName(Path,SettingsFilename))
                return false;

            /* See if this path exists */
            if(!PathExists(Path.c_str()))
            {
                /* Try making it */
                if(!MakePathDir(Path.c_str()))
                    return false;
            }

            SettingsFilename=Path;
            SettingsFilename+=SETTINGS_FILE;
            UseFilename=SettingsFilename.c_str();
        }

        g_Settings.RegisterAllMembers(cfg);

/* DEBUG PAUL: We need to ask each plugin to save it's settings data here */

        cfg.SaveCFGFile(UseFilename);
    }
    catch(...)
    {
        return false;
    }
    return true;
}

Settings::Settings()
{
    DefaultSettings();
}

//ConnectionSettings::ConnectionSettings()
//{
//    DefaultSettings();
//}

///////////////////
bool DataProcessorCFG::LoadData(string &LoadedString)
{
    *Ptr=e_DataProcessorType_Text;

    if(strcmp(LoadedString.c_str(),"Text")==0)
        *Ptr=e_DataProcessorType_Text;
    if(strcmp(LoadedString.c_str(),"Binary")==0)
        *Ptr=e_DataProcessorType_Binary;

    return true;
}

bool DataProcessorCFG::SaveData(string &StoreString)
{
    switch(*Ptr)
    {
        default:
        case e_DataProcessorTypeMAX:
        case e_DataProcessorType_Text:
            StoreString="Text";
        break;
        case e_DataProcessorType_Binary:
            StoreString="Binary";
        break;
    }
    return true;
}

bool Settings_RegisterDataProcessorType(class TinyCFG &cfg,const char *XmlName,
      e_DataProcessorTypeType &Data)
{
    class DataProcessorCFG *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new DataProcessorCFG;
    }
    catch(std::bad_alloc const &)
    {
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=&Data;
    NewDataClass->XmlName=XmlName;

    return cfg.RegisterGeneric(NewDataClass);
}
///////////////////
bool WindowStartupPosCFG::LoadData(string &LoadedString)
{
    *Ptr=e_WindowStartupPos_OSDefault;  /* Defaults to white */

    if(strcmp(LoadedString.c_str(),"OSDefault")==0)
        *Ptr=e_WindowStartupPos_OSDefault;
    if(strcmp(LoadedString.c_str(),"RestoreFromSession")==0)
        *Ptr=e_WindowStartupPos_RestoreFromSession;
    if(strcmp(LoadedString.c_str(),"RestoreFromSettings")==0)
        *Ptr=e_WindowStartupPos_RestoreFromSettings;

    return true;
}

bool WindowStartupPosCFG::SaveData(string &StoreString)
{
    switch(*Ptr)
    {
        default:
        case e_WindowStartupPosMAX:
        case e_WindowStartupPos_OSDefault:
            StoreString="OSDefault";
        break;
        case e_WindowStartupPos_RestoreFromSession:
            StoreString="RestoreFromSession";
        break;
        case e_WindowStartupPos_RestoreFromSettings:
            StoreString="RestoreFromSettings";
        break;
    }
    return true;
}

bool Settings::RegisterWindowStartupPos(class TinyCFG &cfg,const char *XmlName,
      e_WindowStartupPosType &Data)
{
    class WindowStartupPosCFG *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new WindowStartupPosCFG;
    }
    catch(std::bad_alloc const &)
    {
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=&Data;
    NewDataClass->XmlName=XmlName;

    return cfg.RegisterGeneric(NewDataClass);
}
////////////////////
bool SysColorCFG::LoadElement(class TinyCFG *CFG)
{
    int col;
    const char *ColorStr;

    for(col=0;col<e_SysColMAX;col++)
    {
        ColorStr=CFG->ReadDataElement(m_SysColorNames[col]);
        if(ColorStr!=NULL)
            (*Ptr)[col]=strtol(ColorStr,NULL,16);
    }

    return true;
}

bool SysColorCFG::SaveElement(class TinyCFG *CFG)
{
    int col;
    char buff[100];

    for(col=0;col<e_SysColMAX;col++)
    {
        sprintf(buff,"%06X",(*Ptr)[col]);
        CFG->WriteDataElement(m_SysColorNames[col],buff);
    }

    return true;
}

bool Settings_RegisterSysColorType(class TinyCFG &cfg,
        const char *XmlName,uint32_t (*Data)[e_SysColMAX])
{
    class SysColorCFG *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new SysColorCFG;
    }
    catch(std::bad_alloc const &)
    {
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=Data;
    NewDataClass->XmlName=XmlName;

    return cfg.RegisterGeneric(NewDataClass);
}
////////////////////
bool DefaultColorCFG::LoadData(string &LoadedString)
{
    *Ptr=strtol(LoadedString.c_str(),NULL,16);

    return true;
}

bool DefaultColorCFG::SaveData(string &StoreString)
{
    char buff[100];
    sprintf(buff,"%06X",*Ptr);
    StoreString=buff;
    return true;
}

bool Settings_RegisterDefaultColorType(class TinyCFG &cfg,const char *XmlName,
      uint32_t &Data)
{
    class DefaultColorCFG *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new DefaultColorCFG;
    }
    catch(std::bad_alloc const &)
    {
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=&Data;
    NewDataClass->XmlName=XmlName;

    return cfg.RegisterGeneric(NewDataClass);
}
///////////////////
class ScreenClearCFG : public TinyCFGBaseData
{
   public:
      e_ScreenClearType *Ptr;
      bool LoadData(string &LoadedString);
      bool SaveData(string &StoreString);
};
bool ScreenClearCFG::LoadData(string &LoadedString)
{
    *Ptr=e_ScreenClear_Clear;  /* Defaults to clear */

    if(strcmp(LoadedString.c_str(),"Clear")==0)
        *Ptr=e_ScreenClear_Clear;
    if(strcmp(LoadedString.c_str(),"Scroll")==0)
        *Ptr=e_ScreenClear_Scroll;
    if(strcmp(LoadedString.c_str(),"ScrollAll")==0)
        *Ptr=e_ScreenClear_ScrollAll;
    if(strcmp(LoadedString.c_str(),"ScrollWithHR")==0)
        *Ptr=e_ScreenClear_ScrollWithHR;

    return true;
}

bool ScreenClearCFG::SaveData(string &StoreString)
{
    switch(*Ptr)
    {
        default:
        case e_ScreenClearMAX:
        case e_ScreenClear_Clear:
            StoreString="Clear";
        break;
        case e_ScreenClear_Scroll:
            StoreString="Scroll";
        break;
        case e_ScreenClear_ScrollAll:
            StoreString="ScrollAll";
        break;
        case e_ScreenClear_ScrollWithHR:
            StoreString="ScrollWithHR";
        break;
    }
    return true;
}

bool Settings::RegisterScreenClear(class TinyCFG &cfg,const char *XmlName,
      e_ScreenClearType &Data)
{
    class ScreenClearCFG *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new ScreenClearCFG;
    }
    catch(std::bad_alloc const &)
    {
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=&Data;
    NewDataClass->XmlName=XmlName;

    return cfg.RegisterGeneric(NewDataClass);
}
////////////////////
bool Settings::GetDefaultSettingsPathAndName(std::string &Path,
        std::string &Filename)
{
    try
    {
        if(GetAppDataPath(Path)==false)
            return false;

        Filename=SETTINGS_FILE;
    }
    catch(...)
    {
        return false;
    }
    return true;
}
////////////////////
bool KeyCommandsCFG::LoadElement(class TinyCFG *CFG)
{
    int cmd;
    const char *KeyData;

    for(cmd=0;cmd<e_CmdMAX;cmd++)
    {
        KeyData=CFG->ReadDataElement(GetCmdName((e_CmdType)cmd));
        if(KeyData!=NULL)
            ConvertString2KeySeq(&(*Ptr)[cmd],KeyData);
    }

    return true;
}

bool KeyCommandsCFG::SaveElement(class TinyCFG *CFG)
{
    int cmd;
    char buff[100];

    for(cmd=0;cmd<e_CmdMAX;cmd++)
    {
        strcpy(buff,ConvertKeySeq2String(&(*Ptr)[cmd]));
        CFG->WriteDataElement(GetCmdName((e_CmdType)cmd),buff);
    }

    return true;
}

bool Settings::RegisterKeyCommandType(class TinyCFG &cfg,const char *XmlName,
        struct CommandKeySeq (*Data)[e_CmdMAX])
{
    class KeyCommandsCFG *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new KeyCommandsCFG;
    }
    catch(std::bad_alloc const &)
    {
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=Data;
    NewDataClass->XmlName=XmlName;

    return cfg.RegisterGeneric(NewDataClass);
}
///////////////////
class BackspaceKeyCFG : public TinyCFGBaseData
{
   public:
      e_BackspaceKeyType *Ptr;
      bool LoadData(string &LoadedString);
      bool SaveData(string &StoreString);
};
bool BackspaceKeyCFG::LoadData(string &LoadedString)
{
    *Ptr=e_BackspaceKey_BS;  /* Defaults to BS */

    if(strcmp(LoadedString.c_str(),"BS")==0)
        *Ptr=e_BackspaceKey_BS;
    if(strcmp(LoadedString.c_str(),"DEL")==0)
        *Ptr=e_BackspaceKey_DEL;

    return true;
}

bool BackspaceKeyCFG::SaveData(string &StoreString)
{
    switch(*Ptr)
    {
        default:
        case e_BackspaceKeyMAX:
        case e_BackspaceKey_BS:
            StoreString="BS";
        break;
        case e_BackspaceKey_DEL:
            StoreString="DEL";
        break;
    }
    return true;
}

bool ConSettings::RegisterBackspaceKey(class TinyCFG &cfg,const char *XmlName,
      e_BackspaceKeyType &Data)
{
    class BackspaceKeyCFG *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new BackspaceKeyCFG;
    }
    catch(std::bad_alloc const &)
    {
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=&Data;
    NewDataClass->XmlName=XmlName;

    return cfg.RegisterGeneric(NewDataClass);
}
///////////////////
class EnterKeyCFG : public TinyCFGBaseData
{
   public:
      e_EnterKeyType *Ptr;
      bool LoadData(string &LoadedString);
      bool SaveData(string &StoreString);
};
bool EnterKeyCFG::LoadData(string &LoadedString)
{
    *Ptr=e_EnterKey_LF;  /* Defaults to LF */

    if(strcmp(LoadedString.c_str(),"LF")==0)
        *Ptr=e_EnterKey_LF;
    if(strcmp(LoadedString.c_str(),"CR")==0)
        *Ptr=e_EnterKey_CR;
    if(strcmp(LoadedString.c_str(),"CRLF")==0)
        *Ptr=e_EnterKey_CRLF;

    return true;
}

bool EnterKeyCFG::SaveData(string &StoreString)
{
    switch(*Ptr)
    {
        default:
        case e_EnterKeyMAX:
        case e_EnterKey_LF:
            StoreString="LF";
        break;
        case e_EnterKey_CR:
            StoreString="CR";
        break;
        case e_EnterKey_CRLF:
            StoreString="CRLF";
        break;
    }
    return true;
}

bool ConSettings::RegisterEnterKey(class TinyCFG &cfg,const char *XmlName,
      e_EnterKeyType &Data)
{
    class EnterKeyCFG *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new EnterKeyCFG;
    }
    catch(std::bad_alloc const &)
    {
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=&Data;
    NewDataClass->XmlName=XmlName;

    return cfg.RegisterGeneric(NewDataClass);
}
///////////////////
class ClipboardModeCFG : public TinyCFGBaseData
{
   public:
      e_ClipboardModeType *Ptr;
      bool LoadData(string &LoadedString);
      bool SaveData(string &StoreString);
};
bool ClipboardModeCFG::LoadData(string &LoadedString)
{
    *Ptr=e_ClipboardMode_Smart;  /* Defaults to smart */

    if(strcmp(LoadedString.c_str(),"None")==0)
        *Ptr=e_ClipboardMode_None;
    if(strcmp(LoadedString.c_str(),"Normal")==0)
        *Ptr=e_ClipboardMode_Normal;
    if(strcmp(LoadedString.c_str(),"SHIFTCTRL")==0)
        *Ptr=e_ClipboardMode_ShiftCtrl;
    if(strcmp(LoadedString.c_str(),"ALT")==0)
        *Ptr=e_ClipboardMode_Alt;
    if(strcmp(LoadedString.c_str(),"Smart")==0)
        *Ptr=e_ClipboardMode_Smart;

    return true;
}

bool ClipboardModeCFG::SaveData(string &StoreString)
{
    switch(*Ptr)
    {
        default:
        case e_ClipboardModeMAX:
        case e_ClipboardMode_Smart:
            StoreString="Smart";
        break;
        case e_ClipboardMode_None:
            StoreString="None";
        break;
        case e_ClipboardMode_Normal:
            StoreString="Normal";
        break;
        case e_ClipboardMode_ShiftCtrl:
            StoreString="SHIFTCTRL";
        break;
        case e_ClipboardMode_Alt:
            StoreString="ALT";
        break;
    }
    return true;
}

bool ConSettings::RegisterClipboardMode(class TinyCFG &cfg,const char *XmlName,
      e_ClipboardModeType &Data)
{
    class ClipboardModeCFG *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new ClipboardModeCFG;
    }
    catch(std::bad_alloc const &)
    {
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=&Data;
    NewDataClass->XmlName=XmlName;

    return cfg.RegisterGeneric(NewDataClass);
}
////////////////////
class BeepCFG : public TinyCFGBaseData
{
   public:
      e_BeepType *Ptr;
      bool LoadData(string &LoadedString);
      bool SaveData(string &StoreString);
};
bool BeepCFG::LoadData(string &LoadedString)
{
    *Ptr=e_Beep_System;  /* Defaults to system */

    if(strcmp(LoadedString.c_str(),"None")==0)
        *Ptr=e_Beep_None;
    if(strcmp(LoadedString.c_str(),"System")==0)
        *Ptr=e_Beep_System;
    if(strcmp(LoadedString.c_str(),"BuiltIn")==0)
        *Ptr=e_Beep_BuiltIn;
    if(strcmp(LoadedString.c_str(),"AudioOnly")==0)
        *Ptr=e_Beep_AudioOnly;
    if(strcmp(LoadedString.c_str(),"VisualOnly")==0)
        *Ptr=e_Beep_VisualOnly;

    return true;
}

bool BeepCFG::SaveData(string &StoreString)
{
    switch(*Ptr)
    {
        default:
        case e_BeepMAX:
        case e_Beep_System:
            StoreString="System";
        break;
        case e_Beep_None:
            StoreString="None";
        break;
        case e_Beep_BuiltIn:
            StoreString="BuiltIn";
        break;
        case e_Beep_AudioOnly:
            StoreString="AudioOnly";
        break;
        case e_Beep_VisualOnly:
            StoreString="VisualOnly";
        break;
    }
    return true;
}

bool ConSettings::RegisterBeep(class TinyCFG &cfg,const char *XmlName,
      e_BeepType &Data)
{
    class BeepCFG *NewDataClass;

    /* Make a new class to handle this new piece of data */
    try
    {
        NewDataClass=new BeepCFG;
    }
    catch(std::bad_alloc const &)
    {
        return false;
    }

    /* Setup the data */
    NewDataClass->Ptr=&Data;
    NewDataClass->XmlName=XmlName;

    return cfg.RegisterGeneric(NewDataClass);
}
///////////////////

/*******************************************************************************
 * NAME:
 *    ApplySettings
 *
 * SYNOPSIS:
 *    void ApplySettings(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function applies (or reapplies) the settings to the system.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 ******************************************************************************/
void ApplySettings(void)
{
    MW_ApplySettings();

    /* Apply to all connections */
    Con_ApplySettings2AllConnections();
}

void Settings::RegisterAllMembers(class TinyCFG &cfg)
{
    cfg.StartBlock("MainWindow");
        RegisterWindowStartupPos(cfg,"RestoreWindowPos",RestoreWindowPos);
        cfg.Register("LeftPanelFromSettings",LeftPanelFromSettings);
        cfg.Register("RightPanelFromSettings",RightPanelFromSettings);
        cfg.Register("BottomPanelFromSettings",BottomPanelFromSettings);
        cfg.Register("LeftPanelAutoHide",LeftPanelAutoHide);
        cfg.Register("RightPanelAutoHide",RightPanelAutoHide);
        cfg.Register("BottomPanelAutoHide",BottomPanelAutoHide);
        cfg.Register("LeftPanelSize",LeftPanelSize);
        cfg.Register("RightPanelSize",RightPanelSize);
        cfg.Register("BottomPanelSize",BottomPanelSize);
        cfg.Register("LeftPanelOpenOnStartup",LeftPanelOpenOnStartup);
        cfg.Register("RightPanelOpenOnStartup",RightPanelOpenOnStartup);
        cfg.Register("BottomPanelOpenOnStartup",BottomPanelOpenOnStartup);
        cfg.Register("AppMaximized",AppMaximized);
        cfg.Register("WindowPosX",WindowPosX);
        cfg.Register("WindowPosY",WindowPosY);
        cfg.Register("WindowWidth",WindowWidth);
        cfg.Register("WindowHeight",WindowHeight);

        cfg.StartBlock("Panels");
            cfg.StartBlock("StopWatch");
                cfg.Register("StopWatchAutoLap",StopWatchAutoLap);
                cfg.Register("StopWatchAutoStart",StopWatchAutoStart);
                cfg.Register("StopWatchShowPanel",StopWatchShowPanel);
            cfg.EndBlock();
            cfg.StartBlock("Capture");
                cfg.Register("CaptureTimestamp",CaptureTimestamp);
                cfg.Register("CaptureAppend",CaptureAppend);
                cfg.Register("CaptureStripCtrl",CaptureStripCtrl);
                cfg.Register("CaptureStripEsc",CaptureStripEsc);
                cfg.Register("CaptureHexDump",CaptureHexDump);
                cfg.Register("CaptureDefaultFilename",CaptureDefaultFilename);
                cfg.Register("CaptureShowPanel",CaptureShowPanel);
            cfg.EndBlock();
            cfg.StartBlock("HexDisplay");
                cfg.Register("HexDisplayEnabled",HexDisplayEnabled);
                cfg.Register("HexDisplayBufferSize",HexDisplayBufferSize);
            cfg.EndBlock();
        cfg.EndBlock();
    cfg.EndBlock();

    cfg.StartBlock("Display");
        cfg.Register("AlwaysShowTabs",AlwaysShowTabs);
        cfg.Register("CloseButtonOnTabs",CloseButtonOnTabs);
        cfg.Register("MouseCursorIBeam",MouseCursorIBeam);
        cfg.StartBlock("HexDisplays");
            cfg.Register("HexDisplaysFGColor",HexDisplaysFGColor);
            cfg.Register("HexDisplaysBGColor",HexDisplaysBGColor);
            cfg.Register("HexDisplaysSelBGColor",HexDisplaysSelBGColor);
            cfg.Register("HexDisplaysFontName",HexDisplaysFontName);
            cfg.Register("HexDisplaysFontSize",HexDisplaysFontSize);
            cfg.Register("HexDisplaysFontBold",HexDisplaysFontBold);
            cfg.Register("HexDisplaysFontItalic",HexDisplaysFontItalic);
        cfg.EndBlock();
    cfg.EndBlock();

    cfg.StartBlock("Connections");
        cfg.Register("AutoConnectOnNewConnection",AutoConnectOnNewConnection);
    cfg.EndBlock();

    cfg.StartBlock("Behaviour");
        cfg.Register("BookmarksOpenNewTabs",BookmarksOpenNewTabs);
    cfg.EndBlock();

    RegisterKeyCommandType(cfg,"KeyBindings",&KeyMapping);
    cfg.Register("DotInputStartsAt0",DotInputStartsAt0);

    cfg.StartBlock("ConnectionDefaults");
        DefaultConSettings.RegisterAllMembers(cfg);
    cfg.EndBlock();

    cfg.StartBlock("Terminal");
        RegisterScreenClear(cfg,"ScreenClear",ScreenClear);
    cfg.EndBlock();
}

void ConSettings::RegisterAllMembers(class TinyCFG &cfg)
{
    cfg.Register("CursorColor",CursorColor,true);
    cfg.Register("CursorBlink",CursorBlink);
    cfg.Register("FontName",FontName);
    cfg.Register("FontSize",FontSize);
    cfg.Register("FontBold",FontBold);
    cfg.Register("FontItalic",FontItalic);

    Settings_RegisterSysColorType(cfg,"ColorsNormal",&SysColors[e_SysColShade_Normal]);
    Settings_RegisterSysColorType(cfg,"ColorsBright",&SysColors[e_SysColShade_Bright]);
    Settings_RegisterSysColorType(cfg,"ColorsDark",&SysColors[e_SysColShade_Dark]);
    Settings_RegisterDefaultColorType(cfg,"DefaultBackgroundColor",DefaultColors[e_DefaultColors_BG]);
    Settings_RegisterDefaultColorType(cfg,"DefaultForgroundColor",DefaultColors[e_DefaultColors_FG]);

    cfg.Register("FixedWidth",TermSizeFixedWidth);
    cfg.Register("FixedHeight",TermSizeFixedHeight);
    cfg.Register("Width",TermSizeWidth);
    cfg.Register("Height",TermSizeHeight);
    cfg.Register("ScrollBufferLines",ScrollBufferLines);
    cfg.Register("CenterTextInWindow",CenterTextInWindow);

    cfg.StartBlock("DataProcessors");
    Settings_RegisterDataProcessorType(cfg,"DataProcessorType",DataProcessorType);
    cfg.Register("EnabledTextDataProcessors",EnabledTextDataProcessors);
    cfg.Register("EnabledTermEmuDataProcessors",EnabledTermEmuDataProcessors);
    cfg.Register("EnabledBinaryDataProcessors",EnabledBinaryDataProcessors);
    cfg.EndBlock();

    cfg.StartBlock("KeyPressProcessors");
    cfg.Register("EnabledKeyPressProcessors",EnabledKeyPressProcessors);
    cfg.EndBlock();

    cfg.StartBlock("Keyboard");
        RegisterBackspaceKey(cfg,"BackspaceKey",BackspaceKeyMode);
        RegisterEnterKey(cfg,"EnterKey",EnterKeyMode);
        RegisterClipboardMode(cfg,"ClipboardMode",ClipboardMode);
    cfg.EndBlock();

    cfg.StartBlock("AttribEnable");
    cfg.Register("BoldEnabled",BoldEnabled);
    cfg.Register("ItalicEnabled",ItalicEnabled);
    cfg.Register("UnderlineEnabled",UnderlineEnabled);
    cfg.Register("OverlineEnabled",OverlineEnabled);
    cfg.Register("ReverseEnabled",ReverseEnabled);
    cfg.Register("LineThroughEnabled",LineThroughEnabled);
    cfg.Register("ColorsEnabled",ColorsEnabled);
    cfg.EndBlock();

    cfg.StartBlock("Sound");
        RegisterBeep(cfg,"Beep",BeepMode);
        cfg.Register("UseCustomSound",UseCustomSound);
        cfg.Register("BeepSound",BeepFilename);
    cfg.EndBlock();
}

bool AreConSettingsEqual(class ConSettings &Con1,class ConSettings &Con2)
{
    if(Con1.CursorColor!=Con2.CursorColor)
        return false;
    if(Con1.CursorBlink!=Con2.CursorBlink)
        return false;

    if(Con1.FontName!=Con2.FontName)
        return false;
    if(Con1.FontSize!=Con2.FontSize)
        return false;
    if(Con1.FontBold!=Con2.FontBold)
        return false;
    if(Con1.FontItalic!=Con2.FontItalic)
        return false;

    if(memcmp(Con1.SysColors,Con2.SysColors,sizeof(Con1.SysColors))!=0)
        return false;
    if(memcmp(Con1.DefaultColors,Con2.DefaultColors,
            sizeof(Con1.DefaultColors))!=0)
    {
        return false;
    }

    if(Con1.TermSizeFixedWidth!=Con2.TermSizeFixedWidth)
        return false;
    if(Con1.TermSizeFixedHeight!=Con2.TermSizeFixedHeight)
        return false;
    if(Con1.TermSizeWidth!=Con2.TermSizeWidth)
        return false;
    if(Con1.TermSizeHeight!=Con2.TermSizeHeight)
        return false;
    if(Con1.ScrollBufferLines!=Con2.ScrollBufferLines)
        return false;
    if(Con1.CenterTextInWindow!=Con2.CenterTextInWindow)
        return false;
    if(Con1.DataProcessorType!=Con2.DataProcessorType)
        return false;
    if(Con1.BackspaceKeyMode!=Con2.BackspaceKeyMode)
        return false;
    if(Con1.EnterKeyMode!=Con2.EnterKeyMode)
        return false;
    if(Con1.ClipboardMode!=Con2.ClipboardMode)
        return false;

    if(Con1.EnabledTextDataProcessors!=Con2.EnabledTextDataProcessors)
        return false;
    if(Con1.EnabledTermEmuDataProcessors!=Con2.EnabledTermEmuDataProcessors)
        return false;
    if(Con1.EnabledKeyPressProcessors!=Con2.EnabledKeyPressProcessors)
        return false;
    if(Con1.EnabledBinaryDataProcessors!=Con2.EnabledBinaryDataProcessors)
        return false;

    if(Con1.BoldEnabled!=Con2.BoldEnabled)
        return false;
    if(Con1.ItalicEnabled!=Con2.ItalicEnabled)
        return false;
    if(Con1.UnderlineEnabled!=Con2.UnderlineEnabled)
        return false;
    if(Con1.OverlineEnabled!=Con2.OverlineEnabled)
        return false;
    if(Con1.ReverseEnabled!=Con2.ReverseEnabled)
        return false;
    if(Con1.LineThroughEnabled!=Con2.LineThroughEnabled)
        return false;
    if(Con1.ColorsEnabled!=Con2.ColorsEnabled)
        return false;

    return true;
}

void Settings::DefaultSettings(void)
{
    RestoreWindowPos=e_WindowStartupPos_OSDefault;
    LeftPanelFromSettings=false;
    RightPanelFromSettings=false;
    BottomPanelFromSettings=false;
    LeftPanelAutoHide=false;
    RightPanelAutoHide=false;
    BottomPanelAutoHide=false;

    LeftPanelSize=80;
    RightPanelSize=80;
    BottomPanelSize=80;
    LeftPanelOpenOnStartup=false;
    RightPanelOpenOnStartup=false;
    BottomPanelOpenOnStartup=false;
    AppMaximized=true;
    WindowPosX=-1;
    WindowPosY=-1;
    WindowWidth=640;
    WindowHeight=480;
    CloseButtonOnTabs=false;
    MouseCursorIBeam=true;

//            ScrollBackLines=20;
    AlwaysShowTabs=true;
    AutoConnectOnNewConnection=true;

    DefaultCmdKeyMapping(KeyMapping);
    DotInputStartsAt0=false;

    /**** Behaviour ****/
    BookmarksOpenNewTabs=true;

    /**** Panels ****/
    /* Stop watch */
    StopWatchAutoLap=false;
    StopWatchAutoStart=false;
    StopWatchShowPanel=true;

    /* Capture */
    CaptureTimestamp=false;
    CaptureAppend=false;
    CaptureStripCtrl=true;
    CaptureStripEsc=true;
    CaptureHexDump=false;
    CaptureDefaultFilename="";
    CaptureShowPanel=false;

    /* Hex Display */
    HexDisplayEnabled=true;
    HexDisplayBufferSize=100000;
    HexDisplaysFGColor=0xFFFFFF;
    HexDisplaysBGColor=0x000000;
    HexDisplaysSelBGColor=0x5d81a9;
    UI_GetDefaultFixedWidthFont(HexDisplaysFontName);
    HexDisplaysFontSize=12;
    HexDisplaysFontBold=false;
    HexDisplaysFontItalic=false;

    DefaultConSettings.DefaultSettings();
}

void ConSettings::DefaultSettings(void)
{
    CursorColor=0xFFFB00;
    CursorBlink=true;

    UI_GetDefaultFixedWidthFont(FontName);
    FontSize=12;
    FontBold=false;
    FontItalic=false;
    GetPresetSysColors(e_SysColPreset_WhippyTerm,SysColors);
    DefaultColors[e_DefaultColors_BG]=SysColors[e_SysColShade_Normal][e_SysCol_Black];
    DefaultColors[e_DefaultColors_FG]=SysColors[e_SysColShade_Normal][e_SysCol_White];

    TermSizeFixedWidth=false;
    TermSizeFixedHeight=false;
    TermSizeWidth=80;
    TermSizeHeight=25;
    ScrollBufferLines=10000;
    CenterTextInWindow=true;

    BackspaceKeyMode=e_BackspaceKey_BS;
    EnterKeyMode=e_EnterKey_LF;
    ClipboardMode=e_ClipboardMode_Smart;

    EnabledTextDataProcessors.clear();
    EnabledTextDataProcessors.push_back("UnicodeDecoder");

    DataProcessorType=e_DataProcessorType_Text;
    EnabledTermEmuDataProcessors.clear();
    EnabledTermEmuDataProcessors.push_back("ANSIX364Decoder");

    EnabledKeyPressProcessors.clear();

    EnabledBinaryDataProcessors.clear();

    BoldEnabled=true;
    ItalicEnabled=true;
    UnderlineEnabled=true;
    OverlineEnabled=true;
    ReverseEnabled=true;
    LineThroughEnabled=true;
    ColorsEnabled=true;

    BeepMode=e_Beep_System;
    UseCustomSound=false;
    BeepFilename="";
}
