/*******************************************************************************
 * FILENAME: main.h
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
 *    This software is the property of Paul Hutchinson. and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (25 Aug 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __MAIN_H_
#define __MAIN_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <QObject>
#include <QApplication>
//#include "App/InputDriverSystem.h"
#include "Form_MainWindow.h"
#include "Widget_TextCanvas.h"
#include "UI/UISystem.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/
class MainApp : public QApplication
{
    Q_OBJECT 

    public:
        MainApp(int & argc, char ** argv);
        ~MainApp();
        bool notify(QObject *receiver, QEvent *event);

public slots:
        void App1SecTimerTick();
        void AppUploadDownloadTimer_triggered();

    private:
        QTimer *App1SecTimer;
        QTimer *AppUploadDownloadTimer;
};

class MainMethodCB : public QObject
{
    Q_OBJECT

public:
    MainMethodCB() : QObject() {}
    virtual ~MainMethodCB() {}

public slots:
    void NewDataEvent(t_IOSystemHandle *IOHandle);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/
extern Form_MainWindow *g_FocusedMainWindow;
extern MainApp *g_MainApp;

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
