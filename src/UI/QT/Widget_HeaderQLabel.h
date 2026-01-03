/*******************************************************************************
 * FILENAME: Widget_HeaderQLabel.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 27 Dec 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (27 Dec 2025)
 *       Created
 *
 *******************************************************************************/
#ifndef __WIDGET_HEADERQLABEL_H_
#define __WIDGET_HEADERQLABEL_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <QLabel>
#include <QSplitter>
#include <QMouseEvent>
#include <stdint.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef enum
{
    e_WHQEvent_HeaderMoved, // The index of a header has changed (maybe more than one, but only one event sent)
    e_WHQEventMAX
} e_WHQEventType;

union WHQEventData
{
    int junk;
};

struct WHQEvent
{
    e_WHQEventType EventType;
    uintptr_t UserData;
    union WHQEventData *Info;
    class HeaderQLabel *Source;
};

/***  CLASS DEFINITIONS                ***/
class HeaderQLabel : public QLabel
{
public:
    HeaderQLabel(QWidget *p);
    ~HeaderQLabel() {}
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    QSplitter *ParentSplitter;
    void SetEventHandler(bool (*EventHandler)(const struct WHQEvent *Event),uintptr_t UserData);

private:
    bool SendEvent(e_WHQEventType EventType,union WHQEventData *Info);
    bool SendEvent(e_WHQEventType EventType);

    QPoint dragStartPosition;
    bool (*WHQEventHandler)(const struct WHQEvent *Event);
    uintptr_t WHQEventHandlerUserData;
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif   /* end of "#ifndef __WIDGET_HEADERQLABEL_H_" */
