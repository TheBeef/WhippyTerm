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
