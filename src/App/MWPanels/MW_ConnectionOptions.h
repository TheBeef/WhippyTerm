/*******************************************************************************
 * FILENAME: MW_ConnectionOptions.h
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This is a .h file.
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (03 Mar 2021)
 *       Created
 *
 *******************************************************************************/
#ifndef __MW_CONNECTIONOPTIONS_H_
#define __MW_CONNECTIONOPTIONS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/IOSystem.h"
#include "App/Util/KeyValue.h"
#include "UI/UIMainWindow.h"
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/
class MWConnectionOptions
{
    public:
        MWConnectionOptions();
        ~MWConnectionOptions();

        void Setup(class TheMainWindow *Parent,t_UIMainWindow *Win);
        void ActivateCtrls(bool Active);

        void ApplyConnectionOptions(class Connection *ApplyCon);
        bool SetupConnectionOptionsPanel(const std::string &UniqueID,t_KVList &Options);
        void ConnectionChanged(void);
        void ConnectionAbout2Changed(void);

        void OptionChanged(void);

    private:
        t_ConnectionOptionsDataType *ConnectionOptionsWidgets;
        t_UIMainWindow *UIWin;
        class TheMainWindow *MW;
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
