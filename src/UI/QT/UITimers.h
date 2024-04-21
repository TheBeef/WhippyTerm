/*******************************************************************************
 * FILENAME: UITimers.h
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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (23 Sep 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __QTUITIMERS_H_
#define __QTUITIMERS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <QObject>
#include <QWidget>
#include <QTimer>
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/
class UITimers : public QWidget
{
    Q_OBJECT

public:
    UITimers(QWidget *parent = 0);
    ~UITimers();
    QTimer *Timer;
    void (*TimeoutCB)(uintptr_t UserData);
    uintptr_t UserData;
    bool Repeats;
    int TimeoutIn_ms;

public slots:
    void TimerTick();
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
