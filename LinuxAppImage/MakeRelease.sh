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

rm -rf WhippyTerm
mkdir -p WhippyTerm/usr/bin
mkdir -p WhippyTerm/usr/share/applications
mkdir -p WhippyTerm/usr/share/icons/hicolor/256x256/apps
objcopy --strip-debug --strip-unneeded $2/WhippyTerm WhippyTerm/usr/bin/whippyterm
cp ../LinuxInstaller/Debian24.template/usr/share/applications/* WhippyTerm/usr/share/applications
cp ../LinuxInstaller/Debian24.template/usr/share/icons/* WhippyTerm/usr/share/icons/hicolor/256x256/apps
export VERSION=$1
./linuxdeployqt-continuous-x86_64.AppImage WhippyTerm/usr/share/applications/WhippyTerm.desktop -qmake=/usr/lib/qt6/bin/qmake -appimage


#rm WhippyTerm_$1-x86_64.AppImage
#
#
#objcopy --strip-debug --strip-unneeded $2/WhippyTerm WhippyTerm_AppDir/usr/bin/whippyterm
#chmod 755 WhippyTerm_$1-1/usr/bin/whippyterm
#
#linuxdeployqt-continuous-x86_64.AppImage WhippyTerm_AppDir/usr/share/applications/WhippyTerm.desktop -appimage
#mv Whippy_Term-x86_64.AppImage WhippyTerm_$1-x86_64.AppImage
