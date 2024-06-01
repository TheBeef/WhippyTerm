//#define ALL_PLUGINS 1
/*******************************************************************************
 * FILENAME: RegisterStdPlugins.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file registers all the standard built in plugins.
 *    Standard plugins are like normal plugins except they don't get
 *    loaded from disk, but are instead compiled into the exe.
 *
 *    They use the same plugin API and regsiter them selfs the same as normal
 *    plugins.
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
#include "App/IOSystem.h"
#include "App/PluginSupport/SystemSupport.h"
#include "App/StdPlugins/RegisterStdPlugins.h"
#include "Version.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/* List of built in plugins */
extern "C"
{
    unsigned int Comport_RegisterPlugin(const struct PI_SystemAPI *SysAPI,unsigned int Version);
    unsigned int CodePage437Decode_RegisterPlugin(const struct PI_SystemAPI *SysAPI,unsigned int Version);
    unsigned int UnicodeDecoder_RegisterPlugin(const struct PI_SystemAPI *SysAPI,unsigned int Version);
    unsigned int ANSIX3_64_RegisterPlugin(const struct PI_SystemAPI *SysAPI,unsigned int Version);
    unsigned int BasicCtrlCharsDecoder_RegisterPlugin(const struct PI_SystemAPI *SysAPI,unsigned int Version);
    unsigned int RAWFileUpload_RegisterPlugin(const struct PI_SystemAPI *SysAPI,unsigned int Version);
    unsigned int XModemUpload_RegisterPlugin(const struct PI_SystemAPI *SysAPI,unsigned int Version);
    unsigned int TCPClient_RegisterPlugin(const struct PI_SystemAPI *SysAPI,unsigned int Version);
    unsigned int TCPServer_RegisterPlugin(const struct PI_SystemAPI *SysAPI,unsigned int Version);
    unsigned int UDPClient_RegisterPlugin(const struct PI_SystemAPI *SysAPI,unsigned int Version);
    unsigned int UDPServer_RegisterPlugin(const struct PI_SystemAPI *SysAPI,unsigned int Version);

#ifdef ALL_PLUGINS
    /* For testing we do all plugins */
    unsigned int HTTPClient_RegisterPlugin(const struct PI_SystemAPI *SysAPI,unsigned int Version);
    unsigned int RemoteSPI_RegisterPlugin(const struct PI_SystemAPI *SysAPI,unsigned int Version);
    unsigned int TestLB_RegisterPlugin(const struct PI_SystemAPI *SysAPI,unsigned int Version);
    unsigned int TestFile_RegisterPlugin(const struct PI_SystemAPI *SysAPI,unsigned int Version);
#endif
    
    unsigned int BASIC_HEX_RegisterPlugin(const struct PI_SystemAPI *SysAPI,unsigned int Version);
}

/*** VARIABLE DEFINITIONS     ***/

/*******************************************************************************
 * NAME:
 *    RegisterStdPlugins
 *
 * SYNOPSIS:
 *    void RegisterStdPlugins(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function registers all the standard plugins in the system.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void RegisterStdPlugins(void)
{
    /* IO Drivers */
    Comport_RegisterPlugin(&g_PISystemAPI,WHIPPYTERM_VERSION);

    /* Term Emulation */
    ANSIX3_64_RegisterPlugin(&g_PISystemAPI,WHIPPYTERM_VERSION);
    BasicCtrlCharsDecoder_RegisterPlugin(&g_PISystemAPI,WHIPPYTERM_VERSION);

    /* e_TextDataProcessorClass_CharEncoding */
    CodePage437Decode_RegisterPlugin(&g_PISystemAPI,WHIPPYTERM_VERSION);
    UnicodeDecoder_RegisterPlugin(&g_PISystemAPI,WHIPPYTERM_VERSION);
    
    BASIC_HEX_RegisterPlugin(&g_PISystemAPI,WHIPPYTERM_VERSION);
    
    /* File Transfer Protocols */
    RAWFileUpload_RegisterPlugin(&g_PISystemAPI,WHIPPYTERM_VERSION);
    XModemUpload_RegisterPlugin(&g_PISystemAPI,WHIPPYTERM_VERSION);

    TCPClient_RegisterPlugin(&g_PISystemAPI,WHIPPYTERM_VERSION);
    TCPServer_RegisterPlugin(&g_PISystemAPI,WHIPPYTERM_VERSION);

    UDPClient_RegisterPlugin(&g_PISystemAPI,WHIPPYTERM_VERSION);
    UDPServer_RegisterPlugin(&g_PISystemAPI,WHIPPYTERM_VERSION);

#ifdef ALL_PLUGINS
    /* For testing we do all plugins */
    HTTPClient_RegisterPlugin(&g_PISystemAPI,WHIPPYTERM_VERSION);
    RemoteSPI_RegisterPlugin(&g_PISystemAPI,WHIPPYTERM_VERSION);
    TestLB_RegisterPlugin(&g_PISystemAPI,WHIPPYTERM_VERSION);
    TestFile_RegisterPlugin(&g_PISystemAPI,WHIPPYTERM_VERSION);
#endif
}
