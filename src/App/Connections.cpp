/*******************************************************************************
 * FILENAME: Connections.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the main class for the managing a connection.  It does the
 *    real work on the application.  It manages connections to the device,
 *    handles signals from the driver, reads data from the device and sends
 *    it to the UI (main window), handles sending bytes to the correct
 *    plug in, writes them to the driver, and all other connections needs.
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
#include "App/Connections.h"
#include "App/ConnectionsGlobal.h"
#include "App/MainWindow.h"
#include "App/DataProcessorsSystem.h"
#include "App/FileTransferProtocolSystem.h"
#include "App/Settings.h"
#include "App/Display/DisplayText.h"
#include "App/Display/DisplayBinary.h"
#include "OS/OSTime.h"
#include "UI/UIAsk.h"
#include "UI/UIClipboard.h"
#include "UI/UIDebug.h"
#include "UI/UISystem.h"
#include "UI/UITimers.h"
#include "Version.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <list>

using namespace std;

/*** DEFINES                  ***/
#define MAX_BYTES_PROCESSED_AT_1_TIME   1000    // Process up to 1k of data before redrawing the screen
//#define MAX_TIME_2_PROCESS_BYTES        10      // 10mS to process as many bytes as we can before we handle UI events again
#define MAX_TIME_2_PROCESS_BYTES        100      // 100mS to process as many bytes as we can before we handle UI events again
//#define MAX_TIME_2_PROCESS_BYTES        1000      // 1000mS to process as many bytes as we can before we handle UI events again

#define AUTOLAP_TIMEOUT                 500    // in ms

#define TRANSMIT_DELAY_BUFFER_SIZE      4000   // A little under a page size

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
typedef list<class Connection *> t_ConnectionListType;
typedef t_ConnectionListType::iterator i_ConnectionListType;

/*** FUNCTION PROTOTYPES      ***/
void Con_ComTestTimeout(uintptr_t UserData);
void Con_DelayTransmitTimeout(uintptr_t UserData);

/*** VARIABLE DEFINITIONS     ***/
t_ConnectionListType m_Connections;

void Connection::Debug1(void)
{
//    DB->Debug1();
}
void Connection::Debug2(void)
{
//    DB->Debug2();
}
void Connection::Debug3(void)
{
//    DB->Debug3();
}
void Connection::Debug4(void)
{
//    DB->Debug4();
}
void Connection::Debug5(void)
{
//    DB->Debug5();
}
void Connection::Debug6(void)
{
//    DB->Debug6();
}

/*******************************************************************************
 * NAME:
 *    Con_AllocateConnection
 *
 * SYNOPSIS:
 *    class Connection *Con_AllocateConnection(const char *URI);
 *
 * PARAMETERS:
 *    URI [I] -- The URI to use to open the connection.  This will figure out
 *               what driver to use and what type of display to use.
 *
 * FUNCTION:
 *    This function allocates a new connection and connects it to the
 *    IO system.
 *
 * RETURNS:
 *    A pointer to the new connection class or NULL if there was an error.
 *
 * SEE ALSO:
 *    Con_FreeConnection()
 ******************************************************************************/
class Connection *Con_AllocateConnection(const char *URI)
{
    class Connection *NewCon;

    try
    {
        NewCon=new Connection(URI);
        m_Connections.push_back(NewCon);
    }
    catch(const char *Msg)
    {
        UIAsk("Error",Msg,e_AskBox_Error,e_AskBttns_Ok);
        return NULL;
    }
    catch(...)
    {
        return NULL;
    }

    return NewCon;
}

/*******************************************************************************
 * NAME:
 *    Con_FreeConnection
 *
 * SYNOPSIS:
 *    void Con_FreeConnection(class Connection *Con);
 *
 * PARAMETERS:
 *    Con [I] -- The connection to free
 *
 * FUNCTION:
 *    This function frees a connection allocated with Con_AllocateConnection()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Con_AllocateConnection()
 ******************************************************************************/
void Con_FreeConnection(class Connection *Con)
{
    m_Connections.remove(Con);
    delete Con;
}

/*******************************************************************************
 * NAME:
 *    Con_DisplayBufferEvent
 *
 * SYNOPSIS:
 *    bool Con_DisplayBufferEvent(const struct TCEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event we should process.
 *
 * FUNCTION:
 *    This function is called from the text canvas control in the UI.  It
 *    is sent to us when there is an event in the connection tabs.
 *
 *    This is just a global space function.  It looks up the correct connection
 *    and sends the event there.
 *
 * RETURNS:
 *    true -- The event should act normally
 *    false -- There event should be canceled
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Con_DisplayBufferEvent(const struct DBEvent *Event)
{
    class Connection *Con=(class Connection *)Event->UserData;

    if(Con!=NULL)
        return Con->ProcessDisplayEvent(Event);
    return true;
}

/*******************************************************************************
 * NAME:
 *    Con_FileTransTick
 *
 * SYNOPSIS:
 *    void Con_FileTransTick(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function sends a file transfer tick to all the connections.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Con_FileTransTick(void)
{
    i_ConnectionListType Con;
    class Connection *ConPtr;

    for(Con=m_Connections.begin();Con!=m_Connections.end();Con++)
    {
        ConPtr=*Con;
        ConPtr->FileTransTick();
    }
}

/*******************************************************************************
 * NAME:
 *    Con_ComTestTimeout
 *
 * SYNOPSIS:
 *    void Con_ComTestTimeout(uintptr_t UserData);
 *
 * PARAMETERS:
 *    UsedData [I] -- The connection that this timer is for
 *
 * FUNCTION:
 *    This function is a call back from the UI that is called when the
 *    send timer goes off for the com test.  It just calls the
 *    InformOfComTestTimeout() function.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::InformOfComTestTimeout()
 ******************************************************************************/
void Con_ComTestTimeout(uintptr_t UserData)
{
    class Connection *Con=(class Connection *)UserData;
    Con->InformOfComTestTimeout();
}

/*******************************************************************************
 * NAME:
 *    Con_DelayTransmitTimeout
 *
 * SYNOPSIS:
 *    void Con_DelayTransmitTimeout(uintptr_t UserData);
 *
 * PARAMETERS:
 *    UsedData [I] -- The connection that this timer is for
 *
 * FUNCTION:
 *    This function is a call back from the UI that is called when the
 *    delay transmit timer goes off.  It just calls the
 *    InformOfDelayTransmitTimeout() function.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Con_DelayTransmitTimeout(uintptr_t UserData)
{
    class Connection *Con=(class Connection *)UserData;
    Con->InformOfDelayTransmitTimeout();
}

/*******************************************************************************
 * NAME:
 *    Con_ApplySettings2AllConnections
 *
 * SYNOPSIS:
 *    void Con_ApplySettings2AllConnections(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function applies the global settings to all open connections.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Con_ApplySettings2AllConnections(void)
{
    i_ConnectionListType Con;
    class Connection *ConPtr;
    bool HadAnError;

    HadAnError=false;
    for(Con=m_Connections.begin();Con!=m_Connections.end();Con++)
    {
        ConPtr=*Con;
        if(!ConPtr->ApplySettings())
            HadAnError=true;
    }
    if(HadAnError)
    {
        UIAsk("Error","There was an error applying settinsg to a connection",
                e_AskBox_Error,e_AskBttns_Ok);
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::Connection
 *
 * SYNOPSIS:
 *    Connection::Connection(void *ParentWidget,const char *URI);
 *
 * PARAMETERS:
 *    URI [I] -- The URI to use to open the connection.  This will figure out
 *               what driver to use and what type of display to use.
 *
 * FUNCTION:
 *    This is the constructor
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Init()
 ******************************************************************************/
Connection::Connection(const char *URI)
{
    IOHandle=NULL;
    Display=NULL;
    MW=NULL;
    BridgedTo=NULL;
    BridgedFrom=NULL;

    Bookmark=0;

    /* Copy the default settings for this connection from settings */
    UsingCustomSettings=false;
    CustomSettings=g_Settings.DefaultConSettings;

    FTPConData=FTPS_AllocFTPData();
    if(FTPConData==NULL)
        throw("Failed to allocate file transfer protocol data");

    TransmitDelayTimer=AllocUITimer();
    if(TransmitDelayTimer==NULL)
        throw("Failed to allocate delay timer");

    if(!SetConnectionBasedOnURI(URI))
        throw("Failed to setup the connection");

    SetupUITimer(TransmitDelayTimer,Con_DelayTransmitTimeout,(uintptr_t)this,
            false);

    IsConnected=false;
    BlockSendDevice=false;
    WhenBridgedLockoutConnection=false;
    ConnectionLockedOut=false;
    ShowNonPrintables=false;
    ShowEndOfLines=false;

    DisplayName[0]=0;

    TransmitDelayByte=0;
    TransmitDelayLine=0;
    TransmitDelayBuffer=NULL;
    TransmitDelayBufferSize=0;
    TransmitDelayBufferWritePos=0;
    TransmitDelayBufferReadPos=0;

    CaptureToFile.WriteHandle=NULL;
    CaptureToFile.Filename=g_Settings.CaptureDefaultFilename;
    CaptureToFile.Options.Timestamp=g_Settings.CaptureTimestamp;
    CaptureToFile.Options.Append=g_Settings.CaptureAppend;
    CaptureToFile.Options.StripCtrl=g_Settings.CaptureStripCtrl;
    CaptureToFile.Options.StripEsc=g_Settings.CaptureStripEsc;
    CaptureToFile.Options.SaveAsHexDump=g_Settings.CaptureHexDump;

    StopWatch.StartTime=0;
    StopWatch.StopTime=0;
    StopWatch.LastLapTime=0;
    StopWatch.Running=false;
    StopWatch.AutoStartOnTx=g_Settings.StopWatchAutoStart;
    StopWatch.AutoLap=g_Settings.StopWatchAutoLap;

    Upload.Filename="";
    Upload.ProtocolID="";
    Upload.LastTimeoutTick=0;
    Upload.Timeout=0;
    Upload.Stats.InProgress=false;
    Upload.Stats.BytesSent=0;
    Upload.Stats.TotalFileSize=0;

    Download.Filename="";
    Download.ProtocolID="";
    Download.LastTimeoutTick=0;
    Download.Timeout=0;
    Download.Stats.InProgress=false;
    Download.Stats.BytesRx=0;
    Download.Stats.TotalFileSize=0;

    HexDisplay.Paused=false;
    HexDisplay.BufferSize=0;
    HexDisplay.Buffer=NULL;
    HexDisplay.InsertPos=NULL;
    HexDisplay.BufferWrapped=false;

    ComTest.Sender=false;
    ComTest.SendingPackets=false;
    ComTest.PacketLen=1;
    ComTest.PacketsCount=1;
    ComTest.DelayBetweenPackets_mS=0;
    ComTest.Timer=NULL;
    ComTest.Packet=NULL;
    ComTest.RxPatternIndex=0;
    ComTest.UpdateFn=NULL;
    ComTest.Syncing=false;
    ComTest.Stats.InProgress=false;
    ComTest.Stats.PacketsSent=0;
    ComTest.Stats.BytesSent=0;
    ComTest.Stats.PacketsRx=0;
    ComTest.Stats.ErrorsDetected=0;
    ComTest.Stats.BytesPerSec=0;
    ComTest.Stats.SendErrors=0;
    ComTest.Stats.SendBusyErrors=0;
    ComTest.Stats.LastRxTimeStamp=0;

    if(!ApplySettings())
        throw("Failed to apply settings to the new connection");

    LeftPanelInfo=e_LeftPanelTabMAX;
    RightPanelInfo=e_RightPanelTabMAX;
    BottomPanelInfo=e_BottomPanelTabMAX;
}

/*******************************************************************************
 * NAME:
 *    Connection::~Connection
 *
 * SYNOPSIS:
 *    Connection::~Connection();
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This is the destructor.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
Connection::~Connection()
{
    if(BridgedFrom!=NULL)
        BridgedFrom->BridgeConnectionFreeing();

    /* Free everything we allocated */
    FreeConnectionResources(true);

    DPS_FreeProcessorConData(&ProcessorData);
    FTPS_FreeFTPData(FTPConData);
}

/*******************************************************************************
 * NAME:
 *    Connection::FreeConnectionResources
 *
 * SYNOPSIS:
 *    void Connection::FreeConnectionResources(bool FreeDB);
 *
 * PARAMETERS:
 *    FreeDB [I] -- If this is true then we free the display buffer.
 *
 * FUNCTION:
 *    This function free's everything that was allocated with this
 *    connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::FreeConnectionResources(bool FreeDB)
{
    if(TransmitDelayTimer!=NULL)
    {
        FreeUITimer(TransmitDelayTimer);
        TransmitDelayTimer=NULL;
    }

    if(ComTest.Timer!=NULL)
    {
        FreeUITimer(ComTest.Timer);
        ComTest.Timer=NULL;
    }

    /* Free the hex buffer */
    if(HexDisplay.Buffer!=NULL)
        free(HexDisplay.Buffer);
    HexDisplay.Buffer=NULL;
    HexDisplay.InsertPos=NULL;
    HexDisplay.BufferSize=0;

    if(IOHandle!=NULL)
    {
        if(IsConnected)
            IOS_Close(IOHandle);
        IOS_FreeIOSystemHandle(IOHandle);
    }

    if(FreeDB)
    {
        if(Display!=NULL)
            delete Display;
        Display=NULL;
    }

    IOHandle=NULL;
    IsConnected=false;
}

/*******************************************************************************
 * NAME:
 *    Connection::Init
 *
 * SYNOPSIS:
 *    bool Connection::Init(class TheMainWindow *MainWindow,void *ParentWidget,
 *              class ConSettings *SourceSettings);
 *
 * PARAMETERS:
 *    MainWindow [I] -- The main window that this connection is attached to.
 *    ParentWidget [I] -- The widget that this connection will live in
 *                        (normally the tab but can be any valid UI widget).
 *                        This can be NULL if we are running without a UI.
 *    SourceSettings [I] -- What settings to use for this connection (this
 *                          will be copied).  If this is NULL then global
 *                          settings will be used.
 *
 * FUNCTION:
 *    This function init's the connection and makes it ready to be opened.
 *    This will allocate the display's, default any needed settings.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * NOTES:
 *    You must call FinalizeNewConnection() to finish the init process.
 *
 *    The process for opening a conenction is to:
 *      1) Allocate the connection
 *      2) Setup anything you need to init the connection
 *      3) Init the connection
 *      4) Setup anything that needs a working connection
 *      5) Call the finalize function
 *
 * SEE ALSO:
 *    Connection::FinalizeNewConnection()
 ******************************************************************************/
bool Connection::Init(class TheMainWindow *MainWindow,void *ParentWidget,
        class ConSettings *SourceSettings)
{
    i_DPSDataProcessorsType FirstProcessor;
    string UniqueID;
    struct ConnectionInfoList ConInfo;
    t_KVList Options;

    Display=nullptr;
    try
    {
        MW=MainWindow;

        /* Setup what settings we are using */
        if(SourceSettings==nullptr)
        {
            UsingCustomSettings=false;
            CustomSettings=g_Settings.DefaultConSettings;
        }
        else
        {
            UsingCustomSettings=true;
            CustomSettings=*SourceSettings;
        }

        /* We only setup the data processors and display if we have a parent
           widget */
        if(ParentWidget!=nullptr)
        {
            /* Allocate the data processors based on the settings */
            if(!DPS_AllocProcessorConData(&ProcessorData,&CustomSettings))
                throw(0);

            /* Check if this is a text connection or a binary connection.  Do
               this by looking at the first processor and seeing what type it
               is (if we don't have a processor then assume text). */
            FirstProcessor=ProcessorData.DataProcessorsList.begin();
            if(FirstProcessor==ProcessorData.DataProcessorsList.end() ||
                    FirstProcessor->Info.ProType==e_DataProcessorType_Text)
            {
                /* We are text connection or there are no processors */
                Display=new DisplayText();
            }
            else
            {
                /* We are binary */
                Display=new DisplayBinary();
            }

            if(!Display->Init(ParentWidget,Con_DisplayBufferEvent,
                    (uintptr_t)this))
            {
                throw(0);
            }

            /* Use our copy of the settings */
            Display->SetCustomSettings(&CustomSettings);
        }

        /* IOHandle was allocated in the constructor */
        IOS_GetUniqueID(IOHandle,UniqueID);
        IOS_GetConnectionOptions(IOHandle,Options);
        if(IOS_GetConnectionInfo(UniqueID.c_str(),Options,&ConInfo))
        {
            BlockSendDevice=ConInfo.BlockDevice;
            if(Display!=nullptr)
                Display->SetBlockDeviceMode(BlockSendDevice);
        }
    }
    catch(...)
    {
        if(Display!=nullptr)
            delete Display;

        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    Connection::FinalizeNewConnection
 *
 * SYNOPSIS:
 *    void Connection::FinalizeNewConnection(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called after a new connection is allocated.  It does
 *    system house keeping things like opening the connection if the settings
 *    are set to do so, and any other house keeping that should be done right
 *    after a new connection becomes active.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    This is mostly to do "normal" things on a fully functioning connection
 *    that you might want to do after it's allocated.  For example this function
 *    will open the connection if the settings tell it to do so.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::FinalizeNewConnection(void)
{
    /* We auto open this when we open the tab */
    if(g_Settings.AutoConnectOnNewConnection)
        IOS_Open(IOHandle);

    /* DEBUG PAUL: Set show non-printables based on settings */
    //SetShowNonPrintable(xxx);


    /* Give this connection focus */
    GiveFocus();
}

/*******************************************************************************
 * NAME:
 *    Connection::SetConnection
 *
 * SYNOPSIS:
 *    bool Connection::SetConnection(const std::string &UniqueID);
 *
 * PARAMETERS:
 *    UniqueID [I] -- The unique ID of the IO system to connect to
 *
 * FUNCTION:
 *    This function allocates the connection IO system to use.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    Connection::SetConnectionBasedOnURI(),
 ******************************************************************************/
bool Connection::SetConnection(const std::string &UniqueID)
{
    IOHandle=IOS_AllocIOSystemHandle(UniqueID.c_str(),(uintptr_t)this);
    if(IOHandle==NULL)
        return false;
    return true;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetConnectionBasedOnURI
 *
 * SYNOPSIS:
 *    bool Connection::SetConnectionBasedOnURI(const char *URI);
 *
 * PARAMETERS:
 *    URI [I] -- The URI to use to connect the IO system.
 *
 * FUNCTION:
 *    This function allocates the connection IO system to use based on a
 *    URI instead of a Unique ID.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    Connection::SetConnection()
 ******************************************************************************/
bool Connection::SetConnectionBasedOnURI(const char *URI)
{
    IOHandle=IOS_AllocIOSystemHandleFromURI(URI,(uintptr_t)this);
    if(IOHandle==NULL)
        return false;
    return true;
}

/*******************************************************************************
 * NAME:
 *    Connection::ApplySettings
 *
 * SYNOPSIS:
 *    bool Connection::ApplySettings(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function takes the global settings and apply any changes.
 *
 * RETURNS:
 *    true -- All ok
 *    false -- There was an error applying the settings.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Connection::ApplySettings(void)
{
    int RoundedBufferSize;
    union ConMWInfo EventData;

    if(!UsingCustomSettings)
    {
        /* We need to copy the global settings over our settings */
        CustomSettings=g_Settings.DefaultConSettings;
        ApplyCustomSettings();
    }

    RoundedBufferSize=g_Settings.HexDisplayBufferSize;
    if(RoundedBufferSize<16)
        RoundedBufferSize=16;
    RoundedBufferSize=RoundedBufferSize/16*16; // Round to be a multable of 16

    if(HexDisplay.BufferSize!=RoundedBufferSize ||
            (HexDisplay.Buffer==NULL && g_Settings.HexDisplayEnabled) ||
            (HexDisplay.Buffer!=NULL && !g_Settings.HexDisplayEnabled))
    {
        /* Free the old buffer (if needed) */
        if(HexDisplay.Buffer!=NULL)
            free(HexDisplay.Buffer);
        HexDisplay.Buffer=NULL;
        HexDisplay.InsertPos=NULL;

        HexDisplay.BufferSize=RoundedBufferSize;

        if(g_Settings.HexDisplayEnabled)
        {
            /* Allocate a new buffer */
            HexDisplay.Buffer=(uint8_t *)malloc(HexDisplay.BufferSize);
            if(HexDisplay.Buffer==NULL)
                return false;
        }
        else
        {
            HexDisplay.Buffer=NULL;
            HexDisplay.BufferSize=0;
        }
        HexDisplay.InsertPos=HexDisplay.Buffer;

        /* Update the HEX UI */
        EventData.HexDis.Buffer=HexDisplay.Buffer;
        EventData.HexDis.InsertPos=HexDisplay.InsertPos;
        EventData.HexDis.BufferIsCircular=HexDisplay.BufferWrapped;
        EventData.HexDis.BufferSize=HexDisplay.BufferSize;

        SendMWEvent(ConMWEvent_HexDisplayBufferChange,&EventData);
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetCustomSettings
 *
 * SYNOPSIS:
 *    void Connection::SetCustomSettings(class ConSettings &NewSettings)
 *
 * PARAMETERS:
 *    NewSettings [I] -- The new setting to apply to this connection.
 *
 * FUNCTION:
 *    This function applies new custom settings to this connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::SetCustomSettings(class ConSettings &NewSettings)
{
    /* Copy the settings and reapply them */
    UsingCustomSettings=true;
    CustomSettings=NewSettings;

    ApplyCustomSettings();
}

/*******************************************************************************
 * NAME:
 *    Connection::ApplyCustomSettings
 *
 * SYNOPSIS:
 *    void Connection::ApplyCustomSettings(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function applies the custom settings to this connection again.
 *    This should be called after you have changed the custom settings.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::ApplyCustomSettings(void)
{
    if(!UsingCustomSettings)
    {
        /* Restore the globals */
        CustomSettings=g_Settings.DefaultConSettings;
    }

    if(Display==NULL)
        return;

    /* Free the processor data and then reallocate it (so we switch) */
    if(!DPS_ReapplyProcessor2Connection(&ProcessorData,
            &CustomSettings))
    {
        UIAsk("Error","Failed to reallocate data processor data",
                e_AskBox_Error,e_AskBttns_Ok);
        /* DEBUG PAUL: We are now dead what do we do? */
        return;
    }

    /* Set the cursor color */
    if(Display!=nullptr)
        Display->ApplySettings();
}

/*******************************************************************************
 * NAME:
 *    Connection::SetConnectionOptions
 *
 * SYNOPSIS:
 *    bool Connection::SetConnectionOptions(const t_KVList &Options);
 *
 * PARAMETERS:
 *    Options [I] -- The options to apply to the connection.
 *
 * FUNCTION:
 *    This function changes the options for a connection.  It may need to
 *    close the connection to apply the changes.
 *
 * RETURNS:
 *    true -- All ok
 *    false -- There was an error
 *
 * SEE ALSO:
 *    Connection::GetConnectionOptions()
 ******************************************************************************/
bool Connection::SetConnectionOptions(const t_KVList &Options)
{
    bool RetValue;

    RetValue=IOS_SetConnectionOptions(IOHandle,Options);
    SendMWEvent(ConMWEvent_ConOptionsChange);

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    Connection::GetConnectionOptions
 *
 * SYNOPSIS:
 *    bool Connection::GetConnectionOptions(t_KVList &Options);
 *
 * PARAMETERS:
 *    Options [O] -- The options for the open connection.
 *
 * FUNCTION:
 *    This function gets the connection options for this connection.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    Connection::SetConnectionOptions()
 ******************************************************************************/
bool Connection::GetConnectionOptions(t_KVList &Options)
{
    if(IOHandle==NULL)
        return false;

    IOS_GetConnectionOptions(IOHandle,Options);

    return true;
}

/*******************************************************************************
 * NAME:
 *    Connection::GetCurrentProcessorData
 *
 * SYNOPSIS:
 *    struct ProcessorConData *Connection::GetCurrentProcessorData(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the current processor connection data.  This should
 *    really only be used by the Data Processors System.
 *
 * RETURNS:
 *    A pointer to the processor connection data for this connection.
 *
 * SEE ALSO:
 *    Con_GetCurrentProcessorData()
 ******************************************************************************/
struct ProcessorConData *Connection::GetCurrentProcessorData(void)
{
    return &ProcessorData;
}

/*******************************************************************************
 * NAME:
 *    Connection::ReParentWidget
 *
 * SYNOPSIS:
 *    void Connection::ReParentWidget(void *NewParentWidget);
 *
 * PARAMETERS:
 *    NewParentWidget [I] -- The new parent widget for this connection.
 *
 * FUNCTION:
 *    This function will change the UI parent widget that this connection
 *    uses.  It will change any UI widgets that this connection uses (owns)
 *    to this new parent.  This is normally used when the main window wants
 *    to change the UI tab that the connection lives in.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::ReParentWidget(void *NewParentWidget)
{
    if(Display!=nullptr)
        Display->Reparent(NewParentWidget);
}

///*******************************************************************************
// * NAME:
// *    Connection::AddTab
// *
// * SYNOPSIS:
// *    bool Connection::AddTab(void);
// *
// * PARAMETERS:
// *    NONE
// *
// * FUNCTION:
// *    This function tells the main window to add a new tab to the system.
// *
// *    What a "tab" is the connection system doesn't care.
// *
// * RETURNS:
// *    true -- Things worked out
// *    false -- There was an error
// *
// * SEE ALSO:
// *    
// ******************************************************************************/
//bool Connection::AddTab(void)
//{
//    void *ParentWidget;
//    string UniqueID;
//    struct ConnectionInfoList ConInfo;
//
//    if(MW==NULL)
//        return false;
//
//    /* Ok, we need to tell the main window to add a tab (or whatever it's
//       actually using) */
//    ParentWidget=MW->AllocTab(this);
//    if(ParentWidget==NULL)
//        return NULL;
//
////    if(!DB->Init(ParentWidget,&CustomSettings,Con_DisplayBufferEvent,
////            (uintptr_t)this))
////    {
////        MW->FreeTab(this);
////        return false;
////    }
//
//    MW->SetActiveTab(this);
//    SendMWEvent(ConMWEvent_StatusChange);
//    SendMWEvent(ConMWEvent_NewConnection);
//
//    if(IOHandle!=NULL)
//    {
//        IOS_GetUniqueID(IOHandle,UniqueID);
//        if(IOS_GetConnectionInfo(UniqueID.c_str(),NULL,&ConInfo))
//        {
//            if(ConInfo.BlockDevice)
//            {
////                DB->SetBlockDeviceMode(true);
//                BlockSendDevice=true;
//            }
//        }
//
//        /* We auto open this when we open the tab */
//        if(g_Settings.AutoConnectOnNewConnection)
//            IOS_Open(IOHandle);
//    }
//
//    return true;
//}

/*******************************************************************************
 * NAME:
 *    Connection::SetMainWindow
 *
 * SYNOPSIS:
 *    void Connection::SetMainWindow(class TheMainWindow *MainWindow);
 *
 * PARAMETERS:
 *    MainWindow [I] -- The main window to that this connection lives in.
 *
 * FUNCTION:
 *    This function sets the main window that this connection lives in.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::SetMainWindow(class TheMainWindow *MainWindow)
{
    MW=MainWindow;
}

///*******************************************************************************
// * NAME:
// *    Connection::GetDisplayFrameCtrl
// *
// * SYNOPSIS:
// *    t_UIDisplayFrameCtrl *Connection::GetDisplayFrameCtrl(void);
// *
// * PARAMETERS:
// *    NONE
// *
// * FUNCTION:
// *    This function gets access to the display frame that was allocated when
// *    the tab was added.
// *
// * RETURNS:
// *    A pointer to the DisplayFrame or NULL if it has not been set.
// *
// * SEE ALSO:
// *    
// ******************************************************************************/
//t_UIDisplayFrameCtrl *Connection::GetDisplayFrameCtrl(void)
//{
////    return DB->GetDisplayFrameCtrl();
//return NULL;
//}

/*******************************************************************************
 * NAME:
 *    Connection:SetDisplayName
 *
 * SYNOPSIS:
 *    void Connection:SetDisplayName(const char *Name);
 *
 * PARAMETERS:
 *    Name [I] -- The name for this connection.  If you pass in NULL then
 *                the default name for this connection is used (the drivers
 *                short name).
 *
 * FUNCTION:
 *    This function sets the display name for this connection.  The display
 *    name is used when talking about this connection to the user.  It can
 *    also be used as a file name.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::GetDisplayName()
 ******************************************************************************/
void Connection::SetDisplayName(const char *Name)
{
    union ConMWInfo Info;
    const char *UseName;
    struct ConnectionInfoList ConInfo;
    t_KVList Options;

    try
    {
        UseName=Name;

        if(UseName==NULL)
        {
            string UniqueID;

            if(IOHandle==NULL)
                return;

            IOS_GetUniqueID(IOHandle,UniqueID);
            IOS_GetConnectionOptions(IOHandle,Options);

            UseName="Unknown";
            if(IOS_GetConnectionInfo(UniqueID.c_str(),Options,&ConInfo))
                UseName=ConInfo.Title.c_str();
        }

        strncpy(DisplayName,UseName,MAX_CONNECTION_NAME_LEN);
        DisplayName[MAX_CONNECTION_NAME_LEN]=0;

        if(MW!=NULL)
        {
            Info.NameChange.NewName=DisplayName;
            SendMWEvent(ConMWEvent_NameChange,&Info);
        }
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::GetDisplayName
 *
 * SYNOPSIS:
 *    void Connection::GetDisplayName(std::string &Name);
 *
 * PARAMETERS:
 *    Name [O] -- The display name for this connection.
 *
 * FUNCTION:
 *    This function gets the display name for this connection.  This is the
 *    label that normally shows up in the tab.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::GetDisplayName(std::string &Name)
{
    Name=DisplayName;
}

/*******************************************************************************
 * NAME:
 *    Connection::TextCanvasResize
 *
 * SYNOPSIS:
 *    void Connection::TextCanvasResize(int Width,int Height);
 *
 * PARAMETERS:
 *    Width [I] -- The new width of the text area canvas in pixels
 *    Height [I] -- The new height of the text area canvas in pixels
 *
 * FUNCTION:
 *    This function is called when the text canvas is resized.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::TextCanvasResize(int Width,int Height)
{
//    if(DB!=NULL)
//        DB->SetDisplaySize(Width,Height);
}

/*******************************************************************************
 * NAME:
 *    Connection::KeyPress
 *
 * SYNOPSIS:
 *    bool Connection::KeyPress(uint8_t Mods,e_UIKeys Key,
 *              const uint8_t *TextPtr,unsigned int TextLen);
 *
 * PARAMETERS:
 *    Mods [I] -- What keys where held when this key event happended
 *    Key [I] -- What "special" key was pressed (things like arrow keys)
 *    TextPtr [I] -- A string with the key in it (UTF8).  If the key can be
 *                   converted to a char this will have it.
 *    TextLen [I] -- The number of bytes in 'TextPtr'
 *
 * FUNCTION:
 *    This function is called when there is a text canvas key press (a key
 *    press that is to be sent).
 *
 * RETURNS:
 *    true -- The key press was consumed
 *    false -- The key press was ignored.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Connection::KeyPress(uint8_t Mods,e_UIKeys Key,const uint8_t *TextPtr,
        unsigned int TextLen)
{
    bool RetValue;

    if(IOHandle==NULL || Display==NULL || MW==NULL)
        return false;

//    /* If the display isn't set to have focus ignore this key press */
//    if(!DB->GetFocusStatus())
//        return false;

    if(MW->KeyPress(Mods,Key,TextPtr,TextLen))
        return true;

    Con_SetActiveConnection(this);

    RetValue=false;
    if(!DPS_ProcessorKeyPress(TextPtr,TextLen,Key,Mods))
    {
        /* Send the key out */
        if(TextLen>0)
        {
            WriteData(TextPtr,TextLen,e_ConWriteSource_Keyboard);
            RetValue=true;
        }
    }

    Con_SetActiveConnection(NULL);

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    Connection::WriteData
 *
 * SYNOPSIS:
 *    e_ConWriteType Connection::WriteData(const uint8_t *Data,int Bytes,
 *              e_ConWriteSourceType Source);
 *
 * PARAMETERS:
 *    Data [I] -- The data to send
 *    Bytes [I] -- The number of bytes to send
 *    Source [I] -- Who is sending this data.  This is used for filtering.
 *                      e_ConWriteSource_Keyboard -- User typed on the keyboard
 *                              (or a macro playback?)
 *                      e_ConWriteSource_Upload -- From the upload file plugin
 *                      e_ConWriteSource_Download -- From the download file
 *                              plugin
 *                      e_ConWriteSource_Paste -- Pasted from the clip board
 *                      e_ConWriteSource_Buffers -- Sent from the send buffers
 *                              system.
 *                      e_ConWriteSource_BlockSend -- Send from the block send
 *                              buffer (used with block devices).
 *                      e_ConWriteSource_Bridge -- Sent from a bridged
 *                              connection.
 *
 * FUNCTION:
 *    This function writes to the connection.  It send this data to the
 *    IO driver system.
 *
 * RETURNS:
 *    e_ConWrite_Success -- Things worked.  The bytes have been sent
 *              (or at least queued)
 *    e_ConWrite_Failed -- There was an error sending.  It has been noted.
 *    e_ConWrite_Busy -- We could not sent this, but we can try again later.
 *    e_ConWrite_Ignored -- Because of the mode we are in we didn't send it.
 *
 * SEE ALSO:
 *    Con_WriteData()
 ******************************************************************************/
e_ConWriteType Connection::WriteData(const uint8_t *Data,int Bytes,
        e_ConWriteSourceType Source)
{
    e_ConWriteType RetValue;

    if(!IsConnected)
        return e_ConWrite_Failed;

    /* We ignore key presses if it's a block send connection */
    if(Source==e_ConWriteSource_Keyboard && BlockSendDevice)
        return e_ConWrite_Ignored;

    if(StopWatch.AutoStartOnTx)
    {
        if(!StopWatch.Running)
        {
            StopWatchStartStop(true);
            SendMWEvent(ConMWEvent_StopWatchAutoStarted);
        }
    }

    if(ConnectionLockedOut)
    {
        if(Upload.Stats.InProgress)
        {
            if(Source!=e_ConWriteSource_Upload)
                return e_ConWrite_Ignored;
        }
        else if(Download.Stats.InProgress)
        {
            if(Source!=e_ConWriteSource_Download)
                return e_ConWrite_Ignored;
        }
        else if(Source!=e_ConWriteSource_Bridge)
        {
            return e_ConWrite_Ignored;
        }

//        if(Source!=e_ConWriteSource_Upload && Upload.Stats.InProgress)
//            return e_ConWrite_Ignored;
//
//        if(Source!=e_ConWriteSource_Download && Download.Stats.InProgress)
//            return e_ConWrite_Ignored;
//
//        if(Source!=e_ConWriteSource_Bridge)
//        {
//            return e_ConWrite_Ignored;
//        }
    }

    if(TransmitDelayByte!=0 || TransmitDelayLine!=0)
    {
        /* Ok, we need to queue this because we can't block the GUI */
        if(TransmitDelayBufferWritePos>0 &&
                (Source!=e_ConWriteSource_Keyboard &&
                Source!=e_ConWriteSource_BlockSend))
        {
            /* We already have something queued (and it's not a keyboard
               input), we reject it because otherwize we might end up
               buffer a whole file (maybe Gb of data) */
            RetValue=e_ConWrite_Busy;
        }
        else
        {
            /* Queue the data for sending later. */
            if(QueueTransmitDelayData(Data,Bytes))
            {
                /* Send the first byte / block if we haven't already sent
                   something (or we are finished the last timeout) */
                if(!UITimerRunning(TransmitDelayTimer))
                    InformOfDelayTransmitTimeout();

                RetValue=e_ConWrite_Success;
            }
            else
            {
                RetValue=e_ConWrite_Failed;
            }
        }
    }
    else
    {
        RetValue=InternalWriteBytes(Data,Bytes);
    }
    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    Connection::InternalWriteBytes
 *
 * SYNOPSIS:
 *    e_ConWriteType Connection::InternalWriteBytes(const uint8_t *Data,
 *          int Bytes);
 *
 * PARAMETERS:
 *    Data [I] -- The data to send
 *    Bytes [I] -- The number of bytes to send
 *
 * FUNCTION:
 *    This is an internal helper function that writes bytes out to the device.
 *    This handles converting device errors into 
 *
 * RETURNS:
 *    e_ConWrite_Success -- Things worked.  The bytes have been sent
 *              (or at least queued)
 *    e_ConWrite_Failed -- There was an error sending.  It has been noted.
 *    e_ConWrite_Busy -- We could not sent this, but we can try again later.
 *    e_ConWrite_Ignored -- Because of the mode we are in we didn't send it.
 *
 * NOTES:
 *    This is for internal use, see Connection::WriteData() instead
 *
 * SEE ALSO:
 *    Connection::WriteData()
 ******************************************************************************/
e_ConWriteType Connection::InternalWriteBytes(const uint8_t *Data,int Bytes)
{
    e_ConWriteType RetValue;

    RetValue=e_ConWrite_Failed;
    switch(IOS_WriteData(IOHandle,Data,Bytes))
    {
        case e_IOSysIOError_Success:
            RetValue=e_ConWrite_Success;
        break;
        case e_IOSysIOError_GenericIO:
            /* Ok, we need to track the errors here.  DO NOT put a pop up box */
            /* DEBUG PAUL: TDB */
            RetValue=e_ConWrite_Failed;
        break;
        case e_IOSysIOError_Disconnect:
            /* Tell the system that we have been disconnected */
            InformOfDisconnected();
            RetValue=e_ConWrite_Failed;
        break;
        case e_IOSysIOError_Busy:
            RetValue=e_ConWrite_Busy;
        break;
        case e_IOSysIOErrorMAX:
        default:
        break;
    }

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    Connection::TransmitQueuedData
 *
 * SYNOPSIS:
 *    void Connection::TransmitQueuedData(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function tells the driver to send any queued data.  This is only
 *    really usefull for block send style drivers as stream device always
 *    send their data.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::TransmitQueuedData(void)
{
    if(!IsConnected)
        return;

    if(IOS_TransmitQueuedData(IOHandle)!=e_IOSysIOError_Success)
        UIAsk("Error","Failed to send.",e_AskBox_Error,e_AskBttns_Ok);
}

/*******************************************************************************
 * NAME:
 *    Connection::InformOfConnected
 *
 * SYNOPSIS:
 *    void Connection::InformOfConnected(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when a driver changes the connected status of
 *    a connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::InformOfConnected(void)
{
    if(Display==NULL)
        return;

    IsConnected=true;
    SendMWEvent(ConMWEvent_StatusChange);
    RethinkCursor();
//    DB->SetConnectStatus(true);
}

/*******************************************************************************
 * NAME:
 *    Connection::InformOfDisconnected
 *
 * SYNOPSIS:
 *    void Connection::InformOfDisconnected(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when a driver changes the connected status of
 *    a connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::InformOfDisconnected(void)
{
    if(Display==NULL)
        return;

    IsConnected=false;
    SendMWEvent(ConMWEvent_StatusChange);
    RethinkCursor();
//    DB->SetConnectStatus(false);
}

/*******************************************************************************
 * NAME:
 *    Connection::GetConnectedStatus
 *
 * SYNOPSIS:
 *    bool Connection::GetConnectedStatus(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the current connected status for this connection.
 *
 * RETURNS:
 *    true -- The connection is connected
 *    false -- The connection is disconnected
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Connection::GetConnectedStatus(void)
{
    return IsConnected;
}

/*******************************************************************************
 * NAME:
 *    Connection::InformOfDataAvaiable
 *
 * SYNOPSIS:
 *    bool Connection::InformOfDataAvaiable(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called to tell this connection that there is data ready
 *    to be read in.
 *
 * RETURNS:
 *    true -- We have more to process.  Schedule to call us again (after you
 *            have processed anything else you need to)
 *    false -- No more data to process.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Connection::InformOfDataAvaiable(void)
{
//    long TotalBytes;
    uint32_t StartTime;
    int bytes;
    uint8_t inbuff[100];
    bool ProcessBlock;
    bool RetValue;

    Con_SetActiveConnection(this);

    /* Read the data from the driver, pass is though the data processors,
       pass that data on to the main window */

/* DEBUG PAUL: Looks like this is only 1/2 done.  It doesn't seem to handle
   timeouts, and TotalBytes correctly */

    RetValue=false;
    StartTime=GetElapsedTime_ms();
//    TotalBytes=0;
    do
    {
        bytes=IOS_ReadData(IOHandle,inbuff,sizeof(inbuff));

        if(BridgedTo!=NULL)
        {
            /* Send this into the bridged connection */
            BridgedTo->WriteData(inbuff,bytes,e_ConWriteSource_Bridge);
        }

//        if(TotalBytes>=MAX_BYTES_PROCESSED_AT_1_TIME)
//        {
//            /* Refresh the screen */
//            TotalBytes=0;
//        }

        if(bytes>0)
        {
            if(ComTest.Stats.InProgress)
            {
                /* We are doing the com test */
                HandleComTestRx(inbuff,bytes);
            }
            else
            {
                /* Normal processing */
                ProcessBlock=true;
                if(Upload.Stats.InProgress || Download.Stats.InProgress)
                {
                    if(FTPS_ProcessIncomingBytes(FTPConData,inbuff,bytes))
                        ProcessBlock=false;
                }

                HandleHexDisplayIncomingData(inbuff,bytes);
                HandleCaptureIncomingData(inbuff,bytes);
                StopWatchHandleAutoLap();

                if(ProcessBlock)
                    DPS_ProcessorIncomingBytes(inbuff,bytes);
            }
        }

        if(GetElapsedTime_ms()-StartTime>MAX_TIME_2_PROCESS_BYTES)
            break;
    } while(bytes>0);

    if(bytes>0)
    {
        /* Ok, we have more data waiting, flag it for us to come back in
           (and then return up to let the UI queue finish) */
//
//        /* Process all the messages before we add this connection again.
//           This just makes the UI more responsive */
//        UI_ProcessAllPendingUIEvents();
//
//        FlagNewDataAvailable(ConnectionID);
        RetValue=true;
    }

    Con_SetActiveConnection(NULL);

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    Connection::HandleMiddleMousePress
 *
 * SYNOPSIS:
 *    void Connection::HandleMiddleMousePress(int x,int y);
 *
 * PARAMETERS:
 *    x [I] -- The x pos of the mouse
 *    y [I] -- The y pos of the mouse
 *
 * FUNCTION:
 *    This function handles the pressing of the middle button.  It copies to
 *    the clipboard.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::HandleMiddleMousePress(int x,int y)
{
    string ClipText;

    if(IOHandle==NULL)
        return;

    UI_GetClipboardText(ClipText,e_Clipboard_Selection);

    /* See if we have anything to do */
    if(ClipText=="")
        return;

    /* Send the text out the connection */
    WriteData((const uint8_t *)ClipText.c_str(),ClipText.length(),
            e_ConWriteSource_Paste);
}

/*******************************************************************************
 * NAME:
 *    Connection::WriteChar2Display
 *
 * SYNOPSIS:
 *    void Connection::WriteChar2Display(uint8_t *Chr);
 *
 * PARAMETERS:
 *    Chr [I] -- The char to write (this is UTF8).  0 Term'ed
 *
 * FUNCTION:
 *    This function adds a char to the display buffer for this connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::InsertString()
 ******************************************************************************/
void Connection::WriteChar2Display(uint8_t *Chr)
{
    if(Display==nullptr)
        return;

    Display->WriteChar(Chr);
}

/*******************************************************************************
 * NAME:
 *    Connection::InsertString
 *
 * SYNOPSIS:
 *    bool Connection::InsertString(uint8_t *Str,uint32_t Len);
 *
 * PARAMETERS:
 *    Str [I] -- The UTF8 string to insert
 *    Len [I] -- The number of bytes in the string
 *
 * FUNCTION:
 *    This function adds a UTF8 string to the display of this connection.
 *
 * RETURNS:
 *    true -- String was added
 *    false -- There was an error adding the string.
 *
 * SEE ALSO:
 *    Connection::WriteChar2Display()
 ******************************************************************************/
bool Connection::InsertString(uint8_t *Str,uint32_t Len)
{
/* DEBUG PAUL: walk the string breaking it up into UTF8 chars and calling
   Display->WriteChar() */
return false;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetFGColor
 *
 * SYNOPSIS:
 *    void Connection::SetFGColor(uint32_t FGColor);
 *
 * PARAMETERS:
 *    FGColor [I] -- The new forground color in the format 0xRRGGBB
 *
 * FUNCTION:
 *    This function sets the forground color.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::GetFGColor()
 ******************************************************************************/
void Connection::SetFGColor(uint32_t FGColor)
{
    if(Display!=NULL)
        Display->CurrentStyle.FGColor=FGColor;
}

/*******************************************************************************
 * NAME:
 *    Connection::GetFGColor
 *
 * SYNOPSIS:
 *    uint32_t Connection::GetFGColor(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the forground color.
 *
 * RETURNS:
 *    The forgound color in 0xRRGGBB format.
 *
 * SEE ALSO:
 *    Connection::SetFGColor()
 ******************************************************************************/
uint32_t Connection::GetFGColor(void)
{
    if(Display!=NULL)
        return Display->CurrentStyle.FGColor;
    return 0;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetBGColor
 *
 * SYNOPSIS:
 *    void Connection::SetBGColor(uint32_t BGColor);
 *
 * PARAMETERS:
 *    BGColor [I] -- The new background color in the format 0xRRGGBB
 *
 * FUNCTION:
 *    This function sets the background color.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::GetBGColor()
 ******************************************************************************/
void Connection::SetBGColor(uint32_t BGColor)
{
    if(Display!=NULL)
        Display->CurrentStyle.BGColor=BGColor;
}

/*******************************************************************************
 * NAME:
 *    Connection::GetBGColor
 *
 * SYNOPSIS:
 *    uint32_t Connection::GetBGColor(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the background color.
 *
 * RETURNS:
 *    The background color in 0xRRGGBB format.
 *
 * SEE ALSO:
 *    Connection::SetBGColor()
 ******************************************************************************/
uint32_t Connection::GetBGColor(void)
{
    if(Display!=NULL)
        return Display->CurrentStyle.BGColor;
    return 0;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetULineColor
 *
 * SYNOPSIS:
 *    void Connection::SetULineColor(uint32_t ULineColor);
 *
 * PARAMETERS:
 *    ULineColor [I] -- The new underline color in the format 0xRRGGBB
 *
 * FUNCTION:
 *    This function sets the underline color.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::GetULineColor()
 ******************************************************************************/
void Connection::SetULineColor(uint32_t ULineColor)
{
    if(Display!=NULL)
        Display->CurrentStyle.ULineColor=ULineColor;
}

/*******************************************************************************
 * NAME:
 *    Connection::GetULineColor
 *
 * SYNOPSIS:
 *    uint32_t Connection::GetULineColor(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the underline color.
 *
 * RETURNS:
 *    The underline color in 0xRRGGBB format.
 *
 * SEE ALSO:
 *    Connection::SetULineColor()
 ******************************************************************************/
uint32_t Connection::GetULineColor(void)
{
    if(Display!=NULL)
        return Display->CurrentStyle.ULineColor;
    return 0;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetAttribs
 *
 * SYNOPSIS:
 *    void Connection::SetAttribs(uint16_t Attribs);
 *
 * PARAMETERS:
 *    Attribs [I] -- The new attribs to use.  These are bit values where
 *                   the following bits are supported:
 *                      TXT_ATTRIB_UNDERLINE -- Underline the text
 *                      TXT_ATTRIB_UNDERLINE_DOUBLE -- Double underline the text
 *                      TXT_ATTRIB_UNDERLINE_DOTTED -- Dotted underline
 *                      TXT_ATTRIB_UNDERLINE_DASHED -- Dashed underline
 *                      TXT_ATTRIB_UNDERLINE_WAVY -- A wavy underline
 *                      TXT_ATTRIB_OVERLINE -- Put a line over the text
 *                      TXT_ATTRIB_LINETHROUGHT -- Put a line though the text
 *                      TXT_ATTRIB_BOLD -- Bold Text
 *                      TXT_ATTRIB_ITALIC -- Italic Text
 *                      TXT_ATTRIB_OUTLINE -- Draw an outline around the leters.
 *
 * FUNCTION:
 *    This function sets the underline color.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::GetAttribs()
 ******************************************************************************/
void Connection::SetAttribs(uint16_t Attribs)
{
    if(Display!=NULL)
        Display->CurrentStyle.Attribs=Attribs;
}

/*******************************************************************************
 * NAME:
 *    Connection::GetAttribs
 *
 * SYNOPSIS:
 *    uint16_t Connection::GetAttribs(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the currently applied attribs.  These are bit values.
 *
 * RETURNS:
 *    The attribs that are currently turned on.
 *
 * SEE ALSO:
 *    Connection::SetAttribs()
 ******************************************************************************/
uint16_t Connection::GetAttribs(void)
{
    if(Display!=NULL)
        return Display->CurrentStyle.Attribs;
    return 0;
}

/*******************************************************************************
 * NAME:
 *    Connection::DoFunction
 *
 * SYNOPSIS:
 *    void Connection::DoFunction(e_ConFuncType Fn,uintptr_t Arg1,
 *              uintptr_t Arg2,uintptr_t Arg3,uintptr_t Arg4);
 *
 * PARAMETERS:
 *    Fn [I] -- The function to do.  See the functions in DataProcessSystem()
 *    Arg1 [I] -- Depends on 'Fn'
 *    Arg2 [I] -- Depends on 'Fn'
 *    Arg3 [I] -- Depends on 'Fn'
 *    Arg4 [I] -- Depends on 'Fn'
 *
 * FUNCTION:
 *    This function a function on the connection.  See DataProcessSystem()
 *    for a description of the functions (there is a different function for
 *    each one)
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::DoFunction(e_ConFuncType Fn,uintptr_t Arg1,uintptr_t Arg2,
        uintptr_t Arg3,uintptr_t Arg4)
{
    if(Display==NULL)
        return;

    switch(Fn)
    {
        case e_ConFunc_NewLine:
            Display->DoLineFeed();
        break;
        case e_ConFunc_Return:
            Display->DoReturn();
        break;
        case e_ConFunc_Backspace:
            Display->DoBackspace();
        break;
        case e_ConFunc_MoveCursor:
            Display->SetCursorXY(Arg1,Arg2);
        break;
        case e_ConFunc_ClearScreen:
//            Display_ClearScreen(DB,NULL);
        break;
        case e_ConFunc_ClearArea:
//            Display_ClearArea(DB,NULL,Arg1,Arg2,Arg3,Arg4);
        break;
        case e_ConFunc_Tab:
            Display->AddTab();
        break;
        case e_ConFunc_NoteNonPrintable:
            Display->NoteNonPrintable((const char *)Arg1);
        break;

        case e_ConFuncMAX:
        default:
        break;
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::GetCursorXY
 *
 * SYNOPSIS:
 *    void Connection::GetCursorXY(int *RetCursorX,int *RetCursorY);
 *
 * PARAMETERS:
 *    RetCursorX [O] -- The cursor X pos
 *    RetCursorY [O] -- The cursor Y pos
 *
 * FUNCTION:
 *    This function gets the X and Y position of the cursor relative to the
 *    upper left of the screen.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    If there is no display set for this connect then this returns 0.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::GetCursorXY(int *RetCursorX,int *RetCursorY)
{
    if(Display==nullptr)
    {
        *RetCursorX=0;
        *RetCursorY=0;
        return;
    }

    Display->GetCursorXY((unsigned int *)RetCursorX,(unsigned int *)RetCursorY);
}

void Connection::GetScreenSize(int32_t *RetRows,int32_t *RetColumns)
{
//    if(DB!=NULL)
//        DB->GetBufferSize(*RetRows,*RetColumns);
}

/*******************************************************************************
 * NAME:
 *    Connection::SetConnectedState
 *
 * SYNOPSIS:
 *    void Connection::SetConnectedState(bool Connect);
 *
 * PARAMETERS:
 *    Connect [I] -- True=connect, false=disconnect.
 *
 * FUNCTION:
 *    This function tries to change the connected status of a connection.
 *    It tells the driver to change the state and if it changes the
 *    driver will send a new connection state up.
 *
 *    DEBUG PAUL: This should really return an error if it didn't work so we
 *                can tell the user that it didn't work.
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::SetConnectedState(bool Connect)
{
    if(IOHandle==NULL)
        return;

    if(Connect)
    {
        IOS_Open(IOHandle);
    }
    else
    {
        if(IsConnected)
            IOS_Close(IOHandle);
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::ToggleConnectedState
 *
 * SYNOPSIS:
 *    void Connection::ToggleConnectedState(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function toggles to the connect / disconnect state.  It calls
 *    SetConnectedState() to change the state.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::SetConnectedState()
 ******************************************************************************/
void Connection::ToggleConnectedState(void)
{
    SetConnectedState(!IsConnected);
}

/*******************************************************************************
 * NAME:
 *    Connection::SendResizeEvent2Siblings
 *
 * SYNOPSIS:
 *    void Connection::SendResizeEvent2Siblings(int Width,int Height);
 *
 * PARAMETERS:
 *    Width [I] -- The new width of the text area canvas in pixels
 *    Height [I] -- The new height of the text area canvas in pixels
 *
 * FUNCTION:
 *    This function sends a TextCanvasResize() to all connections with the
 *    same main window as this connection.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    This is needed because the GUI doesn't send resize events to hidden
 *    text canvases (only to the active tab), but we still need to handle
 *    resizes on the hidden text canvases.
 *
 *    All the text canvases in the same window have the same size anyway so
 *    we just send this resize even to everyone.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::SendResizeEvent2Siblings(int Width,int Height)
{
    i_ConnectionListType Con;
    class Connection *ConPtr;

    for(Con=m_Connections.begin();Con!=m_Connections.end();Con++)
    {
        ConPtr=*Con;
        if(ConPtr->GetMainWindowHandle()==MW)
            ConPtr->TextCanvasResize(Width,Height);
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::GetMainWindowHandle
 *
 * SYNOPSIS:
 *    class TheMainWindow *Connection::GetMainWindowHandle(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the main window that this connection belongs to.
 *
 * RETURNS:
 *    A pointer to the main window that this connection lives in.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
class TheMainWindow *Connection::GetMainWindowHandle(void)
{
    return MW;
}

/*******************************************************************************
 * NAME:
 *    Connection::GiveFocus
 *
 * SYNOPSIS:
 *    void Connection::GiveFocus(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gives this connection input focus.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::GiveFocus(void)
{
//    if(DB!=NULL)
//        DB->GiveFocus();
}

/*******************************************************************************
 * NAME:
 *    Connection::CopySelectionToClipboard
 *
 * SYNOPSIS:
 *    void Connection::CopySelectionToClipboard(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function copies the current selection to the clipboard (if there
 *    is one).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::CopySelectionToClipboard(void)
{
    string SelectContents;

    if(Display==NULL)
        return;

    if(Display->GetSelectionString(SelectContents))
    {
        UI_SetClipboardText(SelectContents,e_Clipboard_Selection);
        UI_SetClipboardText(SelectContents,e_Clipboard_Clipboard);
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::PasteFromClipboard
 *
 * SYNOPSIS:
 *    void Connection::PasteFromClipboard(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function inserts the text from the clipboard into the input system.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::PasteFromClipboard(void)
{
    string ClipText;

    if(IOHandle==NULL)
        return;

    UI_GetClipboardText(ClipText,e_Clipboard_Clipboard);
    if(ClipText=="")
    {
        /* Try the selection clip instead */
        UI_GetClipboardText(ClipText,e_Clipboard_Selection);
    }

    /* See if we have anything to do */
    if(ClipText=="")
        return;

    /* Send the text out the connection */
    WriteData((const uint8_t *)ClipText.c_str(),ClipText.length(),
            e_ConWriteSource_Paste);
}

/*******************************************************************************
 * NAME:
 *    Connection::GetConnectionUniqueID
 *
 * SYNOPSIS:
 *    void Connection::GetConnectionUniqueID(std::string &UniqueID);
 *
 * PARAMETERS:
 *    UniqueID [O] -- The UniqueID for this connection
 *
 * FUNCTION:
 *    This function gets the uniqueID for the connection from the IO system.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::GetConnectionUniqueID(std::string &UniqueID)
{
    if(IOHandle==NULL)
        return;

    IOS_GetUniqueID(IOHandle,UniqueID);
}

///*******************************************************************************
// * NAME:
// *    Connection::ReopenConnectionBasedOnURI
// *
// * SYNOPSIS:
// *    bool Connection::ReopenConnectionBasedOnURI(const char *URI);
// *
// * PARAMETERS:
// *    URI [I] -- The URI to open the connection for.
// *
// * FUNCTION:
// *    This function opens a new connection based on a URI.  It will close
// *    and free all the resources of the existing URI (except the UI tab).
// *
// * RETURNS:
// *    true -- Things worked out
// *    false -- There was an error
// *
// * SEE ALSO:
// *    
// ******************************************************************************/
//bool Connection::ReopenConnectionBasedOnURI(const char *URI)
//{
//    string ConnectionName;
//    const char *p;
//    bool RetValue;
//    string UniqueID;
//    struct ConnectionInfoList ConInfo;
//
//    //` DEBUG PAUL: This needs to be reworked as I am not sure it handles processors correctly (and should be calling FinishInit())
//    RetValue=false;
//    try
//    {
//        FreeConnectionResources(false);
//
//        IOHandle=IOS_AllocIOSystemHandleFromURI(URI,(uintptr_t)this);
//        if(IOHandle==NULL)
//            return false;
//
//        p=URI;
//        while(*p!=':' && *p!=0)
//            p++;
//        if(*p==0)
//            return false;
//        ConnectionName.assign(URI,p-URI);
//
//        if(IOS_GetUniqueIDFromURI(URI,UniqueID))
//            if(IOS_GetConnectionInfo(UniqueID.c_str(),NULL,&ConInfo))
//                ConnectionName=ConInfo.ShortName;
//
//        SetDisplayName(ConnectionName.c_str());
//
//        /* Clear the display buffer */
////        DB->ResetBuffer();
//
//        /* We auto open this when we open the tab */
//        if(g_Settings.AutoConnectOnNewConnection)
//            IOS_Open(IOHandle);
//
//        SendMWEvent(ConMWEvent_StatusChange);
//        RetValue=true;
//    }
//    catch(...)
//    {
//        RetValue=false;
//    }
//
//    return RetValue;
//}

/*******************************************************************************
 * NAME:
 *    Connection::GetURI
 *
 * SYNOPSIS:
 *    bool Connection::GetURI(std::string &URI);
 *
 * PARAMETERS:
 *    URI [O] -- The URI for this connection.
 *
 * FUNCTION:
 *    This function will build a URI string for the current connection.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Connection::GetURI(std::string &URI)
{
    if(IOHandle==NULL)
        return false;

    return IOS_GetDeviceURI(IOHandle,URI);
}

/*******************************************************************************
 * NAME:
 *    Connection::ProcessDisplayEvent
 *
 * SYNOPSIS:
 *    bool Connection::ProcessDisplayEvent(const struct DBEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event to process
 *
 * FUNCTION:
 *    This is the event handler for the text canvas.
 *
 * RETURNS:
 *    true -- The event should act normally
 *    false -- There event should be canceled
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Connection::ProcessDisplayEvent(const struct DBEvent *Event)
{
    bool AcceptEvent;

    AcceptEvent=true;
    switch(Event->EventType)
    {
        case e_DBEvent_MouseMiddlePress:
            HandleMiddleMousePress(Event->Info->Mouse.x,Event->Info->Mouse.y);
        break;
        case e_DBEvent_SelectionChanged:
            if(Display!=NULL)
            {
                string SelectContents;

                if(Display->GetSelectionString(SelectContents))
                    UI_SetClipboardText(SelectContents,e_Clipboard_Selection);
            }
        break;
        case e_DBEvent_KeyEvent:
            AcceptEvent=KeyPress(Event->Info->Key.Mods,Event->Info->Key.Key,
                    Event->Info->Key.TextPtr,Event->Info->Key.TextLen);
        break;
        case e_DBEvent_SendBlockData:
            if(WriteData((uint8_t *)Event->Info->BlockSend.Buffer,
                    Event->Info->BlockSend.Len,e_ConWriteSource_BlockSend)!=
                    e_ConWrite_Success)
            {
                UIAsk("Error","Failed to send.  Write error",
                        e_AskBox_Error,e_AskBttns_Ok);
            }
            /* Send the data we just queued */
            TransmitQueuedData();
        break;
        case e_DBEvent_FocusChange:
            RethinkCursor();
        break;
        case e_DBEventMAX:
        default:
        break;
    }
    return AcceptEvent;
}

/*******************************************************************************
 * NAME:
 *    Connection::GetCaptureOptions
 *
 * SYNOPSIS:
 *    void Connection::GetCaptureOptions(struct CaptureToFileOptions &Options);
 *
 * PARAMETERS:
 *    Options [O] -- The current capture options for this connection.
 *
 * FUNCTION:
 *    This function gets the current options for the capture to file.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::GetCaptureOptions(struct CaptureToFileOptions &Options)
{
    Options=CaptureToFile.Options;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetCaptureOption_Timestamp
 *
 * SYNOPSIS:
 *    void Connection::SetCaptureOption_Timestamp(bool On);
 *
 * PARAMETERS:
 *    On [I] -- Turn it on (true) or off (false)
 *
 * FUNCTION:
 *    This function changes the setting of the capture to file timestamp option.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::SetCaptureOption_Timestamp(bool On)
{
    if(CaptureToFile.WriteHandle==NULL)
        CaptureToFile.Options.Timestamp=On;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetCaptureOption_Append
 *
 * SYNOPSIS:
 *    void Connection::SetCaptureOption_Append(bool On);
 *
 * PARAMETERS:
 *    On [I] -- Turn it on (true) or off (false)
 *
 * FUNCTION:
 *    This function changes the setting of the capture to file append option.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::SetCaptureOption_Append(bool On)
{
    if(CaptureToFile.WriteHandle==NULL)
        CaptureToFile.Options.Append=On;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetCaptureOption_StripCtrl
 *
 * SYNOPSIS:
 *    void Connection::SetCaptureOption_StripCtrl(bool On);
 *
 * PARAMETERS:
 *    On [I] -- Turn it on (true) or off (false)
 *
 * FUNCTION:
 *    This function changes the setting of the capture to file strip ctrl
 *    option.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::SetCaptureOption_StripCtrl(bool On)
{
    if(CaptureToFile.WriteHandle==NULL)
        CaptureToFile.Options.StripCtrl=On;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetCaptureOption_StripEsc
 *
 * SYNOPSIS:
 *    void Connection::SetCaptureOption_StripEsc(bool On);
 *
 * PARAMETERS:
 *    On [I] -- Turn it on (true) or off (false)
 *
 * FUNCTION:
 *    This function changes the setting of the capture to file strip esc seq
 *    option.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::SetCaptureOption_StripEsc(bool On)
{
    if(CaptureToFile.WriteHandle==NULL)
        CaptureToFile.Options.StripEsc=On;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetCaptureOption_HexDump
 *
 * SYNOPSIS:
 *    void Connection::SetCaptureOption_HexDump(bool On);
 *
 * PARAMETERS:
 *    On [I] -- Turn it on (true) or off (false)
 *
 * FUNCTION:
 *    This function changes the setting of the capture to file hex dump
 *    option.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::SetCaptureOption_HexDump(bool On)
{
    if(CaptureToFile.WriteHandle==NULL)
        CaptureToFile.Options.SaveAsHexDump=On;
}

/*******************************************************************************
 * NAME:
 *    Connection::GetCaptureSaving
 *
 * SYNOPSIS:
 *    bool Connection::GetCaptureSaving(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function returns if the capture to file system is currently saving
 *    incoming data to the disk.
 *
 * RETURNS:
 *    true -- We are saving.
 *    false -- We are not saving.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Connection::GetCaptureSaving(void)
{
    return CaptureToFile.WriteHandle!=NULL;
}

/*******************************************************************************
 * NAME:
 *    Connection::StartCapture
 *
 * SYNOPSIS:
 *    bool Connection::StartCapture(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function opens the capture save file and starts saving all incoming
 *    data to the file.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error.  We could not open the file.
 *
 * SEE ALSO:
 *    Connection::StopCapture()
 ******************************************************************************/
bool Connection::StartCapture(void)
{
    const char *OpenMode;
    time_t curtime;
    char buff[100];

    if(CaptureToFile.WriteHandle!=NULL)
        fclose(CaptureToFile.WriteHandle);

    if(CaptureToFile.Filename=="")
        return false;

    if(CaptureToFile.Options.Append)
        OpenMode="ab";
    else
        OpenMode="wb";

    CaptureToFile.WriteHandle=fopen(CaptureToFile.Filename.c_str(),OpenMode);
    if(CaptureToFile.WriteHandle==NULL)
        return false;

    if(CaptureToFile.Options.SaveAsHexDump)
    {
        sprintf(buff,"%08X:",0);
        fwrite(buff,8+1,1,CaptureToFile.WriteHandle);   // 8 hex + ':'
    }
    else
    {
        if(CaptureToFile.Options.Timestamp)
        {
            /* Now output a timestamp */
            time(&curtime);
            strcpy(buff,ctime(&curtime));
            buff[24]=':';
            fwrite(buff,strlen(buff),1,CaptureToFile.WriteHandle);
        }
    }

    CaptureToFile.EscSeqSkiping=false;
    CaptureToFile.HexDumpInsertPos=0;
    CaptureToFile.HexDumpOffset=0;

    return true;
}

/*******************************************************************************
 * NAME:
 *    Connection::StopCapture
 *
 * SYNOPSIS:
 *    void Connection::StopCapture(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function stops the capture to file function.  It closes the
 *    save file handle.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::StartCapture()
 ******************************************************************************/
void Connection::StopCapture(void)
{
    char buff[100];
    int r;

    if(CaptureToFile.WriteHandle!=NULL)
    {
        if(CaptureToFile.Options.SaveAsHexDump)
        {
            /* Finish off the hex dump */

            /* Fill in the hex part with spaces */
            strcpy(buff,"   ");
            for(r=CaptureToFile.HexDumpInsertPos;
                    r<CAPTURE_HEXDUMP_VALUES_PER_LINE;r++)
            {
                fwrite(buff,3,1,CaptureToFile.WriteHandle);
            }

            /* Write the AscII preview */
            fwrite("   ",3,1,CaptureToFile.WriteHandle);
            fwrite(CaptureToFile.HexDumpBuff,CaptureToFile.HexDumpInsertPos,
                    1,CaptureToFile.WriteHandle);
            fwrite("\n",1,1,CaptureToFile.WriteHandle);
        }

        fclose(CaptureToFile.WriteHandle);
    }
    CaptureToFile.WriteHandle=NULL;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetCaptureFilename
 *
 * SYNOPSIS:
 *    void Connection::SetCaptureFilename(const char *Filename);
 *
 * PARAMETERS:
 *    Filename [I] -- The filename to save to
 *
 * FUNCTION:
 *    This function sets the filename that the capture to file will save to.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::GetCaptureFilename()
 ******************************************************************************/
void Connection::SetCaptureFilename(const char *Filename)
{
    try
    {
        CaptureToFile.Filename=Filename;
    }
    catch(...)
    {
        CaptureToFile.Filename="";
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::GetCaptureFilename
 *
 * SYNOPSIS:
 *    void Connection::GetCaptureFilename(std::string &Filename);
 *
 * PARAMETERS:
 *    Filename [O] -- The filename we are going to save to
 *
 * FUNCTION:
 *    This function gets the filename the capture will save to.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::SetCaptureFilename()
 ******************************************************************************/
void Connection::GetCaptureFilename(std::string &Filename)
{
    try
    {
        Filename=CaptureToFile.Filename;
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::HandleCaptureIncomingData
 *
 * SYNOPSIS:
 *    void Connection::HandleCaptureIncomingData(const uint8_t *Inbuff,
 *              int bytes);
 *
 * PARAMETERS:
 *    InBuff [I] -- The bytes we just read in
 *    bytes [I] -- The number of bytes we just read in
 *
 * FUNCTION:
 *    This function handles saving incoming data to the capture system.
 *    It will strip things and make adjustments based on capture options.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::HandleCaptureIncomingData(const uint8_t *Inbuff,int bytes)
{
    const uint8_t *Pos;
    const uint8_t *EndPos;
    const uint8_t *LastStart;
    char buff[100];
    time_t curtime;

    /* Check if we are actively saving */
    if(CaptureToFile.WriteHandle==NULL)
        return;

    Pos=Inbuff;
    EndPos=Pos+bytes;

    if(CaptureToFile.Options.SaveAsHexDump)
    {
        for(;Pos<EndPos;Pos++)
        {
            CaptureToFile.HexDumpOffset++;

            sprintf(buff,"%02X ",*Pos);
            fwrite(buff,3,1,CaptureToFile.WriteHandle);
            CaptureToFile.HexDumpBuff[CaptureToFile.HexDumpInsertPos++]=
                    (*Pos<32 || *Pos>126)?'.':*Pos;
            if(CaptureToFile.HexDumpInsertPos>=CAPTURE_HEXDUMP_VALUES_PER_LINE)
            {
                /* Write out the AscII preview */
                fwrite("   ",3,1,CaptureToFile.WriteHandle);
                fwrite(CaptureToFile.HexDumpBuff,
                        CAPTURE_HEXDUMP_VALUES_PER_LINE,1,
                        CaptureToFile.WriteHandle);

                sprintf(buff,"\n%08X:",CaptureToFile.HexDumpOffset);
                fwrite(buff,8+1+1,1,CaptureToFile.WriteHandle); // 8 hex + \n + ':'

                CaptureToFile.HexDumpInsertPos=0;
            }
        }
    }
    else
    {
        /* Handle options in blocks */
        LastStart=Pos;
        for(;Pos<EndPos;Pos++)
        {
            if(CaptureToFile.Options.Timestamp)
            {
                if(*Pos=='\n')
                {
                    /* Output the block and then a timestamp */
                    fwrite(LastStart,Pos-LastStart+1,1,
                            CaptureToFile.WriteHandle);
                    /* Now output a timestamp */
                    time(&curtime);
                    strcpy(buff,ctime(&curtime));
                    buff[24]=':';
                    fwrite(buff,strlen(buff),1,CaptureToFile.WriteHandle);
                    LastStart=Pos+1;
                    continue;
                }
            }

            if(CaptureToFile.Options.StripEsc)
            {
                if(!CaptureToFile.EscSeqSkiping)
                {
                    /* We are looking for esc char */
                    if(*Pos==27)    // 27 esc
                    {
                        /* Output the block and skip this char */
                        fwrite(LastStart,Pos-LastStart,1,
                                CaptureToFile.WriteHandle);
                        CaptureToFile.EscSeqSkiping=true;
                        LastStart=Pos+1;
                        continue;
                    }
                }
                else
                {
                    /* We strip everything until we see a letter (upper or
                       lower) or another ESC */
                    if(*Pos==27 ||
                            (*Pos>='a' && *Pos<='z') ||
                            (*Pos>='A' && *Pos<='Z'))
                    {
                        /* Ok, exit this mode */
                        CaptureToFile.EscSeqSkiping=false;

                        /* If we hit an ESC ESC we want to output both */
                        if(*Pos==27)
                        {
                            buff[0]=27;
                            buff[1]=27;
                            fwrite(buff,2,1,CaptureToFile.WriteHandle);
                        }
                    }
                    /* Skip this char */
                    LastStart=Pos+1;
                    continue;
                }
            }

            if(CaptureToFile.Options.StripCtrl)
            {
                if(*Pos<32 && *Pos!='\n' && *Pos!='\r')
                {
                    /* Output the block and skip this char */
                    fwrite(LastStart,Pos-LastStart,1,CaptureToFile.WriteHandle);
                    LastStart=Pos+1;
                    continue;
                }
            }
        }
        if(LastStart!=Pos)
        {
            fwrite(LastStart,Pos-LastStart,1,CaptureToFile.WriteHandle);
        }
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::SendMWEvent
 *
 * SYNOPSIS:
 *    void Connection::SendMWEvent(ConMWEventType Event,
 *              union ConMWInfo *ExtraInfo);
 *
 * PARAMETERS:
 *    Event [I] -- The event type to send
 *    ExtraInfo [I] -- Extra info for this event.  NULL for none.
 *
 * FUNCTION:
 *    This function sends a connection event to the main window.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::SendMWEvent(ConMWEventType Event,union ConMWInfo *ExtraInfo)
{
    struct ConMWEvent NewEvent;

    if(MW==NULL)
        return;

    NewEvent.EventType=Event;
    NewEvent.Con=this;
    NewEvent.Info=ExtraInfo;

    MW->ConnectionEvent(&NewEvent);
}

/*******************************************************************************
 * NAME:
 *    Connection::GetStopWatchOptions
 *
 * SYNOPSIS:
 *    void Connection::GetStopWatchOptions(bool &AutoStartOn,bool &AutoLapOn);
 *
 * PARAMETERS:
 *    AutoStartOn [O] -- The auto start option
 *    AutoLapOn [O] -- The auto lap option
 *
 * FUNCTION:
 *    This function gets the status of the options for the stopwatch.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::GetStopWatchOptions(bool &AutoStartOn,bool &AutoLapOn)
{
    AutoStartOn=StopWatch.AutoStartOnTx;
    AutoLapOn=StopWatch.AutoLap;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetStopWatchOptions
 *
 * SYNOPSIS:
 *    void Connection::SetStopWatchOptions(bool AutoStartOn,bool AutoLapOn);
 *
 * PARAMETERS:
 *    AutoStartOn [I] -- What to set the auto start to
 *    AutoLapOn [I] -- What to set the auto lap to
 *
 * FUNCTION:
 *    This function changes the options for the stop watch
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::SetStopWatchOptions(bool AutoStartOn,bool AutoLapOn)
{
    StopWatch.AutoStartOnTx=AutoStartOn;
    StopWatch.AutoLap=AutoLapOn;
}

/*******************************************************************************
 * NAME:
 *    Connection::GetStopWatchRunning
 *
 * SYNOPSIS:
 *    bool Connection::GetStopWatchRunning(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets if the stop watch is currently running or not.
 *
 * RETURNS:
 *    true -- Stop watch is counting
 *    false -- Stop watch is stopped
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Connection::GetStopWatchRunning(void)
{
    return StopWatch.Running;
}

/*******************************************************************************
 * NAME:
 *    Connection::StopWatchStartStop
 *
 * SYNOPSIS:
 *    void Connection::StopWatchStartStop(bool Start);
 *
 * PARAMETERS:
 *    Start [I] -- Do we start (true) or stop (false) the stop watch.
 *
 * FUNCTION:
 *    This function starts/stops the stopwatch.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::StopWatchStartStop(bool Start)
{
    uint64_t PauseOffset;

    if(Start)
    {
        PauseOffset=StopWatch.StopTime-StopWatch.StartTime;

        StopWatch.StopTime=GetMSCounter();
        StopWatch.StartTime=StopWatch.StopTime-PauseOffset;

        StopWatch.Running=true;

        StopWatch.LastRxDataTime=GetMSCounter();
        StopWatch.LastLapTime=StopWatch.StartTime;
    }
    else
    {
        StopWatch.Running=false;

        StopWatch.StopTime=GetMSCounter();
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::StopWatchReset
 *
 * SYNOPSIS:
 *    void Connection::StopWatchReset(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function resets the stop watch.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::StopWatchReset(void)
{
    StopWatch.StartTime=GetMSCounter();
    StopWatch.StopTime=StopWatch.StartTime;

    StopWatch.LastRxDataTime=GetMSCounter();
    StopWatch.LastLapTime=StopWatch.StartTime;
}

/*******************************************************************************
 * NAME:
 *    Connection::StopWatchGetTime
 *
 * SYNOPSIS:
 *    uint64_t Connection::StopWatchGetTime(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the time that has passed in the stop watch.
 *
 * RETURNS:
 *    The delta of start and stop times.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
uint64_t Connection::StopWatchGetTime(void)
{
    /* Update stop time if we are running */
    if(StopWatch.Running)
        StopWatch.StopTime=GetMSCounter();

    return StopWatch.StopTime-StopWatch.StartTime;
}

/*******************************************************************************
 * NAME:
 *    Connection::StopWatchTakeLap
 *
 * SYNOPSIS:
 *    void Connection::StopWatchTakeLap(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function adds a lap counter to the list of laps.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    This function will send off a number of events.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::StopWatchTakeLap(void)
{
    uint64_t LapTime;
    uint64_t LapDelta;
    union ConMWInfo Info;

    /* Grab the latest */
    if(StopWatch.Running)
        StopWatch.StopTime=GetMSCounter();

    if(StopWatch.Laps.empty())
        StopWatch.LastLapTime=StopWatch.StopTime;

    /* How long has it been since we started */
    LapTime=StopWatch.StopTime-StopWatch.StartTime;

    /* How long has it been since the last lap */
    LapDelta=StopWatch.StopTime-StopWatch.LastLapTime;

    StopWatch.LastLapTime=StopWatch.StopTime;

    /* Log it */
    StopWatch.Laps.push_back(LapTime);

    /* Send an event */
    Info.SW.LapTime=LapTime;
    Info.SW.LapDelta=LapDelta;
    SendMWEvent(ConMWEvent_StopWatchAddLap,&Info);
}

/*******************************************************************************
 * NAME:
 *    Connection::StopWatchHandleAutoLap
 *
 * SYNOPSIS:
 *    void Connection::StopWatchHandleAutoLap(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    Handles the auto lap functionally.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::StopWatchHandleAutoLap(void)
{
    if(!StopWatch.Running || !StopWatch.AutoLap)
        return;

    if(GetMSCounter()-StopWatch.LastRxDataTime>=AUTOLAP_TIMEOUT)
        StopWatchTakeLap();

    StopWatch.LastRxDataTime=GetMSCounter();
}

/*******************************************************************************
 * NAME:
 *    Connection::StopWatchClearLaps
 *
 * SYNOPSIS:
 *    void Connection::StopWatchClearLaps(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clears the stop watch lap log.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::StopWatchClearLaps(void)
{
    StopWatch.Laps.clear();
}

/*******************************************************************************
 * NAME:
 *    Connection::StopWatchGetNextLapInfo
 *
 * SYNOPSIS:
 *    bool Connection::StopWatchGetNextLapInfo(bool First,uint64_t &LapTime);
 *
 * PARAMETERS:
 *    First [I] -- Start from the first entry
 *    LapTime [O] -- The time that was recorded when the lap was taken
 *
 * FUNCTION:
 *    This function gets the next lap info from the list of saved laps.
 *
 * RETURNS:
 *    true -- This info is valid
 *    false -- There is no more entries and this one is not valid.
 *
 * SEE ALSO:
 *    Connection::StopWatchStartGettingLapInfo()
 ******************************************************************************/
bool Connection::StopWatchGetNextLapInfo(bool First,uint64_t &LapTime)
{
    if(First)
        StopWatch.CurrentLapInfoPoint=StopWatch.Laps.begin();

    if(StopWatch.CurrentLapInfoPoint==StopWatch.Laps.end())
        return false;

    LapTime=*StopWatch.CurrentLapInfoPoint;
    StopWatch.CurrentLapInfoPoint++;

    return true;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetUploadFilename
 *
 * SYNOPSIS:
 *    void Connection::SetUploadFilename(const char *Filename);
 *
 * PARAMETERS:
 *    Filename [I] -- The filename to send
 *
 * FUNCTION:
 *    This function sets the filename that the upload will try to upload.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::GetUploadFilename()
 ******************************************************************************/
void Connection::SetUploadFilename(const char *Filename)
{
    try
    {
        Upload.Filename=Filename;
    }
    catch(...)
    {
        Upload.Filename="";
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::GetUploadFilename
 *
 * SYNOPSIS:
 *    void Connection::GetUploadFilename(std::string &Filename);
 *
 * PARAMETERS:
 *    Filename [O] -- The filename we are going to try to upload
 *
 * FUNCTION:
 *    This function gets the filename the upload will try to send.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::SetUploadFilename()
 ******************************************************************************/
void Connection::GetUploadFilename(std::string &Filename)
{
    try
    {
        Filename=Upload.Filename;
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::SetUploadProtocol
 *
 * SYNOPSIS:
 *    void Connection::SetUploadProtocol(int NewProtocol);
 *
 * PARAMETERS:
 *    NewProtocol [I] -- The new protocol to use for uploads
 *
 * FUNCTION:
 *    This function sets the protocol that will be used for uploads.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::GetUploadProtocol()
 ******************************************************************************/
void Connection::SetUploadProtocol(const char *NewProtocol)
{
    try
    {
        Upload.ProtocolID=NewProtocol;
    }
    catch(...)
    {
        Upload.ProtocolID="";
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::GetUploadProtocol
 *
 * SYNOPSIS:
 *    void Connection::GetUploadProtocol(std::string &SelectedProtocol);
 *
 * PARAMETERS:
 *    SelectedProtocol [O] -- The protocol ID that is selected.
 *
 * FUNCTION:
 *    This function gets the current upload protocol that is in use.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::GetUploadProtocol(std::string &SelectedProtocol)
{
    try
    {
        SelectedProtocol=Upload.ProtocolID;
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::GetUploadOptionsPtr
 *
 * SYNOPSIS:
 *    t_KVList *Connection::GetUploadOptionsPtr(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function returns a pointer to the internal KVList of options for
 *    the file upload.
 *
 * RETURNS:
 *    A pointer to the KVList of options.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_KVList *Connection::GetUploadOptionsPtr(void)
{
    return &Upload.UploadOptions;
}

/*******************************************************************************
 * NAME:
 *    Connection::StartUpload
 *
 * SYNOPSIS:
 *    e_FileTransErrType Connection::StartUpload(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called to start an upload.  It will take the settings
 *    out of the 'Upload' member variable.
 *
 * RETURNS:
 *    See Connection::StartUploadOfFile()
 *
 * SEE ALSO:
 *    Connection::StartUploadOfFile(), Connection::AbortUpload()
 ******************************************************************************/
e_FileTransErrType Connection::StartUpload(void)
{
    uint64_t FileSize;
    FILE *in;

    /* Collect some info about the file */
    in=fopen(Upload.Filename.c_str(),"rb");
    if(in==NULL)
        return e_FileTransErr_FileError;
    fseek(in,0,SEEK_END);
    FileSize=ftell(in);
    fclose(in);
    in=NULL;

    Upload.Stats.InProgress=true;   // Mark as started so if the upload file function sends the whole file it can clear the in progress
    Upload.Stats.BytesSent=0;
    Upload.Stats.TotalFileSize=FileSize;

    Upload.Timeout=0;   // Disable the timer until they turn it on

    /* Start the transfer */
    if(!FTPS_UploadFile(FTPConData,this,Upload.Filename.c_str(),
            Upload.ProtocolID.c_str(),Upload.UploadOptions,FileSize))
    {
        Upload.Stats.InProgress=false;
        return e_FileTransErr_ProtocolInitFail;
    }
    else
    {
        /* We need to check if we are still in progress because the FTP can
           do everything in the UploadFile() call and be done. */
        RethinkLockOut();
    }

    return e_FileTransErr_Success;
}

/*******************************************************************************
 * NAME:
 *    Connection::AbortUpload
 *
 * SYNOPSIS:
 *    void Connection::AbortUpload(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function aborts the current upload in progress.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::StartUpload()
 ******************************************************************************/
void Connection::AbortUpload(void)
{
    if(Upload.Stats.InProgress)
    {
        FTPS_AbortTransfer(FTPConData);
        FinishedUpload(true);
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::UploadGetStats
 *
 * SYNOPSIS:
 *    struct UploadStats const *Connection::UploadGetStats(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets info about the current upload.
 *
 * RETURNS:
 *    A pointer to the upload stats.  This is valid as long as the connection
 *    is valid.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
struct UploadStats const *Connection::UploadGetStats(void)
{
    return &Upload.Stats;
}

/*******************************************************************************
 * NAME:
 *    Connection::UploadSetNumberOfBytesSent
 *
 * SYNOPSIS:
 *    void Connection::UploadSetNumberOfBytesSent(uint64_t BytesTransfered);
 *
 * PARAMETERS:
 *    BytesTransfered [I] -- The number of bytes that have successfully been
 *          uploaded
 *
 * FUNCTION:
 *    This function is called from the FTP system to tell us progress has
 *    been made on the file upload.
 *
 *    It updates the stats for this upload.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::UploadSetNumberOfBytesSent(uint64_t BytesTransfered)
{
    Upload.Stats.BytesSent=BytesTransfered;

    /* Flag the UI for an update */
    SendMWEvent(ConMWEvent_UploadStatUpdate);
}

/*******************************************************************************
 * NAME:
 *    Connection::FinishedUpload
 *
 * SYNOPSIS:
 *    void Connection::FinishedUpload(bool Aborted);
 *
 * PARAMETERS:
 *    Aborted [I] -- Are we aborting or just ending
 *
 * FUNCTION:
 *    This function is called from the FTP system because the driver
 *    wants to stop a transfer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::FinishedUpload(bool Aborted)
{
    Upload.Stats.InProgress=false;
    Upload.Timeout=0;

    if(Aborted)
    {
        UploadSetNumberOfBytesSent(0);
        SendMWEvent(ConMWEvent_UploadAborted);
    }
    else
    {
        UploadSetNumberOfBytesSent(Upload.Stats.TotalFileSize);
        SendMWEvent(ConMWEvent_UploadDone);
    }

    RethinkLockOut();
}

/*******************************************************************************
 * NAME:
 *    Connection::SetDownloadProtocol
 *
 * SYNOPSIS:
 *    void Connection::SetDownloadProtocol(int NewProtocol);
 *
 * PARAMETERS:
 *    NewProtocol [I] -- The new protocol to use for uploads
 *
 * FUNCTION:
 *    This function sets the protocol that will be used for uploads.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::GetDownloadProtocol()
 ******************************************************************************/
void Connection::SetDownloadProtocol(const char *NewProtocol)
{
    try
    {
        Download.ProtocolID=NewProtocol;
    }
    catch(...)
    {
        Download.ProtocolID="";
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::GetDownloadProtocol
 *
 * SYNOPSIS:
 *    void Connection::GetDownloadProtocol(std::string &SelectedProtocol);
 *
 * PARAMETERS:
 *    SelectedProtocol [O] -- The protocol ID that is selected.
 *
 * FUNCTION:
 *    This function gets the current upload protocol that is in use.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::GetDownloadProtocol(std::string &SelectedProtocol)
{
    try
    {
        SelectedProtocol=Download.ProtocolID;
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::GetDownloadOptionsPtr
 *
 * SYNOPSIS:
 *    t_KVList *Connection::GetDownloadOptionsPtr(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function returns a pointer to the internal KVList of options for
 *    the file upload.
 *
 * RETURNS:
 *    A pointer to the KVList of options.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_KVList *Connection::GetDownloadOptionsPtr(void)
{
    return &Download.DownloadOptions;
}

/*******************************************************************************
 * NAME:
 *    Connection::StartDownload
 *
 * SYNOPSIS:
 *    e_FileTransErrType Connection::StartDownload(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called to start an upload.  It will take the settings
 *    out of the 'Download' member variable.
 *
 * RETURNS:
 *    See Connection::StartDownloadOfFile()
 *
 * SEE ALSO:
 *    Connection::StartDownloadOfFile(), Connection::AbortDownload()
 ******************************************************************************/
e_FileTransErrType Connection::StartDownload(void)
{
    Download.Stats.InProgress=true;
    Download.Stats.BytesRx=0;
    Download.Stats.TotalFileSize=0; // 0 = Unknown

    Download.Timeout=0; // Disable the timer until they turn it on

    /* Start the transfer */
    if(!FTPS_DownloadFile(FTPConData,this,Download.ProtocolID.c_str(),
            Download.DownloadOptions))
    {
        Download.Stats.InProgress=false;
        return e_FileTransErr_ProtocolInitFail;
    }

    RethinkLockOut();

    return e_FileTransErr_Success;
}

/*******************************************************************************
 * NAME:
 *    Connection::AbortDownload
 *
 * SYNOPSIS:
 *    void Connection::AbortDownload(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function aborts the current upload in progress.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::StartDownload()
 ******************************************************************************/
void Connection::AbortDownload(void)
{
    if(Download.Stats.InProgress)
    {
        FTPS_AbortTransfer(FTPConData);
        FinishedDownload(true);
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::DownloadGetStats
 *
 * SYNOPSIS:
 *    struct DownloadStats const *Connection::DownloadGetStats(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets info about the current upload.
 *
 * RETURNS:
 *    A pointer to the upload stats.  This is valid as long as the connection
 *    is valid.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
struct DownloadStats const *Connection::DownloadGetStats(void)
{
    return &Download.Stats;
}

/*******************************************************************************
 * NAME:
 *    Connection::DownloadSetNumberOfBytesRecv
 *
 * SYNOPSIS:
 *    void Connection::DownloadSetNumberOfBytesRecv(uint64_t BytesTransfered);
 *
 * PARAMETERS:
 *    BytesTransfered [I] -- The number of bytes that have successfully been
 *          downloaded
 *
 * FUNCTION:
 *    This function is called from the FTP system to tell us progress has
 *    been made on the file download.
 *
 *    It updates the stats for this download.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::DownloadSetNumberOfBytesRecv(uint64_t BytesTransfered)
{
    Download.Stats.BytesRx=BytesTransfered;

    /* Flag the UI for an update */
    SendMWEvent(ConMWEvent_DownloadStatUpdate);
}

/*******************************************************************************
 * NAME:
 *    Connection::FinishedDownload
 *
 * SYNOPSIS:
 *    void Connection::FinishedDownload(bool Aborted);
 *
 * PARAMETERS:
 *    Aborted [I] -- Are we aborting or just ending
 *
 * FUNCTION:
 *    This function is called from the FTP system because the driver
 *    wants to stop a transfer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::FinishedDownload(bool Aborted)
{
    Download.Stats.InProgress=false;

    if(Aborted)
    {
        DownloadSetNumberOfBytesRecv(0);
        SendMWEvent(ConMWEvent_DownloadAborted);
    }
    else
    {
        if(Download.Stats.TotalFileSize>0)
            DownloadSetNumberOfBytesRecv(Download.Stats.TotalFileSize);
        SendMWEvent(ConMWEvent_DownloadDone);
    }

    RethinkLockOut();
}

/*******************************************************************************
 * NAME:
 *    Connection::FileTransTick
 *
 * SYNOPSIS:
 *    void Connection::FileTransTick(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function runs the file transfer timeouts for a upload / download.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::FileTransTick(void)
{
    uint64_t Time;

    if(Upload.Stats.InProgress && Upload.Timeout!=0)
    {
        Time=GetMSCounter();

        if(Time-Upload.LastTimeoutTick>=Upload.Timeout)
        {
            /* Send timeout */
            FTPS_TimeoutTransfer(FTPConData);
            FileTransRestartTimeout();
        }
    }

    if(Download.Stats.InProgress && Download.Timeout!=0)
    {
        Time=GetMSCounter();

        if(Time-Download.LastTimeoutTick>=Download.Timeout)
        {
            /* Send timeout */
            FTPS_TimeoutTransfer(FTPConData);
            FileTransRestartTimeout();
        }
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::FileTransSetTimeout
 *
 * SYNOPSIS:
 *    void Connection::FileTransSetTimeout(uint32_t MSec);
 *
 * PARAMETERS:
 *    MSec [I] -- The number of ms for this timeout
 *
 * FUNCTION:
 *    This function sets how many ms have to go pass before we send a timeout
 *    to the FTP.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::FileTransSetTimeout(uint32_t MSec)
{
    Upload.Timeout=MSec;
    Download.Timeout=MSec;
    FileTransRestartTimeout();
}

/*******************************************************************************
 * NAME:
 *    Connection::FileTransRestartTimeout
 *
 * SYNOPSIS:
 *    void Connection::FileTransRestartTimeout(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function restarts the FTP timeout counter.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::FileTransRestartTimeout(void)
{
    uint64_t Time;

    Time=GetMSCounter();

    Upload.LastTimeoutTick=Time;
    Download.LastTimeoutTick=Time;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetDownloadFileName
 *
 * SYNOPSIS:
 *    void Connection::SetDownloadFileName(const char *Filename);
 *
 * PARAMETERS:
 *    Filename [I] -- The filename to set for the download.
 *
 * FUNCTION:
 *    This function sets the filename that the connection will be using
 *    to download files.
 *
 *    This is really only used for display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    GetDownloadFileName()
 ******************************************************************************/
void Connection::SetDownloadFileName(const char *Filename)
{
    Download.Filename=Filename;
}

/*******************************************************************************
 * NAME:
 *    Connection::GetDownloadFileName
 *
 * SYNOPSIS:
 *    const char *Connection::GetDownloadFileName(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the filename that was set file download.
 *
 * RETURNS:
 *    A pointer to the string the string with the download filename in it.
 *    This is valid until SetDownloadFileName() is called or the connection
 *    is freed.
 *
 * SEE ALSO:
 *    SetDownloadFileName()
 ******************************************************************************/
const char *Connection::GetDownloadFileName(void)
{
    return Download.Filename.c_str();
}

/*******************************************************************************
 * NAME:
 *    Connection::HandleHexDisplayIncomingData
 *
 * SYNOPSIS:
 *    void Connection::HandleHexDisplayIncomingData(const uint8_t *Inbuff,
 *              int Bytes);
 *
 * PARAMETERS:
 *    InBuff [I] -- The bytes we just read in
 *    Bytes [I] -- The number of bytes we just read in
 *
 * FUNCTION:
 *    This function adds new bytes to the internal buffer.  The internal buffer
 *    is circular and will wrap to be start of the buffer if more bytes are
 *    inserted that the size of the buffer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::HandleHexDisplayIncomingData(const uint8_t *inbuff,int Bytes)
{
    uint8_t *BufferEnd;
    const uint8_t *CopyFrom;
    int CopyBytes;
    union ConMWInfo EventData;

    if(HexDisplay.Paused || !g_Settings.HexDisplayEnabled)
        return;

    if(HexDisplay.Buffer==NULL)
        return;

    CopyFrom=inbuff;

    if(Bytes>HexDisplay.BufferSize)
    {
        CopyFrom+=Bytes-HexDisplay.BufferSize;
        memcpy(HexDisplay.Buffer,CopyFrom,HexDisplay.BufferSize);
        HexDisplay.InsertPos=HexDisplay.Buffer;
        return;
    }

    BufferEnd=HexDisplay.Buffer+HexDisplay.BufferSize;

    if(HexDisplay.InsertPos+Bytes>BufferEnd)
    {
        /* We need to split it over to copies */
        CopyBytes=BufferEnd-HexDisplay.InsertPos;
        memcpy(HexDisplay.InsertPos,CopyFrom,CopyBytes);
        memcpy(HexDisplay.Buffer,&CopyFrom[CopyBytes],Bytes-CopyBytes);
        HexDisplay.InsertPos=HexDisplay.Buffer+Bytes-CopyBytes;
        HexDisplay.BufferWrapped=true;
    }
    else
    {
        /* Things will fit copy the whole thing */
        memcpy(HexDisplay.InsertPos,CopyFrom,Bytes);
        HexDisplay.InsertPos+=Bytes;
    }

    if(HexDisplay.InsertPos>=BufferEnd)
    {
        HexDisplay.InsertPos=HexDisplay.Buffer;
        HexDisplay.BufferWrapped=true;
    }

    /* Update the UI */
    EventData.HexDis.Buffer=HexDisplay.Buffer;
    EventData.HexDis.InsertPos=HexDisplay.InsertPos;
    EventData.HexDis.BufferIsCircular=HexDisplay.BufferWrapped;
    EventData.HexDis.BufferSize=HexDisplay.BufferSize;

    SendMWEvent(ConMWEvent_HexDisplayUpdate,&EventData);
}

/*******************************************************************************
 * NAME:
 *    Connection::GetHexDisplayPaused
 *
 * SYNOPSIS:
 *    bool Connection::GetHexDisplayPaused(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the pause status of the hex display capture.
 *
 * RETURNS:
 *    true -- Hex display is enabled
 *    false -- Hex display is disabled
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Connection::GetHexDisplayPaused(void)
{
    return HexDisplay.Paused;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetHexDisplayPaused
 *
 * SYNOPSIS:
 *    bool Connection::SetHexDisplayPaused(bool Paused,bool Force=false);
 *
 * PARAMETERS:
 *    Paused [I] -- Pause collecting more data for the hex display.
 *    Force [I] -- If true then we do it even if it wouldn't be needed.
 *
 * FUNCTION:
 *    This function changes the pause state of the hex display capture system.
 *
 * RETURNS:
 *    true -- New value taken
 *    false -- There was an error allocating the new buffer.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Connection::SetHexDisplayPaused(bool Paused,bool Force)
{
    /* If there is no change don't do anything */
    if(HexDisplay.Paused==Paused && !Force)
        return true;

    HexDisplay.Paused=Paused;

    return true;
}

/*******************************************************************************
 * NAME:
 *    Connection::HexDisplayGetBufferInfo
 *
 * SYNOPSIS:
 *    void Connection::HexDisplayGetBufferInfo(const uint8_t **Buffer,
 *              const uint8_t **InsertPos,bool *BufferIsCircular,
 *              int *BufferSize);
 *
 * PARAMETERS:
 *    Buffer [O] -- A pointer to write with the address of the hex display
 *                  buffer
 *    InsertPos [O] -- A pointer to the end of the data.  This is one past then
 *                     end of valid data.  When 'BufferIsCircular' is
 *                     true then data starts at this point, if it's false
 *                     then data starts at 'Buffer'.
 *    BufferIsCircular [O] -- This tells you if the buffer has wrapped or not.
 *                            Before the wrap data goes from 'Buffer' to
 *                            'InsertPos'.  After the wrap the data goes from
 *                            'InsertPos' until 'InsertPos-1'
 *    BufferSize [O] -- The size of the hex display buffer.  This is the
 *                      total size.  When 'BufferIsCircular' is true then
 *                      you must wrap back to 'Buffer' and keep going until
 *                      you get back to 'InsertPos'.
 *
 * FUNCTION:
 *    This function gets access to the RAW buffers for the hex display.
 *    This is ment to be used to display the contents of the buffer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::HexDisplayGetBufferInfo(const uint8_t **Buffer,
        const uint8_t **InsertPos,bool *BufferIsCircular,int *BufferSize)
{
    *Buffer=HexDisplay.Buffer;
    *InsertPos=HexDisplay.InsertPos;
    *BufferIsCircular=HexDisplay.BufferWrapped;
    *BufferSize=HexDisplay.BufferSize;
}

/*******************************************************************************
 * NAME:
 *    Connection::HexDisplayClear
 *
 * SYNOPSIS:
 *    void Connection::HexDisplayClear(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clears the hex display.  Does not inform the UI.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::HexDisplayClear(void)
{
    HexDisplay.InsertPos=HexDisplay.Buffer;
    HexDisplay.BufferWrapped=false;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetLeftPanelInfo
 *
 * SYNOPSIS:
 *    void Connection::SetLeftPanelInfo(e_LeftPanelTabType SelectedTab);
 *
 * PARAMETERS:
 *    SelectedTab [I] -- What tab has been selected
 *
 * FUNCTION:
 *    This function stores info about the left panel.  This is stored
 *    per connection info.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::GetLeftPanelInfo()
 ******************************************************************************/
void Connection::SetLeftPanelInfo(e_LeftPanelTabType SelectedTab)
{
    LeftPanelInfo=SelectedTab;
}

/*******************************************************************************
 * NAME:
 *    Connection::GetLeftPanelInfo
 *
 * SYNOPSIS:
 *    e_LeftPanelTabType Connection::GetLeftPanelInfo(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function returns the stored info about this panel.
 *
 * RETURNS:
 *    The selected panel.
 *
 * SEE ALSO:
 *    Connection::SetLeftPanelInfo()
 ******************************************************************************/
e_LeftPanelTabType Connection::GetLeftPanelInfo(void)
{
    return LeftPanelInfo;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetRightPanelInfo
 *
 * SYNOPSIS:
 *    void Connection::SetRightPanelInfo(e_RightPanelTabType SelectedTab);
 *
 * PARAMETERS:
 *    SelectedTab [I] -- What tab has been selected
 *
 * FUNCTION:
 *    This function stores info about the right panel.  This is stored
 *    per connection info.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::GetRightPanelInfo()
 ******************************************************************************/
void Connection::SetRightPanelInfo(e_RightPanelTabType SelectedTab)
{
    RightPanelInfo=SelectedTab;
}

/*******************************************************************************
 * NAME:
 *    Connection::GetLeftPanelInfo
 *
 * SYNOPSIS:
 *    e_RightPanelTabType Connection::GetRightPanelInfo(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function returns the stored info about this panel.
 *
 * RETURNS:
 *    The selected panel.
 *
 * SEE ALSO:
 *    Connection::SetRightPanelInfo()
 ******************************************************************************/
e_RightPanelTabType Connection::GetRightPanelInfo(void)
{
    return RightPanelInfo;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetBottomPanelInfo
 *
 * SYNOPSIS:
 *    void Connection::SetBottomPanelInfo(e_BottomPanelTabType SelectedTab);
 *
 * PARAMETERS:
 *    SelectedTab [I] -- What tab has been selected
 *
 * FUNCTION:
 *    This function stores info about the bottom panel.  This is stored
 *    per connection info.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::GetBottomPanelInfo()
 ******************************************************************************/
void Connection::SetBottomPanelInfo(e_BottomPanelTabType SelectedTab)
{
    BottomPanelInfo=SelectedTab;
}

/*******************************************************************************
 * NAME:
 *    Connection::GetBottomPanelInfo
 *
 * SYNOPSIS:
 *    e_BottomPanelTabType Connection::GetBottomPanelInfo(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function returns the stored info about this panel.
 *
 * RETURNS:
 *    The selected panel or 'e_BottomPanelTabMAX' if this is new connection.
 *
 * SEE ALSO:
 *    Connection::SetBottomPanelInfo()
 ******************************************************************************/
e_BottomPanelTabType Connection::GetBottomPanelInfo(void)
{
    return BottomPanelInfo;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetupComTest
 *
 * SYNOPSIS:
 *    void Connection::SetupComTest(bool Sender,uint32_t PacketLen,
 *              uint32_t PacketsCount,uint32_t Delay,uint8_t *PacketData);
 *
 * PARAMETERS:
 *    Sender [I] -- Is this connection to send packs?
 *    PacketLen [I] -- The number of bytes in a packet.
 *    PacketsCount [I] -- The number of packets to send before we stop
 *    Delay [I] -- The delay between packets in mS.
 *    PacketData [I] -- The data pattern to send.  This must have 'PacketLen'
 *                      bytes in it.  This must remain valid until this
 *                      function is called again.  NULL is valid (to end
 *                      the test).
 *
 * FUNCTION:
 *    This function sets up for a com test.  It stops any test in progress
 *    and reset the stats and the test state machine.
 *
 *    You need to start the test before the connection will tx / rx any data.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    This function does not take control of 'PacketData'.  It just uses it
 *    but it must remain valid until the test is finished.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::SetupComTest(bool Sender,uint32_t PacketLen,
        uint32_t PacketsCount,uint32_t Delay,uint8_t *PacketData)
{
    ComTest.Sender=Sender;
    ComTest.SendingPackets=false;
    ComTest.PacketLen=PacketLen;
    ComTest.PacketsCount=PacketsCount;
    ComTest.DelayBetweenPackets_mS=Delay;
    ComTest.Packet=PacketData;
    ComTest.RxPatternIndex=0;
    ComTest.Syncing=false;

    ComTest.Stats.InProgress=false;
    ComTest.Stats.PacketsSent=0;
    ComTest.Stats.BytesSent=0;
    ComTest.Stats.PacketsRx=0;
    ComTest.Stats.ErrorsDetected=0;
    ComTest.Stats.BytesPerSec=0;
    ComTest.Stats.SendErrors=0;
    ComTest.Stats.SendBusyErrors=0;
    ComTest.Stats.LastRxTimeStamp=0;
}

/*******************************************************************************
 * NAME:
 *    Connection::StartComTest
 *
 * SYNOPSIS:
 *    bool Connection::StartComTest(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function starts a com test on this connection.  If it's a sender
 *    it will start sending packets.  It will also send an rx packets to
 *    the com test system.
 *
 * RETURNS:
 *    true -- This connection has started the test
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Connection::StartComTest(void)
{
    if(ComTest.Packet==NULL || ComTest.PacketLen<1 || ComTest.PacketsCount<1)
        return false;

    ComTest.RxPatternIndex=0;
    ComTest.Syncing=false;

    ComTest.Stats.PacketsSent=0;
    ComTest.Stats.BytesSent=0;
    ComTest.Stats.PacketsRx=0;
    ComTest.Stats.ErrorsDetected=0;
    ComTest.Stats.BytesPerSec=0;
    ComTest.Stats.SendErrors=0;
    ComTest.Stats.SendBusyErrors=0;
    time(&ComTest.Stats.LastRxTimeStamp);

    if(ComTest.Sender && ComTest.DelayBetweenPackets_mS>0)
    {
        /* If we are the sender then we need to setup a timer to send packets */
        ComTest.Timer=AllocUITimer();
        if(ComTest.Timer==NULL)
            return false;

        SetupUITimer(ComTest.Timer,Con_ComTestTimeout,(uintptr_t)this,true);

        UITimerSetTimeout(ComTest.Timer,ComTest.DelayBetweenPackets_mS);
        UITimerStart(ComTest.Timer);
    }

    ComTest.SendingPackets=false;
    if(ComTest.Sender)
        ComTest.SendingPackets=true;

    ComTest.Stats.InProgress=true;

    /* Send the first packet */
    InformOfComTestTimeout();

    if(ComTest.UpdateFn!=NULL)
        ComTest.UpdateFn(this);

    return true;
}

/*******************************************************************************
 * NAME:
 *    Connection::StopComTest
 *
 * SYNOPSIS:
 *    void Connection::StopComTest(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function stops a com test.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::StopComTest(void)
{
    if(ComTest.Timer!=NULL)
    {
        UITimerStop(ComTest.Timer);
        FreeUITimer(ComTest.Timer);
        ComTest.Timer=NULL;
    }
    ComTest.Stats.InProgress=false;
    if(ComTest.UpdateFn!=NULL)
        ComTest.UpdateFn(this);
}

/*******************************************************************************
 * NAME:
 *    Connection::InformOfComTestTimeout
 *
 * SYNOPSIS:
 *    void Connection::InformOfComTestTimeout(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when the timer goes off to send com test data.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::InformOfComTestTimeout(void)
{
    /* Send the next test packet */
    if(ComTest.Packet==NULL || ComTest.PacketLen<1 || ComTest.PacketsCount<1 ||
            !ComTest.Stats.InProgress || IOHandle==NULL ||
            !ComTest.SendingPackets)
    {
        return;
    }

    /* We bypass WriteData() so we can send a little faster (this also
       skips all the other sub systems) */
    switch(IOS_WriteData(IOHandle,ComTest.Packet,ComTest.PacketLen))
    {
        case e_IOSysIOError_Success:
            ComTest.Stats.PacketsSent++;
            ComTest.Stats.BytesSent+=ComTest.PacketLen;
            if(ComTest.Stats.PacketsSent>=ComTest.PacketsCount)
            {
                /* We are done sending */
                ComTest.SendingPackets=false;
                if(ComTest.Timer!=NULL)
                {
                    UITimerStop(ComTest.Timer);
                    FreeUITimer(ComTest.Timer);
                    ComTest.Timer=NULL;
                }
            }
            if(ComTest.UpdateFn!=NULL)
                ComTest.UpdateFn(this);
        break;
        case e_IOSysIOError_GenericIO:
        case e_IOSysIOError_Disconnect:
        case e_IOSysIOErrorMAX:
        default:
            /* Abort */
            StopComTest();
            ComTest.Stats.SendErrors++;
            if(ComTest.UpdateFn!=NULL)
                ComTest.UpdateFn(this);
        break;
        case e_IOSysIOError_Busy:
            ComTest.Stats.SendBusyErrors++;
            if(ComTest.UpdateFn!=NULL)
                ComTest.UpdateFn(this);
        break;
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::IsComTestRunning
 *
 * SYNOPSIS:
 *    bool Connection::IsComTestRunning(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function check to see if the com test is running or not.
 *
 * RETURNS:
 *    true -- Com test is running
 *    false -- No com test running
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Connection::IsComTestRunning(void)
{
    return ComTest.Stats.InProgress;
}

/*******************************************************************************
 * NAME:
 *    Connection::ComTestFastTick
 *
 * SYNOPSIS:
 *    void Connection::ComTestFastTick(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when doing a com test with 0mS delay between
 *    packets.
 *
 *    You should process events and then call this is in a loop.  It will
 *    send the next packet in the test.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::ComTestFastTick(void)
{
    InformOfComTestTimeout();
}

/*******************************************************************************
 * NAME:
 *    Connection::HandleComTestRx
 *
 * SYNOPSIS:
 *    void Connection::HandleComTestRx(uint8_t *inbuff,int bytes);
 *
 * PARAMETERS:
 *    inbuff [I] -- The bytes we got from the connection
 *    bytes [I] -- The number of bytes in 'inbuff'
 *
 * FUNCTION:
 *    This function handles incoming bytes when in com test mode.  It checks
 *    the pattern and flags errors.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::HandleComTestRx(uint8_t *inbuff,int bytes)
{
    uint8_t *EndPtr;
    uint8_t *p;
    EndPtr=&inbuff[bytes];
    p=inbuff;
    unsigned int s;

    time(&ComTest.Stats.LastRxTimeStamp);

    while(p<EndPtr)
    {
        if(!ComTest.Syncing)
        {
            /* Is it the expected byte */
            if(*p!=ComTest.Packet[ComTest.RxPatternIndex])
            {
                /* Messed up byte */
                ComTest.Stats.ErrorsDetected++;
                if(ComTest.UpdateFn!=NULL)
                    ComTest.UpdateFn(this);
                /* Ok, try to resync */
                ComTest.Syncing=true;
            }
            else
            {
                ComTest.RxPatternIndex++;
                if(ComTest.RxPatternIndex>=ComTest.PacketLen)
                {
                    ComTest.Stats.PacketsRx++;
                    if(ComTest.UpdateFn!=NULL)
                        ComTest.UpdateFn(this);
                    ComTest.RxPatternIndex=0;
                }
            }
        }
        if(ComTest.Syncing)
        {
            /* Search the whole pattern to see if we find the byte */
            s=ComTest.RxPatternIndex;
            for(;;)
            {
                if(*p==ComTest.Packet[s])
                {
                    /* Found it, move to the next byte and exit sync */
                    ComTest.RxPatternIndex=s+1;
                    if(ComTest.RxPatternIndex>=ComTest.PacketLen)
                        ComTest.RxPatternIndex=0;
                    ComTest.Syncing=false;
                    break;
                }

                s++;
                if(s>=ComTest.PacketLen)
                    s=0;
                if(s==ComTest.RxPatternIndex)
                {
                    /* Looked at every byte, didn't find it */
                    break;
                }
            }
        }
        p++;
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::GetComTestStats
 *
 * SYNOPSIS:
 *    const struct ComTestStats *Connection::GetComTestStats(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets access to the com test stats.
 *
 * RETURNS:
 *    A pointer to the com test stats.  This is valid as long as the class is.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
const struct ComTestStats *Connection::GetComTestStats(void)
{
    return &ComTest.Stats;
}

/*******************************************************************************
 * NAME:
 *    Connection::RegisterComTestUpdateFn
 *
 * SYNOPSIS:
 *    void Connection::RegisterComTestUpdateFn(void (*Update)(class Connection *));
 *
 * PARAMETERS:
 *    Update [I] -- The call back to call with updates about the com test.
 *                  void Update(class Connection *Con);
 *
 * FUNCTION:
 *    This function sets the function that gets called from this connection
 *    when there is an update about the com test.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::RegisterComTestUpdateFn(void (*Update)(class Connection *))
{
    ComTest.UpdateFn=Update;
}

/*******************************************************************************
 * NAME:
 *    Connection::GetTransmitDelayPerByte
 *
 * SYNOPSIS:
 *    unsigned int Connection::GetTransmitDelayPerByte(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the number of ms between bytes sent
 *
 * RETURNS:
 *    The number of ms to wait between bytes
 *
 * SEE ALSO:
 *    Connection::GetTransmitDelayPerLine()
 ******************************************************************************/
unsigned int Connection::GetTransmitDelayPerByte(void)
{
    return TransmitDelayByte;
}

/*******************************************************************************
 * NAME:
 *    Connection::GetTransmitDelayPerLine
 *
 * SYNOPSIS:
 *    unsigned int Connection::GetTransmitDelayPerLine(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the number of ms between lines sent
 *
 * RETURNS:
 *    The number of ms to wait between lines
 *
 * SEE ALSO:
 *    Connection::GetTransmitDelayPerByte()
 ******************************************************************************/
unsigned int Connection::GetTransmitDelayPerLine(void)
{
    return TransmitDelayLine;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetTransmitDelayPerByte
 *
 * SYNOPSIS:
 *    void Connection::SetTransmitDelayPerByte(unsigned int ms);
 *
 * PARAMETERS:
 *    ms [I] -- The ms to delay between bytes sent
 *
 * FUNCTION:
 *    Sets the number ms between bytes to wait when sending.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::SetTransmitDelayPerByte(unsigned int ms)
{
    TransmitDelayByte=ms;
    ApplyTransmitDelayChange();
}

/*******************************************************************************
 * NAME:
 *    Connection::SetTransmitDelayPerLine
 *
 * SYNOPSIS:
 *    void Connection::SetTransmitDelayPerLine(unsigned int ms);
 *
 * PARAMETERS:
 *    ms [I] -- The ms to delay between lines sent
 *
 * FUNCTION:
 *    Sets the number ms between lines to wait when sending.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::SetTransmitDelayPerLine(unsigned int ms)
{
    TransmitDelayLine=ms;
    ApplyTransmitDelayChange();
}

/*******************************************************************************
 * NAME:
 *    Connection::ApplyTransmitDelayChange
 *
 * SYNOPSIS:
 *    void Connection::ApplyTransmitDelayChange(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function takes the transmit delay change and updates the buffers
 *    and timers.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::ApplyTransmitDelayChange(void)
{
//    if(TransmitDelayBuffer!=NULL && TransmitDelayByte==0 &&
//            TransmitDelayLine==0)
//    {
//        /* Ok, we don't need the queue any more, free it */
//        FreeTransmitDelayBuffer();
//    }
//    else
//    {
//        if(TransmitDelayLine!=0 && TransmitDelayByte==0)
//            UITimerSetTimeout(TransmitDelayTimer,TransmitDelayLine);
//        if(TransmitDelayByte!=0)
//            UITimerSetTimeout(TransmitDelayTimer,TransmitDelayByte);
//    }

    if(TransmitDelayByte==0 && TransmitDelayLine==0)
    {
        if(UITimerRunning(TransmitDelayTimer))
        {
            /* We are waiting to send the next block, abort the timer and
               just send whatever's left */
            UITimerStop(TransmitDelayTimer);
            InternalWriteBytes(&TransmitDelayBuffer[TransmitDelayBufferReadPos],
                    TransmitDelayBufferWritePos-TransmitDelayBufferReadPos);
        }

        /* We don't need the queue, free it */
        if(TransmitDelayBuffer!=NULL)
            FreeTransmitDelayBuffer();
    }
    else
    {
        if(TransmitDelayLine!=0 && TransmitDelayByte==0)
            UITimerSetTimeout(TransmitDelayTimer,TransmitDelayLine);
        if(TransmitDelayByte!=0)
            UITimerSetTimeout(TransmitDelayTimer,TransmitDelayByte);
    }

}

/*******************************************************************************
 * NAME:
 *    Connection::FreeTransmitDelayBuffer
 *
 * SYNOPSIS:
 *    void Connection::FreeTransmitDelayBuffer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the transmit delay buffers.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 *****************************************************************************/
void Connection::FreeTransmitDelayBuffer(void)
{
    UITimerStop(TransmitDelayTimer);

    free(TransmitDelayBuffer);
    TransmitDelayBuffer=NULL;
    TransmitDelayBufferSize=0;
    TransmitDelayBufferWritePos=0;
}

/*******************************************************************************
 * NAME:
 *    Connection::QueueTransmitDelayData
 *
 * SYNOPSIS:
 *    bool Connection::QueueTransmitDelayData(const uint8_t *Data,
 *              int Bytes);
 *
 * PARAMETERS:
 *    Data [I] -- The data to send
 *    Bytes [I] -- The number of bytes to send
 *
 * FUNCTION:
 *    This function adds data to the transmit delay buffer.
 *
 * RETURNS:
 *    true -- Data was queued
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Connection::QueueTransmitDelayData(const uint8_t *Data,int Bytes)
{
    uint8_t *NewMemory;
    unsigned int AllocSize;

    if(TransmitDelayBuffer==NULL)
    {
        /* Ok, we need to allocate a starting buffer */
        AllocSize=Bytes;
        if(AllocSize<TRANSMIT_DELAY_BUFFER_SIZE)
            AllocSize=TRANSMIT_DELAY_BUFFER_SIZE;
        TransmitDelayBuffer=(uint8_t *)malloc(AllocSize);
        if(TransmitDelayBuffer==NULL)
            return false;
        TransmitDelayBufferSize=AllocSize;
        TransmitDelayBufferWritePos=0;
        TransmitDelayBufferReadPos=0;
    }

    if(TransmitDelayBufferWritePos+Bytes>TransmitDelayBufferSize)
    {
        AllocSize=Bytes;
        if(AllocSize<TRANSMIT_DELAY_BUFFER_SIZE)
            AllocSize=TRANSMIT_DELAY_BUFFER_SIZE;
        AllocSize+=TransmitDelayBufferWritePos;
        NewMemory=(uint8_t *)realloc(TransmitDelayBuffer,AllocSize);
        if(NewMemory==NULL)
            return false;

        TransmitDelayBuffer=NewMemory;
        TransmitDelayBufferSize=AllocSize;
    }

    memcpy(&TransmitDelayBuffer[TransmitDelayBufferWritePos],Data,Bytes);
    TransmitDelayBufferWritePos+=Bytes;

    return true;
}

/*******************************************************************************
 * NAME:
 *    Connection::InformOfDelayTransmitTimeout
 *
 * SYNOPSIS:
 *    void Connection::InformOfDelayTransmitTimeout(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called to send the next byte / block from the delayed
 *    transmit system.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::InformOfDelayTransmitTimeout(void)
{
    const uint8_t *Send;
    uint32_t Delay;
    unsigned int r;

    if(TransmitDelayBuffer==NULL)
        return;

    if(!IsConnected)
    {
        FreeTransmitDelayBuffer();
        return;
    }

    if(TransmitDelayBufferWritePos<=TransmitDelayBufferReadPos)
    {
        /* Nothing to send, reset to the start of buffer and wait for more
           data */
        TransmitDelayBufferWritePos=0;
        TransmitDelayBufferReadPos=0;
        return;
    }

    Send=&TransmitDelayBuffer[TransmitDelayBufferReadPos];

    if(TransmitDelayByte>0)
    {
        /* Delay between bytes, so we only send 1 byte at a time */
        TransmitDelayBufferReadPos++;
        InternalWriteBytes(Send,1);

        /* Setup for the next byte delay */
        Delay=TransmitDelayByte;

        if(*Send=='\n' && TransmitDelayLine>0)
        {
            /* It was a new line char so we delay for the line */
            Delay=TransmitDelayLine;
        }

        if(Delay<TransmitDelayByte)
            Delay=TransmitDelayByte;
    }
    else if(TransmitDelayLine>0)
    {
        /* Ok, send at full speed until we see a newline char */
        for(r=TransmitDelayBufferReadPos;r<TransmitDelayBufferWritePos;r++)
            if(TransmitDelayBuffer[r]=='\n')
                break;
        if(r==TransmitDelayBufferWritePos)
        {
            /* No new line char, send the whole thing */
            InternalWriteBytes(Send,
                    TransmitDelayBufferWritePos-TransmitDelayBufferReadPos);
            TransmitDelayBufferWritePos=0;
            TransmitDelayBufferReadPos=0;
            Delay=0;
        }
        else
        {
            /* Send out up to (and including) the '\n' */
            InternalWriteBytes(Send,r-TransmitDelayBufferReadPos+1);
            TransmitDelayBufferReadPos+=(r-TransmitDelayBufferReadPos+1);
            Delay=TransmitDelayLine;
        }
    }

    if(TransmitDelayLine==0 && TransmitDelayByte==0)
    {
        
        Delay=0;
    }

    if(Delay>0)
    {
        UITimerSetTimeout(TransmitDelayTimer,Delay);
        UITimerStart(TransmitDelayTimer);
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::SetLockOutConnectionWhenBridged
 *
 * SYNOPSIS:
 *    void Connection::SetLockOutConnectionWhenBridged(bool Lockout);
 *
 * PARAMETERS:
 *    Lockout [I] -- Lockout the connection (true) or let it be (false)
 *
 * FUNCTION:
 *    This function locks out input from the user when this connection is
 *    bridged to another connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::SetLockOutConnectionWhenBridged(bool Lockout)
{
    WhenBridgedLockoutConnection=Lockout;

    RethinkLockOut();
}

/*******************************************************************************
 * NAME:
 *    Connection::GetLockOutConnectionWhenBridged
 *
 * SYNOPSIS:
 *    bool Connection::GetLockOutConnectionWhenBridged(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the state of the lockout when connections are bridged.
 *
 * RETURNS:
 *    true -- Connection is locked out when bridged
 *    false -- Connection is not locked out when bridged.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Connection::GetLockOutConnectionWhenBridged(void)
{
    return WhenBridgedLockoutConnection;
}

/*******************************************************************************
 * NAME:
 *    Connection::RethinkLockOut
 *
 * SYNOPSIS:
 *    void Connection::RethinkLockOut(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function will make the connection locked out if it is needed,
 *    if it isn't needed it will unlock the connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::RethinkLockOut(void)
{
    const char *Message;

    Message=NULL;
    if(Download.Stats.InProgress)
        Message="Downloading...";

    if(Upload.Stats.InProgress)
        Message="Uploading...";

    if(BridgedTo!=NULL)
        if(WhenBridgedLockoutConnection)
            Message="Bridged";

    if(Message==NULL)
    {
        Display->SetOverrideMessage(NULL);
        ConnectionLockedOut=false;
    }
    else
    {
        Display->SetOverrideMessage(Message);
        ConnectionLockedOut=true;
    }
}

/*******************************************************************************
 * NAME:
 *    Connection::BridgeConnection
 *
 * SYNOPSIS:
 *    void Connection::BridgeConnection(class Connection *Con);
 *
 * PARAMETERS:
 *    Con [I] -- The connection to bridge to, or NULL for none.
 *
 * FUNCTION:
 *    This function sets what connection this connections is bridged to.
 *    Any bytes that come into this connection are sent to the other connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::GetBridgedConnection()
 ******************************************************************************/
void Connection::BridgeConnection(class Connection *Con)
{
    union ConMWInfo ExtraInfo;

    /* Disconnect us */
    if(BridgedTo!=NULL)
        BridgedTo->SetBridgeFrom(NULL);

    BridgedTo=Con;
    if(Con!=NULL)
        Con->SetBridgeFrom(this);
    RethinkLockOut();

    ExtraInfo.Bridged.BridgedTo=Con;

    SendMWEvent(ConMWEvent_BridgeStateChange,&ExtraInfo);
}

/*******************************************************************************
 * NAME:
 *    Connection::GetBridgedConnection
 *
 * SYNOPSIS:
 *    class Connection *Connection::GetBridgedConnection(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the other connection that this connection is bridged
 *    to.
 *
 * RETURNS:
 *    The other connection this connection is bridged to or NULL if none.
 *
 * SEE ALSO:
 *    Connection::BridgeConnection()
 ******************************************************************************/
class Connection *Connection::GetBridgedConnection(void)
{
    return BridgedTo;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetBridgeFrom
 *
 * SYNOPSIS:
 *    void Connection::SetBridgeFrom(class Connection *From);
 *
 * PARAMETERS:
 *    From [I] -- The connection we are bridged from
 *
 * FUNCTION:
 *    This function sets the connection that this connection is going to
 *    get sent data from.  This is needed so we can tell a bridged connection
 *    that it needs to stop sending data to us (if we are closed for example).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::SetBridgeFrom(class Connection *From)
{
    BridgedFrom=From;
}

/*******************************************************************************
 * NAME:
 *    Connection::BridgeConnectionFreeing
 *
 * SYNOPSIS:
 *    void Connection::BridgeConnectionFreeing(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when we are brided to another connection and
 *    that connection is being freed.
 *
 *    We disconnect the bride and update our display.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::BridgeConnectionFreeing(void)
{
    BridgedTo=NULL;
    BridgedFrom=NULL;
    RethinkLockOut();
}

/*******************************************************************************
 * NAME:
 *    Connection::Connect2Bookmark
 *
 * SYNOPSIS:
 *    void Connection::Connect2Bookmark(int BookmarkUID);
 *
 * PARAMETERS:
 *    BookmarkUID [I] -- The bookmark UID to connect to this connection
 *
 * FUNCTION:
 *    This function assigns a bookmark to this connection.  This can be used
 *    to lookup the bookmark from this connection.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    GetConnectionBookmark()
 ******************************************************************************/
void Connection::Connect2Bookmark(int BookmarkUID)
{
    Bookmark=BookmarkUID;
}

/*******************************************************************************
 * NAME:
 *    Connection::GetConnectionBookmark
 *
 * SYNOPSIS:
 *    int Connection::GetConnectionBookmark(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the linked bookmark for this connection.
 *
 * RETURNS:
 *    0 -- No linked bookmark
 *    >0 -- The UID of the bookmark
 *
 * SEE ALSO:
 *    Connect2Bookmark()
 ******************************************************************************/
int Connection::GetConnectionBookmark(void)
{
    return Bookmark;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetShowNonPrintable
 *
 * SYNOPSIS:
 *    void Connection::SetShowNonPrintable(bool Show);
 *
 * PARAMETERS:
 *    Show [I] -- Do we show them (true) or hide them (false)
 *
 * FUNCTION:
 *    This function sets if this connection should show the non-printable
 *    chars or hide them.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::GetShowNonPrintable()
 ******************************************************************************/
void Connection::SetShowNonPrintable(bool Show)
{
    ShowNonPrintables=Show;

    if(Display!=nullptr)
        Display->SetShowNonPrintable(ShowNonPrintables);
}

/*******************************************************************************
 * NAME:
 *    Connection::GetShowNonPrintable
 *
 * SYNOPSIS:
 *    bool Connection::GetShowNonPrintable(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets if we are showing non-printable char for this
 *    connection or not.
 *
 * RETURNS:
 *    true -- Non-printable chars are being showen
 *    false -- Non-printable chars are hidden.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Connection::GetShowNonPrintable(void)
{
    return ShowNonPrintables;
}

/*******************************************************************************
 * NAME:
 *    Connection::SetShowEndOfLines
 *
 * SYNOPSIS:
 *    void Connection::SetShowEndOfLines(bool Show);
 *
 * PARAMETERS:
 *    Show [I] -- Do we show them (true) or hide them (false)
 *
 * FUNCTION:
 *    This function sets if this connection should show the end of lines
 *    or hide them.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::GetShowEndOfLines()
 ******************************************************************************/
void Connection::SetShowEndOfLines(bool Show)
{
    ShowEndOfLines=Show;

    if(Display!=nullptr)
        Display->SetShowEndOfLines(ShowEndOfLines);
}

/*******************************************************************************
 * NAME:
 *    Connection::GetShowEndOfLines
 *
 * SYNOPSIS:
 *    bool Connection::GetShowEndOfLines(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets if we are showing end of lines for this
 *    connection or not.
 *
 * RETURNS:
 *    true -- End of lines are being showen
 *    false -- End of lines are hidden.
 *
 * SEE ALSO:
 *    SetShowEndOfLines()
 ******************************************************************************/
bool Connection::GetShowEndOfLines(void)
{
    return ShowEndOfLines;
}

/*******************************************************************************
 * NAME:
 *    Connection::RethinkCursor
 *
 * SYNOPSIS:
 *    void Connection::RethinkCursor(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This rethinks what the cursor should look like.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Connection::RethinkCursor(void)
{
    bool CursorBlinking;
    e_TextCursorStyleType CursorStyle;

    if(Display==nullptr)
        return;

    CursorBlinking=false;
    CursorStyle=e_TextCursorStyle_Block;
//    if(DoingOverrideText)
//    {
//        CursorStyle=e_TextCursorStyle_Hidden;
//    }
//    else
    {
        if(Display->GetInFocus())
        {
            if(IsConnected)
            {
                CursorBlinking=CustomSettings.CursorBlink;
                CursorStyle=e_TextCursorStyle_Block;
            }
            else
            {
                CursorStyle=e_TextCursorStyle_Dotted;
            }
        }
        else
        {
            CursorStyle=e_TextCursorStyle_Box;
        }
    }
    Display->SetCursorBlinking(CursorBlinking);
    Display->SetCursorStyle(CursorStyle);
}
