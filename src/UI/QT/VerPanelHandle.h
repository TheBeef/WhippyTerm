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
 *    This software is the property of Paul Hutchinson. and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
