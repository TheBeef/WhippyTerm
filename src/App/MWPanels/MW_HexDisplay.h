/*******************************************************************************
 * FILENAME: MW_HexDisplay.h
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
 *    Paul Hutchinson (16 May 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __MW_HEXDISPLAY_H_
#define __MW_HEXDISPLAY_H_

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
class MWHexDisplay
{
    public:
        MWHexDisplay();
        ~MWHexDisplay();

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
        bool HexDisplayBufferEvent(const struct HDEvent *Event);

    private:
        t_UIMainWindow *UIWin;
        class TheMainWindow *MW;
        bool PanelActive;
        class HexDisplayBuffer *IncomingHistoryHexDisplay;
        void SendSelection2Clipboard(e_ClipboardType Clip);

        void RethinkUI(void);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
