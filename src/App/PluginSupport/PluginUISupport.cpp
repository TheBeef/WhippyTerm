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

struct PI_ComboBox *PIUSDefault_AddComboBox(t_WidgetSysHandle *WidgetHandle,
        PG_BOOL UserEditable,const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData)
{
    return UIPI_AddComboBox((t_UILayoutContainerCtrl *)WidgetHandle,
            UserEditable,Label,EventCB,UserData);
}

void PIUSDefault_FreeComboBox(t_WidgetSysHandle *WidgetHandle,
        struct PI_ComboBox *UICtrl)
{
    UIPI_FreeComboBox(UICtrl);
}

void PIUSDefault_ClearComboBox(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox)
{
    t_UIComboBoxCtrl *RealComboBox=(t_UIComboBoxCtrl *)ComboBox;
    UIClearComboBox(RealComboBox);
}

void PIUSDefault_AddItem2ComboBox(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox,const char *Label,
        uintptr_t ID)
{
    t_UIComboBoxCtrl *RealComboBox=(t_UIComboBoxCtrl *)ComboBox;
    UIAddItem2ComboBox(RealComboBox,Label,ID);
}

void PIUSDefault_SetComboBoxSelectedEntry(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox,uintptr_t ID)
{
    t_UIComboBoxCtrl *RealComboBox=(t_UIComboBoxCtrl *)ComboBox;
    UISetComboBoxSelectedEntry(RealComboBox,ID);
}

uintptr_t PIUSDefault_GetComboBoxSelectedEntry(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox)
{
    t_UIComboBoxCtrl *RealComboBox=(t_UIComboBoxCtrl *)ComboBox;
    return UIGetComboBoxSelectedEntry(RealComboBox);
}

void PIUSDefault_EnableComboBox(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox,PG_BOOL Enabled)
{
    t_UIComboBoxCtrl *RealComboBox=(t_UIComboBoxCtrl *)ComboBox;
    UIEnableComboBox(RealComboBox,Enabled);
}

struct PI_RadioBttnGroup *PIUSDefault_AllocRadioBttnGroup(t_WidgetSysHandle *WidgetHandle,const char *Label)
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
 *    WidgetHandle [I] -- The handle to the widget data we allocated in
 *                        IOS_AllocConnectionOptions()
 *    UICtrl [I] -- The widget to free.
 *
 * FUNCTION:
 *    This function frees a group box.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void PIUSDefault_FreeRadioBttnGroup(t_WidgetSysHandle *WidgetHandle,struct PI_RadioBttnGroup *UICtrl)
{
    UIPI_FreeRadioBttnGroup(UICtrl);
}

struct PI_RadioBttn *PIUSDefault_AddRadioBttn(t_WidgetSysHandle *WidgetHandle,struct PI_RadioBttnGroup *RBGroup,const char *Label,void (*EventCB)(const struct PIRBEvent *Event,void *UserData),void *UserData)
{
    return UIPI_AddRadioBttn(RBGroup,Label,EventCB,UserData);
}

void PIUSDefault_FreeRadioBttn(t_WidgetSysHandle *WidgetHandle,struct PI_RadioBttn *UICtrl)
{
    UIPI_FreeRadioBttn(UICtrl);
}

PG_BOOL PIUSDefault_IsRadioBttnChecked(t_WidgetSysHandle *WidgetHandle,struct PI_RadioBttn *Bttn)
{
    return UIPI_IsRadioBttnChecked(Bttn);
}

void PIUSDefault_SetRadioBttnChecked(t_WidgetSysHandle *WidgetHandle,struct PI_RadioBttn *Bttn,PG_BOOL Checked)
{
    UIPI_SetRadioBttnChecked(Bttn,Checked);
}

void PIUSDefault_EnableRadioBttn(t_WidgetSysHandle *WidgetHandle,struct PI_RadioBttn *Bttn,PG_BOOL Enabled)
{
    UIPI_EnableRadioBttn(Bttn,Enabled);
}

const char *PIUSDefault_GetComboBoxText(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox)
{
    t_UIComboBoxCtrl *RealComboBox=(t_UIComboBoxCtrl *)ComboBox;
    static string ReturnTxt;

    UIGetComboBoxText(RealComboBox,ReturnTxt);

    return ReturnTxt.c_str();
}

void PIUSDefault_SetComboBoxText(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox,const char *Txt)
{
    t_UIComboBoxCtrl *RealComboBox=(t_UIComboBoxCtrl *)ComboBox;

    UISetComboBoxText(RealComboBox,Txt);
}

struct PI_Checkbox *PIUSDefault_AddCheckbox(t_WidgetSysHandle *WidgetHandle,const char *Label,void (*EventCB)(const struct PICheckboxEvent *Event,void *UserData),void *UserData)
{
    return UIPI_AddCheckbox((t_UILayoutContainerCtrl *)WidgetHandle,Label,EventCB,UserData);
}

void PIUSDefault_FreeCheckbox(t_WidgetSysHandle *WidgetHandle,struct PI_Checkbox *UICtrl)
{
    UIPI_FreeCheckbox(UICtrl);
}

PG_BOOL PIUSDefault_IsCheckboxChecked(t_WidgetSysHandle *WidgetHandle,t_PIUICheckboxCtrl *Bttn)
{
    t_UICheckboxCtrl *RealCheckbox=(t_UICheckboxCtrl *)Bttn;

    return UIGetCheckboxCheckStatus(RealCheckbox);
}

void PIUSDefault_SetCheckboxChecked(t_WidgetSysHandle *WidgetHandle,t_PIUICheckboxCtrl *Bttn,PG_BOOL Checked)
{
    t_UICheckboxCtrl *RealCheckbox=(t_UICheckboxCtrl *)Bttn;

    UICheckCheckbox(RealCheckbox,Checked);
}

void PIUSDefault_EnableCheckbox(t_WidgetSysHandle *WidgetHandle,t_PIUICheckboxCtrl *Bttn,PG_BOOL Enabled)
{
    t_UICheckboxCtrl *RealCheckbox=(t_UICheckboxCtrl *)Bttn;

    UIEnableCheckbox(RealCheckbox,Enabled);
}

struct PI_TextInput *PIUSDefault_AddTextInput(t_WidgetSysHandle *WidgetHandle,
        const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData)
{
    return UIPI_AddTextInput((t_UILayoutContainerCtrl *)WidgetHandle,
            Label,EventCB,UserData);
}

void PIUSDefault_FreeTextInput(t_WidgetSysHandle *WidgetHandle,
        struct PI_TextInput *UICtrl)
{
    UIPI_FreeTextInput(UICtrl);
}

const char *PIUSDefault_GetTextInputText(t_WidgetSysHandle *WidgetHandle,t_PIUITextInputCtrl *TextInput)
{
    t_UITextInputCtrl *RealTextInput=(t_UITextInputCtrl *)TextInput;
    static string ReturnTxt;

    UIGetTextCtrlText(RealTextInput,ReturnTxt);

    return ReturnTxt.c_str();
}

void PIUSDefault_SetTextInputText(t_WidgetSysHandle *WidgetHandle,t_PIUITextInputCtrl *TextInput,const char *Txt)
{
    t_UITextInputCtrl *RealTextInput=(t_UITextInputCtrl *)TextInput;

    UISetTextCtrlText(RealTextInput,Txt);
}

void PIUSDefault_EnableTextInput(t_WidgetSysHandle *WidgetHandle,t_PIUITextInputCtrl *TextInput,PG_BOOL Enabled)
{
    t_UITextInputCtrl *RealTextInput=(t_UITextInputCtrl *)TextInput;

    UIEnableTextCtrl(RealTextInput,Enabled);
}

struct PI_NumberInput *PIUSDefault_AddNumberInput(t_WidgetSysHandle *WidgetHandle,
        const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData)
{
    return UIPI_AddNumberInput((t_UILayoutContainerCtrl *)WidgetHandle,Label,
            EventCB,UserData);
}

void PIUSDefault_FreeNumberInput(t_WidgetSysHandle *WidgetHandle,
        struct PI_NumberInput *UICtrl)
{
    UIPI_FreeNumberInput(UICtrl);
}

uint64_t PIUSDefault_GetNumberInputValue(t_WidgetSysHandle *WidgetHandle,t_PIUINumberInputCtrl *NumberInput)
{
    t_UINumberInput *RealNumberInput=(t_UINumberInput *)NumberInput;

    return UIGetNumberInputCtrlValue(RealNumberInput);
}

void PIUSDefault_SetNumberInputValue(t_WidgetSysHandle *WidgetHandle,t_PIUINumberInputCtrl *NumberInput,int64_t Value)
{
    t_UINumberInput *RealNumberInput=(t_UINumberInput *)NumberInput;

    UISetNumberInputCtrlValue(RealNumberInput,Value);
}

void PIUSDefault_EnableNumberInput(t_WidgetSysHandle *WidgetHandle,t_PIUINumberInputCtrl *NumberInput,PG_BOOL Enabled)
{
    t_UINumberInput *RealNumberInput=(t_UINumberInput *)NumberInput;

    UIEnableNumberInputCtrl(RealNumberInput,Enabled);
}

void PIUSDefault_SetNumberInputMinMax(t_WidgetSysHandle *WidgetHandle,t_PIUINumberInputCtrl *NumberInput,int64_t Min,int64_t Max)
{
    t_UINumberInput *RealNumberInput=(t_UINumberInput *)NumberInput;

    UISetNumberInputCtrlMin(RealNumberInput,Min);
    UISetNumberInputCtrlMax(RealNumberInput,Max);
}

struct PI_DoubleInput *PIUSDefault_AddDoubleInput(t_WidgetSysHandle *WidgetHandle,
        const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData)
{
    return UIPI_AddDoubleInput((t_UILayoutContainerCtrl *)WidgetHandle,Label,
            EventCB,UserData);
}

void PIUSDefault_FreeDoubleInput(t_WidgetSysHandle *WidgetHandle,
        struct PI_DoubleInput *UICtrl)
{
    UIPI_FreeDoubleInput(UICtrl);
}

double PIUSDefault_GetDoubleInputValue(t_WidgetSysHandle *WidgetHandle,t_PIUIDoubleInputCtrl *DoubleInput)
{
    t_UIDoubleInput *RealDoubleInput=(t_UIDoubleInput *)DoubleInput;

    return UIGetDoubleInputCtrlValue(RealDoubleInput);
}

void PIUSDefault_SetDoubleInputValue(t_WidgetSysHandle *WidgetHandle,t_PIUIDoubleInputCtrl *DoubleInput,double Value)
{
    t_UIDoubleInput *RealDoubleInput=(t_UIDoubleInput *)DoubleInput;

    UISetDoubleInputCtrlValue(RealDoubleInput,Value);
}

void PIUSDefault_EnableDoubleInput(t_WidgetSysHandle *WidgetHandle,t_PIUIDoubleInputCtrl *DoubleInput,PG_BOOL Enabled)
{
    t_UIDoubleInput *RealDoubleInput=(t_UIDoubleInput *)DoubleInput;

    UIEnableDoubleInputCtrl(RealDoubleInput,Enabled);
}


void PIUSDefault_SetDoubleInputMinMax(t_WidgetSysHandle *WidgetHandle,t_PIUIDoubleInputCtrl *DoubleInput,double Min,double Max)
{
    t_UIDoubleInput *RealDoubleInput=(t_UIDoubleInput *)DoubleInput;

    UISetDoubleInputCtrlMin(RealDoubleInput,Min);
    UISetDoubleInputCtrlMax(RealDoubleInput,Max);
}

void PIUSDefault_SetDoubleInputDecimals(t_WidgetSysHandle *WidgetHandle,t_PIUIDoubleInputCtrl *DoubleInput,int Points)
{
    t_UIDoubleInput *RealDoubleInput=(t_UIDoubleInput *)DoubleInput;

    UISetDoubleInputCtrlDecimals(RealDoubleInput,Points);
}

struct PI_ColumnViewInput *PIUSDefault_AddColumnViewInput(t_WidgetSysHandle *WidgetHandle,
        const char *Label,int Columns,const char *ColumnNames[],
        void (*EventCB)(const struct PICVEvent *Event,void *UserData),
        void *UserData)
{
    return UIPI_AddColumnViewInput((t_UILayoutContainerCtrl *)WidgetHandle,Label,
            Columns,ColumnNames,EventCB,UserData);
}

void PIUSDefault_FreeColumnViewInput(t_WidgetSysHandle *WidgetHandle,
        struct PI_ColumnViewInput *UICtrl)
{
    UIPI_FreeColumnViewInput(UICtrl);
}

void PIUSDefault_ColumnViewInputClear(t_WidgetSysHandle *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl)
{
    t_UIColumnView *RealColumnViewInput=(t_UIColumnView *)UICtrl;

    UIColumnViewClear(RealColumnViewInput);
}

void PIUSDefault_ColumnViewInputRemoveRow(t_WidgetSysHandle *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl,int Row)
{
    t_UIColumnView *RealColumnViewInput=(t_UIColumnView *)UICtrl;

    UIColumnViewRemoveRow(RealColumnViewInput,Row);
}

int PIUSDefault_ColumnViewInputAddRow(t_WidgetSysHandle *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl)
{
    t_UIColumnView *RealColumnViewInput=(t_UIColumnView *)UICtrl;

    return UIColumnViewAddRow(RealColumnViewInput);
}

void PIUSDefault_ColumnViewInputSetColumnText(t_WidgetSysHandle *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl,int Column,int Row,const char *Str)
{
    t_UIColumnView *RealColumnViewInput=(t_UIColumnView *)UICtrl;

    UIColumnViewSetColumnText(RealColumnViewInput,Column,Row,Str);
}

void PIUSDefault_ColumnViewInputSelectRow(t_WidgetSysHandle *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl,int Row)
{
    t_UIColumnView *RealColumnViewInput=(t_UIColumnView *)UICtrl;

    UIColumnViewSelectRow(RealColumnViewInput,Row);
}

void PIUSDefault_ColumnViewInputClearSelection(t_WidgetSysHandle *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl)
{
    t_UIColumnView *RealColumnViewInput=(t_UIColumnView *)UICtrl;

    UIColumnViewClearSelection(RealColumnViewInput);
}

struct PI_ButtonInput *PIUSDefault_AddButtonInput(t_WidgetSysHandle *WidgetHandle,
        const char *Label,
        void (*EventCB)(const struct PIButtonEvent *Event,void *UserData),
        void *UserData)
{
    return UIPI_AddButtonInput((t_UILayoutContainerCtrl *)WidgetHandle,Label,
            EventCB,UserData);
}

void PIUSDefault_FreeButtonInput(t_WidgetSysHandle *WidgetHandle,
        struct PI_ButtonInput *UICtrl)
{
    UIPI_FreeButtonInput(UICtrl);
}

struct PI_Indicator *PIUSDefault_AddIndicator(t_WidgetSysHandle *WidgetHandle,
        const char *Label)
{
    return UIPI_AddIndicator((t_UILayoutContainerCtrl *)WidgetHandle,Label);
}

void PIUSDefault_FreeIndicator(t_WidgetSysHandle *WidgetHandle,
        struct PI_Indicator *UICtrl)
{
    UIPI_FreeIndicator(UICtrl);
}

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
 *    FreeFileReqPathAndFile()
void PIUSDefault_FreeFileReqPathAndFile(char **Path,char **Filename)
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
struct PI_GroupBox *PIUSDefault_AddGroupBox(t_WidgetSysHandle *WidgetHandle,const char *Label)
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

