#include "Form_SettingsSelectType.h"
#include "UI/UISettingsSelectEditType.h"
#include "main.h"

class Form_SettingsSelectType *g_SettingsSelectTypeDialog;
e_USSET_UserSelectedType g_SettingsSelectTypeBttnSelected;

/*******************************************************************************
 * NAME:
 *    UIAlloc_SettingSelectEditType
 *
 * SYNOPSIS:
 *    bool UIAlloc_SettingSelectEditType(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates the settings select edit type dialog.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool UIAlloc_SettingSelectEditType(void)
{
    try
    {
        g_SettingsSelectTypeDialog=new Form_SettingsSelectType(g_MainApp->activeWindow());
        g_SettingsSelectTypeBttnSelected=e_USSET_UserSelectedMAX;
    }
    catch(...)
    {
        g_SettingsSelectTypeDialog=NULL;
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    UIShow_SettingSelectEditType
 *
 * SYNOPSIS:
 *    e_USSET_UserSelectedType UIShow_SettingSelectEditType(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function shows the settings select edit type dialog that was
 *    allocated.  You can only have 1 dialog active at a time.
 *
 *    This is a blocking call.
 *
 * RETURNS:
 *    e_USSET_UserSelected_Global -- User selected the Global button
 *    e_USSET_UserSelected_Connection -- User selected the Connection button
 *    e_USSET_UserSelected_Cancel -- User selected the Cancel button
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
e_USSET_UserSelectedType UIShow_SettingSelectEditType(void)
{
    g_SettingsSelectTypeDialog->exec();
    return g_SettingsSelectTypeBttnSelected;
}

/*******************************************************************************
 * NAME:
 *    UIFree_SettingSelectEditType
 *
 * SYNOPSIS:
 *    void UIFree_SettingSelectEditType(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function frees the current settings select edit dialog.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void UIFree_SettingSelectEditType(void)
{
    delete g_SettingsSelectTypeDialog;

    g_SettingsSelectTypeDialog=NULL;
}

