/*******************************************************************************
 * FILENAME: Form_NewConnectionFromURIAccess.cpp
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
 *
 * CREATED BY:
 *    Paul Hutchinson (10 Feb 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_NewConnectionFromURI.h"
#include "ui_Form_NewConnectionFromURI.h"
#include "Form_MainWindow.h"
#include "UI/UINewConnectionFromURI.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_NewConnectionFromURI *g_NewConnectionFromURIDialog;

t_UITextInputCtrl *UINC_GetTxtInputHandle(e_UINCFU_TxtInput UIObj)
{
    switch(UIObj)
    {
        case e_UINCFU_TxtInput_URI:
            return (t_UITextInputCtrl *)g_NewConnectionFromURIDialog->ui->URI_lineEdit;
        break;
        default:
        case e_UINCFU_TxtInputMAX:
        break;
    }
    return NULL;
}

t_UIListViewCtrl *UINC_GetListViewInputHandle(e_UINCFU_ListViewType UIObj)
{
    switch(UIObj)
    {
        case e_UINCFU_ListView_Driver:
            return (t_UIListViewCtrl *)g_NewConnectionFromURIDialog->ui->DriverList_listWidget;
        break;
        default:
        case e_UINCFU_ListViewMAX:
        break;
    }
    return NULL;
}

t_UIHTMLViewCtrl *UINC_GetHTMLViewInputHandle(e_UINCFU_HTMLViewType UIObj)
{
    switch(UIObj)
    {
        case e_UINCFU_HTMLView_Info:
            return (t_UIHTMLViewCtrl *)g_NewConnectionFromURIDialog->ui->Details_textEdit;
        break;
        default:
        case e_UINCFU_HTMLViewMAX:
        break;
    }
    return NULL;
}

t_UIButtonCtrl *UINC_GetButtonInputHandle(e_UINCFU_ButtonInput UIObj)
{
    switch(UIObj)
    {
        case e_UINCFU_ButtonInput_Ok:
            return (t_UIButtonCtrl *)g_NewConnectionFromURIDialog->ui->buttonBox->button(QDialogButtonBox::Ok);
        break;
        default:
        case e_UINCFU_ButtonInputMAX:
        break;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UIAlloc_NewConnectionFromURI
 *
 * SYNOPSIS:
 *    bool UIAlloc_NewConnectionFromURI(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates a new connection from URI dialog.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (10 Feb 2025)
 *       Created
 ******************************************************************************/
bool UIAlloc_NewConnectionFromURI(void)
{
    try
    {
        g_NewConnectionFromURIDialog=new Form_NewConnectionFromURI(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_NewConnectionFromURIDialog=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_NewConnectionFromURI
 *
 * SYNOPSIS:
 *    bool UIShow_NewConnectionFromURI(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function shows the new connection from URI dialog that was allocated.
 *    You can only have 1 new connection dialog active at a time.
 *
 *    This is a blocking call.
 *
 * RETURNS:
 *    true -- User pressed ok
 *    false -- User pressed cancel
 *
 * SEE ALSO:
 *    
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (10 Feb 2025)
 *       Created
 ******************************************************************************/
bool UIShow_NewConnectionFromURI(void)
{
    if(g_NewConnectionFromURIDialog->exec()==QDialog::Accepted)
    {
        return true;
    }

    return false;
}

/*******************************************************************************
 * NAME:
 *    UIFree_NewConnectionFromURI
 *
 * SYNOPSIS:
 *    void UIFree_NewConnectionFromURI(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the current new connection from URI dialog.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (10 Feb 2025)
 *       Created
 ******************************************************************************/
void UIFree_NewConnectionFromURI(void)
{
    delete g_NewConnectionFromURIDialog;

    g_NewConnectionFromURIDialog=NULL;
}

/*******************************************************************************
 * NAME:
 *    UINC_PrivURIHelp_ClearCtrlText
 *
 * SYNOPSIS:
 *    void UINC_PrivURIHelp_ClearCtrlText(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clears the URI help control.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UINC_PrivURIHelp_ClearCtrlText(void)
{
    QTextEdit *textinput=g_NewConnectionFromURIDialog->ui->Details_textEdit;

    try
    {
        g_NewConnectionFromURIDialog->PrivURIRawHTML="";
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
 *    UINC_PrivURIHelp_AppendText
 *
 * SYNOPSIS:
 *    void UINC_PrivURIHelp_AppendText(const char *NewLine,
 *              e_PrivURIHelp_StyleType Style);
 *
 * PARAMETERS:
 *    NewLine [I] -- The new line to add.  Newline chars are ignored.
 *    Style [I] -- How to render this line
 *
 * FUNCTION:
 *    This function adds a line (or a few if that's how the line gets rendered)
 *    to the URI help ctrl.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UINC_PrivURIHelp_AppendText(const char *NewLine,e_PrivURIHelp_StyleType Style)
{
    QTextEdit *textinput=g_NewConnectionFromURIDialog->ui->Details_textEdit;
    QString HTMLText;
    QString EscHTML;

    try
    {
        EscHTML=NewLine;
        EscHTML=EscHTML.toHtmlEscaped();

        HTMLText=g_NewConnectionFromURIDialog->PrivURIRawHTML;

        switch(Style)
        {
            case e_PrivURIHelp_Style_Heading:
                HTMLText+="<h3>";
                HTMLText+=EscHTML;
                HTMLText+="</h3>";
            break;
            case e_PrivURIHelp_Style_Def:
                HTMLText+="<p style='margin-left:60px;text-indent: -30px;'>";
                HTMLText+=EscHTML;
                HTMLText+="</p>";
            break;
            case e_PrivURIHelp_Style_TextLine:
                HTMLText+=EscHTML;
            break;
            case e_PrivURIHelp_StyleMAX:
            default:
                throw(0);
            break;
        }

        g_NewConnectionFromURIDialog->PrivURIRawHTML=HTMLText;

        textinput->blockSignals(true);
        textinput->setHtml(HTMLText);
    }
    catch(...)
    {
    }
    textinput->blockSignals(false);
}
