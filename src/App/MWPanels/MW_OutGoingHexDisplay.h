/*******************************************************************************
 * FILENAME: MW_OutGoingHexDisplay.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 24 Nov 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (24 Nov 2025)
 *       Created
 *
 *******************************************************************************/
#ifndef __MW_OUTGOINGHEXDISPLAY_H_
#define __MW_OUTGOINGHEXDISPLAY_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/Display/HexDisplayBuffers.h"
#include "UI/UIMainWindow.h"
#include "UI/UIClipboard.h"
#include <list>
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/
class MWOutGoingHexDisplay
{
    public:
        MWOutGoingHexDisplay();
        ~MWOutGoingHexDisplay();

        void Setup(class TheMainWindow *Parent,t_UIMainWindow *Win);
        void ActivateCtrls(bool Active);
        void ConnectionChanged(void);
        void ConnectionAbout2Changed(void);
        void NewConnectionAllocated(class Connection *NewCon);
        void TogglePause(void);
        void InformOfUpdate(class Connection *EffectedCon,const struct ConMWHexDisplayData *UpdateInfo);
        void InformOfBufferChange(class Connection *EffectedCon,const struct ConMWHexDisplayData *UpdateInfo);
        void InformOfPanelTabChange(e_BottomPanelTabType PanelTab);
        void Clear(void);
        void Copy2Clip(void);
        void CopyAs(void);
        void Save(void);
        bool HexDisplayBufferEvent(const struct HDEvent *Event);

    private:
        t_UIMainWindow *UIWin;
        class TheMainWindow *MW;
        bool PanelActive;
        class HexDisplayBuffer *OutGoingHistoryHexDisplay;
        void SendSelection2Clipboard(e_ClipboardType Clip);
        void OpenFindCRCAlgDialog(void);

        void RethinkUI(void);
        void OpenSendBufferDialog(void);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif   /* end of "#ifndef __MW_OUTGOINGHEXDISPLAY_H_" */
