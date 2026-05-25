/*******************************************************************************
 * FILENAME: PluginUISupport.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has support for plugin's access to the UI.
 *
 *    Each sub system that uses the UI provides a widget handle
 *    that it can store extra widget info in.  However not every sub system
 *    needs to provide every function, it can just use these default functions.
 *
 *    If the sub system doesn't need it's own data then it can just
 *    use these functions using 't_WidgetSysHandle' as a
 *    't_UILayoutContainerCtrl'.
 *
 * COPYRIGHT:
 *    Copyright 2018 Paul Hutchinson.
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
 *    Paul Hutchinson (27 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "App/MainApp.h"
#include "App/PluginSupport/PluginUISupport.h"
#include "App/Settings.h"
#include "UI/UIAsk.h"
#include "UI/UIPlugins.h"
#include "UI/UIControl.h"
#include "UI/UIFileReq.h"
#include <string.h>
#include <stdlib.h>
#include <string>

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
struct PI_UIAPI m_PIUSDefault_UIAPI=
{
    PIUSDefault_AddComboBox,
    PIUSDefault_FreeComboBox,
    PIUSDefault_ClearComboBox,
    PIUSDefault_AddItem2ComboBox,
    PIUSDefault_SetComboBoxSelectedEntry,
    PIUSDefault_GetComboBoxSelectedEntry,
    PIUSDefault_GetComboBoxText,
    PIUSDefault_SetComboBoxText,
    PIUSDefault_EnableComboBox,

    PIUSDefault_AllocRadioBttnGroup,
    PIUSDefault_FreeRadioBttnGroup,
    PIUSDefault_AddRadioBttn,
    PIUSDefault_FreeRadioBttn,
    PIUSDefault_IsRadioBttnChecked,
    PIUSDefault_SetRadioBttnChecked,
    PIUSDefault_EnableRadioBttn,

    PIUSDefault_AddCheckbox,
    PIUSDefault_FreeCheckbox,
    PIUSDefault_IsCheckboxChecked,
    PIUSDefault_SetCheckboxChecked,
    PIUSDefault_EnableCheckbox,

    PIUSDefault_AddTextInput,
    PIUSDefault_FreeTextInput,
    PIUSDefault_GetTextInputText,
    PIUSDefault_SetTextInputText,
    PIUSDefault_EnableTextInput,

    PIUSDefault_AddNumberInput,
    PIUSDefault_FreeNumberInput,
    PIUSDefault_GetNumberInputValue,
    PIUSDefault_SetNumberInputValue,
    PIUSDefault_SetNumberInputMinMax,
    PIUSDefault_EnableNumberInput,

    PIUSDefault_AddDoubleInput,
    PIUSDefault_FreeDoubleInput,
    PIUSDefault_GetDoubleInputValue,
    PIUSDefault_SetDoubleInputValue,
    PIUSDefault_SetDoubleInputMinMax,
    PIUSDefault_SetDoubleInputDecimals,
    PIUSDefault_EnableDoubleInput,

    PIUSDefault_AddColumnViewInput,
    PIUSDefault_FreeColumnViewInput,
    PIUSDefault_ColumnViewInputClear,
    PIUSDefault_ColumnViewInputRemoveRow,
    PIUSDefault_ColumnViewInputAddRow,
    PIUSDefault_ColumnViewInputSetColumnText,
    PIUSDefault_ColumnViewInputSelectRow,
    PIUSDefault_ColumnViewInputClearSelection,

    PIUSDefault_AddButtonInput,
    PIUSDefault_FreeButtonInput,

    PIUSDefault_AddIndicator,
    PIUSDefault_FreeIndicator,
    PIUSDefault_SetIndicator,

    PIUSDefault_Ask,

    /* Version 2 */
    PIUSDefault_FileReq,
    PIUSDefault_FreeFileReqPathAndFile,

    PIUSDefault_AddTextBox,
    PIUSDefault_FreeTextBox,
    PIUSDefault_SetTextBox,

    PIUSDefault_AddGroupBox,
    PIUSDefault_FreeGroupBox,
    PIUSDefault_SetGroupBoxLabel,

    PIUSDefault_AddColorPick,
    PIUSDefault_FreeColorPick,
    PIUSDefault_GetColorPickValue,
    PIUSDefault_SetColorPickValue,

    /* Version 3 */
    PIUSDefault_AddStylePick,
    PIUSDefault_FreeStylePick,
    PIUSDefault_GetStylePickValue,
    PIUSDefault_SetStylePickValue,
    PIUSDefault_Style2StrHelper,
    PIUSDefault_Str2StyleHelper,

    PIUSDefault_ChangeTextBoxProp,
    PIUSDefault_ChangeComboBoxProp,
    PIUSDefault_ChangeRadioBttnProp,
    PIUSDefault_ChangeCheckboxProp,
    PIUSDefault_ChangeTextInputProp,
    PIUSDefault_ChangeNumberInputProp,
    PIUSDefault_ChangeDoubleInputProp,
    PIUSDefault_ChangeColumnViewInputProp,
    PIUSDefault_ChangeButtonInputProp,
    PIUSDefault_ChangeIndicatorProp,
    PIUSDefault_ChangeGroupBoxProp,
    PIUSDefault_ChangeColorPickProp,
    PIUSDefault_ChangeStylePickProp,
    PIUSDefault_ChangeWebLinkProp,

    PIUSDefault_SetLabelText,

    PIUSDefault_AddWebLink,
    PIUSDefault_FreeWebLink,
    PIUSDefault_SetWebLinkURL,
};

/*******************************************************************************
 * NAME:
 *    PIUSDefault_GetDefaultAPI
 *
 * SYNOPSIS:
 *    const struct PI_UIAPI *PIUSDefault_GetDefaultAPI(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the system default plugin UI 'PI_UIAPI' structure.
 *    You use this if us don't want to override any of the UI functions.
 *
 * RETURNS:
 *    A pointer to the default 'PI_UIAPI' structure.
 *
 * NOTES:
 *    If you use this then you must have your 't_WidgetSysHandle'
 *    be the container widget (t_UILayoutContainerCtrl) that will be passed
 *    directly to the UI.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
const struct PI_UIAPI *PIUSDefault_GetDefaultAPI(void)
{
    return &m_PIUSDefault_UIAPI;
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_AddComboBox
 *
 * SYNOPSIS:
 *    struct PI_ComboBox *PIUSDefault_AddComboBox(t_WidgetSysHandle
 *            *WidgetHandle,PG_BOOL UserEditable,const char *Label,
 *            void (*EventCB)(const struct PICBEvent *Event,void *UserData),
 *            void *UserData);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UserEditable [I] -- true to let the user type a free-form value in
 *                        addition to selecting from the drop-down list;
 *                        false to lock the selection to the listed items.
 *    Label [I] -- The text label to display next to the widget.
 *    EventCB [I] -- Callback that fires when the widget generates an event. The
 *                   event structure is a 'struct PICBEvent'. May be NULL if
 *                   events are not needed.
 *    UserData [I] -- Pointer passed to 'EventCB'.
 *
 * FUNCTION:
 *    Adds a new combo box (drop-down list) to the subsystem's container.
 *
 * RETURNS:
 *    A handle to the new widget on success, or NULL if the widget could not be
 *    allocated.
 *
 * SEE ALSO:
 *    PIUSDefault_FreeComboBox(), PIUSDefault_AddItem2ComboBox(),
 *    PIUSDefault_ClearComboBox()
 ******************************************************************************/
struct PI_ComboBox *PIUSDefault_AddComboBox(t_WidgetSysHandle *WidgetHandle,
        PG_BOOL UserEditable,const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData)
{
    return UIPI_AddComboBox((t_UILayoutContainerCtrl *)WidgetHandle,
            UserEditable,Label,EventCB,UserData);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_FreeComboBox
 *
 * SYNOPSIS:
 *    void PIUSDefault_FreeComboBox(t_WidgetSysHandle *WidgetHandle,
 *            struct PI_ComboBox *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The combo box returned by PIUSDefault_AddComboBox().
 *
 * FUNCTION:
 *    Destroys a combo box and releases every resource associated with it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddComboBox()
 ******************************************************************************/
void PIUSDefault_FreeComboBox(t_WidgetSysHandle *WidgetHandle,
        struct PI_ComboBox *UICtrl)
{
    UIPI_FreeComboBox(UICtrl);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ClearComboBox
 *
 * SYNOPSIS:
 *    void PIUSDefault_ClearComboBox(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUIComboBoxCtrl *ComboBox);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    ComboBox [I] -- The combo box whose items are being removed.
 *
 * FUNCTION:
 *    Removes every entry from the combo box's drop-down list, leaving it empty.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddItem2ComboBox()
 ******************************************************************************/
void PIUSDefault_ClearComboBox(t_WidgetSysHandle *WidgetHandle,
        t_PIUIComboBoxCtrl *ComboBox)
{
    t_UIComboBoxCtrl *RealComboBox=(t_UIComboBoxCtrl *)ComboBox;
    UIClearComboBox(RealComboBox);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_AddItem2ComboBox
 *
 * SYNOPSIS:
 *    void PIUSDefault_AddItem2ComboBox(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUIComboBoxCtrl *ComboBox,const char *Label,uintptr_t ID);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    ComboBox [I] -- The combo box to add the entry to.
 *    Label [I] -- The text to display for the new entry.
 *    ID [I] -- A caller-defined identifier for this entry.
 *
 * FUNCTION:
 *    Appends a new entry to the bottom of the combo box's drop-down list.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_ClearComboBox(), PIUSDefault_SetComboBoxSelectedEntry(),
 *    PIUSDefault_GetComboBoxSelectedEntry()
 ******************************************************************************/
void PIUSDefault_AddItem2ComboBox(t_WidgetSysHandle *WidgetHandle,
        t_PIUIComboBoxCtrl *ComboBox,const char *Label,uintptr_t ID)
{
    t_UIComboBoxCtrl *RealComboBox=(t_UIComboBoxCtrl *)ComboBox;
    UIAddItem2ComboBox(RealComboBox,Label,ID);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_SetComboBoxSelectedEntry
 *
 * SYNOPSIS:
 *    void PIUSDefault_SetComboBoxSelectedEntry(t_WidgetSysHandle
 *            *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox,uintptr_t ID);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    ComboBox [I] -- The combo box whose selection is being changed.
 *    ID [I] -- The ID (as passed to AddItem2ComboBox()) of the entry to select.
 *
 * FUNCTION:
 *    Makes the entry with the given ID the currently selected entry in the
 *    combo box.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddItem2ComboBox(), PIUSDefault_GetComboBoxSelectedEntry()
 ******************************************************************************/
void PIUSDefault_SetComboBoxSelectedEntry(t_WidgetSysHandle *WidgetHandle,
        t_PIUIComboBoxCtrl *ComboBox,uintptr_t ID)
{
    t_UIComboBoxCtrl *RealComboBox=(t_UIComboBoxCtrl *)ComboBox;
    UISetComboBoxSelectedEntry(RealComboBox,ID);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_GetComboBoxSelectedEntry
 *
 * SYNOPSIS:
 *    uintptr_t PIUSDefault_GetComboBoxSelectedEntry(t_WidgetSysHandle
 *            *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    ComboBox [I] -- The combo box being queried.
 *
 * FUNCTION:
 *    Returns the ID of the entry that is currently selected in the combo box.
 *
 * RETURNS:
 *    The ID of the selected entry, as supplied when the entry was added with
 *    AddItem2ComboBox().
 *
 * SEE ALSO:
 *    PIUSDefault_SetComboBoxSelectedEntry(), PIUSDefault_AddItem2ComboBox()
 ******************************************************************************/
uintptr_t PIUSDefault_GetComboBoxSelectedEntry(t_WidgetSysHandle *WidgetHandle,
        t_PIUIComboBoxCtrl *ComboBox)
{
    t_UIComboBoxCtrl *RealComboBox=(t_UIComboBoxCtrl *)ComboBox;
    return UIGetComboBoxSelectedEntry(RealComboBox);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_EnableComboBox
 *
 * SYNOPSIS:
 *    void PIUSDefault_EnableComboBox(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUIComboBoxCtrl *ComboBox,PG_BOOL Enabled);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    ComboBox [I] -- The combo box being enabled or disabled.
 *    Enabled [I] -- true to make the combo box respond to user input,
 *                   false to grey it out and ignore input.
 *
 * FUNCTION:
 *    Enables or disables user interaction with the combo box.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddComboBox()
 ******************************************************************************/
void PIUSDefault_EnableComboBox(t_WidgetSysHandle *WidgetHandle,
        t_PIUIComboBoxCtrl *ComboBox,PG_BOOL Enabled)
{
    t_UIComboBoxCtrl *RealComboBox=(t_UIComboBoxCtrl *)ComboBox;
    UIEnableComboBox(RealComboBox,Enabled);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_AllocRadioBttnGroup
 *
 * SYNOPSIS:
 *    struct PI_RadioBttnGroup *PIUSDefault_AllocRadioBttnGroup(
 *            t_WidgetSysHandle *WidgetHandle,const char *Label);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Label [I] -- The text label to display next to the widget.
 *
 * FUNCTION:
 *    Allocates a new radio-button group.  A group is the container into which
 *    individual radio buttons are added with PIUSDefault_AddRadioBttn().
 *
 * RETURNS:
 *    A handle to the new widget on success, or NULL if the widget could not be
 *    allocated.
 *
 * SEE ALSO:
 *    PIUSDefault_FreeRadioBttnGroup(), PIUSDefault_AddRadioBttn()
 ******************************************************************************/
struct PI_RadioBttnGroup *PIUSDefault_AllocRadioBttnGroup(t_WidgetSysHandle *WidgetHandle,
        const char *Label)
{
    return UIPI_AllocRadioBttnGroup((t_UILayoutContainerCtrl *)WidgetHandle,
            Label);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_FreeRadioBttnGroup
 *
 * SYNOPSIS:
 *    static void PIUSDefault_FreeRadioBttnGroup(t_WidgetSysHandle *WidgetHandle,
 *              struct PI_RadioBttnGroup *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The widget to free.
 *
 * FUNCTION:
 *    This function frees a group box of radio buttons.  You must free the
 *    radio button BEFORE calling this function.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AllocRadioBttnGroup()
 ******************************************************************************/
void PIUSDefault_FreeRadioBttnGroup(t_WidgetSysHandle *WidgetHandle,
        struct PI_RadioBttnGroup *UICtrl)
{
    UIPI_FreeRadioBttnGroup(UICtrl);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_AddRadioBttn
 *
 * SYNOPSIS:
 *    struct PI_RadioBttn *PIUSDefault_AddRadioBttn(t_WidgetSysHandle
 *            *WidgetHandle,struct PI_RadioBttnGroup *RBGroup,
 *            const char *Label,
 *            void (*EventCB)(const struct PIRBEvent *Event,void *UserData),
 *            void *UserData);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    RBGroup [I] -- The radio-button group (from
 *                   PIUSDefault_AllocRadioBttnGroup()) that this button will
 *                   belong to.
 *    Label [I] -- The text label to display next to the widget.
 *    EventCB [I] -- Callback that fires when the widget generates an event. The
 *                   event structure is a 'struct PIRBEvent'. May be NULL if
 *                   events are not needed.
 *    UserData [I] -- Pointer passed through to 'EventCB'.
 *
 * FUNCTION:
 *    Adds a new radio button to an existing radio-button group.
 *
 * RETURNS:
 *    A handle to the new widget on success, or NULL if the widget could not be
 *    allocated.
 *
 * SEE ALSO:
 *    PIUSDefault_AllocRadioBttnGroup(), PIUSDefault_FreeRadioBttn(),
 *    PIUSDefault_SetRadioBttnChecked(), PIUSDefault_IsRadioBttnChecked(),
 *    PIUSDefault_SetRadioBttnChecked(), PIUSDefault_EnableRadioBttn()
 ******************************************************************************/
struct PI_RadioBttn *PIUSDefault_AddRadioBttn(t_WidgetSysHandle *WidgetHandle,
        struct PI_RadioBttnGroup *RBGroup,const char *Label,
        void (*EventCB)(const struct PIRBEvent *Event,void *UserData),
        void *UserData)
{
    return UIPI_AddRadioBttn(RBGroup,Label,EventCB,UserData);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_FreeRadioBttn
 *
 * SYNOPSIS:
 *    void PIUSDefault_FreeRadioBttn(t_WidgetSysHandle *WidgetHandle,
 *            struct PI_RadioBttn *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The radio button returned by PIUSDefault_AddRadioBttn().
 *
 * FUNCTION:
 *    Destroys a radio button and releases every resource associated with it.
 *    Does not free the containing group.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddRadioBttn(), PIUSDefault_FreeRadioBttnGroup()
 ******************************************************************************/
void PIUSDefault_FreeRadioBttn(t_WidgetSysHandle *WidgetHandle,struct PI_RadioBttn *UICtrl)
{
    UIPI_FreeRadioBttn(UICtrl);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_IsRadioBttnChecked
 *
 * SYNOPSIS:
 *    PG_BOOL PIUSDefault_IsRadioBttnChecked(t_WidgetSysHandle
 *            *WidgetHandle,struct PI_RadioBttn *Bttn);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Bttn [I] -- The radio button being queried.
 *
 * FUNCTION:
 *    Returns whether the given radio button is the currently-checked one in its
 *    group.
 *
 * RETURNS:
 *    true -- The button is checked.
 *    false -- The button is not checked.
 *
 * SEE ALSO:
 *    PIUSDefault_SetRadioBttnChecked(), PIUSDefault_AddRadioBttn()
 ******************************************************************************/
PG_BOOL PIUSDefault_IsRadioBttnChecked(t_WidgetSysHandle *WidgetHandle,
        struct PI_RadioBttn *Bttn)
{
    return UIPI_IsRadioBttnChecked(Bttn);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_SetRadioBttnChecked
 *
 * SYNOPSIS:
 *    void PIUSDefault_SetRadioBttnChecked(t_WidgetSysHandle *WidgetHandle,
 *            struct PI_RadioBttn *Bttn,PG_BOOL Checked);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Bttn [I] -- The radio button being modified.
 *    Checked [I] -- true to check this button, false to clear it.
 *
 * FUNCTION:
 *    Sets the checked state of a radio button.  Because radio buttons are
 *    mutually exclusive within a group, checking one button will normally
 *    uncheck whichever other button in the group was previously checked.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_IsRadioBttnChecked(), PIUSDefault_AddRadioBttn()
 ******************************************************************************/
void PIUSDefault_SetRadioBttnChecked(t_WidgetSysHandle *WidgetHandle,
        struct PI_RadioBttn *Bttn,PG_BOOL Checked)
{
    UIPI_SetRadioBttnChecked(Bttn,Checked);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_EnableRadioBttn
 *
 * SYNOPSIS:
 *    void PIUSDefault_EnableRadioBttn(t_WidgetSysHandle *WidgetHandle,
 *            struct PI_RadioBttn *Bttn,PG_BOOL Enabled);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Bttn [I] -- The radio button being enabled or disabled.
 *    Enabled [I] -- true to make the button respond to user input; false
 *                   to grey it out and ignore input.
 *
 * FUNCTION:
 *    Enables or disables user interaction with a single radio button. The other
 *    buttons in the same group are unaffected.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddRadioBttn()
 ******************************************************************************/
void PIUSDefault_EnableRadioBttn(t_WidgetSysHandle *WidgetHandle,
        struct PI_RadioBttn *Bttn,PG_BOOL Enabled)
{
    UIPI_EnableRadioBttn(Bttn,Enabled);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_GetComboBoxText
 *
 * SYNOPSIS:
 *    const char *PIUSDefault_GetComboBoxText(t_WidgetSysHandle
 *            *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    ComboBox [I] -- The combo box being read.
 *
 * FUNCTION:
 *    Returns the text currently shown in the combo box's edit area. For an
 *    editable combo box this may be a value the user typed rather than one of
 *    the listed entries.
 *
 * RETURNS:
 *    A pointer to a NUL-terminated string holding the current text. The buffer
 *    is owned by this function and is valid only until the next call to
 *    PIUSDefault_GetComboBoxText().
 *
 * SEE ALSO:
 *    PIUSDefault_SetComboBoxText(), PIUSDefault_AddComboBox()
 ******************************************************************************/
const char *PIUSDefault_GetComboBoxText(t_WidgetSysHandle *WidgetHandle,
        t_PIUIComboBoxCtrl *ComboBox)
{
    t_UIComboBoxCtrl *RealComboBox=(t_UIComboBoxCtrl *)ComboBox;
    static string ReturnTxt;

    UIGetComboBoxText(RealComboBox,ReturnTxt);

    return ReturnTxt.c_str();
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_SetComboBoxText
 *
 * SYNOPSIS:
 *    void PIUSDefault_SetComboBoxText(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUIComboBoxCtrl *ComboBox,const char *Txt);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    ComboBox [I] -- The combo box whose text is being set.
 *    Txt [I] -- The new text. Must be NUL-terminated.
 *
 * FUNCTION:
 *    Replaces the text currently shown in the combo box's edit area. Useful for
 *    editable combo boxes.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_GetComboBoxText()
 ******************************************************************************/
void PIUSDefault_SetComboBoxText(t_WidgetSysHandle *WidgetHandle,
        t_PIUIComboBoxCtrl *ComboBox,const char *Txt)
{
    t_UIComboBoxCtrl *RealComboBox=(t_UIComboBoxCtrl *)ComboBox;

    UISetComboBoxText(RealComboBox,Txt);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_AddCheckbox
 *
 * SYNOPSIS:
 *    struct PI_Checkbox *PIUSDefault_AddCheckbox(t_WidgetSysHandle
 *            *WidgetHandle,const char *Label,
 *            void (*EventCB)(const struct PICheckboxEvent *Event,
 *                    void *UserData),
 *            void *UserData);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Label [I] -- The text label to display next to the widget.
 *    EventCB [I] -- Callback that fires when the widget generates an event. The
 *                   event structure is a 'struct PICheckboxEvent'. May be NULL
 *                   if events are not needed.
 *    UserData [I] -- Pointer passed through to 'EventCB'
 *
 * FUNCTION:
 *    Adds a new checkbox to the subsystem's container.
 *
 * RETURNS:
 *    A handle to the new widget on success, or NULL if the widget could not be
 *    allocated.
 *
 * SEE ALSO:
 *    PIUSDefault_FreeCheckbox(), PIUSDefault_IsCheckboxChecked(),
 *    PIUSDefault_SetCheckboxChecked()
 ******************************************************************************/
struct PI_Checkbox *PIUSDefault_AddCheckbox(t_WidgetSysHandle *WidgetHandle,
        const char *Label,void (*EventCB)(const struct PICheckboxEvent *Event,
        void *UserData),void *UserData)
{
    return UIPI_AddCheckbox((t_UILayoutContainerCtrl *)WidgetHandle,Label,EventCB,UserData);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_FreeCheckbox
 *
 * SYNOPSIS:
 *    void PIUSDefault_FreeCheckbox(t_WidgetSysHandle *WidgetHandle,
 *            struct PI_Checkbox *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The checkbox returned by AddCheckbox(). After this call the
 *                  handle is no longer valid.
 *
 * FUNCTION:
 *    Destroys a checkbox and releases every resource associated with it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddCheckbox()
 ******************************************************************************/
void PIUSDefault_FreeCheckbox(t_WidgetSysHandle *WidgetHandle,struct PI_Checkbox *UICtrl)
{
    UIPI_FreeCheckbox(UICtrl);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_IsCheckboxChecked
 *
 * SYNOPSIS:
 *    PG_BOOL PIUSDefault_IsCheckboxChecked(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUICheckboxCtrl *Bttn);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Bttn [I] -- The checkbox being queried.
 *
 * FUNCTION:
 *    Returns whether the checkbox is currently checked or not.
 *
 * RETURNS:
 *    true -- The checkbox is checked.
 *    false -- The checkbox is not checked.
 *
 * SEE ALSO:
 *    PIUSDefault_SetCheckboxChecked(), PIUSDefault_AddCheckbox()
 ******************************************************************************/
PG_BOOL PIUSDefault_IsCheckboxChecked(t_WidgetSysHandle *WidgetHandle,
        t_PIUICheckboxCtrl *Bttn)
{
    t_UICheckboxCtrl *RealCheckbox=(t_UICheckboxCtrl *)Bttn;

    return UIGetCheckboxCheckStatus(RealCheckbox);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_SetCheckboxChecked
 *
 * SYNOPSIS:
 *    void PIUSDefault_SetCheckboxChecked(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUICheckboxCtrl *Bttn,PG_BOOL Checked);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Bttn [I] -- The checkbox being modified.
 *    Checked [I] -- true to check the box, false to clear it.
 *
 * FUNCTION:
 *    Sets the checked state of the checkbox.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_IsCheckboxChecked(), PIUSDefault_AddCheckbox()
 ******************************************************************************/
void PIUSDefault_SetCheckboxChecked(t_WidgetSysHandle *WidgetHandle,t_PIUICheckboxCtrl *Bttn,PG_BOOL Checked)
{
    t_UICheckboxCtrl *RealCheckbox=(t_UICheckboxCtrl *)Bttn;

    UICheckCheckbox(RealCheckbox,Checked);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_EnableCheckbox
 *
 * SYNOPSIS:
 *    void PIUSDefault_EnableCheckbox(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUICheckboxCtrl *Bttn,PG_BOOL Enabled);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Bttn [I] -- The checkbox being enabled or disabled.
 *    Enabled [I] -- true to make the checkbox respond to user input;
 *                   false to grey it out and ignore input.
 *
 * FUNCTION:
 *    Enables or disables user interaction with the checkbox.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddCheckbox()
 ******************************************************************************/
void PIUSDefault_EnableCheckbox(t_WidgetSysHandle *WidgetHandle,t_PIUICheckboxCtrl *Bttn,PG_BOOL Enabled)
{
    t_UICheckboxCtrl *RealCheckbox=(t_UICheckboxCtrl *)Bttn;

    UIEnableCheckbox(RealCheckbox,Enabled);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_AddTextInput
 *
 * SYNOPSIS:
 *    struct PI_TextInput *PIUSDefault_AddTextInput(t_WidgetSysHandle
 *            *WidgetHandle,const char *Label,
 *            void (*EventCB)(const struct PICBEvent *Event,void *UserData),
 *            void *UserData);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Label [I] -- The text label to display next to the widget.
 *    EventCB [I] -- Callback that fires when the widget generates an event. The
 *                   event structure is a 'struct PICBEvent'. May be NULL if
 *                   events are not needed.
 *    UserData [I] -- Pointer passed through to 'EventCB'
 *
 * FUNCTION:
 *    Adds a new single-line text input field.
 *
 * RETURNS:
 *    A handle to the new widget on success, or NULL if the widget could not be
 *    allocated.
 *
 * SEE ALSO:
 *    PIUSDefault_FreeTextInput(), PIUSDefault_GetTextInputText(),
 *    PIUSDefault_SetTextInputText()
 ******************************************************************************/
struct PI_TextInput *PIUSDefault_AddTextInput(t_WidgetSysHandle *WidgetHandle,
        const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData)
{
    return UIPI_AddTextInput((t_UILayoutContainerCtrl *)WidgetHandle,
            Label,EventCB,UserData);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_FreeTextInput
 *
 * SYNOPSIS:
 *    void PIUSDefault_FreeTextInput(t_WidgetSysHandle *WidgetHandle,
 *            struct PI_TextInput *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The text input returned by AddTextInput(). After this call
 *                  the handle is no longer valid.
 *
 * FUNCTION:
 *    Destroys a text input field and releases every resource associated with
 *    it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddTextInput()
 ******************************************************************************/
void PIUSDefault_FreeTextInput(t_WidgetSysHandle *WidgetHandle,
        struct PI_TextInput *UICtrl)
{
    UIPI_FreeTextInput(UICtrl);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_GetTextInputText
 *
 * SYNOPSIS:
 *    const char *PIUSDefault_GetTextInputText(t_WidgetSysHandle
 *            *WidgetHandle,t_PIUITextInputCtrl *TextInput);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    TextInput [I] -- The text input field being read.
 *
 * FUNCTION:
 *    Returns the text the user has typed (or that was set with
 *    PIUSDefault_GetTextInputText()) into a text input field.
 *
 * RETURNS:
 *    A pointer to a NUL-terminated string holding the current text. The buffer
 *    is owned by this function and is valid only until the next call to
 *    PIUSDefault_GetTextInputText().
 *
 * SEE ALSO:
 *    PIUSDefault_SetTextInputText()
 ******************************************************************************/
const char *PIUSDefault_GetTextInputText(t_WidgetSysHandle *WidgetHandle,
        t_PIUITextInputCtrl *TextInput)
{
    t_UITextInputCtrl *RealTextInput=(t_UITextInputCtrl *)TextInput;
    static string ReturnTxt;

    UIGetTextCtrlText(RealTextInput,ReturnTxt);

    return ReturnTxt.c_str();
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_SetTextInputText
 *
 * SYNOPSIS:
 *    void PIUSDefault_SetTextInputText(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUITextInputCtrl *TextInput,const char *Txt);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    TextInput [I] -- The text input field being modified.
 *    Txt [I] -- The new text. Must be NUL-terminated.
 *
 * FUNCTION:
 *    Replaces the text in a text input field.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_GetTextInputText()
 ******************************************************************************/
void PIUSDefault_SetTextInputText(t_WidgetSysHandle *WidgetHandle,
        t_PIUITextInputCtrl *TextInput,const char *Txt)
{
    t_UITextInputCtrl *RealTextInput=(t_UITextInputCtrl *)TextInput;

    UISetTextCtrlText(RealTextInput,Txt);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_EnableTextInput
 *
 * SYNOPSIS:
 *    void PIUSDefault_EnableTextInput(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUITextInputCtrl *TextInput,PG_BOOL Enabled);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    TextInput [I] -- The text input field being enabled or disabled.
 *    Enabled [I] -- true to make the field respond to user input; false
 *                   to grey it out and ignore input.
 *
 * FUNCTION:
 *    Enables or disables user interaction with a text input field.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddTextInput()
 ******************************************************************************/
void PIUSDefault_EnableTextInput(t_WidgetSysHandle *WidgetHandle,
        t_PIUITextInputCtrl *TextInput,PG_BOOL Enabled)
{
    t_UITextInputCtrl *RealTextInput=(t_UITextInputCtrl *)TextInput;

    UIEnableTextCtrl(RealTextInput,Enabled);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_AddNumberInput
 *
 * SYNOPSIS:
 *    struct PI_NumberInput *PIUSDefault_AddNumberInput(t_WidgetSysHandle
 *            *WidgetHandle,const char *Label,
 *            void (*EventCB)(const struct PICBEvent *Event,void *UserData),
 *            void *UserData);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Label [I] -- The text label to display next to the widget.
 *    EventCB [I] -- Callback that fires when the widget generates an event. The
 *                   event structure is a 'struct PICBEvent'. May be NULL if
 *                   events are not needed.
 *    UserData [I] -- Pointer passed through to 'EventCB'
 *
 * FUNCTION:
 *    Adds a new integer-number input field (a spin box) to the subsystem's
 *    container. Call PIUSDefault_SetNumberInputMinMax() afterwards to apply
 *    value limits.
 *
 * RETURNS:
 *    A handle to the new widget on success, or NULL if the widget could not be
 *    allocated.
 *
 * SEE ALSO:
 *    PIUSDefault_FreeNumberInput(), PIUSDefault_GetNumberInputValue(),
 *    PIUSDefault_SetNumberInputValue(), PIUSDefault_SetNumberInputMinMax()
 ******************************************************************************/
struct PI_NumberInput *PIUSDefault_AddNumberInput(t_WidgetSysHandle *WidgetHandle,
        const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData)
{
    return UIPI_AddNumberInput((t_UILayoutContainerCtrl *)WidgetHandle,Label,
            EventCB,UserData);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_FreeNumberInput
 *
 * SYNOPSIS:
 *    void PIUSDefault_FreeNumberInput(t_WidgetSysHandle *WidgetHandle,
 *            struct PI_NumberInput *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The number input returned by AddNumberInput(). After this
 *                  call the handle is no longer valid.
 *
 * FUNCTION:
 *    Destroys a number input field and releases every resource associated with
 *    it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddNumberInput()
 ******************************************************************************/
void PIUSDefault_FreeNumberInput(t_WidgetSysHandle *WidgetHandle,
        struct PI_NumberInput *UICtrl)
{
    UIPI_FreeNumberInput(UICtrl);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_GetNumberInputValue
 *
 * SYNOPSIS:
 *    uint64_t PIUSDefault_GetNumberInputValue(t_WidgetSysHandle
 *            *WidgetHandle,t_PIUINumberInputCtrl *NumberInput);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    NumberInput [I] -- The number input field being read.
 *
 * FUNCTION:
 *    Returns the integer value currently shown in a number input field.
 *
 * RETURNS:
 *    The current value as a 64-bit unsigned integer. The return type is
 *    uint64_t for transport but the underlying value is signed; see
 *    PIUSDefault_SetNumberInputValue().
 *
 * SEE ALSO:
 *    PIUSDefault_SetNumberInputValue(), PIUSDefault_SetNumberInputMinMax()
 ******************************************************************************/
uint64_t PIUSDefault_GetNumberInputValue(t_WidgetSysHandle *WidgetHandle,
        t_PIUINumberInputCtrl *NumberInput)
{
    t_UINumberInput *RealNumberInput=(t_UINumberInput *)NumberInput;

    return UIGetNumberInputCtrlValue(RealNumberInput);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_SetNumberInputValue
 *
 * SYNOPSIS:
 *    void PIUSDefault_SetNumberInputValue(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUINumberInputCtrl *NumberInput,int64_t Value);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    NumberInput [I] -- The number input field being modified.
 *    Value [I] -- The new value. It will be clamped to the range previously set
 *                 with SetNumberInputMinMax().
 *
 * FUNCTION:
 *    Sets the integer value displayed in a number input field.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_GetNumberInputValue(), PIUSDefault_SetNumberInputMinMax()
 ******************************************************************************/
void PIUSDefault_SetNumberInputValue(t_WidgetSysHandle *WidgetHandle,
        t_PIUINumberInputCtrl *NumberInput,int64_t Value)
{
    t_UINumberInput *RealNumberInput=(t_UINumberInput *)NumberInput;

    UISetNumberInputCtrlValue(RealNumberInput,Value);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_EnableNumberInput
 *
 * SYNOPSIS:
 *    void PIUSDefault_EnableNumberInput(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUINumberInputCtrl *NumberInput,PG_BOOL Enabled);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    NumberInput [I] -- The number input field being enabled or disabled.
 *    Enabled [I] -- true to make the field respond to user input; false
 *                   to grey it out and ignore input.
 *
 * FUNCTION:
 *    Enables or disables user interaction with a number input field.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddNumberInput()
 ******************************************************************************/
void PIUSDefault_EnableNumberInput(t_WidgetSysHandle *WidgetHandle,
        t_PIUINumberInputCtrl *NumberInput,PG_BOOL Enabled)
{
    t_UINumberInput *RealNumberInput=(t_UINumberInput *)NumberInput;

    UIEnableNumberInputCtrl(RealNumberInput,Enabled);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_SetNumberInputMinMax
 *
 * SYNOPSIS:
 *    void PIUSDefault_SetNumberInputMinMax(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUINumberInputCtrl *NumberInput,int64_t Min,int64_t Max);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    NumberInput [I] -- The number input field being constrained.
 *    Min [I] -- The smallest value the user is allowed to enter.
 *    Max [I] -- The largest value the user is allowed to enter.
 *
 * FUNCTION:
 *    Sets the inclusive minimum and maximum value the user is allowed to enter
 *    into a number input field. Values typed or pasted outside this range are
 *    clamped.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddNumberInput(), PIUSDefault_SetNumberInputValue()
 ******************************************************************************/
void PIUSDefault_SetNumberInputMinMax(t_WidgetSysHandle *WidgetHandle,
        t_PIUINumberInputCtrl *NumberInput,int64_t Min,int64_t Max)
{
    t_UINumberInput *RealNumberInput=(t_UINumberInput *)NumberInput;

    UISetNumberInputCtrlMin(RealNumberInput,Min);
    UISetNumberInputCtrlMax(RealNumberInput,Max);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_AddDoubleInput
 *
 * SYNOPSIS:
 *    struct PI_DoubleInput *PIUSDefault_AddDoubleInput(t_WidgetSysHandle
 *            *WidgetHandle,const char *Label,
 *            void (*EventCB)(const struct PICBEvent *Event,void *UserData),
 *            void *UserData);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Label [I] -- The text label to display next to the widget.
 *    EventCB [I] -- Callback that fires when the widget generates an event. The
 *                   event structure is a 'struct PICBEvent'. May be NULL if
 *                   events are not needed.
 *    UserData [I] -- Pointer passed through to 'EventCB'.
 *
 * FUNCTION:
 *    Adds a new floating-point number input field to the subsystem's container.
 *    Call PIUSDefault_SetDoubleInputMinMax() and
 *    PIUSDefault_SetDoubleInputDecimals() afterwards to apply value limits
 *    and choose the displayed precision.
 *
 * RETURNS:
 *    A handle to the new widget on success, or NULL if the widget could not be
 *    allocated.
 *
 * SEE ALSO:
 *    PIUSDefault_FreeDoubleInput(), PIUSDefault_GetDoubleInputValue(),
 *    PIUSDefault_SetDoubleInputValue(), PIUSDefault_SetDoubleInputMinMax(),
 *    PIUSDefault_SetDoubleInputDecimals()
 ******************************************************************************/
struct PI_DoubleInput *PIUSDefault_AddDoubleInput(t_WidgetSysHandle *WidgetHandle,
        const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData)
{
    return UIPI_AddDoubleInput((t_UILayoutContainerCtrl *)WidgetHandle,Label,
            EventCB,UserData);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_FreeDoubleInput
 *
 * SYNOPSIS:
 *    void PIUSDefault_FreeDoubleInput(t_WidgetSysHandle *WidgetHandle,
 *            struct PI_DoubleInput *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The double input returned by AddDoubleInput(). After this
 *                  call the handle is no longer valid.
 *
 * FUNCTION:
 *    Destroys a double input field and releases every resource associated with
 *    it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddDoubleInput()
 ******************************************************************************/
void PIUSDefault_FreeDoubleInput(t_WidgetSysHandle *WidgetHandle,
        struct PI_DoubleInput *UICtrl)
{
    UIPI_FreeDoubleInput(UICtrl);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_GetDoubleInputValue
 *
 * SYNOPSIS:
 *    double PIUSDefault_GetDoubleInputValue(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUIDoubleInputCtrl *DoubleInput);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    DoubleInput [I] -- The double input field being read.
 *
 * FUNCTION:
 *    Returns the floating-point value currently shown in a double input field.
 *
 * RETURNS:
 *    The current value as a double.
 *
 * SEE ALSO:
 *    PIUSDefault_SetDoubleInputValue()
 ******************************************************************************/
double PIUSDefault_GetDoubleInputValue(t_WidgetSysHandle *WidgetHandle,
        t_PIUIDoubleInputCtrl *DoubleInput)
{
    t_UIDoubleInput *RealDoubleInput=(t_UIDoubleInput *)DoubleInput;

    return UIGetDoubleInputCtrlValue(RealDoubleInput);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_SetDoubleInputValue
 *
 * SYNOPSIS:
 *    void PIUSDefault_SetDoubleInputValue(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUIDoubleInputCtrl *DoubleInput,double Value);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    DoubleInput [I] -- The double input field being modified.
 *    Value [I] -- The new value. It will be clamped to the range previously set
 *                 with SetDoubleInputMinMax().
 *
 * FUNCTION:
 *    Sets the floating-point value displayed in a double input field.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_GetDoubleInputValue(), PIUSDefault_SetDoubleInputMinMax(),
 *    PIUSDefault_SetDoubleInputDecimals()
 ******************************************************************************/
void PIUSDefault_SetDoubleInputValue(t_WidgetSysHandle *WidgetHandle,
        t_PIUIDoubleInputCtrl *DoubleInput,double Value)
{
    t_UIDoubleInput *RealDoubleInput=(t_UIDoubleInput *)DoubleInput;

    UISetDoubleInputCtrlValue(RealDoubleInput,Value);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_EnableDoubleInput
 *
 * SYNOPSIS:
 *    void PIUSDefault_EnableDoubleInput(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUIDoubleInputCtrl *DoubleInput,PG_BOOL Enabled);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    DoubleInput [I] -- The double input field being enabled or disabled.
 *    Enabled [I] -- true to make the field respond to user input; false
 *                   to grey it out and ignore input.
 *
 * FUNCTION:
 *    Enables or disables user interaction with a double input field.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddDoubleInput()
 ******************************************************************************/
void PIUSDefault_EnableDoubleInput(t_WidgetSysHandle *WidgetHandle,
        t_PIUIDoubleInputCtrl *DoubleInput,PG_BOOL Enabled)
{
    t_UIDoubleInput *RealDoubleInput=(t_UIDoubleInput *)DoubleInput;

    UIEnableDoubleInputCtrl(RealDoubleInput,Enabled);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_SetDoubleInputMinMax
 *
 * SYNOPSIS:
 *    void PIUSDefault_SetDoubleInputMinMax(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUIDoubleInputCtrl *DoubleInput,double Min,double Max);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    DoubleInput [I] -- The double input field being constrained.
 *    Min [I] -- The smallest value the user is allowed to enter.
 *    Max [I] -- The largest value the user is allowed to enter.
 *
 * FUNCTION:
 *    Sets the inclusive minimum and maximum value the user is allowed to enter
 *    into a double input field. Values typed or pasted outside this range are
 *    clamped.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddDoubleInput(), PIUSDefault_SetDoubleInputValue(),
 *    PIUSDefault_SetDoubleInputDecimals()
 ******************************************************************************/
void PIUSDefault_SetDoubleInputMinMax(t_WidgetSysHandle *WidgetHandle,t_PIUIDoubleInputCtrl *DoubleInput,double Min,double Max)
{
    t_UIDoubleInput *RealDoubleInput=(t_UIDoubleInput *)DoubleInput;

    UISetDoubleInputCtrlMin(RealDoubleInput,Min);
    UISetDoubleInputCtrlMax(RealDoubleInput,Max);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_SetDoubleInputDecimals
 *
 * SYNOPSIS:
 *    void PIUSDefault_SetDoubleInputDecimals(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUIDoubleInputCtrl *DoubleInput,int Points);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    DoubleInput [I] -- The double input field whose precision is being set.
 *    Points [I] -- The number of digits to display after the decimal point.
 *
 * FUNCTION:
 *    Sets how many digits after the decimal point are displayed in a double
 *    input field. This affects only the display; the underlying value is not
 *    rounded.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddDoubleInput(), PIUSDefault_SetDoubleInputValue()
 ******************************************************************************/
void PIUSDefault_SetDoubleInputDecimals(t_WidgetSysHandle *WidgetHandle,
        t_PIUIDoubleInputCtrl *DoubleInput,int Points)
{
    t_UIDoubleInput *RealDoubleInput=(t_UIDoubleInput *)DoubleInput;

    UISetDoubleInputCtrlDecimals(RealDoubleInput,Points);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_AddColumnViewInput
 *
 * SYNOPSIS:
 *    struct PI_ColumnViewInput *PIUSDefault_AddColumnViewInput(
 *            t_WidgetSysHandle *WidgetHandle,const char *Label,int Columns,
 *            const char *ColumnNames[],
 *            void (*EventCB)(const struct PICVEvent *Event,void *UserData),
 *            void *UserData);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Label [I] -- The text label to display next to the widget.
 *    Columns [I] -- The number of columns in the view. Must match the length of
 *                   the 'ColumnNames' array.
 *    ColumnNames [I] -- An array of 'Columns' used as the column headings.
 *    EventCB [I] -- Callback that fires when the widget generates an event. The
 *                   event structure is a 'struct PICVEvent'. May be NULL if
 *                   events are not needed.
 *    UserData [I] -- Pointer passed through to 'EventCB'
 *
 * FUNCTION:
 *    Adds a new multi-column list view (similar to a spreadsheet, with named
 *    column headings and selectable rows) to the subsystem's container.
 *
 * RETURNS:
 *    A handle to the new widget on success, or NULL if the widget could not be
 *    allocated.
 *
 * SEE ALSO:
 *    PIUSDefault_FreeColumnViewInput(), PIUSDefault_ColumnViewInputAddRow(),
 *    PIUSDefault_ColumnViewInputSetColumnText(),
 *    PIUSDefault_ColumnViewInputSelectRow()
 ******************************************************************************/
struct PI_ColumnViewInput *PIUSDefault_AddColumnViewInput(t_WidgetSysHandle *WidgetHandle,
        const char *Label,int Columns,const char *ColumnNames[],
        void (*EventCB)(const struct PICVEvent *Event,void *UserData),
        void *UserData)
{
    return UIPI_AddColumnViewInput((t_UILayoutContainerCtrl *)WidgetHandle,Label,
            Columns,ColumnNames,EventCB,UserData);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_FreeColumnViewInput
 *
 * SYNOPSIS:
 *    void PIUSDefault_FreeColumnViewInput(t_WidgetSysHandle *WidgetHandle,
 *            struct PI_ColumnViewInput *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The column view returned by AddColumnViewInput(). After this
 *                  call the handle is no longer valid.
 *
 * FUNCTION:
 *    Destroys a column view and releases every resource associated with it,
 *    including the storage for every row and cell.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddColumnViewInput()
 ******************************************************************************/
void PIUSDefault_FreeColumnViewInput(t_WidgetSysHandle *WidgetHandle,
        struct PI_ColumnViewInput *UICtrl)
{
    UIPI_FreeColumnViewInput(UICtrl);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ColumnViewInputClear
 *
 * SYNOPSIS:
 *    void PIUSDefault_ColumnViewInputClear(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUIColumnViewInputCtrl *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The column view whose rows are being removed.
 *
 * FUNCTION:
 *    Removes every row from a column view, leaving it empty. The column
 *    headings are not affected.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_ColumnViewInputAddRow(),
 *    PIUSDefault_ColumnViewInputRemoveRow()
 ******************************************************************************/
void PIUSDefault_ColumnViewInputClear(t_WidgetSysHandle *WidgetHandle,
        t_PIUIColumnViewInputCtrl *UICtrl)
{
    t_UIColumnView *RealColumnViewInput=(t_UIColumnView *)UICtrl;

    UIColumnViewClear(RealColumnViewInput);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ColumnViewInputRemoveRow
 *
 * SYNOPSIS:
 *    void PIUSDefault_ColumnViewInputRemoveRow(t_WidgetSysHandle
 *            *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl,int Row);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The column view being modified.
 *    Row [I] -- The zero-based index of the row to remove. Rows with a higher
 *               index shift down by one.
 *
 * FUNCTION:
 *    Removes a single row from a column view.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_ColumnViewInputAddRow(), PIUSDefault_ColumnViewInputClear()
 ******************************************************************************/
void PIUSDefault_ColumnViewInputRemoveRow(t_WidgetSysHandle *WidgetHandle,
        t_PIUIColumnViewInputCtrl *UICtrl,int Row)
{
    t_UIColumnView *RealColumnViewInput=(t_UIColumnView *)UICtrl;

    UIColumnViewRemoveRow(RealColumnViewInput,Row);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ColumnViewInputAddRow
 *
 * SYNOPSIS:
 *    int PIUSDefault_ColumnViewInputAddRow(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUIColumnViewInputCtrl *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The column view to add the row to.
 *
 * FUNCTION:
 *    Appends a new, empty row to the bottom of a column view. The caller then
 *    fills in the cells of the new row with
 *    PIUSDefault_ColumnViewInputSetColumnText().
 *
 * RETURNS:
 *    The zero-based index of the new row.
 *
 * SEE ALSO:
 *    PIUSDefault_ColumnViewInputSetColumnText(),
 *    PIUSDefault_ColumnViewInputRemoveRow()
 ******************************************************************************/
int PIUSDefault_ColumnViewInputAddRow(t_WidgetSysHandle *WidgetHandle,
        t_PIUIColumnViewInputCtrl *UICtrl)
{
    t_UIColumnView *RealColumnViewInput=(t_UIColumnView *)UICtrl;

    return UIColumnViewAddRow(RealColumnViewInput);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ColumnViewInputSetColumnText
 *
 * SYNOPSIS:
 *    void PIUSDefault_ColumnViewInputSetColumnText(t_WidgetSysHandle
 *            *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl,int Column,
 *            int Row,const char *Str);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The column view being modified.
 *    Column [I] -- The zero-based column index of the cell.
 *    Row [I] -- The zero-based row index of the cell.
 *    Str [I] -- The new text for the cell.
 *
 * FUNCTION:
 *    Sets the text shown in a single cell of a column view.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_ColumnViewInputAddRow()
 ******************************************************************************/
void PIUSDefault_ColumnViewInputSetColumnText(t_WidgetSysHandle *WidgetHandle,
        t_PIUIColumnViewInputCtrl *UICtrl,int Column,int Row,const char *Str)
{
    t_UIColumnView *RealColumnViewInput=(t_UIColumnView *)UICtrl;

    UIColumnViewSetColumnText(RealColumnViewInput,Column,Row,Str);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ColumnViewInputSelectRow
 *
 * SYNOPSIS:
 *    void PIUSDefault_ColumnViewInputSelectRow(t_WidgetSysHandle
 *            *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl,int Row);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The column view being modified.
 *    Row [I] -- The zero-based index of the row to select.
 *
 * FUNCTION:
 *    Makes the given row the currently-selected row in a column view. Any
 *    previously selected row becomes unselected.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_ColumnViewInputClearSelection()
 ******************************************************************************/
void PIUSDefault_ColumnViewInputSelectRow(t_WidgetSysHandle *WidgetHandle,
        t_PIUIColumnViewInputCtrl *UICtrl,int Row)
{
    t_UIColumnView *RealColumnViewInput=(t_UIColumnView *)UICtrl;

    UIColumnViewSelectRow(RealColumnViewInput,Row);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ColumnViewInputClearSelection
 *
 * SYNOPSIS:
 *    void PIUSDefault_ColumnViewInputClearSelection(t_WidgetSysHandle
 *            *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The column view being modified.
 *
 * FUNCTION:
 *    Unselects whatever row was selected in a column view, leaving the view
 *    with no row selected.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_ColumnViewInputSelectRow()
 ******************************************************************************/
void PIUSDefault_ColumnViewInputClearSelection(t_WidgetSysHandle *WidgetHandle,
        t_PIUIColumnViewInputCtrl *UICtrl)
{
    t_UIColumnView *RealColumnViewInput=(t_UIColumnView *)UICtrl;

    UIColumnViewClearSelection(RealColumnViewInput);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_AddButtonInput
 *
 * SYNOPSIS:
 *    struct PI_ButtonInput *PIUSDefault_AddButtonInput(t_WidgetSysHandle
 *            *WidgetHandle,const char *Label,
 *            void (*EventCB)(const struct PIButtonEvent *Event,
 *                    void *UserData),
 *            void *UserData);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Label [I] -- The text to display on the face of the button.
 *    EventCB [I] -- Callback that fires when the widget generates an event. The
 *                   event structure is a 'struct PIButtonEvent'. May be NULL if
 *                   events are not needed.
 *    UserData [I] -- Pointer passed through to 'EventCB'
 *
 * FUNCTION:
 *    Adds a new push button to the subsystem's container.
 *
 * RETURNS:
 *    A handle to the new widget on success, or NULL if the widget could not be
 *    allocated.
 *
 * SEE ALSO:
 *    PIUSDefault_FreeButtonInput()
 ******************************************************************************/
struct PI_ButtonInput *PIUSDefault_AddButtonInput(t_WidgetSysHandle *WidgetHandle,
        const char *Label,
        void (*EventCB)(const struct PIButtonEvent *Event,void *UserData),
        void *UserData)
{
    return UIPI_AddButtonInput((t_UILayoutContainerCtrl *)WidgetHandle,Label,
            EventCB,UserData);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_FreeButtonInput
 *
 * SYNOPSIS:
 *    void PIUSDefault_FreeButtonInput(t_WidgetSysHandle *WidgetHandle,
 *            struct PI_ButtonInput *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The button returned by PIUSDefault_AddButtonInput().
 *                  After this call the handle is no longer valid.
 *
 * FUNCTION:
 *    Destroys a push button and releases every resource associated with it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddButtonInput()
 ******************************************************************************/
void PIUSDefault_FreeButtonInput(t_WidgetSysHandle *WidgetHandle,
        struct PI_ButtonInput *UICtrl)
{
    UIPI_FreeButtonInput(UICtrl);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_AddIndicator
 *
 * SYNOPSIS:
 *    struct PI_Indicator *PIUSDefault_AddIndicator(t_WidgetSysHandle
 *            *WidgetHandle,const char *Label);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Label [I] -- The text label to display next to the indicator.
 *
 * FUNCTION:
 *    Adds a new on/off indicator (a 'lamp' or 'LED' control) to the subsystem's
 *    container. Indicators are read-only -- they show a state to the user but
 *    do not accept input.
 *
 * RETURNS:
 *    A handle to the new widget on success, or NULL if the widget could not be
 *    allocated.
 *
 * SEE ALSO:
 *    PIUSDefault_FreeIndicator(), PIUSDefault_SetIndicator()
 ******************************************************************************/
struct PI_Indicator *PIUSDefault_AddIndicator(t_WidgetSysHandle *WidgetHandle,
        const char *Label)
{
    return UIPI_AddIndicator((t_UILayoutContainerCtrl *)WidgetHandle,Label);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_FreeIndicator
 *
 * SYNOPSIS:
 *    void PIUSDefault_FreeIndicator(t_WidgetSysHandle *WidgetHandle,
 *            struct PI_Indicator *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The indicator returned by AddIndicator(). After this call
 *                  the handle is no longer valid.
 *
 * FUNCTION:
 *    Destroys an indicator and releases every resource associated with it.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddIndicator()
 ******************************************************************************/
void PIUSDefault_FreeIndicator(t_WidgetSysHandle *WidgetHandle,
        struct PI_Indicator *UICtrl)
{
    UIPI_FreeIndicator(UICtrl);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_SetIndicator
 *
 * SYNOPSIS:
 *    void PIUSDefault_SetIndicator(t_WidgetSysHandle *WidgetHandle,
 *            t_PIUIIndicatorCtrl *UICtrl,bool On);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The indicator being modified.
 *    On [I] -- true to turn the indicator on (illuminate it); false to turn it
 *              off.
 *
 * FUNCTION:
 *    Sets the on/off state of an indicator.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddIndicator()
 ******************************************************************************/
void PIUSDefault_SetIndicator(t_WidgetSysHandle *WidgetHandle,
        t_PIUIIndicatorCtrl *UICtrl,bool On)
{
    UIPI_SetIndicator(UICtrl,On);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_Ask
 *
 * SYNOPSIS:
 *    static int PIUSDefault_Ask(const char *Message,int Type);
 *
 * PARAMETERS:
 *    Message [I] -- The message to show to the user
 *    Type [I] -- The type of the message box.  Supported values:
 *                  PIUI_ASK_OK -- Just an OK button
 *                  PIUI_ASK_OKCANCEL -- An Ok and cancel buttons
 *                  PIUI_ASK_YESNO -- Yes and no buttons
 *                  PIUI_ASK_RETRYCANCEL -- Retry and cancel buttons
 *                  PIUI_ASK_IGNORECANCEL -- Ignore and cancel buttons
 *
 * FUNCTION:
 *    This function prompts the user with a message box.
 *
 * RETURNS:
 *    -1 -- There was an error
 *    PIUI_ASK_OK_BTTN -- User pressed ok
 *    PIUI_ASK_CANCEL_BTTN -- User pressed cancel
 *    PIUI_ASK_YES_BTTN -- User pressed yes
 *    PIUI_ASK_NO_BTTN -- User pressed no
 *    PIUI_ASK_RETRY_BTTN -- User pressed retry
 *    PIUI_ASK_IGNORE_BTTN -- User pressed ignore
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
int PIUSDefault_Ask(const char *Message,int Type)
{
    e_AskBttnsType Buttons;

    switch(Type)
    {
        case PIUI_ASK_OK:
            Buttons=e_AskBttns_Ok;
        break;
        case PIUI_ASK_OKCANCEL:
            Buttons=e_AskBttns_OkCancel;
        break;
        case PIUI_ASK_YESNO:
            Buttons=e_AskBttns_YesNo;
        break;
        case PIUI_ASK_RETRYCANCEL:
            Buttons=e_AskBttns_RetryCancel;
        break;
        case PIUI_ASK_IGNORECANCEL:
            Buttons=e_AskBttns_IgnoreCancel;
        break;
        default:
            return -1;
    }

    switch(UIAsk(WHIPPYTERM_NAME,Message,e_AskBox_Info,Buttons))
    {
        case e_AskRet_Cancel:
            return PIUI_ASK_CANCEL_BTTN;
        case e_AskRet_Ok:
            return PIUI_ASK_OK_BTTN;
        case e_AskRet_Retry:
            return PIUI_ASK_RETRY_BTTN;
        case e_AskRet_Ignore:
            return PIUI_ASK_IGNORE_BTTN;
        case e_AskRet_Yes:
            return PIUI_ASK_YES_BTTN;
        case e_AskRet_No:
            return PIUI_ASK_NO_BTTN;
        case e_AskRet_YesAll:
            return PIUI_ASK_YES_BTTN;
        case e_AskRet_NoAll:
            return e_AskRet_No;
        break;
        case e_AskRetMAX:
        default:
        return -1;
    }
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_FileReq
 *
 * SYNOPSIS:
 *    PG_BOOL PIUSDefault_FileReq(e_FileReqTypeType Req,const char *Title,
 *          char **Path,char **Filename,const char *Filters,int SelectedFilter);
 *
 * PARAMETERS:
 *    Req [I] -- What type of request are we making
 *    Title [I] -- The title for the req
 *    Path [O] -- A pointer that we will malloc a string into for the path.
 *                You must call FreeFileReqPathAndFile() to free this.
 *    Filename [O] -- A pointer that we will malloc a string into for the
 *                    filename.  You must call FreeFileReqPathAndFile() to free
 *                    this.
 *    Filters [I] -- This is a list of filters split by \n chars.  The format
 *                   of the filters is: DisplayName|Filter\n.  For example:
 *                      All Files|*\n
 *                      Image|*.png;*.bmp;*.jpg;*.jpeg\n
 *    SelectedFilter [I] -- The filter selected by default.  This is an index
 *                          into the 'Filters' list.
 *
 * FUNCTION:
 *    This function request the user select a filename and path.
 *
 * RETURNS:
 *    true -- Things worked out and user pressed ok
 *    false -- There was an error or the user press cancel
 *
 * NOTES:
 *    You must call FreeFileReqPathAndFile() with the path and filename to
 *    free the memory.  This only needs to be done if this function returns
 *    true.
 *
 * SEE ALSO:
 *    PIUSDefault_FreeFileReqPathAndFile()
 ******************************************************************************/
PG_BOOL PIUSDefault_FileReq(e_FileReqTypeType Req,const char *Title,char **Path,
        char **Filename,const char *Filters,int SelectedFilter)
{
    string SelPath;
    string SelFile;

    *Path=NULL;
    *Filename=NULL;

    switch(Req)
    {
        case e_FileReqType_Load:
            if(!UI_LoadFileReq(Title,SelPath,SelFile,Filters,SelectedFilter))
                return false;
        break;
        case e_FileReqType_Save:
            if(!UI_SaveFileReq(Title,SelPath,SelFile,Filters,SelectedFilter))
                return false;
        break;
        case e_FileReqTypeMAX:
        default:
            return false;
    }
    *Path=(char *)malloc(SelPath.length()+1);
    if(*Path==NULL)
        return false;
    *Filename=(char *)malloc(SelFile.length()+1);
    if(*Filename==NULL)
    {
        free(*Path);
        *Path=NULL;
        return false;
    }
    strcpy(*Path,SelPath.c_str());
    strcpy(*Filename,SelFile.c_str());
    return true;
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_FreeFileReqPathAndFile
 *
 * SYNOPSIS:
 *    void PIUSDefault_FreeFileReqPathAndFile(char **Path,char **Filename);
 *
 * PARAMETERS:
 *    Path [I] -- The path pointer to free.  This will also be set to NULL.
 *    Filename [I] -- The filename pointer to free.  This will also be set to
 *                    NULL.
 *
 * FUNCTION:
 *    This function frees the memory allocated with PIUSDefault_FileReq().
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_FileReq()
 ******************************************************************************/
void PIUSDefault_FreeFileReqPathAndFile(char **Path,char **Filename)
{
    free(*Path);
    *Path=NULL;
    free(*Filename);
    *Filename=NULL;
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_AddTextBox
 *
 * SYNOPSIS:
 *    struct PI_TextBox *PIUSDefault_AddTextBox(t_WidgetSysHandle *WidgetHandle,
 *              const char *Label,const char *Text);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Text [I] -- The text for inside this text display.
 *
 * FUNCTION:
 *    This function allocates a text box widget.  A text box widget is for
 *    displaying plain text.
 *
 * RETURNS:
 *    A handle to the text box.
 *
 * SEE ALSO:
 *    PIUSDefault_FreeTextBox(), PIUSDefault_SetTextBox(),
 *    PIUSDefault_SetTextBoxText()
 ******************************************************************************/
struct PI_TextBox *PIUSDefault_AddTextBox(t_WidgetSysHandle *WidgetHandle,
        const char *Label,const char *Text)
{
    return UIPI_AddTextBox((t_UILayoutContainerCtrl *)WidgetHandle,Label,Text);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_FreeTextBox
 *
 * SYNOPSIS:
 *    void PIUSDefault_FreeTextBox(t_WidgetSysHandle *WidgetHandle,
 *              struct PI_TextBox *BoxHandle);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    BoxHandle [I] -- The handle to the widget to free.  Allocated with
 *                     PIUSDefault_AddTextBox()
 *
 * FUNCTION:
 *    This function frees the widget allocated with PIUSDefault_AddTextBox()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddTextBox()
 ******************************************************************************/
void PIUSDefault_FreeTextBox(t_WidgetSysHandle *WidgetHandle,
        struct PI_TextBox *BoxHandle)
{
    UIPI_FreeTextBox(BoxHandle);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_SetTextBox
 *
 * SYNOPSIS:
 *    void PIUSDefault_SetTextBox(t_WidgetSysHandle *WidgetHandle,
 *          struct PI_TextBox *UICtrl,const char *Text);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    Text [I] -- The new text to put in the widget
 *
 * FUNCTION:
 *    This function replaces the text in the text box with new text.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddTextBox()
 ******************************************************************************/
void PIUSDefault_SetTextBox(t_WidgetSysHandle *WidgetHandle,
        t_PIUITextBoxCtrl *UICtrl,const char *Text)
{
    UIPI_SetTextBoxText(UICtrl,Text);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_SetTextBoxText
 *
 * SYNOPSIS:
 *    PG_BOOL PIUSDefault_SetTextBoxText(t_WidgetSysHandle *WidgetHandle,
 *          t_PIUITextBoxCtrl *UICtrl,e_TextBoxPropType Prop,uint32_t Value,
 *          void *Ptr);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    Prop [I] -- The property to change
 *                Supported properties:
 *                      e_TextBoxProp_FontMode -- Changes if the display is
 *                              fixed width or not.
 *                          Value: fixed width (true), or proportional (false).
 *                          Ptr: NULL
 *    Value [I] -- The value for the property (depends on 'Prop')
 *    Ptr [I] -- A pointer for the property (depends on 'Prop')
 *
 * FUNCTION:
 *    This function changes a property of a text box control.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    PIUSDefault_AddTextBox()
 ******************************************************************************/
PG_BOOL PIUSDefault_ChangeTextBoxProp(t_WidgetSysHandle *WidgetHandle,
        t_PIUITextBoxCtrl *UICtrl,e_TextBoxPropType Prop,uint32_t Value,
        void *Ptr)
{
    return UIPI_SetTextBoxText(UICtrl,Prop,Value,Ptr);
}



/*******************************************************************************
 * NAME:
 *    PIUSDefault_AddGroupBox
 *
 * SYNOPSIS:
 *    struct PI_GroupBox *PIUSDefault_AddGroupBox(
 *              t_WidgetSysHandle *WidgetHandle,const char *Label);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Label [I] -- The label text for this group box (the title).
 *
 * FUNCTION:
 *    This function allocates a group box widget.  A text box widget has
 *    other widgets in it.
 *
 * RETURNS:
 *    A handle to the group box.
 *
 * SEE ALSO:
 *    PIUSDefault_FreeGroupBox(), PIUSDefault_SetGroupBoxLabel()
 ******************************************************************************/
struct PI_GroupBox *PIUSDefault_AddGroupBox(t_WidgetSysHandle *WidgetHandle,
        const char *Label)
{
    return UIPI_AddGroupBox((t_UILayoutContainerCtrl *)WidgetHandle,Label);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_FreeGroupBox
 *
 * SYNOPSIS:
 *    void PIUSDefault_FreeGroupBox(t_WidgetSysHandle *WidgetHandle,
 *              struct PI_GroupBox *BoxHandle);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    BoxHandle [I] -- The handle to the widget to free.  Allocated with
 *                     PIUSDefault_AddGroupBox()
 *
 * FUNCTION:
 *    This function frees the widget allocated with PIUSDefault_AddGroupBox()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddGroupBox()
 ******************************************************************************/
void PIUSDefault_FreeGroupBox(t_WidgetSysHandle *WidgetHandle,
        struct PI_GroupBox *BoxHandle)
{
    UIPI_FreeGroupBox(BoxHandle);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_SetGroupBoxLabel
 *
 * SYNOPSIS:
 *    void PIUSDefault_SetGroupBoxLabel(t_WidgetSysHandle *WidgetHandle,
 *              t_PIUIGroupBoxCtrl *UICtrl,const char *Label);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    Label [I] -- The new label for this group box.
 *
 * FUNCTION:
 *    This function replaces sets a new label (title) for this group box
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddGroupBox()
 ******************************************************************************/
void PIUSDefault_SetGroupBoxLabel(t_WidgetSysHandle *WidgetHandle,
        t_PIUIGroupBoxCtrl *UICtrl,const char *Label)
{
    UIPI_SetGroupBoxLabel(UICtrl,Label);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_AddColorPick
 *
 * SYNOPSIS:
 *    struct PI_ColorPick *PIUSDefault_AddColorPick(t_WidgetSysHandle *WidgetHandle,
 *          const char *Label,uint32_t RGB,
 *          void (*EventCB)(const struct PIColorPickEvent *Event,void *UserData),
 *          void *UserData)
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Label [I] -- The label text for this color picker.
 *    RGB [I] -- The RGB value to set the widget to at the start
 *    EventCB [I] -- The event callback when the user changes picker color.
 *                   This can be NULL
 *    Event [I] -- The color picker event
 *    UserData [I] -- The user data that will be sent back into event callback.
 *
 * FUNCTION:
 *    This function allocates a color picker widget.
 *
 * RETURNS:
 *    A handle to the color picker.
 *
 * SEE ALSO:
 *    PIUSDefault_FreeColorPick(), PIUSDefault_GetColorPickValue(),
 *    PIUSDefault_SetColorPickValue()
 ******************************************************************************/
struct PI_ColorPick *PIUSDefault_AddColorPick(t_WidgetSysHandle *WidgetHandle,
        const char *Label,uint32_t RGB,
        void (*EventCB)(const struct PIColorPickEvent *Event,void *UserData),
        void *UserData)
{
    return UIPI_AddColorPickInput((t_UILayoutContainerCtrl *)WidgetHandle,
            Label,RGB,EventCB,UserData);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_FreeColorPick
 *
 * SYNOPSIS:
 *    void PIUSDefault_FreeColorPick(t_WidgetSysHandle *WidgetHandle,
 *              struct PI_ColorPick *Handle)
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Handle [I] -- The handle to the widget to free.  Allocated with
 *                     PIUSDefault_AddColorPick()
 *
 * FUNCTION:
 *    This function frees the widget allocated with PIUSDefault_AddGroupBox()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddColorPick()
 ******************************************************************************/
void PIUSDefault_FreeColorPick(t_WidgetSysHandle *WidgetHandle,struct PI_ColorPick *Handle)
{
    UIPI_FreeColorPickInput(Handle);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_GetColorPickValue
 *
 * SYNOPSIS:
 *    uint32_t PIUSDefault_GetColorPickValue(t_WidgetSysHandle *WidgetHandle,
 *              t_PIUIColorPickCtrl *UICtrl);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *
 * FUNCTION:
 *    This function reads the current color value from the widget.
 *
 * RETURNS:
 *    The current color value in the widget
 *
 * SEE ALSO:
 *    PIUSDefault_AddColorPick()
 ******************************************************************************/
uint32_t PIUSDefault_GetColorPickValue(t_WidgetSysHandle *WidgetHandle,
        t_PIUIColorPickCtrl *UICtrl)
{
    return UIPI_GetColorPickValue(UICtrl);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_SetColorPickValue
 *
 * SYNOPSIS:
 *    void PIUSDefault_SetColorPickValue(t_WidgetSysHandle *WidgetHandle,
 *              t_PIUIColorPickCtrl *UICtrl,uint32_t RGB);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    RGB [I] -- The new color to set this widget to
 *
 * FUNCTION:
 *    This function sets the current color value in the widget.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddColorPick()
 ******************************************************************************/
void PIUSDefault_SetColorPickValue(t_WidgetSysHandle *WidgetHandle,
        t_PIUIColorPickCtrl *UICtrl,uint32_t RGB)
{
    UIPI_SetColorPickValue(UICtrl,RGB);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_AddStylePick
 *
 * SYNOPSIS:
 *    struct PI_StylePick *PIUSDefault_AddStylePick(t_WidgetSysHandle *WidgetHandle,
 *          const char *Label,struct StyleData *SD,
 *          void (*EventCB)(const struct PIStylePickEvent *Event,void *UserData),
 *          void *UserData)
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Label [I] -- The label text for this style picker.
 *    SD [I] -- The styling data to apply to this widget.
 *    EventCB [I] -- The event callback when the user changes picker style.
 *                   This can be NULL
 *    Event [I] -- The style picker event
 *    UserData [I] -- The user data that will be sent back into event callback.
 *
 * FUNCTION:
 *    This function allocates a style picker widget.
 *
 * RETURNS:
 *    A handle to the style picker.
 *
 * SEE ALSO:
 *    PIUSDefault_FreeStylePick(), PIUSDefault_GetStylePickValue(),
 *    PIUSDefault_SetStylePickValue()
 ******************************************************************************/
struct PI_StylePick *PIUSDefault_AddStylePick(t_WidgetSysHandle *WidgetHandle,
        const char *Label,struct StyleData *SD,
        void (*EventCB)(const struct PIStylePickEvent *Event,void *UserData),
        void *UserData)
{
    return UIPI_AddStylePickInput((t_UILayoutContainerCtrl *)WidgetHandle,
            Label,SD,EventCB,UserData);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_FreeStylePick
 *
 * SYNOPSIS:
 *    void PIUSDefault_FreeStylePick(t_WidgetSysHandle *WidgetHandle,
 *              struct PI_StylePick *Handle)
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Handle [I] -- The handle to the widget to free.  Allocated with
 *                     PIUSDefault_AddStylePick()
 *
 * FUNCTION:
 *    This function frees the widget allocated with PIUSDefault_AddGroupBox()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddStylePick()
 ******************************************************************************/
void PIUSDefault_FreeStylePick(t_WidgetSysHandle *WidgetHandle,
        struct PI_StylePick *Handle)
{
    UIPI_FreeStylePickInput(Handle);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_GetStylePickValue
 *
 * SYNOPSIS:
 *    void PIUSDefault_GetStylePickValue(t_WidgetSysHandle *WidgetHandle,
 *              t_PIUIStylePickCtrl *UICtrl,struct StyleData *SD);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    SD [O] -- The style data that this widget is set to
 *
 * FUNCTION:
 *    This function reads the current style value from the widget.
 *
 * RETURNS:
 *    The current style value in the widget
 *
 * SEE ALSO:
 *    PIUSDefault_AddStylePick()
 ******************************************************************************/
void PIUSDefault_GetStylePickValue(t_WidgetSysHandle *WidgetHandle,
        t_PIUIStylePickCtrl *UICtrl,struct StyleData *SD)
{
    UIPI_GetStylePickValue(UICtrl,SD);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_SetStylePickValue
 *
 * SYNOPSIS:
 *    void PIUSDefault_SetStylePickValue(t_WidgetSysHandle *WidgetHandle,
 *              t_PIUIStylePickCtrl *UICtrl,struct StyleData *SD);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    SD [I] -- The styling data to apply to this widget.
 *
 * FUNCTION:
 *    This function sets the current style value in the widget.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    PIUSDefault_AddStylePick()
 ******************************************************************************/
void PIUSDefault_SetStylePickValue(t_WidgetSysHandle *WidgetHandle,
        t_PIUIStylePickCtrl *UICtrl,struct StyleData *SD)
{
    UIPI_SetStylePickValue(UICtrl,SD);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_Style2StrHelper
 *
 * SYNOPSIS:
 *    PG_BOOL PIUSDefault_Style2StrHelper(struct StyleData *SD,char *Str,
 *              int MaxLen);
 *
 * PARAMETERS:
 *    SD [I] -- The style data to convert to a string
 *    Str [O] -- The place to store the string
 *    MaxLen [I] -- The size of the 'Str' buffer.  It is suggested to use
 *                  'SUGGESTED_STYLE_DATA_STR_BUFFER_LEN'
 *
 * FUNCTION:
 *    This function takes styling data and converts it to a string that can be
 *    used to store this styling data in a KV list or any where you want to
 *    store the data.  It is not ment to be shown to the user.
 *
 * RETURNS:
 *    true -- The buffer is big enough and the conversion has been done.
 *    false -- The buffer is not big enough.
 *
 * SEE ALSO:
 *    PIUSDefault_Str2StyleHelper()
 ******************************************************************************/
PG_BOOL PIUSDefault_Style2StrHelper(struct StyleData *SD,char *Str,
        int MaxLen)
{
    if(MaxLen<7*4+1)    // 6 dig + 1 comma, 4 numbers, + an extra 1 for end of string
        return false;

    sprintf(Str,"%06X,%06X,%06X,%06X,",SD->FGColor,SD->BGColor,SD->ULineColor,
            SD->Attribs);
    return true;
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_Str2StyleHelper
 *
 * SYNOPSIS:
 *    void PIUSDefault_Str2StyleHelper(struct StyleData *SD,const char *Str);
 *
 * PARAMETERS:
 *    SD [O] -- Where to place the converted string
 *    Str [I] -- The string to convert.
 *
 * FUNCTION:
 *    This function converts a string made with PIUSDefault_Str2StyleHelper()
 *    back to style data.
 *
 * RETURNS:
 *    NONE
 *
 * NOTES:
 *    If the string is not valid or is missing things then defaults will be
 *    used.
 *
 * SEE ALSO:
 *    PIUSDefault_Str2StyleHelper()
 ******************************************************************************/
void PIUSDefault_Str2StyleHelper(struct StyleData *SD,const char *Str)
{
    char *NextStart;

    memset(SD,0x00,sizeof(struct StyleData));

    /* Use styling defaults */
    SD->FGColor=g_Settings.DefaultConSettings.DefaultColors[e_DefaultColors_FG];
    SD->BGColor=g_Settings.DefaultConSettings.DefaultColors[e_DefaultColors_BG];
    SD->ULineColor=SD->FGColor;
    SD->Attribs=0;

    NextStart=(char *)Str;
    SD->FGColor=strtoul(NextStart,&NextStart,16);
    if(NextStart!=NULL && *NextStart==',')
    {
        NextStart++;
        SD->BGColor=strtoul(NextStart,&NextStart,16);
        if(NextStart!=NULL && *NextStart==',')
        {
            NextStart++;
            SD->ULineColor=strtoul(NextStart,&NextStart,16);
            if(NextStart!=NULL && *NextStart==',')
            {
                NextStart++;
                SD->Attribs=strtoul(NextStart,&NextStart,16);
            }
        }
    }
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ChangeComboBoxProp
 *
 * SYNOPSIS:
 *    PG_BOOL PIUSDefault_ChangeComboBoxProp(t_WidgetSysHandle *WidgetHandle,
 *          t_PIUIComboBoxCtrl *UICtrl,e_ComboBoxPropType Prop,uint32_t Value,
 *          void *Ptr);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    Prop [I] -- The property to change
 *    Value [I] -- The value for the property (depends on 'Prop')
 *    Ptr [I] -- A pointer for the property (depends on 'Prop')
 *
 * FUNCTION:
 *    This function changes a property of a combo box control.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    AddComboBox()
 ******************************************************************************/
PG_BOOL PIUSDefault_ChangeComboBoxProp(t_WidgetSysHandle *WidgetHandle,
        t_PIUIComboBoxCtrl *UICtrl,e_ComboBoxPropType Prop,uint32_t Value,
        void *Ptr)
{
    return false;
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ChangeRadioBttnProp
 *
 * SYNOPSIS:
 *    PG_BOOL PIUSDefault_ChangeRadioBttnProp(t_WidgetSysHandle *WidgetHandle,
 *          t_PIUIRadioBttnCtrl *UICtrl,e_RadioBttnPropType Prop,uint32_t Value,
 *          void *Ptr);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    Prop [I] -- The property to change
 *    Value [I] -- The value for the property (depends on 'Prop')
 *    Ptr [I] -- A pointer for the property (depends on 'Prop')
 *
 * FUNCTION:
 *    This function changes a property of a radio button control.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    AddRadioBttn()
 ******************************************************************************/
PG_BOOL PIUSDefault_ChangeRadioBttnProp(t_WidgetSysHandle *WidgetHandle,
        t_PIUIRadioBttnCtrl *UICtrl,e_RadioBttnPropType Prop,uint32_t Value,
        void *Ptr)
{
    return false;
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ChangeCheckboxProp
 *
 * SYNOPSIS:
 *    PG_BOOL PIUSDefault_ChangeCheckboxProp(t_WidgetSysHandle *WidgetHandle,
 *          t_PIUICheckboxCtrl *UICtrl,e_CheckboxPropType Prop,uint32_t Value,
 *          void *Ptr);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    Prop [I] -- The property to change
 *    Value [I] -- The value for the property (depends on 'Prop')
 *    Ptr [I] -- A pointer for the property (depends on 'Prop')
 *
 * FUNCTION:
 *    This function changes a property of a checkbox control.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    AddCheckbox()
 ******************************************************************************/
PG_BOOL PIUSDefault_ChangeCheckboxProp(t_WidgetSysHandle *WidgetHandle,
        t_PIUICheckboxCtrl *UICtrl,e_CheckboxPropType Prop,uint32_t Value,
        void *Ptr)
{
    return false;
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ChangeTextInputProp
 *
 * SYNOPSIS:
 *    PG_BOOL PIUSDefault_ChangeTextInputProp(t_WidgetSysHandle *WidgetHandle,
 *          t_PIUITextInputCtrl *UICtrl,e_TextInputPropType Prop,uint32_t Value,
 *          void *Ptr);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    Prop [I] -- The property to change
 *    Value [I] -- The value for the property (depends on 'Prop')
 *    Ptr [I] -- A pointer for the property (depends on 'Prop')
 *
 * FUNCTION:
 *    This function changes a property of a text input control.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    AddTextInput()
 ******************************************************************************/
PG_BOOL PIUSDefault_ChangeTextInputProp(t_WidgetSysHandle *WidgetHandle,
        t_PIUITextInputCtrl *UICtrl,e_TextInputPropType Prop,uint32_t Value,
        void *Ptr)
{
    return false;
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ChangeNumberInputProp
 *
 * SYNOPSIS:
 *    PG_BOOL PIUSDefault_ChangeNumberInputProp(t_WidgetSysHandle *WidgetHandle,
 *          t_PIUINumberInputCtrl *UICtrl,e_NumberInputPropType Prop,uint32_t Value,
 *          void *Ptr);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    Prop [I] -- The property to change
 *    Value [I] -- The value for the property (depends on 'Prop')
 *    Ptr [I] -- A pointer for the property (depends on 'Prop')
 *
 * FUNCTION:
 *    This function changes a property of a number input control.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    AddNumberInput()
 ******************************************************************************/
PG_BOOL PIUSDefault_ChangeNumberInputProp(t_WidgetSysHandle *WidgetHandle,
        t_PIUINumberInputCtrl *UICtrl,e_NumberInputPropType Prop,uint32_t Value,
        void *Ptr)
{
    return false;
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ChangeDoubleInputProp
 *
 * SYNOPSIS:
 *    PG_BOOL PIUSDefault_ChangeDoubleInputProp(t_WidgetSysHandle *WidgetHandle,
 *          t_PIUIDoubleInputCtrl *UICtrl,e_DoubleInputPropType Prop,uint32_t Value,
 *          void *Ptr);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    Prop [I] -- The property to change
 *    Value [I] -- The value for the property (depends on 'Prop')
 *    Ptr [I] -- A pointer for the property (depends on 'Prop')
 *
 * FUNCTION:
 *    This function changes a property of a double input control.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    AddDoubleInput()
 ******************************************************************************/
PG_BOOL PIUSDefault_ChangeDoubleInputProp(t_WidgetSysHandle *WidgetHandle,
        t_PIUIDoubleInputCtrl *UICtrl,e_DoubleInputPropType Prop,uint32_t Value,
        void *Ptr)
{
    return false;
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ChangeColumnViewInputProp
 *
 * SYNOPSIS:
 *    PG_BOOL PIUSDefault_ChangeColumnViewInputProp(t_WidgetSysHandle *WidgetHandle,
 *          t_PIUIColumnViewInputCtrl *UICtrl,e_ColumnViewInputPropType Prop,uint32_t Value,
 *          void *Ptr);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    Prop [I] -- The property to change
 *    Value [I] -- The value for the property (depends on 'Prop')
 *    Ptr [I] -- A pointer for the property (depends on 'Prop')
 *
 * FUNCTION:
 *    This function changes a property of a column view control.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    AddColumnViewInput()
 ******************************************************************************/
PG_BOOL PIUSDefault_ChangeColumnViewInputProp(t_WidgetSysHandle *WidgetHandle,
        t_PIUIColumnViewInputCtrl *UICtrl,e_ColumnViewInputPropType Prop,uint32_t Value,
        void *Ptr)
{
    return false;
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ChangeButtonInputProp
 *
 * SYNOPSIS:
 *    PG_BOOL PIUSDefault_ChangeButtonInputProp(t_WidgetSysHandle *WidgetHandle,
 *          t_PIUIButtonInputCtrl *UICtrl,e_ButtonInputPropType Prop,uint32_t Value,
 *          void *Ptr);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    Prop [I] -- The property to change
 *    Value [I] -- The value for the property (depends on 'Prop')
 *    Ptr [I] -- A pointer for the property (depends on 'Prop')
 *
 * FUNCTION:
 *    This function changes a property of a button input control.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    AddButtonInput()
 ******************************************************************************/
PG_BOOL PIUSDefault_ChangeButtonInputProp(t_WidgetSysHandle *WidgetHandle,
        t_PIUIButtonInputCtrl *UICtrl,e_ButtonInputPropType Prop,uint32_t Value,
        void *Ptr)
{
    return false;
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ChangeIndicatorProp
 *
 * SYNOPSIS:
 *    PG_BOOL PIUSDefault_ChangeIndicatorProp(t_WidgetSysHandle *WidgetHandle,
 *          t_PIUIIndicatorCtrl *UICtrl,e_IndicatorPropType Prop,uint32_t Value,
 *          void *Ptr);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    Prop [I] -- The property to change
 *    Value [I] -- The value for the property (depends on 'Prop')
 *    Ptr [I] -- A pointer for the property (depends on 'Prop')
 *
 * FUNCTION:
 *    This function changes a property of a indicator control.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    AddIndicator()
 ******************************************************************************/
PG_BOOL PIUSDefault_ChangeIndicatorProp(t_WidgetSysHandle *WidgetHandle,
        t_PIUIIndicatorCtrl *UICtrl,e_IndicatorPropType Prop,uint32_t Value,
        void *Ptr)
{
    return false;
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ChangeGroupBoxProp
 *
 * SYNOPSIS:
 *    PG_BOOL PIUSDefault_ChangeGroupBoxProp(t_WidgetSysHandle *WidgetHandle,
 *          t_PIUIGroupBoxCtrl *UICtrl,e_GroupBoxPropType Prop,uint32_t Value,
 *          void *Ptr);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    Prop [I] -- The property to change
 *    Value [I] -- The value for the property (depends on 'Prop')
 *    Ptr [I] -- A pointer for the property (depends on 'Prop')
 *
 * FUNCTION:
 *    This function changes a property of a group box control.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    AddGroupBox()
 ******************************************************************************/
PG_BOOL PIUSDefault_ChangeGroupBoxProp(t_WidgetSysHandle *WidgetHandle,
        t_PIUIGroupBoxCtrl *UICtrl,e_GroupBoxPropType Prop,uint32_t Value,
        void *Ptr)
{
    return false;
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ChangeColorPickProp
 *
 * SYNOPSIS:
 *    PG_BOOL PIUSDefault_ChangeColorPickProp(t_WidgetSysHandle *WidgetHandle,
 *          t_PIUIColorPickCtrl *UICtrl,e_ColorPickPropType Prop,uint32_t Value,
 *          void *Ptr);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    Prop [I] -- The property to change
 *    Value [I] -- The value for the property (depends on 'Prop')
 *    Ptr [I] -- A pointer for the property (depends on 'Prop')
 *
 * FUNCTION:
 *    This function changes a property of a color picker control.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    AddColorPick()
 ******************************************************************************/
PG_BOOL PIUSDefault_ChangeColorPickProp(t_WidgetSysHandle *WidgetHandle,
        t_PIUIColorPickCtrl *UICtrl,e_ColorPickPropType Prop,uint32_t Value,
        void *Ptr)
{
    return false;
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ChangeStylePickProp
 *
 * SYNOPSIS:
 *    PG_BOOL PIUSDefault_ChangeStylePickProp(t_WidgetSysHandle *WidgetHandle,
 *          t_PIUIStylePickCtrl *UICtrl,e_StylePickPropType Prop,uint32_t Value,
 *          void *Ptr);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    Prop [I] -- The property to change
 *    Value [I] -- The value for the property (depends on 'Prop')
 *    Ptr [I] -- A pointer for the property (depends on 'Prop')
 *
 * FUNCTION:
 *    This function changes a property of a style picker control.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    AddStylePick()
 ******************************************************************************/
PG_BOOL PIUSDefault_ChangeStylePickProp(t_WidgetSysHandle *WidgetHandle,
        t_PIUIStylePickCtrl *UICtrl,e_StylePickPropType Prop,uint32_t Value,
        void *Ptr)
{
    return false;
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_SetLabelText
 *
 * SYNOPSIS:
 *    void PIUSDefault_SetLabelText(t_WidgetSysHandle *WidgetHandle,
 *          t_PIUILabelCtrl *UILabel,const char *Txt);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UILabel [I] -- The label for a widget to change.
 *    Txt [I] -- The new text to set this label to
 *
 * FUNCTION:
 *    This function changes the text for a widgets label.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void PIUSDefault_SetLabelText(t_WidgetSysHandle *WidgetHandle,
        t_PIUILabelCtrl *UILabel,const char *Txt)
{
    UIPI_SetLabelText(UILabel,Txt);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_AddWebLink
 *
 * SYNOPSIS:
 *    struct PI_WebLink *PIUSDefault_AddWebLink(t_WidgetSysHandle *WidgetHandle,
 *              const char *Label,const char *Text,const char *URL);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    Label [I] -- The label for this web link.  The text that is shown to the
 *                 user.
 *    Text [I] -- The text to display to the user.
 *    URL [I] -- The URL to open when the user clicks on the 'Text'.
 *
 * FUNCTION:
 *    This function allocates and adds a web link control to the display.
 *
 * RETURNS:
 *    A pointer to the web link or NULL if there was an error.
 *
 * SEE ALSO:
 *    PIUSDefault_FreeWebLink(), PIUSDefault_SetWebLinkURL(),
 *    PIUSDefault_ChangeWebLinkProp()
 ******************************************************************************/
struct PI_WebLink *PIUSDefault_AddWebLink(t_WidgetSysHandle *WidgetHandle,
        const char *Label,const char *Text,const char *URL)
{
    return UIPI_AddWebLink((t_UILayoutContainerCtrl *)WidgetHandle,Label,Text,
            URL);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_FreeWebLink
 *
 * SYNOPSIS:
 *    void PIUSDefault_FreeWebLink(t_WidgetSysHandle *WidgetHandle,
 *              struct PI_WebLink *WebLinkHandle);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    WebLinkHandle [I] -- The web link to free
 *
 * FUNCTION:
 *    This function frees a web link control that was allocated with
 *    PIUSDefault_AddWebLink().
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void PIUSDefault_FreeWebLink(t_WidgetSysHandle *WidgetHandle,
        struct PI_WebLink *WebLinkHandle)
{
    UIPI_FreeWebLink(WebLinkHandle);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_SetWebLinkURL
 *
 * SYNOPSIS:
 *    void PIUSDefault_SetWebLinkURL(t_WidgetSysHandle *WidgetHandle,
 *              t_PIUIWebLinkCtrl *UICtrl,const char *Text,const char *URL);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    Text [I] -- The text to display.
 *    URL [I] -- The URL to open when the link is clicked on
 *
 * FUNCTION:
 *    This function changes the text and URL that the web link goes to when
 *    clicked on.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    AddWebLink()
 ******************************************************************************/
void PIUSDefault_SetWebLinkURL(t_WidgetSysHandle *WidgetHandle,
        t_PIUIWebLinkCtrl *UICtrl,const char *Text,const char *URL)
{
    UIPI_SetWebLinkURL(UICtrl,Text,URL);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_ChangeWebLinkProp
 *
 * SYNOPSIS:
 *    PG_BOOL PIUSDefault_ChangeWebLinkProp(t_WidgetSysHandle *WidgetHandle,
 *          t_PIUIWebLinkCtrl *UICtrl,e_WebLinkPropType Prop,uint32_t Value,
 *          void *Ptr);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to the widget data.
 *    UICtrl [I] -- The handle to the widget to work on.
 *    Prop [I] -- The property to change
 *    Value [I] -- The value for the property (depends on 'Prop')
 *    Ptr [I] -- A pointer for the property (depends on 'Prop')
 *
 * FUNCTION:
 *    This function changes a property of a web link control.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    AddWebLink()
 ******************************************************************************/
PG_BOOL PIUSDefault_ChangeWebLinkProp(t_WidgetSysHandle *WidgetHandle,
        t_PIUIWebLinkCtrl *UICtrl,e_WebLinkPropType Prop,uint32_t Value,
        void *Ptr)
{
    return false;
}
