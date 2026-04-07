/*******************************************************************************
 * FILENAME: Thread.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    Win32 implementation using CreateMutex and CreateThread.
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
#include <windows.h>

/*** TYPE DEFINITIONS         ***/
struct ThreadLaunchData
{
    void (*fn)(void *arg);
    void *arg;
};

/*** FUNCTION PROTOTYPES      ***/
static DWORD WINAPI ThreadLaunchFn(LPVOID TmpData);

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
struct ThreadHandle *StartThread(void (*ThreadFn)(void *), void *Arg)
{
    HANDLE hThread = NULL;
    struct ThreadLaunchData *NewLaunchData = NULL;

    try
    {
        NewLaunchData = new struct ThreadLaunchData;
        NewLaunchData->fn = ThreadFn;
        NewLaunchData->arg = Arg;

        hThread = CreateThread(NULL, 0, ThreadLaunchFn, NewLaunchData, 0, NULL);

        if (hThread == NULL)
            throw(0);
    }
    catch (...)
    {
        if (NewLaunchData != NULL)
            delete NewLaunchData;
        hThread = NULL;
    }

    return (struct ThreadHandle *)hThread;
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
    HANDLE hThread = (HANDLE)Thread;

    if (hThread != NULL)
    {
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    }
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
    HANDLE hMutex = CreateMutex(
        NULL,               // default security attributes
        FALSE,              // initially not owned
        NULL);              // unnamed mutex

    return (struct ThreadMutex *)hMutex;
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
    HANDLE hMutex = (HANDLE)Mutex;

    if (hMutex != NULL)
    {
        CloseHandle(hMutex);
    }
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
    HANDLE hMutex = (HANDLE)Mutex;

    if (hMutex != NULL)
    {
        // Blocks until ownership is granted
        WaitForSingleObject(hMutex, INFINITE);
    }
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
    HANDLE hMutex = (HANDLE)Mutex;

    if (hMutex != NULL)
    {
        ReleaseMutex(hMutex);
    }
}

/*******************************************************************************
 * NAME:
 *    ThreadLaunchFn
 *
 * SYNOPSIS:
 *    static DWORD WINAPI ThreadLaunchFn(LPVOID TmpData)
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
static DWORD WINAPI ThreadLaunchFn(LPVOID TmpData)
{
    struct ThreadLaunchData *LData = (struct ThreadLaunchData *)TmpData;

    if (LData != NULL)
    {
        LData->fn(LData->arg);
        delete LData;
    }

    return 0;
}
