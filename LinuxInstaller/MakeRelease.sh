#!/bin/bash
if [ $# -lt 2 ]; then
	echo "Usage:"
	echo "    MakeRelease.sh <Version> <PathToExe>"
	echo "Example:"
	echo "    MakeRelease.sh 2.3.0.0 ../build-WT-Desktop5-Debug"
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

rm -r WhippyTerm_$1-1
cp -r WhippyTerm_x.x.x.x-1/ WhippyTerm_$1-1/
gzip -9 -c -n ../Tools/MakeReleaseNotes/changelog.Debian > WhippyTerm_$1-1/usr/share/doc/whippyterm/changelog.Debian.gz
chmod 644 WhippyTerm_$1-1/usr/share/doc/whippyterm/changelog.Debian.gz
cp ../manpages/whippyterm.1 WhippyTerm_$1-1/usr/share/man/man1/whippyterm.1
chmod 644 WhippyTerm_$1-1/usr/share/man/man1/whippyterm.1
gzip -9n WhippyTerm_$1-1/usr/share/man/man1/whippyterm.1
rm -r WhippyTerm_$1-1/usr/bin/README
rm -r WhippyTerm_$1-1/usr/share/man/man1/README

sed -i "s/Version: x.x.x.x-1/Version: $1-1/g" WhippyTerm_$1-1/DEBIAN/control

objcopy --strip-debug --strip-unneeded $2/WhippyTerm WhippyTerm_$1-1/usr/bin/whippyterm
chmod 755 WhippyTerm_$1-1/usr/bin/whippyterm
chmod 0755 WhippyTerm_$1-1/usr
chmod 0755 WhippyTerm_$1-1/usr/bin
chmod 0755 WhippyTerm_$1-1/usr/share
chmod 0755 WhippyTerm_$1-1/usr/share/applications
chmod 0644 WhippyTerm_$1-1/usr/share/applications/WhippyTerm.desktop
chmod 0755 WhippyTerm_$1-1/usr/share/icons
chmod 0644 WhippyTerm_$1-1/usr/share/icons/WhippyTerm.png
chmod 0755 WhippyTerm_$1-1/usr/share/doc
chmod 0755 WhippyTerm_$1-1/usr/share/doc/whippyterm
chmod 0644 WhippyTerm_$1-1/usr/share/doc/whippyterm/copyright

fakeroot dpkg-deb --build WhippyTerm_$1-1

