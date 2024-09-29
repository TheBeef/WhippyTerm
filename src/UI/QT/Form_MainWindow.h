#ifndef FORM_MAINWINDOW_H
#define FORM_MAINWINDOW_H

#include "UI/UIMainWindow.h"
#include <QLineEdit>
#include <QLabel>
#include <QAction>
#include <QMainWindow>
#include <QTimer>
#include <QAction>
#include <QMenu>
#include <QKeyEvent>
#include <list>

namespace Ui {
class Form_MainWindow;
}

typedef enum
{
    e_Panel_Bottom,
    e_Panel_Left,
    e_Panel_Right,
    e_PanelMAX
} e_PanelType;

typedef std::list<QAction *> t_MWListOfActionsType;
typedef std::list<QMenu *> t_MWListOfMenusType;

class Form_MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit Form_MainWindow(QWidget *parent = 0);
    ~Form_MainWindow();
    void Setup(class TheMainWindow *MW);
    void keyPressEvent(QKeyEvent * event);
    void focusInEvent(QFocusEvent * event);
    void focusOutEvent(QFocusEvent * event);
    bool focusNextPrevChild(bool next);
    Ui::Form_MainWindow *ui;
    void PanelClicked(e_PanelType Panel);
    void EnableBottomSplitter(bool Enable);
    void EnableLeftSplitter(bool Enable);
    void EnableRightSplitter(bool Enable);
    int BottomPanelLastSize;
    int LeftPanelLastSize;
    int RightPanelLastSize;
    bool BottomClosed;
    bool LeftClosed;
    bool RightClosed;
    QLineEdit *ToolBarURI_LineEdit;
    t_MWListOfActionsType BookmarkMenuItems;
    t_MWListOfMenusType BookmarkMenus;
    t_MWListOfActionsType FTPUploadMenuItems;
    t_MWListOfActionsType FTPDownloadMenuItems;
    t_MWListOfActionsType ApplyTerminalEmulationMenuItems;
    QTimer *StopWatchTimer;

public slots:
    void on_TopBottomSplitter_splitterMoved(int pos, int index);
    void on_LeftRightSplitter_splitterMoved(int pos, int index);

private slots:
    void URIreturnPressed();
    void URIEdited(const QString &arg1);
    void on_actionQuit_triggered();
    void on_actionAbout_Whippy_Term_triggered();
    void on_actionTest1_triggered();
    void on_actionTest2_triggered();
    void GlobalCloseTabBttnClicked(void);
    void StopWatchTimer_triggered();

    void on_Main_tabWidget_TabMoved(int from,int to);
    void on_actionComPort_TEST_triggered();
    void on_actionSettings_triggered();
    void on_actionImport_Settings_triggered();
    void on_actionExport_Settings_triggered();
    void on_Main_tabWidget_tabCloseRequested(int index);
    void on_actionClose_Tab_triggered();
    void on_actionClose_All_triggered();
    void on_actionTest4_triggered();
    void on_actionTest5_triggered();
    void on_actionTest6_triggered();
    void on_actionFont_Request_Test_triggered();
    void on_actionNew_Tab_triggered();
    void on_actionConnect_Menu_triggered();
    void on_actionDisconnect_Menu_triggered();
    void on_Main_tabWidget_currentChanged(int index);
    void on_actionNew_Tab_Toolbar_triggered();
    void on_actionInsert_Horizontal_Rule_triggered();
    void on_actionClear_Screen_triggered();
    void on_actionConnectToggle_triggered(bool checked);
    void on_actionMenu_Change_Name_triggered();
    void on_pushButton_SW_Start_clicked();
    void on_pushButton_SW_Reset_clicked();
    void on_pushButton_SW_Lap_clicked();
    void on_checkBox_SW_StartOnTx_clicked(bool checked);
    void on_checkBox_SW_AutoLap_clicked(bool checked);
    void on_actionStats_triggered();
    void on_actionCopy_triggered();
    void on_actionPaste_triggered();
    void on_pushButton_CO_Apply_clicked();
    void on_actionactionURIGo_triggered();
    void on_actionBookmark_this_connection_triggered();
    void actionBookmarkMenuItem_triggered();
    void actionFTPUploadMenuItem_triggered();
    void actionApplyTerminalEmulationMenuItem_triggered();
    void actionFTPDownloadMenuItem_triggered();
    void on_actionManage_Bookmarks_triggered();
    
    void on_pushButton_SW_Clear_clicked();
    
    void on_actionStopWatch_Start_triggered();
    
    void on_actionStopWatch_Stop_triggered();
    
    void on_actionStopWatch_Reset_triggered();
    
    void on_actionStopWatch_Lap_triggered();
    
    void on_actionStopWatch_Clear_triggered();
    
    void on_actionStopWatch_Start_On_Tx_triggered();
    
    void on_actionStopWatch_Auto_Lap_triggered();
    
    void on_lineEdit_Cap_FileName_editingFinished();
    
    void on_lineEdit_UploadFilename_editingFinished();
    
    void on_pushButton_Cap_GetFilename_clicked();
    
    void on_checkBox_Cap_Timestamp_clicked(bool checked);
    
    void on_checkBox_Cap_Append_clicked(bool checked);
    
    void on_checkBox_Cap_StripCtrlChars_clicked(bool checked);
    
    void on_checkBox_Cap_StripEscSeq_clicked(bool checked);
    
    void on_pushButton_Cap_Start_clicked();
    
    void on_actionCapture_To_File_triggered();
    
    void on_actionTimestamp_new_lines_triggered();
    
    void on_actionAppend_To_Existing_File_triggered();
    
    void on_actionStrip_Control_Char_s_triggered();
    
    void on_actionStrip_Esc_Sequences_triggered();
    
    void on_actionCapture_Stop_triggered();
    
    void on_checkBox_Cap_HexDump_clicked(bool checked);
    
    void on_actionHex_Dump_triggered();
    
    void on_pushButton_UploadSelectFilename_clicked();
    
    void on_pushButton_UploadStart_clicked();
    
    void on_comboBox_UploadProtocol_activated(int index);
    
    void on_pushButton_UploadAbort_clicked();
    
    void on_pushButton_DownloadStart_clicked();
    
    void on_pushButton_DownloadAbort_clicked();
    
    void on_comboBox_DownloadProtocol_activated(int index);
    
    void on_BottomPanel_tabWidget_currentChanged(int index);
    
    void on_LeftPanel_tabWidget_currentChanged(int index);
    
    void on_RightPanel_tabWidget_currentChanged(int index);
    
    void on_pushButton_HexClear_clicked();
    
    void on_pushButton_HexCopy_clicked();
    
    void on_pushButton_HexCopyAs_clicked();
    
    void on_checkBox_HexPaused_clicked(bool checked);
    
    void on_pushButton_Buffer_Edit_clicked();
    
    void on_pushButton_Buffer_Send_clicked();
    
    void on_actionConnection_Options_triggered();
    
    void on_actionCom_Test_triggered();
    
    void on_actionTransmit_delay_triggered();
    
    void on_actionManage_Plugins_triggered();
    
    void on_actionInstall_Plugin_triggered();
    
    void on_pushButton_Bridge_Bridge_clicked();
    
    void on_pushButton_Bridge_Release_clicked();
    
    void on_comboBox_Bridge_Con1_activated(int index);
    
    void on_comboBox_Bridge_Con2_activated(int index);
    
    void on_checkBox_Bridge_Lockout1_clicked(bool checked);
    
    void on_checkBox_Bridge_Lockout2_clicked(bool checked);
    
    void on_actionBridge_Connections_triggered();
    
    void on_actionRelease_Bridged_Connections_triggered();
    
    void on_actionGet_Plugins_triggered();
    
    void on_actionConnection_Settings_triggered();
    
    void on_actionRestore_Connection_Settings_to_Defaults_triggered();
    
    void on_actionShow_NonPrintables_triggered();
    
    void on_actionShow_End_Of_Lines_triggered();
    
    void on_actionClear_Scroll_Buffer_triggered();
    
    void on_actionReset_triggered();
    
    void on_actionGoto_Column_triggered();
    
    void on_actionGoto_Line_triggered();
    
    void on_actionSelect_All_triggered();
    
    void on_actionZoom_In_triggered();
    
    void on_actionZoom_Out_triggered();
    
    void on_actionReset_Zoom_triggered();
    
    void on_actionSend_NULL_triggered();
    
    void on_actionSend_Backspace_triggered();
    
    void on_actionSend_Tab_triggered();
    
    void on_actionSend_Line_Feed_triggered();
    
    void on_actionSend_Form_Feed_triggered();
    
    void on_actionSend_Carriage_Return_triggered();
    
    void on_actionSend_Other_triggered();
    
    void on_actionSend_Escape_triggered();
    
    void on_actionSend_Delete_triggered();
    
    void on_actionCopy_toolbar_triggered();
    
    void on_actionPaste_toolbar_triggered();
    
    void on_actionTerminal_Size_triggered();
    
    void on_actionTerminal_Emulation_triggered();
    
    void on_actionFont_triggered();
    
    void on_actionColors_triggered();
    
    void on_actionShow_printf_log_triggered();
    
    void on_actionSend_Buffer_1_triggered();
    
    void on_actionSend_Buffer_2_triggered();
    
    void on_actionSend_Buffer_3_triggered();
    
    void on_actionSend_Buffer_4_triggered();
    
    void on_actionSend_Buffer_5_triggered();
    
    void on_actionSend_Buffer_6_triggered();
    
    void on_actionSend_Buffer_7_triggered();
    
    void on_actionSend_Buffer_8_triggered();
    
    void on_actionSend_Buffer_9_triggered();
    
    void on_actionSend_Buffer_10_triggered();
    
    void on_actionSend_Buffer_11_triggered();
    
    void on_actionSend_Buffer_12_triggered();
    
    void on_actionSave_Buffer_Set_triggered();
    
    void on_actionLoad_Buffer_Set_triggered();
    
    void on_treeWidget_Buffer_BufferList_itemSelectionChanged();
    
    void on_actionClear_All_Buffers_triggered();
    
    void on_actionSend_Buffer_triggered();
    
    void on_actionEdit_Send_Buffer_triggered();
    
private:
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
    void changeEvent(QEvent *event);
    void moveEvent(QMoveEvent *event);
    void DoMenuTriggered(e_UIMWMenuType MenuID);
    void DoToolbarTriggered(e_UIMWToolbarType ToolID);
    void DoBttnTriggered(e_UIMWBttnType BttnID);
    void DoCheckboxTriggered(e_UIMWCheckboxType CheckboxID,bool Checked);
    void DoTextInputChanged(e_UIMWTxtInputType InputID,const char *FinalText);
    bool FirstShow;
    bool IgnoreResize;
    QLabel *ToolbarURILabel_Label;
    QAction *ToolBarURI;
    QAction *ToolBarURILabel;
    bool IgnoreNextKeyEvent;
    bool SendEvent(e_MWEventType EventType,union MWEventData *Info,uintptr_t ID);
    bool SendEvent(e_MWEventType EventType,union MWEventData *Info);
    bool SendEvent(e_MWEventType EventType);
    class TheMainWindow *MainWindowClassPtr;

void Debug_Send2DebugFn(void (*fn)(uintptr_t ID));
};

#endif // FORM_MAINWINDOW_H
