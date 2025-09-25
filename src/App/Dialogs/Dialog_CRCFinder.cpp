/*******************************************************************************
 * FILENAME: Dialog_CRCFinder.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the CRC finder dialog in it.
 *
 * COPYRIGHT:
 *    Copyright 04 Jun 2025 Paul Hutchinson.
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
 *    Paul Hutchinson (04 Jun 2025)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "UI/UICRCFinder.h"
#include "App/Util/CRCSystem.h"
#include "Dialog_CRCFinder.h"
#include "App/Display/HexDisplayBuffers.h"
#include "App/Dialogs/Dialog_ESB_ViewSource.h"
#include "App/Settings.h"
#include "App/Session.h"
#include "UI/UIAsk.h"
#include <stdio.h>
#include <string.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static bool DCF_HexDisplayBufferEvent(const struct HDEvent *Event);
static void DCF_RethinkUI(void);
static bool StripNonHex(const char *CRCStr,string &StripHex);
static void CF_FindCRC(void);
static void CF_ShowSource(void);

/*** VARIABLE DEFINITIONS     ***/
class HexDisplayBuffer *m_DCF_HexDisplay;

/*******************************************************************************
 * NAME:
 *    RunCRCFinderDialog
 *
 * SYNOPSIS:
 *    bool RunCRCFinderDialog(const uint8_t *DefaultData,int Bytes);
 *
 * PARAMETERS:
 *    DefaultData [I] -- The data to fill in to the hex view (the data area)
 *                       when the dialog is opened.
 *    Bytes [I] -- The number of bytes in 'DefaultData'
 *
 * FUNCTION:
 *    This function shows the CRC Finder dialog.  It will let the user find
 *    a crc algorithm that matches a CRC for given data.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RunCRCFinderDialog(const uint8_t *DefaultData,int Bytes)
{
    t_UIContextMenuCtrl *ContextMenu_ClearScreen;

    m_DCF_HexDisplay=NULL;
    try
    {
        if(!UIAlloc_CRCFinder())
            throw("Failed to allocate dialog");

        m_DCF_HexDisplay=new HexDisplayBuffer();
        if(!m_DCF_HexDisplay->Init(UICF_GetHexContainerFrame(),
                DCF_HexDisplayBufferEvent,0))
        {
            throw("Failed to connect hex display to UI");
        }

        m_DCF_HexDisplay->SetEditMode();
        m_DCF_HexDisplay->SetLossOfFocusBehavior(false);
        m_DCF_HexDisplay->SetFont(g_Settings.HexDisplaysFontName.c_str(),
                g_Settings.HexDisplaysFontSize,g_Settings.HexDisplaysFontBold,
                g_Settings.HexDisplaysFontItalic);
        m_DCF_HexDisplay->SetColors(g_Settings.HexDisplaysFGColor,
                g_Settings.HexDisplaysBGColor,g_Settings.HexDisplaysSelBGColor);

        ContextMenu_ClearScreen=m_DCF_HexDisplay->GetContextMenuHandle(e_UICTW_ContextMenu_ClearScreen);
        UISetContextMenuVisible(ContextMenu_ClearScreen,false);

        if(Bytes>0)
            m_DCF_HexDisplay->SetBuffer(DefaultData,Bytes);

        m_DCF_HexDisplay->RebuildDisplay();

        DCF_RethinkUI();

        UIShow_CRCFinder();
    }
    catch(const char *Msg)
    {
        UIAsk("Error",Msg,e_AskBox_Error,e_AskBttns_Ok);
    }
    catch(...)
    {
    }

    if(m_DCF_HexDisplay!=NULL)
        delete m_DCF_HexDisplay;
    m_DCF_HexDisplay=NULL;
    UIFree_CRCFinder();
}

/*******************************************************************************
 * NAME:
 *    DCF_HexDisplayBufferEvent
 *
 * SYNOPSIS:
 *    static bool DCF_HexDisplayBufferEvent(const struct HDEvent *
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
static bool DCF_HexDisplayBufferEvent(const struct HDEvent *Event)
{
    switch(Event->EventType)
    {
        case e_HDEvent_SelectionEvent:
            DCF_RethinkUI();
        break;
        case e_HDEvent_MouseMove:
        case e_HDEvent_CursorMove:
        case e_HDEvent_BufferResize:
        case e_HDEvent_BufferChange:
        break;
        case e_HDEvent_ContextMenu:
            switch(Event->Info.Context.Menu)
            {
                case e_UICTW_ContextMenu_Copy:
                    m_DCF_HexDisplay->
                            SendSelection2Clipboard(e_Clipboard_Clipboard,
                            e_HDBCFormat_Default);
                break;
                case e_UICTW_ContextMenu_Paste:
                    m_DCF_HexDisplay->
                            DoInsertFromClipboard(e_HDBCFormat_Default);
                break;
                case e_UICTW_ContextMenu_FindCRCAlgorithm:
                case e_UICTW_ContextMenu_CopyToSendBuffer:
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
 *    CF_Event
 *
 * SYNOPSIS:
 *    bool CF_Event(const struct CFEvent *Event);
 *
 * PARAMETERS:
 *    Event [I] -- The event from the UI
 *
 * FUNCTION:
 *    This function is called from the UI when there is a CRC Finder UI event.
 *
 * RETURNS:
 *    true -- Handle the event noramlly
 *    false -- Cancel the event
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool CF_Event(const struct CFEvent *Event)
{
    t_UIComboBoxCtrl *CRCTypeInput;
//    t_UIButtonCtrl *ShowSourceInput;

    CRCTypeInput=UICF_GetComboBoxHandle(e_CF_Combox_CRCType);
//    ShowSourceInput=UICF_GetButton(e_CF_Button_ShowSource);

    switch(Event->EventType)
    {
        case e_CFEvent_BttnTriggered:
            switch(Event->Info.Bttn.BttnID)
            {
                case e_CF_Button_FindCRC:
                    CF_FindCRC();
                break;
                case e_CF_Button_ShowSource:
                    CF_ShowSource();
                break;
                case e_CF_ButtonMAX:
                default:
                break;
            }
        break;
        case e_CFEvent_TextEdited:
            switch(Event->Info.Txt.TxtID)
            {
                case e_CF_TextInput_CRC:
                    /* Clear the type of CRC (because we changed the CRC) */
                    UIClearComboBox(CRCTypeInput);
                    DCF_RethinkUI();
                break;
                case e_CF_TextInputMAX:
                default:
                break;
            }
        break;
        case e_CFEvent_TextEditDone:
            switch(Event->Info.Txt.TxtID)
            {
                case e_CF_TextInput_CRC:
                    DCF_RethinkUI();
                break;
                case e_CF_TextInputMAX:
                default:
                break;
            }
        break;
        case e_CTEvent_ComboxChange:
        break;
        case e_CFEventMAX:
        default:
        break;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    DCF_RethinkUI
 *
 * SYNOPSIS:
 *    static void DCF_RethinkUI(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rethinks the UI.  It will enable / disable UI elements
 *    and update things as needed.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DCF_RethinkUI(void)
{
    t_UIContextMenuCtrl *ContextMenu_Copy;
    t_UITextInputCtrl *CRCInput;
    string CRC;
    const char *CRCStr;
    bool SelectionValid;
    bool CRCValid;
    t_UIComboBoxCtrl *CRCType;
    t_UIButtonCtrl *FindCRC;
    t_UIButtonCtrl *ShowSource;
    string StripHex;

    ContextMenu_Copy=m_DCF_HexDisplay->GetContextMenuHandle(e_UICTW_ContextMenu_Copy);
    CRCInput=UICF_GetTextInput(e_CF_TextInput_CRC);
    CRCType=UICF_GetComboBoxHandle(e_CF_Combox_CRCType);
    FindCRC=UICF_GetButton(e_CF_Button_FindCRC);
    ShowSource=UICF_GetButton(e_CF_Button_ShowSource);

    SelectionValid=m_DCF_HexDisplay->GetSelectionBounds(NULL,NULL);

    UIGetTextCtrlText(CRCInput,CRC);
    CRCStr=CRC.c_str();

    /* See if the CRC is valid */
    CRCValid=StripNonHex(CRCStr,StripHex);

    UIEnableButton(FindCRC,CRCValid);
    if(!CRCValid)
    {
        UIEnableComboBox(CRCType,false);
        UIEnableButton(ShowSource,false);
    }
    else
    {
        UIEnableComboBox(CRCType,true);

        if(UIGetComboBoxSelectedIndex(CRCType)<0)
        {
            UIEnableButton(ShowSource,false);
        }
        else
        {
            UIEnableButton(ShowSource,true);
        }
    }

    UIEnableContextMenu(ContextMenu_Copy,SelectionValid);
}

/*******************************************************************************
 * NAME:
 *    StripNonHex
 *
 * SYNOPSIS:
 *    static bool StripNonHex(const char *CRCStr,string &StripHex);
 *
 * PARAMETERS:
 *    CRCStr [I] -- The string to convert
 *    StripHex [O] -- The string we made with the result in it.
 *
 * FUNCTION:
 *    This function takes a string with a hex input in it and strips out
 *    all the non-digit chars returning a striped string that then can
 *    be converted to a number.
 *
 * RETURNS:
 *    true -- The string is good
 *    false -- The string is bad (has something in it that shouldn't or is
 *             the wrong length).
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static bool StripNonHex(const char *CRCStr,string &StripHex)
{
    bool FoundX;
    const char *Pos;

    /* See if the CRC is valid */
    if(*CRCStr==0)
        return false;
    Pos=CRCStr;
    FoundX=false;
    while(*Pos!=0)
    {
        if(*Pos=='x' || *Pos=='X')
        {
            if(FoundX)
            {
                /* We can only have 1 'x' */
                break;
            }
            FoundX=true;

            /* We need to have a diget after the x */
            if(*(Pos+1)<'0' || *(Pos+1)>'f' ||
                (*(Pos+1)>'9' && *(Pos+1)<'A') ||
                (*(Pos+1)>'F' && *(Pos+1)<'a'))
            {
                break;
            }
        }
        if(*Pos==' ')
            FoundX=false;
        if(*Pos<'0' && *Pos!=' ')
            break;
        if(*Pos>'9' && *Pos<'A')
            break;
        if(*Pos>'F' && *Pos<'a' && *Pos!='X')
            break;
        if(*Pos>'f' && *Pos!='x')
            break;
        Pos++;
    }
    if(*Pos!=0)
    {
        /* Found something we didn't like */
        return false;
    }

    /* Strip out anything that shouldn't be there */
    StripHex="";
    StripHex.reserve(Pos-CRCStr);

    Pos=CRCStr;
    while(*Pos!=0)
    {
        if(*Pos=='0' && (*(Pos+1)=='x' || *(Pos+1)=='X'))
        {
            /* Skip the 0x */
            Pos+=2;
            continue;
        }
        if(*Pos==' ')
        {
            Pos++;
            continue;
        }
        StripHex.append(1,*Pos);
        Pos++;
    }
    if(StripHex.empty())
        return false;

    /* The string needs be 2, 4, 8, or 16 bytes in length */
    switch(StripHex.length())
    {
        case 2:     // 1 byte
        case 4:     // 2 bytes
        case 8:     // 4 bytes
        case 16:    // 8 bytes
        break;
        default:
            return false;
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    CF_FindCRC
 *
 * SYNOPSIS:
 *    static void CF_FindCRC(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function is called when the find CRC button is clicked.  It clears
 *    the list of found CRC's and runs the find CRC function.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void CF_FindCRC(void)
{
    t_UIComboBoxCtrl *CRCTypeInput;
    t_UITextInputCtrl *CRCInput;
    t_UIButtonCtrl *ShowSourceInput;
    t_UIRadioBttnCtrl *LittleEndenInput;
    t_CRCListType FoundCRCTypes;
    const uint8_t *Data;
    const uint8_t *EndData;
    int DataSize;
    string CRC;
    string StripHex;
    string NewHex;
    t_ListViewItemListType AvailableCRC;
    i_CRCListType CurCRCType;
    i_ListViewItemListType CurCRCListEntry;
    t_ComboBoxItemListType FoundCRCComboxList;
    struct ComboBoxItem NewComboxItem;

    CRCTypeInput=UICF_GetComboBoxHandle(e_CF_Combox_CRCType);
    CRCInput=UICF_GetTextInput(e_CF_TextInput_CRC);
    ShowSourceInput=UICF_GetButton(e_CF_Button_ShowSource);
    LittleEndenInput=UICF_GetRadioBttnInput(e_CF_RadioBttn_Little);

    UIClearComboBox(CRCTypeInput);
    UIGetTextCtrlText(CRCInput,CRC);
    UIEnableButton(ShowSourceInput,false);

    if(m_DCF_HexDisplay->GetSelectionBounds(&Data,&EndData))
    {
        DataSize=EndData-Data;
    }
    else
    {
        /* We don't have a selection use the full thing */
        if(!m_DCF_HexDisplay->GetBufferInfo(&Data,&DataSize))
        {
            UIAsk("Error","Failed to get internal hex buffer.  Not enough memory?",
                    e_AskBox_Error,e_AskBttns_Ok);
            return;
        }
    }

    if(!StripNonHex(CRC.c_str(),StripHex))
    {
        UIAsk("Error","Invalid CRC hex value",e_AskBox_Error,e_AskBttns_Ok);
        return;
    }

    if(UIIsRadioBttnSelected(LittleEndenInput))
    {
        /* Ok, we need to swap the enden */
        NewHex="";
        switch(StripHex.length())
        {
            case 2:
                /* Nothing to do */
                NewHex=StripHex;
            break;
            case 4:
                NewHex+=StripHex[2];
                NewHex+=StripHex[3];
                NewHex+=StripHex[0];
                NewHex+=StripHex[1];
            break;
            case 8:
                NewHex+=StripHex[6];
                NewHex+=StripHex[7];
                NewHex+=StripHex[4];
                NewHex+=StripHex[5];
                NewHex+=StripHex[2];
                NewHex+=StripHex[3];
                NewHex+=StripHex[0];
                NewHex+=StripHex[1];
            break;
            case 16:
                NewHex+=StripHex[14];
                NewHex+=StripHex[15];
                NewHex+=StripHex[12];
                NewHex+=StripHex[13];
                NewHex+=StripHex[10];
                NewHex+=StripHex[11];
                NewHex+=StripHex[8];
                NewHex+=StripHex[9];
                NewHex+=StripHex[6];
                NewHex+=StripHex[7];
                NewHex+=StripHex[4];
                NewHex+=StripHex[5];
                NewHex+=StripHex[2];
                NewHex+=StripHex[3];
                NewHex+=StripHex[0];
                NewHex+=StripHex[1];
            break;
            default:
                UIAsk("Error","Unsupported CRC hex length",e_AskBox_Error,
                        e_AskBttns_Ok);
                return;
            break;
        }
        StripHex=NewHex;
    }

    FoundCRCTypes=CRC_FindCRC(Data,DataSize,StripHex.c_str());
    if(FoundCRCTypes.empty())
    {
        UIAsk("CRC","No matching CRC algorithm found",e_AskBox_Info,
                e_AskBttns_Ok);
        return;
    }

    /* Ok, we get a list of all available CRC alg's, then we go through
       and make a new list of CRC alg's to put in the combox input (the
       list is in listview format, but we build a combox list) */
    if(!CRC_GetListOfAvailableCRCs(AvailableCRC))
    {
        UIAsk("Error","Failed to get a list of the CRC algorithms.\n"
                "Try again with a different endian?",e_AskBox_Error,
                e_AskBttns_Ok);
        return;
    }

    for(CurCRCType=FoundCRCTypes.begin();CurCRCType!=FoundCRCTypes.end();
            CurCRCType++)
    {
        /* See if this one is in the list (they always should be) */
        for(CurCRCListEntry=AvailableCRC.begin();
                CurCRCListEntry!=AvailableCRC.end();CurCRCListEntry++)
        {
            if(CurCRCListEntry->ID==*CurCRCType)
            {
                /* Found */
                break;
            }
        }
        if(CurCRCListEntry==AvailableCRC.end())
        {
            UIAsk("Error","Internal error.  Could not find CRC alg in the "
                    "list of algorithms but it should have been there????",
                    e_AskBox_Error,e_AskBttns_Ok);
            continue;
        }

        /* Add it to the list of found CRC alg's */
        NewComboxItem.Label=CurCRCListEntry->Label;
        NewComboxItem.ID=CurCRCListEntry->ID;
        FoundCRCComboxList.push_back(NewComboxItem);
    }

    /* Set the combox to the list of found items */
    UIAddList2ComboBox(CRCTypeInput,FoundCRCComboxList);

    /* Enable the show source button */
    UIEnableButton(ShowSourceInput,true);
}

/*******************************************************************************
 * NAME:
 *    CF_ShowSource
 *
 * SYNOPSIS:
 *    static void CF_ShowSource(void);
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
static void CF_ShowSource(void)
{
    t_UIComboBoxCtrl *CRCTypeInput;
    e_CRCType SelectedAlg;
    string Source;

    CRCTypeInput=UICF_GetComboBoxHandle(e_CF_Combox_CRCType);

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

