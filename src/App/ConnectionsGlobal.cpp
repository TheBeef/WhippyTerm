/*******************************************************************************
 * FILENAME: ConnectionsGlobal.cpp
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
#include "App/Connections.h"
#include "App/ConnectionsGlobal.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
class Connection *m_ActiveConnection;

/*******************************************************************************
 * NAME:
 *    Con_SetActiveConnection
 *
 * SYNOPSIS:
 *    void Con_SetActiveConnection(class Connection *ActiveCon);
 *
 * PARAMETERS:
 *    ActiveCon [I] -- The new active connection.  This can be NULL.
 *
 * FUNCTION:
 *    This is a helper function.  It is called from the connection class
 *    to tell us what is the active connection class.
 *
 *    This effects what connection class is called when the global functions
 *    are called.
 *
 *    Normally the connection class sets this calls some plugin/sub system
 *    (that uses the global functions) and then sets this back to NULL.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Con_SetActiveConnection(class Connection *ActiveCon)
{
    m_ActiveConnection=ActiveCon;
}

/*******************************************************************************
 * NAME:
 *    Con_GetCurrentProcessorData
 *
 * SYNOPSIS:
 *    struct ProcessorConData *Con_GetCurrentProcessorData(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the current processor connection data.  This should
 *    really only be used by the Data Processors System.
 *
 * RETURNS:
 *    A pointer to the processor connection data for the active connection
 *    (the connection that is involved in the current event).
 *    This will be NULL if there is no active connection.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
struct ProcessorConData *Con_GetCurrentProcessorData(void)
{
    if(m_ActiveConnection==NULL)
        return NULL;

    return m_ActiveConnection->GetCurrentProcessorData();
}

/*******************************************************************************
 * NAME:
 *    Con_WriteData
 *
 * SYNOPSIS:
 *    void Con_WriteData(const uint8_t *Data,int Bytes);
 *
 * PARAMETERS:
 *    Data [I] -- The data to send
 *    Bytes [I] -- The number of bytes to send
 *
 * FUNCTION:
 *    This function writes to the active connection (if there is one).
 *    It is the same as calling Connection::WriteData().
 *
 *    It assumes this will be triggered by a key press.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Connection::WriteData()
 ******************************************************************************/
void Con_WriteData(const uint8_t *Data,int Bytes)
{
    if(m_ActiveConnection==NULL)
        return;

    m_ActiveConnection->WriteData(Data,Bytes,e_ConWriteSource_Keyboard);
}

/*******************************************************************************
 * NAME:
 *    Con_WriteChar2Display
 *
 * SYNOPSIS:
 *    void Con_WriteChar2Display(uint8_t *Chr);
 *
 * PARAMETERS:
 *    Chr [I] -- The char to write (this is UTF8).  This is 0 term.
 *
 * FUNCTION:
 *    This function writes a char to the display buffer for the active
 *    connection.  This is the same as calling Connection::WriteChar2Display()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Con_WriteChar2Display(uint8_t *Chr)
{
    if(m_ActiveConnection==NULL)
        return;

    m_ActiveConnection->WriteChar2Display(Chr);
}

/*******************************************************************************
 * NAME:
 *    Con_InformOfConnected
 *
 * SYNOPSIS:
 *    void Con_InformOfConnected(uintptr_t ID);
 *
 * PARAMETERS:
 *    ID [I] -- The user data (a pointer to the connection class)
 *
 * FUNCTION:
 *    This is a helper function that the IO system called to tell us that
 *    a connection has had a change in the connection status (it is now
 *    connected)
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Con_InformOfConnected(uintptr_t ID)
{
    class Connection *Con=(class Connection *)ID;

    if(Con==NULL)
        return;

    Con->InformOfConnected();
}

/*******************************************************************************
 * NAME:
 *    Con_InformOfDisconnected
 *
 * SYNOPSIS:
 *    void Con_InformOfDisconnected(uintptr_t ID);
 *
 * PARAMETERS:
 *    ID [I] -- The user data (a pointer to the connection class)
 *
 * FUNCTION:
 *    This is a helper function that the IO system called to tell us that
 *    a connection has had a change in the connection status (it is now
 *    disconnected)
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Con_InformOfDisconnected(uintptr_t ID)
{
    class Connection *Con=(class Connection *)ID;

    if(Con==NULL)
        return;

    Con->InformOfDisconnected();
}

/*******************************************************************************
 * NAME:
 *    Con_InformOfDataAvaiable
 *
 * SYNOPSIS:
 *    bool Con_InformOfDataAvaiable(uintptr_t ID);
 *
 * PARAMETERS:
 *    ID [I] -- The user data (a pointer to the connection class)
 *
 * FUNCTION:
 *    This function is called to tell the connection that there is new data
 *    available to read from the driver.
 *
 * RETURNS:
 *    true -- We have more to process.  Schedule to call us again (after you
 *            have processed anything else you need to)
 *    false -- No more data to process.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
bool Con_InformOfDataAvaiable(uintptr_t ID)
{
    class Connection *Con=(class Connection *)ID;

    if(Con==NULL)
        return false;

    return Con->InformOfDataAvaiable();
}

/*******************************************************************************
 * NAME:
 *    Con_SetFGColor
 *
 * SYNOPSIS:
 *    void Con_SetFGColor(uint32_t FGColor);
 *
 * PARAMETERS:
 *    FGColor [I] -- The new forground color in the format 0xRRGGBB
 *
 * FUNCTION:
 *    This function sets the forground color.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Con_GetFGColor()
 ******************************************************************************/
void Con_SetFGColor(uint32_t FGColor)
{
    if(m_ActiveConnection==NULL)
        return;

    m_ActiveConnection->SetFGColor(FGColor);
}

/*******************************************************************************
 * NAME:
 *    Con_GetFGColor
 *
 * SYNOPSIS:
 *    uint32_t Con_GetFGColor(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the forground color.
 *
 * RETURNS:
 *    The forgound color in 0xRRGGBB format.
 *
 * SEE ALSO:
 *    Con_SetFGColor()
 ******************************************************************************/
uint32_t Con_GetFGColor(void)
{
    if(m_ActiveConnection==NULL)
        return 0;

    return m_ActiveConnection->GetFGColor();
}

/*******************************************************************************
 * NAME:
 *    Con_SetBGColor
 *
 * SYNOPSIS:
 *    void Con_SetBGColor(uint32_t BGColor);
 *
 * PARAMETERS:
 *    BGColor [I] -- The new background color in the format 0xRRGGBB
 *
 * FUNCTION:
 *    This function sets the background color.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Con_GetBGColor()
 ******************************************************************************/
void Con_SetBGColor(uint32_t BGColor)
{
    if(m_ActiveConnection==NULL)
        return;

    m_ActiveConnection->SetBGColor(BGColor);
}

/*******************************************************************************
 * NAME:
 *    Con_GetBGColor
 *
 * SYNOPSIS:
 *    uint32_t Con_GetBGColor(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the background color.
 *
 * RETURNS:
 *    The background color in 0xRRGGBB format.
 *
 * SEE ALSO:
 *    Con_SetBGColor()
 ******************************************************************************/
uint32_t Con_GetBGColor(void)
{
    if(m_ActiveConnection==NULL)
        return 0;

    return m_ActiveConnection->GetBGColor();
}

/*******************************************************************************
 * NAME:
 *    Con_GetSysColor
 *
 * SYNOPSIS:
 *    uint32_t Con_GetSysColor(e_SysColShadeType SysColShade,
 *              e_SysColType SysColor);
 *
 * PARAMETERS:
 *    SysColShade [I] -- What shade of the color to get
 *    SysColor [I] -- What color to get the RGB value for
 *
 * FUNCTION:
 *    This function gets the RGB values for a from the settings.  If the
 *    connection is overriding the global settings then it's values are used
 *    otherwise the global settings are used.
 *
 * RETURNS:
 *    An RGB value for the requested color.
 *
 * SEE ALSO:
 *    Con_GetSysDefaultColor()
 ******************************************************************************/
uint32_t Con_GetSysColor(e_SysColShadeType SysColShade,e_SysColType SysColor)
{
    if(SysColShade>=e_SysColShadeMAX)
        return 0;
    if(SysColor>=e_SysColMAX)
        return 0;

    if(m_ActiveConnection==NULL || !m_ActiveConnection->UsingCustomSettings)
        return g_Settings.DefaultConSettings.SysColors[SysColShade][SysColor];

    return m_ActiveConnection->CustomSettings.SysColors[SysColShade][SysColor];
}

/*******************************************************************************
 * NAME:
 *    Con_GetSysDefaultColor
 *
 * SYNOPSIS:
 *    uint32_t Con_GetSysDefaultColor(e_DefaultColorsType DefaultColor);
 *
 * PARAMETERS:
 *    DefaultColor [I] -- The background or forground color to get
 *
 * FUNCTION:
 *    This function gets the RGB value for the default background / forground
 *    color from settings.
 *
 *    If the connection is overriding the global settings then it's values
 *    are used otherwise the global settings are used.
 *
 * RETURNS:
 *    An RGB value for the requested color.
 *
 * SEE ALSO:
 *    Con_GetSysColor()
 ******************************************************************************/
uint32_t Con_GetSysDefaultColor(e_DefaultColorsType DefaultColor)
{
    if(DefaultColor>=e_DefaultColorsMAX)
        return 0;

    if(m_ActiveConnection==NULL || !m_ActiveConnection->UsingCustomSettings)
        return g_Settings.DefaultConSettings.DefaultColors[DefaultColor];

    return m_ActiveConnection->CustomSettings.DefaultColors[DefaultColor];
}

/*******************************************************************************
 * NAME:
 *    Con_SetULineColor
 *
 * SYNOPSIS:
 *    void Con_SetULineColor(uint32_t ULineColor);
 *
 * PARAMETERS:
 *    ULineColor [I] -- The new underline color in the format 0xRRGGBB
 *
 * FUNCTION:
 *    This function sets the underline color.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Con_GetULineColor()
 ******************************************************************************/
void Con_SetULineColor(uint32_t ULineColor)
{
    if(m_ActiveConnection==NULL)
        return;

    m_ActiveConnection->SetULineColor(ULineColor);
}

/*******************************************************************************
 * NAME:
 *    Con_GetULineColor
 *
 * SYNOPSIS:
 *    uint32_t Con_GetULineColor(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the underline color.
 *
 * RETURNS:
 *    The underline color in 0xRRGGBB format.
 *
 * SEE ALSO:
 *    Con_SetULineColor()
 ******************************************************************************/
uint32_t Con_GetULineColor(void)
{
    if(m_ActiveConnection==NULL)
        return 0;

    return m_ActiveConnection->GetULineColor();
}

/*******************************************************************************
 * NAME:
 *    Con_SetAttribs
 *
 * SYNOPSIS:
 *    void Con_SetAttribs(uint16_t Attribs);
 *
 * PARAMETERS:
 *    Attribs [I] -- The new attribs to use.  These are bit values where
 *                   the following bits are supported:
 *                      TXT_ATTRIB_UNDERLINE -- Underline the text
 *                      TXT_ATTRIB_UNDERLINE_DOUBLE -- Double underline the text
 *                      TXT_ATTRIB_UNDERLINE_DOTTED -- Dotted underline
 *                      TXT_ATTRIB_UNDERLINE_DASHED -- Dashed underline
 *                      TXT_ATTRIB_UNDERLINE_WAVY -- A wavy underline
 *                      TXT_ATTRIB_OVERLINE -- Put a line over the text
 *                      TXT_ATTRIB_LINETHROUGHT -- Put a line though the text
 *                      TXT_ATTRIB_BOLD -- Bold Text
 *                      TXT_ATTRIB_ITALIC -- Italic Text
 *                      TXT_ATTRIB_OUTLINE -- Draw an outline around the leters.
 *                      TXT_ATTRIB_ROUNDBOX -- Draw a inverted rounded box
 *                                             around the text (invert the
 *                                             colors)
 *
 * FUNCTION:
 *    This function sets the underline color.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Con_GetAttribs()
 ******************************************************************************/
void Con_SetAttribs(uint16_t Attribs)
{
    if(m_ActiveConnection==NULL)
        return;

    m_ActiveConnection->SetAttribs(Attribs);
}

/*******************************************************************************
 * NAME:
 *    Con_GetAttribs
 *
 * SYNOPSIS:
 *    uint16_t Con_GetAttribs(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function gets the currently applied attribs.  These are bit values.
 *
 * RETURNS:
 *    The attribs that are currently turned on.
 *
 * SEE ALSO:
 *    Con_SetAttribs()
 ******************************************************************************/
uint16_t Con_GetAttribs(void)
{
    if(m_ActiveConnection==NULL)
        return 0;

    return m_ActiveConnection->GetAttribs();
}

/*******************************************************************************
 * NAME:
 *    Con_DoFunction
 *
 * SYNOPSIS:
 *    void Con_DoFunction(e_ConFuncType Fn,uintptr_t Arg1,uintptr_t Arg2,
 *              uintptr_t Arg3,uintptr_t Arg4);
 *
 * PARAMETERS:
 *    Fn [I] -- The function to do.  See the functions in DataProcessSystem()
 *    Arg1 [I] -- Depends on 'Fn'
 *    Arg2 [I] -- Depends on 'Fn'
 *    Arg3 [I] -- Depends on 'Fn'
 *    Arg4 [I] -- Depends on 'Fn'
 *
 * FUNCTION:
 *    This function a function on the connection.  See DataProcessSystem()
 *    for a description of the functions (there is a different function for
 *    each one)
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void Con_DoFunction(e_ConFuncType Fn,uintptr_t Arg1,uintptr_t Arg2,
        uintptr_t Arg3,uintptr_t Arg4)
{
    if(m_ActiveConnection==NULL)
        return;

    m_ActiveConnection->DoFunction(Fn,Arg1,Arg2,Arg3,Arg4);
}

void Con_GetCursorXY(int32_t *RetCursorX,int32_t *RetCursorY)
{
    if(m_ActiveConnection==NULL)
    {
        *RetCursorX=0;
        *RetCursorY=0;
        return;
    }

    m_ActiveConnection->GetCursorXY(RetCursorX,RetCursorY);
}

void Con_InsertString(uint8_t *Str,uint32_t Len)
{
    if(m_ActiveConnection==NULL)
        return;

    m_ActiveConnection->InsertString(Str,Len);
}

void Con_GetScreenSize(int32_t *RetRows,int32_t *RetColumns)
{
    if(m_ActiveConnection==NULL)
    {
        *RetRows=0;
        *RetColumns=0;
        return;
    }

    m_ActiveConnection->GetScreenSize(RetRows,RetColumns);
}

