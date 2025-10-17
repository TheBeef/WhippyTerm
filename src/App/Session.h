/*******************************************************************************
 * FILENAME: Session.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    .h file
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
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __SESSION_H_
#define __SESSION_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/Settings.h"
#include "App/Util/StorageHelpers.h"
#include "ThirdParty/TinyCFG/TinyCFG.h"
#include "UI/UIMainWindow.h"
#include "App/Util/CRCSystem.h"
#include "App/Util/KeyValue.h"
#include <string>
#include <list>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
struct SessionOpenConnection
{
    std::string Name;
    std::string URI;
    t_KVList Options;
    bool UseCustomSettings;
    class ConSettings CustomSettings;
    bool WasOpen;
};

typedef std::list<struct SessionOpenConnection> t_SessionOpenConnectionList;
typedef t_SessionOpenConnectionList::iterator i_SessionOpenConnectionList;

/***  CLASS DEFINITIONS                ***/
/* Adding to session:
    * Add to 'struct Session'
    * Add to Session_RegisterAllMembers()
    * Add to Session_DefaultSession()
*/
struct Session
{
    /***************/
    /* Main Window */
    /***************/

    /* Panels */
    int LeftPanelSize;
    int RightPanelSize;
    int BottomPanelSize;
    bool LeftPanelOpen;
    bool RightPanelOpen;
    bool BottomPanelOpen;
//    e_LeftPanelTabType SelectedLeftPanelTab;
//    e_RightPanelTabType SelectedRightPanelTab;
//    e_BottomPanelTabType SelectedBottomPanelTab;

    /* Other */
    bool AppMaximized;
    int WindowPosX;
    int WindowPosY;
    int WindowWidth;
    int WindowHeight;
    std::string SendBufferPath; // The path that we load/save buffers
    t_SessionOpenConnectionList OpenConnections;

    /* Connections */
    std::string LastConnectionOpened;
    t_ConnectionsOptions ConnectionsOptions;
    t_ConUploadOptions LastUsedUploadOptions;
    t_ConDownloadOptions LastUsedDownloadOptions;

    /* Edit Buffer */
    e_CRCType LastSelectedCRCType;

    /* Calc CRC */
    e_CRCType LastSelectedCalcCRCType;
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/
extern struct Session g_Session;

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
void InitSessionSystem(void);
bool SaveSession(const char *Filename=NULL);
bool LoadSession(const char *Filename=NULL);
void AutoSaveSessionTick(void);
void SaveSessionIfNeeded(void);
void NoteSessionChanged(void);
void ScanOpenConnections2Session(void);

#endif   /* end of "#ifndef __SESSION_H_" */
