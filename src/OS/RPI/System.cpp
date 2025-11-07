/*******************************************************************************
 * FILENAME: System.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
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
 *    Paul Hutchinson (21 Feb 2022)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "OS/System.h"
#include <signal.h>
#include <dlfcn.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    InitOS
 *
 * SYNOPSIS:
 *    void InitOS(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called to init anything OS related.  Nothing else
 *    is up and running so it's OS only stuff.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void InitOS(void)
{
    /* Kill of sig pipe as we want return codes from read/send/etc instead of
       exiting */
    signal(SIGPIPE, SIG_IGN);
}

/*******************************************************************************
 * NAME:
 *    LoadDLL
 *
 * SYNOPSIS:
 *    struct DLLHandle *LoadDLL(const char *Filename);
 *
 * PARAMETERS:
 *    Filename [I] -- The filename of DLL to load.
 *
 * FUNCTION:
 *    This function loads a dll / shared object and returns a handle to it.
 *
 * RETURNS:
 *    A handle to the DLL or NULL if there was a problem.
 *
 * SEE ALSO:
 *    CloseDLL(), DLL2Function()
 ******************************************************************************/
struct DLLHandle *LoadDLL(const char *Filename)
{
    void *RealHandle;

    RealHandle=dlopen(Filename,RTLD_NOW);

    return (struct DLLHandle *)RealHandle;
}

/*******************************************************************************
 * NAME:
 *    DLL2Function
 *
 * SYNOPSIS:
 *    void *DLL2Function(struct DLLHandle *Handle,const char *FnName);
 *
 * PARAMETERS:
 *    Handle [I] -- The DLL handle returned from LoadDLL()
 *    FnName [I] -- The name of the function to load
 *
 * FUNCTION:
 *    This function finds a function in a DLL.
 *
 * RETURNS:
 *    A pointer to the function or NULL if it could not be found.
 *
 * SEE ALSO:
 *    LoadDLL()
 ******************************************************************************/
void *DLL2Function(struct DLLHandle *Handle,const char *FnName)
{
    void *RealHandle=(void *)Handle;

    return dlsym(RealHandle,FnName);
}

/*******************************************************************************
 * NAME:
 *    CloseDLL
 *
 * SYNOPSIS:
 *    void CloseDLL(struct DLLHandle *Handle);
 *
 * PARAMETERS:
 *    Handle [I] -- The DLL handle returned from LoadDLL()
 *
 * FUNCTION:
 *    This function frees a DLL loaded with LoadDLL().
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    After you close a DLL handle you can no longer call functions in it.
 *
 * SEE ALSO:
 *    LoadDLL()
 ******************************************************************************/
void CloseDLL(struct DLLHandle *Handle)
{
    void *RealHandle=(void *)Handle;

    dlclose(RealHandle);
}

/*******************************************************************************
 * NAME:
 *    LastDLLError
 *
 * SYNOPSIS:
 *    const char *LastDLLError(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets text about the last error that we got from a DLL
 *    function.
 *
 * RETURNS:
 *    A string with the error code in it.  This is valid until the next call
 *    to a DLL funciton.  It may also return NULL if there was no error or
 *    the error to txt is not supported.
 *
 * LIMITATIONS:
 *    If this is not supported it returns NULL.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
const char *LastDLLError(void)
{
    return dlerror();
}

/*******************************************************************************
 * NAME:
 *    RunningOS
 *
 * SYNOPSIS:
 *    e_OSType RunningOS(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function returns what OS we are running on.
 *
 * RETURNS:
 *    e_OS_Linux -- We are running Linux
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
e_OSType RunningOS(void)
{
    return e_OS_RaspberryPI;
}

/*******************************************************************************
 * NAME:
 *    RunningExeBits
 *
 * SYNOPSIS:
 *    int RunningExeBits(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function tells if we are running a 32 or 64 bit executable.
 *
 * RETURNS:
 *    32 -- 32 bit exe
 *    64 -- 64 bit exe
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int RunningExeBits(void)
{
    return 64;
}
