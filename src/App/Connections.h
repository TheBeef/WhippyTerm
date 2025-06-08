/*******************************************************************************
 * FILENAME: Connections.h
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    .h file
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
 * HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __CONNECTIONS_H_
#define __CONNECTIONS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/DataProcessorsSystem.h"
#include "App/FileTransferProtocolSystem.h"
#include "App/Display/DisplayBase.h"
#include "App/IOSystem.h"
#include "App/MaxSizes.h"
#include "App/Settings.h"
#include "UI/UIClipboard.h"
#include "UI/UITimers.h"
#include "UI/UIMainWindow.h"
#include "UI/UIClipboard.h"
#include "UI/UITextMainArea.h"
#include <stdint.h>
#include <string>
#include <list>

/***  DEFINES                          ***/
#define CAPTURE_HEXDUMP_VALUES_PER_LINE         16

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_ConFunc_NewLine=0,
    e_ConFunc_Return,
    e_ConFunc_Backspace,
    e_ConFunc_MoveCursor,
    e_ConFunc_ClearScreen,
    e_ConFunc_ClearScreenAndBackBuffer,
    e_ConFunc_ClearArea,
    e_ConFunc_Tab,
    e_ConFunc_PrevTab,
    e_ConFunc_NoteNonPrintable,
    e_ConFunc_SendBackspace,
    e_ConFunc_SendEnter,
    e_ConFunc_ScrollArea,
    e_ConFuncMAX
} e_ConFuncType;

typedef enum
{
    e_ConWrite_Success,
    e_ConWrite_Failed,
    e_ConWrite_Busy,
    e_ConWrite_Ignored,
    e_ConWriteMAX
} e_ConWriteType;

typedef enum
{
    e_ConWriteSource_Keyboard,
    e_ConWriteSource_Upload,
    e_ConWriteSource_Download,
    e_ConWriteSource_Paste,
    e_ConWriteSource_Buffers,
    e_ConWriteSource_BlockSend,
    e_ConWriteSource_Bridge,
    e_ConWriteSourceMAX
} e_ConWriteSourceType;

struct CaptureToFileOptions
{
    bool Timestamp;
    bool Append;
    bool StripCtrl;
    bool StripEsc;
    bool SaveAsHexDump;
};

struct CaptureToFileType
{
    std::string Filename;
    FILE *WriteHandle;
    struct CaptureToFileOptions Options;
    bool EscSeqSkiping;
    uint8_t HexDumpBuff[CAPTURE_HEXDUMP_VALUES_PER_LINE+1];
    int HexDumpInsertPos;
    int HexDumpOffset;
};

typedef std::list<uint64_t> t_StopWatchLapTimes;
typedef t_StopWatchLapTimes::iterator i_StopWatchLapTimes;

struct StopWatchType
{
    uint64_t StartTime;
    uint64_t StopTime;
    uint64_t LastLapTime;
    bool Running;
    uint64_t LastRxDataTime;
    bool AutoStartOnTx;
    bool AutoLap;
    t_StopWatchLapTimes Laps;
    i_StopWatchLapTimes CurrentLapInfoPoint;
};

struct UploadStats
{
    bool InProgress;
    uint64_t BytesSent;
    uint64_t TotalFileSize;
};

struct UploadType
{
    std::string Filename;
    std::string ProtocolID;
    t_KVList UploadOptions;
    uint64_t LastTimeoutTick;
    uint32_t Timeout;
    bool TimerActive;
    struct UploadStats Stats;
};

struct DownloadStats
{
    bool InProgress;
    uint64_t BytesRx;
    uint64_t TotalFileSize; // 0 = unknown
};

struct DownloadType
{
    std::string Filename;
    std::string ProtocolID;
    t_KVList DownloadOptions;
    uint64_t LastTimeoutTick;
    uint32_t Timeout;
    bool TimerActive;
    struct DownloadStats Stats;
};

typedef enum
{
    e_FileTransErr_Success=0,
    e_FileTransErr_FileError,
    e_FileTransErr_ProtocolInitFail,
    e_FileTransErrMAX
} e_FileTransErrType;

struct HexDisplayType
{
    uint8_t *Buffer;
    int BufferSize;
    uint8_t *InsertPos;
    bool Paused;
    bool BufferWrapped;
};

struct ComTestStats
{
    bool InProgress;
    uint64_t PacketsSent;
    uint64_t BytesSent;
    uint64_t PacketsRx;
    uint64_t ErrorsDetected;
    uint64_t BytesPerSec;               // TBD (and readded to UI)
    uint64_t SendErrors;
    uint64_t SendBusyErrors;
    time_t LastRxTimeStamp;
};

struct ComTestType
{
    bool Sender;            // Is this a source of packets?
    bool SendingPackets;
    uint32_t PacketLen;
    uint32_t PacketsCount;
    uint32_t DelayBetweenPackets_mS;
    uint8_t *Packet;
    uint32_t RxPatternIndex;
    bool Syncing;
    void (*UpdateFn)(class Connection *);
    struct UITimer *Timer;
    struct ComTestStats Stats;
};

typedef enum
{
    ConMWEvent_ConOptionsChange,
    ConMWEvent_NameChange,
    ConMWEvent_StatusChange,
    ConMWEvent_StopWatchAddLap,
    ConMWEvent_StopWatchAutoStarted,
    ConMWEvent_UploadStatUpdate,
    ConMWEvent_UploadAborted,
    ConMWEvent_UploadDone,
    ConMWEvent_NewConnection,
    ConMWEvent_DownloadStatUpdate,
    ConMWEvent_DownloadAborted,
    ConMWEvent_DownloadDone,
    ConMWEvent_HexDisplayUpdate,
    ConMWEvent_BridgeStateChange,
    ConMWEvent_HexDisplayBufferChange,
    ConMWEvent_SelectionChanged,
    ConMWEvent_AutoReopenChanged,
    ConMWEventMAX
} ConMWEventType;

struct ConMWNameChangeData
{
    const char *NewName;
};

struct ConMWStopWatchData
{
    uint64_t LapTime;
    uint64_t LapDelta;
};

struct ConMWHexDisplayData
{
    const uint8_t *Buffer;
    const uint8_t *InsertPos;
    bool BufferIsCircular;          // Do we start at 'Buffer' or 'InsertPos+1'?
    int BufferSize;
};

struct ConMWBridgeData
{
    class Connection *BridgedTo;
};

struct ConAutoReopenData
{
    bool Enabled;
};

union ConMWInfo
{
    struct ConMWNameChangeData NameChange;
    struct ConMWStopWatchData SW;
    struct ConMWHexDisplayData HexDis;
    struct ConMWBridgeData Bridged;
    struct ConAutoReopenData AutoReopen;
};

struct ConMWEvent
{
    ConMWEventType EventType;
    class Connection *Con;
    const union ConMWInfo *Info;
};

/***  CLASS DEFINITIONS                ***/
class Connection
{
    friend void Con_DelayTransmitTimeout(uintptr_t UserData);
    friend void Con_SmartClipTimeout(uintptr_t UserData);
    friend void Con_AutoReopenTimeout(uintptr_t UserData);
    friend void Con_ComTestTimeout(uintptr_t UserData);
    friend void Con_FileTransTick(void);
    friend bool Con_DisplayBufferEvent(const struct DBEvent *Event);

    public:
void Debug1(void);void Debug2(void);void Debug3(void);void Debug4(void);void Debug5(void);void Debug6(void);
        Connection(const char *URI);
        ~Connection();
        bool Init(class TheMainWindow *MainWindow,void *ParentWidget,
                class ConSettings *SourceSettings);
        void FinalizeNewConnection(void);
        void ReParentWidget(void *NewParentWidget);
        bool ApplySettings(void);
        bool SetConnection(const std::string &UniqueID);
        bool SetConnectionBasedOnURI(const char *URI);
        bool SetConnectionOptions(const t_KVList &Options);
        bool GetConnectionOptions(t_KVList &Options);
        void SetMainWindow(class TheMainWindow *MainWindow);
        bool AddTab(void);
        void SetDisplayName(const char *Name);
        void GetDisplayName(std::string &Name);
        void TextCanvasResize(int Width,int Height);
        bool KeyPress(uint8_t Mods,e_UIKeys Key,const uint8_t *TextPtr,unsigned int TextLen);
        e_ConWriteType WriteData(const uint8_t *Data,int Bytes,e_ConWriteSourceType Source);
        void TransmitQueuedData(void);
        void WriteChar2Display(uint8_t *Chr);
        void HandleMiddleMousePress(int x,int y);
        void GetConnectionUniqueID(std::string &UniqueID);
        void GetCaptureOptions(struct CaptureToFileOptions &Options);
        void SetCaptureOption_Timestamp(bool On);
        void SetCaptureOption_Append(bool On);
        void SetCaptureOption_StripCtrl(bool On);
        void SetCaptureOption_StripEsc(bool On);
        void SetCaptureOption_HexDump(bool On);
        bool GetCaptureSaving(void);
        bool StartCapture(void);
        void StopCapture(void);
        void SetCaptureFilename(const char *Filename);
        void GetCaptureFilename(std::string &Filename);
        void GetStopWatchOptions(bool &AutoStartOn,bool &AutoLapOn);
        void SetStopWatchOptions(bool AutoStartOn,bool AutoLapOn);
        bool GetStopWatchRunning(void);
        void StopWatchStartStop(bool Start);
        void StopWatchReset(void);
        uint64_t StopWatchGetTime(void);
        void StopWatchTakeLap(void);
        void StopWatchClearLaps(void);
        bool StopWatchGetNextLapInfo(bool First,uint64_t &LapTime);
        void GetUploadFilename(std::string &Filename);
        void SetUploadFilename(const char *Filename);
        void SetUploadProtocol(const char *NewProtocol);
        void GetUploadProtocol(std::string &SelectedProtocol);
        t_KVList *GetUploadOptionsPtr(void);
        e_FileTransErrType StartUpload(void);
        void AbortUpload(void);
        struct UploadStats const *UploadGetStats(void);
        void UploadSetNumberOfBytesSent(uint64_t BytesTransfered);
        void DownloadSetNumberOfBytesRecv(uint64_t BytesTransfered);
        bool GetHexDisplayPaused(void);
        bool SetHexDisplayPaused(bool Paused,bool Force=false);
        void HexDisplayGetBufferInfo(const uint8_t **Buffer,const uint8_t **InsertPos,bool *BufferIsCircular,int *BufferSize);
        void HexDisplayClear(void);
        void SetLockOutConnectionWhenBridged(bool Lockout);
        bool GetLockOutConnectionWhenBridged(void);
        void BridgeConnection(class Connection *Con);
        class Connection *GetBridgedConnection(void);
        void SetBridgeFrom(class Connection *Con);
        void BridgeConnectionFreeing(void);
        bool GetShowNonPrintable(void);
        void SetShowNonPrintable(bool Show);
        bool GetShowEndOfLines(void);
        void SetShowEndOfLines(bool Show);
        void SelectAll(void);
        bool IsThereASelection(void);
        void ZoomIn(void);
        void ZoomOut(void);
        void ResetZoom(void);
        void DoBell(bool VisualOnly);
        void ToggleAutoReopen(void);
        bool GetCurrentAutoReopenStatus(void);
        void ApplyAttribs2Selection(uint32_t Attribs);
        bool IsThisAttribInSelection(uint32_t Attrib);
        void ApplyBGColor2Selection(uint32_t Color);

        void InformOfConnected(void);
        void InformOfDisconnected(void);
        bool InformOfDataAvaiable(void);
        struct ProcessorConData *GetCurrentProcessorData(void);
        bool GetConnectedStatus(void);
        void SetFGColor(uint32_t FGColor);
        uint32_t GetFGColor(void);
        void SetBGColor(uint32_t BGColor);
        uint32_t GetBGColor(void);
        void SetULineColor(uint32_t ULineColor);
        uint32_t GetULineColor(void);
        void SetAttribs(uint32_t Attribs);
        uint32_t GetAttribs(void);
        void DoFunction(e_ConFuncType Fn,uintptr_t Arg1,uintptr_t Arg2,
                uintptr_t Arg3,uintptr_t Arg4,uintptr_t Arg5,uintptr_t Arg6);
        void GetCursorXY(int *RetCursorX,int *RetCursorY);
        bool InsertString(uint8_t *Str,uint32_t Len);
        void GetScreenSize(int32_t *RetRows,int32_t *RetColumns);
        void SetConnectedState(bool Connected);
        void ToggleConnectedState(void);
        void SendResizeEvent2Siblings(int Width,int Height);
        class TheMainWindow *GetMainWindowHandle(void);
        t_IOSystemHandle *GetIOHandle(void);

        void GiveFocus(void);
        void CopySelectionToClipboard(void);
        void PasteFromClipboard(void);
        void FindCRCFromSelection(void);
        void GotoColumn(int Column);
        void GotoRow(int Row);
        bool GetURI(std::string &URI);
        void FileTransSetTimeout(uint32_t MSec);
        void FileTransRestartTimeout(void);
        void FinishedUpload(bool Aborted);
        void FinishedDownload(bool Aborted);
        void SetDownloadFileName(const char *Filename);
        const char *GetDownloadFileName(void);
        void SetLeftPanelInfo(e_LeftPanelTabType SelectedTab);
        e_LeftPanelTabType GetLeftPanelInfo(void);
        void SetRightPanelInfo(e_RightPanelTabType SelectedTab);
        e_RightPanelTabType GetRightPanelInfo(void);
        void SetBottomPanelInfo(e_BottomPanelTabType SelectedTab);
        e_BottomPanelTabType GetBottomPanelInfo(void);
        void SetCustomSettings(class ConSettings &NewSettings);
        void ApplyCustomSettings(void);
        void Connect2Bookmark(int BookmarkUID);
        int GetConnectionBookmark(void);
        void ClearScreen(void);
        void ClearScrollBackBuffer(void);
        void InsertHorizontalRule(void);
        void ResetTerm(void);

        void SetDownloadProtocol(const char *NewProtocol);
        void GetDownloadProtocol(std::string &SelectedProtocol);
        t_KVList *GetDownloadOptionsPtr(void);
        e_FileTransErrType StartDownload(void);
        void AbortDownload(void);
        struct DownloadStats const *DownloadGetStats(void);

        unsigned int GetTransmitDelayPerByte(void);
        unsigned int GetTransmitDelayPerLine(void);
        void SetTransmitDelayPerByte(unsigned int ms);
        void SetTransmitDelayPerLine(unsigned int ms);

        bool IsConnectionBinary(void);
        t_UIContextMenuCtrl *GetContextMenuHandle(e_UITD_ContextMenuType UIObj);
        t_UIContextSubMenuCtrl *GetContextSubMenuHandle(e_UITD_ContextSubMenuType UIObj);

        /* Com Test */
        void SetupComTest(bool Sender,uint32_t PacketLen,uint32_t PacketsCount,uint32_t Delay,uint8_t *PacketData);
        bool StartComTest(void);
        void StopComTest(void);
        bool IsComTestRunning(void);
        void ComTestFastTick(void);
        const struct ComTestStats *GetComTestStats(void);
        void RegisterComTestUpdateFn(void (*Update)(class Connection *));

        bool UsingCustomSettings;
        class ConSettings CustomSettings;

    private:
        void *OurParentWidget;
        t_IOSystemHandle *IOHandle;
        class DisplayBase *Display;
        class TheMainWindow *MW;
        struct ProcessorConData ProcessorData;
        t_FTPData *FTPConData;
        bool IsConnected;
        char DisplayName[MAX_CONNECTION_NAME_LEN+1];   // The name of this connection
        struct CaptureToFileType CaptureToFile;
        struct StopWatchType StopWatch;
        struct UploadType Upload;
        struct DownloadType Download;
        struct HexDisplayType HexDisplay;
        struct ComTestType ComTest;
        e_LeftPanelTabType LeftPanelInfo;
        e_RightPanelTabType RightPanelInfo;
        e_BottomPanelTabType BottomPanelInfo;
        unsigned int TransmitDelayByte;
        unsigned int TransmitDelayLine;
        uint8_t *TransmitDelayBuffer;
        unsigned int TransmitDelayBufferSize;
        unsigned int TransmitDelayBufferWritePos;
        unsigned int TransmitDelayBufferReadPos;
        struct UITimer *TransmitDelayTimer;
        struct UITimer *SmartClipTimer;
        struct UITimer *AutoReopenTimer;
        bool BlockSendDevice;
        bool WhenBridgedLockoutConnection;
        bool ConnectionLockedOut;
        bool ShowNonPrintables;
        bool ShowEndOfLines;
        class Connection *BridgedTo;        // We send data to this one
        class Connection *BridgedFrom;      // We are getting data from this one
        int Bookmark;   // What bookmark is connected to this connection
        int ZoomLevel;
        int FontSize;
        bool BinaryConnection;
        uint64_t LastBellPlayed;
        bool DoAutoReopen;

        void FreeConnectionResources(bool FreeDB);
        void HandleCaptureIncomingData(const uint8_t *Inbuff,int bytes);
        void SendMWEvent(ConMWEventType Event,union ConMWInfo *ExtraInfo=NULL);
        void StopWatchHandleAutoLap(void);
        void HandleHexDisplayIncomingData(const uint8_t *inbuff,int Bytes);
        void HandleComTestRx(uint8_t *inbuff,int bytes);
        bool QueueTransmitDelayData(const uint8_t *Data,int Bytes);
        void ApplyTransmitDelayChange(void);
        e_ConWriteType InternalWriteBytes(const uint8_t *Data,int Bytes);
        void FreeTransmitDelayBuffer(void);
        void RethinkLockOut(void);
        void RethinkCursor(void);
        void HandleMouseWheelZoom(int Steps);
        e_CmdType HandleSmartClipboard(char key);
        bool IsProcessorATextProcessor(struct ProcessorConData &PData);
        void SendReopenChangeEvent(void);
        void DoAutoReopenIfNeeded(void);
        void HandleFailed2OpenErrorMessage(void);

        /* Call backs */
        void InformOfDelayTransmitTimeout(void);
        void InformOfComTestTimeout(void);
        void InformOfSmartClipTimeout(void);
        void InformOfAutoReopenTimeout(void);
        void FileTransTick(void);
        bool ProcessDisplayEvent(const struct DBEvent *Event);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
class Connection *Con_AllocateConnection(const char *URI);
void Con_FreeConnection(class Connection *Con);
bool Con_TextCavnasEvent(const struct TCEvent *Event);
void Con_FileTransTick(void);
void Con_ApplySettings2AllConnections(void);

#endif
