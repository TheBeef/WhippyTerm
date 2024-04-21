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
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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

    private:
        t_UIMainWindow *UIWin;
        class TheMainWindow *MW;
        bool PanelActive;
        class HexDisplayBuffer *Buffer2SendHexDisplay;
        void SendSelection2Clipboard(e_ClipboardType Clip);
        e_CmdType ConvertBuffer2Cmd(int Buffer);

        void RethinkUI(void);
        void RethinkBuffer(void);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
