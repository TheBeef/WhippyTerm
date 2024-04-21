/*******************************************************************************
 * FILENAME: strnatcmp.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This file was written to support strnatcmp.cpp file.  It doesn't include
 *    anything from the original strnatcmp.h file.  If you want the original
 *    best to goto the strnatcmp project (https://github.com/sourcefrog/natsort)
 *
 * HISTORY:
 *    Paul Hutchinson (18 Jul 2018)
 *       Created
 *
 *******************************************************************************/
#ifndef __STRNATCMP_H_
#define __STRNATCMP_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/
#define nat_char char

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/
int strnatcmp(nat_char const *a, nat_char const *b);
int strnatcasecmp(nat_char const *a, nat_char const *b);

#endif
