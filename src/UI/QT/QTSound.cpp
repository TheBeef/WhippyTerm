/*******************************************************************************
 * FILENAME: QTSound.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 08 Feb 2025 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (08 Feb 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "UI/UISound.h"
#include "QTSound.h"
#include <QApplication>
#include <QSoundEffect>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
static QSoundEffect *m_BuildInEffect=NULL;
static QSoundEffect *m_PlayWavEffect=NULL;

void UI_Beep(void)
{
    QApplication::beep();
}

void UIPlayWav(const char *Filename)
{
    try
    {
        if(m_PlayWavEffect!=NULL)
            delete m_PlayWavEffect;
        m_PlayWavEffect=NULL;

        m_PlayWavEffect=new QSoundEffect(NULL);
        m_PlayWavEffect->setSource(QUrl::fromLocalFile(Filename));
        m_PlayWavEffect->play();
    }
    catch(...)
    {
    }
}

void UIPlayBuiltInBeep(void)
{

    try
    {
        if(m_BuildInEffect!=NULL)
            delete m_BuildInEffect;
        m_BuildInEffect=NULL;

        m_BuildInEffect=new QSoundEffect(NULL);
        m_BuildInEffect->setSource(QUrl("qrc:/S/Sounds/Computer_Error_Alert_Short.wav"));
        m_BuildInEffect->play();
    }
    catch(...)
    {
    }
}

void FreeQTSoundSystem(void)
{
    if(m_PlayWavEffect!=NULL)
        delete m_PlayWavEffect;
    m_PlayWavEffect=NULL;
    if(m_BuildInEffect!=NULL)
        delete m_BuildInEffect;
    m_BuildInEffect=NULL;
}
