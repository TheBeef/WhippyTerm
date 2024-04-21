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
