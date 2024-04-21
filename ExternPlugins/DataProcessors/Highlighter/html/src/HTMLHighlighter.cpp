/*******************************************************************************
 * FILENAME: HTMLHighlighter.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the plugin for highlighting HTML / XML elements in it.
 *
 * COPYRIGHT:
 *    Copyright 2021 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (03 Jul 2021)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "HTMLHighlighter.h"
#include "PluginSDK/Plugin.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/*** DEFINES                  ***/
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      HTMLHighlighter // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x00080000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
typedef enum
{
    e_HHLSS_None,
    e_HHLSS_Search4Start_s,
    e_HHLSS_Search4Start_c,
    e_HHLSS_Search4Start_r,
    e_HHLSS_Search4Start_i,
    e_HHLSS_Search4Start_p,
    e_HHLSS_Search4Start_t,
    e_HHLSS_Search4Start_LT,
    e_HHLSS_Search4Start_IgnoreUntilGT,
    e_HHLSS_Search4Start_Check4GT,
    e_HHLSS_Search4End_LT,
    e_HHLSS_Search4End_Slash,
    e_HHLSS_Search4End_s,
    e_HHLSS_Search4End_c,
    e_HHLSS_Search4End_r,
    e_HHLSS_Search4End_i,
    e_HHLSS_Search4End_p,
    e_HHLSS_Search4End_t,
    e_HHLSS_Search4End_GT,
} e_HHLSSType;

typedef enum
{
    e_HHLC_None,
    e_HHLC_Search4Start_Bang,
    e_HHLC_Search4Start_Dash,
    e_HHLC_Search4End_ExitDecl,
    e_HHLC_Search4End_ExitComment,
    e_HHLC_Search4End_ExitComment_Dash1,
    e_HHLC_Search4End_ExitComment_Dash2,
    e_HHLC_Search4End_ExitComment_LT
} e_HHLCType;

struct HTMLHL_Color
{
    /* Color info */
    e_SysColShadeType FGShade;      // e_SysColShadeMAX == No change
    e_SysColType FGColor;           // e_SysColMAX == Use 'FGOtherColor'
    uint32_t FGOtherColor;
    e_SysColShadeType BGShade;
    e_SysColType BGColor;
    uint32_t BGOtherColor;

    /* Current Coloring state */
    bool InObject;
    uint32_t SavedFGColor;
    uint32_t SavedBGColor;
    bool LeaveObjectOnNextByte;
    bool EnterOnNextByte;
};

struct HTMLHighlighterData
{
    struct HTMLHL_Color Element;
    struct HTMLHL_Color EscSeq;
    struct HTMLHL_Color Quote;
    struct HTMLHL_Color Attrib;
    struct HTMLHL_Color Comment;
    struct HTMLHL_Color Script;

    bool ElementInTagName;
    bool ElementDoingAttribs;
    int CommentState;
    int ExitCommentState;
    int ScriptState;
};

/*** FUNCTION PROTOTYPES      ***/
t_DataProcessorHandleType *HTMLHighlighter_AllocateData(void);
void HTMLHighlighter_FreeData(t_DataProcessorHandleType *DataHandle);
const struct DataProcessorInfo *HTMLHighlighter_GetProcessorInfo(
        unsigned int *SizeOfInfo);
void HTMLHighlighter_ProcessByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed);
static uint32_t HTMLHighlighter_GetColor(struct HTMLHL_Color *ColorInfo,bool FG);
static void HTMLHighlighter_DefaultColorInfo(struct HTMLHL_Color *ColorInfo);
static bool HTMLHighlighter_HandleOnNextByte(struct HTMLHL_Color *ColorInfo);
static void HTMLHighlighter_EnterObject(struct HTMLHL_Color *ColorInfo);
static bool HTMLHighlighter_IsWhiteSpace(const uint8_t *c);
static bool HTMLHighlighter_IsLetter(const uint8_t *c);
static bool HTMLHighlighter_LeaveObjectNow(struct HTMLHL_Color *ColorInfo);
static void HTMLHighlighter_EnterObjectOnNext(struct HTMLHL_Color *ColorInfo);
static void HTMLHighlighter_LeaveObjectOnNext(struct HTMLHL_Color *ColorInfo);

/*** VARIABLE DEFINITIONS     ***/
struct DataProcessorAPI m_HTMLHighlighterCBs=
{
    HTMLHighlighter_AllocateData,
    HTMLHighlighter_FreeData,
    HTMLHighlighter_GetProcessorInfo,
    NULL,
    HTMLHighlighter_ProcessByte,
};
struct DataProcessorInfo m_HTMLHighlighter_Info=
{
    "HTML / XML Highlighter",
    "Highlights HTML/XML elements, attributes, and escape blocks",
    "Highlights HTML/XML elements, attributes, and escape blocks",
    e_DataProcessorType_Text,
    e_DataProcessorClass_Highlighter
};

static const struct PI_SystemAPI *m_HTMLH_System;
static const struct DPS_API *m_HTMLH_DPS;

/*******************************************************************************
 * NAME:
 *    URLHighlighter_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int URLHighlighter_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
 *          unsigned int Version);
 *
 * PARAMETERS:
 *    SysAPI [I] -- The main API to WhippyTerm
 *    Version [I] -- What version of WhippyTerm is running.  This is used
 *                   to make sure we are compatible.  This is in the
 *                   Major<<24 | Minor<<16 | Patch<<8 | Letter format
 *
 * FUNCTION:
 *    This function registers this plugin with the system.
 *
 * RETURNS:
 *    0 if we support this version of WhippyTerm, and the minimum version
 *    we need if we are not.
 *
 * NOTES:
 *    This function is normally is called from the RegisterPlugin() when
 *    it is being used as a normal plugin.  As a std plugin it is called
 *    from RegisterStdPlugins() instead.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
/* This needs to be extern "C" because it is the main entry point for the
   plugin system */
extern "C"
{
    unsigned int REGISTER_PLUGIN_FUNCTION(const struct PI_SystemAPI *SysAPI,
            unsigned int Version)
    {
        if(Version<NEEDED_MIN_API_VERSION)
            return NEEDED_MIN_API_VERSION;

        m_HTMLH_System=SysAPI;
        m_HTMLH_DPS=SysAPI->GetAPI_DataProcessors();

        m_HTMLH_DPS->RegisterDataProcessor("HTMLHighlighter",
                &m_HTMLHighlighterCBs,sizeof(m_HTMLHighlighterCBs));

        return 0;
    }
}

/*******************************************************************************
 * NAME:
 *    HTMLHighlighter_AllocateData
 *
 * SYNOPSIS:
 *    t_DataProcessorHandleType *HTMLHighlighter_AllocateData(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function allocates any needed data for this input filter.
 *
 * RETURNS:
 *    A pointer to the data, NULL if there was an error.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
t_DataProcessorHandleType *HTMLHighlighter_AllocateData(void)
{
    struct HTMLHighlighterData *Data;
    Data=(struct HTMLHighlighterData *)malloc(sizeof(struct HTMLHighlighterData));
    if(Data==NULL)
        return NULL;

    HTMLHighlighter_DefaultColorInfo(&Data->Element);
    Data->Element.FGShade=e_SysColShade_Bright;
    Data->Element.FGColor=e_SysCol_White;

    HTMLHighlighter_DefaultColorInfo(&Data->EscSeq);
    Data->EscSeq.FGShade=e_SysColShade_Bright;
    Data->EscSeq.FGColor=e_SysCol_Yellow;

    HTMLHighlighter_DefaultColorInfo(&Data->Quote);
    Data->Quote.FGShade=e_SysColShade_Bright;
    Data->Quote.FGColor=e_SysCol_Red;

    HTMLHighlighter_DefaultColorInfo(&Data->Attrib);
    Data->Attrib.FGShade=e_SysColShade_Bright;
    Data->Attrib.FGColor=e_SysCol_Cyan;

    HTMLHighlighter_DefaultColorInfo(&Data->Comment);
    Data->Comment.FGShade=e_SysColShade_Bright;
    Data->Comment.FGColor=e_SysCol_Green;

    HTMLHighlighter_DefaultColorInfo(&Data->Script);
    Data->Script.FGShade=e_SysColShade_Bright;
    Data->Script.FGColor=e_SysCol_Blue;

    Data->CommentState=0;   // Not in a comment
    Data->ScriptState=0;    // Not in a script block

    return (t_DataProcessorHandleType *)Data;
}

/*******************************************************************************
 *  NAME:
 *    HTMLHighlighter_FreeData
 *
 *  SYNOPSIS:
 *    void HTMLHighlighter_FreeData(t_DataProcessorHandleType *DataHandle);
 *
 *  PARAMETERS:
 *    DataHandle [I] -- The data handle to free.  This will need to be
 *                      case to your internal data type before you use it.
 *
 *  FUNCTION:
 *    This function frees the memory allocated with AllocateData().
 *
 *  RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HTMLHighlighter_FreeData(t_DataProcessorHandleType *DataHandle)
{
    struct HTMLHighlighterData *Data=(struct HTMLHighlighterData *)DataHandle;

    free(Data);
}

/*******************************************************************************
 * NAME:
 *    HTMLHighlighter_GetProcessorInfo
 *
 * SYNOPSIS:
 *    const struct DataProcessorInfo *HTMLHighlighter_GetProcessorInfo(
 *              unsigned int *SizeOfInfo);
 *
 * PARAMETERS:
 *    SizeOfInfo [O] -- The size of 'struct DataProcessorInfo'.  This is used
 *                        for forward / backward compatibility.
 *
 * FUNCTION:
 *    This function gets info about the plugin.  'DataProcessorInfo' has
 *    the following fields:
 *          DisplayName -- The name we show the user
 *          Tip -- A tool tip (for when you hover the mouse over this plugin)
 *          Help -- A help string for this plugin.
 *          ProType -- The type of process.  Supported values:
 *                  e_DataProcessorType_Text -- This is a text processor.
 *                      This is the more clasic type of processor (like VT100).
 *                  e_DataProcessorType_Binary -- This is a binary processor.
 *                      These are processors for binary protocol.  This may
 *                      be something as simple as a hex dump.
 *          ProClass -- This only applies to 'e_DataProcessorType_Text' type
 *              processors. This is what class of text processor is
 *              this.  Supported classes:
 *                      e_DataProcessorClass_Other -- This is a generic class
 *                          more than one of these processors can be active
 *                          at a time but no other requirements exist.
 *                      e_DataProcessorClass_CharEncoding -- This is a
 *                          class that converts the raw stream into some kind
 *                          of char encoding.  For example unicode is converted
 *                          from a number of bytes to chars in the system.
 *                      e_DataProcessorClass_TermEmulation -- This is a
 *                          type of terminal emulator.  An example of a
 *                          terminal emulator is VT100.
 *                      e_DataProcessorClass_Highlighter -- This is a processor
 *                          that highlights strings as they come in the input
 *                          stream.  For example a processor that underlines
 *                          URL's.
 *                      e_DataProcessorClass_Logger -- This is a processor
 *                          that saves the input.  It may save to a file or
 *                          send out a debugging service.  And example is
 *                          a processor that saves all the raw bytes to a file.
 * SEE ALSO:
 *    
 ******************************************************************************/
const struct DataProcessorInfo *HTMLHighlighter_GetProcessorInfo(
        unsigned int *SizeOfInfo)
{
    *SizeOfInfo=sizeof(struct DataProcessorInfo);
    return &m_HTMLHighlighter_Info;
}

/*******************************************************************************
 * NAME:
 *    HTMLHighlighter_DefaultColorInfo
 *
 * SYNOPSIS:
 *    static void HTMLHighlighter_DefaultColorInfo(struct HTMLHL_Color *ColorInfo);
 *
 * PARAMETERS:
 *    ColorInfo [O] -- The color info to default.
 *
 * FUNCTION:
 *    This function defaults a color info structure.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void HTMLHighlighter_DefaultColorInfo(struct HTMLHL_Color *ColorInfo)
{
    ColorInfo->InObject=false;
    ColorInfo->LeaveObjectOnNextByte=false;
    ColorInfo->EnterOnNextByte=false;
    ColorInfo->FGShade=e_SysColShadeMAX;
    ColorInfo->BGShade=e_SysColShadeMAX;
}

/*******************************************************************************
 * NAME:
 *    HTMLHighlighter_GetColor
 *
 * SYNOPSIS:
 *    static uint32_t HTMLHighlighter_GetColor(struct HTMLHL_Color *ColorInfo,
 *          bool FG);
 *
 * PARAMETERS:
 *    ColorInfo [I] -- The color info to get the use color from.
 *    FG [I] -- true = forground, false = background.
 *
 * FUNCTION:
 *    This function gets the color you should be using for the color when
 *    highlighting something.
 *
 * RETURNS:
 *    The color to use as the color.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static uint32_t HTMLHighlighter_GetColor(struct HTMLHL_Color *ColorInfo,bool FG)
{
    if(FG)
    {
        if(ColorInfo->FGShade==e_SysColShadeMAX)
        {
            return m_HTMLH_DPS->GetFGColor();
        }
        else
        {
            if(ColorInfo->FGColor==e_SysColMAX)
            {
                return ColorInfo->FGOtherColor;
            }
            else
            {
                return m_HTMLH_DPS->GetSysColor(ColorInfo->FGShade,
                        ColorInfo->FGColor);
            }
        }
    }
    else
    {
        if(ColorInfo->BGShade==e_SysColShadeMAX)
        {
            return m_HTMLH_DPS->GetBGColor();
        }
        else
        {
            if(ColorInfo->BGColor==e_SysColMAX)
            {
                return ColorInfo->BGOtherColor;
            }
            else
            {
                return m_HTMLH_DPS->GetSysColor(ColorInfo->BGShade,
                        ColorInfo->BGColor);
            }
        }
    }
}

/*******************************************************************************
 * NAME:
 *    HTMLHighlighter_HandleOnNextByte
 *
 * SYNOPSIS:
 *    static bool HTMLHighlighter_HandleOnNextByte(struct HTMLHL_Color *ColorInfo);
 *
 * PARAMETERS:
 *    ColorInfo [I] -- The color object to work on
 *
 * FUNCTION:
 *    This function handles restoring the old colors when we leave an
 *    object or entering a new one.
 *
 * RETURNS:
 *    true -- We left the object
 *    false -- We didn't do anything
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static bool HTMLHighlighter_HandleOnNextByte(struct HTMLHL_Color *ColorInfo)
{
    if(ColorInfo->LeaveObjectOnNextByte)
    {
        ColorInfo->LeaveObjectOnNextByte=false;
        ColorInfo->InObject=false;
        m_HTMLH_DPS->SetFGColor(ColorInfo->SavedFGColor);
        m_HTMLH_DPS->SetBGColor(ColorInfo->SavedBGColor);
        return true;
    }

    if(ColorInfo->EnterOnNextByte)
    {
        ColorInfo->EnterOnNextByte=false;
        ColorInfo->InObject=true;
        m_HTMLH_DPS->SetFGColor(HTMLHighlighter_GetColor(ColorInfo,true));
        m_HTMLH_DPS->SetBGColor(HTMLHighlighter_GetColor(ColorInfo,false));
        return true;
    }

    return false;
}

/*******************************************************************************
 * NAME:
 *    HTMLHighlighter_LeaveObjectNow
 *
 * SYNOPSIS:
 *    static bool HTMLHighlighter_LeaveObjectNow(struct HTMLHL_Color *ColorInfo);
 *
 * PARAMETERS:
 *    ColorInfo [I] -- The color object to work on
 *
 * FUNCTION:
 *    This function handles restoring the old colors when we leave an
 *    object.  It also resets the object state.  It does it now instead of
 *    coloring the next byte that comes in.
 *
 * RETURNS:
 *    true -- We left the object
 *    false -- We didn't do anything
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static bool HTMLHighlighter_LeaveObjectNow(struct HTMLHL_Color *ColorInfo)
{
    ColorInfo->LeaveObjectOnNextByte=true;
    return HTMLHighlighter_HandleOnNextByte(ColorInfo);
}

/*******************************************************************************
 * NAME:
 *    HTMLHighlighter_EnterObject
 *
 * SYNOPSIS:
 *    static void HTMLHighlighter_EnterObject(struct HTMLHL_Color *ColorInfo);
 *
 * PARAMETERS:
 *    ColorInfo [I] -- The color object to work on
 *
 * FUNCTION:
 *    This function handles entering an object.  If we are already in the
 *    object then this is ignored.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void HTMLHighlighter_EnterObject(struct HTMLHL_Color *ColorInfo)
{
    if(ColorInfo->InObject)
        return;

    ColorInfo->InObject=true;
    ColorInfo->SavedFGColor=m_HTMLH_DPS->GetFGColor();
    ColorInfo->SavedBGColor=m_HTMLH_DPS->GetBGColor();

    m_HTMLH_DPS->SetFGColor(HTMLHighlighter_GetColor(ColorInfo,true));
    m_HTMLH_DPS->SetBGColor(HTMLHighlighter_GetColor(ColorInfo,false));
//        m_HTMLH_DPS->SetAttribs(m_HTMLH_DPS->GetAttribs()|DB_ATTRIB_UNDERLINE);
}

/*******************************************************************************
 * NAME:
 *    HTMLHighlighter_EnterObjectOnNext
 *
 * SYNOPSIS:
 *    static void HTMLHighlighter_EnterObjectOnNext(struct HTMLHL_Color *ColorInfo);
 *
 * PARAMETERS:
 *    ColorInfo [I] -- The color object to work on
 *
 * FUNCTION:
 *    This function handles entering an object.  If we are already in the
 *    object then this is ignored.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void HTMLHighlighter_EnterObjectOnNext(struct HTMLHL_Color *ColorInfo)
{
    ColorInfo->EnterOnNextByte=true;
    ColorInfo->SavedFGColor=m_HTMLH_DPS->GetFGColor();
    ColorInfo->SavedBGColor=m_HTMLH_DPS->GetBGColor();
}

/*******************************************************************************
 * NAME:
 *    HTMLHighlighter_LeaveObjectOnNext
 *
 * SYNOPSIS:
 *    static void HTMLHighlighter_LeaveObjectOnNext(struct HTMLHL_Color *ColorInfo);
 *
 * PARAMETERS:
 *    ColorInfo [I] -- The color object to work on
 *
 * FUNCTION:
 *    This function handles exiting an object on the next char.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static void HTMLHighlighter_LeaveObjectOnNext(struct HTMLHL_Color *ColorInfo)
{
    ColorInfo->LeaveObjectOnNextByte=true;
}

/*******************************************************************************
 * NAME:
 *    HTMLHighlighter_IsWhiteSpace
 *
 * SYNOPSIS:
 *    static bool HTMLHighlighter_IsWhiteSpace(const uint8_t *c);
 *
 * PARAMETERS:
 *    c [I] -- The char to check.  This is the start of the 'ProcessedChar'
 *             buffer.
 *
 * FUNCTION:
 *    This function checks if the current 'ProcessedChar' is a white space.
 *
 * RETURNS:
 *    true -- It's a white space
 *    false -- It's something else
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static bool HTMLHighlighter_IsWhiteSpace(const uint8_t *c)
{
    if(*c==' ' || *c=='\t' || *c=='\r'  || *c=='\n')
        return true;
    return false;
}

/*******************************************************************************
 * NAME:
 *    HTMLHighlighter_IsLetter
 *
 * SYNOPSIS:
 *    static bool HTMLHighlighter_IsLetter(const uint8_t *c);
 *
 * PARAMETERS:
 *    c [I] -- The char to check.  This is the start of the 'ProcessedChar'
 *             buffer.
 *
 * FUNCTION:
 *    This function checks if the current 'ProcessedChar' is a letter or
 *    something else (numbers are considered letters).
 *
 * RETURNS:
 *    true -- It's a letter
 *    false -- It's something else
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static bool HTMLHighlighter_IsLetter(const uint8_t *c)
{
    if((*c>='a' && *c<='z') || (*c>='A'  && *c<='z') ||
            (*c>='0'  && *c<='9') || *c>127)
    {
        return true;
    }
    return false;
}

/*******************************************************************************
 *  NAME:
 *    HTMLHighlighter_ProcessByte
 *
 *  SYNOPSIS:
 *    void HTMLHighlighter_ProcessByte(t_DataProcessorHandleType *DataHandle,
 *          const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
 *          PG_BOOL *Consumed)
 *
 *  PARAMETERS:
 *    DataHandle [I] -- The data handle to free.  This will need to be
 *                      case to your internal data type before you use it.
 *    ConnectionID [I] -- The connection to work on
 *    RawByte [I] -- The raw byte to process.  This is the byte that came in.
 *    ProcessedChar [I/O] -- This is a unicode char that has already been
 *                         processed by some of the other input filters.  You
 *                         can change this as you need.  It must remain only
 *                         one unicode char.
 *    CharLen [I/O] -- This number of bytes in 'ProcessedChar'
 *    Consumed [I/O] -- This tells the system (and other filters) if the
 *                      char has been used up and will not be added to the
 *                      screen.
 *    Style [I/O] -- This is the current style that the char will be added
 *                   with.  You can change this to change how the char will
 *                   be added.
 *
 *  FUNCTION:
 *    This function is called for each byte that comes in.
 *
 *  RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void HTMLHighlighter_ProcessByte(t_DataProcessorHandleType *DataHandle,
        const uint8_t RawByte,uint8_t *ProcessedChar,int *CharLen,
        PG_BOOL *Consumed)
{
    struct HTMLHighlighterData *Data=(struct HTMLHighlighterData *)DataHandle;

    HTMLHighlighter_HandleOnNextByte(&Data->Element);
    HTMLHighlighter_HandleOnNextByte(&Data->EscSeq);
    HTMLHighlighter_HandleOnNextByte(&Data->Quote);
    HTMLHighlighter_HandleOnNextByte(&Data->Attrib);
    HTMLHighlighter_HandleOnNextByte(&Data->Comment);
    HTMLHighlighter_HandleOnNextByte(&Data->Script);

    switch(Data->ScriptState)
    {
        case e_HHLSS_None:
        break;
        case e_HHLSS_Search4Start_s:    // Just entered the element (looking for 's')
            if(ProcessedChar[0]=='s')
                Data->ScriptState++;
            else
                Data->ScriptState=0;
        break;
        case e_HHLSS_Search4Start_c: // Looking for s'c'ript
            if(ProcessedChar[0]=='c')
                Data->ScriptState++;
            else
                Data->ScriptState=0;
        break;
        case e_HHLSS_Search4Start_r: // Looking for sc'r'ipt
            if(ProcessedChar[0]=='r')
                Data->ScriptState++;
            else
                Data->ScriptState=0;
        break;
        case e_HHLSS_Search4Start_i: // Looking for scr'i'pt
            if(ProcessedChar[0]=='i')
                Data->ScriptState++;
            else
                Data->ScriptState=0;
        break;
        case e_HHLSS_Search4Start_p: // Looking for scri'p't
            if(ProcessedChar[0]=='p')
                Data->ScriptState++;
            else
                Data->ScriptState=0;
        break;
        case e_HHLSS_Search4Start_t: // Looking for scrip't'
            if(ProcessedChar[0]=='t')
                Data->ScriptState++;
            else
                Data->ScriptState=0;
        break;
        case e_HHLSS_Search4Start_LT: // Looking for a > or white space
            if(ProcessedChar[0]=='>')
            {
                /* Ok we are in a script block */
                Data->ScriptState=e_HHLSS_Search4End_LT;
                HTMLHighlighter_EnterObjectOnNext(&Data->Script);
            }
            else if(HTMLHighlighter_IsWhiteSpace(ProcessedChar))
            {
                /* Ok, they have attribs with this one */
                Data->ScriptState++;
            }
            else
            {
                /* False alarm */
                Data->ScriptState=0;
            }
        break;
        case e_HHLSS_Search4Start_IgnoreUntilGT:    // Ignore everything until a >
            if(ProcessedChar[0]=='/')
            {
                Data->ScriptState=e_HHLSS_Search4Start_Check4GT;
            }
            else if(ProcessedChar[0]=='>')
            {
                /* We are now in a script block */
                Data->ScriptState=e_HHLSS_Search4End_LT;
                HTMLHighlighter_EnterObjectOnNext(&Data->Script);
            }
        break;
        case e_HHLSS_Search4Start_Check4GT:
            if(HTMLHighlighter_IsWhiteSpace(ProcessedChar))
                break;

            if(ProcessedChar[0]=='>')
            {
                /* Ok, if we got here then we had a /> so don't start the
                   script block */
                Data->ScriptState=e_HHLSS_None;
            }
            else
            {
                /* Back to ignoring stuff */
                Data->ScriptState=e_HHLSS_Search4Start_IgnoreUntilGT;
            }
        break;
        case e_HHLSS_Search4End_LT: // Looking to exit the script block
            if(ProcessedChar[0]=='<')
                Data->ScriptState++;
            else
                Data->ScriptState=e_HHLSS_Search4End_LT;
        break;
        case e_HHLSS_Search4End_Slash: // Looking for <'/'script>
            if(ProcessedChar[0]=='/')
                Data->ScriptState++;
            else
                Data->ScriptState=e_HHLSS_Search4End_LT;
        break;
        case e_HHLSS_Search4End_s: // Looking for </'s'cript>
            if(ProcessedChar[0]=='s')
                Data->ScriptState++;
            else
                Data->ScriptState=e_HHLSS_Search4End_LT;
        break;
        case e_HHLSS_Search4End_c: // Looking for </s'c'ript>
            if(ProcessedChar[0]=='c')
                Data->ScriptState++;
            else
                Data->ScriptState=e_HHLSS_Search4End_LT;
        break;
        case e_HHLSS_Search4End_r: // Looking for </sc'r'ipt>
            if(ProcessedChar[0]=='r')
                Data->ScriptState++;
            else
                Data->ScriptState=e_HHLSS_Search4End_LT;
        break;
        case e_HHLSS_Search4End_i: // Looking for </scr'i'pt>
            if(ProcessedChar[0]=='i')
                Data->ScriptState++;
            else
                Data->ScriptState=e_HHLSS_Search4End_LT;
        break;
        case e_HHLSS_Search4End_p: // Looking for </scri'p't>
            if(ProcessedChar[0]=='p')
                Data->ScriptState++;
            else
                Data->ScriptState=e_HHLSS_Search4End_LT;
        break;
        case e_HHLSS_Search4End_t: // Looking for </scrip't'>
            if(ProcessedChar[0]=='t')
                Data->ScriptState++;
            else
                Data->ScriptState=e_HHLSS_Search4End_LT;
        break;
        case e_HHLSS_Search4End_GT: // Looking for </script'>'
            if(ProcessedChar[0]=='>')
            {
                /* We are done */
                Data->ScriptState=e_HHLSS_None;
                HTMLHighlighter_LeaveObjectNow(&Data->Script);
                HTMLHighlighter_LeaveObjectOnNext(&Data->Element);   // Also exit the element
            }
            else if(HTMLHighlighter_IsWhiteSpace(ProcessedChar))
            {
                /* Continue searching for the > */
            }
            else
            {
                /* False alarm */
                Data->ScriptState=e_HHLSS_Search4End_LT;
            }
        break;
    }
    if(Data->Script.InObject)
    {
        /* We are in a script block.  Ignore everything else */
        return;
    }

    /* Comments / decl */
    switch(Data->CommentState)
    {
        case e_HHLC_None:
        break;
        case e_HHLC_Search4Start_Bang: // Just entered the element
            if(ProcessedChar[0]=='!')
            {
                HTMLHighlighter_EnterObject(&Data->Comment);
                Data->ExitCommentState=0;
                Data->CommentState++;
            }
            else
            {
                Data->CommentState=e_HHLC_None;
            }
        break;
        case e_HHLC_Search4Start_Dash: // Ok, it's going to be - or something else
            if(ProcessedChar[0]=='-')
            {
                /* Comment */
                Data->CommentState=e_HHLC_Search4End_ExitComment;  // Comment exit
            }
            else
            {
                /* Decl */
                Data->CommentState=e_HHLC_Search4End_ExitDecl;  // Decl exit
            }
        break;
        case e_HHLC_Search4End_ExitDecl: // Looking for the decl exit
            /* We exit on a > */
            if(ProcessedChar[0]=='>')
            {
                Data->CommentState=e_HHLC_None;
                HTMLHighlighter_LeaveObjectNow(&Data->Comment);
            }
            else
            {
                /* Ignore everything else */
                return;
            }
        break;
        case e_HHLC_Search4End_ExitComment: // Looking for the comment exit
            if(ProcessedChar[0]=='-')
                Data->CommentState++;
            return; // Ignore everything else
        break;
        case e_HHLC_Search4End_ExitComment_Dash1:
            if(ProcessedChar[0]=='-')
                Data->CommentState++;
            else
                Data->CommentState=e_HHLC_Search4End_ExitComment;
            return; // Ignore everything else
        break;
        case e_HHLC_Search4End_ExitComment_Dash2:
            if(ProcessedChar[0]!='>')
            {
                if(ProcessedChar[0]=='-')
                {
                    /* We do allow for ----> */
                    Data->CommentState=e_HHLC_Search4End_ExitComment_Dash2;
                }
                else
                {
                    Data->CommentState=e_HHLC_Search4End_ExitComment;
                }
                return; // Ignore everything else
            }
            else
            {
                Data->CommentState=e_HHLC_None;
                HTMLHighlighter_LeaveObjectNow(&Data->Comment);
            }
        break;
    }

    /* &amp; stuff */
    if(ProcessedChar[0]=='&')
    {
        HTMLHighlighter_EnterObject(&Data->EscSeq);
    }
    if(Data->EscSeq.InObject)
    {
        if(ProcessedChar[0]==';')
        {
            HTMLHighlighter_LeaveObjectOnNext(&Data->EscSeq);
        }
    }

    /* Elements */
    if(ProcessedChar[0]=='<')
    {
        HTMLHighlighter_EnterObject(&Data->Element);
        Data->ElementInTagName=true;
        Data->ElementDoingAttribs=false;
        Data->CommentState=e_HHLC_Search4Start_Bang;    // Might be a comment
        Data->ScriptState=e_HHLSS_Search4Start_s;       // Or maybe a script
    }

    if(Data->Element.InObject)
    {
        if(!Data->Quote.InObject)
        {
            if(Data->ElementDoingAttribs)
            {
                if(!HTMLHighlighter_IsWhiteSpace(ProcessedChar))
                {
                    HTMLHighlighter_EnterObject(&Data->Attrib);
                }
                if(Data->Attrib.InObject)
                {
                    if(!HTMLHighlighter_IsLetter(ProcessedChar))
                    {
                        HTMLHighlighter_LeaveObjectNow(&Data->Attrib);
                    }
                }
            }
            if(Data->ElementInTagName)
            {
                /* A space ends the tag name */
                if(HTMLHighlighter_IsWhiteSpace(ProcessedChar))
                {
                    Data->ElementInTagName=false;
                    Data->ElementDoingAttribs=true;
                }
            }
        }

        if(ProcessedChar[0]=='\"')
        {
            if(Data->Quote.InObject)
                HTMLHighlighter_LeaveObjectOnNext(&Data->Quote);
            else
                HTMLHighlighter_EnterObject(&Data->Quote);
        }

        if(ProcessedChar[0]=='>' && !Data->Quote.InObject)
        {
            HTMLHighlighter_LeaveObjectOnNext(&Data->Element);
        }
    }
}

