/*******************************************************************************
 * FILENAME:
 *    HozPanelHandle.cpp
 *
 * PROJECT:
 *    
 *
 * FILE DESCRIPTION:
 *    
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
#include "HozPanelHandle.h"
#include "Form_MainWindow.h"
#include "UI/UISystem.h"

/*** DEFINES                  ***/
#define BAR_MARGIN_TB    3
#define BAR_CENTER_MARGIN  11

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

HozPanelHandle::HozPanelHandle(QWidget *parent) : QFrame(parent)
{
    MainWindow=NULL;
    setMouseTracking(true);
    DrawPointingUp=false;
    MouseIn=false;
}

void HozPanelHandle::ChangeArrowDir(bool Up)
{
    DrawPointingUp=Up;
    update();
}

void HozPanelHandle::enterEvent(QEnterEvent *event)
{
    MouseIn=true;
    update();
}

void HozPanelHandle::leaveEvent(QEvent *event)
{
    MouseIn=false;
    update();
}

void HozPanelHandle::ClearHighlight(void)
{
    MouseIn=false;
    update();
}

void HozPanelHandle::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->position().y()>=0 && event->position().y()<=this->height() &&
            event->position().x()>=0 && event->position().x()<=this->width())
    {
        if(MainWindow!=NULL)
            MainWindow->PanelClicked(e_Panel_Bottom);
    }
}

void HozPanelHandle::RegisterClickHandler(Form_MainWindow *MW)
{
    MainWindow=MW;
}

void HozPanelHandle::paintEvent(QPaintEvent *Event)
{
    QPainter painter(this);
    QBrush brush;
    QImage Background(":/G/Graphics/PanelHandleBackground.png");
    QImage Up(":/G/Graphics/Arrow_Up.png");
    QImage Down(":/G/Graphics/Arrow_Down.png");
    QImage BackgroundDark(":/G/Graphics/PanelHandleBackground_Dark.png");
    QImage UpDark(":/G/Graphics/Arrow_Up_Dark.png");
    QImage DownDark(":/G/Graphics/Arrow_Down_Dark.png");
    QImage *Arrow;
    QImage *UseBackground;
    int BarWidth;
    int BarHeight;
    int x,y,x2,y2;
    int ArrowWidth;
    qreal InverseDPR;

    if(OS_IsSystemInDarkMode())
    {
        UseBackground=&BackgroundDark;
        if(DrawPointingUp)
            Arrow=&UpDark;
        else
            Arrow=&DownDark;
    }
    else
    {
        UseBackground=&Background;
        if(DrawPointingUp)
            Arrow=&Up;
        else
            Arrow=&Down;
    }

    brush=painter.brush();

    InverseDPR=1.0/this->devicePixelRatio();

    BarWidth=(this->width()-1)*this->devicePixelRatio();
    BarHeight=(this->height()-1)*this->devicePixelRatio();
    ArrowWidth=Arrow->width();

    painter.scale(InverseDPR,InverseDPR);

    /* Fill the whole background */
    if(MouseIn)
        brush.setColor(QColor(0x96,0xC7,0xE6));
    else
        brush.setColor(this->palette().color(QPalette::Window));
    brush.setStyle(Qt::SolidPattern);
    painter.fillRect(0,0,BarWidth,BarHeight,brush);

    /* Draw the border */
    painter.setPen(this->palette().color(QPalette::Light));
    painter.drawLine(0,0,BarWidth,0);
    painter.drawLine(0,0,0,BarHeight);

    painter.setPen(this->palette().color(QPalette::Dark));
    painter.drawLine(BarWidth,0,BarWidth,BarHeight);
    painter.drawLine(0,BarHeight,BarWidth,BarHeight);

    /* Fill in the dots */
    brush.setTextureImage(*UseBackground);
    brush.setStyle(Qt::TexturePattern);
    brush.setTextureImage(*UseBackground);

    /* Left side */
    x=5+ArrowWidth+5+1;
    y=BAR_MARGIN_TB;
    x2=BarWidth/2-BAR_CENTER_MARGIN-ArrowWidth/2;
    y2=BarHeight-BAR_MARGIN_TB;
    painter.fillRect(x,y,x2-x,y2-y,brush);
    /* Right side */
    x=BarWidth/2+BAR_CENTER_MARGIN+ArrowWidth/2;
    y=BAR_MARGIN_TB;
    x2=BarWidth-(5+ArrowWidth+5+1);
    y2=BarHeight-BAR_MARGIN_TB;
    painter.fillRect(x,y,x2-x,y2-y,brush);

    /* Draw the arrows */
    painter.drawImage(6,BAR_MARGIN_TB,*Arrow);
    painter.drawImage(BarWidth/2-ArrowWidth/2,BAR_MARGIN_TB,*Arrow);
    painter.drawImage(BarWidth-4-ArrowWidth,BAR_MARGIN_TB,*Arrow);

    /* Draw the dividers */
    painter.setPen(this->palette().color(QPalette::Dark));
    painter.drawLine(5+ArrowWidth+5,BAR_MARGIN_TB,
            5+ArrowWidth+5,BarHeight-BAR_MARGIN_TB);
    painter.drawLine(BarWidth-(5+ArrowWidth+5),BAR_MARGIN_TB,
            BarWidth-(5+ArrowWidth+5),BarHeight-BAR_MARGIN_TB);
}

