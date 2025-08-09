/*******************************************************************************
 * FILENAME: Widget_CloseBttn.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __WIDGET_CLOSEBTTN_H_
#define __WIDGET_CLOSEBTTN_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <QWidget>

/***  DEFINES                          ***/
#define CLOSEBTTN_WIDTH     18
#define CLOSEBTTN_HEIGHT    16

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/
class MyCloseBttn : public QWidget
{
    Q_OBJECT
    
public:
    MyCloseBttn(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *);

private:
    void enterEvent(QEnterEvent *event);
    void leaveEvent(QEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    bool MouseIn;
    bool MouseClicked;
    QSize sizeHint() const;

signals:
    void Clicked(void);
};

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
