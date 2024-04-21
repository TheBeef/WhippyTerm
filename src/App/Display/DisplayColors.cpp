/*******************************************************************************
 * FILENAME: DisplayColors.cpp
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
 *    This software is the property of Paul Hutchinson. and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (27 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "PluginSDK/Plugin.h"
#include "DisplayColors.h"
#include <string.h>

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/

/*** FUNCTION PROTOTYPES      ***/

/*** VARIABLE DEFINITIONS     ***/
const uint32_t m_DefaultSysColors[e_SysColShadeMAX][e_SysColMAX]=
{
    /* e_SysColShade_Normal */
    {
        0x000000,
        0xAA0000,
        0x00AA00,
        0xAAAA00,
        0x0000AA,
        0xAA00AA,
        0x00AAAA,
        0xAAAAAA
    },
    /* e_SysColShade_Bright */
    {
        0x555555,
        0xFF5555,
        0x55FF55,
        0xFFFF55,
        0x5555FF,
        0xFF55FF,
        0x55FFFF,
        0xFFFFFF
    },
    /* e_SysColShade_Dark */
    {
        0x000000,
        0x550000,
        0x005500,
        0x555500,
        0x000055,
        0x550055,
        0x005555,
        0x555555
    },
};

const uint32_t m_VGASysColors[e_SysColShadeMAX][e_SysColMAX]=
{
    /* e_SysColShade_Normal */
    {
        0x000000,
        0xAA0000,
        0x00AA00,
        0xAA5500,
        0x0000AA,
        0xAA00AA,
        0x00AAAA,
        0xAAAAAA
    },
    /* e_SysColShade_Bright */
    {
        0x555555,
        0xFF5555,
        0x55FF55,
        0xFFFF55,
        0x5555FF,
        0xFF55FF,
        0x55FFFF,
        0xFFFFFF
    },
    /* e_SysColShade_Dark */
    {
        0x000000,
        0x550000,
        0x005500,
        0x555500,
        0x000055,
        0x550055,
        0x005555,
        0x555555
    },
};

const uint32_t m_CMDSysColors[e_SysColShadeMAX][e_SysColMAX]=
{
    /* e_SysColShade_Normal */
    {
        0x000000,
        0x800000,
        0x008000,
        0x808000,
        0x000080,
        0x800080,
        0x008080,
        0xc0c0c0
    },
    /* e_SysColShade_Bright */
    {
        0x808080,
        0xff0000,
        0x00ff00,
        0xffff00,
        0x0000ff,
        0xff00ff,
        0x00ffff,
        0xffffff
    },
    /* e_SysColShade_Dark */
    {
        0x000000,
        0x550000,
        0x005500,
        0x555500,
        0x000055,
        0x550055,
        0x005555,
        0x555555
    },
};

const uint32_t m_TerminalAppSysColors[e_SysColShadeMAX][e_SysColMAX]=
{
    /* e_SysColShade_Normal */
    {
        0x000000,
        0xc23621,
        0x25bc26,
        0xadad27,
        0x492ee1,
        0xd338d3,
        0x33bbc8,
        0xcbcccd
    },
    /* e_SysColShade_Bright */
    {
        0x818383,
        0xfc391f,
        0x31e722,
        0xeaec23,
        0x5833ff,
        0xf935f8,
        0x14f0f0,
        0xe9ebeb
    },
    /* e_SysColShade_Dark */
    {
        0x000000,
        0x550000,
        0x005500,
        0x555500,
        0x000055,
        0x550055,
        0x005555,
        0x555555
    },
};

const uint32_t m_PuTTYSysColors[e_SysColShadeMAX][e_SysColMAX]=
{
    /* e_SysColShade_Normal */
    {
        0x000000,
        0xbb0000,
        0x00bb00,
        0xbbbb00,
        0x0000bb,
        0xbb00bb,
        0x00bbbb,
        0xbbbbbb
    },
    /* e_SysColShade_Bright */
    {
        0x555555,
        0xff5555,
        0x55ff55,
        0xffff55,
        0x5555ff,
        0xff55ff,
        0x55ffff,
        0xffffff
    },
    /* e_SysColShade_Dark */
    {
        0x000000,
        0x550000,
        0x005500,
        0x555500,
        0x000055,
        0x550055,
        0x005555,
        0x555555
    },
};

const uint32_t m_mIRCSysColors[e_SysColShadeMAX][e_SysColMAX]=
{
    /* e_SysColShade_Normal */
    {
        0x000000,
        0x7f0000,
        0x009300,
        0xfc7f00,
        0x00007f,
        0x9c009c,
        0x009393,
        0xd2d2d2
    },
    /* e_SysColShade_Bright */
    {
        0x7f7f7f,
        0xff0000,
        0x00fc00,
        0xffff00,
        0x0000fc,
        0xff00ff,
        0x00ffff,
        0xffffff
    },
    /* e_SysColShade_Dark */
    {
        0x000000,
        0x550000,
        0x005500,
        0x555500,
        0x000055,
        0x550055,
        0x005555,
        0x555555
    },
};

const uint32_t m_xtermSysColors[e_SysColShadeMAX][e_SysColMAX]=
{
    /* e_SysColShade_Normal */
    {
        0x000000,
        0xcd0000,
        0x00cd00,
        0xcdcd00,
        0x0000ee,
        0xcd00cd,
        0x00cdcd,
        0xe5e5e5
    },
    /* e_SysColShade_Bright */
    {
        0x7f7f7f,
        0xff0000,
        0x00ff00,
        0xffff00,
        0x5c5cff,
        0xff00ff,
        0x00ffff,
        0xffffff
    },
    /* e_SysColShade_Dark */
    {
        0x000000,
        0x550000,
        0x005500,
        0x555500,
        0x000055,
        0x550055,
        0x005555,
        0x555555
    },
};

const uint32_t m_UbuntuSysColors[e_SysColShadeMAX][e_SysColMAX]=
{
    /* e_SysColShade_Normal */
    {
        0x010101,
        0xde382b,
        0x39b54a,
        0xffc706,
        0x006fb8,
        0x762671,
        0x2cb5e9,
        0xcccccc
    },
    /* e_SysColShade_Bright */
    {
        0x808080,
        0xff0000,
        0x00ff00,
        0xffff00,
        0x0000ff,
        0xff00ff,
        0x00ffff,
        0xffffff
    },
    /* e_SysColShade_Dark */
    {
        0x000000,
        0x550000,
        0x005500,
        0x555500,
        0x000055,
        0x550055,
        0x005555,
        0x555555
    },
};

/*******************************************************************************
 * NAME:
 *    GetPresetSysColors
 *
 * SYNOPSIS:
 *    void GetPresetSysColors(e_SysColPresetType Preset,
 *              uint32_t Colors[e_SysColShadeMAX][e_SysColMAX]);
 *
 * PARAMETERS:
 *    Preset [I] -- What preset to get.
 *    Colors [O] -- The colors to set
 *
 * FUNCTION:
 *    This function sets a color array to a default color set.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void GetPresetSysColors(e_SysColPresetType Preset,
        uint32_t Colors[e_SysColShadeMAX][e_SysColMAX])
{
    switch(Preset)
    {
        case e_SysColPresetMAX:
        default:
        break;
        case e_SysColPreset_WhippyTerm:
            memcpy(Colors,m_DefaultSysColors,sizeof(m_DefaultSysColors));
        break;
        case e_SysColPreset_VGA:
            memcpy(Colors,m_VGASysColors,sizeof(m_DefaultSysColors));
        break;
        case e_SysColPreset_CMD:
            memcpy(Colors,m_CMDSysColors,sizeof(m_DefaultSysColors));
        break;
        case e_SysColPreset_TerminalApp:
            memcpy(Colors,m_TerminalAppSysColors,sizeof(m_DefaultSysColors));
        break;
        case e_SysColPreset_PuTTY:
            memcpy(Colors,m_PuTTYSysColors,sizeof(m_DefaultSysColors));
        break;
        case e_SysColPreset_mIRC:
            memcpy(Colors,m_mIRCSysColors,sizeof(m_DefaultSysColors));
        break;
        case e_SysColPreset_xterm:
            memcpy(Colors,m_xtermSysColors,sizeof(m_DefaultSysColors));
        break;
        case e_SysColPreset_Ubuntu:
            memcpy(Colors,m_UbuntuSysColors,sizeof(m_DefaultSysColors));
        break;
    }
}
