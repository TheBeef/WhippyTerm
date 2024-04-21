/*******************************************************************************
 * FILENAME: QTPlugin.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the plugin related UI functions in it.
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
 *    Paul Hutchinson (11 Jan 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "UI/UIPlugins.h"
#include "QTPlugins.h"
#include <QFormLayout>
#include <QComboBox>
#include <QLabel>
#include <QButtonGroup>
#include <QRadioButton>
#include <QVBoxLayout>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct RadioBttnGroup
{
    QWidget *GroupWidget;
    QVBoxLayout *VerticalLayout;
    QLabel *Label;
};

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
/*******************************************************************************
 * NAME:
 *    UIPI_AddComboBox
 *
 * SYNOPSIS:
 *    struct PI_ComboBox *UIPI_AddComboBox(
 *              t_UIContainerCtrl *ContainerWidget,PG_BOOL UserEditable,
 *              const char *Label,void (*EventCB)(const struct PICBEvent *Event,
 *              void *UserData),void *UserData);
 *
 * PARAMETERS:
 *    ContainerWidget [I] -- The parent widget to add this combox box to
 *    UserEditable [I] -- Can the user type new strings into this box
 *    Label [I] -- The label to apply to this box
 *    EventCB [I] -- A callback for events.
 *    UserData [I] -- User data that will be sent to the 'EventCB'
 *
 * FUNCTION:
 *    This is standard version of add combox box.
 *
 * RETURNS:
 *    The combox box handle.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
struct PI_ComboBox *UIPI_AddComboBox(t_UIContainerCtrl *ContainerWidget,
        PG_BOOL UserEditable,const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData)
{
    struct PI_ComboBox *NewPIC;
    QFormLayout *Layout=(QFormLayout *)ContainerWidget;
    PIQComboBox *NewCtrl;
    QLabel *NewLabel;

    NewCtrl=NULL;
    NewLabel=NULL;
    try
    {
        NewPIC=new PI_ComboBox();
        NewCtrl=new PIQComboBox(Layout->parentWidget());
        NewLabel=new QLabel(Layout->parentWidget());

        NewLabel->setText(Label);
        NewCtrl->EventCB=EventCB;
        NewCtrl->UserData=UserData;
        NewCtrl->setEditable(UserEditable);

        Layout->addRow(NewLabel,NewCtrl);

        QObject::connect(NewCtrl,SIGNAL(currentIndexChanged(int)),NewCtrl,SLOT(ComboBoxChanged(int)));
        QObject::connect(NewCtrl,SIGNAL(editTextChanged(const QString &)),NewCtrl,SLOT(ComboBoxTextChanged(const QString &)));

        NewPIC->Ctrl=(t_PIUIComboBoxCtrl *)NewCtrl;
        NewPIC->Label=(t_PIUILabelCtrl *)NewLabel;
        NewPIC->UIData=NULL;
    }
    catch(...)
    {
        if(NewCtrl!=NULL)
            delete NewCtrl;
        if(NewLabel!=NULL)
            delete NewLabel;

        return NULL;
    }

    return (struct PI_ComboBox *)NewPIC;
}

void UIPI_FreeComboBox(struct PI_ComboBox *UICtrl)
{
    PIQComboBox *Ctrl;
    QLabel *Label;

    Ctrl=(PIQComboBox *)UICtrl->Ctrl;
    Label=(QLabel *)UICtrl->Label;

    delete Ctrl;
    delete Label;
    delete UICtrl;
}

void PIQComboBox::ComboBoxChanged(int index)
{
    struct PICBEvent Event;

    if(EventCB!=NULL)
    {
        Event.EventType=e_PIECB_IndexChanged;

        EventCB(&Event,UserData);
    }
}

void PIQComboBox::ComboBoxTextChanged(const QString &NewText)
{
    struct PICBEvent Event;

    if(EventCB!=NULL)
    {
        Event.EventType=e_PIECB_TextInputChanged;

        EventCB(&Event,UserData);
    }
}

/*******************************************************************************
 * NAME:
 *    UIPI_AllocRadioBttnGroup
 *
 * SYNOPSIS:
 *    static t_PI_RadioBttnGroup *UIPI_AllocRadioBttnGroup(
 *          t_UIContainerCtrl *ContainerWidget,const char *Label);
 *
 * PARAMETERS:
 *    ContainerWidget [I] -- The parent widget to add this combox box to
 *    Label [I] -- The lable for this group
 *
 * FUNCTION:
 *    This function allocates a group box for radio buttons.  Radio buttons
 *    can only live in a group box.
 *
 * RETURNS:
 *    A handle to the group that you can put radio buttons into.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_PI_RadioBttnGroup *UIPI_AllocRadioBttnGroup(
        t_UIContainerCtrl *ContainerWidget,const char *Label)
{
    QFormLayout *Layout=(QFormLayout *)ContainerWidget;
    struct RadioBttnGroup *NewGroup;

    NewGroup=NULL;
    try
    {
        NewGroup=new struct RadioBttnGroup;
        NewGroup->GroupWidget=NULL;
        NewGroup->VerticalLayout=NULL;
        NewGroup->Label=NULL;

        NewGroup->GroupWidget=new QWidget(Layout->parentWidget());
        NewGroup->VerticalLayout=new QVBoxLayout(NewGroup->GroupWidget);
        NewGroup->Label=new QLabel(Layout->parentWidget());;

        NewGroup->Label->setText(Label);

        Layout->addRow(NewGroup->Label,NewGroup->GroupWidget);
    }
    catch(...)
    {
        if(NewGroup!=NULL)
        {
            if(NewGroup->Label!=NULL)
                delete NewGroup->Label;

            if(NewGroup->VerticalLayout!=NULL)
                delete NewGroup->VerticalLayout;

            if(NewGroup->GroupWidget!=NULL)
                delete NewGroup->GroupWidget;

            delete NewGroup;
        }

        return NULL;
    }

    return (t_PI_RadioBttnGroup *)NewGroup;
}

void UIPI_FreeRadioBttnGroup(t_PI_RadioBttnGroup *UICtrl)
{
    struct RadioBttnGroup *Group=(struct RadioBttnGroup *)UICtrl;

    delete Group->Label;
    delete Group->VerticalLayout;
    delete Group->GroupWidget;

    delete Group;
}

struct PI_RadioBttn *UIPI_AddRadioBttn(t_PI_RadioBttnGroup *RBGroup,
            const char *Label,void (*EventCB)(const struct PIRBEvent *Event,void *UserData),void *UserData)
{
    struct RadioBttnGroup *Group=(struct RadioBttnGroup *)RBGroup;
    struct PI_RadioBttn *NewPIRB;
    PIQRadioButton *NewCtrl;

    NewPIRB=NULL;
    NewCtrl=NULL;
    try
    {
        NewPIRB=new PI_RadioBttn();
        NewCtrl=new PIQRadioButton(Label,Group->GroupWidget);

        NewCtrl->EventCB=EventCB;
        NewCtrl->UserData=UserData;
        NewCtrl->ThisBttn=NewPIRB;

        Group->VerticalLayout->addWidget(NewCtrl);

        QObject::connect(NewCtrl,SIGNAL(clicked(bool)),NewCtrl,SLOT(ButtonClicked(bool)));

        NewPIRB->Ctrl=(t_PIUIRadioBttnCtrl *)NewCtrl;
        NewPIRB->UIData=NULL;
    }
    catch(...)
    {
        if(NewPIRB!=NULL)
            delete NewPIRB;
        if(NewCtrl!=NULL)
            delete NewCtrl;

        return NULL;
    }

    return NewPIRB;
}

void UIPI_FreeRadioBttn(struct PI_RadioBttn *UICtrl)
{
    PIQRadioButton *Ctrl;

    Ctrl=(PIQRadioButton *)UICtrl->Ctrl;

    delete Ctrl;
    delete UICtrl;
}

PG_BOOL UIPI_IsRadioBttnChecked(struct PI_RadioBttn *Bttn)
{
    PIQRadioButton *Ctrl=(PIQRadioButton *)Bttn->Ctrl;

    return Ctrl->isChecked();
}

void UIPI_SetRadioBttnChecked(struct PI_RadioBttn *Bttn,PG_BOOL Checked)
{
    PIQRadioButton *Ctrl=(PIQRadioButton *)Bttn->Ctrl;

    Ctrl->setChecked(Checked);
}

void UIPI_EnableRadioBttn(struct PI_RadioBttn *Bttn,PG_BOOL Enabled)
{
    PIQRadioButton *Ctrl=(PIQRadioButton *)Bttn->Ctrl;

    Ctrl->setEnabled(Enabled);
}

void PIQRadioButton::ButtonClicked(bool checked)
{
    struct PIRBEvent Event;

    if(EventCB!=NULL)
    {
        Event.EventType=e_PIERB_Changed;
        Event.Bttn=ThisBttn;

        EventCB(&Event,UserData);
    }
}

struct PI_Checkbox *UIPI_AddCheckbox(t_UIContainerCtrl *ContainerWidget,
            const char *Label,void (*EventCB)(const struct PICheckboxEvent *Event,void *UserData),void *UserData)
{
    QFormLayout *Layout=(QFormLayout *)ContainerWidget;
    struct PI_Checkbox *NewPIRB;
    PIQCheckbox *NewCtrl;
    QLabel *NewLabel;

    NewPIRB=NULL;
    NewCtrl=NULL;
    try
    {
        NewPIRB=new PI_Checkbox();
        NewCtrl=new PIQCheckbox(Label,Layout->parentWidget());
        NewLabel=new QLabel(Layout->parentWidget());

        NewLabel->setText("");

        NewCtrl->EventCB=EventCB;
        NewCtrl->UserData=UserData;
        NewCtrl->ThisBttn=NewPIRB;

        Layout->addRow(NewLabel,NewCtrl);

        QObject::connect(NewCtrl,SIGNAL(clicked(bool)),NewCtrl,SLOT(ButtonClicked(bool)));

        NewPIRB->Ctrl=(t_PIUICheckboxCtrl *)NewCtrl;
        NewPIRB->Label=(t_PIUILabelCtrl *)NewLabel;
        NewPIRB->UIData=NULL;
    }
    catch(...)
    {
        if(NewPIRB!=NULL)
            delete NewPIRB;
        if(NewCtrl!=NULL)
            delete NewCtrl;

        return NULL;
    }

    return NewPIRB;
}

void UIPI_FreeCheckbox(struct PI_Checkbox *UICtrl)
{
    PIQCheckbox *Ctrl;
    QLabel *Label;

    Ctrl=(PIQCheckbox *)UICtrl->Ctrl;
    Label=(QLabel *)UICtrl->Label;

    delete Label;
    delete Ctrl;
    delete UICtrl;
}

void PIQCheckbox::ButtonClicked(bool checked)
{
    struct PICheckboxEvent Event;

    if(EventCB!=NULL)
    {
        Event.EventType=e_PIECheckbox_Changed;
        Event.CheckBox=ThisBttn;
        Event.Checked=checked;

        EventCB(&Event,UserData);
    }
}

/*******************************************************************************
 * NAME:
 *    UIPI_AddTextInput
 *
 * SYNOPSIS:
 *    struct PI_TextInput *UIPI_AddTextInput(
 *              t_UIContainerCtrl *ContainerWidget,PG_BOOL UserEditable,
 *              const char *Label,void (*EventCB)(const struct PICBEvent *Event,
 *              void *UserData),void *UserData);
 *
 * PARAMETERS:
 *    ContainerWidget [I] -- The parent widget to add this combox box to
 *    UserEditable [I] -- Can the user type new strings into this box
 *    Label [I] -- The label to apply to this box
 *    EventCB [I] -- A callback for events.
 *    UserData [I] -- User data that will be sent to the 'EventCB'
 *
 * FUNCTION:
 *    This is standard version of add combox box.
 *
 * RETURNS:
 *    The combox box handle.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
struct PI_TextInput *UIPI_AddTextInput(t_UIContainerCtrl *ContainerWidget,
        const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData)
{
    struct PI_TextInput *NewPIC;
    QFormLayout *Layout=(QFormLayout *)ContainerWidget;
    PIQTextInput *NewCtrl;
    QLabel *NewLabel;

    NewCtrl=NULL;
    NewLabel=NULL;
    try
    {
        NewPIC=new PI_TextInput();
        NewCtrl=new PIQTextInput(Layout->parentWidget());
        NewLabel=new QLabel(Layout->parentWidget());

        NewLabel->setText(Label);
        NewCtrl->EventCB=EventCB;
        NewCtrl->UserData=UserData;

        Layout->addRow(NewLabel,NewCtrl);

//        QObject::connect(NewCtrl,SIGNAL(textEdited(const QString &)),NewCtrl,SLOT(TextInputTextChanged(const QString &)));
        QObject::connect(NewCtrl,SIGNAL(textChanged(const QString &)),NewCtrl,SLOT(TextInputTextChanged(const QString &)));

        NewPIC->Ctrl=(t_PIUITextInputCtrl *)NewCtrl;
        NewPIC->Label=(t_PIUILabelCtrl *)NewLabel;
        NewPIC->UIData=NULL;
    }
    catch(...)
    {
        if(NewCtrl!=NULL)
            delete NewCtrl;
        if(NewLabel!=NULL)
            delete NewLabel;

        return NULL;
    }

    return (struct PI_TextInput *)NewPIC;
}

void UIPI_FreeTextInput(struct PI_TextInput *UICtrl)
{
    PIQTextInput *Ctrl;
    QLabel *Label;

    Ctrl=(PIQTextInput *)UICtrl->Ctrl;
    Label=(QLabel *)UICtrl->Label;

    delete Ctrl;
    delete Label;
    delete UICtrl;
}

void PIQTextInput::TextInputTextChanged(const QString &NewText)
{
    struct PICBEvent Event;

    if(EventCB!=NULL)
    {
        Event.EventType=e_PIECB_TextInputChanged;

        EventCB(&Event,UserData);
    }
}

/*******************************************************************************
 * NAME:
 *    UIPI_AddNumberInput
 *
 * SYNOPSIS:
 *    struct PI_NumberInput *UIPI_AddNumberInput(
 *              t_UIContainerCtrl *ContainerWidget,bool UseFloats,
 *              const char *Label,void (*EventCB)(const struct PICBEvent *Event,
 *              void *UserData),void *UserData);
 *
 * PARAMETERS:
 *    ContainerWidget [I] -- The parent widget to add this combox box to
 *    UseFloats [I] -- Are we using floating point or int?
 *    Label [I] -- The label to apply to this box
 *    EventCB [I] -- A callback for events.
 *    UserData [I] -- User data that will be sent to the 'EventCB'
 *
 * FUNCTION:
 *    This is standard version of number input box.
 *
 * RETURNS:
 *    The number input handle.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
struct PI_NumberInput *UIPI_AddNumberInput(t_UIContainerCtrl *ContainerWidget,
        const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData)
{
    struct PI_NumberInput *NewPIC;
    QFormLayout *Layout=(QFormLayout *)ContainerWidget;
    PIQNumberInput *NewCtrl;
    QLabel *NewLabel;

    NewCtrl=NULL;
    NewLabel=NULL;
    try
    {
        NewPIC=new PI_NumberInput();
        NewCtrl=new PIQNumberInput(Layout->parentWidget());
        NewLabel=new QLabel(Layout->parentWidget());

        NewLabel->setText(Label);
        NewCtrl->EventCB=EventCB;
        NewCtrl->UserData=UserData;

        Layout->addRow(NewLabel,NewCtrl);

        QObject::connect(NewCtrl,SIGNAL(valueChanged(int)),NewCtrl,SLOT(NumberInputChanged(int)));

        NewPIC->Ctrl=(t_PIUINumberInputCtrl *)NewCtrl;
        NewPIC->Label=(t_PIUILabelCtrl *)NewLabel;
        NewPIC->UIData=NULL;
    }
    catch(...)
    {
        if(NewCtrl!=NULL)
            delete NewCtrl;
        if(NewLabel!=NULL)
            delete NewLabel;

        return NULL;
    }

    return (struct PI_NumberInput *)NewPIC;
}

void UIPI_FreeNumberInput(struct PI_NumberInput *UICtrl)
{
    PIQNumberInput *Ctrl;
    QLabel *Label;

    Ctrl=(PIQNumberInput *)UICtrl->Ctrl;
    Label=(QLabel *)UICtrl->Label;

    delete Ctrl;
    delete Label;
    delete UICtrl;
}

void PIQNumberInput::NumberInputChanged(int NewNumber)
{
    struct PICBEvent Event;

    if(EventCB!=NULL)
    {
        Event.EventType=e_PIECB_TextInputChanged;

        EventCB(&Event,UserData);
    }
}

/*******************************************************************************
 * NAME:
 *    UIPI_AddDoubleInput
 *
 * SYNOPSIS:
 *    struct PI_DoubleInput *UIPI_AddDoubleInput(
 *              t_UIContainerCtrl *ContainerWidget,bool UseFloats,
 *              const char *Label,void (*EventCB)(const struct PICBEvent *Event,
 *              void *UserData),void *UserData);
 *
 * PARAMETERS:
 *    ContainerWidget [I] -- The parent widget to add this combox box to
 *    UseFloats [I] -- Are we using floating point or int?
 *    Label [I] -- The label to apply to this box
 *    EventCB [I] -- A callback for events.
 *    UserData [I] -- User data that will be sent to the 'EventCB'
 *
 * FUNCTION:
 *    This is standard version of double input box.
 *
 * RETURNS:
 *    The double input handle.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
struct PI_DoubleInput *UIPI_AddDoubleInput(t_UIContainerCtrl *ContainerWidget,
        const char *Label,
        void (*EventCB)(const struct PICBEvent *Event,void *UserData),
        void *UserData)
{
    struct PI_DoubleInput *NewPIC;
    QFormLayout *Layout=(QFormLayout *)ContainerWidget;
    PIQDoubleInput *NewCtrl;
    QLabel *NewLabel;

    NewCtrl=NULL;
    NewLabel=NULL;
    try
    {
        NewPIC=new PI_DoubleInput();
        NewCtrl=new PIQDoubleInput(Layout->parentWidget());
        NewLabel=new QLabel(Layout->parentWidget());

        NewLabel->setText(Label);
        NewCtrl->EventCB=EventCB;
        NewCtrl->UserData=UserData;

        Layout->addRow(NewLabel,NewCtrl);

        QObject::connect(NewCtrl,SIGNAL(valueChanged(double)),NewCtrl,SLOT(DoubleInputChanged(double)));

        NewPIC->Ctrl=(t_PIUIDoubleInputCtrl *)NewCtrl;
        NewPIC->Label=(t_PIUILabelCtrl *)NewLabel;
        NewPIC->UIData=NULL;
    }
    catch(...)
    {
        if(NewCtrl!=NULL)
            delete NewCtrl;
        if(NewLabel!=NULL)
            delete NewLabel;

        return NULL;
    }

    return (struct PI_DoubleInput *)NewPIC;
}

void UIPI_FreeDoubleInput(struct PI_DoubleInput *UICtrl)
{
    PIQDoubleInput *Ctrl;
    QLabel *Label;

    Ctrl=(PIQDoubleInput *)UICtrl->Ctrl;
    Label=(QLabel *)UICtrl->Label;

    delete Ctrl;
    delete Label;
    delete UICtrl;
}

void PIQDoubleInput::DoubleInputChanged(double NewDouble)
{
    struct PICBEvent Event;

    if(EventCB!=NULL)
    {
        Event.EventType=e_PIECB_TextInputChanged;

        EventCB(&Event,UserData);
    }
}
