/*******************************************************************************
 * FILENAME: AskMessageBox.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the ask message box functions in it for doing QT message
 *    boxes.
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
#include "UI/UIAsk.h"
#include "AskMessageBox.h"
#include <QMessageBox>
#include "Form_MainWindow.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    UIAsk
 *
 * SYNOPSIS:
 *    e_AskRetType UIAsk(const char *Msg);
 *    e_AskRetType UIAsk(const char *Title,const char *Msg,
 *          e_AskBoxType BoxType,e_AskBttnsType Buttons);
 *    e_AskRetType UIAsk(const char *Title,const std::string &Msg,
 *          e_AskBoxType BoxType,e_AskBttnsType Buttons=e_AskBttns_Default);
 *    e_AskRetType UIAsk(const std::string Title,const std::string Msg,
 *          e_AskBoxType BoxType,e_AskBttnsType Buttons=e_AskBttns_Default);
 *
 * PARAMETERS:
 *    Title [I] -- The window title
 *    Msg [I] -- The message to display.  This can have \n's
 *    BoxType [I] -- The type of popup box:
 *              e_AskBox_Warning -- This is a warning
 *              e_AskBox_Question -- Asking a question
 *              e_AskBox_Info -- Tells the user something
 *              e_AskBox_Error -- There was an error
 *    Buttons [I] -- What buttons are available for the user to pick from.
 *                      e_AskBttns_Default -- The buttons depend on the
 *                                            'BoxType'.
 *                      e_AskBttns_Ok -- Only an ok button
 *                      e_AskBttns_OkCancel -- Ok and cancel buttons
 *                      e_AskBttns_RetryCancel -- Retry and cancel buttons
 *                      e_AskBttns_YesNo -- Yes and no buttons
 *                      e_AskBttns_YesNoCancel -- Yes, No, and Cancel buttons
 *                      e_AskBttns_YesNoYesAll -- Yes, No, and a "Yes to all"
 *                                                buttons
 *                      e_AskBttns_YesNoNoAll -- Yes, no and a "No to all"
 *                                               buttons.
 *                      e_AskBttns_YesNoAlls -- Yes, no, "Yes to all", and
 *                                              "No to all" buttons
 *                      e_AskBttns_IgnoreCancel -- Ignore and Cancel buttons.
 *
 * FUNCTION:
 *    This function does a message popup window that blocks until the user
 *    selects a choice.
 *
 * RETURNS:
 *    This function returns one of the following (depending on what buttons
 *    are available and what button the user selected): 
 *          e_AskRet_Cancel -- The cancel button
 *          e_AskRet_Ok -- The ok button
 *          e_AskRet_Retry -- The retry button
 *          e_AskRet_Ignore -- The ignore button
 *          e_AskRet_Yes -- The yes button
 *          e_AskRet_No -- The no button
 *          e_AskRet_YesAll -- The "yes to all" button
 *          e_AskRet_NoAll -- The "no to all" button
 *
 * SEE ALSO:
 *    
 *******************************************************************************
 * REVISION HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 ******************************************************************************/
e_AskRetType UIAsk(const char *Msg)
{
    return UIAsk("Message",Msg,e_AskBox_Info,e_AskBttns_Ok);
}
e_AskRetType UIAsk(const char *Title,const std::string &Msg,
        e_AskBoxType BoxType,e_AskBttnsType Buttons)
{
    return UIAsk(Title,Msg.c_str(),BoxType,Buttons);
}
e_AskRetType UIAsk(const std::string &Title,const std::string &Msg,
        e_AskBoxType BoxType,e_AskBttnsType Buttons)
{
    return UIAsk(Title.c_str(),Msg.c_str(),BoxType,Buttons);
}
e_AskRetType UIAsk(const char *Title,const char *Msg,
        e_AskBoxType BoxType,e_AskBttnsType Buttons)
{
    e_AskBttnsType UseButtons;   // The modified copy of 'Buttons'
    int Selected;   // What did the user select
    QMessageBox::StandardButtons QTBttns;        // What buttons are we displaying
    e_AskRetType RetValue;  // The value we are returning
    QWidget *ParentWindow;

    UseButtons=Buttons;
    if(UseButtons==e_AskBttns_Default)
    {
        switch(BoxType)
        {
            case e_AskBox_Warning:
                UseButtons=e_AskBttns_Ok;
            break;
            case e_AskBox_Question:
                UseButtons=e_AskBttns_YesNo;
            break;
            case e_AskBox_Info:
                UseButtons=e_AskBttns_Ok;
            break;
            case e_AskBox_Error:
                UseButtons=e_AskBttns_Ok;
            break;
            default:
            case e_AskBoxMAX:
                return e_AskRetMAX;
        }
    }

    switch(UseButtons)
    {
        case e_AskBttns_Ok:
            QTBttns=QMessageBox::Ok;
        break;
        case e_AskBttns_OkCancel:
            QTBttns=QMessageBox::Ok | QMessageBox::Cancel;
        break;
        case e_AskBttns_RetryCancel:
            QTBttns=QMessageBox::Retry | QMessageBox::Cancel;
        break;
        case e_AskBttns_YesNo:
            QTBttns=QMessageBox::Yes | QMessageBox::No;
        break;
        case e_AskBttns_YesNoCancel:
            QTBttns=QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel;
        break;
        case e_AskBttns_YesNoYesAll:
            QTBttns=QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll;
        break;
        case e_AskBttns_YesNoNoAll:
            QTBttns=QMessageBox::Yes | QMessageBox::No | QMessageBox::NoToAll;
        break;
        case e_AskBttns_YesNoAlls:
            QTBttns=QMessageBox::Yes | QMessageBox::No | QMessageBox::YesToAll | QMessageBox::NoToAll;
        break;
        case e_AskBttns_IgnoreCancel:
            QTBttns=QMessageBox::Ignore | QMessageBox::Cancel;
        break;
        case e_AskBttns_Default:
        case e_AskBttnsMAX:
        default:
            return e_AskRetMAX;
    }

    /* DEBUG PAUL: These functions may not work if ParentWindow is NULL */
    ParentWindow=g_MainApp->activeWindow();
    switch(BoxType)
    {
        case e_AskBox_Warning:
            Selected=QMessageBox::warning(ParentWindow,Title,Msg,QTBttns);
        break;
        case e_AskBox_Question:
            Selected=QMessageBox::question(ParentWindow,Title,Msg,QTBttns);
        break;
        case e_AskBox_Info:
            Selected=QMessageBox::information(ParentWindow,Title,Msg,QTBttns);
        break;
        case e_AskBox_Error:
            Selected=QMessageBox::critical(ParentWindow,Title,Msg,QTBttns);
        break;
        default:
        case e_AskBoxMAX:
            return e_AskRetMAX;
    }

    switch(Selected)
    {
        case QMessageBox::Ok:
            RetValue=e_AskRet_Ok;
        break;
        default:
        case QMessageBox::Cancel:
            RetValue=e_AskRet_Cancel;
        break;
        case QMessageBox::Yes:
            RetValue=e_AskRet_Yes;
        break;
        case QMessageBox::YesToAll:
            RetValue=e_AskRet_YesAll;
        break;
        case QMessageBox::No:
            RetValue=e_AskRet_No;
        break;
        case QMessageBox::NoToAll:
            RetValue=e_AskRet_NoAll;
        break;
        case QMessageBox::Retry:
            RetValue=e_AskRet_Retry;
        break;
        case QMessageBox::Ignore:
            RetValue=e_AskRet_Ignore;
        break;
    }

    return RetValue;
}


