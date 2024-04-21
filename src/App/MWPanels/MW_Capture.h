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
