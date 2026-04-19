/*******************************************************************************
 * FILENAME: Thread.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 06 Apr 2026 Paul Hutchinson.
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
 *    Paul Hutchinson (06 Apr 2026)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "OS/Thread.h"
#include <pthread.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct ThreadLaunchData
{
    void (*fn)(void *arg);
    void *arg;
};

/*** FUNCTION PROTOTYPES      ***/
static void *ThreadLaunchFn(void *TmpData);

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    StartThread
 *
 * SYNOPSIS:
 *    struct ThreadHandle *StartThread(void (*ThreadFn)(void *),void *Arg);
 *
 * PARAMETERS:
 *    ThreadFn [I] -- The function to call as the threads main()
 *    Arg [I] -- The arg to send to 'ThreadFn'
 *
 * FUNCTION:
 *    This function allocates and starts a thread.
 *
 * RETURNS:
 *    A pointer to the thread handle or NULL if there was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
struct ThreadHandle *StartThread(void (*ThreadFn)(void *),void *Arg)
{
    pthread_t *NewThread;
    int ret;
    struct ThreadLaunchData *NewLaunchData;

    NewThread=NULL;
    NewLaunchData=NULL;
    try
    {
        NewLaunchData=new struct ThreadLaunchData;
        NewLaunchData->fn=ThreadFn;
        NewLaunchData->arg=Arg;

        NewThread=new pthread_t;

        ret=pthread_create(NewThread,NULL,ThreadLaunchFn,(void *)NewLaunchData);
        if(ret!=0)
            throw(0);

        pthread_detach(*NewThread);
    }
    catch(...)
    {
        if(NewThread!=NULL)
            delete NewThread;
        if(NewLaunchData!=NULL)
            delete NewLaunchData;
        NewThread=NULL;
    }
    return (struct ThreadHandle *)NewThread;
}

/*******************************************************************************
 * NAME:
 *    Wait4ThreadToExit
 *
 * SYNOPSIS:
 *    void Wait4ThreadToExit(struct ThreadHandle *Thread);
 *
 * PARAMETERS:
 *    Thread [I] -- The thread to work on
 *
 * FUNCTION:
 *    This function waits for a thread to exit and then frees the thread handle.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    StartThread()
 ******************************************************************************/
void Wait4ThreadToExit(struct ThreadHandle *Thread)
{
    pthread_t *RealThread=(pthread_t *)Thread;

    pthread_join(*RealThread,NULL);

    delete RealThread;
}


/*******************************************************************************
 * NAME:
 *    AllocMutex
 *
 * SYNOPSIS:
 *    struct ThreadMutex *AllocMutex(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates a mutex
 *
 * RETURNS:
 *    A pointer to the mutex or NULL if there was an error
 *
 * SEE ALSO:
 *    FreeMutex()
 ******************************************************************************/
struct ThreadMutex *AllocMutex(void)
{
    pthread_mutex_t *NewMutex;

    NewMutex=NULL;
    try
    {
        NewMutex=new pthread_mutex_t;

        if(pthread_mutex_init(NewMutex,NULL)!=0)
            throw(0);
    }
    catch(...)
    {
        if(NewMutex!=NULL)
            delete NewMutex;
    }
    return (struct ThreadMutex *)NewMutex;
}

/*******************************************************************************
 * NAME:
 *    FreeMutex
 *
 * SYNOPSIS:
 *    void FreeMutex(struct ThreadMutex *Mutex);
 *
 * PARAMETERS:
 *    Mutex [I] -- The mutex to work on
 *
 * FUNCTION:
 *    This function frees a mutex that was allocated with AllocMutex()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    AllocMutex()
 ******************************************************************************/
void FreeMutex(struct ThreadMutex *Mutex)
{
    pthread_mutex_t *RealMutex=(pthread_mutex_t *)Mutex;

    pthread_mutex_destroy(RealMutex);

    delete RealMutex;
}

/*******************************************************************************
 * NAME:
 *    LockMutex
 *
 * SYNOPSIS:
 *    void LockMutex(struct ThreadMutex *Mutex);
 *
 * PARAMETERS:
 *    Mutex [I] -- The mutex to work on
 *
 * FUNCTION:
 *    This function locks a mutex.  If the mutex is locked by someone else
 *    then this function blocks until it's unlocked.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UnLockMutex()
 ******************************************************************************/
void LockMutex(struct ThreadMutex *Mutex)
{
    pthread_mutex_t *RealMutex=(pthread_mutex_t *)Mutex;

    pthread_mutex_lock(RealMutex);
}

/*******************************************************************************
 * NAME:
 *    UnLockMutex
 *
 * SYNOPSIS:
 *    void UnLockMutex(struct ThreadMutex *Mutex);
 *
 * PARAMETERS:
 *    Mutex [I] -- The mutex to work on
 *
 * FUNCTION:
 *    This function unlocks a mutex.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    LockMutex()
 ******************************************************************************/
void UnLockMutex(struct ThreadMutex *Mutex)
{
    pthread_mutex_t *RealMutex=(pthread_mutex_t *)Mutex;

    pthread_mutex_unlock(RealMutex);
}

/*******************************************************************************
 * NAME:
 *    ThreadLaunchFn
 *
 * SYNOPSIS:
 *    static void *ThreadLaunchFn(void *TmpData);
 *
 * PARAMETERS:
 *    TmpData [I] -- The launch data
 *
 * FUNCTION:
 *    This is a helper function that runs in the thread and calls the real
 *    threads start function.
 *
 * RETURNS:
 *    0
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void *ThreadLaunchFn(void *TmpData)
{
    struct ThreadLaunchData *LData=(struct ThreadLaunchData *)TmpData;

    LData->fn(LData->arg);
    delete LData;

    return 0;
}
