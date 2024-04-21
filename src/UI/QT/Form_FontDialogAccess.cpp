/*******************************************************************************
 * FILENAME: Form_FontDialogAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the font requester access functions in it.
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
 *    Paul Hutchinson (02 Jun 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_FontDialog.h"
#include "ui_Form_FontDialog.h"
#include "Form_MainWindow.h"
#include "UI/UIFontReq.h"
#include "main.h"
#include <QFont>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    UI_FontReq
 *
 * SYNOPSIS:
 *    bool UI_FontReq(const char *Title,std::string &FontName,
 *          int &FontSize,long &FontStyle,long Flags);
 *
 * PARAMETERS:
 *    Title [I] -- The title for the dialog
 *    FontName [I/O] -- The name of the font to use.  This is also set to the
 *                      value of the new font if the user presses ok.
 *    FontSize [I/O] -- The font size to use.  This is also set to the value
 *                      of the new font if the user presses ok.
 *    FontStyle [I/O] -- What styles of font to use.  Supported values:
 *                          UIFONT_STYLE_BOLD -- Bold
 *                          UIFONT_STYLE_ITALIC -- Italic
 *                       This is also set to the value of the new font if the
 *                       user presses ok.
 *    Flags [I] -- Supported flags:
 *                      UIFONT_FLAGS_FIXEDWIDTH -- Only let the user select
 *                              fixed width fonts.
 *
 * FUNCTION:
 *    This function displays the font requester for the user to select a font.
 *
 * RETURNS:
 *    true -- The user picked ok
 *    false -- The user canceled
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UI_FontReq(const char *Title,std::string &FontName,
        int &FontSize,long &FontStyle,long Flags)
{
    class Form_FontDialog *FontDialog;
    bool RetValue;

    try
    {
        FontDialog=new Form_FontDialog(g_MainApp->activeWindow());
        FontDialog->SetOptions(Title,FontName,FontSize,FontStyle,Flags);

        if(FontDialog->exec()==QDialog::Accepted)
        {
            FontDialog->GetFontInfo(FontName,FontSize,FontStyle);
            RetValue=true;
        }
        else
        {
            RetValue=false;
        }

        delete FontDialog;
    }
    catch(...)
    {
        RetValue=false;
    }

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    UI_GetDefaultFixedWidthFont
 *
 * SYNOPSIS:
 *    void UI_GetDefaultFixedWidthFont(std::string &FontName);
 *
 * PARAMETERS:
 *    FontName [O] -- The name of the default fixed width font
 *
 * FUNCTION:
 *    This function returns the name of the systems default fixed width
 *    font.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UI_GetDefaultFixedWidthFont(std::string &FontName)
{
    QFont font("Monospace");
    font.setStyleHint(QFont::TypeWriter);

    FontName=qPrintable(font.family());
}

