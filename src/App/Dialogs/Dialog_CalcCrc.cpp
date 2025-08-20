/*******************************************************************************
 * FILENAME: Dialog_CalcCrc.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the edit send buffer dialog in it.
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
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
 *    Paul Hutchinson (16 Aug 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/Dialogs/Dialog_CalcCrc.h"
#include "App/Dialogs/Dialog_ESB_ViewSource.h"
#include "App/Display/HexDisplayBuffers.h"
#include "App/Util/ClipboardHelpers.h"
#include "App/Settings.h"
#include "App/Session.h"
#include "UI/UICalcCrc.h"
#include "UI/UIAsk.h"
#include "App/Util/CRCSystem.h"
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static bool DCCRC_HexDisplayBufferEvent(const struct HDEvent *Event);
static void DCCRC_ReCalcCRC(void);
static void DCCRC_ClearCRC(void);
static void DCCRC_ShowSource(void);

/*** VARIABLE DEFINITIONS     ***/
class HexDisplayBuffer *m_DCCRC_HexDisplay;

//e_CRCType m_DCCRC_UseCRCType=e_CRC_CRC32;

/*******************************************************************************
 * NAME:
 *    RunCalcCrcDialog
 *
 * SYNOPSIS:
 *    bool RunCalcCrcDialog(uint8_t *DefaultData,int Bytes);
 *
 * PARAMETERS:
 *    DefaultData [I] -- The buffer to use for the CRC.
 *    Bytes [I] -- The size of 'DefaultData'.
 *
 * FUNCTION:
 *    This function shows the calculate CRC dialog.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RunCalcCrcDialog(uint8_t *DefaultData,int Bytes)
{
    t_UIContextMenuCtrl *ContextMenu_ClearScreen;
    t_ListViewItemListType ListOfCRCs;
    i_ListViewItemListType CurCrc;
    t_ComboBoxItemListType ComboxListOfCRCs;
    struct ComboBoxItem NewItem;
    t_UIComboBoxCtrl *CRCTypeInput;

    m_DCCRC_HexDisplay=NULL;
    try
    {
        if(!UIAlloc_CalcCrc())
            throw("Failed to allocate dialog");

        m_DCCRC_HexDisplay=new HexDisplayBuffer();
        if(!m_DCCRC_HexDisplay->Init(UICCRC_GetHexContainerFrame(),
                DCCRC_HexDisplayBufferEvent,0))
        {
            throw("Failed to connect hex display to UI");
        }

        m_DCCRC_HexDisplay->SetEditMode();
        m_DCCRC_HexDisplay->SetLossOfFocusBehavior(false);
        m_DCCRC_HexDisplay->SetFont(g_Settings.HexDisplaysFontName.c_str(),
                g_Settings.HexDisplaysFontSize,g_Settings.HexDisplaysFontBold,
                g_Settings.HexDisplaysFontItalic);
        m_DCCRC_HexDisplay->SetColors(g_Settings.HexDisplaysFGColor,
                g_Settings.HexDisplaysBGColor,g_Settings.HexDisplaysSelBGColor);

        ContextMenu_ClearScreen=m_DCCRC_HexDisplay->GetContextMenuHandle(e_UICTW_ContextMenu_ClearScreen);
        UISetContextMenuVisible(ContextMenu_ClearScreen,false);

        if(Bytes>0 && DefaultData!=NULL)
            m_DCCRC_HexDisplay->SetBuffer(DefaultData,Bytes);

        m_DCCRC_HexDisplay->RebuildDisplay();

        if(!CRC_GetListOfAvailableCRCs(ListOfCRCs))
            throw("Failed to build the list of CRC's (out of memory)");

        /* We don't use a listview, convert to a comboxbox list */
        for(CurCrc=ListOfCRCs.begin();CurCrc!=ListOfCRCs.end();
                CurCrc++)
        {
            NewItem.Label=CurCrc->Label;
            NewItem.ID=CurCrc->ID;
            ComboxListOfCRCs.push_back(NewItem);
        }

        CRCTypeInput=UICCRC_GetComboBoxHandle(e_CCRC_Combox_CRCType);
        UIAddList2ComboBox(CRCTypeInput,ComboxListOfCRCs);
        UISetComboBoxSelectedEntry(CRCTypeInput,g_Session.LastSelectedCalcCRCType);

        DCCRC_ReCalcCRC();

        UIShow_CalcCrc();

        g_Session.LastSelectedCalcCRCType=(e_CRCType)
                UIGetComboBoxSelectedEntry(CRCTypeInput);
    }
    catch(const char *Msg)
    {
        UIAsk("Error",Msg,e_AskBox_Error,e_AskBttns_Ok);
    }
    catch(...)
    {
    }

    if(m_DCCRC_HexDisplay!=NULL)
        delete m_DCCRC_HexDisplay;
    m_DCCRC_HexDisplay=NULL;
    UIFree_CalcCrc();
}

/*******************************************************************************
 * NAME:
 *    CCRC_Event
 *
 * SYNOPSIS:
 *    bool CCRC_Event(const struct CCRCEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event from the UI
 *
 * FUNCTION:
 *    This function is called from the UI when there is an event for the
 *    edit send buffer dialog.
 *
 * RETURNS:
 *    true -- Handle the event noramlly
 *    false -- Cancel the event
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool CCRC_Event(const struct CCRCEvent *Event)
{
    switch(Event->EventType)
    {
        case e_CCRCEvent_BttnTriggered:
            switch(Event->Info.Bttn.BttnID)
            {
                case e_CCRC_Button_CalcCRC:
                    DCCRC_ReCalcCRC();
                break;
                case e_CCRC_Button_ShowSource:
                    DCCRC_ShowSource();
                break;
                case e_CCRC_ButtonMAX:
                default:
                break;
            }
        break;
        case e_CCRCEvent_ComboxChange:
            DCCRC_ClearCRC();
        break;
        case e_CCRCEventMAX:
        default:
        break;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    DCCRC_HexDisplayBufferEvent
 *
 * SYNOPSIS:
 *    static bool DCCRC_HexDisplayBufferEvent(const struct HDEvent *
 *              Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event from the hex display buffer.
 *
 * FUNCTION:
 *    This function handles events from the hex display.
 *
 * RETURNS:
 *    true -- Accept this event
 *    false -- Cancel this event
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static bool DCCRC_HexDisplayBufferEvent(const struct HDEvent *Event)
{
    switch(Event->EventType)
    {
        case e_HDEvent_SelectionEvent:
        case e_HDEvent_MouseMove:
        case e_HDEvent_CursorMove:
        break;
        case e_HDEvent_BufferChange:
        case e_HDEvent_BufferResize:
            DCCRC_ClearCRC();
        break;
        case e_HDEvent_ContextMenu:
            switch(Event->Info.Context.Menu)
            {
                case e_UICTW_ContextMenu_Copy:
                    m_DCCRC_HexDisplay->
                            SendSelection2Clipboard(e_Clipboard_Clipboard,
                            e_HDBCFormat_Default);
                break;
                case e_UICTW_ContextMenu_Paste:
                    m_DCCRC_HexDisplay->
                            DoInsertFromClipboard(e_HDBCFormat_Default);
                break;
                case e_UICTW_ContextMenu_FindCRCAlgorithm:
                case e_UICTW_ContextMenu_EndianSwap:
                case e_UICTW_ContextMenu_ClearScreen:
                case e_UICTW_ContextMenu_Edit:
                case e_UICTW_ContextMenu_ZoomIn:
                case e_UICTW_ContextMenu_ZoomOut:
                case e_UICTW_ContextMenuMAX:
                default:
                break;
            }
        break;
        case e_HDEventMAX:
        default:
        break;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    DCCRC_ReCalcCRC
 *
 * SYNOPSIS:
 *    static void DCCRC_ReCalcCRC(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function recalc's the CRC and places it in the CRC input.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DCCRC_ReCalcCRC(void)
{
    const uint8_t *Data;
    int DataSize;
    t_UIComboBoxCtrl *CRCTypeInput;
    uint64_t CRC;
    t_UITextInputCtrl *CRCText;
    char buff[100];
    unsigned int Bits;

    CRCTypeInput=UICCRC_GetComboBoxHandle(e_CCRC_Combox_CRCType);
    CRCText=UICCRC_GetTextInput(e_CCRC_TextInput_CRC);

    if(!m_DCCRC_HexDisplay->GetBufferInfo(&Data,&DataSize))
    {
        UIAsk("Error","Failed to get internal hex buffer.  Not enough memory?",
                e_AskBox_Error,e_AskBttns_Ok);
        return;
    }

    Bits=CRC_CalcCRC((e_CRCType)UIGetComboBoxSelectedEntry(CRCTypeInput),
            Data,DataSize,&CRC);
    if(Bits==0)
    {
        /* We failed to calc the CRC */
        UIAsk("Error","Failed to calculate the CRC",e_AskBox_Error,
                e_AskBttns_Ok);
        return;
    }

    switch(Bits)
    {
        case 8:
            sprintf(buff,"%02" PRIX64,CRC);
        break;
        case 16:
            sprintf(buff,"%04" PRIX64,CRC);
        break;
        case 32:
            sprintf(buff,"%08" PRIX64,CRC);
        break;
        case 64:
            sprintf(buff,"%016" PRIX64,CRC);
        break;
    }
    UISetTextCtrlText(CRCText,buff);
}

/*******************************************************************************
 * NAME:
 *    DCCRC_ClearCRC
 *
 * SYNOPSIS:
 *    static void DCCRC_ClearCRC(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function clears the cal'c CRC input (clears the result)
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DCCRC_ClearCRC(void)
{
    t_UITextInputCtrl *CRCText;
    CRCText=UICCRC_GetTextInput(e_CCRC_TextInput_CRC);
    UISetTextCtrlText(CRCText,"");
}

/*******************************************************************************
 * NAME:
 *    DCCRC_ShowSource
 *
 * SYNOPSIS:
 *    static void DCCRC_ShowSource(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function handles the show source button being pressed.  Opens the
 *    show source dialog.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DCCRC_ShowSource(void)
{
    t_UIComboBoxCtrl *CRCTypeInput;
    e_CRCType SelectedAlg;
    string Source;

    CRCTypeInput=UICCRC_GetComboBoxHandle(e_CCRC_Combox_CRCType);

    SelectedAlg=(e_CRCType)UIGetComboBoxSelectedEntry(CRCTypeInput);
    if(SelectedAlg>=e_CRCMAX)
    {
        RunESB_ViewSourceDialog("Error.  Invalid CRC selection");
    }
    else
    {
        if(CRC_BuildSource4CRC(SelectedAlg,Source))
            RunESB_ViewSourceDialog(Source.c_str());
    }
}

