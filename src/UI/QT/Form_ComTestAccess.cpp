/*******************************************************************************
 * FILENAME: Form_ComTestAccess.cpp
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
 *    Paul Hutchinson (20 Sep 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_ComTest.h"
#include "ui_Form_ComTest.h"
#include "UI/UIComTest.h"
#include "main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_ComTest *g_ComTest;

/*******************************************************************************
 * NAME:
 *    UIAlloc_ComTest
 *
 * SYNOPSIS:
 *    bool UIAlloc_ComTest(void);
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
bool UIAlloc_ComTest(void)
{
    try
    {
        g_ComTest=new Form_ComTest(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_ComTest=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_ComTest
 *
 * SYNOPSIS:
 *    bool UIShow_ComTest(void);
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
bool UIShow_ComTest(void)
{
    return g_ComTest->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_ComTest
 *
 * SYNOPSIS:
 *    void UIFree_ComTest(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the dialog allocated with UIAlloc_ComTest()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_ComTest(void)
{
    delete g_ComTest;

    g_ComTest=NULL;
}

/*******************************************************************************
 * NAME:
 *    UICT_GetButton
 *
 * SYNOPSIS:
 *    t_UIButtonCtrl *UICT_GetButton(e_CT_Button bttn);
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
t_UIButtonCtrl *UICT_GetButton(e_CT_Button bttn)
{
    switch(bttn)
    {
        case e_CT_Button_Rescan1:
            return (t_UIButtonCtrl *)g_ComTest->ui->Rescan1_pushButton;
        case e_CT_Button_Rescan2:
            return (t_UIButtonCtrl *)g_ComTest->ui->Rescan2_pushButton;
        case e_CT_Button_Start:
            return (t_UIButtonCtrl *)g_ComTest->ui->Start_pushButton;
        case e_CT_Button_Stop:
            return (t_UIButtonCtrl *)g_ComTest->ui->Stop_pushButton;
        case e_CT_ButtonMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UICT_GetTextInput
 *
 * SYNOPSIS:
 *    t_UITextInputCtrl *UICT_GetTextInput(e_CT_TextInput UIObj);
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
t_UITextInputCtrl *UICT_GetTextInput(e_CT_TextInput UIObj)
{
    switch(UIObj)
    {
        case e_CT_TextInput_Stat_PacketsSent:
            return (t_UITextInputCtrl *)g_ComTest->ui->StatPacketsSent_lineEdit;
        case e_CT_TextInput_Stat_PacketsRecv:
            return (t_UITextInputCtrl *)g_ComTest->ui->StatPacketsRx_lineEdit;
        case e_CT_TextInput_Stat_Bytes:
            return (t_UITextInputCtrl *)g_ComTest->ui->StatBytesSend_lineEdit;
        case e_CT_TextInput_Stat_SendErrors:
            return (t_UITextInputCtrl *)g_ComTest->ui->StatSendErrors_lineEdit;
        case e_CT_TextInput_Stat_SendBusyErrors:
            return (t_UITextInputCtrl *)g_ComTest->ui->StatSendBusyErrors_lineEdit;
//        case e_CT_TextInput_Stat_BytesPerSec:
//            return (t_UITextInputCtrl *)g_ComTest->ui->StatBytesPerSec_lineEdit;
        case e_CT_TextInput_Stat_RxErrors:
            return (t_UITextInputCtrl *)g_ComTest->ui->StatRxErrors_lineEdit;

        case e_CT_TextInputMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UICT_GetOptionsFrameContainer
 *
 * SYNOPSIS:
 *    t_UIContainerCtrl *UICT_GetOptionsFrameContainer(e_CT_Frame UIObj);
 *
 * PARAMETERS:
 *    UIObj [I] -- The frame container to get the handle for
 *
 * FUNCTION:
 *    This function gets the handle for a frame container.
 *
 * RETURNS:
 *    The handle to the frame container or NULL if it's not known.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIContainerCtrl *UICT_GetOptionsFrameContainer(e_CT_Frame UIObj)
{
    switch(UIObj)
    {
        case e_CT_Frame_Connection1:
            return (t_UIContainerCtrl *)g_ComTest->ui->Options1_Layout;
        case e_CT_Frame_Connection2:
            return (t_UIContainerCtrl *)g_ComTest->ui->Options2_Layout;
        case e_CT_Frame_ConnectionMAX:
        default:
            return NULL;
    }
    return NULL;
}


/*******************************************************************************
 * NAME:
 *    UICT_GetComboBoxHandle
 *
 * SYNOPSIS:
 *    t_UIComboBoxCtrl *UICT_GetComboBoxHandle(e_CT_ComboxType UIObj);
 *
 * PARAMETERS:
 *    UIObj [I] -- The combox box to get the handle for
 *
 * FUNCTION:
 *    This function gets the handle for a combox box.
 *
 * RETURNS:
 *    The handle to the combox box or NULL if it's not known.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIComboBoxCtrl *UICT_GetComboBoxHandle(e_CT_ComboxType UIObj)
{
    switch(UIObj)
    {
        case e_CT_Combox_ConnectionList1:
            return (t_UIComboBoxCtrl *)g_ComTest->ui->ConnectionList1_comboBox;
        case e_CT_Combox_ConnectionList2:
            return (t_UIComboBoxCtrl *)g_ComTest->ui->ConnectionList2_comboBox;
        case e_CT_Combox_Pattern:
            return (t_UIComboBoxCtrl *)g_ComTest->ui->Pattern_comboBox;
        case e_CT_ComboxMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UICT_GetCheckboxHandle
 *
 * SYNOPSIS:
 *    t_UIRadioBttnCtrl *UICT_GetCheckboxHandle(e_CT_CheckboxType UIObj);
 *
 * PARAMETERS:
 *    UIObj [I] -- The check box to get the handle for
 *
 * FUNCTION:
 *    This function gets the handle for a check box.
 *
 * RETURNS:
 *    The handle to the check box or NULL if it's not known.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UICheckboxCtrl *UICT_GetCheckboxHandle(e_CT_CheckboxType UIObj)
{
    switch(UIObj)
    {
        case e_CT_Checkbox_Loopback:
            return (t_UICheckboxCtrl *)g_ComTest->ui->Loopback_checkBox;
        case e_CT_Checkbox_FullDuplex:
            return (t_UICheckboxCtrl *)g_ComTest->ui->FullDuplex_checkBox;
        case e_CT_CheckboxMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UICT_GetNumberInputHandle
 *
 * SYNOPSIS:
 *    t_UINumberInput *UICT_GetNumberInputHandle(e_CT_NumberType UIObj);
 *
 * PARAMETERS:
 *    UIObj [I] -- The number input to get the handle for
 *
 * FUNCTION:
 *    This function gets the handle for a number input.
 *
 * RETURNS:
 *    The handle to the number input or NULL if it's not known.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UINumberInput *UICT_GetNumberInputHandle(e_CT_NumberType UIObj)
{
    switch(UIObj)
    {
        case e_CT_Number_PacketLength:
            return (t_UINumberInput *)g_ComTest->ui->PacketLength_spinBox;
        case e_CT_Number_Packets:
            return (t_UINumberInput *)g_ComTest->ui->Packets_spinBox;
        case e_CT_NumberMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UICT_GetDoubleNumberInputHandle
 *
 * SYNOPSIS:
 *    t_UIDoubleInput *UICT_GetDoubleNumberInputHandle(e_CT_DoubleNumberType UIObj);
 *
 * PARAMETERS:
 *    UIObj [I] -- The double number to get the handle for
 *
 * FUNCTION:
 *    This function gets the handle for a double number.
 *
 * RETURNS:
 *    The handle to the double number or NULL if it's not known.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIDoubleInput *UICT_GetDoubleNumberInputHandle(e_CT_DoubleNumberType UIObj)
{
    switch(UIObj)
    {
        case e_CT_DoubleNumber_Delay:
            return (t_UIDoubleInput *)g_ComTest->ui->Delay_doubleSpinBox;
        case e_CT_DoubleNumberMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UICT_GetLabelHandle
 *
 * SYNOPSIS:
 *    t_UILabelCtrl *UICT_GetLabelHandle(e_CT_LabelType UIObj);
 *
 * PARAMETERS:
 *    UIObj [I] -- The label to get the handle for
 *
 * FUNCTION:
 *    This function gets the handle for a label.
 *
 * RETURNS:
 *    The handle to the label or NULL if it's not known.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UILabelCtrl *UICT_GetLabelHandle(e_CT_LabelType UIObj)
{
    switch(UIObj)
    {
        case e_CT_Label_Status:
            return (t_UILabelCtrl *)g_ComTest->ui->Status_label;
        case e_CT_LabelMAX:
        default:
            return NULL;
    }
    return NULL;
}

