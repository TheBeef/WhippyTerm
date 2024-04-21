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
 *    use these functions using 't_WidgetSysHandle' as a 't_UIContainerCtrl'.
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
#include "App/PluginSupport/PluginUISupport.h"
#include "UI/UIPlugins.h"
#include "UI/UIControl.h"
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
    PIUSDefault_AllocRadioBttnGroup,
    PIUSDefault_FreeRadioBttnGroup,
    PIUSDefault_AddRadioBttn,
    PIUSDefault_FreeRadioBttn,
    PIUSDefault_IsRadioBttnChecked,
    PIUSDefault_SetRadioBttnChecked,
    PIUSDefault_GetComboBoxText,
    PIUSDefault_SetComboBoxText,
    PIUSDefault_AddCheckbox,
    PIUSDefault_FreeCheckbox,
    PIUSDefault_IsCheckboxChecked,
    PIUSDefault_SetCheckboxChecked,

    PIUSDefault_AddTextInput,
    PIUSDefault_FreeTextInput,
    PIUSDefault_GetTextInputText,
    PIUSDefault_SetTextInputText,

    PIUSDefault_AddNumberInput,
    PIUSDefault_FreeNumberInput,
    PIUSDefault_GetNumberInputValue,
    PIUSDefault_SetNumberInputValue,
    PIUSDefault_SetNumberInputMinMax,

    PIUSDefault_AddDoubleInput,
    PIUSDefault_FreeDoubleInput,
    PIUSDefault_GetDoubleInputValue,
    PIUSDefault_SetDoubleInputValue,
    PIUSDefault_SetDoubleInputMinMax,
    PIUSDefault_SetDoubleInputDecimals,

    PIUSDefault_EnableComboBox,
    PIUSDefault_EnableRadioBttn,
    PIUSDefault_EnableCheckbox,
    PIUSDefault_EnableTextInput,
    PIUSDefault_EnableNumberInput,
    PIUSDefault_EnableDoubleInput,
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
 *    be the container widget (t_UIContainerCtrl) that will be passed directly
 *    to the UI.
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
    return UIPI_AddComboBox((t_UIContainerCtrl *)WidgetHandle,
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

t_PI_RadioBttnGroup *PIUSDefault_AllocRadioBttnGroup(t_WidgetSysHandle *WidgetHandle,const char *Label)
{
    return UIPI_AllocRadioBttnGroup((t_UIContainerCtrl *)WidgetHandle,Label);
}

/*******************************************************************************
 * NAME:
 *    PIUSDefault_FreeRadioBttnGroup
 *
 * SYNOPSIS:
 *    static void PIUSDefault_FreeRadioBttnGroup(t_WidgetSysHandle *WidgetHandle,
 *              t_PI_RadioBttnGroup *UICtrl);
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
void PIUSDefault_FreeRadioBttnGroup(t_WidgetSysHandle *WidgetHandle,t_PI_RadioBttnGroup *UICtrl)
{
    UIPI_FreeRadioBttnGroup(UICtrl);
}

struct PI_RadioBttn *PIUSDefault_AddRadioBttn(t_WidgetSysHandle *WidgetHandle,t_PI_RadioBttnGroup *RBGroup,const char *Label,void (*EventCB)(const struct PIRBEvent *Event,void *UserData),void *UserData)
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
    return UIPI_AddCheckbox((t_UIContainerCtrl *)WidgetHandle,Label,EventCB,UserData);
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
    return UIPI_AddTextInput((t_UIContainerCtrl *)WidgetHandle,
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
    return UIPI_AddNumberInput((t_UIContainerCtrl *)WidgetHandle,Label,
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
    return UIPI_AddDoubleInput((t_UIContainerCtrl *)WidgetHandle,Label,
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
