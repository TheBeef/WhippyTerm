/*******************************************************************************
 * FILENAME: MW_Capture.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    .h file
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (09 Mar 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __MW_CAPTURE_H_
#define __MW_CAPTURE_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIMainWindow.h"
#include <stdio.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/
class MWCapture
{
    public:
        MWCapture();
        ~MWCapture();

        void Setup(class TheMainWindow *Parent,t_UIMainWindow *Win);
        void ActivateCtrls(bool Active);
        void ConnectionChanged(void);
        void ConnectionAbout2Changed(void);
        void Prompt4FileAndStart(void);
        bool SelectFilename(void);
        void Start(void);
        void Stop(void);
        void PressStartBttn(void);
        void NoteFilenameChanged(void);
        void ToggleTimestamp(void);
        void ToggleAppend(void);
        void ToggleStripCtrlChars(void);
        void ToggleStripEscSeq(void);
        void ToggleHexDump(void);

    private:
        void RethinkUI(void);

        t_UIMainWindow *UIWin;
        class TheMainWindow *MW;

        bool PanelActive;
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
