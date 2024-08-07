/*******************************************************************************
 * FILENAME: KeyValueSupport.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the support for a key value list type in it.
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
 * CREATED BY:
 *    Paul Hutchinson (27 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Util/KeyValue.h"
#include "PluginSDK/Plugin.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    PI_KVClear
 *
 * SYNOPSIS:
 *    void PI_KVClear(t_PIKVList *Handle)
 *
 * PARAMETERS:
 *    Handle [I] -- The KV list to work on
 *
 * FUNCTION:
 *    This function clears the list of KV entries (erases the list).
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PI_KVAddItem(), PI_KVGetItem()
 ******************************************************************************/
void PI_KVClear(t_PIKVList *Handle)
{
    t_KVList *List=(t_KVList *)Handle;

    try
    {
        List->clear();
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    PI_KVAddItem
 *
 * SYNOPSIS:
 *    bool PI_KVAddItem(t_PIKVList *Handle,const char *Key,const char *Value)
 *
 * PARAMETERS:
 *    Handle [I] -- The KV list to work on
 *    Key [I] -- The key to add / override
 *    Value [I] -- The value we set this key to.
 *
 * FUNCTION:
 *    This function adds a new KV key and value to the list.  If the key already
 *    exists the value will be overriden
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    PI_KVClear(), PI_KVGetItem()
 ******************************************************************************/
bool PI_KVAddItem(t_PIKVList *Handle,const char *Key,const char *Value)
{
    t_KVList *List=(t_KVList *)Handle;

    try
    {
        (*List)[Key]=Value;
    }
    catch(...)
    {
        return false;
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    PI_KVGetItem
 *
 * SYNOPSIS:
 *    const char *PI_KVGetItem(const t_PIKVList *Handle,const char *Key);
 *
 * PARAMETERS:
 *    Handle [I] -- The KV list to work on
 *    Key [I] -- The key to look up
 *
 * FUNCTION:
 *    This function looks up a key and returns it's value from the KV list.
 *
 * RETURNS:
 *    A pointer to the value of the key or NULL if it was not found.  This
 *    remains valid until the list is modified or freed.
 *
 * SEE ALSO:
 *    PI_KVClear(), PI_KVAddItem()
 ******************************************************************************/
const char *PI_KVGetItem(const t_PIKVList *Handle,const char *Key)
{
    t_KVList *List=(t_KVList *)Handle;
    i_KVList Iter;

    try
    {
        Iter=List->find(Key);
        if(Iter==List->end())
            return NULL;
        return Iter->second.c_str();
    }
    catch(...)
    {
        return NULL;
    }
}
