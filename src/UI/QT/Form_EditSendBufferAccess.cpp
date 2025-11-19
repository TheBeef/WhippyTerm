/*******************************************************************************
 * FILENAME: Form_EditSendBufferAccess.cpp
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
#include "Form_EditSendBuffer.h"
#include "ui_Form_EditSendBuffer.h"
#include "UI/UIEditSendBuffer.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_EditSendBuffer *g_EditSendBuffer;

/*******************************************************************************
 * NAME:
 *    UIAlloc_EditSendBuffer
 *
 * SYNOPSIS:
 *    bool UIAlloc_EditSendBuffer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates the hex dispaly copy as dialog.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was a problem allocating the dialog.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIAlloc_EditSendBuffer(void)
{
    try
    {
        g_EditSendBuffer=new Form_EditSendBuffer(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_EditSendBuffer=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_EditSendBuffer
 *
 * SYNOPSIS:
 *    bool UIShow_EditSendBuffer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function runs the hex display copy as dialog.
 *
 * RETURNS:
 *    true -- User pressed ok
 *    false -- User pressed cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIShow_EditSendBuffer(void)
{
    return g_EditSendBuffer->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_EditSendBuffer
 *
 * SYNOPSIS:
 *    void UIFree_EditSendBuffer(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the dialog allocated with UIAlloc_EditSendBuffer()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_EditSendBuffer(void)
{
    delete g_EditSendBuffer;

    g_EditSendBuffer=NULL;
}

/*******************************************************************************
 * NAME:
 *    UIESB_GetButton
 *
 * SYNOPSIS:
 *    t_UIButtonCtrl *UIESB_GetButton(e_ESB_Button bttn);
 *
 * PARAMETERS:
 *    bttn [I] -- What button to get the handle for
 *
 * FUNCTION:
 *    This function gets the handle for a button in the dialog.
 *
 * RETURNS:
 *    A handle to the buffer or NULL if it's unknown
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIButtonCtrl *UIESB_GetButton(e_ESB_Button bttn)
{
    switch(bttn)
    {
        case e_ESB_Button_Clear:
            return (t_UIButtonCtrl *)g_EditSendBuffer->ui->Clear_pushButton;
        case e_ESB_Button_Fill:
            return (t_UIButtonCtrl *)g_EditSendBuffer->ui->Fill_pushButton;
        case e_ESB_Button_EndianSwap:
            return (t_UIButtonCtrl *)g_EditSendBuffer->ui->EndianSwap_pushButton;
        case e_ESB_Button_CRCType:
            return (t_UIButtonCtrl *)g_EditSendBuffer->ui->CRCType_pushButton;
        case e_ESB_Button_InsertCRC:
            return (t_UIButtonCtrl *)g_EditSendBuffer->ui->InsertCRC_pushButton;
        case e_ESB_Button_InsertAsNumber:
            return (t_UIButtonCtrl *)g_EditSendBuffer->ui->InsertAsNumber_pushButton;
        case e_ESB_Button_InsertAsText:
            return (t_UIButtonCtrl *)g_EditSendBuffer->ui->InsertAsText_pushButton;
        case e_ESB_Button_InsertProperties:
            return (t_UIButtonCtrl *)g_EditSendBuffer->ui->InsertProperties_pushButton;
        case e_ESB_ButtonMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UIESB_GetTextInput
 *
 * SYNOPSIS:
 *    t_UITextInputCtrl *UIESB_GetTextInput(e_ESB_TextInput UIObj);
 *
 * PARAMETERS:
 *    UIObj [I] -- The text input to get the handle for
 *
 * FUNCTION:
 *    This function gets the handle for a text input.
 *
 * RETURNS:
 *    The handle to the text input or NULL if it's not known.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UITextInputCtrl *UIESB_GetTextInput(e_ESB_TextInput UIObj)
{
    switch(UIObj)
    {
        case e_ESB_TextInput_SelectedBytes:
            return (t_UITextInputCtrl *)g_EditSendBuffer->ui->SelectedBytes_lineEdit;
        case e_ESB_TextInput_Offset:
            return (t_UITextInputCtrl *)g_EditSendBuffer->ui->Offset_lineEdit;
        case e_ESB_TextInput_CRC:
            return (t_UITextInputCtrl *)g_EditSendBuffer->ui->CRC_lineEdit;
        case e_ESB_TextInput_BufferSize:
            return (t_UITextInputCtrl *)g_EditSendBuffer->ui->BufferSize_lineEdit;
        case e_ESB_TextInput_InsertTxt:
            return (t_UITextInputCtrl *)g_EditSendBuffer->ui->InsertTxt_lineEdit;
        case e_ESB_TextInput_BufferName:
            return (t_UITextInputCtrl *)g_EditSendBuffer->ui->BufferName_lineEdit;
        case e_ESB_TextInputMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UIESB_GetContextMenu
 *
 * SYNOPSIS:
 *    t_UIContextMenuCtrl *UIESB_GetContextMenu(e_ESB_ContextMenu UIObj);
 *
 * PARAMETERS:
 *    UIObj [I] -- The context menu to get the handle for
 *
 * FUNCTION:
 *    This function gets the handle for a context menu.
 *
 * RETURNS:
 *    The handle to the context menu or NULL if it's not known.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIContextMenuCtrl *UIESB_GetContextMenu(e_ESB_ContextMenu UIObj)
{
    switch(UIObj)
    {
        case e_ESB_ContextMenu_LoadBuffer:
            return (t_UIContextMenuCtrl *)g_EditSendBuffer->LoadBufferMenu;
        case e_ESB_ContextMenu_SaveBuffer:
            return (t_UIContextMenuCtrl *)g_EditSendBuffer->SaveBufferMenu;
        case e_ESB_ContextMenu_ExportData:
            return (t_UIContextMenuCtrl *)g_EditSendBuffer->ImportDataMenu;
        case e_ESB_ContextMenu_ImportData:
            return (t_UIContextMenuCtrl *)g_EditSendBuffer->ExportDataMenu;
        case e_ESB_ContextMenu_InsertFromDisk:
            return (t_UIContextMenuCtrl *)g_EditSendBuffer->InsertFromDiskMenu;
        case e_ESB_ContextMenu_InsertFromClipboard:
            return (t_UIContextMenuCtrl *)g_EditSendBuffer->InsertFromClipboardMenu;
        case e_ESB_ContextMenuMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UIESB_GetHexContainerFrame
 *
 * SYNOPSIS:
 *    t_UIFrameContainerCtrl *UIESB_GetHexContainerFrame(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the frame handle for the hex display.
 *
 * RETURNS:
 *    A pointer to the hex display frame.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIFrameContainerCtrl *UIESB_GetHexContainerFrame(void)
{
    return (t_UIFrameContainerCtrl *)g_EditSendBuffer->ui->Hex_frame;
}

