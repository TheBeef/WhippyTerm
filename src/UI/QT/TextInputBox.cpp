/*******************************************************************************
 * FILENAME: TextInputBox.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * CREATED BY:
 *    Paul Hutchinson (19 Nov 2020)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "TextInputBox.h"
#include <QInputDialog>
#include <QLineEdit>
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    UITextInputBox
 *
 * SYNOPSIS:
 *    bool UITextInputBox(const std::string &Title,const std::string &Msg,
 *              std::string &RetStr);
 *
 * PARAMETERS:
 *    Title [I] -- The title for the window
 *    Msg [I] -- The message to place above the input
 *    RetStr [I/O] -- The default text and where the text input is put.
 *
 * FUNCTION:
 *    This function opens a text input window and asks the user to input
 *    a string.
 *
 * RETURNS:
 *    true -- User clicks ok
 *    false -- User cancels
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UITextInputBox(const std::string &Title,const std::string &Msg,
        std::string &RetStr)
{
    QWidget *ParentWindow;
    QString text;
    bool ok;
    QString QTitle;
    QString QMsg;
    QString QRetStr;

    QTitle=Title.c_str();
    QMsg=Msg.c_str();
    QRetStr=RetStr.c_str();

    ParentWindow=g_MainApp->activeWindow();

    text=QInputDialog::getText(ParentWindow,QTitle,QMsg,QLineEdit::Normal,
            QRetStr,&ok);

    if(!ok)
        return false;

    RetStr=qPrintable(text);

    return true;
}
