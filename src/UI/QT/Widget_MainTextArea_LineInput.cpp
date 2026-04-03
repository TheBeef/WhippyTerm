/*** HEADER FILES TO INCLUDE  ***/
#include "Widget_MainTextArea_LineInput.h"
#include <QKeyEvent>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

Widget_MainTextArea_LineInput::Widget_MainTextArea_LineInput(QWidget *p) : QComboBox(p)
{
}

void Widget_MainTextArea_LineInput::keyPressEvent(QKeyEvent *event)
{
    if(event->key()==Qt::Key_Up)
    {
        emit upPressed();
    }
    else if(event->key()==Qt::Key_Down)
    {
        emit downPressed();
    }
    else
    {
        QComboBox::keyPressEvent(event);
    }
}

