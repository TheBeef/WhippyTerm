/*******************************************************************************
 * FILENAME: UITimers.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This has the UI timers in it.
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
 * CREATED BY:
 *    Paul Hutchinson (22 Sep 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "UITimers.h"
#include "UI/UITimers.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

UITimers::UITimers(QWidget *parent) : QWidget(parent)
{
}

UITimers::~UITimers()
{
}

void UITimers::TimerTick()
{
    if(TimeoutCB!=NULL)
        TimeoutCB(UserData);
}

/*******************************************************************************
 * NAME:
 *    AllocUITimer
 *
 * SYNOPSIS:
 *    struct UITimer *AllocUITimer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates a UI timer.
 *
 * RETURNS:
 *    A pointer to the timer or NULL if there was an error
 *
 * SEE ALSO:
 *    FreeUITimer(), 
 ******************************************************************************/
struct UITimer *AllocUITimer(void)
{
    class UITimers *NewTimer;

    NewTimer=NULL;
    try
    {
        NewTimer=new class UITimers(NULL);
        NewTimer->Timer=new QTimer(NewTimer);

        QObject::connect(NewTimer->Timer,SIGNAL(timeout()),NewTimer,SLOT(TimerTick()));
    }
    catch(...)
    {
        if(NewTimer!=NULL)
            delete NewTimer;
        return NULL;
    }
    return (struct UITimer *)NewTimer;
}

/*******************************************************************************
 * NAME:
 *    FreeUITimer
 *
 * SYNOPSIS:
 *    void FreeUITimer(struct UITimer *Timer);
 *
 * PARAMETERS:
 *    Timer [I] -- The timer to free
 *
 * FUNCTION:
 *    This function frees a UI timer allocated AllocUITimer()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    AllocUITimer()
 ******************************************************************************/
void FreeUITimer(struct UITimer *Timer)
{
    class UITimers *RealTimer=(class UITimers *)Timer;

    delete RealTimer;
}

/*******************************************************************************
 * NAME:
 *    SetupUITimer
 *
 * SYNOPSIS:
 *    void SetupUITimer(struct UITimer *Timer,
 *          void (*Timeout)(uintptr_t UserData),uintptr_t UserData,
 *          bool Repeats);
 *
 * PARAMETERS:
 *    Timer [I] -- The timer to work on
 *    Timeout [I] -- The function to call when the timer goes off
 *    UserData [I] -- The user data to send to the call back.
 *    Repeats [I] -- Does this timer repeat or is it a one time timer (when
 *                   started does it just keep sending events or does it
 *                   have to be restarted manually)
 *
 * FUNCTION:
 *    
 *
 * RETURNS:
 *    NONE
 *
 * CALLBACKS:
 *==============================================================================
 * NAME:
 *    Timeout
 *
 * SYNOPSIS:
 *    void Timeout(uintptr_t UserData);
 *
 * PARAMETERS:
 *    UserData [I] -- The user data app that was sent into SetupUITimer()
 *
 * FUNCTION:
 *    This function is called when the timer goes off
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 *==============================================================================
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void SetupUITimer(struct UITimer *Timer,void (*Timeout)(uintptr_t UserData),
        uintptr_t UserData,bool Repeats)
{
    class UITimers *RealTimer=(class UITimers *)Timer;

    RealTimer->TimeoutCB=Timeout;
    RealTimer->UserData=UserData;
    RealTimer->Repeats=Repeats;
}

/*******************************************************************************
 * NAME:
 *    UITimerSetTimeout
 *
 * SYNOPSIS:
 *    void UITimerSetTimeout(struct UITimer *Timer,uint32_t ms);
 *
 * PARAMETERS:
 *    Timer [I] -- The timer to work on
 *    ms [I] -- The number of ms before this timer goes off
 *
 * FUNCTION:
 *    This function changes the timer out for a timer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UITimerSetTimeout(struct UITimer *Timer,uint32_t ms)
{
    class UITimers *RealTimer=(class UITimers *)Timer;

    RealTimer->TimeoutIn_ms=ms;
}

/*******************************************************************************
 * NAME:
 *    UITimerStart
 *
 * SYNOPSIS:
 *    void UITimerStart(struct UITimer *Timer);
 *
 * PARAMETERS:
 *    Timer [I] -- The timer to work on
 *
 * FUNCTION:
 *    This function starts the timer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UITimerStart(struct UITimer *Timer)
{
    class UITimers *RealTimer=(class UITimers *)Timer;

    RealTimer->Timer->setSingleShot(!RealTimer->Repeats);
    RealTimer->Timer->start(RealTimer->TimeoutIn_ms);
}

/*******************************************************************************
 * NAME:
 *    UITimerStop
 *
 * SYNOPSIS:
 *    void UITimerStop(struct UITimer *Timer);
 *
 * PARAMETERS:
 *    Timer [I] -- The timer to work on
 *
 * FUNCTION:
 *    This function stops the timer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UITimerStop(struct UITimer *Timer)
{
    class UITimers *RealTimer=(class UITimers *)Timer;

    RealTimer->Timer->stop();
}

/*******************************************************************************
 * NAME:
 *    UITimerRunning
 *
 * SYNOPSIS:
 *    bool UITimerRunning(struct UITimer *Timer);
 *
 * PARAMETERS:
 *    Timer [I] -- The timer to work on
 *
 * FUNCTION:
 *    This function checks to see if a timer is running (started)
 *
 * RETURNS:
 *    true -- Timer is running
 *    false -- Timer is stopped
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UITimerRunning(struct UITimer *Timer)
{
    class UITimers *RealTimer=(class UITimers *)Timer;

    return RealTimer->Timer->isActive();
}

