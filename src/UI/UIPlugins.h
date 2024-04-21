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
t_PI_RadioBttnGroup *UIPI_AllocRadioBttnGroup(
        t_UIContainerCtrl *ContainerWidget,const char *Label);
void UIPI_FreeRadioBttnGroup(t_PI_RadioBttnGroup *UICtrl);
struct PI_RadioBttn *UIPI_AddRadioBttn(t_PI_RadioBttnGroup *RBGroup,
            const char *Label,void (*EventCB)(const struct PIRBEvent *Event,void *UserData),void *UserData);
void UIPI_FreeRadioBttn(struct PI_RadioBttn *UICtrl);
PG_BOOL UIPI_IsRadioBttnChecked(struct PI_RadioBttn *Bttn);
void UIPI_SetRadioBttnChecked(struct PI_RadioBttn *Bttn,PG_BOOL Checked);
void UIPI_EnableRadioBttn(struct PI_RadioBttn *Bttn,PG_BOOL Enabled);

struct PI_Checkbox *UIPI_AddCheckbox(t_UIContainerCtrl *ContainerWidget,
            const char *Label,
            void (*EventCB)(const struct PICheckboxEvent *Event,void *UserData),void *UserData);
void UIPI_FreeCheckbox(struct PI_Checkbox *UICtrl);

struct PI_TextInput *UIPI_AddTextInput(t_UIContainerCtrl *ContainerWidget,
        const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData);
void UIPI_FreeTextInput(struct PI_TextInput *UICtrl);

struct PI_NumberInput *UIPI_AddNumberInput(t_UIContainerCtrl *ContainerWidget,
        const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData);

struct PI_NumberInput *UIPI_AddNumberInput(t_UIContainerCtrl *ContainerWidget,
        const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData);
void UIPI_FreeNumberInput(struct PI_NumberInput *UICtrl);

struct PI_DoubleInput *UIPI_AddDoubleInput(t_UIContainerCtrl *ContainerWidget,
        const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData);
void UIPI_FreeDoubleInput(struct PI_DoubleInput *UICtrl);

#endif
