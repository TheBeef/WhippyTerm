/*******************************************************************************
 * FILENAME: UIFontReq.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file has the UI access to font requesters.
 *
 * HISTORY:
 *    Paul Hutchinson (02 Jun 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __UIFONTREQ_H_
#define __UIFONTREQ_H_

/***  HEADER FILES TO INCLUDE          ***/
#include <stdbool.h>
#include <string>

/***  DEFINES                          ***/
#define UIFONT_STYLE_BOLD           (1<<0)
#define UIFONT_STYLE_ITALIC         (1<<1)

/* UI_FontReq Flags */
#define UIFONT_FLAGS_FIXEDWIDTH         0x0001

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool UI_FontReq(const char *Title,std::string &FontName,
        int &FontSize,long &FontStyle,long Flags);
void UI_GetDefaultFixedWidthFont(std::string &FontName);
#endif
