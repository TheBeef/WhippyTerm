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
 *    This software is the property of Paul Hutchinson. and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
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
