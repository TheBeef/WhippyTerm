/*******************************************************************************
 * FILENAME: Widget_MovableTabWidget.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __WIDGET_MOVABLETABWIDGET_H_
#define __WIDGET_MOVABLETABWIDGET_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <QtWidgets>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/
class MovableTabWidget : public QTabWidget
{
    Q_OBJECT

    public:
        MovableTabWidget(QWidget *parent = 0);

signals:
        void TabMoved(int,int);

private slots:
        void ReSendTabMoved(int from,int to);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
