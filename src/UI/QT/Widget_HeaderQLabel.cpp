/*******************************************************************************
 * FILENAME: Widget_HeaderQLabel.cpp
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
 * CREATED BY:
 *    Paul Hutchinson (27 Dec 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Widget_HeaderQLabel.h"
#include <QDrag>
#include <QMouseEvent>
#include <QMimeData>
#include <QApplication>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

HeaderQLabel::HeaderQLabel(QWidget *p) : QLabel(p)
{
    WHQEventHandler=NULL;
    WHQEventHandlerUserData=0;

    ParentSplitter=NULL;
    setAutoFillBackground(false);
    setStyleSheet(QString::fromUtf8("background-color:lightgray"));
    setAcceptDrops(true);
}

void HeaderQLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        dragStartPosition = event->pos();
}

void HeaderQLabel::mouseMoveEvent(QMouseEvent *event)
{
    QDrag *drag;
    QMimeData *mimeData;
    
    if (!(event->buttons() & Qt::LeftButton))
        return;
    if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return;

    drag = new QDrag(this);
    mimeData = new QMimeData;
    mimeData->setData("application/vnd.whippyterm.columndrag",this->objectName().toUtf8());
    drag->setMimeData(mimeData);

//    Qt::DropAction dropAction = drag->exec(Qt::MoveAction);
    drag->exec(Qt::MoveAction);
}

void HeaderQLabel::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/vnd.whippyterm.columndrag"))
        event->acceptProposedAction();
}

void HeaderQLabel::dropEvent(QDropEvent *event)
{
//    int parentindex;
    int destindex;

    if (event->source() == this)
        return;
    if (event->proposedAction() != Qt::MoveAction)
    {
        return;
    }
    if(ParentSplitter==NULL)
        return;

    event->acceptProposedAction();

    /* Reorder the columns */
//    parentindex=ParentSplitter->indexOf((QWidget *)event->source()->parent()->parent());
    destindex=ParentSplitter->indexOf((QWidget *)this->parent()->parent());

    ParentSplitter->insertWidget(destindex,(QWidget *)event->source()->parent()->parent());

    SendEvent(e_WHQEvent_HeaderMoved);
}

void HeaderQLabel::SetEventHandler(bool (*EventHandler)(const struct WHQEvent *Event),uintptr_t UserData)
{
    WHQEventHandler=EventHandler;
    WHQEventHandlerUserData=UserData;
}

/*******************************************************************************
 * NAME:
 *    HeaderQLabel::SendEvent()
 *
 * SYNOPSIS:
 *    bool HeaderQLabel::SendEvent(e_WHQEventType EventType,
 *          union WHQEventData *Info);
 *
 * PARAMETERS:
 *    EventType [I] -- The event to send
 *    Info [I] -- Extra info to send with this event.  Can be NULL for none.
 *
 * FUNCTION:
 *    This function sends a text canvas event out to it's registered event
 *    handler.
 *
 * RETURNS:
 *    true -- Accept this event
 *    false -- Cancel this event
 *
 * NOTES:
 *    The return value is normally ignored as most events have no other action.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool HeaderQLabel::SendEvent(e_WHQEventType EventType,union WHQEventData *Info)
{
    struct WHQEvent NewEvent;

    NewEvent.EventType=EventType;
    NewEvent.UserData=WHQEventHandlerUserData;
    NewEvent.Info=Info;
    NewEvent.Source=this;

    if(WHQEventHandler!=NULL)
        return WHQEventHandler(&NewEvent);
    return true;
}

bool HeaderQLabel::SendEvent(e_WHQEventType EventType)
{
    return SendEvent(EventType,nullptr);
}
