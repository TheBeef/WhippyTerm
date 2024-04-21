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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
