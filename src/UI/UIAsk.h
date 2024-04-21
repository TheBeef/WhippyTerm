/*******************************************************************************
 * FILENAME: UIAsk.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the UI to the ask message box.  The ask message box
 *    is a popup window with buttons at the bottom the user can pick from.
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
 *
 * HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __UIASK_H_
#define __UIASK_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <string>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

typedef enum
{
    e_AskBttns_Default,
    e_AskBttns_Ok,
    e_AskBttns_OkCancel,
    e_AskBttns_RetryCancel,
    e_AskBttns_YesNo,
    e_AskBttns_YesNoCancel,
    e_AskBttns_YesNoYesAll,
    e_AskBttns_YesNoNoAll,
    e_AskBttns_YesNoAlls,
    e_AskBttns_IgnoreCancel,
    e_AskBttnsMAX
} e_AskBttnsType;

typedef enum
{
    e_AskBox_Warning=0,
    e_AskBox_Question,
    e_AskBox_Info,
    e_AskBox_Error,
    e_AskBoxMAX
} e_AskBoxType;

typedef enum
{
    e_AskRet_Cancel,
    e_AskRet_Ok,
    e_AskRet_Retry,
    e_AskRet_Ignore,
    e_AskRet_Yes,
    e_AskRet_No,
    e_AskRet_YesAll,
    e_AskRet_NoAll,
    e_AskRetMAX
} e_AskRetType;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
e_AskRetType UIAsk(const char *Msg);
e_AskRetType UIAsk(const char *Title,const char *Msg,e_AskBoxType BoxType=e_AskBox_Info,e_AskBttnsType Buttons=e_AskBttns_Default);
e_AskRetType UIAsk(const char *Title,const std::string &Msg,e_AskBoxType BoxType=e_AskBox_Info,e_AskBttnsType Buttons=e_AskBttns_Default);
e_AskRetType UIAsk(const std::string &Title,const std::string &Msg,e_AskBoxType BoxType=e_AskBox_Info,e_AskBttnsType Buttons=e_AskBttns_Default);

#endif   /* end of "#ifndef __UIASK_H_" */
