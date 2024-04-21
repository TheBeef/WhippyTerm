/*******************************************************************************
 * FILENAME: DynamicUI.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2018 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson. and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * HISTORY:
 *    Paul Hutchinson (10 Jul 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __DYNAMICUI_H_
#define __DYNAMICUI_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <QObject>
#include <QComboBox>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/
class DynQComboBox : public QComboBox
{
    Q_OBJECT

public:
    DynQComboBox(QWidget *parent=0) : QComboBox(parent) {}

    void (*EventCB)(const struct DynEvent *Event);

public slots:
    void ComboBoxChanged(int index);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
