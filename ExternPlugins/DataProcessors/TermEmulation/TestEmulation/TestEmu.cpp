/*******************************************************************************
 * FILENAME: TestEmu.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
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
#include "TestEmu.h"
#include "PluginSDK/Plugin.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*** DEFINES                  ***/
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      TestEmu             // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x01010000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
const struct DataProcessorInfo *TestEmu_GetProcessorInfo(unsigned int *SizeOfInfo);
void TestEmu_ProcessByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed);
PG_BOOL TestEmu_ProcessKeyPress(t_DataProcessorHandleType *DataHandle,
        const uint8_t *KeyChar,int KeyCharLen,e_UIKeys ExtendedKey,
        uint8_t Mod);
t_DataProSettingsWidgetsType *TestEmu_AllocSettingsWidgets(t_WidgetSysHandle *WidgetHandle,t_PIKVList *Settings);
void TestEmu_FreeSettingsWidgets(t_DataProSettingsWidgetsType *PrivData);
void TestEmu_StoreSettings(t_DataProSettingsWidgetsType *PrivData,t_PIKVList *Settings);

/*** VARIABLE DEFINITIONS     ***/
struct DataProcessorAPI m_TestEmuCBs=
{
    /* V1 */
    NULL,
    NULL,
    TestEmu_GetProcessorInfo,
    TestEmu_ProcessKeyPress,
    TestEmu_ProcessByte,
    NULL, // ProcessIncomingBinaryByte
    /* V2 */
    NULL,                                       // ProcessOutGoingData
    TestEmu_AllocSettingsWidgets,
    TestEmu_FreeSettingsWidgets,
    TestEmu_StoreSettings,
};

struct DataProcessorInfo m_TestEmu_Info=
{
    "Test Emulation",
    "A test emulation for testing WhippyTerm emulations",
    "Used for testing emulation in WhippyTerm",
    e_DataProcessorType_Text,
    e_TextDataProcessorClass_TermEmulation,
    e_BinaryDataProcessorModeMAX,
};

static const struct PI_UIAPI *m_TestChars_UIAPI;
static const struct PI_SystemAPI *m_TestChars_SysAPI;
static const struct DPS_API *m_DPS;

/*******************************************************************************
 * NAME:
 *    TestEmu_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int TestEmu_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
 *          unsigned int Version);
 *
 * PARAMETERS:
 *    SysAPI [I] -- The main API to WhippyTerm
 *    Version [I] -- What version of WhippyTerm is running.  This is used
 *                   to make sure we are compatible.  This is in the
 *                   Major<<24 | Minor<<16 | Rev<<8 | Patch
 *
 * FUNCTION:
 *    This function registers this plugin with the system.
 *
 * RETURNS:
 *    0 if we support this version of WhippyTerm, and the minimum version
 *    we need if we are not.
 *
 * NOTES:
 *    This function is normally is called from the RegisterPlugin() when
 *    it is being used as a normal plugin.  As a std plugin it is called
 *    from RegisterStdPlugins() instead.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
/* This needs to be extern "C" because it is the main entry point for the
   plugin system */
extern "C"
{
    unsigned int REGISTER_PLUGIN_FUNCTION(const struct PI_SystemAPI *SysAPI,
            unsigned int Version)
    {
        if(Version<NEEDED_MIN_API_VERSION)
            return NEEDED_MIN_API_VERSION;

        m_TestChars_SysAPI=SysAPI;
        m_DPS=SysAPI->GetAPI_DataProcessors();
        m_TestChars_UIAPI=m_DPS->GetAPI_UI();

        /* If we are have the correct experimental API */
        if(m_TestChars_SysAPI->GetExperimentalID()>0 &&
                m_TestChars_SysAPI->GetExperimentalID()<1)
        {
            return 0xFFFFFFFF;
        }

        m_DPS->RegisterDataProcessor("TestEmu",
                &m_TestEmuCBs,sizeof(m_TestEmuCBs));

        return 0;
    }
}

/*******************************************************************************
 * NAME:
 *    TestEmu_GetProcessorInfo
 *
 * SYNOPSIS:
 *    const struct DataProcessorInfo *TestEmu_GetProcessorInfo(
 *              unsigned int *SizeOfInfo);
 *
 * PARAMETERS:
 *    SizeOfInfo [O] -- The size of 'struct DataProcessorInfo'.  This is used
 *                        for forward / backward compatibility.
 *
 * FUNCTION:
 *    This function gets info about the plugin.  'DataProcessorInfo' has
 *    the following fields:
 *          DisplayName -- The name we show the user
 *          Tip -- A tool tip (for when you hover the mouse over this plugin)
 *          Help -- A help string for this plugin.
 *          ProType -- The type of process.  Supported values:
 *                  e_DataProcessorType_Text -- This is a text processor.
 *                      This is the more clasic type of processor (like VT100).
 *                  e_DataProcessorType_Binary -- This is a binary processor.
 *                      These are processors for binary protocol.  This may
 *                      be something as simple as a hex dump.
 *          TxtClass -- This only applies to 'e_DataProcessorType_Text' type
 *              processors. This is what class of text processor is
 *              this.  Supported classes:
 *                      e_TextDataProcessorClass_Other -- This is a generic class
 *                          more than one of these processors can be active
 *                          at a time but no other requirements exist.
 *                      e_TextDataProcessorClass_CharEncoding -- This is a
 *                          class that converts the raw stream into some kind
 *                          of char encoding.  For example unicode is converted
 *                          from a number of bytes to chars in the system.
 *                      e_TextDataProcessorClass_TermEmulation -- This is a
 *                          type of terminal emulator.  An example of a
 *                          terminal emulator is VT100.
 *                      e_TextDataProcessorClass_Highlighter -- This is a processor
 *                          that highlights strings as they come in the input
 *                          stream.  For example a processor that underlines
 *                          URL's.
 *                      e_TextDataProcessorClass_Logger -- This is a processor
 *                          that saves the input.  It may save to a file or
 *                          send out a debugging service.  And example is
 *                          a processor that saves all the raw bytes to a file.
 * SEE ALSO:
 *    
 ******************************************************************************/
const struct DataProcessorInfo *TestEmu_GetProcessorInfo(
        unsigned int *SizeOfInfo)
{
    *SizeOfInfo=sizeof(struct DataProcessorInfo);
    return &m_TestEmu_Info;
}

/*******************************************************************************
 *  NAME:
 *    TestEmu_ProcessByte
 *
 *  SYNOPSIS:
 *    void TestEmu_ProcessByte(
 *              t_DataProcessorHandleType *DataHandle,const uint8_t RawByte,
 *              uint8_t *ProcessedChar,int *CharLen,PG_BOOL *Consumed);
 *
 *  PARAMETERS:
 *    DataHandle [I] -- The data handle to your internal data.
 *    RawByte [I] -- The raw byte to process.  This is the byte that came in.
 *    ProcessedChar [I/O] -- This is a unicode char that has already been
 *                         processed by some of the other input filters.  You
 *                         can change this as you need.  It must remain only
 *                         one unicode char.
 *    CharLen [I/O] -- This number of bytes in 'ProcessedChar'
 *    Consumed [I/O] -- This tells the system (and other filters) if the
 *                      char has been used up and will not be added to the
 *                      screen.
 *
 *  FUNCTION:
 *    This function is called for each byte that comes in.
 *
 *  RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void TestEmu_ProcessByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed)
{
}

/*******************************************************************************
 * NAME:
 *   TestEmu_ProcessKeyPress
 *
 * SYNOPSIS:
 *   PG_BOOL TestEmu_ProcessKeyPress(
 *          t_DataProcessorHandleType *DataHandle,const uint8_t *KeyChar,
 *          int KeyCharLen,e_UIKeys ExtendedKey,uint8_t Mod)
 *
 * PARAMETERS:
 *   DataHandle [I] -- The data handle to work on.  This is your internal
 *                     data.
 *   KeyChar [I] -- The key that was pressed.  In a UTF-8 string.  This is
 *                  a single char but maybe any bytes.  This len will be in
 *                  'KeyCharLen'.
 *   KeyCharLen [I] -- The number of bytes in 'KeyChar'.  If this is 0
 *                     then 'ExtendedKey' is used for what key was pressed.
 *   ExtendedKey [I] -- The non text key that was pressed.  This includes
 *                      things like the arrow keys, function keys, etc.
 *                      This is only used if 'KeyCharLen' is 0.
 *   Mod [I] -- What modifier keys where held when this key was pressed.
 *              This is a bit field (so more than one key can be held.
 *              Supported bits:
 *                   KEYMOD_SHIFT -- Shift key
 *                   KEYMOD_CONTROL -- Control hey
 *                   KEYMOD_ALT -- Alt key
 *                   KEYMOD_LOGO -- Logo key (Windows, Amiga, Command, etc)
 *
 * FUNCTION:
 *   This function is called for each key pressed that would normally be
 *   sent out.  This includes extended keys like the arrows.  This is only
 *   called for data processors that are
 *   e_InputProcessorClass_TermEmulation only.
 *
 * RETURNS:
 *   true -- This key was used up (do not pass to other key processors)
 *   false -- This key should continue on it's way though the key press
 *            processor list.
 ******************************************************************************/
PG_BOOL TestEmu_ProcessKeyPress(
        t_DataProcessorHandleType *DataHandle,const uint8_t *KeyChar,
        int KeyCharLen,e_UIKeys ExtendedKey,uint8_t Mod)
{
    return false;
}

struct TestEmuWidgetData
{
    t_WidgetSysHandle *FirstTabWidgetHandle;
    struct PI_Checkbox *TestCheckbox;

    t_WidgetSysHandle *TabWidgetHandle2;
    struct PI_Checkbox *TestCheckbox2;
};

void TheEventCB(const struct PICheckboxEvent *Event,void *UserData)
{
    struct TestEmuWidgetData *WData=(struct TestEmuWidgetData *)UserData;

    m_TestChars_UIAPI->SetCheckboxChecked(WData->TabWidgetHandle2,WData->TestCheckbox2->Ctrl,
            !m_TestChars_UIAPI->IsCheckboxChecked(WData->FirstTabWidgetHandle,WData->TestCheckbox->Ctrl));
}

t_DataProSettingsWidgetsType *TestEmu_AllocSettingsWidgets(t_WidgetSysHandle *WidgetHandle,t_PIKVList *Settings)
{
    struct TestEmuWidgetData *WData;
    const char *C1;
    const char *C2;

    m_DPS->SetCurrentSettingsTabName("THREE!");

    WData=new TestEmuWidgetData;
    WData->FirstTabWidgetHandle=WidgetHandle;

    WData->TestCheckbox=m_TestChars_UIAPI->AddCheckbox(WidgetHandle,"Test Checkbox 3",TheEventCB,(void *)WData);

    WData->TabWidgetHandle2=m_DPS->AddNewSettingsTab("FOUR!");
    WData->TestCheckbox2=m_TestChars_UIAPI->AddCheckbox(WData->TabWidgetHandle2,"Test Checkbox 4",NULL,NULL);

    C1=m_TestChars_SysAPI->KVGetItem(Settings,"Checkbox3");
    C2=m_TestChars_SysAPI->KVGetItem(Settings,"Checkbox4");

    if(C1==NULL)
        C1="0";
    if(C2==NULL)
        C2="0";

    m_TestChars_UIAPI->SetCheckboxChecked(WData->FirstTabWidgetHandle,WData->TestCheckbox->Ctrl,atoi(C1));
    m_TestChars_UIAPI->SetCheckboxChecked(WData->TabWidgetHandle2,WData->TestCheckbox2->Ctrl,atoi(C2));

    return (t_DataProSettingsWidgetsType *)WData;
}

void TestEmu_FreeSettingsWidgets(t_DataProSettingsWidgetsType *PrivData)
{
    struct TestEmuWidgetData *WData=(struct TestEmuWidgetData *)PrivData;

    m_TestChars_UIAPI->FreeCheckbox(WData->FirstTabWidgetHandle,WData->TestCheckbox);
    m_TestChars_UIAPI->FreeCheckbox(WData->TabWidgetHandle2,WData->TestCheckbox2);

    delete WData;
}

void TestEmu_StoreSettings(t_DataProSettingsWidgetsType *PrivData,t_PIKVList *Settings)
{
    struct TestEmuWidgetData *WData=(struct TestEmuWidgetData *)PrivData;
    bool C1;
    bool C2;
    char buff[100];

    C1=m_TestChars_UIAPI->IsCheckboxChecked(WData->FirstTabWidgetHandle,WData->TestCheckbox->Ctrl);
    C2=m_TestChars_UIAPI->IsCheckboxChecked(WData->TabWidgetHandle2,WData->TestCheckbox2->Ctrl);

    sprintf(buff,"%d",C1);
    m_TestChars_SysAPI->KVAddItem(Settings,"Checkbox3",buff);

    sprintf(buff,"%d",C2);
    m_TestChars_SysAPI->KVAddItem(Settings,"Checkbox4",buff);
}

