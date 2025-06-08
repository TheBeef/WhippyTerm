/*******************************************************************************
 * FILENAME: Dialog_EditSendBuffer.cpp
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
#include "App/Dialogs/Dialog_EditSendBuffer.h"
#include "App/Dialogs/Dialog_ESB_CRCType.h"
#include "App/Dialogs/Dialog_ESB_Fill.h"
#include "App/Dialogs/Dialog_ESB_InsertProp.h"
#include "App/Dialogs/Dialog_PasteData.h"
#include "App/Display/HexDisplayBuffers.h"
#include "App/Util/ClipboardHelpers.h"
#include "App/SendBuffer.h"
#include "App/Settings.h"
#include "App/Session.h"
#include "UI/UIEditSendBuffer.h"
#include "UI/UIAsk.h"
#include "UI/UIFileReq.h"
#include "App/Util/CRCSystem.h"
#include <stdio.h>
#include <string.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static bool DESB_HexDisplayBufferEvent(const struct HDEvent *Event);
static void DESB_DoInsertNumber(void);
static void DESB_DoInsertCRC(void);
static void DESB_DoEndianSwap(void);
static void DESB_DoImportDataFromDisk(void);
static void DESB_DoExportData2Disk(void);
static void DESB_RethinkButtons(void);
static void DESB_DoInsertText(void);
static void DESB_DoInsertFromDisk(void);
static void DESB_DoInsertFromClipboard(void);
static void DESB_DoSaveBuffer(void);
static void DESB_DoLoadBuffer(void);

/*** VARIABLE DEFINITIONS     ***/
class HexDisplayBuffer *m_DESB_HexDisplay;

e_CRCType m_DESB_UseCRCType=e_CRC_CRC32;
uint8_t m_DESB_LastFillValue=0xFF;
e_INEndianType m_DESB_Endian=e_INEndian_Little;
e_INBitsType m_DESB_Bits=e_INBits_16;

/*******************************************************************************
 * NAME:
 *    RunEditSendBufferDialog
 *
 * SYNOPSIS:
 *    bool RunEditSendBufferDialog(int BufferNumber,uint8_t **CustomBuffer,
 *              int *CustomBufferSize);
 *
 * PARAMETERS:
 *    BufferNumber [I] -- What buffer to edit.  This is ignored if
 *                        'CustomBuffer' is not NULL.
 *    CustomBuffer [I] -- The buffer to edit if using a custom buffer.  This is
 *                        a pointer to a pointer because we may free the
 *                        org pointer and allocate a new one, in that case we
 *                        need to return the new pointer.
 *    CustomBufferSize [I] -- The size of 'CustomBuffer'.
 *
 * FUNCTION:
 *    This function shows the edit send buffer dialog.  It will edit the
 *    requested buffer from the global send buffers system.
 *
 * RETURNS:
 *    true -- User selected Ok
 *    false -- User selected Cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool RunEditSendBufferDialog(int BufferNumber,uint8_t **CustomBuffer,
        int *CustomBufferSize)
{
    bool RetValue;
    const uint8_t *Memory;
    uint32_t BuffSize;
    const uint8_t *EditBuffer;
    int BufferSize;
    t_UITextInputCtrl *BufferName;
    string Name;
    char buff[100];
    t_UIContextMenuCtrl *ContextMenu_ClearScreen;
    t_UIContextMenuCtrl *ContextMenu_EndianSwap;

    m_DESB_HexDisplay=NULL;
    try
    {
        if(!UIAlloc_EditSendBuffer())
            return false;

        m_DESB_HexDisplay=new HexDisplayBuffer();
        if(!m_DESB_HexDisplay->Init(UIESB_GetHexContainerFrame(),
                DESB_HexDisplayBufferEvent,0))
        {
            throw("Failed to connect hex display to UI");
        }

        if(CustomBuffer==NULL)
        {
            if(!g_SendBuffers.GetBufferInfo(BufferNumber,&Memory,&BuffSize))
            {
                throw("Failed to get buffer to edit");
            }
        }

        m_DESB_HexDisplay->SetEditMode();
        m_DESB_HexDisplay->SetLossOfFocusBehavior(false);
        m_DESB_HexDisplay->SetFont(g_Settings.HexDisplaysFontName.c_str(),
                g_Settings.HexDisplaysFontSize,g_Settings.HexDisplaysFontBold,
                g_Settings.HexDisplaysFontItalic);
        m_DESB_HexDisplay->SetColors(g_Settings.HexDisplaysFGColor,
                g_Settings.HexDisplaysBGColor,g_Settings.HexDisplaysSelBGColor);

        ContextMenu_ClearScreen=m_DESB_HexDisplay->GetContextMenuHandle(e_UICTW_ContextMenu_ClearScreen);
        ContextMenu_EndianSwap=m_DESB_HexDisplay->GetContextMenuHandle(e_UICTW_ContextMenu_EndianSwap);

        UISetContextMenuVisible(ContextMenu_ClearScreen,false);
        UISetContextMenuVisible(ContextMenu_EndianSwap,true);

        if(CustomBuffer==NULL)
            m_DESB_HexDisplay->SetBuffer(Memory,BuffSize);
        else
            m_DESB_HexDisplay->SetBuffer(*CustomBuffer,*CustomBufferSize);

        m_DESB_HexDisplay->RebuildDisplay();

        m_DESB_UseCRCType=g_Session.LastSelectedCRCType;

        BufferName=UIESB_GetTextInput(e_ESB_TextInput_BufferName);
        if(CustomBuffer==NULL)
        {
            Name=g_SendBuffers.GetBufferName(BufferNumber);
            UISetTextCtrlText(BufferName,Name.c_str());
        }
        else
        {
            UIEnableTextCtrl(BufferName,false);
        }

        DESB_RethinkButtons();

        RetValue=UIShow_EditSendBuffer();
        if(RetValue)
        {
            if(!m_DESB_HexDisplay->GetBufferInfo(&EditBuffer,&BufferSize))
                throw("Failed to get buffer");

            if(CustomBuffer==NULL)
            {
                BufferName=UIESB_GetTextInput(e_ESB_TextInput_BufferName);
                UIGetTextCtrlText(BufferName,Name);
                if(Name=="")
                {
                    sprintf(buff,"Buffer %d",BufferNumber+1);
                    Name=buff;
                }

                g_SendBuffers.SetBufferName(BufferNumber,Name.c_str());
                g_SendBuffers.SetBuffer(BufferNumber,EditBuffer,BufferSize);
                g_SendBuffers.SaveBuffers();
            }
            else
            {
                /* We are using a custom buffer, free the old one then
                   copy the data from the hex buffer before we free it */
                free(*CustomBuffer);

                *CustomBuffer=(uint8_t *)malloc(BufferSize);
                if(*CustomBuffer==NULL)
                    throw("Failed to allocate memory needed copy the buffer");
                memcpy(*CustomBuffer,EditBuffer,BufferSize);

                *CustomBufferSize=BufferSize;
            }
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

    if(m_DESB_HexDisplay!=NULL)
        delete m_DESB_HexDisplay;
    m_DESB_HexDisplay=NULL;
    UIFree_EditSendBuffer();

    return RetValue;
}

/*******************************************************************************
 * NAME:
 *    ESB_Event
 *
 * SYNOPSIS:
 *    bool ESB_Event(const struct ESBEvent *Event);
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
bool ESB_Event(const struct ESBEvent *Event)
{
    const uint8_t *SelStartPtr;
    const uint8_t *SelEndPtr;
    char buff[100];
    t_UITextInputCtrl *CRC;
    t_UITextInputCtrl *BufferSize;
    string Name;
    string NewSizeTxt;
    int NewSize;
    bool Locked;

    CRC=UIESB_GetTextInput(e_ESB_TextInput_CRC);

    switch(Event->EventType)
    {
        case e_ESBEvent_BttnTriggered:
            switch(Event->Info.Bttn.BttnID)
            {
                case e_ESB_Button_Clear:
                    if(UIAsk("Are you sure?","This will erase all the data.\n"
                            "Are you sure?",e_AskBox_Question,
                            e_AskBttns_YesNo)==e_AskRet_Yes)
                    {
                        m_DESB_HexDisplay->SetBufferSize(0);
                    }
                break;
                case e_ESB_Button_Fill:
                    if(!m_DESB_HexDisplay->GetSelectionBounds(&SelStartPtr,
                            &SelEndPtr))
                    {
                        return true;
                    }

                    if(!RunESB_FillDialog(m_DESB_LastFillValue))
                        return true;

                    /* Replace the current selection with the value */
                    m_DESB_HexDisplay->
                            FillSelectionWithValue(m_DESB_LastFillValue);
                break;
                case e_ESB_Button_EndianSwap:
                    DESB_DoEndianSwap();
                break;
                case e_ESB_Button_CRCType:
                    if(RunESB_CRCTypeDialog(m_DESB_UseCRCType))
                    {
                        /* Redo the CRC */
                        if(m_DESB_HexDisplay->GetSelectionBounds(&SelStartPtr,
                                &SelEndPtr))
                        {
                            CRC_CalCRCStr(m_DESB_UseCRCType,SelStartPtr,
                                    SelEndPtr-SelStartPtr,buff);
                            UISetTextCtrlText(CRC,buff);
                        }

                        g_Session.LastSelectedCRCType=m_DESB_UseCRCType;
                        NoteSessionChanged();
                    }
                break;
                case e_ESB_Button_InsertCRC:
                    DESB_DoInsertCRC();
                break;
                case e_ESB_Button_InsertAsNumber:
                    DESB_DoInsertNumber();
                    return true;
                break;
                case e_ESB_Button_InsertAsText:
                    DESB_DoInsertText();
                    return true;
                break;
                case e_ESB_Button_InsertProperties:
                    if(m_DESB_HexDisplay->GetSelectionBounds(NULL,NULL))
                        Locked=true;
                    else
                        Locked=false;
                    RunESB_InsertPropDialog(m_DESB_Endian,m_DESB_Bits,Locked);
                break;
                case e_ESB_ButtonMAX:
                default:
                break;
            }
        break;
        case e_ESBEvent_TextEditDone:
            switch(Event->Info.Txt.TxtID)
            {
                case e_ESB_TextInput_BufferSize:
                    /* User changed the size of the buffer */
                    BufferSize=UIESB_GetTextInput(e_ESB_TextInput_BufferSize);
                    UIGetTextCtrlText(BufferSize,NewSizeTxt);
                    NewSize=strtol(NewSizeTxt.c_str(),NULL,0);

                    m_DESB_HexDisplay->SetBufferSize(NewSize);
                    sprintf(buff,"%d",NewSize);
                    UISetTextCtrlText(BufferSize,buff);
                break;
                case e_ESB_TextInput_SelectedBytes:
                case e_ESB_TextInput_Offset:
                case e_ESB_TextInput_CRC:
                case e_ESB_TextInput_InsertTxt:
                case e_ESB_TextInput_BufferName:
                case e_ESB_TextInputMAX:
                default:
                break;
            }
        break;
        case e_ESBEvent_ContextMenuClicked:
            switch(Event->Info.Context.MenuID)
            {
                case e_ESB_ContextMenu_LoadBuffer:
                    DESB_DoLoadBuffer();
                break;
                case e_ESB_ContextMenu_SaveBuffer:
                    DESB_DoSaveBuffer();
                break;
                case e_ESB_ContextMenu_ExportData:
                    DESB_DoExportData2Disk();
                break;
                case e_ESB_ContextMenu_ImportData:
                    DESB_DoImportDataFromDisk();
                break;
                case e_ESB_ContextMenu_InsertFromDisk:
                    DESB_DoInsertFromDisk();
                break;
                case e_ESB_ContextMenu_InsertFromClipboard:
                    DESB_DoInsertFromClipboard();
                break;
                case e_ESB_ContextMenuMAX:
                default:
                break;
            }
        break;
        case e_ESBEventMAX:
        default:
        break;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    DESB_HexDisplayBufferEvent
 *
 * SYNOPSIS:
 *    static bool DESB_HexDisplayBufferEvent(const struct HDEvent *
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
static bool DESB_HexDisplayBufferEvent(const struct HDEvent *Event)
{
    t_UITextInputCtrl *SelectionBytes;
    t_UITextInputCtrl *CRC;
    t_UITextInputCtrl *Offset;
    t_UITextInputCtrl *BufferSize;
    char buff[100];
    int BytesInSelection;

    switch(Event->EventType)
    {
        case e_HDEvent_SelectionEvent:
            SelectionBytes=UIESB_GetTextInput(e_ESB_TextInput_SelectedBytes);
            CRC=UIESB_GetTextInput(e_ESB_TextInput_CRC);

            BytesInSelection=Event->Info.Select.EndOfSel-
                    Event->Info.Select.StartOfSel;

            if(Event->Info.Select.SelectionValid)
                sprintf(buff,"%d",BytesInSelection);
            else
                buff[0]=0;
            UISetTextCtrlText(SelectionBytes,buff);

            if(Event->Info.Select.SelectionValid)
            {
                CRC_CalCRCStr(m_DESB_UseCRCType,Event->Info.Select.StartOfSel,
                        BytesInSelection,buff);
                UISetTextCtrlText(CRC,buff);
            }
            DESB_RethinkButtons();
        break;
        case e_HDEvent_MouseMove:
        break;
        case e_HDEvent_CursorMove:
            Offset=UIESB_GetTextInput(e_ESB_TextInput_Offset);
            sprintf(buff,"%d",Event->Info.Cursor.Pos);
            UISetTextCtrlText(Offset,buff);
        break;
        case e_HDEvent_BufferResize:
            BufferSize=UIESB_GetTextInput(e_ESB_TextInput_BufferSize);
            sprintf(buff,"%d",Event->Info.Buffer.Size);
            UISetTextCtrlText(BufferSize,buff);
        break;
        case e_HDEvent_ContextMenu:
            switch(Event->Info.Context.Menu)
            {
                case e_UICTW_ContextMenu_Copy:
                    m_DESB_HexDisplay->
                            SendSelection2Clipboard(e_Clipboard_Clipboard,
                            e_HDBCFormat_Default);
                break;
                case e_UICTW_ContextMenu_Paste:
                    m_DESB_HexDisplay->
                            DoInsertFromClipboard(e_HDBCFormat_Default);
                break;
                case e_UICTW_ContextMenu_EndianSwap:
                    DESB_DoEndianSwap();
                break;
                case e_UICTW_ContextMenu_FindCRCAlgorithm:
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
 *    DESB_DoInsertNumber
 *
 * SYNOPSIS:
 *    static void DESB_DoInsertNumber(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function does the insert number button press.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DESB_DoInsertNumber(void)
{
    const uint8_t *StartOfBuffPtr;
    int BuffSize;
    int SelSize;
    int InsertPos;
    const uint8_t *SelStartPtr;
    const uint8_t *SelEndPtr;
    uint64_t Number;
    uint8_t NumberBuff[8];
    int Bytes2Insert;
    bool DoReplace;
    t_UITextInputCtrl *InsertTxt;
    std::string InsertTxtAsStr;

    if(m_DESB_HexDisplay->GetSelectionBounds(&SelStartPtr,&SelEndPtr))
    {
        /* Only let us do this if we are one of the supported
           sizes */
        SelSize=SelEndPtr-SelStartPtr;
        switch(SelSize)
        {
            case 1:
                m_DESB_Bits=e_INBits_8;
            break;
            case 2:
                m_DESB_Bits=e_INBits_16;
            break;
            case 4:
                m_DESB_Bits=e_INBits_32;
            break;
            case 8:
                m_DESB_Bits=e_INBits_64;
            break;
            default:
                /* User shouldn't have been able to push the button so just
                   ignore */
            return;
        }
        if(!m_DESB_HexDisplay->GetBufferInfo(&StartOfBuffPtr,&BuffSize))
            return;
        InsertPos=SelStartPtr-StartOfBuffPtr;
        DoReplace=true;
    }
    else
    {
        /* Inserting the values */
        InsertPos=m_DESB_HexDisplay->GetCursorPos();
        DoReplace=false;
    }

    InsertTxt=UIESB_GetTextInput(e_ESB_TextInput_InsertTxt);
    UIGetTextCtrlText(InsertTxt,InsertTxtAsStr);
    Number=strtoll(InsertTxtAsStr.c_str(),NULL,0);

    switch(m_DESB_Endian)
    {
        case e_INEndian_Big:
            switch(m_DESB_Bits)
            {
                case e_INBits_8:
                    NumberBuff[0]=Number&0xFF;
                    Bytes2Insert=1;
                break;
                case e_INBits_16:
                    NumberBuff[0]=(Number>>8)&0xFF;
                    NumberBuff[1]=Number&0xFF;
                    Bytes2Insert=2;
                break;
                case e_INBits_32:
                    NumberBuff[0]=(Number>>24)&0xFF;
                    NumberBuff[1]=(Number>>16)&0xFF;
                    NumberBuff[2]=(Number>>8)&0xFF;
                    NumberBuff[3]=Number&0xFF;
                    Bytes2Insert=4;
                break;
                case e_INBits_64:
                    NumberBuff[0]=(Number>>56)&0xFF;
                    NumberBuff[1]=(Number>>48)&0xFF;
                    NumberBuff[2]=(Number>>40)&0xFF;
                    NumberBuff[3]=(Number>>32)&0xFF;
                    NumberBuff[4]=(Number>>24)&0xFF;
                    NumberBuff[5]=(Number>>16)&0xFF;
                    NumberBuff[6]=(Number>>8)&0xFF;
                    NumberBuff[7]=Number&0xFF;
                    Bytes2Insert=8;
                break;
                case e_INBitsMAX:
                default:
                    return;
            }
        break;
        case e_INEndian_Little:
            switch(m_DESB_Bits)
            {
                case e_INBits_8:
                    NumberBuff[0]=Number&0xFF;
                    Bytes2Insert=1;
                break;
                case e_INBits_16:
                    NumberBuff[1]=(Number>>8)&0xFF;
                    NumberBuff[0]=Number&0xFF;
                    Bytes2Insert=2;
                break;
                case e_INBits_32:
                    NumberBuff[3]=(Number>>24)&0xFF;
                    NumberBuff[2]=(Number>>16)&0xFF;
                    NumberBuff[1]=(Number>>8)&0xFF;
                    NumberBuff[0]=Number&0xFF;
                    Bytes2Insert=4;
                break;
                case e_INBits_64:
                    NumberBuff[7]=(Number>>56)&0xFF;
                    NumberBuff[6]=(Number>>48)&0xFF;
                    NumberBuff[5]=(Number>>40)&0xFF;
                    NumberBuff[4]=(Number>>32)&0xFF;
                    NumberBuff[3]=(Number>>24)&0xFF;
                    NumberBuff[2]=(Number>>16)&0xFF;
                    NumberBuff[1]=(Number>>8)&0xFF;
                    NumberBuff[0]=Number&0xFF;
                    Bytes2Insert=8;
                break;
                case e_INBitsMAX:
                default:
                    return;
            }
        break;
        case e_INEndianMAX:
        default:
            return;
    }
    m_DESB_HexDisplay->FillWithValue(InsertPos,NumberBuff,Bytes2Insert,
            DoReplace);
    m_DESB_HexDisplay->GiveFocus();
}

/*******************************************************************************
 * NAME:
 *    DESB_DoInsertText
 *
 * SYNOPSIS:
 *    static void DESB_DoInsertText(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function does the insert text button press.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DESB_DoInsertText(void)
{
    const uint8_t *StartOfBuffPtr;
    int BuffSize;
    int InsertPos;
    const uint8_t *SelStartPtr;
    const uint8_t *SelEndPtr;
    bool DoReplace;
    string Text;
    t_UITextInputCtrl *InsertTxt;
    unsigned int MaxLen;
    unsigned int Len;

    if(m_DESB_HexDisplay->GetSelectionBounds(&SelStartPtr,&SelEndPtr))
    {
        /* Replacing the selection */
        MaxLen=SelEndPtr-SelStartPtr;
        if(!m_DESB_HexDisplay->GetBufferInfo(&StartOfBuffPtr,&BuffSize))
            return;
        InsertPos=SelStartPtr-StartOfBuffPtr;
        DoReplace=true;

        /* Fill with zero's so if it's short it will be padded with 0's */
        m_DESB_HexDisplay->FillSelectionWithValue(0x00);
    }
    else
    {
        /* Inserting the values */
        MaxLen=~0;
        InsertPos=m_DESB_HexDisplay->GetCursorPos();
        DoReplace=false;
    }

    InsertTxt=UIESB_GetTextInput(e_ESB_TextInput_InsertTxt);
    UIGetTextCtrlText(InsertTxt,Text);
    Len=MaxLen;
    if(Text.length()<Len)
        Len=Text.length();

    m_DESB_HexDisplay->FillWithValue(InsertPos,(uint8_t *)Text.c_str(),
            Len,DoReplace);
    m_DESB_HexDisplay->GiveFocus();
}

/*******************************************************************************
 * NAME:
 *    DESB_DoInsertCRC
 *
 * SYNOPSIS:
 *    static void DESB_DoInsertCRC(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function does the insert CRC button.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DESB_DoInsertCRC(void)
{
    const uint8_t *SelStartPtr;
    const uint8_t *SelEndPtr;
    int Byte;
    uint8_t NumberBuff[8];
    char buff[100];
    int Bytes2Insert;
    t_UITextInputCtrl *CRC;
    int r;
    int InsertPoint;
    bool InsertAfterSelection;
    bool DoingBig;

    CRC=UIESB_GetTextInput(e_ESB_TextInput_CRC);

    InsertAfterSelection=false;
    if(m_DESB_HexDisplay->GetSelectionBounds(&SelStartPtr,&SelEndPtr))
        InsertAfterSelection=true;

    /* CRC input has a hex number in it (or blank) */
    UIGetTextCtrlText(CRC,buff,sizeof(buff));

    /* If blank ignore */
    if(buff[0]==0)
        return;

    /* Ok, we need to convert this into numbers */
    Bytes2Insert=strlen(buff)/2;

    switch(m_DESB_Endian)
    {
        case e_INEndian_Big:
            DoingBig=true;
        break;
        case e_INEndian_Little:
        case e_INEndianMAX:
        default:
            DoingBig=false;
        break;
    }
    /* Work from the back */
    for(r=Bytes2Insert-1;r>=0;r--)
    {
        Byte=strtol(&buff[r*2],NULL,16);
        buff[r*2]=0;
        if(DoingBig)
            NumberBuff[r]=Byte;
        else
            NumberBuff[(Bytes2Insert-1)-r]=Byte;
    }

    InsertPoint=m_DESB_HexDisplay->GetCursorPos();
    if(InsertAfterSelection)
    {
        if(m_DESB_HexDisplay->GetAnchorPos()>InsertPoint)
            InsertPoint=m_DESB_HexDisplay->GetAnchorPos();
        InsertPoint++;
    }

    m_DESB_HexDisplay->FillWithValue(InsertPoint,NumberBuff,Bytes2Insert,false);
    m_DESB_HexDisplay->GiveFocus();
}

/*******************************************************************************
 * NAME:
 *    DESB_DoEndianSwap
 *
 * SYNOPSIS:
 *    static void DESB_DoEndianSwap(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function does the endian swap button.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DESB_DoEndianSwap(void)
{
    const uint8_t *SelStartPtr;
    const uint8_t *SelEndPtr;
    const uint8_t *StartOfBuffPtr;
    int BuffSize;
    int InsertPos;
    uint8_t NumberBuff[8];
    int r;
    int SelSize;

    if(!m_DESB_HexDisplay->GetSelectionBounds(&SelStartPtr,&SelEndPtr))
    {
        /* No selection means nothing to do */
        return;
    }

    SelSize=SelEndPtr-SelStartPtr;
    if(SelSize!=1 && SelSize!=2 && SelSize!=4 && SelSize!=8)
    {
        /* We only support these lengths */
        return;
    }

    /* Reverse things */
    for(r=0;r<SelSize;r++)
        NumberBuff[r]=SelStartPtr[SelSize-1-r];

    /* Replacing the selection */
    if(!m_DESB_HexDisplay->GetBufferInfo(&StartOfBuffPtr,&BuffSize))
        return;
    InsertPos=SelStartPtr-StartOfBuffPtr;

    m_DESB_HexDisplay->FillWithValue(InsertPos,NumberBuff,SelSize,true);
    m_DESB_HexDisplay->GiveFocus();
    m_DESB_HexDisplay->SetSelectionBounds(&StartOfBuffPtr[InsertPos],
            &StartOfBuffPtr[InsertPos+SelSize-1]);
}

/*******************************************************************************
 * NAME:
 *    DESB_DoImportDataFromDisk
 *
 * SYNOPSIS:
 *    static void DESB_DoImportDataFromDisk(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function loads a file from disk and places it into a buffer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DESB_DoImportDataFromDisk(void)
{
    string File;
    string LoadFilename;
    FILE *in;
    unsigned long FileSize;
    uint8_t *NewBuffer;

    if(!UI_LoadFileReq("Import file into buffer",g_Session.SendBufferPath,
            File,"All Files|*\n",0))
    {
        return;
    }

    LoadFilename=UI_ConcatFile2Path(g_Session.SendBufferPath,File);

    in=fopen(LoadFilename.c_str(),"rb");
    if(in==NULL)
    {
        UIAsk("Error","Failed to load file",e_AskBox_Error,e_AskBttns_Ok);
        return;
    }
    fseek(in,0,SEEK_END);
    FileSize=ftell(in);
    fseek(in,0,SEEK_SET);

    /* Clip at 1M */
    if(FileSize>1000000)
    {
        UIAsk("Error","File is too large (1M max)",e_AskBox_Error,
                e_AskBttns_Ok);
        fclose(in);
        return;
    }
    NewBuffer=(uint8_t *)malloc(FileSize);
    if(NewBuffer==NULL)
    {
        UIAsk("Error","Out of memory",e_AskBox_Error,e_AskBttns_Ok);
        fclose(in);
        return;
    }

    if(fread(NewBuffer,FileSize,1,in)!=1)
    {
        UIAsk("Error","Error reading file",e_AskBox_Error,
                e_AskBttns_Ok);
        fclose(in);
        return;
    }

    fclose(in);

    m_DESB_HexDisplay->SetBuffer(NewBuffer,FileSize);
    m_DESB_HexDisplay->RebuildDisplay();

    /* SetBuffer copies the buffer, so we need to free it */
    free(NewBuffer);
}

/*******************************************************************************
 * NAME:
 *    DESB_DoExportData2Disk
 *
 * SYNOPSIS:
 *    static void DESB_DoExportData2Disk(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function does the export buffer to file button.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DESB_DoExportData2Disk(void)
{
    string File;
    string SaveFilename;
    FILE *out;
    const uint8_t *StartOfBuffPtr;
    int BufferSize;

    if(!m_DESB_HexDisplay->GetBufferInfo(&StartOfBuffPtr,&BufferSize))
    {
        UIAsk("Error","Failed to get a copy of the internal buffer",
                e_AskBox_Error,e_AskBttns_Ok);
        return;
    }

    if(!UI_SaveFileReq("Export buffer data into binary file",
            g_Session.SendBufferPath,File,"All Files|*",0))
    {
        return;
    }

    SaveFilename=UI_ConcatFile2Path(g_Session.SendBufferPath,File);

    out=fopen(SaveFilename.c_str(),"wb");
    if(out==NULL)
    {
        UIAsk("Error","Failed to save to file",e_AskBox_Error,e_AskBttns_Ok);
        return;
    }

    if(fwrite(StartOfBuffPtr,BufferSize,1,out)!=1)
    {
        UIAsk("Error","Error saving to file",e_AskBox_Error,
                e_AskBttns_Ok);
        fclose(out);
        return;
    }

    fclose(out);
}

/*******************************************************************************
 * NAME:
 *    DESB_DoInsertFromDisk
 *
 * SYNOPSIS:
 *    static void DESB_DoInsertFromDisk(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function insert a file into the current buffer.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DESB_DoInsertFromDisk(void)
{
    string File;
    string LoadFilename;
    FILE *in;
    unsigned long FileSize;
    uint8_t *NewBuffer;
    int InsertPos;

/* DEBUG PAUL: Change this to it's own session path */
    if(!UI_LoadFileReq("Insert file into buffer",g_Session.SendBufferPath,
            File,"All Files|*\n",0))
    {
        return;
    }

    LoadFilename=UI_ConcatFile2Path(g_Session.SendBufferPath,File);

    in=fopen(LoadFilename.c_str(),"rb");
    if(in==NULL)
    {
        UIAsk("Error","Failed to load file",e_AskBox_Error,e_AskBttns_Ok);
        return;
    }
    fseek(in,0,SEEK_END);
    FileSize=ftell(in);
    fseek(in,0,SEEK_SET);

    /* Clip at 1M */
    if(FileSize>1000000)
    {
        UIAsk("Error","File is too large (1M max)",e_AskBox_Error,
                e_AskBttns_Ok);
        fclose(in);
        return;
    }
    NewBuffer=(uint8_t *)malloc(FileSize);
    if(NewBuffer==NULL)
    {
        UIAsk("Error","Out of memory",e_AskBox_Error,e_AskBttns_Ok);
        fclose(in);
        return;
    }

    if(fread(NewBuffer,FileSize,1,in)!=1)
    {
        UIAsk("Error","Error reading file",e_AskBox_Error,
                e_AskBttns_Ok);
        fclose(in);
        return;
    }

    fclose(in);

    InsertPos=m_DESB_HexDisplay->GetCursorPos();
    m_DESB_HexDisplay->FillWithValue(InsertPos,NewBuffer,FileSize,false);
    m_DESB_HexDisplay->RebuildDisplay();
    m_DESB_HexDisplay->GiveFocus();

    /* FillWithValue copies the buffer, so we need to free it */
    free(NewBuffer);
}

/*******************************************************************************
 * NAME:
 *    DESB_RethinkButtons
 *
 * SYNOPSIS:
 *    static void DESB_RethinkButtons(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function rethinks the button state of all the buttons in the dialog.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DESB_RethinkButtons(void)
{
    t_UIButtonCtrl *Fill;
    t_UIButtonCtrl *EndianSwap;
    t_UIButtonCtrl *InsertCRC;
    t_UIButtonCtrl *InsertAsNumber;
    t_UIButtonCtrl *InsertAsText;
    t_UIButtonCtrl *InsertProperties;
    t_UIContextMenuCtrl *ContextMenu_Copy;
    t_UIContextMenuCtrl *ContextMenu_EndianSwap;

    bool FillEnabled;
    bool EndianSwapEnabled;
    bool InsertCRCEnabled;
    bool SelectionValid;
    bool InsertAsNumberEnabled;
    bool InsertAsTextEnabled;
    bool InsertPropertiesEnabled;
    const uint8_t *StartPtr,*EndPtr;
    int SelSize;

    Fill=UIESB_GetButton(e_ESB_Button_Fill);
    EndianSwap=UIESB_GetButton(e_ESB_Button_EndianSwap);
    InsertCRC=UIESB_GetButton(e_ESB_Button_InsertCRC);
    InsertAsNumber=UIESB_GetButton(e_ESB_Button_InsertAsNumber);
    InsertAsText=UIESB_GetButton(e_ESB_Button_InsertAsText);
    InsertProperties=UIESB_GetButton(e_ESB_Button_InsertProperties);
    ContextMenu_Copy=m_DESB_HexDisplay->GetContextMenuHandle(e_UICTW_ContextMenu_Copy);
    ContextMenu_EndianSwap=m_DESB_HexDisplay->GetContextMenuHandle(e_UICTW_ContextMenu_EndianSwap);

    FillEnabled=false;
    InsertAsNumberEnabled=false;
    InsertAsTextEnabled=true;
    InsertPropertiesEnabled=true;
    EndianSwapEnabled=false;
    InsertCRCEnabled=true;

    SelectionValid=m_DESB_HexDisplay->GetSelectionBounds(&StartPtr,&EndPtr);

    if(SelectionValid)
    {
        SelSize=EndPtr-StartPtr;

        FillEnabled=true;
        InsertAsTextEnabled=true;

        switch(SelSize)
        {
            case 2:
            case 4:
            case 8:
                EndianSwapEnabled=true;
                /* No break here */
            case 1:
                InsertAsNumberEnabled=true;
            break;
            default:
            break;
        }
    }
    else
    {
        InsertCRCEnabled=true;
        InsertAsNumberEnabled=true;
    }

    UIEnableButton(Fill,FillEnabled);
    UIEnableButton(EndianSwap,EndianSwapEnabled);
    UIEnableButton(InsertCRC,InsertCRCEnabled);
    UIEnableButton(InsertAsNumber,InsertAsNumberEnabled);
    UIEnableButton(InsertAsText,InsertAsTextEnabled);
    UIEnableButton(InsertProperties,InsertPropertiesEnabled);

    UIEnableContextMenu(ContextMenu_Copy,SelectionValid);
    UIEnableContextMenu(ContextMenu_EndianSwap,EndianSwapEnabled);
}

/*******************************************************************************
 * NAME:
 *    DESB_DoInsertFromClipboard
 *
 * SYNOPSIS:
 *    static void DESB_DoInsertFromClipboard(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function asks the user what the format of the data in the clip
 *    board is and reads the data in that format and inserts it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DESB_DoInsertFromClipboard(void)
{
    e_PasteDataType DataFormat;
    e_HDBCFormatType ClipFormat;

    DataFormat=RunPasteDataDialog();
    switch(DataFormat)
    {
        case e_PasteData_Text:
            ClipFormat=e_HDBCFormat_AscII;
        break;
        case e_PasteData_HexDump:
            ClipFormat=e_HDBCFormat_Hex;
        break;
        case e_PasteData_Cancel:
        case e_PasteDataMAX:
        default:
        return;
    }

    m_DESB_HexDisplay->DoInsertFromClipboard(ClipFormat);
    m_DESB_HexDisplay->GiveFocus();
}

/*******************************************************************************
 * NAME:
 *    DESB_DoSaveBuffer
 *
 * SYNOPSIS:
 *    static void DESB_DoSaveBuffer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function saves the current buffer and data to the disk.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DESB_DoSaveBuffer(void)
{
    string File;
    string SaveFilename;
    t_UITextInputCtrl *BufferName;
    string Name;
    const uint8_t *EditBuffer;
    int BufferSize;

    if(!UI_SaveFileReq("Save buffer",g_Session.SendBufferPath,File,
            "Buffers|*.buf\nAll Files|*",0))
    {
        return;
    }

    SaveFilename=UI_ConcatFile2Path(g_Session.SendBufferPath,File);

    BufferName=UIESB_GetTextInput(e_ESB_TextInput_BufferName);
    UIGetTextCtrlText(BufferName,Name);

    if(!m_DESB_HexDisplay->GetBufferInfo(&EditBuffer,&BufferSize))
    {
        UIAsk("Error","Failed to get buffer",e_AskBox_Error);
        return;
    }

    SendBuffer::SaveBuffer2File(SaveFilename.c_str(),Name.c_str(),
            EditBuffer,BufferSize);
}

/*******************************************************************************
 * NAME:
 *    DESB_DoLoadBuffer
 *
 * SYNOPSIS:
 *    static void DESB_DoLoadBuffer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function load a buffer from disk.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void DESB_DoLoadBuffer(void)
{
    string File;
    string LoadFilename;
    t_UITextInputCtrl *BufferName;
    char *Name;
    uint8_t *EditBuffer;
    uint32_t BufferSize;

    if(!UI_LoadFileReq("Load buffer",g_Session.SendBufferPath,File,
            "Buffers|*.buf\nAll Files|*",0))
    {
        return;
    }

    LoadFilename=UI_ConcatFile2Path(g_Session.SendBufferPath,File);

    if(SendBuffer::LoadBufferFromFile(LoadFilename.c_str(),&Name,
            &EditBuffer,&BufferSize))
    {
        BufferName=UIESB_GetTextInput(e_ESB_TextInput_BufferName);
        UISetTextCtrlText(BufferName,Name);
        free(Name);

        m_DESB_HexDisplay->SetBuffer(EditBuffer,BufferSize);
        free(EditBuffer);
    }
    m_DESB_HexDisplay->RebuildDisplay();
    DESB_RethinkButtons();
}
