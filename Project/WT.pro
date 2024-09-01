#-------------------------------------------------
#
# Project created by QtCreator 2016-03-28T11:05:51
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = WhippyTerm
TEMPLATE = app

INCLUDEPATH = \
../src \
../src/UI/QT

CONFIG += depend_includepath

QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter -Wno-nonnull-compare
QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-implicit-fallthrough -Wswitch-default -Wswitch-enum
QMAKE_CFLAGS += -Wno-unused-parameter -Wno-implicit-fallthrough-Wswitch-default -Wswitch-enum

DEFINES -= UNICODE
DEFINES += __STDC_LIMIT_MACROS
DEFINES += BUILT_IN_PLUGINS=1

QMAKE_CXXFLAGS += -Wall
QMAKE_CFLAGS += -Wall

CONFIG(debug, debug|release)
{
    DEFINES += DEBUG=1
}

win32 {
DEFINES += __STDC_FORMAT_MACROS
DEFINES += DEBUGWINDOWSBUILD
QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++
RC_ICONS = ..\WindowsInstaller\WhippyTerm.ico
}

SOURCES += ../src/UI/QT/main.cpp\
    ../ExternPlugins/DataProcessors/TermEmulation/BasicHex/src/BasicHex.cpp \
    ../src/App/Dialogs/Dialog_SendByte.cpp \
    ../src/App/MWPanels/MW_AuxControls.cpp \
    ../src/App/StdPlugins/IODrivers/TCPClient/src/TCPClient_Main.cpp \
    ../src/App/StdPlugins/IODrivers/TCPServer/src/TCPServer_Main.cpp \
    ../src/App/StdPlugins/IODrivers/UDPClient/src/UDPClient_Main.cpp \
    ../src/App/StdPlugins/IODrivers/UDPServer/src/UDPServer_Main.cpp \
    ../src/UI/QT/Form_DebugPrintLog.cpp \
    ../src/UI/QT/Form_DebugPrintLogAccess.cpp \
    ../src/UI/QT/Form_MainWindow.cpp \
    ../src/UI/QT/Form_SendByte.cpp \
    ../src/UI/QT/Form_SendByteAccess.cpp \
    ../src/UI/QT/Frame_TextCavnasOverrideBox.cpp \
    ../src/UI/QT/VerPanelHandle.cpp \
    ../src/UI/QT/HozPanelHandle.cpp \
    ../src/UI/QT/Widget_MovableTabWidget.cpp \
    ../src/App/MainApp.cpp \
    ../src/UI/QT/Form_MainWindowAccess.cpp \
    ../src/App/MainWindow.cpp \
    ../src/UI/QT/AskMessageBox.cpp \
    ../src/UI/QT/Widget_CloseBttn.cpp \
    ../src/App/Session.cpp \
    ../src/App/Util/StorageHelpers.cpp \
    ../src/App/PluginSupport/KeyValueSupport.cpp \
    ../src/App/Settings.cpp \
    ../src/App/Display/DisplayColors.cpp \
    ../src/UI/QT/UIControl.cpp \
    ../src/UI/QT/DebugUtils.cpp \
    ../src/UI/QT/Form_FontDialog.cpp \
    ../src/UI/QT/Form_FontDialogAccess.cpp \
    ../src/App/Dialogs/Dialog_Settings.cpp \
    ../src/UI/QT/Form_SettingsAccess.cpp \
    ../src/UI/QT/Form_Settings.cpp \
    ../src/App/Dialogs/Dialog_NewConnection.cpp \
    ../src/UI/QT/Form_NewConnection.cpp \
    ../src/UI/QT/Form_NewConnectionAccess.cpp \
    ../src/App/Connections.cpp \
    ../src/App/IOSystem.cpp \
    ../src/App/StdPlugins/RegisterStdPlugins.cpp \
    ../src/App/PluginSupport/PluginUISupport.cpp \
    ../src/UI/QT/QTPlugins.cpp \
    ../src/App/PluginSupport/SystemSupport.cpp \
    ../src/ThirdParty/strnatcmp/strnatcmp.cpp \
    ../src/App/DataProcessorsSystem.cpp \
    ../src/App/StdPlugins/DataProcessors/TermEmulation/ANSIX3_64.cpp \
    ../src/App/ConnectionsGlobal.cpp \
    ../src/App/StdPlugins/IODrivers/Comport/Comport_Main.cpp \
    ../src/App/StdPlugins/IODrivers/Comport/Comport_ConnectionOptions.cpp \
    ../src/App/StdPlugins/DataProcessors/TermEmulation/BasicCtrlCharsDecoder.cpp \
    ../src/App/StdPlugins/DataProcessors/CharEncoding/CodePage437Decoder.cpp \
    ../src/App/StdPlugins/DataProcessors/CharEncoding/UnicodeDecoder.cpp \
    ../src/UI/QT/FileRequesters.cpp \
    ../src/UI/QT/Form_DebugStats.cpp \
    ../src/UI/QT/ColorRequesters.cpp \
    ../src/UI/QT/Form_About.cpp \
    ../src/App/Dialogs/Dialog_About.cpp \
    ../src/UI/QT/Form_AboutAccess.cpp \
    ../src/UI/QT/Clipboard.cpp \
    ../src/App/Commands.cpp \
    ../src/UI/QT/QTKeyMappings.cpp \
    ../src/App/KeySeqs.cpp \
    ../src/UI/QT/Form_ChangeConnectionName.cpp \
    ../src/App/Dialogs/Dialog_ChangeConnectionName.cpp \
    ../src/UI/QT/Form_ChangeConnectionNameAccess.cpp \
    ../src/App/Bookmarks.cpp \
    ../src/ThirdParty/TinyCFG/TinyCFG.cpp \
    ../src/UI/QT/Form_AddBookmark.cpp \
    ../src/App/Dialogs/Dialog_AddBookmark.cpp \
    ../src/UI/QT/Form_AddBookmarkAccess.cpp \
    ../src/UI/QT/TextInputBox.cpp \
    ../src/UI/QT/Form_ManBookmark.cpp \
    ../src/UI/QT/Form_ManBookmarkAccess.cpp \
    ../src/App/Dialogs/Dialog_ManBookmark.cpp \
    ../src/App/MWPanels/MW_ConnectionOptions.cpp \
    ../src/App/MWPanels/MW_StopWatch.cpp \
    ../src/App/MWPanels/MW_Capture.cpp \
    ../src/App/StdPlugins/FileTransfersProtocols/RAWFile/RAWFileUpload.cpp \
    ../src/App/FileTransferProtocolSystem.cpp \
    ../src/App/MWPanels/MW_Upload.cpp \
    ../src/App/StdPlugins/FileTransfersProtocols/XModem/XModem.cpp \
    ../src/App/MWPanels/MW_Download.cpp \
    ../src/App/Display/HexDisplayBuffers.cpp \
    ../src/App/MWPanels/MW_HexDisplay.cpp \
    ../src/UI/QT/Form_HexDisplayCopyAs.cpp \
    ../src/App/Dialogs/Dialog_HexDisplayCopyAs.cpp \
    ../src/UI/QT/Form_HexDisplayCopyAsAccess.cpp \
    ../src/App/MWPanels/MW_SendBuffers.cpp \
    ../src/App/SendBuffer.cpp \
    ../src/ThirdParty/SimpleRIFF/RIFF.cpp \
    ../src/UI/QT/Form_EditSendBufferAccess.cpp \
    ../src/UI/QT/Form_EditSendBuffer.cpp \
    ../src/App/Dialogs/Dialog_EditSendBuffer.cpp \
    ../src/UI/QT/Form_ESB_CRCType.cpp \
    ../src/App/Dialogs/Dialog_ESB_CRCType.cpp \
    ../src/UI/QT/Form_ESB_CRCTypeAccess.cpp \
    ../src/UI/QT/Form_ESB_ViewSource.cpp \
    ../src/App/Dialogs/Dialog_ESB_ViewSource.cpp \
    ../src/UI/QT/Form_ESB_ViewSourceAccess.cpp \
    ../src/App/Util/CRCSystem.cpp \
    ../src/UI/QT/Form_ESB_Fill.cpp \
    ../src/UI/QT/Form_ESB_FillAccess.cpp \
    ../src/App/Dialogs/Dialog_ESB_Fill.cpp \
    ../src/UI/QT/Form_ConnectionOptions.cpp \
    ../src/App/Dialogs/Dialog_ConnectionOptions.cpp \
    ../src/UI/QT/Form_ConnectionOptionsAccess.cpp \
    ../src/UI/QT/Form_ComTest.cpp \
    ../src/UI/QT/Form_ComTestAccess.cpp \
    ../src/App/Dialogs/Dialog_ComTest.cpp \
    ../src/UI/QT/UITimers.cpp \
    ../src/UI/QT/Form_TransmitDelay.cpp \
    ../src/UI/QT/Form_TransmitDelayAccess.cpp \
    ../src/App/Dialogs/Dialog_TransmitDelay.cpp \
    ../src/App/PluginSupport/ExternPluginsSystem.cpp \
    ../src/UI/QT/Form_ManagePlugins.cpp \
    ../src/App/Dialogs/Dialog_ManagePlugins.cpp \
    ../src/UI/QT/Form_ManagePluginsAccess.cpp \
    ../src/UI/QT/Form_InstallPlugin.cpp \
    ../src/App/Dialogs/Dialog_InstallPlugin.cpp \
    ../src/UI/QT/Form_InstallPluginAccess.cpp \
    ../src/App/MWPanels/MW_Bridge.cpp \
    ../src/UI/QT/Form_BridgeConnection.cpp \
    ../src/UI/QT/Form_BridgeConnectionAccess.cpp \
    ../src/App/Dialogs/Dialog_Bridge.cpp \
    ../src/UI/QT/Form_ESB_InsertProp.cpp \
    ../src/UI/QT/Form_ESB_InsertPropAccess.cpp \
    ../src/App/Dialogs/Dialog_ESB_InsertProp.cpp \
    ../src/UI/QT/Form_PasteData.cpp \
    ../src/App/Dialogs/Dialog_PasteData.cpp \
    ../src/UI/QT/Form_PasteDataAccess.cpp \
    ../src/App/Util/ClipboardHelpers.cpp \
    ../src/App/Display/DisplayBase.cpp \
    ../src/App/Display/DisplayText.cpp \
    ../src/App/Display/DisplayBinary.cpp \
    ../src/UI/QT/Frame_MainTextArea.cpp \
    ../src/UI/QT/Widget_TextCanvas.cpp \
    ../src/UI/QT/Frame_MainTextAreaAccess.cpp \
    ../src/App/Util/TextStyleHelpers.cpp \

win32 {
# Windows
    LIBS+=-lSetupApi
#DEBUG PAUL: Temp.  Needed for a plugin
    LIBS+=-lws2_32

    SOURCES += \
    ../src/OS/Windows/FilePaths.cpp \
    ../src/OS/Windows/Directorys.cpp \
    ../src/OS/Windows/OSTime.cpp \
    ../src/OS/Windows/System.cpp \
    ../src/App/StdPlugins/IODrivers/Comport/OS/Win/Comport_OS_Serial.cpp \
    ../src/App/StdPlugins/IODrivers/TCPClient/src/OS/Win/TCPClient_OS_Socket.cpp \
    ../src/App/StdPlugins/IODrivers/TCPServer/src/OS/Win/TCPServer_OS_Socket.cpp \
    ../src/App/StdPlugins/IODrivers/UDPClient/src/OS/Win/UDPClient_OS_Socket.cpp \
    ../src/App/StdPlugins/IODrivers/UDPServer/src/OS/Win/UDPServer_OS_Socket.cpp \
}

unix:!macx {
# Linux
    LIBS+=-ldl

     SOURCES += \
    ../src/OS/Linux/FilePaths.cpp \
    ../src/OS/Linux/Directorys.cpp \
    ../src/OS/Linux/OSTime.cpp \
    ../src/OS/Linux/System.cpp \
    ../src/App/StdPlugins/IODrivers/Comport/OS/Linux/Comport_OS_Serial.cpp \
    ../src/App/StdPlugins/IODrivers/TCPClient/src/OS/Linux/TCPClient_OS_Socket.cpp \
    ../src/App/StdPlugins/IODrivers/TCPServer/src/OS/Linux/TCPServer_OS_Socket.cpp \
    ../src/App/StdPlugins/IODrivers/UDPClient/src/OS/Linux/UDPClient_OS_Socket.cpp \
    ../src/App/StdPlugins/IODrivers/UDPServer/src/OS/Linux/UDPServer_OS_Socket.cpp \

    CONFIG(debug, debug|release)
    {
        SOURCES += \
            ../ExternPlugins/IODrivers/TestLoopback/src/LB2_Main.cpp \
    }
}

macx {
# Mac OS X
     SOURCES += \
    ../src/OS/Mac/FilePaths.cpp \
    ../src/OS/Mac/Directorys.cpp \
    ../src/OS/Mac/OSTime.cpp \
    ../src/OS/Mac/System.cpp \
    ../src/App/StdPlugins/IODrivers/Comport/OS/Mac/Comport_OS_Serial.cpp \
}

HEADERS  += ../src/UI/QT/Form_MainWindow.h \
    ../src/UI/QT/Form_DebugPrintLog.h \
    ../src/UI/QT/Form_SendByte.h \
    ../src/UI/QT/Frame_TextCavnasOverrideBox.h \
    ../src/UI/QT/VerPanelHandle.h \
    ../src/UI/QT/HozPanelHandle.h \
    ../src/UI/QT/Widget_MovableTabWidget.h \
    ../src/UI/QT/main.h \
    ../src/UI/QT/Widget_CloseBttn.h \
    ../src/UI/QT/AskMessageBox.h \
    ../src/UI/QT/Form_FontDialog.h \
    ../src/UI/QT/Form_Settings.h \
    ../src/UI/QT/Form_NewConnection.h \
    ../src/UI/QT/QTPlugins.h \
    ../src/UI/QT/Form_DebugStats.h \
    ../src/UI/QT/Form_About.h \
    ../src/UI/QT/Form_ChangeConnectionName.h \
    ../src/UI/QT/Form_AddBookmark.h \
    ../src/UI/QT/Form_ManBookmark.h \
    ../src/UI/QT/Form_HexDisplayCopyAs.h \
    ../src/UI/QT/Form_EditSendBuffer.h \
    ../src/UI/QT/Form_ESB_CRCType.h \
    ../src/UI/QT/Form_ESB_ViewSource.h \
    ../src/UI/QT/Form_ESB_Fill.h \
    ../src/UI/QT/Form_ConnectionOptions.h \
    ../src/UI/QT/Form_ComTest.h \
    ../src/UI/QT/UITimers.h \
    ../src/UI/QT/Form_TransmitDelay.h \
    ../src/UI/QT/Form_ManagePlugins.h \
    ../src/UI/QT/Form_InstallPlugin.h \
    ../src/UI/QT/Frame_MainTextArea.h \
    ../src/UI/QT/Form_BridgeConnection.h \
    ../src/UI/QT/Form_ESB_InsertProp.h \
    ../src/UI/QT/Form_PasteData.h \
    ../src/UI/QT/Widget_TextCanvas.h

FORMS    += ../src/UI/QT/Form_MainWindow.ui \
    ../src/UI/QT/Form_DebugPrintLog.ui \
    ../src/UI/QT/Form_FontDialog.ui \
    ../src/UI/QT/Form_SendByte.ui \
    ../src/UI/QT/Form_Settings.ui \
    ../src/UI/QT/Form_NewConnection.ui \
    ../src/UI/QT/Form_DebugStats.ui \
    ../src/UI/QT/Form_About.ui \
    ../src/UI/QT/Form_ChangeConnectionName.ui \
    ../src/UI/QT/Form_AddBookmark.ui \
    ../src/UI/QT/Form_ManBookmark.ui \
    ../src/UI/QT/Form_HexDisplayCopyAs.ui \
    ../src/UI/QT/Form_EditSendBuffer.ui \
    ../src/UI/QT/Form_ESB_CRCType.ui \
    ../src/UI/QT/Form_ESB_ViewSource.ui \
    ../src/UI/QT/Form_ESB_Fill.ui \
    ../src/UI/QT/Form_ConnectionOptions.ui \
    ../src/UI/QT/Form_ComTest.ui \
    ../src/UI/QT/Form_TransmitDelay.ui \
    ../src/UI/QT/Form_ManagePlugins.ui \
    ../src/UI/QT/Form_InstallPlugin.ui \
    ../src/UI/QT/Frame_MainTextArea.ui \
    ../src/UI/QT/Form_BridgeConnection.ui \
    ../src/UI/QT/Form_ESB_InsertProp.ui \
    ../src/UI/QT/Form_PasteData.ui \
    ../src/UI/QT/Frame_TextCavnasOverrideBox.ui

RESOURCES += \
    ../src/UI/QT/MainResource.qrc
