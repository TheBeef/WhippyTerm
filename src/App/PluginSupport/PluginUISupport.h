/*******************************************************************************
 * FILENAME: PluginUISupport.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    .h file
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
 * HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __PLUGINUISUPPORT_H_
#define __PLUGINUISUPPORT_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/Plugin.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
const struct PI_UIAPI *PIUSDefault_GetDefaultAPI(void);
struct PI_ComboBox *PIUSDefault_AddComboBox(t_WidgetSysHandle *WidgetHandle,PG_BOOL UserEditable,const char *Label,void (*EventCB)(const struct PICBEvent *Event,void *UserData),void *UserData);
void PIUSDefault_FreeComboBox(t_WidgetSysHandle *WidgetHandle,struct PI_ComboBox *UICtrl);
void PIUSDefault_ClearComboBox(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox);
void PIUSDefault_AddItem2ComboBox(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox,const char *Label,uintptr_t ID);
void PIUSDefault_SetComboBoxSelectedEntry(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox,uintptr_t ID);
uintptr_t PIUSDefault_GetComboBoxSelectedEntry(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox);
void PIUSDefault_EnableComboBox(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox,PG_BOOL Enabled);
struct PI_RadioBttnGroup *PIUSDefault_AllocRadioBttnGroup(t_WidgetSysHandle *WidgetHandle,const char *Label);
void PIUSDefault_FreeRadioBttnGroup(t_WidgetSysHandle *WidgetHandle,struct PI_RadioBttnGroup *UICtrl);
struct PI_RadioBttn *PIUSDefault_AddRadioBttn(t_WidgetSysHandle *WidgetHandle,struct PI_RadioBttnGroup *RBGroup,const char *Label,void (*EventCB)(const struct PIRBEvent *Event,void *UserData),void *UserData);
void PIUSDefault_FreeRadioBttn(t_WidgetSysHandle *WidgetHandle,struct PI_RadioBttn *UICtrl);
PG_BOOL PIUSDefault_IsRadioBttnChecked(t_WidgetSysHandle *WidgetHandle,struct PI_RadioBttn *Bttn);
void PIUSDefault_SetRadioBttnChecked(t_WidgetSysHandle *WidgetHandle,struct PI_RadioBttn *Bttn,PG_BOOL Checked);
void PIUSDefault_EnableRadioBttn(t_WidgetSysHandle *WidgetHandle,struct PI_RadioBttn *Bttn,PG_BOOL Enabled);
const char *PIUSDefault_GetComboBoxText(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox);
void PIUSDefault_SetComboBoxText(t_WidgetSysHandle *WidgetHandle,t_PIUIComboBoxCtrl *ComboBox,const char *Txt);
struct PI_Checkbox *PIUSDefault_AddCheckbox(t_WidgetSysHandle *WidgetHandle,const char *Label,void (*EventCB)(const struct PICheckboxEvent *Event,void *UserData),void *UserData);
void PIUSDefault_FreeCheckbox(t_WidgetSysHandle *WidgetHandle,struct PI_Checkbox *UICtrl);
PG_BOOL PIUSDefault_IsCheckboxChecked(t_WidgetSysHandle *WidgetHandle,t_PIUICheckboxCtrl *Bttn);
void PIUSDefault_SetCheckboxChecked(t_WidgetSysHandle *WidgetHandle,t_PIUICheckboxCtrl *Bttn,PG_BOOL Checked);
void PIUSDefault_EnableCheckbox(t_WidgetSysHandle *WidgetHandle,t_PIUICheckboxCtrl *Bttn,PG_BOOL Enabled);

struct PI_TextInput *PIUSDefault_AddTextInput(t_WidgetSysHandle *WidgetHandle,const char *Label,void (*EventCB)(const struct PICBEvent *Event,void *UserData),void *UserData);
void PIUSDefault_FreeTextInput(t_WidgetSysHandle *WidgetHandle,struct PI_TextInput *UICtrl);
const char *PIUSDefault_GetTextInputText(t_WidgetSysHandle *WidgetHandle,t_PIUITextInputCtrl *TextInput);
void PIUSDefault_SetTextInputText(t_WidgetSysHandle *WidgetHandle,t_PIUITextInputCtrl *TextInput,const char *Txt);
void PIUSDefault_EnableTextInput(t_WidgetSysHandle *WidgetHandle,t_PIUITextInputCtrl *TextInput,PG_BOOL Enabled);

struct PI_NumberInput *PIUSDefault_AddNumberInput(t_WidgetSysHandle *WidgetHandle,const char *Label,void (*EventCB)(const struct PICBEvent *Event,void *UserData),void *UserData);
void PIUSDefault_FreeNumberInput(t_WidgetSysHandle *WidgetHandle,struct PI_NumberInput *UICtrl);
uint64_t PIUSDefault_GetNumberInputValue(t_WidgetSysHandle *WidgetHandle,t_PIUINumberInputCtrl *NumberInput);
void PIUSDefault_SetNumberInputValue(t_WidgetSysHandle *WidgetHandle,t_PIUINumberInputCtrl *NumberInput,int64_t Value);
void PIUSDefault_EnableNumberInput(t_WidgetSysHandle *WidgetHandle,t_PIUINumberInputCtrl *NumberInput,PG_BOOL Enabled);
void PIUSDefault_SetNumberInputMinMax(t_WidgetSysHandle *WidgetHandle,t_PIUINumberInputCtrl *NumberInput,int64_t Min,int64_t Max);

struct PI_DoubleInput *PIUSDefault_AddDoubleInput(t_WidgetSysHandle *WidgetHandle,const char *Label,void (*EventCB)(const struct PICBEvent *Event,void *UserData),void *UserData);
void PIUSDefault_FreeDoubleInput(t_WidgetSysHandle *WidgetHandle,struct PI_DoubleInput *UICtrl);
double PIUSDefault_GetDoubleInputValue(t_WidgetSysHandle *WidgetHandle,t_PIUIDoubleInputCtrl *DoubleInput);
void PIUSDefault_SetDoubleInputValue(t_WidgetSysHandle *WidgetHandle,t_PIUIDoubleInputCtrl *DoubleInput,double Value);
void PIUSDefault_EnableDoubleInput(t_WidgetSysHandle *WidgetHandle,t_PIUIDoubleInputCtrl *DoubleInput,PG_BOOL Enabled);
void PIUSDefault_SetDoubleInputMinMax(t_WidgetSysHandle *WidgetHandle,t_PIUIDoubleInputCtrl *DoubleInput,double Min,double Max);
void PIUSDefault_SetDoubleInputDecimals(t_WidgetSysHandle *WidgetHandle,t_PIUIDoubleInputCtrl *DoubleInput,int Points);

struct PI_ColumnViewInput *PIUSDefault_AddColumnViewInput(t_WidgetSysHandle *WidgetHandle,const char *Label,int Columns,const char *ColumnNames[],void (*EventCB)(const struct PICVEvent *Event,void *UserData),void *UserData);
void PIUSDefault_FreeColumnViewInput(t_WidgetSysHandle *WidgetHandle,struct PI_ColumnViewInput *UICtrl);
void PIUSDefault_ColumnViewInputClear(t_WidgetSysHandle *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl);
void PIUSDefault_ColumnViewInputRemoveRow(t_WidgetSysHandle *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl,int Row);
int PIUSDefault_ColumnViewInputAddRow(t_WidgetSysHandle *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl);
void PIUSDefault_ColumnViewInputSetColumnText(t_WidgetSysHandle *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl,int Column,int Row,const char *Str);
void PIUSDefault_ColumnViewInputSelectRow(t_WidgetSysHandle *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl,int Row);
void PIUSDefault_ColumnViewInputClearSelection(t_WidgetSysHandle *WidgetHandle,t_PIUIColumnViewInputCtrl *UICtrl);

struct PI_ButtonInput *PIUSDefault_AddButtonInput(t_WidgetSysHandle *WidgetHandle,const char *Label,void (*EventCB)(const struct PIButtonEvent *Event,void *UserData),void *UserData);
void PIUSDefault_FreeButtonInput(t_WidgetSysHandle *WidgetHandle,struct PI_ButtonInput *UICtrl);

struct PI_Indicator *PIUSDefault_AddIndicator(t_WidgetSysHandle *WidgetHandle,const char *Label);
void PIUSDefault_FreeIndicator(t_WidgetSysHandle *WidgetHandle,struct PI_Indicator *UICtrl);
void PIUSDefault_SetIndicator(t_WidgetSysHandle *WidgetHandle,t_PIUIIndicatorCtrl *UICtrl,bool On);

int PIUSDefault_Ask(const char *Message,int Type);

PG_BOOL PIUSDefault_FileReq(e_FileReqTypeType Req,const char *Title,char **Path,char **Filename,const char *Filters,int SelectedFilter);
void PIUSDefault_FreeFileReqPathAndFile(char **Path,char **Filename);

struct PI_TextBox *PIUSDefault_AddTextBox(t_WidgetSysHandle *WidgetHandle,const char *Label,const char *Text);
void PIUSDefault_FreeTextBox(t_WidgetSysHandle *WidgetHandle,struct PI_TextBox *BoxHandle);
void PIUSDefault_SetTextBox(t_WidgetSysHandle *WidgetHandle,t_PIUITextBoxCtrl *UICtrl,const char *Text);

struct PI_GroupBox *PIUSDefault_AddGroupBox(t_WidgetSysHandle *WidgetHandle,const char *Label);
void PIUSDefault_FreeGroupBox(t_WidgetSysHandle *WidgetHandle,struct PI_GroupBox *BoxHandle);
void PIUSDefault_SetGroupBoxLabel(t_WidgetSysHandle *WidgetHandle,t_PIUIGroupBoxCtrl *UICtrl,const char *Label);

#endif
