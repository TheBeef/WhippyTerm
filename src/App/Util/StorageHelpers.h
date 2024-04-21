/*******************************************************************************
 * FILENAME: StorageHelpers.h
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
 * HISTORY:
 *    Paul Hutchinson (27 Sep 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __STORAGEHELPERS_H_
#define __STORAGEHELPERS_H_

/***  HEADER FILES TO INCLUDE          ***/
#include "App/Util/KeyValue.h"
#include "ThirdParty/TinyCFG/TinyCFG.h"
#include <string>
#include <map>
#include <time.h>

/***  DEFINES                          ***/

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/
struct ConOptions
{
    time_t Age;
    t_KVList Options;
};

typedef std::map<std::string,struct ConOptions> t_ConnectionsOptions;
typedef t_ConnectionsOptions::iterator i_ConnectionsOptions;

typedef std::map<std::string,t_KVList> t_KVListMap;
typedef t_KVListMap::iterator i_KVListMap;

struct ConUploadOptions
{
    time_t Age;
    std::string Filename;
    std::string Protocol;
    t_KVList Options;
};

typedef std::map<std::string,struct ConUploadOptions> t_ConUploadOptions;
typedef t_ConUploadOptions::iterator i_ConUploadOptions;

struct ConDownloadOptions
{
    time_t Age;
    std::string Protocol;
    t_KVList Options;
};

typedef std::map<std::string,struct ConDownloadOptions> t_ConDownloadOptions;
typedef t_ConDownloadOptions::iterator i_ConDownloadOptions;

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
bool RegisterKVListMap_TinyCFG(class TinyCFG &cfg,const char *XmlName,
      t_KVListMap &Data);   // WARNING: never tested
bool RegisterConUploadOptions_TinyCFG(class TinyCFG &cfg,const char *XmlName,
      t_ConUploadOptions &Data);
bool RegisterConDownloadOptions_TinyCFG(class TinyCFG &cfg,const char *XmlName,
      t_ConDownloadOptions &Data);
bool RegisterConnectionOptions_TinyCFG(class TinyCFG &cfg,const char *XmlName,
      t_ConnectionsOptions &Data);
bool RegisterKVList_TinyCFG(class TinyCFG &cfg,const char *XmlName,
        t_KVList &Data);

#endif
