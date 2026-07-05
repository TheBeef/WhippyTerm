/*******************************************************************************
 * FILENAME: Dialog_SettingsHexDumpAppearance.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the settings hex dump appearance.  This is a sub dialog for
 *    the settings dialog.
 *
 * COPYRIGHT:
 *    Copyright 04 Jul 2026 Paul Hutchinson.
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
 *    Paul Hutchinson (04 Jul 2026)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Dialog_SettingsHexDumpAppearance.h"
#include "UI/UISettingsHexDumpAppearance.h"
#include "UI/UIAsk.h"
#include "UI/UIColorReq.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
uint32_t m_SHDA_DivColor;

/*******************************************************************************
 * NAME:
 *    RunSettingsHexDumpAppearanceDialog
 *
 * SYNOPSIS:
 *    void RunSettingsHexDumpAppearanceDialog(unsigned int *BytesPerLine,
 *              unsigned int *DivEvery,unsigned int *DivWidth,
 *              uint32_t *DivColor)
 *
 * PARAMETERS:
 *    BytesPerLine [I/O] -- How many bytes do we display per line
 *    DivEvery [I/O] -- Add a div every x bytes
 *    DivWidth [I/O] -- The number pixels to draw the div line as
 *    DivColor [I/O] -- What color to draw the div as
 *
 * FUNCTION:
 *    This function shows the settings hex dump appearance dialog.
 *
 * RETURNS:
 *    true -- User pressed OK
 *    false -- User pressed cancel or there was an error.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RunSettingsHexDumpAppearanceDialog(unsigned int *BytesPerLine,
        unsigned int *DivEvery,unsigned int *DivWidth,uint32_t *DivColor)
{
    t_UINumberInput *Num;
    t_UIColorPreviewCtrl *ColorPreview;

    try
    {
        if(!UIAlloc_Settings_HexDumpAppearance())
            throw("Failed to allocate dialog");

        Num=UISHDA_GetNumberInput(e_UISHDA_NumberInput_DivLineWidth);
        UISetNumberInputCtrlValue(Num,*DivWidth);
        Num=UISHDA_GetNumberInput(e_UISHDA_NumberInput_BytesPerLine);
        UISetNumberInputCtrlValue(Num,*BytesPerLine);
        Num=UISHDA_GetNumberInput(e_UISHDA_NumberInput_DividerEvery);
        UISetNumberInputCtrlValue(Num,*DivEvery);

        m_SHDA_DivColor=*DivColor;
        ColorPreview=UISHDA_GetColorPreview(e_UISHDA_ColorPreview_DivLineColor);
        UISetColorPreviewColor(ColorPreview,m_SHDA_DivColor);

        if(UIShow_Settings_HexDumpAppearance())
        {
            Num=UISHDA_GetNumberInput(e_UISHDA_NumberInput_DivLineWidth);
            *DivWidth=UIGetNumberInputCtrlValue(Num);
            Num=UISHDA_GetNumberInput(e_UISHDA_NumberInput_BytesPerLine);
            *BytesPerLine=UIGetNumberInputCtrlValue(Num);
            Num=UISHDA_GetNumberInput(e_UISHDA_NumberInput_DividerEvery);
            *DivEvery=UIGetNumberInputCtrlValue(Num);

            *DivColor=m_SHDA_DivColor;
        }
    }
    catch(const char *Msg)
    {
        UIAsk("Error",Msg,e_AskBox_Error,e_AskBttns_Ok);
    }
    catch(...)
    {
    }

    UIFree_Settings_HexDumpAppearance();
}

/*******************************************************************************
 * NAME:
 *    SHDA_Event
 *
 * SYNOPSIS:
 *    bool SHDA_Event(const struct SHDAEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event from the UI
 *
 * FUNCTION:
 *    This function is called from the UI when there is an event for the
 *    settings hex dump appearance dialog.
 *
 * RETURNS:
 *    true -- Handle the event noramlly
 *    false -- Cancel the event
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool SHDA_Event(const struct SHDAEvent *Event)
{
    uint32_t SelColor;
    t_UIColorPreviewCtrl *ColorPreview;

    switch(Event->EventType)
    {
        case e_SHDAEvent_BttnTriggered:
            switch(Event->Info.Bttn.BttnID)
            {
                case e_UISHDA_Button_SelectDivLineColor:
                    SelColor=UIGetColor(m_SHDA_DivColor);
                    if(SelColor!=0xFFFFFFFF)
                    {
                        m_SHDA_DivColor=SelColor;
                        ColorPreview=UISHDA_GetColorPreview(
                                e_UISHDA_ColorPreview_DivLineColor);
                        UISetColorPreviewColor(ColorPreview,m_SHDA_DivColor);
                    }
                break;
                case e_UISHDA_ButtonMAX:
                default:
                break;
            }
        break;
        case e_SHDAEvent_NumberInputChanged:
        break;
        case e_SHDAEventMAX:
        default:
        break;
    }
    return true;
}
