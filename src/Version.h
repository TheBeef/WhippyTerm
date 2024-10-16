/*******************************************************************************
 * FILENAME: Version.h
 * 
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This has the version number for Whippy Term in it.
 *          Major -- This is the major part of the feature number.
 *                   This is used with 'Minor' to determine the feature set
 *                   that is in a release.  It doesn't really mean anything,
 *                   it is bumped any time it is felt the minor is getting
 *                   to big or it is felt there have been enough changes to
 *                   warrant a change.
 *
 *                   When this goes up all other fields go to 0.
 *
 *          Minor -- This is the minor part of the feature number.  It is used
 *                   with the 'Major' number to make a set of features in this
 *                   release.
 *
 *                   A set of features is the things that are in this release,
 *                   they may include bug fixes, new things that the software
 *                   does or changes to existing behaviour.  It does not
 *                   include any bugs that where added in this release (the
 *                   bug had to exist in a previously releaes version).
 *
 *                   When you plan the next release you decide what's in and
 *                   bump this number (and maybe 'Major' depending).  You then
 *                   make the changes and when they are all done you make a
 *                   release candidate for testing / release.
 *
 *                   When this is changed 'Rev' and 'Patch' goto 0.
 *
 *          Rev -- This is a revision number of the release.  This number starts
 *                 at 0 for the first release candidate of Major.Minor and is
 *                 bumped every time a new release candidate is made (after
 *                 you have fixed some bugs in the previous release candidate).
 *                 When all known bugs (or it is decided no more will be fixed)
 *                 then that last release candidate is just released (no
 *                 new build / 'Rev' needed).
 *
 *                 You should not add any new features at this point, only
 *                 bug fixes should be done.  If you want to add a new feature
 *                 then going back and bumping 'Minor' as this is now a new
 *                 feature set.
 *
 *                 When this is changed 'Patch' goto 0.
 *
 *          Patch -- This number will be 0 on any new release.  This is used
 *                   if you have a release already in the wild and need to make
 *                   a patch to that version but don't want to bring any changes
 *                   from releases that where made after this version.
 *                   For example if you have the following release out:
 *                      1.2.0.0
 *                      1.2.5.0
 *                      1.3.0.0
 *                      2.5.11.0
 *                   And you need to make a new version of 1.2.5.0 but don't
 *                   want to bring in changes from 1.3.0.0 or 2.5.11.0 you
 *                   would make a new version 1.2.5.1, this would be a one
 *                   off version that is 1.2.5.0 but with some small fixes.
 *
 *                   The patch number lets users know that this is patched
 *                   version and any changes / fixes will not be in the next
 *                   version number up (as they where released before this
 *                   patch was made).
 *
 *                   This number maybe displayed as a capital letter instead
 *                   of a number (ie 1.2.5A instead of 1.2.5.1)
 *
 * COPYRIGHT:
 *    Copyright 2020 Paul Hutchinson.
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
 * HISTORY:
 *    Paul Hutchinson (07 Jul 2020)
 *       Created
 *
 *******************************************************************************/
#ifndef __VERSION_H_
#define __VERSION_H_

/***  HEADER FILES TO INCLUDE          ***/

/***  DEFINES                          ***/

/* This is used to mark the build as an official release.  If this is 0 then we
   are being an experimental version that is not ment for general availablility.
   This are normally DEBUG builds with debug symbols turned on.
   When a build is NOT an official release then:
    * The build is clearly marked as an unofficial build (test build)
    * The plugin API is not fixed and can break backward compatibility (back
      to the last official release)
    * The version number does not have any meaning and some other mechanism
      (like the build date and time) is used to know what is in this build.
*/
#define OFFICIAL_RELEASE    0               // 1=official, 0=developer build (don't forget to update ExperimentalID.h on release)

#define WHIPPYTERM_VERSION_MAJOR       0
#define WHIPPYTERM_VERSION_MINOR       14
#define WHIPPYTERM_VERSION_REV         0
#define WHIPPYTERM_VERSION_PATCH       0

#define WHIPPYTERM_VERSION ((WHIPPYTERM_VERSION_MAJOR<<24) | \
                    (WHIPPYTERM_VERSION_MINOR<<16) | \
                    (WHIPPYTERM_VERSION_REV<<8)  | \
                    (WHIPPYTERM_VERSION_PATCH))

#define VER_STR_HELPER(x) #x
#define VER_STR(x) VER_STR_HELPER(x)

#define WHIPPYTERM_VERSION_STR  VER_STR(WHIPPYTERM_VERSION_MAJOR) "." VER_STR(WHIPPYTERM_VERSION_MINOR) "."  VER_STR(WHIPPYTERM_VERSION_REV) "."  VER_STR(WHIPPYTERM_VERSION_PATCH)

/***  MACROS                           ***/

/***  TYPE DEFINITIONS                 ***/

/***  CLASS DEFINITIONS                ***/

/***  GLOBAL VARIABLE DEFINITIONS      ***/

/***  EXTERNAL FUNCTION PROTOTYPES     ***/

#endif
