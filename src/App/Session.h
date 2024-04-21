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
 *    This software is the property of Paul Hutchinson. and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __SESSION_H_
#define __SESSION_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/Util/StorageHelpers.h"
#include "ThirdParty/TinyCFG/TinyCFG.h"
#include "UI/UIMainWindow.h"
#include "App/Util/CRCSystem.h"
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/
/* Adding to session:
    * Add to 'struct Session'
    * Add to Session_RegisterAllMembers()
    * Add to Session_DefaultSession()
    * Add to Session_Changed()
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

    /* Connections */
    std::string LastConnectionOpened;
    t_ConnectionsOptions ConnectionsOptions;
    t_ConUploadOptions LastUsedUploadOptions;
    t_ConDownloadOptions LastUsedDownloadOptions;

    /* Edit Buffer */
    e_CRCType LastSelectedCRCType;
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

#endif   /* end of "#ifndef __SESSION_H_" */
