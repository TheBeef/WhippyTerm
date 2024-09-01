/*******************************************************************************
 * FILENAME: MW_AuxControls.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This is a .h file.
 *
 * COPYRIGHT:
 *    Copyright 2024 Paul Hutchinson.
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
 *    Paul Hutchinson (16 Aug 2024)
 *       Created
 *
 *******************************************************************************/
#ifndef __MW_AUXCONTROLS_H_
#define __MW_AUXCONTROLS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/IOSystem.h"
#include "App/Util/KeyValue.h"
#include "UI/UIMainWindow.h"
#include <map>
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef std::map<class Connection *,t_ConnectionAuxCtrlsDataType *> t_MWAuxControlsConWidgets;
typedef t_MWAuxControlsConWidgets::iterator i_MWAuxControlsConWidgets;

/***  CLASS DEFINITIONS                ***/
class MWAuxControls
{
    public:
        MWAuxControls();
        ~MWAuxControls();

        void Setup(class TheMainWindow *Parent,t_UIMainWindow *Win);
        void ActivateCtrls(bool Active);

        bool NewConnection(class Connection *Con);
        void RemoveConnection(class Connection *Con);
        void ConnectionChanged(void);
        void ConnectionAbout2Changed(void);

    private:
        t_MWAuxControlsConWidgets ConWidgets;
        t_UIMainWindow *UIWin;
        class TheMainWindow *MW;
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif   /* end of "#ifndef __MW_AUXCONTROLS_H_" */
