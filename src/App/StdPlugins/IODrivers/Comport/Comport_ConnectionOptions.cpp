/*******************************************************************************
 * FILENAME: Comport_ConnectionOptions.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
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
#include "Comport_ConnectionOptions.h"
#include "Comport_Main.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct Comport_OptionWidgets
{
    struct PI_ComboBox *BaudRate;
    struct PI_ComboBox *DataBits;
    struct PI_ComboBox *Parity;
    struct PI_ComboBox *StopBits;
    struct PI_ComboBox *FlowControl;
};

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    Comport_ConnectionOptionsWidgets_AllocWidgets
 *
 * SYNOPSIS:
 *    t_ConnectionOptionsWidgetsType *Comport_ConnectionOptionsWidgets_AllocWidgets(
 *          t_WidgetSysHandle *WidgetHandle);
 *
 * PARAMETERS:
 *    ID [I] -- The ID of the device
 *
 * FUNCTION:
 *    This function adds options widgets to a container widget.  These are
 *    options for the connection.  It's things like bit rate, parity, or
 *    any other options the device supports.
 *
 *    The device driver needs to keep handles to the widgets added because it
 *    needs to free them when RemoveNewConnectionOptionsWidgets() called.
 *
 * RETURNS:
 *    The private options data that you want to use.  This is a private
 *    structure that you allocate and then cast to
 *    (t_ConnectionOptionsWidgetsType *) when you return.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_ConnectionOptionsWidgetsType *Comport_ConnectionOptionsWidgets_AllocWidgets(
        t_WidgetSysHandle *WidgetHandle)
{
    struct Comport_OptionWidgets *Widgets;

    Widgets=NULL;
    try
    {
        Widgets=new struct Comport_OptionWidgets;
        Widgets->BaudRate=NULL;
        Widgets->DataBits=NULL;
        Widgets->Parity=NULL;
        Widgets->StopBits=NULL;
        Widgets->FlowControl=NULL;

        Widgets->BaudRate=g_CP_UI->AddComboBox(WidgetHandle,false,
                "Baud rate",NULL,NULL);
        if(Widgets->BaudRate==NULL)
            throw(0);

        Widgets->DataBits=g_CP_UI->AddComboBox(WidgetHandle,false,
                "Data Bits",NULL,NULL);
        if(Widgets->DataBits==NULL)
            throw(0);

        Widgets->Parity=g_CP_UI->AddComboBox(WidgetHandle,false,
                "Parity",NULL,NULL);
        if(Widgets->Parity==NULL)
            throw(0);

        Widgets->StopBits=g_CP_UI->AddComboBox(WidgetHandle,false,
                "Stop Bits",NULL,NULL);
        if(Widgets->StopBits==NULL)
            throw(0);

        Widgets->FlowControl=g_CP_UI->AddComboBox(WidgetHandle,false,
                "Flow Control",NULL,NULL);
        if(Widgets->FlowControl==NULL)
            throw(0);

        g_CP_UI->ClearComboBox(WidgetHandle,Widgets->BaudRate->Ctrl);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->BaudRate->Ctrl,"110",110);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->BaudRate->Ctrl,"300",300);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->BaudRate->Ctrl,"600",600);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->BaudRate->Ctrl,"1200",1200);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->BaudRate->Ctrl,"2400",2400);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->BaudRate->Ctrl,"4800",4800);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->BaudRate->Ctrl,"9600",9600);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->BaudRate->Ctrl,"19200",19200);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->BaudRate->Ctrl,"38400",38400);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->BaudRate->Ctrl,"57600",57600);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->BaudRate->Ctrl,"115200",115200);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->BaudRate->Ctrl,"230400",230400);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->BaudRate->Ctrl,"460800",460800);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->BaudRate->Ctrl,"921600",921600);

        g_CP_UI->ClearComboBox(WidgetHandle,Widgets->DataBits->Ctrl);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->DataBits->Ctrl,"7 Bit",e_ComportDataBits_7);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->DataBits->Ctrl,"8 Bit",e_ComportDataBits_8);

        g_CP_UI->ClearComboBox(WidgetHandle,Widgets->Parity->Ctrl);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->Parity->Ctrl,"None",e_ComportParity_none);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->Parity->Ctrl,"Odd",e_ComportParity_odd);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->Parity->Ctrl,"Even",e_ComportParity_even);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->Parity->Ctrl,"Mark",e_ComportParity_mark);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->Parity->Ctrl,"Space",e_ComportParity_space);

        g_CP_UI->ClearComboBox(WidgetHandle,Widgets->StopBits->Ctrl);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->StopBits->Ctrl,"1 Bit",e_ComportStopBits_1);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->StopBits->Ctrl,"2 Bit",e_ComportStopBits_2);

        g_CP_UI->ClearComboBox(WidgetHandle,Widgets->FlowControl->Ctrl);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->FlowControl->Ctrl,"None",e_ComportFlowControl_None);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->FlowControl->Ctrl,"XON/XOFF",e_ComportFlowControl_XONXOFF);
        g_CP_UI->AddItem2ComboBox(WidgetHandle,Widgets->FlowControl->Ctrl,"Hardware",e_ComportFlowControl_Hardware);
    }
    catch(...)
    {
        if(Widgets!=NULL)
        {
            if(Widgets->BaudRate!=NULL)
                g_CP_UI->FreeComboBox(WidgetHandle,Widgets->BaudRate);

            if(Widgets->DataBits!=NULL)
                g_CP_UI->FreeComboBox(WidgetHandle,Widgets->DataBits);

            if(Widgets->Parity!=NULL)
                g_CP_UI->FreeComboBox(WidgetHandle,Widgets->Parity);

            if(Widgets->StopBits!=NULL)
                g_CP_UI->FreeComboBox(WidgetHandle,Widgets->StopBits);

            if(Widgets->FlowControl!=NULL)
                g_CP_UI->FreeComboBox(WidgetHandle,Widgets->FlowControl);

            delete Widgets;
        }
        return NULL;
    }

    return (t_ConnectionOptionsWidgetsType *)Widgets;
}

/*******************************************************************************
 * NAME:
 *    Comport_ConnectionOptionsWidgets_FreeWidgets
 *
 * SYNOPSIS:
 *    void Comport_ConnectionOptionsWidgets_FreeWidgets(t_ConnectionOptionsWidgetsType *
 *              ConOptions,t_WidgetSysHandle *WidgetHandle);
 *
 * PARAMETERS:
 *    ConOptions [I] -- The options data that was allocated with
 *          ConnectionOptionsWidgets_AllocWidgets().
 *    WidgetHandle [I] -- The handle to send to the widgets
 *
 * FUNCTION:
 *    Frees the widgets added with ConnectionOptionsWidgets_AllocWidgets()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Comport_ConnectionOptionsWidgets_AllocWidgets()
 ******************************************************************************/
void Comport_ConnectionOptionsWidgets_FreeWidgets(
        t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle)
{
    struct Comport_OptionWidgets *Widgets=(struct Comport_OptionWidgets *)ConOptions;

    g_CP_UI->FreeComboBox(WidgetHandle,Widgets->BaudRate);
    g_CP_UI->FreeComboBox(WidgetHandle,Widgets->DataBits);
    g_CP_UI->FreeComboBox(WidgetHandle,Widgets->Parity);
    g_CP_UI->FreeComboBox(WidgetHandle,Widgets->StopBits);
    g_CP_UI->FreeComboBox(WidgetHandle,Widgets->FlowControl);

    delete Widgets;
}

/*******************************************************************************
 * NAME:
 *    Comport_ConnectionOptionsWidgets_StoreUI
 *
 * SYNOPSIS:
 *      void Comport_ConnectionOptionsWidgets_StoreUI(
 *              t_ConnectionOptionsWidgetsType *ConOptions,
 *              t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
 *              t_PIKVList *Options);
 *
 * PARAMETERS:
 *    ConOptions [I] -- The options data that was allocated with
 *          ConnectionOptionsWidgets_AllocWidgets().
 *    WidgetHandle [I] -- The handle to send to the widgets
 *    DeviceUniqueID [I] -- This is the unique ID for the device we are working
 *                          on.
 *    Options [O] -- The options for this connection.
 *
 * FUNCTION:
 *    This function takes the widgets added with
 *    ConnectionOptionsWidgets_AllocWidgets() and stores them is a key/value pair
 *    list in 'Options'.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Comport_ConnectionOptionsWidgets_StoreUI(t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
        t_PIKVList *Options)
{
    struct Comport_OptionWidgets *Widgets=(struct Comport_OptionWidgets *)ConOptions;
    struct ComportPortOptions PortOptions;

    PortOptions.BitRate=g_CP_UI->GetComboBoxSelectedEntry(WidgetHandle,
            Widgets->BaudRate->Ctrl);
    PortOptions.DataBits=(e_ComportDataBitsType)g_CP_UI->
            GetComboBoxSelectedEntry(WidgetHandle,Widgets->DataBits->Ctrl);
    PortOptions.Parity=(e_ComportParityType)g_CP_UI->
            GetComboBoxSelectedEntry(WidgetHandle,Widgets->Parity->Ctrl);
    PortOptions.StopBits=(e_ComportStopBitsType)g_CP_UI->
            GetComboBoxSelectedEntry(WidgetHandle,Widgets->StopBits->Ctrl);
    PortOptions.FlowControl=(e_ComportFlowControlType)g_CP_UI->
            GetComboBoxSelectedEntry(WidgetHandle,Widgets->FlowControl->Ctrl);

    Comport_Convert2KVList(&PortOptions,Options);
}

/*******************************************************************************
 * NAME:
 *    Comport_ConnectionOptionsWidgets_UpdateUI
 *
 * SYNOPSIS:
 *    void Comport_ConnectionOptionsWidgets_UpdateUI(
 *          t_ConnectionOptionsWidgetsType *ConOptions,
 *          t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
 *          t_PIKVList *Options);
 *
 * PARAMETERS:
 *    ConOptions [I] -- The options data that was allocated with
 *                      ConnectionOptionsWidgets_AllocWidgets().
 *    WidgetHandle [I] -- The handle to send to the widgets
 *    DeviceUniqueID [I] -- This is the unique ID for the device we are working
 *                          on.
 *    Options [I] -- The options for this connection.
 *
 * FUNCTION:
 *    This function takes the widgets added with
 *    ConnectionOptionsWidgets_AllocWidgets() and sets them to the values stored in
 *    the key/value pair list in 'Options'.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    ConnectionOptionsWidgets_StoreUI()
 ******************************************************************************/
void Comport_ConnectionOptionsWidgets_UpdateUI(
        t_ConnectionOptionsWidgetsType *ConOptions,
        t_WidgetSysHandle *WidgetHandle,const char *DeviceUniqueID,
        t_PIKVList *Options)
{
    struct Comport_OptionWidgets *Widgets=(struct Comport_OptionWidgets *)ConOptions;
    struct ComportPortOptions PortOptions;

    Comport_ConvertFromKVList(&PortOptions,Options);

    g_CP_UI->SetComboBoxSelectedEntry(WidgetHandle,Widgets->BaudRate->Ctrl,
            PortOptions.BitRate);
    g_CP_UI->SetComboBoxSelectedEntry(WidgetHandle,Widgets->DataBits->Ctrl,
            PortOptions.DataBits);
    g_CP_UI->SetComboBoxSelectedEntry(WidgetHandle,Widgets->Parity->Ctrl,
            PortOptions.Parity);
    g_CP_UI->SetComboBoxSelectedEntry(WidgetHandle,Widgets->StopBits->Ctrl,
            PortOptions.StopBits);
    g_CP_UI->SetComboBoxSelectedEntry(WidgetHandle,Widgets->FlowControl->Ctrl,
            PortOptions.FlowControl);
}

