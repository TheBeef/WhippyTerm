/*******************************************************************************
 * FILENAME:
 *    VerPanelHandle.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    .h file
 *
 * COPYRIGHT:
 *    Copyright 2018 Paul Hutchinson.
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
 *    Paul Hutchinson (27 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include <QtGui>
#include "VerPanelHandle.h"
#include "Form_MainWindow.h"

/*** DEFINES                  ***/
#define BAR_MARGIN_TB    3
#define BAR_CENTER_MARGIN  11

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

VerPanelHandle::VerPanelHandle(QWidget *parent) : QFrame(parent)
{
    MainWindow=NULL;
    setMouseTracking(true);
    DrawPointingLeft=false;
    MouseIn=false;
    LeftPanel=false;
}

void VerPanelHandle::ChangeArrowDir(bool Left)
{
    DrawPointingLeft=Left;
    update();
}

void VerPanelHandle::enterEvent(QEnterEvent *event)
{
    MouseIn=true;
    update();
}

void VerPanelHandle::leaveEvent(QEvent *event)
{
    MouseIn=false;
    update();
}

void VerPanelHandle::ClearHighlight(void)
{
    MouseIn=false;
    update();
}

void VerPanelHandle::LeftMode(void)
{
    LeftPanel=true;
}

void VerPanelHandle::mouseReleaseEvent(QMouseEvent *event)
{
    e_PanelType PanelType;
    
    if(event->position().y()>=0 && event->position().y()<=this->height() &&
            event->position().x()>=0 && event->position().x()<=this->width())
    {
        PanelType=e_Panel_Right;
        if(LeftPanel)
            PanelType=e_Panel_Left;

        if(MainWindow!=NULL)
            MainWindow->PanelClicked(PanelType);
    }
}

void VerPanelHandle::RegisterClickHandler(Form_MainWindow *MW)
{
    MainWindow=MW;
}

void VerPanelHandle::paintEvent(QPaintEvent *Event)
{
    QPainter painter(this);
    QBrush brush;
    QImage Background(":/G/Graphics/PanelHandleBackground.png");
    QImage Left(":/G/Graphics/Arrow_Left.png");
    QImage Right(":/G/Graphics/Arrow_Right.png");
    QImage *Arrow;
    int BarWidth;
    int BarHeight;
    int x,y,x2,y2;
    int ArrowHeight;
    qreal InverseDPR;

    if(DrawPointingLeft)
        Arrow=&Left;
    else
        Arrow=&Right;

    brush=painter.brush();

    InverseDPR=1.0/this->devicePixelRatio();

    BarWidth=(this->width()-1)*this->devicePixelRatio();
    BarHeight=(this->height()-1)*this->devicePixelRatio();
    ArrowHeight=Arrow->height();

    painter.scale(InverseDPR,InverseDPR);

    /* Fill the whole background */
    if(MouseIn)
        brush.setColor(QColor(0x96,0xC7,0xE6));
    else
        brush.setColor(this->palette().color(QPalette::Window));
    brush.setStyle(Qt::SolidPattern);
    painter.fillRect(0,0,BarWidth,BarHeight,brush);

    /* Draw the border */
    painter.setPen(QColor(255,255,255));
    painter.drawLine(0,0,BarWidth,0);
    painter.drawLine(0,0,0,BarHeight);

    painter.setPen(QColor(128,128,128));
    painter.drawLine(BarWidth,0,BarWidth,BarHeight);
    painter.drawLine(0,BarHeight,BarWidth,BarHeight);

    /* Fill in the dots */
    brush.setTextureImage(Background);
    brush.setStyle(Qt::TexturePattern);
    brush.setTextureImage(Background);

    /* Top */
    x=BAR_MARGIN_TB;
    y=5+ArrowHeight+5+1;
    x2=BarWidth-BAR_MARGIN_TB;
    y2=BarHeight/2-BAR_CENTER_MARGIN-ArrowHeight/2;
    painter.fillRect(x,y,x2-x,y2-y,brush);

    /* Bottom */
    x=BAR_MARGIN_TB;
    y=BarHeight/2+BAR_CENTER_MARGIN+ArrowHeight/2;
    x2=BarWidth-BAR_MARGIN_TB;
    y2=BarHeight-(5+ArrowHeight+5+1);
    painter.fillRect(x,y,x2-x,y2-y,brush);

    /* Draw the arrows */
    painter.drawImage(BAR_MARGIN_TB,6,*Arrow);
    painter.drawImage(BAR_MARGIN_TB,BarHeight/2-ArrowHeight/2,*Arrow);
    painter.drawImage(BAR_MARGIN_TB,BarHeight-5-ArrowHeight,*Arrow);

    /* Draw the dividers */
    painter.setPen(QColor(0x90,0x90,0x90));
    painter.drawLine(BAR_MARGIN_TB,5+ArrowHeight+5,
            BarWidth-BAR_MARGIN_TB,5+ArrowHeight+5);
    painter.drawLine(BAR_MARGIN_TB,BarHeight-(5+ArrowHeight+5),
            BarWidth-BAR_MARGIN_TB,BarHeight-(5+ArrowHeight+5));
}

