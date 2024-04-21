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
