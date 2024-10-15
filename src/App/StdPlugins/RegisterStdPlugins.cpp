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
#include "App/MainApp.h"
#include "App/PluginSupport/SystemSupport.h"
#include "App/StdPlugins/RegisterStdPlugins.h"
#include "UI/UIAsk.h"
#include "Version.h"

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/
static void RegisterStdPlugin(unsigned int (*RegFn)(const struct PI_SystemAPI *SysAPI,
        unsigned int Version),const char *Name);

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
    unsigned int TestFile_RegisterPlugin(const struct PI_SystemAPI *SysAPI,unsigned int Version);
#endif

#ifdef DEBUG
    unsigned int TestLB_RegisterPlugin(const struct PI_SystemAPI *SysAPI,unsigned int Version);
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
    RegisterStdPlugin(ANSIX3_64_RegisterPlugin,"ANSIX3_64");
    RegisterStdPlugin(BasicCtrlCharsDecoder_RegisterPlugin,"BasicCtrlCharsDecoder");

    /* e_TextDataProcessorClass_CharEncoding */
    RegisterStdPlugin(CodePage437Decode_RegisterPlugin,"CodePage437Decode");
    RegisterStdPlugin(UnicodeDecoder_RegisterPlugin,"UnicodeDecoder");

    RegisterStdPlugin(BASIC_HEX_RegisterPlugin,"BASIC_HEX");

    /* File Transfer Protocols */
    RegisterStdPlugin(RAWFileUpload_RegisterPlugin,"RAWFileUpload");
    RegisterStdPlugin(XModemUpload_RegisterPlugin,"XModemUpload");

    RegisterStdPlugin(TCPClient_RegisterPlugin,"TCPClient");
    RegisterStdPlugin(TCPServer_RegisterPlugin,"TCPServer");

    RegisterStdPlugin(UDPClient_RegisterPlugin,"UDPClient");
    RegisterStdPlugin(UDPServer_RegisterPlugin,"UDPServer");

#ifdef ALL_PLUGINS
    /* For testing we do all plugins */
    RegisterStdPlugin(HTTPClient_RegisterPlugin,"HTTPClient");
    RegisterStdPlugin(RemoteSPI_RegisterPlugin,"RemoteSPI");
    RegisterStdPlugin(TestFile_RegisterPlugin,"TestFile");
#endif

#ifdef DEBUG
    RegisterStdPlugin(TestLB_RegisterPlugin,"TestLB");
#endif
}

/*******************************************************************************
 * NAME:
 *    RegisterStdPlugin
 *
 * SYNOPSIS:
 *    static void RegisterStdPlugin(unsigned int (*RegFn)(const struct PI_SystemAPI *SysAPI,
 *              unsigned int Version),const char *Name)
 *
 * PARAMETERS:
 *    RegFn [I] -- The register function to call.
 *    Name [I] -- The name of the plugin (for errors)
 *
 * FUNCTION:
 *    This function registers a build in standard plugin.  It will call the
 *    register function and display any errors that come up.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void RegisterStdPlugin(unsigned int (*RegFn)(const struct PI_SystemAPI *SysAPI,
        unsigned int Version),const char *Name)
{
    unsigned int ReqVer;
    uint8_t Maj;
    uint8_t Min;
    uint8_t Rev;
    uint8_t Patch;
    const char *Msg;
    char buff[200];

    ReqVer=RegFn(&g_PISystemAPI,WHIPPYTERM_VERSION);
    if(ReqVer!=0)
    {
        /* Tell user what version of WhippyTerm is needed */
        if(ReqVer==0xFFFFFFFF)
        {
            Msg="The internal plugin %s used the experimental plugin API and "
                    "is not supported by this version of " WHIPPYTERM_NAME;
        }
        else
        {
            Maj=(ReqVer>>24)&0xFF;
            Min=(ReqVer>>16)&0xFF;
            Rev=(ReqVer>>8)&0xFF;
            Patch=ReqVer&0xFF;
            if(Rev==0 && Patch==0)
                Msg="The internal plugin %s requires version %d.%d of " WHIPPYTERM_NAME;
            else if(Patch==0)
                Msg="The internal plugin %s requires version %d.%d.%d of " WHIPPYTERM_NAME;
            else
                Msg="The internal plugin %s requires version %d.%d.%d%c of " WHIPPYTERM_NAME;
        }
        snprintf(buff,sizeof(buff),Msg,Name,Maj,Min,Rev,Patch+'A'-1);
        UIAsk("Error",buff,e_AskBox_Error);
    }
}
