/*******************************************************************************
 * FILENAME: MW_Bridge.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    It's a .h file.
 *
 * COPYRIGHT:
 *    Copyright 2022 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (26 Aug 2022)
 *       Created
 *
 *******************************************************************************/
#ifndef __MW_BRIDGE_H_
#define __MW_BRIDGE_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "UI/UIMainWindow.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/
class MWBridge
{
    public:
        MWBridge();
        ~MWBridge();

        void Setup(class TheMainWindow *Parent,t_UIMainWindow *Win);
        void ActivateCtrls(bool Active);
        void ConnectionChanged(void);
        void ConnectionAbout2Changed(void);
        void BridgeConnections(void);
        void ReleaseConnections(void);
        void ConnectionAddedRemoved(void);
        void SelectedConnectionChanged(void);
        void LockConnectionChange(int Connection);
        void ConnectionAttribChanged(void);
        void ConnectionBridgedChanged(class Connection *Con1,
                class Connection *Con2);

    private:
        t_UIMainWindow *UIWin;
        class TheMainWindow *MW;
        bool PanelActive;

        void RethinkControls(void);
        void RethinkLockNames(void);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
