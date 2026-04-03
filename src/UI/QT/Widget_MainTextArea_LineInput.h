#ifndef __WIDGET_MAINTEXTAREA_LINEINPUT_H_
#define __WIDGET_MAINTEXTAREA_LINEINPUT_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <QComboBox>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
class Widget_MainTextArea_LineInput : public QComboBox
{
    Q_OBJECT

public:
    Widget_MainTextArea_LineInput(QWidget *p);
    ~Widget_MainTextArea_LineInput() {}
    void keyPressEvent(QKeyEvent *event);

signals:
    void upPressed();
    void downPressed();
};

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif   /* end of "#ifndef __WIDGET_MAINTEXTAREA_LINEINPUT_H_" */
