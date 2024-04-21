/*******************************************************************************
 * FILENAME: MW_Download.h
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
 *    Paul Hutchinson (04 May 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __MW_DOWNLOAD_H_
#define __MW_DOWNLOAD_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIMainWindow.h"
#include "App/FileTransferProtocolSystem.h"
#include <list>
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef std::list<e_UIMenuCtrl *> t_FTPDownloadMenuList;
typedef t_FTPDownloadMenuList::iterator i_FTPDownloadMenuList;

/***  CLASS DEFINITIONS                ***/
class MWDownload
{
    public:
        MWDownload();
        ~MWDownload();

        void Setup(class TheMainWindow *Parent,t_UIMainWindow *Win);
        void ActivateCtrls(bool Active);
        void ConnectionChanged(void);
        void ConnectionAbout2Changed(void);
        void NewConnectionAllocated(class Connection *NewCon);
        void Start(void);
        void Abort(void);
        void DownloadProtocolChange(int Index);
        void DownloadStatChanged(void);
        void InformOfDownloadAborted(void);
        void InformOfDownloadDone(void);
        void DownloadMenuTriggered(uint64_t ID);

    private:
        t_UIMainWindow *UIWin;
        class TheMainWindow *MW;
        t_FTPDownloadMenuList MenuItems;
        t_FTPS_ProtocolInfoType FTPsAvail;
        bool PanelActive;
        t_ProtocolOptionsDataType *OptionWidgets;

        void RethinkUI(void);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
