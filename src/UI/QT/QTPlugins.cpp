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
#include <QPainter>

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

void UIPI_ShowComboBox(struct PI_ComboBox *UICtrl,bool Show)
{
    PIQComboBox *Ctrl;
    QLabel *Label;

    Ctrl=(PIQComboBox *)UICtrl->Ctrl;
    Label=(QLabel *)UICtrl->Label;

    Ctrl->setVisible(Show);
    Label->setVisible(Show);
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
 *    static struct PI_RadioBttnGroup *UIPI_AllocRadioBttnGroup(
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
struct PI_RadioBttnGroup *UIPI_AllocRadioBttnGroup(
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

    return (struct PI_RadioBttnGroup *)NewGroup;
}

void UIPI_FreeRadioBttnGroup(struct PI_RadioBttnGroup *UICtrl)
{
    struct RadioBttnGroup *Group=(struct RadioBttnGroup *)UICtrl;

    delete Group->Label;
    delete Group->VerticalLayout;
    delete Group->GroupWidget;

    delete Group;
}

void UIPI_ShowRadioBttnGroup(struct PI_RadioBttnGroup *UICtrl,bool Show)
{
    struct RadioBttnGroup *Group=(struct RadioBttnGroup *)UICtrl;

    Group->GroupWidget->setVisible(Show);
    Group->Label->setVisible(Show);
}

struct PI_RadioBttn *UIPI_AddRadioBttn(struct PI_RadioBttnGroup *RBGroup,
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

void UIPI_ShowRadioBttnInput(struct PI_RadioBttn *UICtrl,bool Show)
{
    PIQRadioButton *Ctrl;

    Ctrl=(PIQRadioButton *)UICtrl->Ctrl;

    Ctrl->setVisible(Show);
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

void UIPI_ShowCheckboxInput(struct PI_Checkbox *UICtrl,bool Show)
{
    PIQCheckbox *Ctrl;
    QLabel *Label;

    Ctrl=(PIQCheckbox *)UICtrl->Ctrl;
    Label=(QLabel *)UICtrl->Label;

    Ctrl->setVisible(Show);
    Label->setVisible(Show);
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

void UIPI_ShowTextInput(struct PI_TextInput *UICtrl,bool Show)
{
    PIQTextInput *Ctrl;
    QLabel *Label;

    Ctrl=(PIQTextInput *)UICtrl->Ctrl;
    Label=(QLabel *)UICtrl->Label;

    Ctrl->setVisible(Show);
    Label->setVisible(Show);
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

void UIPI_ShowNumberInput(struct PI_NumberInput *UICtrl,bool Show)
{
    PIQNumberInput *Ctrl;
    QLabel *Label;

    Ctrl=(PIQNumberInput *)UICtrl->Ctrl;
    Label=(QLabel *)UICtrl->Label;

    Ctrl->setVisible(Show);
    Label->setVisible(Show);
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

void UIPI_ShowDoubleInput(struct PI_DoubleInput *UICtrl,bool Show)
{
    PIQDoubleInput *Ctrl;
    QLabel *Label;

    Ctrl=(PIQDoubleInput *)UICtrl->Ctrl;
    Label=(QLabel *)UICtrl->Label;

    Ctrl->setVisible(Show);
    Label->setVisible(Show);
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

/*******************************************************************************
 * NAME:
 *    UIPI_AddColumnViewInput
 *
 * SYNOPSIS:
 *    struct PI_ColumnViewInput *UIPI_AddColumnViewInput(
 *              t_UIContainerCtrl *ContainerWidget,const char *Label,
 *              int Columns,const char *ColumnNames[],
 *              void (*EventCB)(const struct PICVEvent *Event,void *UserData),
 *              void *UserData);
 *
 * PARAMETERS:
 *    ContainerWidget [I] -- The parent widget to add this combox box to
 *    Label [I] -- The label to apply to this box
 *    Columns [I] -- The number of columns this widget will have
 *    ColumnNames [I] -- This is an array of pointers to the names of
 *                       the columns.  This will be in a header at the top.
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
 *    UIPI_FreeColumnViewInput()
 ******************************************************************************/
struct PI_ColumnViewInput *UIPI_AddColumnViewInput(t_UIContainerCtrl *ContainerWidget,
        const char *Label,int Columns,const char *ColumnNames[],
        void (*EventCB)(const struct PICVEvent *Event,void *UserData),
        void *UserData)
{
    struct PI_ColumnViewInput *NewPIC;
    QFormLayout *Layout=(QFormLayout *)ContainerWidget;
    PIQColumnViewInput *NewCtrl;
    QLabel *NewLabel;
    int col;
    QTreeWidgetItem *HeaderItem;

    NewCtrl=NULL;
    NewLabel=NULL;
    try
    {
        if(Columns<1)
            throw(0);

        NewPIC=new PI_ColumnViewInput();
        NewCtrl=new PIQColumnViewInput(Layout->parentWidget());

        HeaderItem=NewCtrl->headerItem();
        for(col=0;col<Columns;col++)
            HeaderItem->setText(col,ColumnNames[col]);

        NewCtrl->EventCB=EventCB;
        NewCtrl->UserData=UserData;
        NewLabel=new QLabel(Layout->parentWidget());

        NewLabel->setText(Label);

        Layout->addRow(NewLabel,NewCtrl);

        QObject::connect(NewCtrl,SIGNAL(itemSelectionChanged()),NewCtrl,SLOT(ColumnViewInputSelectionChanged()));

        NewPIC->Ctrl=(t_PIUIColumnViewInputCtrl *)NewCtrl;
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

    return (struct PI_ColumnViewInput *)NewPIC;
}

void UIPI_FreeColumnViewInput(struct PI_ColumnViewInput *UICtrl)
{
    PIQColumnViewInput *Ctrl;
    QLabel *Label;

    Ctrl=(PIQColumnViewInput *)UICtrl->Ctrl;
    Label=(QLabel *)UICtrl->Label;

    delete Ctrl;
    delete Label;
    delete UICtrl;
}

void UIPI_ShowColumnViewInput(struct PI_ColumnViewInput *UICtrl,bool Show)
{
    PIQColumnViewInput *Ctrl;
    QLabel *Label;

    Ctrl=(PIQColumnViewInput *)UICtrl->Ctrl;
    Label=(QLabel *)UICtrl->Label;

    Ctrl->setVisible(Show);
    Label->setVisible(Show);
}

void PIQColumnViewInput::ColumnViewInputSelectionChanged()
{
    QList<QTreeWidgetItem *> Sel;
    struct PICVEvent Event;
    int Row;

    Sel=this->selectedItems();
    if(!Sel.isEmpty())
        Row=this->indexOfTopLevelItem(Sel.first());
    else
        Row=-1;

    if(EventCB!=NULL)
    {
        Event.EventType=e_PIECV_IndexChanged;
        Event.Index=Row;

        EventCB(&Event,UserData);
    }
}

/*******************************************************************************
 * NAME:
 *    UIPI_AddButtonInput
 *
 * SYNOPSIS:
 *    struct PI_ButtonInput *UIPI_AddButtonInput(
 *              t_UIContainerCtrl *ContainerWidget,const char *Label,
 *              void (*EventCB)(const struct PIButtonEvent *Event,void *UserData),
 *              void *UserData);
 *
 * PARAMETERS:
 *    ContainerWidget [I] -- The parent widget to add this combox box to
 *    Label [I] -- The label to apply to this box
 *    EventCB [I] -- A callback for events.
 *    UserData [I] -- User data that will be sent to the 'EventCB'
 *
 * FUNCTION:
 *    This is standard version of button box.
 *
 * RETURNS:
 *    The button input handle.
 *
 * SEE ALSO:
 *    UIPI_FreeButtonInput()
 ******************************************************************************/
struct PI_ButtonInput *UIPI_AddButtonInput(t_UIContainerCtrl *ContainerWidget,
        const char *Label,
        void (*EventCB)(const struct PIButtonEvent *Event,void *UserData),
        void *UserData)
{
    struct PI_ButtonInput *NewPIC;
    QFormLayout *Layout=(QFormLayout *)ContainerWidget;
    PIQPushButtonInput *NewCtrl;
    QLabel *NewLabel;
    QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);

    NewCtrl=NULL;
    NewLabel=NULL;
    try
    {
        NewPIC=new PI_ButtonInput();
        NewCtrl=new PIQPushButtonInput(Label,Layout->parentWidget());

        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(NewCtrl->sizePolicy().hasHeightForWidth());
        NewCtrl->setSizePolicy(sizePolicy);

        NewCtrl->EventCB=EventCB;
        NewCtrl->UserData=UserData;
        NewLabel=new QLabel(Layout->parentWidget());

        NewLabel->setText("");

        Layout->addRow(NewLabel,NewCtrl);

        QObject::connect(NewCtrl,SIGNAL(clicked(bool)),NewCtrl,SLOT(Buttonclicked(bool)));

        NewPIC->Ctrl=(t_PIUIButtonInputCtrl *)NewCtrl;
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

    return (struct PI_ButtonInput *)NewPIC;
}

void UIPI_FreeButtonInput(struct PI_ButtonInput *UICtrl)
{
    PIQPushButtonInput *Ctrl;
    QLabel *Label;

    Ctrl=(PIQPushButtonInput *)UICtrl->Ctrl;
    Label=(QLabel *)UICtrl->Label;

    delete Ctrl;
    delete Label;
    delete UICtrl;
}

void UIPI_ShowButtonInput(struct PI_ButtonInput *UICtrl,bool Show)
{
    PIQPushButtonInput *Ctrl;
    QLabel *Label;

    Ctrl=(PIQPushButtonInput *)UICtrl->Ctrl;
    Label=(QLabel *)UICtrl->Label;

    Ctrl->setVisible(Show);
    Label->setVisible(Show);
}

void PIQPushButtonInput::Buttonclicked(bool checked)
{
    struct PIButtonEvent Event;

    if(EventCB!=NULL)
    {
        Event.EventType=e_PIEButton_Press;

        EventCB(&Event,UserData);
    }
}

/*******************************************************************************
 * NAME:
 *    UIPI_AddIndicator
 *
 * SYNOPSIS:
 *    struct PI_Indicator *UIPI_AddIndicator(
 *              t_UIContainerCtrl *ContainerWidget,const char *Label);
 *
 * PARAMETERS:
 *    ContainerWidget [I] -- The parent widget to add this combox box to
 *    Label [I] -- The label to apply to this box
 *
 * FUNCTION:
 *    This is standard version of an indicator.
 *
 * RETURNS:
 *    The indicator handle.
 *
 * SEE ALSO:
 *    UIPI_FreeIndicator()
 ******************************************************************************/
struct PI_Indicator *UIPI_AddIndicator(t_UIContainerCtrl *ContainerWidget,
        const char *Label)
{
    struct PI_Indicator *NewPIC;
    QFormLayout *Layout=(QFormLayout *)ContainerWidget;
    PIQIndicator *NewCtrl;
    QLabel *NewLabel;

    NewCtrl=NULL;
    NewLabel=NULL;
    try
    {
        NewPIC=new PI_Indicator();
        NewCtrl=new PIQIndicator(Layout->parentWidget());
        NewCtrl->On=false;
//NewCtrl->On=true;

        NewLabel=new QLabel(Layout->parentWidget());

        NewLabel->setText(Label);

        Layout->addRow(NewLabel,NewCtrl);

        NewPIC->Ctrl=(t_PIUIIndicatorCtrl *)NewCtrl;
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

    return (struct PI_Indicator *)NewPIC;
}

void UIPI_FreeIndicator(struct PI_Indicator *UICtrl)
{
    PIQIndicator *Ctrl;
    QLabel *Label;

    Ctrl=(PIQIndicator *)UICtrl->Ctrl;
    Label=(QLabel *)UICtrl->Label;

    delete Ctrl;
    delete Label;
    delete UICtrl;
}

void UIPI_SetIndicator(t_PIUIIndicatorCtrl *UICtrl,bool On)
{
    PIQIndicator *Ctrl=(PIQIndicator *)UICtrl;

    Ctrl->On=On;
    Ctrl->update();
}

void UIPI_ShowIndicator(struct PI_Indicator *UICtrl,bool Show)
{
    PIQIndicator *Ctrl;
    QLabel *Label;

    Ctrl=(PIQIndicator *)UICtrl->Ctrl;
    Label=(QLabel *)UICtrl->Label;

    Ctrl->setVisible(Show);
    Label->setVisible(Show);
}

void PIQIndicator::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QBrush brush;
    int BarWidth;
    int BarHeight;
    int Left;
    int Right;
    int Top;
    int Bottom;
    QColor BgColor;
    QColor TopColor;
    QColor BottomColor;
    int BaseColor;

    brush=painter.brush();

    BarWidth=this->width()-1;
    BarHeight=this->height()-1;

    Left=0;
    Top=0;
    Right=25;
    Bottom=10;

//    Left+=BarWidth/2-Right/2;
//    Right+=BarWidth/2-Right/2;

    Top+=BarHeight/2-Bottom/2;
    Bottom+=BarHeight/2-Bottom/2;

//FF -> E4 -> C9
//89 -> 6e -> 53
    if(On)
    {
        BaseColor=0xE4;
        BgColor.setRgb(BaseColor,0x00,0x00);
        TopColor.setRgb(BaseColor+0x1B,0x00,0x00);
        BottomColor.setRgb(BaseColor-0x1B,0x00,0x00);
    }
    else
    {
        BaseColor=0x6E;
        BgColor.setRgb(BaseColor,BaseColor,BaseColor);
        TopColor.setRgb(BaseColor+0x1B,BaseColor+0x1B,BaseColor+0x1B);
        BottomColor.setRgb(BaseColor-0x1B,BaseColor-0x1B,BaseColor-0x1B);
    }

    painter.setPen(QColor(0x00,0x00,0x00));
    painter.drawRect(Left,Top,Right-Left,Bottom-Top);

    brush.setColor(BgColor);
    brush.setStyle(Qt::SolidPattern);
    painter.fillRect(Left+2,Top+2,      Right-Left-3,Bottom-Top-3,brush);

    /* Draw the border */
    painter.setPen(TopColor);
    painter.drawLine(Left+1,Bottom-1,   Left+1,Top+1);
    painter.drawLine(Left+1,Top+1,      Right-1,Top+1);

    painter.setPen(BottomColor);
    painter.drawLine(Right-1,Top+2,     Right-1,Bottom-1);
    painter.drawLine(Left+2,Bottom-1,   Right-1,Bottom-1);

//        brush.setColor(QColor(0xE4,0x00,0x00));
//        brush.setStyle(Qt::SolidPattern);
//        painter.fillRect(Left+2,Top+2,      Right-Left-3,Bottom-Top-3,brush);
//
//        /* Draw the border */
//        painter.setPen(QColor(0xFF,0x00,0x00));
//        painter.drawLine(Left+1,Bottom-1,   Left+1,Top+1);
//        painter.drawLine(Left+1,Top+1,      Right-1,Top+1);
//
//        painter.setPen(QColor(0xC9,0x00,0x00));
//        painter.drawLine(Right-1,Top+2,     Right-1,Bottom-1);
//        painter.drawLine(Left+2,Bottom-1,   Right-1,Bottom-1);
//    }
//    else
//    {
//        brush.setColor(QColor(0x9E,0x9E,0x9E));
//        brush.setStyle(Qt::SolidPattern);
//        painter.fillRect(Left+2,Top+2,      Right-Left-3,Bottom-Top-3,brush);
//
//        /* Draw the border */
//        painter.setPen(QColor(0xB9,0xB9,0xB9));
//        painter.drawLine(Left+1,Bottom-1,   Left+1,Top+1);
//        painter.drawLine(Left+1,Top+1,      Right-1,Top+1);
//
//        painter.setPen(QColor(0x83,0x83,0x83));
//        painter.drawLine(Right-1,Top+2,     Right-1,Bottom-1);
//        painter.drawLine(Left+2,Bottom-1,   Right-1,Bottom-1);
//    }
}

