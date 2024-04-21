/*******************************************************************************
 * FILENAME: Clipboard.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This has the handlers for the clip board.
 *
 * COPYRIGHT:
 *    Copyright 2020 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (10 Jul 2020)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "UI/UIClipboard.h"
//#include <QGuiApplication>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    UI_SetClipboardText
 *
 * SYNOPSIS:
 *    void UI_SetClipboardText(std::string Text,e_ClipboardType Clip);
 *
 * PARAMETERS:
 *    Text [I] -- The string to put in the clipboard
 *    Clip [I] -- What clipboard to set
 *
 * FUNCTION:
 *    This function copies a string (utf8) to the clip board.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UI_SetClipboardTextCStr(), UI_GetClipboardText()
 ******************************************************************************/
void UI_SetClipboardText(std::string Text,e_ClipboardType Clip)
{
    UI_SetClipboardTextCStr(Text.c_str(),Clip);
}

/*******************************************************************************
 * NAME:
 *    UI_SetClipboardTextCStr
 *
 * SYNOPSIS:
 *    void UI_SetClipboardTextCStr(const char *Text,e_ClipboardType Clip);
 *
 * PARAMETERS:
 *    Text [I] -- The c string to put in the clipboard
 *    Clip [I] -- What clipboard to set
 *
 * FUNCTION:
 *    This function copies a c string (utf8) to the clip board.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    UI_SetClipboardText(), UI_GetClipboardText()
 ******************************************************************************/
void UI_SetClipboardTextCStr(const char *Text,e_ClipboardType Clip)
{
    QString QStr;
    QClipboard::Mode UseClipBoard;

    try
    {
        QStr=QString::fromUtf8(Text);

        switch(Clip)
        {
            case e_Clipboard_Selection:
                UseClipBoard=QClipboard::Selection;
            break;
            case e_Clipboard_Clipboard:
                UseClipBoard=QClipboard::Clipboard;
            break;
            case e_ClipboardMAX:
            default:
            break;
        }
        if(Clip>=e_ClipboardMAX)
            return;

        QApplication::clipboard()->clear(UseClipBoard);

        /* Basic text */
        QApplication::clipboard()->setText(QStr,UseClipBoard);
    }
    catch(...)
    {
    }
}

/*******************************************************************************
 * NAME:
 *    UI_GetClipboardText
 *
 * SYNOPSIS:
 *    void UI_GetClipboardText(std::string &Text,e_ClipboardType Clip);
 *
 * PARAMETERS:
 *    Text [O] -- The clipboard will be copied here.
 *    Clip [I] -- What clipboard to grab from
 *
 * FUNCTION:
 *    This function gets the text from the clipboard and places it in the
 *    'Text' string.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UI_GetClipboardText(std::string &Text,e_ClipboardType Clip)
{
    QClipboard::Mode UseClipBoard;

    switch(Clip)
    {
        case e_Clipboard_Selection:
            UseClipBoard=QClipboard::Selection;
        break;
        case e_Clipboard_Clipboard:
            UseClipBoard=QClipboard::Clipboard;
        break;
        case e_ClipboardMAX:
        default:
        break;
    }
    if(Clip>=e_ClipboardMAX)
    {
        Text.clear();
        return;
    }

    Text=qPrintable(QApplication::clipboard()->text(UseClipBoard));
}
