/*******************************************************************************
 * FILENAME: MW_Upload.h
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
 *    Paul Hutchinson (17 Mar 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __MW_UPLOAD_H_
#define __MW_UPLOAD_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIMainWindow.h"
#include "App/FileTransferProtocolSystem.h"
#include <list>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef std::list<e_UIMenuCtrl *> t_FTPUploadMenuList;
typedef t_FTPUploadMenuList::iterator i_FTPUploadMenuList;

/***  CLASS DEFINITIONS                ***/
class MWUpload
{
    public:
        MWUpload();
        ~MWUpload();

        void Setup(class TheMainWindow *Parent,t_UIMainWindow *Win);
        void RescanAvailableProtocols(void);
        void FreePluginResourcesIfNeeded(const char *PluginIDStr);
        void ActivateCtrls(bool Active);
        void ConnectionChanged(void);
        void ConnectionAbout2Changed(void);
        void NewConnectionAllocated(class Connection *NewCon);
        void SelectFilename(void);
        void Start(void);
        void Abort(void);
        void UploadProtocolChange(int Index);
        void NoteFilenameChanged(void);
        void UploadStatChanged(void);
        void InformOfUploadAborted(void);
        void InformOfUploadDone(void);
        void UploadMenuTriggered(uint64_t ID);
        void UpdateGUI(void);

    private:
        t_UIMainWindow *UIWin;
        class TheMainWindow *MW;
        t_FTPUploadMenuList MenuItems;
        t_FTPS_ProtocolInfoType FTPsAvail;
        bool PanelActive;
        t_ProtocolOptionsDataType *OptionWidgets;

        void RethinkUI(void);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
