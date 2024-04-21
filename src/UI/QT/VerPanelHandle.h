/*******************************************************************************
 * FILENAME: VerPanelHandle.h
 * 
 * PROJECT: 
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    A custom widget for the vert panel handle
 *
 * COPYRIGHT:
 *    Copyright 2018 Paul Hutchinson.
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
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __VERPANELHANDLE_H_
#define __VERPANELHANDLE_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <QFrame>
#include "Form_MainWindow.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/
class VerPanelHandle : public QFrame
{
    Q_OBJECT
    
public:
    VerPanelHandle(QWidget *parent = 0);
    void ChangeArrowDir(bool Up);
    void ClearHighlight(void);
    void LeftMode(void);
    void RegisterClickHandler(Form_MainWindow *MW);

protected:
    void paintEvent(QPaintEvent *);

private:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool MouseIn;
    bool DrawPointingLeft;
    bool LeftPanel;
    Form_MainWindow *MainWindow;
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif   /* end of "#ifndef __VERPANELHANDLE_H_" */
