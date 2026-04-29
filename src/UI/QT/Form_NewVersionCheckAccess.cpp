/*******************************************************************************
 * FILENAME: Form_NewVersionCheckAccess.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * CREATED BY:
 *    Paul Hutchinson (21 Apr 2026)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "Form_NewVersionCheck.h"
#include "ui_Form_NewVersionCheck.h"
#include "UI/UINewVersionCheck.h"
#include "main.h"
#include <QString>
#include <QLabel>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Form_NewVersionCheck *g_NewVersionCheck;

/*******************************************************************************
 * NAME:
 *    UIAlloc_NewVersionCheck
 *
 * SYNOPSIS:
 *    bool UIAlloc_NewVersionCheck(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates the crc finder dialog.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was a problem allocating the dialog.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIAlloc_NewVersionCheck(void)
{
    try
    {
        g_NewVersionCheck=new Form_NewVersionCheck(g_MainApp->activeWindow());
    }
    catch(...)
    {
        g_NewVersionCheck=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_NewVersionCheck
 *
 * SYNOPSIS:
 *    bool UIShow_NewVersionCheck(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function runs the crc finder dialog.
 *
 * RETURNS:
 *    true -- User pressed ok
 *    false -- User pressed cancel
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIShow_NewVersionCheck(void)
{
    return g_NewVersionCheck->exec();
}

/*******************************************************************************
 * NAME:
 *    UIFree_NewVersionCheck
 *
 * SYNOPSIS:
 *    void UIFree_NewVersionCheck(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the dialog allocated with UIAlloc_NewVersionCheck()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_NewVersionCheck(void)
{
    delete g_NewVersionCheck;

    g_NewVersionCheck=NULL;
}

/*******************************************************************************
 * NAME:
 *    UINVC_GetButtonHandle
 *
 * SYNOPSIS:
 *    t_UIButtonCtrl *UINVC_GetButtonHandle(e_NVC_Label UIObj);
 *
 * PARAMETERS:
 *    UIObj [I] -- The button to get the handle for
 *
 * FUNCTION:
 *    This function gets the handle for a button.
 *
 * RETURNS:
 *    The handle to the button or NULL if it's not known.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_UIButtonCtrl *UINVC_GetButtonHandle(e_NVC_Button UIObj)
{
    switch(UIObj)
    {
        case e_NVC_Button_GotoWebPage:
            return (t_UIButtonCtrl *)g_NewVersionCheck->ui->OpenWebPage_pushButton;
        case e_NVC_ButtonMAX:
        default:
            return NULL;
    }
    return NULL;
}

/*******************************************************************************
 * NAME:
 *    UINVC_GetLabelHandle
 *
 * SYNOPSIS:
 *    t_UILabelCtrl *UINVC_GetLabelHandle(e_NVC_Label UIObj);
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
t_UILabelCtrl *UINVC_GetLabelHandle(e_NVC_Label UIObj)
{
    switch(UIObj)
    {
        case e_NVC_Label_Result:
            return (t_UILabelCtrl *)g_NewVersionCheck->ui->Result_label;
        case e_NVC_LabelMAX:
        default:
            return NULL;
    }
    return NULL;
}


void NVC_SetStepMode(e_VersionCheckStepType Step,e_VersionCheckModeType Mode)
{
    QString NewModeChar;

    NewModeChar=" ";
    switch(Mode)
    {
        case e_VersionCheckMode_Clear:
            NewModeChar=" ";
        break;
        case e_VersionCheckMode_Doing:
            NewModeChar=QChar(0x27A1);
        break;
        case e_VersionCheckMode_Done:
            NewModeChar=QChar(0x2714);
        break;
        case e_VersionCheckModeMAX:
        break;
    }

    switch(Step)
    {
        case e_VersionCheckStep_Connect:
            g_NewVersionCheck->ui->Connect_label->setText(NewModeChar);
        break;
        case e_VersionCheckStep_Reading:
            g_NewVersionCheck->ui->Read_label->setText(NewModeChar);
        break;
        case e_VersionCheckStep_Closing:
            g_NewVersionCheck->ui->Closing_label->setText(NewModeChar);
        break;
        case e_VersionCheckStep_Check:
            g_NewVersionCheck->ui->Checking_label->setText(NewModeChar);
        break;
        case e_VersionCheckStepMAX:
        default:
        break;
    }
}

void NVC_ShowHideNewVersionFound(bool Show)
{
    g_NewVersionCheck->ui->NewVersionAvailable_widget->setVisible(Show);
}

void NVC_EnableOkBttn(bool Enable)
{
    QPushButton *okButton;

    okButton = g_NewVersionCheck->ui->buttonBox->button(QDialogButtonBox::Ok);

    okButton->setEnabled(Enable);
}


