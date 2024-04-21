/*******************************************************************************
 * FILENAME: QTPlugins.h
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
 *    Paul Hutchinson (12 Jan 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __QTPLUGINS_H_
#define __QTPLUGINS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <QObject>
#include <QComboBox>
#include <QRadioButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/
class PIQComboBox : public QComboBox
{
    Q_OBJECT

public:
    PIQComboBox(QWidget *parent=0) : QComboBox(parent) {}

    void (*EventCB)(const struct PICBEvent *Event,void *UserData);
    void *UserData;

public slots:
    void ComboBoxChanged(int index);
    void ComboBoxTextChanged(const QString &NewText);
};

class PIQRadioButton : public QRadioButton
{
    Q_OBJECT

public:
    PIQRadioButton(const QString &text,QWidget *parent=nullptr) : QRadioButton(text,parent) {}

    void (*EventCB)(const struct PIRBEvent *Event,void *UserData);
    void *UserData;

    struct PI_RadioBttn *ThisBttn;

public slots:
    void ButtonClicked(bool checked);
};

class PIQCheckbox : public QCheckBox
{
    Q_OBJECT

public:
    PIQCheckbox(const QString &text,QWidget *parent=nullptr) : QCheckBox(text,parent) {}

    void (*EventCB)(const struct PICheckboxEvent *Event,void *UserData);
    void *UserData;

    struct PI_Checkbox *ThisBttn;

public slots:
    void ButtonClicked(bool checked);
};

class PIQTextInput : public QLineEdit
{
    Q_OBJECT

public:
    PIQTextInput(QWidget *parent=0) : QLineEdit(parent) {}

    void (*EventCB)(const struct PICBEvent *Event,void *UserData);
    void *UserData;

public slots:
    void TextInputTextChanged(const QString &NewText);
};

class PIQNumberInput : public QSpinBox
{
    Q_OBJECT

public:
    PIQNumberInput(QWidget *parent=0) : QSpinBox(parent) {}

    void (*EventCB)(const struct PICBEvent *Event,void *UserData);
    void *UserData;

public slots:
    void NumberInputChanged(int i);
};

class PIQDoubleInput : public QDoubleSpinBox
{
    Q_OBJECT

public:
    PIQDoubleInput(QWidget *parent=0) : QDoubleSpinBox(parent) {}

    void (*EventCB)(const struct PICBEvent *Event,void *UserData);
    void *UserData;

public slots:
    void DoubleInputChanged(double i);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
