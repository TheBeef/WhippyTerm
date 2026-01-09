#include <QApplication>
//#include "Form_MainWindow.h"
#include "App/MainApp.h"
//#include "App/MainWindow.h"
//#include "UI/UIControl.h"
#include "UI/UISystem.h"
#include "UI/UIDebug.h"
#include <QDebug>
#include <QObject>
#include <QMetaType>
#include <QMutex>
#include <QDateTime>
#include <QDesktopServices>
#include <QUrl>
#include <QGuiApplication>
#include <QPalette>
#include <QStyleHints>
#include "QTKeyHandleScrollLock.h"
#include "Form_MainWindow.h"
#include "QTKeyMappings.h"
#include "main.h"
#include <stdint.h>
#include <strings.h>

#include <QKeyEvent>
#include <QTimer>

#define STARTING_DATAEVENT_QUEUE_SIZE 10

MainApp *g_MainApp;
class MainMethodCB g_MainMethodCB;
QMutex m_DataAvail_mutex;
//t_ConID m_DataAvail_ArraySize;
//uint8_t *m_DataAvail_AlreadyBeingProcessed;

static void HandleGlobalKeyPressEvent(QKeyEvent * event);

Form_MainWindow *g_FocusedMainWindow=NULL;
static void (*m_KeyCallback)(uint8_t Mods,e_UIKeys UIKey,std::string &Text);

MainApp::MainApp(int & argc, char ** argv) : QApplication(argc,argv)
{
    App1SecTimer=new QTimer(this);
    connect(App1SecTimer, SIGNAL(timeout()), this, SLOT(App1SecTimerTick()));
    App1SecTimer->start(1000);

    App100msTimer=new QTimer(this);
    connect(App100msTimer, SIGNAL(timeout()), this, SLOT(App100msTimer_triggered()));

    App100msTimer->start(100);              // We only provide .1 sec timeouts
}

MainApp::~MainApp()
{
}

void MainApp::App100msTimer_triggered()
{
    QTScrollLockHelperTick();

    /* DEBUG PAUL: May want to provide an API to speed this up when a plugin requests a timeout below 100ms */
    AppInformOf_FileTransTimerTick();
}

void MainApp::App1SecTimerTick()
{
    App1SecTick();
}

bool MainApp::notify(QObject * receiver, QEvent * event)
{
    /* Handle Scroll Lock on Linux */
    if(event->type()==QEvent::KeyPress)
    {
        QKeyEvent *Key;
        Key=(QKeyEvent *)event;
        if(Key->key()==Qt::Key_ScrollLock)
            QTInformOfScrollLockPressed();
    }

    if(m_KeyCallback!=NULL)
    {
        if(event->type()==QEvent::KeyPress)
        {
            HandleGlobalKeyPressEvent((QKeyEvent *)event);
            return true;
        }
    }
    else
    {
        /* We do all of this so we can by pass the QAction shortcuts */
        /* I'm not sure this is working anyway.... */
        if(receiver==g_FocusedMainWindow)
        {
            if(event->type()==QEvent::KeyPress)
            {
                g_FocusedMainWindow->keyPressEvent((QKeyEvent *)event);

                /* Kill it */
                return true;
            }
            if(event->type()==QEvent::KeyRelease)
                return true;
        }
    }

    return QApplication::notify(receiver,event);
}

int main(int argc, char *argv[])
{
    MainApp a(argc, argv);
    int RetValue;

    g_MainApp=&a;

//    /* Allocate a queue */
//    m_DataEventQueue=(struct DataEventQueueEntry *)
//            malloc(sizeof(struct DataEventQueueEntry)*
//            STARTING_DATAEVENT_QUEUE_SIZE);
//    if(m_DataEventQueue==NULL)
//    {
//        /* We failed to get the queue */
//        return -1;
//    }
//    m_DataEventQueueSize=STARTING_DATAEVENT_QUEUE_SIZE;
//    memset(m_DataEventQueue,0,sizeof(sizeof(struct DataEventQueueEntry)*
//            STARTING_DATAEVENT_QUEUE_SIZE));

    qRegisterMetaType<t_IOSystemHandle *>("t_IOSystemHandle *");

    QTInitScrollLockHandler();

    if(!AppMain(argc,argv))
    {
//        free(m_DataEventQueue);
        return -1;
    }

    RetValue=a.exec();
//    free(m_DataEventQueue);
    return RetValue;
}

void UIExit(int ExitCode)
{
    g_MainApp->exit(ExitCode);
}

void DEBUG_RunEventLoop(void)
{
    g_MainApp->processEvents();
}

void DebugMsg(const char *fmt,...)
{
    char buff[1000];

    va_list args;
    va_start(args,fmt);
    vsprintf(buff,fmt,args);
    va_end (args);

    qDebug("%s",buff);
}

/*******************************************************************************
 * NAME:
 *    FlagDrvDataEvent
 *
 * SYNOPSIS:
 *    void FlagDrvDataEvent(t_IOSystemHandle *IOHandle);
 *
 * PARAMETERS:
 *    IOHandle [I] -- The IO handle that this driver is for
 *
 * FUNCTION:
 *    This function is called from a thread to inform the main thread (and
 *    system) that there is a data event on the connection.
 *
 *    This has to be in the UI because it need to tell the main thread
 *    (however that is done).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void FlagDrvDataEvent(t_IOSystemHandle *IOHandle)
{
DB_StartTimer(e_DBT_SignalFromThread);
    QMetaObject::invokeMethod(&g_MainMethodCB, "NewDataEvent",
            Qt::QueuedConnection,Q_ARG(t_IOSystemHandle *, IOHandle));

//    uint8_t *NewDataAvailArray;
//    long AdjustedConnections;   // The number of connection we need to have in the tracking array
//    bool Add2Queue; // Are we going to call NewDataEvent() by placing it on the queue?

//    /* Note that we are adding a data available */
//    m_DataAvail_mutex.lock();
//
//    m_DataAvail_mutex.unlock();

//    if(Add2Queue)
//    {
//        QMetaObject::invokeMethod(&g_MainMethodCB, "NewDataAvailable",
//                Qt::QueuedConnection,Q_ARG(t_ConID, ConnectionID));
//    }
}

/* This runs in the main thread */
void MainMethodCB::NewDataEvent(t_IOSystemHandle *IOHandle)
{
DB_StopTimer(e_DBT_SignalFromThread);
    IOS_InformOfNewDataEvent(IOHandle);
}

/*******************************************************************************
 * NAME:
 *    UI_ProcessAllPendingUIEvents
 *
 * SYNOPSIS:
 *    void UI_ProcessAllPendingUIEvents(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function processes all the pending events from the UI queue.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UI_ProcessAllPendingUIEvents(void)
{
    g_MainApp->processEvents();
}

t_UIMutex *AllocMutex(void)
{
    QMutex *NewMutex;
    try
    {
        NewMutex=new QMutex();
    }
    catch(...)
    {
        return NULL;
    }
    return (t_UIMutex *)NewMutex;
}

void FreeMutex(t_UIMutex *Mut)
{
    QMutex *TheMutex=(QMutex *)Mut;

    delete TheMutex;
}

void LockMutex(t_UIMutex *Mut)
{
    QMutex *TheMutex=(QMutex *)Mut;

    TheMutex->lock();
}

void UnLockMutex(t_UIMutex *Mut)
{
    QMutex *TheMutex=(QMutex *)Mut;

    TheMutex->unlock();
}

uint64_t GetMSCounter(void)
{
    return QDateTime::currentMSecsSinceEpoch();
}

void UI_GotoWebPage(const char *WebSite)
{
    QDesktopServices::openUrl(QUrl(WebSite));
}

int caseinsensitivestrcmp(const char *a,const char *b)
{
    return strcasecmp(a,b);
}

void UI_CollectAllKeyPresses(void (*Callback)(uint8_t Mods,e_UIKeys UIKey,std::string &Text))
{
    m_KeyCallback=Callback;
}

static void HandleGlobalKeyPressEvent(QKeyEvent *event)
{
    uint8_t Mods;
    e_UIKeys UIKey;
    std::string Text;
    char c;

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

    m_KeyCallback(Mods,UIKey,Text);
}

/* Copied from: https://stackoverflow.com/questions/75457687/detect-dark-application-style-theme-of-currently-used-desktop-in-qt */
bool OS_IsSystemInDarkMode(void)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    const auto scheme = QGuiApplication::styleHints()->colorScheme();
    return scheme == Qt::ColorScheme::Dark;
#else
    const QPalette defaultPalette;
    const auto text = defaultPalette.color(QPalette::WindowText);
    const auto window = defaultPalette.color(QPalette::Window);
    return text.lightness() > window.lightness();
#endif // QT_VERSION
}
