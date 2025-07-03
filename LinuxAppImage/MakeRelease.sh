#!/bin/bash
if [ $# -lt 2 ]; then
	echo "Usage:"
	echo "    MakeRelease.sh <Version> <PathToExe>"
	echo "Example:"
	echo "    MakeRelease.sh 2.3.0.0 ../Project/build/Desktop5-Release"
	echo "Note: Version number should include ww.xx.yy.zz"
        echo "WHERE:"
	echo "  ww -- Major"
	echo "  xx -- Minor"
	echo "  yy -- Rev"
	echo "  zz -- Patch"
#	echo "Parts of the version are padded with 0 because .deb treat is as a full number and warn about upgrading to an old version without it"
	exit 1
fi

set -x

rm WhippyTerm_$1-x86_64.AppImage


objcopy --strip-debug --strip-unneeded $2/WhippyTerm WhippyTerm_AppDir/usr/bin/whippyterm
chmod 755 WhippyTerm_$1-1/usr/bin/whippyterm

linuxdeployqt-continuous-x86_64.AppImage WhippyTerm_AppDir/usr/share/applications/WhippyTerm.desktop -appimage
mv Whippy_Term-x86_64.AppImage WhippyTerm_$1-x86_64.AppImage
