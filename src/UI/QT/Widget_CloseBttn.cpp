/*******************************************************************************
 * FILENAME: Widget_CloseBttn.cpp
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
#include "Widget_CloseBttn.h"
#include <QtGui>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

MyCloseBttn::MyCloseBttn(QWidget *parent) : QWidget(parent)
{
    MouseIn=false;
    MouseClicked=false;
}

void MyCloseBttn::enterEvent(QEvent *event)
{
    MouseIn=true;
    QWidget::enterEvent(event);
    update();
}

void MyCloseBttn::leaveEvent(QEvent *event)
{
    MouseIn=false;
    QWidget::leaveEvent(event);
    update();
}

void MyCloseBttn::mouseMoveEvent(QMouseEvent * event)
{
    if(event->y()>=0 && event->y()<=this->height() &&
            event->x()>=0 && event->x()<=this->width())
    {
        if(!MouseIn)
        {
            MouseIn=true;
            update();
        }
    }
    else
    {
        if(MouseIn)
        {
            MouseIn=false;
            update();
        }
    }
}

void MyCloseBttn::mouseReleaseEvent(QMouseEvent *event)
{
    MouseClicked=false;
    update();
    if(event->y()>=0 && event->y()<=this->height() &&
            event->x()>=0 && event->x()<=this->width())
    {
        emit Clicked();
    }
    QWidget::mouseReleaseEvent(event);
}

void MyCloseBttn::mousePressEvent(QMouseEvent *event)
{
    if(event->y()>=0 && event->y()<=this->height() &&
            event->x()>=0 && event->x()<=this->width())
    {
        MouseClicked=true;
        update();
    }
    QWidget::mousePressEvent(event);
}

void MyCloseBttn::paintEvent(QPaintEvent *Event)
{
    QPainter painter(this);
    QImage Normal(":/G/Graphics/CloseBttn_Normal.png");
    QImage Hover(":/G/Graphics/CloseBttn_Hover.png");
    QImage Clicked(":/G/Graphics/CloseBttn_Clicked.png");
    QImage Disabled(":/G/Graphics/CloseBttn_Disabled.png");

    if(isEnabled())
    {
        if(MouseClicked && MouseIn)
            painter.drawImage(2,0,Clicked);
        else if(MouseIn)
            painter.drawImage(2,0,Hover);
        else
            painter.drawImage(2,0,Normal);
    }
    else
    {
        painter.drawImage(2,0,Disabled);
    }
}

QSize MyCloseBttn::sizeHint() const
{
    return QSize(CLOSEBTTN_WIDTH,CLOSEBTTN_HEIGHT);
}
