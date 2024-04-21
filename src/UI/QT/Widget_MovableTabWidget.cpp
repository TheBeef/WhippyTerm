/*******************************************************************************
 * FILENAME: Widget_MovableTabWidget.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * CREATED BY:
 *    Paul Hutchinson (27 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Widget_MovableTabWidget.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

MovableTabWidget::MovableTabWidget(QWidget *parent) : QTabWidget(parent)
{
    connect(tabBar(),SIGNAL(tabMoved(int,int)),this,SLOT(ReSendTabMoved(int,int)));
}

void MovableTabWidget::ReSendTabMoved(int from,int to)
{
    emit TabMoved(from,to);
}

