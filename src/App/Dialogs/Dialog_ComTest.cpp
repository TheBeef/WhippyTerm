/*******************************************************************************
 * FILENAME: Dialog_ComTest.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
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
 *    Paul Hutchinson (20 Sep 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
//#include "App/Session.h"
//#include "App/MainWindow.h"
//#include "App/Settings.h"
#include "App/Connections.h"
#include "App/IOSystem.h"
//#include "App/MaxSizes.h"
#include "App/Dialogs/Dialog_ComTest.h"
#include "UI/UIAsk.h"
#include "UI/UIComTest.h"
#include "UI/UISystem.h"
#include "UI/UITimers.h"
#include <string>
#include <string.h>
#include <inttypes.h>
#include <time.h>
//#include <map>
//#include <string.h>

using namespace std;

/*** DEFINES                  ***/
#define DCT_TEST_TIMEOUT_POLL_RATE          1000    // When running a test how often do we check for timeout (ms)
#define DCT_TEST_END_TIMEOUT                10.0    // 10 seconds between rx bytes == end of test (double)

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
typedef enum
{
    e_DCT_Con1,
    e_DCT_Con2,
    e_DCTMAX
} e_DCTType;

typedef enum
{
    e_DCT_Pattern_55,
    e_DCT_Pattern_AA,
    e_DCT_Pattern_FF,
    e_DCT_Pattern_00,
    e_DCT_Pattern_55AA,
    e_DCT_Pattern_ABC,
    e_DCT_Pattern_AscII,
    e_DCT_Pattern_Binary,
    e_DCT_Pattern_Random,
    e_DCT_PatternMAX
} e_DCT_PatternType;

typedef enum
{
    e_DCT_TestResult_NotRun,
    e_DCT_TestResult_Pass,
    e_DCT_TestResult_Fail,
    e_DCT_TestResultMAX
} e_DCT_TestResultType;

/*** FUNCTION PROTOTYPES      ***/
static void DCT_FillInConnectionPullDown(t_UIComboBoxCtrl *ConPullDown,
        e_DCTType Con,string *ReselectLabel);
static bool DCT_GetListOfConnections(void);
static void DCT_ChangeConnectionListSelectedEntry(struct ConnectionInfoList *Con,
        e_DCTType Connection);
static void DCT_GetSelectedConLabel(t_UIComboBoxCtrl *ConPullDown,
        string &SelectedLabel);
static void DCT_RethinkUI(void);
static void DCT_StartTest(void);
static class Connection *DCT_AllocConnection4Test(e_DCTType ConNumber);
static void DCT_StopTest(void);
static void DCT_ComTestUpdateCallback(class Connection *Con);
static void DCT_TestTimeoutCheck(uintptr_t UserData);

/*** VARIABLE DEFINITIONS     ***/
static struct ConnectionInfoList *m_DCT_Connections;
static t_ConnectionOptionsDataType *m_DCT_OptionsWidgets[e_DCTMAX];
static t_KVList m_DCT_ConnectionOptions[e_DCTMAX];
static bool m_DCT_DoingTest;
static class Connection *m_DCT_Connection1;
static class Connection *m_DCT_Connection2;
static uint8_t *m_DCT_PatternBuffer;
static struct UITimer *m_DCT_TestTimeoutCheckTimer;
static e_DCT_TestResultType m_DCT_TestResult;

/*******************************************************************************
 * NAME:
 *    RunComTestDialog
 *
 * SYNOPSIS:
 *    void RunComTestDialog(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function shows the com test dialog.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RunComTestDialog(void)
{
    bool AllocatedUI;
    t_UIComboBoxCtrl *ConPullDown1;
    t_UIComboBoxCtrl *ConPullDown2;
    t_UIComboBoxCtrl *Pattern;
    t_UINumberInput *PacketLength;
    t_UINumberInput *Packets;
    t_UIButtonCtrl *Stop;
    int r;

    for(r=0;r<e_DCTMAX;r++)
        m_DCT_OptionsWidgets[r]=NULL;
    AllocatedUI=false;
    m_DCT_Connections=NULL;
    m_DCT_PatternBuffer=NULL;
    m_DCT_TestTimeoutCheckTimer=NULL;
    try
    {
        m_DCT_DoingTest=false;
        m_DCT_TestResult=e_DCT_TestResult_NotRun;

        m_DCT_TestTimeoutCheckTimer=AllocUITimer();
        if(m_DCT_TestTimeoutCheckTimer==NULL)
            throw("Failed to allocate a timer for the test");

        SetupUITimer(m_DCT_TestTimeoutCheckTimer,DCT_TestTimeoutCheck,0,true);
        UITimerSetTimeout(m_DCT_TestTimeoutCheckTimer,DCT_TEST_TIMEOUT_POLL_RATE);

        DCT_GetListOfConnections();

        for(r=0;r<e_DCTMAX;r++)
            m_DCT_ConnectionOptions[r].clear();

        if(!UIAlloc_ComTest())
            throw("Failed to open window");

        Stop=UICT_GetButton(e_CT_Button_Stop);
        PacketLength=UICT_GetNumberInputHandle(e_CT_Number_PacketLength);
        Packets=UICT_GetNumberInputHandle(e_CT_Number_Packets);
        Pattern=UICT_GetComboBoxHandle(e_CT_Combox_Pattern);
        ConPullDown1=UICT_GetComboBoxHandle(e_CT_Combox_ConnectionList1);
        ConPullDown2=UICT_GetComboBoxHandle(e_CT_Combox_ConnectionList2);

        UIAddItem2ComboBox(Pattern,"Pattern 0x55",e_DCT_Pattern_55);
        UIAddItem2ComboBox(Pattern,"Pattern 0xAA",e_DCT_Pattern_AA);
        UIAddItem2ComboBox(Pattern,"Pattern 0xFF",e_DCT_Pattern_FF);
        UIAddItem2ComboBox(Pattern,"Pattern 0x00",e_DCT_Pattern_00);
        UIAddItem2ComboBox(Pattern,"Pattern 0x55,0xAA",e_DCT_Pattern_55AA);
        UIAddItem2ComboBox(Pattern,"ABC",e_DCT_Pattern_ABC);
        UIAddItem2ComboBox(Pattern,"AscII",e_DCT_Pattern_AscII);
        UIAddItem2ComboBox(Pattern,"Binary 0-255",e_DCT_Pattern_Binary);
        UIAddItem2ComboBox(Pattern,"Random",e_DCT_Pattern_Random);

        DCT_FillInConnectionPullDown(ConPullDown1,e_DCT_Con1,NULL);
        DCT_FillInConnectionPullDown(ConPullDown2,e_DCT_Con2,NULL);

        UISetNumberInputCtrlValue(PacketLength,100);
        UISetNumberInputCtrlValue(Packets,100);
        UIEnableButton(Stop,false);

        DCT_RethinkUI();

        if(UIShow_ComTest())
        {
//                IOS_StoreConnectionOptions(m_DCT_OptionsWidgets,
//                        DCTOptions->second.Options);
        }
    }
    catch(const char *Error)
    {
        UIAsk("Error opening com test",Error,e_AskBox_Error,e_AskBttns_Ok);
    }
    catch(...)
    {
        UIAsk("Error opening com test","Failed to open com test",
                e_AskBox_Error,e_AskBttns_Ok);
    }

    if(m_DCT_TestTimeoutCheckTimer!=NULL)
        FreeUITimer(m_DCT_TestTimeoutCheckTimer);

    for(r=0;r<e_DCTMAX;r++)
        if(m_DCT_OptionsWidgets[r]!=NULL)
            IOS_FreeConnectionOptions(m_DCT_OptionsWidgets[r]);
    if(m_DCT_Connections!=NULL)
        IOS_FreeListOfAvailableConnections(m_DCT_Connections);
    if(AllocatedUI)
        UIFree_ComTest();
    if(m_DCT_PatternBuffer!=NULL)
        free(m_DCT_PatternBuffer);
}

/*******************************************************************************
 * NAME:
 *    CT_Event
 *
 * SYNOPSIS:
 *    bool CT_Event(const struct CTEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event that got us here.
 *
 * FUNCTION:
 *    This function is called from the UI to tell us that something happend.
 *
 * RETURNS:
 *    true -- The event should act normally
 *    false -- There event should be canceled
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool CT_Event(const struct CTEvent *Event)
{
    struct ConnectionInfoList *Con;
    t_UIComboBoxCtrl *ConPullDown1;
    t_UIComboBoxCtrl *ConPullDown2;
    string SelectedCon1;
    string SelectedCon2;

    ConPullDown1=UICT_GetComboBoxHandle(e_CT_Combox_ConnectionList1);
    ConPullDown2=UICT_GetComboBoxHandle(e_CT_Combox_ConnectionList2);

    switch(Event->EventType)
    {
        case e_CTEvent_BttnTriggered:
            switch(Event->Info.Bttn.BttnID)
            {
                case e_CT_Button_Rescan1:
                case e_CT_Button_Rescan2:
                    DCT_GetSelectedConLabel(ConPullDown1,SelectedCon1);
                    DCT_GetSelectedConLabel(ConPullDown2,SelectedCon2);
                    IOS_ScanForConnections();
                    DCT_GetListOfConnections();
                    DCT_FillInConnectionPullDown(ConPullDown1,e_DCT_Con1,
                            &SelectedCon1);
                    DCT_FillInConnectionPullDown(ConPullDown2,e_DCT_Con2,
                            &SelectedCon2);
                break;
                case e_CT_Button_Start:
                    DCT_StartTest();
                break;
                case e_CT_Button_Stop:
                    m_DCT_TestResult=e_DCT_TestResult_NotRun;
                    DCT_StopTest();
                break;
                case e_CT_ButtonMAX:
                default:
                break;
            }
        break;
        case e_CTEvent_ComboxChange:
            switch(Event->Info.Combox.BoxID)
            {
                case e_CT_Combox_ConnectionList1:
                    Con=(struct ConnectionInfoList *)Event->ID;
                    DCT_ChangeConnectionListSelectedEntry(Con,e_DCT_Con1);
                break;
                case e_CT_Combox_ConnectionList2:
                    Con=(struct ConnectionInfoList *)Event->ID;
                    DCT_ChangeConnectionListSelectedEntry(Con,e_DCT_Con2);
                break;
                case e_CT_Combox_Pattern:
                    DCT_RethinkUI();
                break;
                case e_CT_ComboxMAX:
                default:
                break;
            }
        break;
        case e_CTEvent_CheckboxChange:
            switch(Event->Info.Checkbox.BoxID)
            {
                case e_CT_Checkbox_Loopback:
                    DCT_RethinkUI();
                break;
                case e_CT_Checkbox_FullDuplex:
                    DCT_RethinkUI();
                break;
                case e_CT_CheckboxMAX:
                default:
                break;
            }
        break;
        case e_CTEventMAX:
        default:
        break;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    DCT_GetListOfConnections
 *
 * SYNOPSIS:
 *    static bool DCT_GetListOfConnections(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the list of available connections.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static bool DCT_GetListOfConnections(void)
{
    int r;

    if(m_DCT_Connections!=NULL)
    {
        for(r=0;r<e_DCTMAX;r++)
        {
            if(m_DCT_OptionsWidgets[r]!=NULL)
            {
                IOS_FreeConnectionOptions(m_DCT_OptionsWidgets[r]);
                m_DCT_OptionsWidgets[r]=NULL;
            }
        }

        /* Free the old list */
        IOS_FreeListOfAvailableConnections(m_DCT_Connections);
    }

    m_DCT_Connections=IOS_GetListOfAvailableConnections();
    if(m_DCT_Connections==NULL)
        return false;

    return true;
}

/*******************************************************************************
 * NAME:
 *    DCT_GetSelectedConLabel
 *
 * SYNOPSIS:
 *    static void DCT_GetSelectedConLabel(t_UIComboBoxCtrl *ConPullDown,
 *              string &SelectedLabel);
 *
 * PARAMETERS:
 *    ConPullDown [I] -- The connection list to get the selected entry from
 *    SelectedLabel [O] -- The label of the selected entry or ""
 *
 * FUNCTION:
 *    This function gets the selected entry from the UI
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DCT_GetSelectedConLabel(t_UIComboBoxCtrl *ConPullDown,
        string &SelectedLabel)
{
    uintptr_t SelectedID;
    struct ConnectionInfoList *Entry;

    SelectedID=UIGetComboBoxSelectedEntry(ConPullDown);

    /* Find this entry in the list of connections */
    for(Entry=m_DCT_Connections;Entry!=NULL;Entry=Entry->Next)
        if((uintptr_t)Entry==SelectedID)
            break;

    SelectedLabel="";
    if(Entry!=NULL)
        SelectedLabel=Entry->Name;
}

/*******************************************************************************
 * NAME:
 *    DCT_FillInConnectionPullDown
 *
 * SYNOPSIS:
 *    static void DCT_FillInConnectionPullDown(t_UIComboBoxCtrl *ConPullDown,
 *          e_DCTType Con,string *ReselectLabel);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function fills in the pull down list of connections.  It tries
 *    to keep the same selected entry if it can.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DCT_FillInConnectionPullDown(t_UIComboBoxCtrl *ConPullDown,
        e_DCTType Con,string *ReselectLabel)
{
    struct ConnectionInfoList *Entry;
    struct ConnectionInfoList *NewSelection;

    UIClearComboBox(ConPullDown);

    if(m_DCT_Connections==NULL)
        return;

    NewSelection=NULL;
    for(Entry=m_DCT_Connections;Entry!=NULL;Entry=Entry->Next)
    {
        UIAddItem2ComboBox(ConPullDown,Entry->Name,(uintptr_t)Entry);
        if(Entry->InUse)
        {
            UIStyleComboBoxItem(ConPullDown,(uintptr_t)Entry,
                    UISTYLE_STRIKETHROUGH);
        }
        if(ReselectLabel!=NULL && Entry->Name==*ReselectLabel)
            NewSelection=Entry;
    }

    if(NewSelection!=NULL)
        UISetComboBoxSelectedEntry(ConPullDown,(uintptr_t)NewSelection);

    if(NewSelection==NULL)
        DCT_ChangeConnectionListSelectedEntry(m_DCT_Connections,Con);
    else
        DCT_ChangeConnectionListSelectedEntry(NewSelection,Con);
}

/*******************************************************************************
 * NAME:
 *    DCT_ChangeConnectionListSelectedEntry
 *
 * SYNOPSIS:
 *    void DCT_ChangeConnectionListSelectedEntry(struct ConnectionInfoList *Con);
 *
 * PARAMETERS:
 *    Con [I] -- The new selected connection.
 *
 * FUNCTION:
 *    This function changes the currently selected connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void DCT_ChangeConnectionListSelectedEntry(struct ConnectionInfoList *Con,
        e_DCTType Connection)
{
    e_CT_Frame FrameID;
    t_UIContainerCtrl *Frame;

    if(m_DCT_OptionsWidgets[Connection]!=NULL)
    {
        /* Free the old options */
        IOS_FreeConnectionOptions(m_DCT_OptionsWidgets[Connection]);
    }

    switch(Connection)
    {
        case e_DCT_Con1:
            FrameID=e_CT_Frame_Connection1;
        break;
        case e_DCT_Con2:
            FrameID=e_CT_Frame_Connection2;
        break;
        case e_DCTMAX:
        default:
            return;
    }
    Frame=UICT_GetOptionsFrameContainer(FrameID);

    m_DCT_OptionsWidgets[Connection]=IOS_AllocConnectionOptions(Con,Frame,
            m_DCT_ConnectionOptions[Connection],NULL,NULL);
}

/*******************************************************************************
 * NAME:
 *    DCT_RethinkUI
 *
 * SYNOPSIS:
 *    static void DCT_RethinkUI(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rethinks what is valid and invalid and sets the UI as needed
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DCT_RethinkUI(void)
{
    t_UIButtonCtrl *Rescan1;
    t_UIButtonCtrl *Rescan2;
    t_UIButtonCtrl *Start;
    t_UIButtonCtrl *Stop;
    t_UIContainerCtrl *Options1;
    t_UIContainerCtrl *Options2;
    t_UIComboBoxCtrl *ConPullDown1;
    t_UIComboBoxCtrl *ConPullDown2;
    t_UIComboBoxCtrl *Pattern;
    t_UINumberInput *PacketLength;
    t_UINumberInput *Packets;
    t_UIDoubleInput *Delay;
    t_UICheckboxCtrl *Loopback;
    t_UICheckboxCtrl *FullDuplex;
    t_UILabelCtrl *StatusCtrl;

    bool DoingLoopback;
    bool DoingFullDuplex;
    e_DCT_PatternType SelectedPattern;
    bool LoopbackEnabled;
    bool FullDuplexEnabled;
    bool Connection1Enabled;
    bool Connection2Enabled;
    bool PatternEnabled;
    bool PacketLengthEnabled;
    bool PacketsEnabled;
    bool DelayEnabled;
    bool StartEnabled;
    bool StopEnabled;

    Rescan1=UICT_GetButton(e_CT_Button_Rescan1);
    Rescan2=UICT_GetButton(e_CT_Button_Rescan2);
    Start=UICT_GetButton(e_CT_Button_Start);
    Stop=UICT_GetButton(e_CT_Button_Stop);

    Options1=UICT_GetOptionsFrameContainer(e_CT_Frame_Connection1);
    Options2=UICT_GetOptionsFrameContainer(e_CT_Frame_Connection2);

    ConPullDown1=UICT_GetComboBoxHandle(e_CT_Combox_ConnectionList1);
    ConPullDown2=UICT_GetComboBoxHandle(e_CT_Combox_ConnectionList2);
    Pattern=UICT_GetComboBoxHandle(e_CT_Combox_Pattern);

    PacketLength=UICT_GetNumberInputHandle(e_CT_Number_PacketLength);
    Packets=UICT_GetNumberInputHandle(e_CT_Number_Packets);

    Delay=UICT_GetDoubleNumberInputHandle(e_CT_DoubleNumber_Delay);

    Loopback=UICT_GetCheckboxHandle(e_CT_Checkbox_Loopback);
    FullDuplex=UICT_GetCheckboxHandle(e_CT_Checkbox_FullDuplex);

    StatusCtrl=UICT_GetLabelHandle(e_CT_Label_Status);

    DoingLoopback=UIGetCheckboxCheckStatus(Loopback);
    DoingFullDuplex=UIGetCheckboxCheckStatus(FullDuplex);

    SelectedPattern=(e_DCT_PatternType)UIGetComboBoxSelectedEntry(Pattern);

    LoopbackEnabled=true;
    FullDuplexEnabled=true;
    Connection1Enabled=true;
    Connection2Enabled=true;
    PatternEnabled=true;
    PacketLengthEnabled=true;
    PacketsEnabled=true;
    DelayEnabled=true;
    StartEnabled=true;
    StopEnabled=true;

    if(m_DCT_DoingTest)
    {
        LoopbackEnabled=false;
        FullDuplexEnabled=false;
        Connection1Enabled=false;
        Connection2Enabled=false;
        PatternEnabled=false;
        PacketLengthEnabled=false;
        PacketsEnabled=false;
        DelayEnabled=false;
        StartEnabled=false;
    }
    else
    {
        StopEnabled=false;

        if(DoingFullDuplex)
        {
            LoopbackEnabled=false;
        }
        else if(DoingLoopback)
        {
            Connection2Enabled=false;
            FullDuplexEnabled=false;
        }

        switch(SelectedPattern)
        {
            case e_DCT_Pattern_55:
            case e_DCT_Pattern_AA:
            case e_DCT_Pattern_FF:
            case e_DCT_Pattern_00:
            case e_DCT_Pattern_Random:
            break;
            case e_DCT_Pattern_55AA:
            case e_DCT_Pattern_ABC:
            case e_DCT_Pattern_AscII:
            case e_DCT_Pattern_Binary:
            break;
            case e_DCT_PatternMAX:
            default:
            break;
        }

        switch(SelectedPattern)
        {
            case e_DCT_Pattern_55:
            case e_DCT_Pattern_AA:
            case e_DCT_Pattern_FF:
            case e_DCT_Pattern_00:
            case e_DCT_Pattern_Random:
            break;
            case e_DCT_Pattern_55AA:
            break;
            case e_DCT_Pattern_ABC:
            break;
            case e_DCT_Pattern_AscII:
            break;
            case e_DCT_Pattern_Binary:
            break;
            case e_DCT_PatternMAX:
            default:
            break;
        }
    }

    UIEnableCheckbox(Loopback,LoopbackEnabled);
    UIEnableCheckbox(FullDuplex,FullDuplexEnabled);

    UIEnableButton(Rescan1,Connection1Enabled);
    UIEnableButton(Rescan2,Connection2Enabled);
    UIEnableButton(Start,StartEnabled);
    UIEnableButton(Stop,StopEnabled);

    UIEnableComboBox(ConPullDown1,Connection1Enabled);
    UIEnableComboBox(ConPullDown2,Connection2Enabled);
    UIEnableComboBox(Pattern,PatternEnabled);

    UIEnableContainerCtrl(Options1,Connection1Enabled);
    UIEnableContainerCtrl(Options2,Connection2Enabled);

    UIEnableNumberInputCtrl(PacketLength,PacketLengthEnabled);
    UIEnableNumberInputCtrl(Packets,PacketsEnabled);

    UIEnableDoubleInputCtrl(Delay,DelayEnabled);

    if(m_DCT_DoingTest)
    {
        UISetLabelText(StatusCtrl,"Running...");
        UISetLabelBackgroundColor(StatusCtrl,0xFFFFFFFF);
    }
    else
    {
        if(m_DCT_TestResult>=e_DCT_TestResultMAX)
            m_DCT_TestResult=e_DCT_TestResult_NotRun;
        switch(m_DCT_TestResult)
        {
            case e_DCT_TestResult_NotRun:
            case e_DCT_TestResultMAX:
            default:
                UISetLabelText(StatusCtrl,"");
                UISetLabelBackgroundColor(StatusCtrl,0xFFFFFFFF);
            break;
            case e_DCT_TestResult_Pass:
                UISetLabelText(StatusCtrl,"Pass");
                UISetLabelBackgroundColor(StatusCtrl,0x00FF00);
            break;
            case e_DCT_TestResult_Fail:
                UISetLabelText(StatusCtrl,"Failed");
                UISetLabelBackgroundColor(StatusCtrl,0xFF0000);
            break;
        }
    }
}

/*******************************************************************************
 * NAME:
 *    DCT_AllocConnection4Test
 *
 * SYNOPSIS:
 *    static class Connection *DCT_AllocConnection4Test(e_DCTType ConNumber);
 *
 * PARAMETERS:
 *    ConNumber [I] -- The connection (e_DCT_Con1, or e_DCT_Con2) to allocate
 *
 * FUNCTION:
 *    This function allocates and sets the options for a connection based
 *    on what the UI is set to.
 *
 * RETURNS:
 *    The new connection or NULL if there was an error.  If there was an error
 *    the user has been informed.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static class Connection *DCT_AllocConnection4Test(e_DCTType ConNumber)
{
    t_UIComboBoxCtrl *ConPullDown;
    class Connection *Connection;
    struct ConnectionInfoList *ConInfo;
    string URI;

    Connection=NULL;
    try
    {
        switch(ConNumber)
        {
            case e_DCT_Con1:
                ConPullDown=UICT_GetComboBoxHandle(e_CT_Combox_ConnectionList1);
            break;
            case e_DCT_Con2:
                ConPullDown=UICT_GetComboBoxHandle(e_CT_Combox_ConnectionList2);
            break;
            case e_DCTMAX:
            default:
                throw("Unknown connection number.  Internal error");
        }
        ConInfo=(struct ConnectionInfoList *)
                UIGetComboBoxSelectedEntry(ConPullDown);
        if(ConInfo==NULL)
            throw("No selected connection to open");

        IOS_StoreConnectionOptions(m_DCT_OptionsWidgets[ConNumber],
                m_DCT_ConnectionOptions[ConNumber]);

        if(!IOS_MakeURIFromDetectedCon(ConInfo,
                m_DCT_ConnectionOptions[ConNumber],URI))
        {
            throw("Failed to make a URI for this connection");
        }

        /* Allocate the connections needed for this test (We are running
           without a UI) */
        Connection=Con_AllocateConnection(URI.c_str());
        if(Connection==NULL)
            throw("Failed to allocate connection");

        /* We are running without a UI (so everything goes in as NULL) */
        Connection->Init(NULL,NULL,NULL);

        if(!Connection->
                SetConnectionOptions(m_DCT_ConnectionOptions[ConNumber]))
        {
            throw("Failed to set connection options");
        }

        Connection->RegisterComTestUpdateFn(DCT_ComTestUpdateCallback);
    }
    catch(const char *Msg)
    {
        /* DEBUG PAUL: Ask here */
        UIAsk("Error starting com test",Msg,e_AskBox_Error,e_AskBttns_Ok);

        if(Connection!=NULL)
            Con_FreeConnection(Connection);

        return NULL;
    }
    return Connection;
}

/*******************************************************************************
 * NAME:
 *    DCT_StartTest
 *
 * SYNOPSIS:
 *    static void DCT_StartTest(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called to start the com test.
 *
 *    This will not return until the test is finished.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DCT_StartTest(void)
{
    t_UICheckboxCtrl *Loopback;
    t_UICheckboxCtrl *FullDuplex;
    t_UIComboBoxCtrl *Pattern;
    t_UINumberInput *PacketLength;
    t_UINumberInput *Packets;
    t_UIDoubleInput *Delay;
    t_UITextInputCtrl *PacketsSentCtrl;
    t_UITextInputCtrl *PacketsRecvCtrl;
    t_UITextInputCtrl *BytesCtrl;
    t_UITextInputCtrl *SendErrorsCtrl;
    t_UITextInputCtrl *SendBusyErrorsCtrl;
//    t_UITextInputCtrl *BytesPerSecCtrl;
    t_UITextInputCtrl *RxErrorsCtrl;
    bool DoingLoopback;
    bool DoingFullDuplex;
    long Packets2Send;
    long PatLen;
    uint32_t PacketDelaymS;
    e_DCT_PatternType SelectedPattern;
    int r;
    uint8_t CurPat;
    uint8_t InsertLetter;

    if(m_DCT_Connection1!=NULL)
    {
        Con_FreeConnection(m_DCT_Connection1);
        m_DCT_Connection1=NULL;
    }
    if(m_DCT_Connection2!=NULL)
    {
        Con_FreeConnection(m_DCT_Connection2);
        m_DCT_Connection2=NULL;
    }
    try
    {
        Loopback=UICT_GetCheckboxHandle(e_CT_Checkbox_Loopback);
        FullDuplex=UICT_GetCheckboxHandle(e_CT_Checkbox_FullDuplex);
        Pattern=UICT_GetComboBoxHandle(e_CT_Combox_Pattern);
        PacketLength=UICT_GetNumberInputHandle(e_CT_Number_PacketLength);
        Packets=UICT_GetNumberInputHandle(e_CT_Number_Packets);
        Delay=UICT_GetDoubleNumberInputHandle(e_CT_DoubleNumber_Delay);

        PacketsSentCtrl=UICT_GetTextInput(e_CT_TextInput_Stat_PacketsSent);
        PacketsRecvCtrl=UICT_GetTextInput(e_CT_TextInput_Stat_PacketsRecv);
        BytesCtrl=UICT_GetTextInput(e_CT_TextInput_Stat_Bytes);
        SendErrorsCtrl=UICT_GetTextInput(e_CT_TextInput_Stat_SendErrors);
        SendBusyErrorsCtrl=UICT_GetTextInput(e_CT_TextInput_Stat_SendBusyErrors);
//        BytesPerSecCtrl=UICT_GetTextInput(e_CT_TextInput_Stat_BytesPerSec);
        RxErrorsCtrl=UICT_GetTextInput(e_CT_TextInput_Stat_RxErrors);

        DoingLoopback=UIGetCheckboxCheckStatus(Loopback);
        DoingFullDuplex=UIGetCheckboxCheckStatus(FullDuplex);
        Packets2Send=UIGetNumberInputCtrlValue(Packets);
        PatLen=UIGetNumberInputCtrlValue(PacketLength);
        PacketDelaymS=UIGetDoubleInputCtrlValue(Delay)*1000.0;
        SelectedPattern=(e_DCT_PatternType)UIGetComboBoxSelectedEntry(Pattern);

        UISetTextCtrlText(PacketsSentCtrl,"");
        UISetTextCtrlText(BytesCtrl,"");
        UISetTextCtrlText(SendErrorsCtrl,"");
        UISetTextCtrlText(SendBusyErrorsCtrl,"");
        UISetTextCtrlText(PacketsRecvCtrl,"");
        UISetTextCtrlText(RxErrorsCtrl,"");
//        UISetTextCtrlText(BytesPerSecCtrl,"");

        if(m_DCT_PatternBuffer!=NULL)
            free(m_DCT_PatternBuffer);
        m_DCT_PatternBuffer=(uint8_t *)malloc(PatLen);
        if(m_DCT_PatternBuffer==NULL)
            throw("Out of memory");

        switch(SelectedPattern)
        {
            case e_DCT_Pattern_55:
                memset(m_DCT_PatternBuffer,0x55,PatLen);
            break;
            case e_DCT_Pattern_AA:
                memset(m_DCT_PatternBuffer,0xAA,PatLen);
            break;
            case e_DCT_Pattern_FF:
                memset(m_DCT_PatternBuffer,0xFF,PatLen);
            break;
            case e_DCT_Pattern_00:
                memset(m_DCT_PatternBuffer,0x00,PatLen);
            break;
            case e_DCT_Pattern_55AA:
                CurPat=0x55;
                for(r=0;r<PatLen;r++)
                {
                    m_DCT_PatternBuffer[r]=CurPat;
                    CurPat^=0xFF;
                }
            break;
            case e_DCT_Pattern_ABC:
                for(r=0;r<PatLen/26;r++)
                {
                    memcpy(&m_DCT_PatternBuffer[r*26],
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ",26);
                }
                memcpy(&m_DCT_PatternBuffer[r*26],"ABCDEFGHIJKLMNOPQRSTUVWXYZ",
                        PatLen%26);
            break;
            case e_DCT_Pattern_AscII:
                InsertLetter=' ';
                for(r=0;r<PatLen;r++)
                {
                    m_DCT_PatternBuffer[r]=InsertLetter;
                    InsertLetter++;
                    if(InsertLetter==126)
                        InsertLetter=' ';
                }
            break;
            case e_DCT_Pattern_Binary:
                for(r=0;r<PatLen;r++)
                    m_DCT_PatternBuffer[r]=r&0xFF;
            break;
            case e_DCT_Pattern_Random:
                for(r=0;r<PatLen;r++)
                    m_DCT_PatternBuffer[r]=rand()%256;
            break;
            case e_DCT_PatternMAX:
            default:
                throw("Unsupported pattern (program logic error)");
            break;
        }

        m_DCT_Connection1=DCT_AllocConnection4Test(e_DCT_Con1);
        if(m_DCT_Connection1==NULL)
            throw(0);

        m_DCT_Connection1->SetupComTest(true,PatLen,Packets2Send,PacketDelaymS,
                m_DCT_PatternBuffer);

        if(!DoingLoopback)
        {
            /* We need con 2 as well */
            m_DCT_Connection2=DCT_AllocConnection4Test(e_DCT_Con2);
            if(m_DCT_Connection2==NULL)
                throw(0);

            m_DCT_Connection2->SetupComTest(DoingFullDuplex,PatLen,Packets2Send,
                    PacketDelaymS,m_DCT_PatternBuffer);

            m_DCT_Connection2->SetConnectedState(true);
        }

        m_DCT_Connection1->SetConnectedState(true);

        UITimerStart(m_DCT_TestTimeoutCheckTimer);

        /* Start the test */
        if(!DoingLoopback)
        {
            if(!m_DCT_Connection2->StartComTest())
                throw("Failed to start com test on connection 2");
        }
        if(!m_DCT_Connection1->StartComTest())
            throw("Failed to start com test on connection 1");

        m_DCT_DoingTest=true;
        DCT_RethinkUI();
        DCT_ComTestUpdateCallback(NULL);

        if(PacketDelaymS==0)
        {
            /* Run the UI until we are done */
            if(DoingLoopback && !DoingFullDuplex)
            {
                /* Only sending from connection 1 */
                while(m_DCT_DoingTest && m_DCT_Connection1->IsComTestRunning())
                {
                    UI_ProcessAllPendingUIEvents();
                    m_DCT_Connection1->ComTestFastTick();
                }
            }
            else
            {
                /* We are sending from connection 1 and 2 */
                while(m_DCT_DoingTest && m_DCT_Connection1->IsComTestRunning()
                        && m_DCT_Connection2->IsComTestRunning())
                {
                    UI_ProcessAllPendingUIEvents();
                    m_DCT_Connection1->ComTestFastTick();
                    m_DCT_Connection2->ComTestFastTick();
                }
            }
        }
    }
    catch(const char *Msg)
    {
        UIAsk("Error starting com test",Msg,e_AskBox_Error,e_AskBttns_Ok);
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    DCT_StopTest
 *
 * SYNOPSIS:
 *    static void DCT_StopTest(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function stops a running test.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DCT_StopTest(void)
{
    if(m_DCT_Connection1!=NULL)
        m_DCT_Connection1->SetConnectedState(false);
    if(m_DCT_Connection2!=NULL)
        m_DCT_Connection2->SetConnectedState(false);

    UITimerStop(m_DCT_TestTimeoutCheckTimer);

    m_DCT_DoingTest=false;
    DCT_RethinkUI();
}

/*******************************************************************************
 * NAME:
 *    DCT_ComTestUpdateCallback
 *
 * SYNOPSIS:
 *    static void DCT_ComTestUpdateCallback(class Connection *Con);
 *
 * PARAMETERS:
 *    Con [I] -- The connection this update is from.  Ignored.
 *
 * FUNCTION:
 *    This function updates the UI with info from the com test.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DCT_ComTestUpdateCallback(class Connection *Con)
{
    t_UITextInputCtrl *PacketsSentCtrl;
    t_UITextInputCtrl *PacketsRecvCtrl;
    t_UITextInputCtrl *BytesCtrl;
    t_UITextInputCtrl *SendErrorsCtrl;
    t_UITextInputCtrl *SendBusyErrorsCtrl;
//    t_UITextInputCtrl *BytesPerSecCtrl;
    t_UITextInputCtrl *RxErrorsCtrl;
    t_UICheckboxCtrl *FullDuplexCtrl;
    t_UINumberInput *Packets;
    const struct ComTestStats *ConStatsTx;
    const struct ComTestStats *ConStatsRx;
    char buff[100];
    bool DoingFullDuplex;
    unsigned long Packets2Send;
    uint64_t ErrorCount;
    uint64_t PacketsRxed;
    uint64_t BytesSent;
    uint64_t SendBusyErrors;
    uint64_t SendErrors;
    uint64_t PacketsSent;

    if(!m_DCT_DoingTest)
        return;

    if(m_DCT_Connection1==NULL)
        return;

    PacketsSentCtrl=UICT_GetTextInput(e_CT_TextInput_Stat_PacketsSent);
    PacketsRecvCtrl=UICT_GetTextInput(e_CT_TextInput_Stat_PacketsRecv);
    BytesCtrl=UICT_GetTextInput(e_CT_TextInput_Stat_Bytes);
    SendErrorsCtrl=UICT_GetTextInput(e_CT_TextInput_Stat_SendErrors);
    SendBusyErrorsCtrl=UICT_GetTextInput(e_CT_TextInput_Stat_SendBusyErrors);
//    BytesPerSecCtrl=UICT_GetTextInput(e_CT_TextInput_Stat_BytesPerSec);
    RxErrorsCtrl=UICT_GetTextInput(e_CT_TextInput_Stat_RxErrors);
    Packets=UICT_GetNumberInputHandle(e_CT_Number_Packets);
    FullDuplexCtrl=UICT_GetCheckboxHandle(e_CT_Checkbox_FullDuplex);

    Packets2Send=UIGetNumberInputCtrlValue(Packets);
    DoingFullDuplex=UIGetCheckboxCheckStatus(FullDuplexCtrl);

    ConStatsTx=m_DCT_Connection1->GetComTestStats();
    if(m_DCT_Connection2!=NULL)
        ConStatsRx=m_DCT_Connection2->GetComTestStats();
    else
        ConStatsRx=ConStatsTx;

    ErrorCount=ConStatsRx->ErrorsDetected;
    PacketsRxed=ConStatsRx->PacketsRx;
    SendBusyErrors=ConStatsTx->SendBusyErrors;
    SendErrors=ConStatsTx->SendErrors;
    BytesSent=ConStatsTx->BytesSent;
    PacketsSent=ConStatsTx->PacketsSent;
    if(DoingFullDuplex)
    {
        /* We need to add any errors on connection 1 */
        ErrorCount+=ConStatsTx->ErrorsDetected;
        PacketsRxed+=ConStatsTx->PacketsRx;
        SendBusyErrors+=ConStatsRx->SendBusyErrors;
        SendErrors+=ConStatsRx->SendErrors;
        BytesSent+=ConStatsRx->BytesSent;
        PacketsSent+=ConStatsRx->PacketsSent;
    }

    sprintf(buff,"%" PRIu64,PacketsSent);
    UISetTextCtrlText(PacketsSentCtrl,buff);

    sprintf(buff,"%" PRIu64,BytesSent);
    UISetTextCtrlText(BytesCtrl,buff);

    sprintf(buff,"%" PRIu64,SendErrors);
    UISetTextCtrlText(SendErrorsCtrl,buff);

    sprintf(buff,"%" PRIu64,SendBusyErrors);
    UISetTextCtrlText(SendBusyErrorsCtrl,buff);

    sprintf(buff,"%" PRIu64,PacketsRxed);
    UISetTextCtrlText(PacketsRecvCtrl,buff);

    sprintf(buff,"%" PRIu64,ErrorCount);
    UISetTextCtrlText(RxErrorsCtrl,buff);

    sprintf(buff,"%" PRIu64,ConStatsRx->BytesPerSec);
//    UISetTextCtrlText(BytesPerSecCtrl,buff);

    /* Check if we are done */
    if(DoingFullDuplex)
    {
        if(ConStatsRx->PacketsRx==Packets2Send &&
                ConStatsTx->PacketsRx==Packets2Send)
        {
            /* Ok, we got to the end */
            if(ConStatsTx->SendErrors==0 && ConStatsTx->ErrorsDetected==0 &&
                    ConStatsRx->SendErrors==0 && ConStatsRx->ErrorsDetected==0)
            {
                m_DCT_TestResult=e_DCT_TestResult_Pass;
            }
            else
            {
                m_DCT_TestResult=e_DCT_TestResult_Fail;
            }
            DCT_StopTest();
        }
    }
    else
    {
        if(ConStatsRx->PacketsRx==Packets2Send)
        {
            /* Ok, we got to the end */
            if(ConStatsTx->SendErrors==0 && ConStatsRx->ErrorsDetected==0)
                m_DCT_TestResult=e_DCT_TestResult_Pass;
            else
                m_DCT_TestResult=e_DCT_TestResult_Fail;
            DCT_StopTest();
        }
    }
}

/*******************************************************************************
 * NAME:
 *    DCT_TestTimeoutCheck
 *
 * SYNOPSIS:
 *    static void DCT_TestTimeoutCheck(uintptr_t UserData);
 *
 * PARAMETERS:
 *    UserData [I] -- Not used
 *
 * FUNCTION:
 *    This function polls the connections to see if we are still getting
 *    bytes on the rx connection.  If we aren't then we stop the test
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DCT_TestTimeoutCheck(uintptr_t UserData)
{
    const struct ComTestStats *ConStatsTx;
    const struct ComTestStats *ConStatsRx;
    time_t CurTime;
    double TimePassed;
    t_UIDoubleInput *Delay;
    double PacketDelay;
    t_UICheckboxCtrl *FullDuplexCtrl;
    bool DoingFullDuplex;

    if(!m_DCT_DoingTest)
        return;

    if(m_DCT_Connection1==NULL)
        return;

    Delay=UICT_GetDoubleNumberInputHandle(e_CT_DoubleNumber_Delay);
    FullDuplexCtrl=UICT_GetCheckboxHandle(e_CT_Checkbox_FullDuplex);

    PacketDelay=UIGetDoubleInputCtrlValue(Delay);
    DoingFullDuplex=UIGetCheckboxCheckStatus(FullDuplexCtrl);

    ConStatsTx=m_DCT_Connection1->GetComTestStats();
    if(m_DCT_Connection2!=NULL)
        ConStatsRx=m_DCT_Connection2->GetComTestStats();
    else
        ConStatsRx=m_DCT_Connection1->GetComTestStats();

    time(&CurTime);
    if(DoingFullDuplex)
    {
        TimePassed=difftime(CurTime,ConStatsRx->LastRxTimeStamp);
        if(TimePassed>PacketDelay+DCT_TEST_END_TIMEOUT)
        {
            TimePassed=difftime(CurTime,ConStatsTx->LastRxTimeStamp);
            if(TimePassed>PacketDelay+DCT_TEST_END_TIMEOUT)
            {
                /* Timed out */
                m_DCT_TestResult=e_DCT_TestResult_Fail;
                DCT_StopTest();
            }
        }
    }
    else
    {
        TimePassed=difftime(CurTime,ConStatsRx->LastRxTimeStamp);
        if(TimePassed>PacketDelay+DCT_TEST_END_TIMEOUT)
        {
            /* Timed out */
            m_DCT_TestResult=e_DCT_TestResult_Fail;
            DCT_StopTest();
        }
    }
}

