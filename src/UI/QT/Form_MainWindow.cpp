#include "Form_MainWindow.h"
#include "ui_Form_MainWindow.h"
#include "UI/UIMainWindow.h"
#include "UI/UIAsk.h"
#include "QTKeyMappings.h"
#include "main.h"
#include "VerPanelHandle.h"
#include "Version.h"
#include "main.h"
#include "ContextMenuHelper.h"
#include <stdio.h>
#include <string>
#include <QCloseEvent>

#include "Form_DebugStats.h"

#include "UI/UISettings.h"
#include "UI/UIDebug.h"
#include "UI/UIFontReq.h"

using namespace std;

bool g_DEBUG_DoInsertTest;

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
#include <QToolButton>
#include <QStyleFactory>
#include "App/MainWindow.h"
void Form_MainWindow::on_actionTest1_triggered()
{
    Debug_Send2DebugFn(MWDebug1);
//    MW_Test();
//qApp->setStyle("Windows");
//    UIMW_SwitchTabControlCloseBttnPos(true);
}

void Form_MainWindow::Debug_Send2DebugFn(void (*fn)(uintptr_t ID))
{
    uintptr_t ID;
    QWidget *Tab;

    Tab=ui->Main_tabWidget->widget(ui->Main_tabWidget->currentIndex());
    if(Tab!=0)
    {
        ID=Tab->objectName().toULongLong();
        fn(ID);
    }
}

void Form_MainWindow::on_actionTest2_triggered()
{
    Debug_Send2DebugFn(MWDebug2);
//    MW_Test2();
//    UIMW_SwitchTabControlCloseBttnPos(false);
}

void Form_MainWindow::on_actionTest4_triggered()
{
    Debug_Send2DebugFn(MWDebug4);
//    MW_Test4();
}

void Form_MainWindow::on_actionTest5_triggered()
{
    Debug_Send2DebugFn(MWDebug5);
//    MW_Test5();
}

void Form_MainWindow::on_actionTest6_triggered()
{
    Debug_Send2DebugFn(MWDebug6);
//    g_DEBUG_DoInsertTest=!g_DEBUG_DoInsertTest;
}

//int g_DEBUG_Comlist;
void Form_MainWindow::on_actionComPort_TEST_triggered()
{
//    g_DEBUG_Comlist++;
//    if(g_DEBUG_Comlist>3)
//        g_DEBUG_Comlist=0;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

Form_MainWindow::Form_MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Form_MainWindow)
{
    FirstShow=true;
    IgnoreNextKeyEvent=false;
    QMenu *menu;

    ui->setupUi(this);

    menu=new QMenu();
    menu->addAction(ui->actionToolbarMenu_ColorSel_Black);
    menu->addAction(ui->actionToolbarMenu_ColorSel_Blue);
    menu->addAction(ui->actionToolbarMenu_ColorSel_Green);
    menu->addAction(ui->actionToolbarMenu_ColorSel_Cyan);
    menu->addAction(ui->actionToolbarMenu_ColorSel_Red);
    menu->addAction(ui->actionToolbarMenu_ColorSel_Magenta);
    menu->addAction(ui->actionToolbarMenu_ColorSel_Brown);
    menu->addAction(ui->actionToolbarMenu_ColorSel_White);
    menu->addAction(ui->actionToolbarMenu_ColorSel_Gray);
    menu->addAction(ui->actionToolbarMenu_ColorSel_LightBlue);
    menu->addAction(ui->actionToolbarMenu_ColorSel_LightGreen);
    menu->addAction(ui->actionToolbarMenu_ColorSel_LightCyan);
    menu->addAction(ui->actionToolbarMenu_ColorSel_LightRed);
    menu->addAction(ui->actionToolbarMenu_ColorSel_LightMagenta);
    menu->addAction(ui->actionToolbarMenu_ColorSel_Yellow);
    menu->addAction(ui->actionToolbarMenu_ColorSel_BrightWhite);
    ui->StyleColor_toolButton->setMenu(menu);

    StopWatchTimer=new QTimer(this);
    StopWatchTimer->setInterval(13);   // 13ms so the updates look like they are coming in faster
    connect(StopWatchTimer, SIGNAL(timeout()), this, SLOT(StopWatchTimer_triggered()));

    DelayedThemeChangeTimer=new QTimer(this);
    DelayedThemeChangeTimer->setSingleShot(true);
    connect(DelayedThemeChangeTimer, SIGNAL(timeout()), this, SLOT(DelayedThemeChangeTimer_triggered()));

    ui->treeWidget_Buffer_BufferList->header()->resizeSection(0,256);

    AddContextMenu2Widget(this,ui->treeWidget_Buffer_BufferList);

    RethinkColors();

#if OFFICIAL_RELEASE==1
    /* Hide anything that shouldn't be in the release build */
    ui->menuDebug->menuAction()->setVisible(false);
#endif
}

Form_MainWindow::~Form_MainWindow()
{
    delete ui;
}

void Form_MainWindow::Setup(class TheMainWindow *MW)
{
    MainWindowClassPtr=MW;

    ui->LeftResizeFrame->LeftMode();
    ui->LeftResizeFrame->ChangeArrowDir(true);

    ui->BottomResizeFrame->RegisterClickHandler(this);
    ui->RightResizeFrame->RegisterClickHandler(this);
    ui->LeftResizeFrame->RegisterClickHandler(this);

    /* Make sure the panel handles are on top */
    ui->BottomResizeFrame->raise();
    ui->RightResizeFrame->raise();
    ui->LeftResizeFrame->raise();
}

void Form_MainWindow::RethinkColors(void)
{
    /* Fix the color of the div line between the toolbar and the main area */
    QPalette p=this->palette();
    p.setColor(QPalette::WindowText,this->palette().color(QPalette::Midlight));
//    p.setColor(QPalette::WindowText,Qt::red); // Just for testing, unrem to make sure you have your bar correct
    ui->line_3->setPalette(p);

    /* Fix all the div bars in the toolbar */
    ui->line_6->setPalette(p);
    ui->line_4->setPalette(p);
    ui->line_5->setPalette(p);
    ui->line_7->setPalette(p);
}

/*******************************************************************************
 * NAME:
 *    Form_MainWindow::SendEvent()
 *
 * SYNOPSIS:
 *    bool Form_MainWindow::SendEvent(e_MWEventType EventType,
 *          union MWEventData *Info,uintptr_t ID);
 *
 * PARAMETERS:
 *    EventType [I] -- The event to send (see UIMainWindow.h)
 *    Info [I] -- Extra info to send with this event.  Can be NULL for none.
 *    ID [I] -- The user data for this event
 *
 * FUNCTION:
 *    This function sends a main window event out of the UI system to the main
 *    code.
 *
 * RETURNS:
 *    true -- Accept this event
 *    false -- Cancel this event
 *
 * NOTES:
 *    The return value is normally ignored as most events have no other action.
 *    However there are some that you want to cancel the next part of the
 *    event (close main window for example).
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Form_MainWindow::SendEvent(e_MWEventType EventType,union MWEventData *Info,
        uintptr_t ID)
{
    struct MWEvent NewEvent;

    NewEvent.EventType=EventType;
    NewEvent.ID=ID;
    NewEvent.UIWindow=(t_UIMainWindow *)this;
    NewEvent.MW=MainWindowClassPtr;
    if(Info!=NULL)
        NewEvent.Info=*Info;

    return MW_Event(&NewEvent);
}
bool Form_MainWindow::SendEvent(e_MWEventType EventType,union MWEventData *Info)
{
    return SendEvent(EventType,Info,objectName().toULongLong());
}
bool Form_MainWindow::SendEvent(e_MWEventType EventType)
{
    return SendEvent(EventType,NULL,objectName().toULongLong());
}

void Form_MainWindow::keyPressEvent(QKeyEvent * event)
{
    uint8_t Mods;
    e_UIKeys UIKey;
    union MWEventData EventData;
    std::string Text;
    char c;

    if(IgnoreNextKeyEvent)
    {
        IgnoreNextKeyEvent=false;
        return;
    }

    Mods=0;
    if(event->modifiers() & Qt::ShiftModifier)
        Mods|=KEYMOD_SHIFT;
    if(event->modifiers() & Qt::ControlModifier)
        Mods|=KEYMOD_CONTROL;
    if(event->modifiers() & Qt::AltModifier)
        Mods|=KEYMOD_ALT;
    if(event->modifiers() & Qt::MetaModifier)
        Mods|=KEYMOD_LOGO;

    UIKey=ConvertQTKey2UIKey((Qt::Key)event->key());

    if(UIKey==e_UIKeysMAX)
    {
        /* Convert CTRL-? (AscII<32) to their letter and let 'Mods' say
           it was a letter with the ctrl key held */
        if(event->text().length()==1 && event->text()[0]<' ')
        {
            /* Remove the CTRL part */
            c=event->text()[0].toLatin1();
            c+='@'; // Move up to ABC range
            Text=c;
        }
        else
        {
            Text=event->text().toStdString();
        }
    }
    else
    {
        Text="";
    }

    EventData.Key.Mods=Mods;
    EventData.Key.Key=UIKey;
    EventData.Key.Text=(const uint8_t *)Text.c_str();
    EventData.Key.TextLen=Text.length();
    SendEvent(e_MWEvent_MainWindowKeyPress,&EventData);
}

bool Form_MainWindow::focusNextPrevChild(bool next)
{
//    if(g_FocusedTextCanvas!=NULL)
//        return false;
    return QMainWindow::focusNextPrevChild(next);
}

void Form_MainWindow::focusInEvent(QFocusEvent * event)
{
DebugMsg("MAIN WINDOW FOCUS");
    g_FocusedMainWindow=this;

    QMainWindow::focusInEvent(event);
}

void Form_MainWindow::focusOutEvent(QFocusEvent * event)
{
    g_FocusedMainWindow=NULL;

    QMainWindow::focusOutEvent(event);
}

void Form_MainWindow::on_actionQuit_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Quit);
}

void Form_MainWindow::DoMenuTriggered(e_UIMWMenuType MenuID)
{
    union MWEventData EventData;

    EventData.Menu.InputID=MenuID;
    SendEvent(e_MWEvent_MenuTriggered,&EventData);
}

void Form_MainWindow::DoContextMenuTriggered(e_UIMW_ContextMenuType MenuID)
{
    union MWEventData EventData;

    EventData.ContextMenu.InputID=MenuID;
    SendEvent(e_MWEvent_ContextMenuTriggered,&EventData);
}

void Form_MainWindow::DoBttnTriggered(e_UIMWBttnType BttnID)
{
    union MWEventData EventData;

    EventData.Bttn.InputID=BttnID;
    SendEvent(e_MWEvent_BttnTriggered,&EventData);
}

void Form_MainWindow::DoToolbarTriggered(e_UIMWToolbarType ToolID)
{
    union MWEventData EventData;

    EventData.Toolbar.InputID=ToolID;
    SendEvent(e_MWEvent_ToolbarTriggered,&EventData);
}

void Form_MainWindow::DoToolbarMenuTriggered(e_UIMWToolbarMenuType ToolMenuID)
{
    union MWEventData EventData;

    EventData.ToolbarMenu.InputID=ToolMenuID;
    SendEvent(e_MWEvent_ToolbarMenuTriggered,&EventData);
}

void Form_MainWindow::DoCheckboxTriggered(e_UIMWCheckboxType CheckboxID,bool Checked)
{
    union MWEventData EventData;

    EventData.Checkbox.Checked=Checked;
    EventData.Checkbox.InputID=CheckboxID;
    SendEvent(e_MWEvent_CheckboxTriggered,&EventData);
}

void Form_MainWindow::DoTextInputChanged(e_UIMWTxtInputType InputID,const char *FinalText)
{
    union MWEventData EventData;

    EventData.Txt.InputID=InputID;
    EventData.Txt.Text=FinalText;
    SendEvent(e_MWEvent_TxtInputEditFinished,&EventData);
}

void Form_MainWindow::on_actionAbout_Whippy_Term_triggered()
{
    DoMenuTriggered(e_UIMWMenu_About);
}

void Form_MainWindow::PanelClicked(e_PanelType Panel)
{
    switch(Panel)
    {
        case e_Panel_Bottom:
            SendEvent(e_MWEvent_BottomPanelClicked);
        break;
        case e_Panel_Left:
            SendEvent(e_MWEvent_LeftPanelClicked);
        break;
        case e_Panel_Right:
            SendEvent(e_MWEvent_RightPanelClicked);
        break;
        case e_PanelMAX:
        break;
    }
}

void Form_MainWindow::EnableBottomSplitter(bool Enable)
{
    QSplitterHandle *hndl;
    int i;

    for(i=0;i<ui->TopBottomSplitter->count();i++)
    {
        hndl=ui->TopBottomSplitter->handle(i);
        hndl->setEnabled(Enable);
    }

    ui->BottomWidgetsPanel->setVisible(Enable);
}

void Form_MainWindow::on_TopBottomSplitter_splitterMoved(int pos, int index)
{
    union MWEventData EInfo;
    QList<int> CurrentSizes;
    bool PanelOpen;

    if(FirstShow)
        return;

    CurrentSizes=ui->TopBottomSplitter->sizes();

    PanelOpen=CurrentSizes[1]>16;

    /* See how big the panel is (and what direction the arrows should be
       pointing) */
    if(PanelOpen)
    {
        /* We are open so we need to save the new size */
        BottomClosed=false;
        ui->BottomResizeFrame->ChangeArrowDir(false);
        BottomPanelLastSize=CurrentSizes[1];
    }
    else
    {
        BottomClosed=true;
        ui->BottomResizeFrame->ChangeArrowDir(true);
    }

    EInfo.PanelInfo.NewSize=BottomPanelLastSize;
    EInfo.PanelInfo.PanelOpen=PanelOpen;
    SendEvent(e_MWEvent_BottomPanelSizeChange,&EInfo);
}

void Form_MainWindow::resizeEvent(QResizeEvent *event)
{
    union MWEventData EventData;
    QList<int> CurrentSizes;
    QList<int> newSizes;
    int SpaceLeft;
    int BottomSize;
    int LeftSize;
    int RightSize;

    if(FirstShow)
        return;

    /* Bottom panel */
    CurrentSizes=ui->TopBottomSplitter->sizes();
    SpaceLeft=CurrentSizes[0]+CurrentSizes[1];

    if(BottomClosed)
    {
        /* Make sure the bottom panel stays closed */
        BottomSize=0;
    }
    else
    {
        /* Keep it the open size */
        BottomSize=BottomPanelLastSize;
    }

    newSizes.append(SpaceLeft-BottomSize);
    newSizes.append(BottomSize);
    ui->TopBottomSplitter->setSizes(newSizes);

    /* Left + Right panels */
    if(LeftClosed)
    {
        /* Make sure the left panel stays closed */
        LeftSize=16;
    }
    else
    {
        /* Keep it the open size */
        LeftSize=LeftPanelLastSize;
    }

    if(RightClosed)
    {
        /* Make sure the right panel stays closed */
        RightSize=16;
    }
    else
    {
        /* Keep it the open size */
        RightSize=RightPanelLastSize;
    }

    CurrentSizes=ui->LeftRightSplitter->sizes();
    SpaceLeft=CurrentSizes[0]+CurrentSizes[1]+CurrentSizes[2];
    newSizes.clear();
    newSizes.append(LeftSize);
    newSizes.append(SpaceLeft-LeftSize-RightSize);
    newSizes.append(RightSize);
    ui->LeftRightSplitter->setSizes(newSizes);

    EventData.NewSize.Width=event->size().width();
    EventData.NewSize.Height=event->size().height();
    SendEvent(e_MWEvent_WindowResize,&EventData);
}

void Form_MainWindow::showEvent(QShowEvent *event)
{
    QList<int> CurrentSizes;

    /* On first show we get all the sizes */
    if(FirstShow)
    {
        FirstShow=false;

        CurrentSizes=ui->TopBottomSplitter->sizes();

        BottomPanelLastSize=CurrentSizes[1];
        BottomClosed=false;

        CurrentSizes=ui->LeftRightSplitter->sizes();
        LeftPanelLastSize=CurrentSizes[0];
        RightPanelLastSize=CurrentSizes[2];
        LeftClosed=false;
        RightClosed=false;

        SendEvent(e_MWEvent_FirstShow);
    }
}

void Form_MainWindow::on_LeftRightSplitter_splitterMoved(int pos, int index)
{
    union MWEventData EInfo;
    QList<int> CurrentSizes;

    if(FirstShow)
        return;

    CurrentSizes=ui->LeftRightSplitter->sizes();

    if(index==1)
    {
        /* Left side */
        /* See how big the panel is (and what direction the arrows should be
           pointing) */
        if(CurrentSizes[0]>16)
        {
            /* We are open so we need to save the new size */
            LeftPanelLastSize=CurrentSizes[0];

            LeftClosed=false;
        }
        else
        {
            LeftClosed=true;
        }
        ui->LeftResizeFrame->ChangeArrowDir(!LeftClosed);

        EInfo.PanelInfo.NewSize=LeftPanelLastSize;
        EInfo.PanelInfo.PanelOpen=!LeftClosed;
        SendEvent(e_MWEvent_LeftPanelSizeChange,&EInfo);
    }
    else
    {
        /* Right side */
        /* See how big the panel is (and what direction the arrows should be
           pointing) */
        if(CurrentSizes[2]>16)
        {
            /* We are open so we need to save the new size */
            RightPanelLastSize=CurrentSizes[2];

            RightClosed=false;
        }
        else
        {
            RightClosed=true;
        }
        ui->RightResizeFrame->ChangeArrowDir(RightClosed);
        EInfo.PanelInfo.NewSize=RightPanelLastSize;
        EInfo.PanelInfo.PanelOpen=!RightClosed;
        SendEvent(e_MWEvent_RightPanelSizeChange,&EInfo);
    }
}

void Form_MainWindow::EnableLeftSplitter(bool Enable)
{
    QSplitterHandle *hndl;

    hndl=ui->LeftRightSplitter->handle(0);
    hndl->setEnabled(Enable);
    hndl=ui->LeftRightSplitter->handle(1);
    hndl->setEnabled(Enable);

    ui->LeftWidgetsPanel->setVisible(Enable);
}

void Form_MainWindow::EnableRightSplitter(bool Enable)
{
    QSplitterHandle *hndl;

//    hndl=ui->LeftRightSplitter->handle(1);
//    hndl->setEnabled(Enable);
    hndl=ui->LeftRightSplitter->handle(2);
    hndl->setEnabled(Enable);

    ui->RightWidgetsPanel->setVisible(Enable);
}

void Form_MainWindow::closeEvent(QCloseEvent *event)
{
    if(!SendEvent(e_MWEvent_MainWindowClose))
        event->ignore();
    else
        event->accept();
}

void Form_MainWindow::changeEvent(QEvent *event)
{
    union MWEventData EventData;
    QWindowStateChangeEvent *StateEvent;

    if(FirstShow)
        return;

    if(event->type()==QEvent::WindowStateChange)
    {
        StateEvent=static_cast<QWindowStateChangeEvent *>(event);

        if(this->windowState()==Qt::WindowNoState)
        {
            /* Normal state */
            if(StateEvent->oldState()&Qt::WindowMaximized)
            {
                /* Restored to normal */
                EventData.MaximizedInfo.Max=false;
                SendEvent(e_MWEvent_WindowSet2Maximized,&EventData);
            }
        }
        else if(this->windowState()&Qt::WindowMaximized)
        {
            if(!(StateEvent->oldState()&Qt::WindowMaximized))
            {
                /* Maximized */
                EventData.MaximizedInfo.Max=true;
                SendEvent(e_MWEvent_WindowSet2Maximized,&EventData);
            }
        }
    }
}

void Form_MainWindow::moveEvent(QMoveEvent *event)
{
    union MWEventData EventData;

    /* Don't know why but it seems we have to read out of the window instead
       of using the event ones */
    EventData.Moved.x=this->x();
    EventData.Moved.y=this->y();
    SendEvent(e_MWEvent_WindowMoved,&EventData);
}

void Form_MainWindow::on_actionSettings_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Settings);
}

void Form_MainWindow::on_actionImport_Settings_triggered()
{
    DoMenuTriggered(e_UIMWMenu_ImportSettings);
}

void Form_MainWindow::on_actionExport_Settings_triggered()
{
    DoMenuTriggered(e_UIMWMenu_ExportSettings);
}

void Form_MainWindow::on_Main_tabWidget_tabCloseRequested(int index)
{
    uintptr_t ID;
    QWidget *Tab;

    Tab=this->ui->Main_tabWidget->widget(index);
    if(Tab!=0)
    {
        ID=Tab->objectName().toULongLong();

        SendEvent(e_MWEvent_TabClose,NULL,ID);
    }
}

void Form_MainWindow::GlobalCloseTabBttnClicked(void)
{
    on_Main_tabWidget_tabCloseRequested(ui->Main_tabWidget->currentIndex());
}

void Form_MainWindow::on_Main_tabWidget_TabMoved(int from,int to)
{
//    uintptr_t FromID;
//    uintptr_t ToID;
//    QWidget *Tab;

//    Tab=this->ui->Main_tabWidget->widget(from);
//    if(Tab!=0)
//    {
//        FromID=Tab->objectName().toULongLong();
//        Tab=this->ui->Main_tabWidget->widget(to);
//        if(Tab!=0)
//        {
//            ToID=Tab->objectName().toULongLong();

//            InformOf_TabMoved(FromID,ToID);
//        }
//    }
}

void Form_MainWindow::on_actionClose_Tab_triggered()
{
    DoMenuTriggered(e_UIMWMenu_CloseTab);
}

void Form_MainWindow::on_actionClose_All_triggered()
{
    DoMenuTriggered(e_UIMWMenu_CloseAll);
}

void Form_MainWindow::on_actionFont_Request_Test_triggered()
{
    std::string FontName;
    int FontSize;
    long FontStyle;

    FontName="";
    FontSize=12;
    FontStyle=UIFONT_STYLE_BOLD|UIFONT_STYLE_ITALIC;

    if(UI_FontReq("Test font select",FontName,FontSize,FontStyle,0))
    {
        qDebug("FONT:%s,%d,%ld",FontName.c_str(),FontSize,FontStyle);
    }
}

void Form_MainWindow::on_actionNew_Tab_triggered()
{
    DoMenuTriggered(e_UIMWMenu_NewTab);
}

void Form_MainWindow::on_actionConnect_Menu_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Connect);
}

void Form_MainWindow::on_actionDisconnect_Menu_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Disconnect);
}

void Form_MainWindow::on_Main_tabWidget_currentChanged(int index)
{
    union MWEventData EventData;
    uintptr_t ID;
    QWidget *Tab;

    Tab=this->ui->Main_tabWidget->widget(index);

    if(Tab!=0)
    {
        ID=Tab->objectName().toULongLong();
        EventData.PanelTab.TabID=e_UIMWTabCtrl_MainTabs;
        EventData.PanelTab.NewIndex=index;
        SendEvent(e_MWEvent_PanelTabChange,&EventData,ID);
    }
}

void Form_MainWindow::on_actionInsert_Horizontal_Rule_triggered()
{
    DoMenuTriggered(e_UIMWMenu_InsertHorizontalRule);
}

void Form_MainWindow::on_actionClear_Screen_triggered()
{
    DoMenuTriggered(e_UIMWMenu_ClearScreen);
}

void Form_MainWindow::on_actionMenu_Change_Name_triggered()
{
    DoMenuTriggered(e_UIMWMenu_ChangeConnectionName);
}

void Form_MainWindow::on_pushButton_SW_Start_clicked()
{
    DoBttnTriggered(e_UIMWBttn_StopWatch_Start);
}

void Form_MainWindow::on_pushButton_SW_Reset_clicked()
{
    DoBttnTriggered(e_UIMWBttn_StopWatch_Reset);
}

void Form_MainWindow::on_pushButton_SW_Lap_clicked()
{
    DoBttnTriggered(e_UIMWBttn_StopWatch_Lap);
}

void Form_MainWindow::on_pushButton_SW_Clear_clicked()
{
    DoBttnTriggered(e_UIMWBttn_StopWatch_Clear);
}

void Form_MainWindow::on_checkBox_SW_StartOnTx_clicked(bool checked)
{
    DoCheckboxTriggered(e_UIMWCheckbox_StopWatch_StartOnTx,checked);
}

void Form_MainWindow::on_checkBox_SW_AutoLap_clicked(bool checked)
{
    DoCheckboxTriggered(e_UIMWCheckbox_StopWatch_AutoLap,checked);
}

void Form_MainWindow::on_actionStats_triggered()
{
    Form_DebugStats *DebugStats;
    DebugStats=new Form_DebugStats(this);
    DebugStats->show();
}

void Form_MainWindow::on_actionCopy_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Copy);
}

void Form_MainWindow::on_actionPaste_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Paste);
}

void Form_MainWindow::on_pushButton_CO_Apply_clicked()
{
    DoBttnTriggered(e_UIMWBttn_ConnectionOptionApply);
}

void Form_MainWindow::on_actionBookmark_this_connection_triggered()
{
    DoMenuTriggered(e_UIMWMenu_AddBookmark);
}

void Form_MainWindow::actionBookmarkMenuItem_triggered()
{
    uintptr_t ID;
    QObject *MenuAction;

    MenuAction=sender();
    ID=MenuAction->objectName().toULongLong();

    SendEvent(e_MWEvent_BookmarkMenuSelected,NULL,ID);
}

void Form_MainWindow::actionFTPUploadMenuItem_triggered()
{
    uintptr_t ID;
    QObject *MenuAction;

    MenuAction=sender();
    ID=MenuAction->objectName().toULongLong();

    SendEvent(e_MWEvent_UploadMenuTriggered,NULL,ID);
}

void Form_MainWindow::actionFTPDownloadMenuItem_triggered()
{
    uintptr_t ID;
    QObject *MenuAction;

    MenuAction=sender();
    ID=MenuAction->objectName().toULongLong();

    SendEvent(e_MWEvent_DownloadMenuTriggered,NULL,ID);
}

void Form_MainWindow::on_actionManage_Bookmarks_triggered()
{
    DoMenuTriggered(e_UIMWMenu_ManageBookmarks);
}

void Form_MainWindow::StopWatchTimer_triggered()
{
    SendEvent(e_MWEvent_StopWatch_Timer);
}

void Form_MainWindow::DelayedThemeChangeTimer_triggered()
{
    RethinkColors();
}

void Form_MainWindow::on_actionStopWatch_Start_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StopWatch_Start);
}

void Form_MainWindow::on_actionStopWatch_Stop_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StopWatch_Stop);
}

void Form_MainWindow::on_actionStopWatch_Reset_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StopWatch_Reset);
}

void Form_MainWindow::on_actionStopWatch_Lap_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StopWatch_Lap);
}

void Form_MainWindow::on_actionStopWatch_Clear_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StopWatch_Clear);
}

void Form_MainWindow::on_actionStopWatch_Start_On_Tx_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StopWatch_StartOnTx);
}

void Form_MainWindow::on_actionStopWatch_Auto_Lap_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StopWatch_AutoLap);
}

void Form_MainWindow::on_lineEdit_Cap_FileName_editingFinished()
{
    DoTextInputChanged(e_UIMWTxtInput_Capture_Filename,
            qPrintable(ui->lineEdit_Cap_FileName->text()));
}

void Form_MainWindow::on_pushButton_Cap_GetFilename_clicked()
{
    DoBttnTriggered(e_UIMWBttn_Capture_SelectFilename);
}

void Form_MainWindow::on_checkBox_Cap_Timestamp_clicked(bool checked)
{
    DoCheckboxTriggered(e_UIMWCheckbox_Capture_Timestamp,checked);
}

void Form_MainWindow::on_checkBox_Cap_Append_clicked(bool checked)
{
    DoCheckboxTriggered(e_UIMWCheckbox_Capture_Append,checked);
}

void Form_MainWindow::on_checkBox_Cap_StripCtrlChars_clicked(bool checked)
{
    DoCheckboxTriggered(e_UIMWCheckbox_Capture_StripCtrlChars,checked);
}

void Form_MainWindow::on_checkBox_Cap_StripEscSeq_clicked(bool checked)
{
    DoCheckboxTriggered(e_UIMWCheckbox_Capture_StripEscSeq,checked);
}

void Form_MainWindow::on_pushButton_Cap_Start_clicked()
{
    DoBttnTriggered(e_UIMWBttn_Capture_Start);
}

void Form_MainWindow::on_actionCapture_To_File_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Capture_CaptureToFile);
}

void Form_MainWindow::on_actionTimestamp_new_lines_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Capture_TimestampToggle);
}

void Form_MainWindow::on_actionAppend_To_Existing_File_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Capture_AppendToggle);
}

void Form_MainWindow::on_actionStrip_Control_Char_s_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Capture_StripCtrlCharsToggle);
}

void Form_MainWindow::on_actionStrip_Esc_Sequences_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Capture_StripEscSeqToggle);
}

void Form_MainWindow::on_actionCapture_Stop_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Capture_Stop);
}

void Form_MainWindow::on_checkBox_Cap_HexDump_clicked(bool checked)
{
    DoCheckboxTriggered(e_UIMWCheckbox_Capture_HexDump,checked);
}

void Form_MainWindow::on_actionHex_Dump_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Capture_HexDumpToggle);
}

void Form_MainWindow::on_pushButton_UploadSelectFilename_clicked()
{
    DoBttnTriggered(e_UIMWBttn_Upload_SelectFilename);
}

void Form_MainWindow::on_pushButton_UploadStart_clicked()
{
    DoBttnTriggered(e_UIMWBttn_Upload_Start);
}

void Form_MainWindow::on_comboBox_UploadProtocol_activated(int index)
{
    uintptr_t ID;   // The ID for this item
    union MWEventData EventData;

    ID=ui->comboBox_UploadProtocol->itemData(index).toULongLong();

    EventData.ComboBox.InputID=e_UIMWComboBox_Upload_Protocol;
    EventData.ComboBox.Index=index;
    SendEvent(e_MWEvent_ComboBoxChanged,&EventData,ID);
}

void Form_MainWindow::on_lineEdit_UploadFilename_editingFinished()
{
    DoTextInputChanged(e_UIMWTxtInput_Upload_Filename,
            qPrintable(ui->lineEdit_UploadFilename->text()));
}

void Form_MainWindow::on_pushButton_UploadAbort_clicked()
{
    DoBttnTriggered(e_UIMWBttn_Upload_Abort);
}

void Form_MainWindow::on_pushButton_DownloadStart_clicked()
{
    DoBttnTriggered(e_UIMWBttn_Download_Start);
}

void Form_MainWindow::on_pushButton_DownloadAbort_clicked()
{
    DoBttnTriggered(e_UIMWBttn_Download_Abort);
}

void Form_MainWindow::on_comboBox_DownloadProtocol_activated(int index)
{
    uintptr_t ID;   // The ID for this item
    union MWEventData EventData;

    ID=ui->comboBox_DownloadProtocol->itemData(index).toULongLong();

    EventData.ComboBox.InputID=e_UIMWComboBox_Download_Protocol;
    EventData.ComboBox.Index=index;
    SendEvent(e_MWEvent_ComboBoxChanged,&EventData,ID);
}

void Form_MainWindow::on_BottomPanel_tabWidget_currentChanged(int index)
{
    union MWEventData EventData;
    uintptr_t ID;
    QWidget *Tab;

    Tab=this->ui->BottomPanel_tabWidget->widget(index);

    if(Tab!=0)
    {
        ID=Tab->objectName().toULongLong();

        EventData.PanelTab.TabID=e_UIMWTabCtrl_BottomPanel;
        EventData.PanelTab.NewIndex=index;
        SendEvent(e_MWEvent_PanelTabChange,&EventData,ID);
    }
}

void Form_MainWindow::on_LeftPanel_tabWidget_currentChanged(int index)
{
    union MWEventData EventData;
    uintptr_t ID;
    QWidget *Tab;

    Tab=this->ui->LeftPanel_tabWidget->widget(index);

    if(Tab!=0)
    {
        ID=Tab->objectName().toULongLong();

        EventData.PanelTab.TabID=e_UIMWTabCtrl_LeftPanel;
        EventData.PanelTab.NewIndex=index;
        SendEvent(e_MWEvent_PanelTabChange,&EventData,ID);
    }
}

void Form_MainWindow::on_RightPanel_tabWidget_currentChanged(int index)
{
    union MWEventData EventData;
    uintptr_t ID;
    QWidget *Tab;

    Tab=this->ui->RightPanel_tabWidget->widget(index);

    if(Tab!=0)
    {
        ID=Tab->objectName().toULongLong();

        EventData.PanelTab.TabID=e_UIMWTabCtrl_RightPanel;
        EventData.PanelTab.NewIndex=index;
        SendEvent(e_MWEvent_PanelTabChange,&EventData,ID);
    }
}

void Form_MainWindow::on_pushButton_HexClear_clicked()
{
    DoBttnTriggered(e_UIMWBttn_HexDisplay_Clear);
}

void Form_MainWindow::on_pushButton_HexCopy_clicked()
{
    DoBttnTriggered(e_UIMWBttn_HexDisplay_Copy);
}

void Form_MainWindow::on_pushButton_HexCopyAs_clicked()
{
    DoBttnTriggered(e_UIMWBttn_HexDisplay_CopyAs);
}

void Form_MainWindow::on_checkBox_HexPaused_clicked(bool checked)
{
    DoCheckboxTriggered(e_UIMWCheckbox_HexDisplay_Paused,checked);
}

void Form_MainWindow::on_pushButton_Buffer_Edit_clicked()
{
    DoBttnTriggered(e_UIMWBttn_SendBuffers_Edit);
}

void Form_MainWindow::on_pushButton_Buffer_Send_clicked()
{
    DoBttnTriggered(e_UIMWBttn_SendBuffers_Send);
}

void Form_MainWindow::on_actionConnection_Options_triggered()
{
    DoMenuTriggered(e_UIMWMenu_ConnectionOptions);
}

void Form_MainWindow::on_actionConnection_Settings_triggered()
{
    DoMenuTriggered(e_UIMWMenu_ConnectionSettings);
}

void Form_MainWindow::on_actionCom_Test_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Tools_ComTest);
}

void Form_MainWindow::on_actionTransmit_delay_triggered()
{
    DoMenuTriggered(e_UIMWMenu_TransmitDelay);
}

void Form_MainWindow::on_actionManage_Plugins_triggered()
{
    DoMenuTriggered(e_UIMWMenu_ManagePlugin);
}

void Form_MainWindow::on_actionInstall_Plugin_triggered()
{
    DoMenuTriggered(e_UIMWMenu_InstallPlugin);
}

void Form_MainWindow::on_pushButton_Bridge_Bridge_clicked()
{
    DoBttnTriggered(e_UIMWBttn_Bridge_Bridge);
}

void Form_MainWindow::on_pushButton_Bridge_Release_clicked()
{
    DoBttnTriggered(e_UIMWBttn_Bridge_Release);
}

void Form_MainWindow::on_comboBox_Bridge_Con2_activated(int index)
{
    uintptr_t ID;   // The ID for this item
    union MWEventData EventData;

    ID=ui->comboBox_Bridge_Con2->itemData(index).toULongLong();

    EventData.ComboBox.InputID=e_UIMWComboBox_Bridge_Connection2;
    EventData.ComboBox.Index=index;
    SendEvent(e_MWEvent_ComboBoxChanged,&EventData,ID);
}

void Form_MainWindow::on_checkBox_Bridge_Lockout1_clicked(bool checked)
{
    DoCheckboxTriggered(e_UIMWCheckbox_Bridge_Lock1,checked);
}

void Form_MainWindow::on_checkBox_Bridge_Lockout2_clicked(bool checked)
{
    DoCheckboxTriggered(e_UIMWCheckbox_Bridge_Lock2,checked);
}

void Form_MainWindow::on_actionBridge_Connections_triggered()
{
    DoMenuTriggered(e_UIMWMenu_BridgeConnections);
}

void Form_MainWindow::on_actionRelease_Bridged_Connections_triggered()
{
    DoMenuTriggered(e_UIMWMenu_ReleaseBridgedConnections);
}

void Form_MainWindow::on_actionGet_Plugins_triggered()
{
    DoMenuTriggered(e_UIMWMenu_GetPlugins);
}

void Form_MainWindow::on_actionShow_NonPrintables_triggered()
{
    DoMenuTriggered(e_UIMWMenu_ShowNonPrintable);
}

void Form_MainWindow::on_actionShow_End_Of_Lines_triggered()
{
    DoMenuTriggered(e_UIMWMenu_ShowEndOfLines);
}

void Form_MainWindow::on_actionClear_Scroll_Buffer_triggered()
{
    DoMenuTriggered(e_UIMWMenu_ClearScrollBackBuffer);
}


void Form_MainWindow::on_actionReset_triggered()
{
    DoMenuTriggered(e_UIMWMenu_ResetTerm);
}


void Form_MainWindow::on_actionGoto_Column_triggered()
{
    DoMenuTriggered(e_UIMWMenu_GotoColumn);
}


void Form_MainWindow::on_actionGoto_Line_triggered()
{
    DoMenuTriggered(e_UIMWMenu_GotoRow);
}


void Form_MainWindow::on_actionSelect_All_triggered()
{
    DoMenuTriggered(e_UIMWMenu_SelectAll);
}

void Form_MainWindow::on_actionZoom_In_triggered()
{
    DoMenuTriggered(e_UIMWMenu_ZoomIn);
}

void Form_MainWindow::on_actionZoom_Out_triggered()
{
    DoMenuTriggered(e_UIMWMenu_ZoomOut);
}

void Form_MainWindow::on_actionReset_Zoom_triggered()
{
    DoMenuTriggered(e_UIMWMenu_ResetZoom);
}

void Form_MainWindow::on_actionSend_NULL_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Send_NULL);
}

void Form_MainWindow::on_actionSend_Backspace_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Send_Backspace);
}

void Form_MainWindow::on_actionSend_Tab_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Send_Tab);
}

void Form_MainWindow::on_actionSend_Line_Feed_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Send_Line_Feed);
}

void Form_MainWindow::on_actionSend_Form_Feed_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Send_Form_Feed);
}

void Form_MainWindow::on_actionSend_Carriage_Return_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Send_Carriage_Return);
}

void Form_MainWindow::on_actionSend_Other_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Send_Other);
}

void Form_MainWindow::on_actionSend_Escape_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Send_Escape);
}

void Form_MainWindow::on_actionSend_Delete_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Send_Delete);
}

void Form_MainWindow::on_actionTerminal_Size_triggered()
{
    DoMenuTriggered(e_UIMWMenu_SettingsQuickJump_TermSize);
}

void Form_MainWindow::on_actionTerminal_Emulation_triggered()
{
    DoMenuTriggered(e_UIMWMenu_SettingsQuickJump_TermEmu);
}

void Form_MainWindow::on_actionFont_triggered()
{
    DoMenuTriggered(e_UIMWMenu_SettingsQuickJump_Font);
}

void Form_MainWindow::on_actionColors_triggered()
{
    DoMenuTriggered(e_UIMWMenu_SettingsQuickJump_Colors);
}

void Form_MainWindow::on_actionShow_printf_log_triggered()
{
    DB_ShowDebugPrintLogForm();
}


void Form_MainWindow::on_actionSend_Buffer_1_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Buffers_SendBuffer1);
}

void Form_MainWindow::on_actionSend_Buffer_2_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Buffers_SendBuffer2);
}

void Form_MainWindow::on_actionSend_Buffer_3_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Buffers_SendBuffer3);
}

void Form_MainWindow::on_actionSend_Buffer_4_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Buffers_SendBuffer4);
}

void Form_MainWindow::on_actionSend_Buffer_5_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Buffers_SendBuffer5);
}

void Form_MainWindow::on_actionSend_Buffer_6_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Buffers_SendBuffer6);
}

void Form_MainWindow::on_actionSend_Buffer_7_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Buffers_SendBuffer7);
}

void Form_MainWindow::on_actionSend_Buffer_8_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Buffers_SendBuffer8);
}

void Form_MainWindow::on_actionSend_Buffer_9_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Buffers_SendBuffer9);
}

void Form_MainWindow::on_actionSend_Buffer_10_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Buffers_SendBuffer10);
}

void Form_MainWindow::on_actionSend_Buffer_11_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Buffers_SendBuffer11);
}

void Form_MainWindow::on_actionSend_Buffer_12_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Buffers_SendBuffer12);
}

void Form_MainWindow::on_actionSave_Buffer_Set_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Buffers_SaveBufferSet);
}

void Form_MainWindow::on_actionLoad_Buffer_Set_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Buffers_LoadBufferSet);
}


void Form_MainWindow::on_treeWidget_Buffer_BufferList_itemSelectionChanged()
{
    union MWEventData EventData;
    QTreeWidgetItem *TreeItem;
    int Row;

    if(ui->treeWidget_Buffer_BufferList->selectedItems().isEmpty())
    {
        Row=-1;
    }
    else
    {
        TreeItem=ui->treeWidget_Buffer_BufferList->selectedItems().first();
        Row=ui->treeWidget_Buffer_BufferList->indexOfTopLevelItem(TreeItem);
    }

    EventData.ColumnView.InputID=e_UIMWColumnView_Buffers_List;
    EventData.ColumnView.NewRow=Row;

    SendEvent(e_MWEvent_ColumnViewChange,&EventData,0);
//    SendEvent(e_MWEvent_ListViewChange,&EventData,
//            (uintptr_t)(ui->listWidget_Buffer_BufferList->item(currentRow)->
//            data(Qt::UserRole).toULongLong()));
}


void Form_MainWindow::on_actionClear_All_Buffers_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Buffers_ClearAllBuffers);
}


void Form_MainWindow::on_actionSend_Buffer_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Buffers_SendBufferSendGeneric);
}


void Form_MainWindow::on_actionEdit_Send_Buffer_triggered()
{
    DoMenuTriggered(e_UIMWMenu_Buffers_EditSenderBuffer);
}

void Form_MainWindow::actionApplyTerminalEmulationMenuItem_triggered()
{
    uintptr_t ID;
    QObject *MenuAction;

    MenuAction=sender();
    ID=MenuAction->objectName().toULongLong();

    SendEvent(e_MWEvent_ApplyTerminalEmulationMenuTriggered,NULL,ID);
}

void Form_MainWindow::ContextMenu(const QPoint &pos)
{
    HandleContextMenuClick(sender(),pos);
}

void Form_MainWindow::on_Menu_treeWidget_Buffer_BufferList_x2_Edit_triggered()
{
    DoContextMenuTriggered(e_UIMW_ContextMenu_SendBuffers_Edit);
}


void Form_MainWindow::on_Menu_treeWidget_Buffer_BufferList_x1_Send_triggered()
{
    DoContextMenuTriggered(e_UIMW_ContextMenu_SendBuffers_Send);
}


void Form_MainWindow::on_Menu_treeWidget_Buffer_BufferList_x5_ClearBuffer_triggered()
{
    DoContextMenuTriggered(e_UIMW_ContextMenu_SendBuffers_Clear);
}


void Form_MainWindow::on_Menu_treeWidget_Buffer_BufferList_x3_Rename_triggered()
{
    DoContextMenuTriggered(e_UIMW_ContextMenu_SendBuffers_Rename);
}


void Form_MainWindow::on_Menu_treeWidget_Buffer_BufferList_x7_LoadBuffer_triggered()
{
    DoContextMenuTriggered(e_UIMW_ContextMenu_SendBuffers_LoadBuffer);
}


void Form_MainWindow::on_Menu_treeWidget_Buffer_BufferList_x8_SaveBuffer_triggered()
{
    DoContextMenuTriggered(e_UIMW_ContextMenu_SendBuffers_SaveBuffer);
}

void Form_MainWindow::on_actionNew_Tab_From_URI_triggered()
{
    DoMenuTriggered(e_UIMWMenu_NewTabFromURI);
}

void Form_MainWindow::on_actionDefault_Settings_triggered()
{
    DoMenuTriggered(e_UIMWMenu_DefaultSettings);
}


void Form_MainWindow::on_actionAuto_Reconnect_triggered()
{
    DoMenuTriggered(e_UIMWMenu_ToggleAutoReconnect);
}

void Form_MainWindow::on_New_Tab_toolButton_clicked()
{
    DoToolbarTriggered(e_UIMWToolbar_NewTab);
}


void Form_MainWindow::on_ConnectToggle_toolButton_clicked()
{
    DoToolbarTriggered(e_UIMWToolbar_ConnectToggle);
}


void Form_MainWindow::on_Copy_toolButton_clicked()
{
    DoToolbarTriggered(e_UIMWToolbar_Copy);
}


void Form_MainWindow::on_Paste_toolButton_clicked()
{
    DoToolbarTriggered(e_UIMWToolbar_Paste);
}


void Form_MainWindow::on_Clear_Screen_toolButton_clicked()
{
    DoToolbarTriggered(e_UIMWToolbar_ClearScreen);
}

void Form_MainWindow::on_URIHelp_toolButton_clicked()
{
    DoToolbarTriggered(e_UIMWToolbar_URIHelp);
}


void Form_MainWindow::on_URIGo_toolButton_clicked()
{
    DoToolbarTriggered(e_UIMWToolbar_URIGo);
}


void Form_MainWindow::on_URI_lineEdit_textEdited(const QString &arg1)
{
    string InputText;
    InputText=ui->URI_lineEdit->text().toStdString();
    DoTextInputChanged(e_UIMWTxtInput_URI,InputText.c_str());
}

void Form_MainWindow::on_URI_lineEdit_returnPressed()
{
    DoToolbarTriggered(e_UIMWToolbar_URIGo);
    IgnoreNextKeyEvent=true;
}

void Form_MainWindow::on_StyleBold_toolButton_clicked()
{
    DoToolbarTriggered(e_UIMWToolbar_StyleBold);
}

void Form_MainWindow::on_StyleItalics_toolButton_clicked()
{
    DoToolbarTriggered(e_UIMWToolbar_StyleItalics);
}

void Form_MainWindow::on_StyleUnderline_toolButton_clicked()
{
    DoToolbarTriggered(e_UIMWToolbar_StyleUnderline);
}

void Form_MainWindow::on_StyleStrikeThrough_toolButton_clicked()
{
    DoToolbarTriggered(e_UIMWToolbar_StyleStrikeThrough);
}

void Form_MainWindow::on_StyleColor_toolButton_clicked()
{
    DoToolbarTriggered(e_UIMWToolbar_StyleBGColor);
}

void Form_MainWindow::on_actionToolbarMenu_ColorSel_Black_triggered()
{
    DoToolbarMenuTriggered(e_UIMWToolbarPopUpMenu_StyleBG_Black);
}


void Form_MainWindow::on_actionToolbarMenu_ColorSel_Blue_triggered()
{
    DoToolbarMenuTriggered(e_UIMWToolbarPopUpMenu_StyleBG_Blue);
}


void Form_MainWindow::on_actionToolbarMenu_ColorSel_Green_triggered()
{
    DoToolbarMenuTriggered(e_UIMWToolbarPopUpMenu_StyleBG_Green);
}


void Form_MainWindow::on_actionToolbarMenu_ColorSel_Cyan_triggered()
{
    DoToolbarMenuTriggered(e_UIMWToolbarPopUpMenu_StyleBG_Cyan);
}


void Form_MainWindow::on_actionToolbarMenu_ColorSel_Red_triggered()
{
    DoToolbarMenuTriggered(e_UIMWToolbarPopUpMenu_StyleBG_Red);
}


void Form_MainWindow::on_actionToolbarMenu_ColorSel_Magenta_triggered()
{
    DoToolbarMenuTriggered(e_UIMWToolbarPopUpMenu_StyleBG_Magenta);
}


void Form_MainWindow::on_actionToolbarMenu_ColorSel_Brown_triggered()
{
    DoToolbarMenuTriggered(e_UIMWToolbarPopUpMenu_StyleBG_Brown);
}


void Form_MainWindow::on_actionToolbarMenu_ColorSel_White_triggered()
{
    DoToolbarMenuTriggered(e_UIMWToolbarPopUpMenu_StyleBG_White);
}


void Form_MainWindow::on_actionToolbarMenu_ColorSel_Gray_triggered()
{
    DoToolbarMenuTriggered(e_UIMWToolbarPopUpMenu_StyleBG_Gray);
}


void Form_MainWindow::on_actionToolbarMenu_ColorSel_LightGreen_triggered()
{
    DoToolbarMenuTriggered(e_UIMWToolbarPopUpMenu_StyleBG_LightGreen);
}


void Form_MainWindow::on_actionToolbarMenu_ColorSel_LightCyan_triggered()
{
    DoToolbarMenuTriggered(e_UIMWToolbarPopUpMenu_StyleBG_LightCyan);
}


void Form_MainWindow::on_actionToolbarMenu_ColorSel_LightRed_triggered()
{
    DoToolbarMenuTriggered(e_UIMWToolbarPopUpMenu_StyleBG_LightRed);
}


void Form_MainWindow::on_actionToolbarMenu_ColorSel_LightMagenta_triggered()
{
    DoToolbarMenuTriggered(e_UIMWToolbarPopUpMenu_StyleBG_LightMagenta);
}


void Form_MainWindow::on_actionToolbarMenu_ColorSel_Yellow_triggered()
{
    DoToolbarMenuTriggered(e_UIMWToolbarPopUpMenu_StyleBG_Yellow);
}


void Form_MainWindow::on_actionToolbarMenu_ColorSel_LightBlue_triggered()
{
    DoToolbarMenuTriggered(e_UIMWToolbarPopUpMenu_StyleBG_LightBlue);
}


void Form_MainWindow::on_actionToolbarMenu_ColorSel_BrightWhite_triggered()
{
    DoToolbarMenuTriggered(e_UIMWToolbarPopUpMenu_StyleBG_BrightWhite);
}

void Form_MainWindow::on_actionStyleBGColor_Black_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleBGColor_Black);
}

void Form_MainWindow::on_actionStyleBGColor_Blue_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleBGColor_Blue);
}

void Form_MainWindow::on_actionStyleBGColor_Green_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleBGColor_Green);
}

void Form_MainWindow::on_actionStyleBGColor_Cyan_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleBGColor_Cyan);
}

void Form_MainWindow::on_actionStyleBGColor_Red_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleBGColor_Red);
}

void Form_MainWindow::on_actionStyleBGColor_Magenta_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleBGColor_Magenta);
}

void Form_MainWindow::on_actionStyleBGColor_Brown_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleBGColor_Brown);
}

void Form_MainWindow::on_actionStyleBGColor_White_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleBGColor_White);
}

void Form_MainWindow::on_actionStyleBGColor_Gray_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleBGColor_Gray);
}

void Form_MainWindow::on_actionStyleBGColor_LightBlue_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleBGColor_LightBlue);
}

void Form_MainWindow::on_actionStyleBGColor_LightGreen_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleBGColor_LightGreen);
}

void Form_MainWindow::on_actionStyleBGColor_LightCyan_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleBGColor_LightCyan);
}

void Form_MainWindow::on_actionStyleBGColor_LightRed_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleBGColor_LightRed);
}

void Form_MainWindow::on_actionStyleBGColor_LightMagenta_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleBGColor_LightMagenta);
}

void Form_MainWindow::on_actionStyleBGColor_Yellow_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleBGColor_Yellow);
}

void Form_MainWindow::on_actionStyleBGColor_BrightWhite_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleBGColor_BrightWhite);
}

void Form_MainWindow::on_actionBold_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleBold);
}

void Form_MainWindow::on_actionItalics_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleItalics);
}

void Form_MainWindow::on_actionUnderline_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleUnderline);
}

void Form_MainWindow::on_actionStrike_Through_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StyleStrikeThrough);
}


void Form_MainWindow::on_actionCTRL_C_Handling_triggered()
{
    DoMenuTriggered(e_UIMWMenu_SettingsQuickJump_CtrlCHandling);
}


void Form_MainWindow::on_actionCRC_Algorithm_Finder_triggered()
{
    DoMenuTriggered(e_UIMWMenu_CRCFinder);
}


void Form_MainWindow::on_actionUse_Global_Settings_triggered()
{
    DoMenuTriggered(e_UIMWMenu_ConnectionUseGlobalSettings);
}


void Form_MainWindow::on_actionCalculate_CRC_triggered()
{
    DoMenuTriggered(e_UIMWMenu_CalcCRC);
}


void Form_MainWindow::on_actionNew_Connection_triggered()
{
    DoMenuTriggered(e_UIMWMenu_NewConnection);
}


void Form_MainWindow::on_actionCommand_Line_Options_triggered()
{
    DoMenuTriggered(e_UIMWMenu_HelpCommandLineOptions);
}


void Form_MainWindow::on_actionCopy_Selection_To_Send_Buffer_triggered()
{
    DoMenuTriggered(e_UIMWMenu_CopySelectionToSendBuffer);
}


void Form_MainWindow::on_actionLeft_Side_Panel_triggered()
{
    DoMenuTriggered(e_UIMWMenu_LeftPanel);
}


void Form_MainWindow::on_actionBottom_Panel_triggered()
{
    DoMenuTriggered(e_UIMWMenu_BottomPanel);
}


void Form_MainWindow::on_actionRight_Side_Panel_triggered()
{
    DoMenuTriggered(e_UIMWMenu_RightPanel);
}

void Form_MainWindow::on_checkBox_OutGoing_HexPaused_clicked(bool checked)
{
    DoCheckboxTriggered(e_UIMWCheckbox_OutGoingHexDisplay_Paused,checked);
}


void Form_MainWindow::on_pushButton_OutGoing_HexClear_clicked()
{
    DoBttnTriggered(e_UIMWBttn_OutGoingHexDisplay_Clear);
}


void Form_MainWindow::on_pushButton_OutGoing_HexCopy_clicked()
{
    DoBttnTriggered(e_UIMWBttn_OutGoingHexDisplay_Copy);
}


void Form_MainWindow::on_pushButton_OutGoing_HexCopyAs_clicked()
{
    DoBttnTriggered(e_UIMWBttn_OutGoingHexDisplay_CopyAs);
}


void Form_MainWindow::on_OutGoingHex_Save_pushButton_clicked()
{
    DoBttnTriggered(e_UIMWBttn_OutGoingHexSave);
}


void Form_MainWindow::on_IncomingHex_Save_pushButton_clicked()
{
    DoBttnTriggered(e_UIMWBttn_InComingHexSave);
}

bool Form_MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::ThemeChange)
    {
        DelayedThemeChangeTimer->start(10);
    }

    return QMainWindow::event(event);
}


void Form_MainWindow::on_ClearScreenOnSend_checkBox_clicked(bool checked)
{
    DoCheckboxTriggered(e_UIMWCheckbox_SendBufferClearScreenOnSend,checked);
}


void Form_MainWindow::on_actionClear_Screen_On_Send_Buffer_triggered()
{
    DoMenuTriggered(e_UIMWMenu_ClearScreenOnSend);
}
 

void Form_MainWindow::on_actionRun_Script_triggered()
{
    DoMenuTriggered(e_UIMWMenu_RunScript);
}


void Form_MainWindow::on_actionStop_Script_triggered()
{
    DoMenuTriggered(e_UIMWMenu_StopScript);
}

