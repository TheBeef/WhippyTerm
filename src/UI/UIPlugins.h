/*******************************************************************************
 * FILENAME: UIPlugins.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has functions that plugins can call to access the UI.
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
 *    Paul Hutchinson (10 Jul 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __UIPLUGINS_H_
#define __UIPLUGINS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "PluginSDK/PluginUI.h"
#include "UI/UIControl.h"

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
struct PI_ComboBox *UIPI_AddComboBox(t_UIContainerCtrl *ContainerWidget,
        PG_BOOL UserEditable,const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData);
void UIPI_FreeComboBox(struct PI_ComboBox *UICtrl);
void UIPI_ShowComboBox(struct PI_ComboBox *UICtrl,bool Show);

struct PI_RadioBttnGroup *UIPI_AllocRadioBttnGroup(
        t_UIContainerCtrl *ContainerWidget,const char *Label);
void UIPI_FreeRadioBttnGroup(struct PI_RadioBttnGroup *UICtrl);
void UIPI_ShowRadioBttnGroup(struct PI_RadioBttnGroup *UICtrl,bool Show);

struct PI_RadioBttn *UIPI_AddRadioBttn(struct PI_RadioBttnGroup *RBGroup,
            const char *Label,void (*EventCB)(const struct PIRBEvent *Event,void *UserData),void *UserData);
void UIPI_FreeRadioBttn(struct PI_RadioBttn *UICtrl);
PG_BOOL UIPI_IsRadioBttnChecked(struct PI_RadioBttn *Bttn);
void UIPI_SetRadioBttnChecked(struct PI_RadioBttn *Bttn,PG_BOOL Checked);
void UIPI_EnableRadioBttn(struct PI_RadioBttn *Bttn,PG_BOOL Enabled);
void UIPI_ShowRadioBttnInput(struct PI_RadioBttn *UICtrl,bool Show);

struct PI_Checkbox *UIPI_AddCheckbox(t_UIContainerCtrl *ContainerWidget,
            const char *Label,
            void (*EventCB)(const struct PICheckboxEvent *Event,void *UserData),void *UserData);
void UIPI_FreeCheckbox(struct PI_Checkbox *UICtrl);
void UIPI_ShowCheckboxInput(struct PI_Checkbox *UICtrl,bool Show);

struct PI_TextInput *UIPI_AddTextInput(t_UIContainerCtrl *ContainerWidget,
        const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData);
void UIPI_FreeTextInput(struct PI_TextInput *UICtrl);
void UIPI_ShowTextInput(struct PI_TextInput *UICtrl,bool Show);

struct PI_NumberInput *UIPI_AddNumberInput(t_UIContainerCtrl *ContainerWidget,
        const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData);
void UIPI_FreeNumberInput(struct PI_NumberInput *UICtrl);
void UIPI_ShowNumberInput(struct PI_NumberInput *UICtrl,bool Show);

struct PI_DoubleInput *UIPI_AddDoubleInput(t_UIContainerCtrl *ContainerWidget,
        const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData);
void UIPI_FreeDoubleInput(struct PI_DoubleInput *UICtrl);
void UIPI_ShowDoubleInput(struct PI_DoubleInput *UICtrl,bool Show);

struct PI_ColumnViewInput *UIPI_AddColumnViewInput(t_UIContainerCtrl *ContainerWidget,
        const char *Label,int Columns,const char *ColumnNames[],
        void (*EventCB)(const struct PICVEvent *Event,void *UserData),
        void *UserData);
void UIPI_FreeColumnViewInput(struct PI_ColumnViewInput *UICtrl);
void UIPI_ShowColumnViewInput(struct PI_ColumnViewInput *UICtrl,bool Show);

struct PI_ButtonInput *UIPI_AddButtonInput(t_UIContainerCtrl *ContainerWidget,
        const char *Label,
        void (*EventCB)(const struct PIButtonEvent *Event,void *UserData),
        void *UserData);
void UIPI_FreeButtonInput(struct PI_ButtonInput *UICtrl);
void UIPI_ShowButtonInput(struct PI_ButtonInput *UICtrl,bool Show);

struct PI_Indicator *UIPI_AddIndicator(t_UIContainerCtrl *ContainerWidget,
        const char *Label);
void UIPI_FreeIndicator(struct PI_Indicator *UICtrl);
void UIPI_ShowIndicator(struct PI_Indicator *UICtrl,bool Show);
void UIPI_SetIndicator(t_PIUIIndicatorCtrl *UICtrl,bool On);

struct PI_TextBox *UIPI_AddTextBox(t_UIContainerCtrl *ContainerWidget,const char *Label,const char *Text);
void UIPI_FreeTextBox(struct PI_TextBox *UICtrl);
void UIPI_ShowTextBox(struct PI_TextBox *UICtrl,bool Show);
void UIPI_SetTextBoxText(t_PIUITextBoxCtrl *UICtrl,const char *NewText);

struct PI_GroupBox *UIPI_AddGroupBox(t_UIContainerCtrl *ContainerWidget,const char *Label);
void UIPI_FreeGroupBox(struct PI_GroupBox *BoxHandle);
void UIPI_ShowGroupBox(struct PI_GroupBox *BoxHandle,bool Show);
void UIPI_SetGroupBoxLabel(t_PIUIGroupBoxCtrl *UICtrl,const char *NewLabel);

#endif
