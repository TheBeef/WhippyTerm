/*******************************************************************************
 * FILENAME: MainApp.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file is the .h file for the MainApp file.
 *
 * HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __MAINAPP_H_
#define __MAINAPP_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "Version.h"
#include <list>
#include <string>

/***  DEFINES                          ***/
#define WHIPPYTERM_NAME                 "WhippyTerm"

#if OFFICIAL_RELEASE==1
 #define WHIPPYTERM_TITLE               WHIPPYTERM_NAME
#else
 #define WHIPPYTERM_TITLE               WHIPPYTERM_NAME " (Developer build)"
#endif

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
typedef std::list<std::string> t_CLIArgList;
typedef t_CLIArgList::iterator i_CLIArgList;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/
extern bool g_CLI_URIOpened;
extern bool g_CLI_BookmarksOpened;
extern t_CLIArgList g_CLI_URIList;
extern t_CLIArgList g_CLI_BookmarkList;

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool AppMain(int argc,char *argv[]);
void AppShutdown(void);
void App1SecTick(void);
void AppInformOf_FileTransTimerTick(void);

#endif
