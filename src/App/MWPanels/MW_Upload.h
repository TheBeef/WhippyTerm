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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
