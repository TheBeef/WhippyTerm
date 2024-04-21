/*******************************************************************************
 * FILENAME: MW_StopWatch.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    It's a .h file.
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
 *    Paul Hutchinson (03 Mar 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __MW_STOPWATCH_H_
#define __MW_STOPWATCH_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIMainWindow.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/
class MWStopWatch
{
    public:
        MWStopWatch();
        ~MWStopWatch();

        void Setup(class TheMainWindow *Parent,t_UIMainWindow *Win);
        void ActivateCtrls(bool Active);
        void ConnectionChanged(void);
        void ConnectionAbout2Changed(void);
        void ToggleStartStop(void);
        void StartTimer(void);
        void StopTimer(void);
        void ResetTimer(void);
        void AddLap(void);
        void ClearLaps(void);
        void UpdateDisplayedTime(void);
        void ToggleAutoStartOnTx(void);
        void ToggleAutoLap(void);

        void AddLapLine(uint64_t LapTime, uint64_t LapDelta);
        void TimerAutoStarted(void);

    private:
        t_UIMainWindow *UIWin;
        class TheMainWindow *MW;
        bool PanelActive;

        void EnableAllControls(bool Enabled);
        void UpdateUIForStartStop(bool Start);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
