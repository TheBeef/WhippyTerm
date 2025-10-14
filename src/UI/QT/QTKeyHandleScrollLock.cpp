/*******************************************************************************
 * FILENAME: QTKeyHandleScrollLock.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file handles the scroll lock key.  This is needed because Linux
 *    handling of the scroll lock key is.... poor.  The windows (and mac?)
 *    versions of this file are simple.
 *
 * COPYRIGHT:
 *    Copyright 11 Oct 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (11 Oct 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include <QGuiApplication>
#include "QTKeyHandleScrollLock.h"
#include "UI/UIKeyboard.h"
#include "UI/UIMainWindow.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
#if defined(Q_OS_WIN)
#include <windows.h>

void QTInitScrollLockHandler(void)
{
}

void EnableHandlingOfScrollLock(bool Enabled)
{
}

bool IsScrollLockOn(void)
{
    return GetKeyState(VK_SCROLL) == 1;
}

void QTInformOfScrollLockPressed(void)
{
    struct MWEvent Event;

    Event.EventType=e_MWEvent_ScrollLockHint;
    Event.MW=NULL;  // All
    MW_Event(&Event);
}

void QTScrollLockHelperTick(void)
{
    /* Does nothing because Windows (of all things) works correctly for scroll
       lock */
}

#elif defined(Q_OS_MACOS)

// Just a place holder to help figure out how to do this on the mac
newCapslockState = ((CGEventSourceFlagsState(kCGEventSourceStateHIDSystemState) & kCGEventFlagMaskAlphaShift) != 0);

void QTScrollLockHelperTick(void)
{
}

#elif defined(Q_OS_UNIX)

#include <X11/Xlib.h>
#include <X11/XKBlib.h>

typedef enum
{
    e_LEDCtrlSystem_Unknown,
    e_LEDCtrlSystem_X11,
    e_LEDCtrlSystem_Wayland,
    e_LEDCtrlSystemMAX
} e_LEDCtrlSystemType;

static void SetScrollLockLED(bool On);

bool m_RunTheScrollLockKey=false;
bool m_ScrollLockOn=false;
static e_LEDCtrlSystemType m_LEDCtrlSystem;
static Display *m_X11_dpy;

void QTInitScrollLockHandler(void)
{
    QString platform;

    /* All we do is detect what system we are using */
    platform=QGuiApplication::platformName();

    if(platform=="xcb")
    {
        m_LEDCtrlSystem=e_LEDCtrlSystem_X11;
        m_X11_dpy=XOpenDisplay(NULL);
    }
    else if(platform == "wayland")
    {
        m_LEDCtrlSystem=e_LEDCtrlSystem_Wayland;
    }
    else
    {
        m_LEDCtrlSystem=e_LEDCtrlSystem_Unknown;
    }
}

void EnableHandlingOfScrollLock(bool Enabled)
{
    m_RunTheScrollLockKey=Enabled;
    if(IsScrollLockOn())
        SetScrollLockLED(m_ScrollLockOn);
}

bool IsScrollLockOn(void)
{
    unsigned state;

    switch(m_LEDCtrlSystem)
    {
        case e_LEDCtrlSystem_Unknown:
        case e_LEDCtrlSystemMAX:
        case e_LEDCtrlSystem_Wayland:
        default:
            return m_ScrollLockOn;
        case e_LEDCtrlSystem_X11:
            if(m_X11_dpy!=NULL)
            {
                if(XkbGetIndicatorState(m_X11_dpy,XkbUseCoreKbd,&state)==Success)
                {
                    m_ScrollLockOn=state&0x04;
                }
            }
            return m_ScrollLockOn;
        break;
    }
}

void QTInformOfScrollLockPressed(void)
{
    /* Simple part, just toggle our tracking */
    m_ScrollLockOn=!IsScrollLockOn();

    SetScrollLockLED(m_ScrollLockOn);
}

void SetScrollLockLED(bool On)
{
    XKeyboardControl values;

    switch(m_LEDCtrlSystem)
    {
        case e_LEDCtrlSystem_Unknown:
        case e_LEDCtrlSystemMAX:
        case e_LEDCtrlSystem_Wayland:
        default:
            /* Nothing we can do for these, so we do... nothing */
        break;
        case e_LEDCtrlSystem_X11:
            if(m_X11_dpy!=NULL)
            {
                memset(&values,0,sizeof(values));
                values.led_mode=On;
                values.led = 3;

                XChangeKeyboardControl(m_X11_dpy, KBLed | KBLedMode, &values);
            }
        break;
    }
}

void QTScrollLockHelperTick(void)
{
    static bool ScrollLockOn=false;
    struct MWEvent Event;

    if(m_RunTheScrollLockKey)
    {
        /* Ok, because we don't always get the scroll lock key press, we need
           to scan for a change in the scroll lock LED and send an event if
           there is one.  This only applies to Linux */
        if(ScrollLockOn!=IsScrollLockOn())
        {
            ScrollLockOn=IsScrollLockOn();
            Event.EventType=e_MWEvent_ScrollLockHint;
            Event.MW=NULL;  // All
            MW_Event(&Event);
        }
    }
}

#endif
