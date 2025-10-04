#!/bin/bash
if [ $# -lt 2 ]; then
	echo "Usage:"
	echo "    MakeRelease.sh <Version> <PathToExe> <TemplateVersion>"
	echo "Example:"
	echo "    MakeRelease.sh 2.3.0.0 ../build-WT-Desktop5-Debug 12"
	echo "Note: Version number should include ww.xx.yy.zz"
        echo "WHERE:"
	echo "  ww -- Major"
	echo "  xx -- Minor"
	echo "  yy -- Rev"
	echo "  zz -- Patch"
#	echo "Parts of the version are padded with 0 because .deb treat is as a full number and warn about upgrading to an old version without it"
	exit 1
fi

Variant=$3

set -x

mkdir -p build
rm -r build/WhippyTerm$Variant_$1-1 2>/dev/null
cp -r Debian$Variant.template/ build/WhippyTerm$Variant_$1-1/
gzip -9 -c -n ../Tools/MakeReleaseNotes/changelog.Debian > build/WhippyTerm$Variant_$1-1/usr/share/doc/whippyterm/changelog.Debian.gz
chmod 644 build/WhippyTerm$Variant_$1-1/usr/share/doc/whippyterm/changelog.Debian.gz
cp ../manpages/whippyterm.1 build/WhippyTerm$Variant_$1-1/usr/share/man/man1/whippyterm.1
chmod 644 build/WhippyTerm$Variant_$1-1/usr/share/man/man1/whippyterm.1
gzip -9n build/WhippyTerm$Variant_$1-1/usr/share/man/man1/whippyterm.1
rm -r build/WhippyTerm$Variant_$1-1/usr/bin/README
rm -r build/WhippyTerm$Variant_$1-1/usr/share/man/man1/README

sed -i "s/Version: x.x.x.x-1/Version: $1-1/g" build/WhippyTerm$Variant_$1-1/DEBIAN/control

objcopy --strip-debug --strip-unneeded $2/WhippyTerm build/WhippyTerm$Variant_$1-1/usr/bin/whippyterm
chmod 755 build/WhippyTerm$Variant_$1-1/usr/bin/whippyterm
chmod 0755 build/WhippyTerm$Variant_$1-1/usr
chmod 0755 build/WhippyTerm$Variant_$1-1/usr/bin
chmod 0755 build/WhippyTerm$Variant_$1-1/usr/share
chmod 0755 build/WhippyTerm$Variant_$1-1/usr/share/applications
chmod 0644 build/WhippyTerm$Variant_$1-1/usr/share/applications/WhippyTerm.desktop
chmod 0755 build/WhippyTerm$Variant_$1-1/usr/share/icons
chmod 0644 build/WhippyTerm$Variant_$1-1/usr/share/icons/whippyterm.png
chmod 0755 build/WhippyTerm$Variant_$1-1/usr/share/doc
chmod 0755 build/WhippyTerm$Variant_$1-1/usr/share/doc/whippyterm
chmod 0644 build/WhippyTerm$Variant_$1-1/usr/share/doc/whippyterm/copyright

fakeroot dpkg-deb --build build/WhippyTerm$Variant_$1-1
cp build/WhippyTerm$Variant_$1-1.deb .
