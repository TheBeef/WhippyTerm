/*******************************************************************************
 * FILENAME: QTSound.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    Has the sound play back system (mostly for playing bells) for the Linux
 *    OS.
 *
 * COPYRIGHT:
 *    Copyright 08 Feb 2025 Paul Hutchinson.
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

#include <QMediaPlayer>
#include <QAudioOutput>   // Qt 6 only

QMediaPlayer *m_BuildInPlayer  = nullptr;
QAudioOutput *m_BuildInAudioOut = nullptr;   // Qt 6 only

void UIPlayBuiltInBeep(void)
{
    if (m_BuildInPlayer == NULL)
    {
        m_BuildInPlayer   = new QMediaPlayer(qApp);
        m_BuildInAudioOut = new QAudioOutput(qApp);
        m_BuildInPlayer->setAudioOutput(m_BuildInAudioOut);
        m_BuildInAudioOut->setVolume(1.0);   // 0.0–1.0
        m_BuildInPlayer->setSource(
            QUrl("qrc:/S/Sounds/Computer_Error_Alert_Short.wav"));
    }
    m_BuildInPlayer->setPosition(0);   // rewind so each press starts fresh
    m_BuildInPlayer->play();
}
//void UIPlayBuiltInBeep(void)
//{
//    try
//    {
//        if(m_BuildInEffect==NULL)
//        {
//            m_BuildInEffect=new QSoundEffect(NULL);
//            m_BuildInEffect->setSource(QUrl("qrc:/S/Sounds/Computer_Error_Alert_Short.wav"));
//        }
//
//        if(m_BuildInEffect->isPlaying())
//        {
//            m_BuildInEffect->stop();
//        }
//        else
//        {
//            m_BuildInEffect->play();
//        }
//    }
//    catch(...)
//    {
//    }
//}

void FreeQTSoundSystem(void)
{
    if(m_PlayWavEffect!=NULL)
        delete m_PlayWavEffect;
    m_PlayWavEffect=NULL;
    if(m_BuildInEffect!=NULL)
        delete m_BuildInEffect;
    m_BuildInEffect=NULL;
}
