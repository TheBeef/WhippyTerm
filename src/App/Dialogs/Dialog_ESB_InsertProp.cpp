/*******************************************************************************
 * FILENAME: Dialog_ESB_InsertProp.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (03 Sep 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_ESB_InsertProp.h"
#include "UI/UI_ESB_InsertProp.h"
#include "UI/UIAsk.h"
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    RunESB_InsertPropDialog
 *
 * SYNOPSIS:
 *    bool RunESB_InsertPropDialog(e_INEndianType &Endian,e_INBitsType &Bits,
 *              bool BitsLocked);
 *
 * PARAMETERS:
 *    Endian [I/O] -- What endian did the user selected
 *    Bits [I/O] -- The number of bits the user selected
 *    BitsLocked [I] -- If this is true then the user can't change the number
 *                      of bits.
 *
 * FUNCTION:
 *    This function shows the edit send buffer insert properties dialog.
 *
 * RETURNS:
 *    true -- User selected Ok
 *    false -- User selected Cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool RunESB_InsertPropDialog(e_INEndianType &Endian,e_INBitsType &Bits,
        bool BitsLocked)
{
    bool RetValue;
    t_UIRadioBttnCtrl *EndianBig;
    t_UIRadioBttnCtrl *EndianLittle;
    t_UIRadioBttnCtrl *Bits8;
    t_UIRadioBttnCtrl *Bits16;
    t_UIRadioBttnCtrl *Bits32;
    t_UIRadioBttnCtrl *Bits64;

    try
    {
        if(!UIAlloc_ESB_InsertProp())
            return false;

        EndianBig=UIESBIP_GetRadioButton(e_ESBIP_RadioButton_Endian_Big);
        EndianLittle=UIESBIP_GetRadioButton(e_ESBIP_RadioButton_Endian_Little);
        Bits8=UIESBIP_GetRadioButton(e_ESBIP_RadioButton_Bits_8);
        Bits16=UIESBIP_GetRadioButton(e_ESBIP_RadioButton_Bits_16);
        Bits32=UIESBIP_GetRadioButton(e_ESBIP_RadioButton_Bits_32);
        Bits64=UIESBIP_GetRadioButton(e_ESBIP_RadioButton_Bits_64);

        switch(Endian)
        {
            case e_INEndian_Big:
                UISelectRadioBttn(EndianBig);
            break;
            case e_INEndian_Little:
                UISelectRadioBttn(EndianLittle);
            break;
            case e_INEndianMAX:
            default:
            break;
        }

        switch(Bits)
        {
            case e_INBits_8:
                UISelectRadioBttn(Bits8);
            break;
            case e_INBits_16:
                UISelectRadioBttn(Bits16);
            break;
            case e_INBits_32:
                UISelectRadioBttn(Bits32);
            break;
            case e_INBits_64:
                UISelectRadioBttn(Bits64);
            break;
            case e_INBitsMAX:
            default:
            break;
        }

        if(BitsLocked)
        {
            UIEnableRadioBttn(Bits8,false);
            UIEnableRadioBttn(Bits16,false);
            UIEnableRadioBttn(Bits32,false);
            UIEnableRadioBttn(Bits64,false);
        }

        RetValue=UIShow_ESB_InsertProp();
        if(RetValue)
        {
            if(UIIsRadioBttnSelected(EndianBig))
                Endian=e_INEndian_Big;
            else if(UIIsRadioBttnSelected(EndianLittle))
                Endian=e_INEndian_Little;

            if(UIIsRadioBttnSelected(Bits8))
                Bits=e_INBits_8;
            else if(UIIsRadioBttnSelected(Bits16))
                Bits=e_INBits_16;
            else if(UIIsRadioBttnSelected(Bits32))
                Bits=e_INBits_32;
            else if(UIIsRadioBttnSelected(Bits64))
                Bits=e_INBits_64;
        }
    }
    catch(const char *Msg)
    {
        UIAsk("Error",Msg,e_AskBox_Error,e_AskBttns_Ok);
        RetValue=false;
    }
    catch(...)
    {
        RetValue=false;
    }

    UIFree_ESB_InsertProp();

    return RetValue;
}
