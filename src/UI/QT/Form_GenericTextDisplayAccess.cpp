/*******************************************************************************
 * FILENAME: Form_GenericTextDisplayAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (13 Sep 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_GenericTextDisplay.h"
#include "ui_Form_GenericTextDisplay.h"
#include "UI/UIGenericTextDisplay.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_GenericTextDisplay *g_GTD_GenericTextDisplay;

/*******************************************************************************
 * NAME:
 *    UIAlloc_GenericTextDisplay
 *
 * SYNOPSIS:
 *    bool UIAlloc_GTD_GenericTextDisplay(const char *Title);
 *
 * PARAMETERS:
 *    Title [I] -- The title for the dialog.
 *
 * FUNCTION:
 *    This function allocates a generic text display form dialog.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was a problem allocating the dialog.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIAlloc_GTD_GenericTextDisplay(const char *Title)
{
    try
    {
        g_GTD_GenericTextDisplay=new Form_GenericTextDisplay(g_MainApp->activeWindow());
        g_GTD_GenericTextDisplay->setWindowTitle(Title);
    }
    catch(...)
    {
        g_GTD_GenericTextDisplay=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_GTD_GenericTextDisplay
 *
 * SYNOPSIS:
 *    bool UIShow_GTD_GenericTextDisplay(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function runs the generic text display form dialog.
 *
 * RETURNS:
 *    true -- User pressed ok
 *    false -- User pressed cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIShow_GTD_GenericTextDisplay(void)
{
    return g_GTD_GenericTextDisplay->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_GTD_GenericTextDisplay
 *
 * SYNOPSIS:
 *    void UIFree_GTD_GenericTextDisplay(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the dialog allocated with UIAlloc_GTD_GenericTextDisplay()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_GTD_GenericTextDisplay(void)
{
    delete g_GTD_GenericTextDisplay;

    g_GTD_GenericTextDisplay=NULL;
}

/*******************************************************************************
 * NAME:
 *    UIGTD_ClearCtrlText
 *
 * SYNOPSIS:
 *    void UIGTD_ClearCtrlText(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clears the text control.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIGTD_ClearCtrlText(void)
{
    QTextEdit *textinput=g_GTD_GenericTextDisplay->ui->textEdit;

    try
    {
        g_GTD_GenericTextDisplay->RawHTML="";
        textinput->blockSignals(true);
        textinput->setHtml("");
    }
    catch(...)
    {
    }
    textinput->blockSignals(false);
}

/*******************************************************************************
 * NAME:
 *    UIGTD_AppendText
 *
 * SYNOPSIS:
 *    void UIGTD_AppendText(const char *NewLine,e_GTD_StyleType Style);
 *
 * PARAMETERS:
 *    NewLine [I] -- The new line to add.  Newline chars are ignored.
 *    Style [I] -- How to render this line
 *
 * FUNCTION:
 *    This function adds a line (or a few if that's how the line gets rendered)
 *    to the text display ctrl.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIGTD_AppendText(const char *NewLine,e_GTD_StyleType Style)
{
    QTextEdit *textinput=g_GTD_GenericTextDisplay->ui->textEdit;
    QString HTMLText;
    QString EscHTML;

    try
    {
        EscHTML=NewLine;
        EscHTML=EscHTML.toHtmlEscaped();

        HTMLText=g_GTD_GenericTextDisplay->RawHTML;

        switch(Style)
        {
            case e_GTD_Style_Heading:
                HTMLText+="<h3>";
                HTMLText+=EscHTML;
                HTMLText+="</h3>";
            break;
            case e_GTD_Style_Def:
                HTMLText+="<p style='margin-left:60px;text-indent: -30px;'>";
                HTMLText+=EscHTML;
                HTMLText+="</p>";
            break;
            case e_GTD_Style_DefLevel2:
                HTMLText+="<p style='margin-left:120px;text-indent: -30px;'>";
                HTMLText+=EscHTML;
                HTMLText+="</p>";
            break;
            case e_GTD_Style_TextLine:
                HTMLText+=EscHTML;
                HTMLText+="<br/>";
            break;
            case e_GTD_StyleTypeMAX:
            default:
                throw(0);
            break;
        }

        g_GTD_GenericTextDisplay->RawHTML=HTMLText;

        textinput->blockSignals(true);
        textinput->setHtml(HTMLText);
    }
    catch(...)
    {
    }
    textinput->blockSignals(false);
}

