/*******************************************************************************
 * FILENAME: MW_SendBuffers.h
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
 *    Paul Hutchinson (17 Jul 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __MW_SENDBUFFERS_H_
#define __MW_SENDBUFFERS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/Commands.h"
#include "App/Display/HexDisplayBuffers.h"
#include "UI/UIMainWindow.h"
#include <list>
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/
class MWSendBuffers
{
    public:
        MWSendBuffers();
        ~MWSendBuffers();

        void Setup(class TheMainWindow *Parent,t_UIMainWindow *Win);
        void ActivateCtrls(bool Active);
        void ConnectionChanged(void);
        void ConnectionAbout2Changed(void);
        void NewConnectionAllocated(class Connection *NewCon);
        bool HexDisplayBufferEvent(const struct HDEvent *Event);
        void Copy2Clip(void);
        bool SendBuffersBufferEvent(const struct HDEvent *Event);
        void SelectedBufferChanged(int BufferIndex);
        void SendBttn2Cmd(void);
        void SendBuffer(int Buffer);
        void EditCurrentBuffer(void);
        void NewSendBufferSetLoaded(void);
        void SendCurrentBuffer(void);

    private:
        t_UIMainWindow *UIWin;
        class TheMainWindow *MW;
        bool PanelActive;
        class HexDisplayBuffer *Buffer2SendHexDisplay;
        void SendSelection2Clipboard(e_ClipboardType Clip);
        e_CmdType ConvertBuffer2Cmd(int Buffer);

        void RethinkUI(void);
        void RethinkBuffer(void);
        void RethinkBufferList(void);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
