#!/bin/sh

############################################################################
#
# TheBar.mcc - Next Generation Toolbar MUI Custom Class
# Copyright (C) 2003-2005 Alfonso Ranieri
# Copyright (C) 2005-2013 by TheBar.mcc Open Source Team
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# TheBar class Support Site:  http://www.sf.net/projects/thebar
#
# $Id$
#
############################################################################

# TheBar.mcc release build script
# invoke this script as "./mkrelease.sh" to build the release archives

rm -rf "release"
mkdir -p "release"
mkdir -p "release/MCC_TheBar"
mkdir -p "release/MCC_TheBar/Demos"
mkdir -p "release/MCC_TheBar/Demos/pics"
mkdir -p "release/MCC_TheBar/Developer"
mkdir -p "release/MCC_TheBar/Developer/Autodocs"
mkdir -p "release/MCC_TheBar/Developer/Autodocs/MCC"
mkdir -p "release/MCC_TheBar/Developer/C"
mkdir -p "release/MCC_TheBar/Developer/C/Examples"
mkdir -p "release/MCC_TheBar/Developer/C/Examples/b2chunky"
mkdir -p "release/MCC_TheBar/Developer/C/include"
mkdir -p "release/MCC_TheBar/Developer/C/include/mui"
mkdir -p "release/MCC_TheBar/Docs"
mkdir -p "release/MCC_TheBar/Libs"
mkdir -p "release/MCC_TheBar/Libs/MUI"
mkdir -p "release/MCC_TheBar/Locale"
mkdir -p "release/MCC_TheBar/Locale/Catalogs"
mkdir -p "release/MCC_TheBar/Tools"

make -C mcc release
make -C mcc/button release
make -C mcp release
make -C demo release

for os in os3 os4 mos aros-i386 aros-ppc aros-x86_64; do
	case $os in
	    os3)         fullsys="AmigaOS3";;
	    os4)         fullsys="AmigaOS4";;
	    mos)         fullsys="MorphOS";;
	    aros-i386)   fullsys="AROS-i386";;
	    aros-ppc)    fullsys="AROS-ppc";;
	    aros-x86_64) fullsys="AROS-x86_64";;
	esac
	mkdir -p "release/MCC_TheBar/Libs/MUI/$fullsys"
	cp -a demo/bin_$os/demo1 "release/MCC_TheBar/Demos/demo1-$fullsys"
	cp -a demo/bin_$os/demo2 "release/MCC_TheBar/Demos/demo2-$fullsys"
	cp -a demo/bin_$os/demo3 "release/MCC_TheBar/Demos/demo3-$fullsys"
	cp -a demo/bin_$os/demo4 "release/MCC_TheBar/Demos/demo4-$fullsys"
	cp -a demo/bin_$os/demo5 "release/MCC_TheBar/Demos/demo5-$fullsys"
	cp -a demo/bin_$os/demo6 "release/MCC_TheBar/Demos/demo6-$fullsys"
	cp -a demo/bin_$os/demo7 "release/MCC_TheBar/Demos/demo7-$fullsys"
	cp -a demo/bin_$os/demo8 "release/MCC_TheBar/Demos/demo8-$fullsys"
	cp -a demo/bin_$os/demo9 "release/MCC_TheBar/Demos/demo9-$fullsys"
	cp -a demo/bin_$os/demo10 "release/MCC_TheBar/Demos/demo10-$fullsys"
	cp -a mcc/bin_$os/TheBar.mcc "release/MCC_TheBar/Libs/MUI/$fullsys/"
	cp -a mcc/bin_$os/TheBarVirt.mcc "release/MCC_TheBar/Libs/MUI/$fullsys/"
	cp -a mcc/button/bin_$os/TheButton.mcc "release/MCC_TheBar/Libs/MUI/$fullsys/"
	cp -a mcp/bin_$os/TheBar.mcp -a "release/MCC_TheBar/Libs/MUI/$fullsys/"
done

make -C mcp catalogs
for language in czech french german greek italian polish swedish turkish; do
	mkdir -p "release/MCC_TheBar/Locale/Catalogs/$language"
	cp -a mcp/locale/$language.catalog "release/MCC_TheBar/Locale/Catalogs/$language/TheBar_mcp.catalog"
done

cp -a -R dist/* "release/"
cp -a AUTHORS ChangeLog COPYING "release/MCC_TheBar/"
cp -a doc/MCC_TheBar.readme "release/MCC_TheBar/ReadMe"
cp -a demo/pics/* "release/MCC_TheBar/Demos/pics/"
cp -a doc/MCC_TheBar.readme "release/MCC_TheBar/Docs/"
cp -a doc/b2chunky.doc "release/MCC_TheBar/Docs/"
cp -a doc/fkeys.doc "release/MCC_TheBar/Docs/"
cp -a doc/MCC_TheBar.doc "release/MCC_TheBar/Developer/Autodocs/MCC/"
cp -a demo/*.c "release/MCC_TheBar/Developer/C/Examples/"
cp -a demo/*.h "release/MCC_TheBar/Developer/C/Examples/"
cp -a demo/Makefile "release/MCC_TheBar/Developer/C/Examples/"
cp -a demo/b2chunky/* "release/MCC_TheBar/Developer/C/Examples/b2chunky/"
cp -a include/mui/TheBar_mcc.h "release/MCC_TheBar/Developer/C/include/mui/"
cp -a mcp/locale/TheBar_mcp.cd "release/MCC_TheBar/Locale/"

releasever=`grep "#define LIB_VERSION" mcc/version.h | awk '{ print $3 }'`
releaserev=`grep "#define LIB_REVISION" mcc/version.h | awk '{ print $3 }'`

echo "  MK MCC_TheBar-$releasever.$releaserev.lha"
find release -nowarn -name ".svn" -exec rm -rf {} \; 2>/dev/null
cd release
rm -f ../MCC_TheBar-$releasever.$releaserev.lha
lha -ao5q ../MCC_TheBar-$releasever.$releaserev.lha *
cp MCC_TheBar/ReadMe ../MCC_TheBar-$releasever.$releaserev.readme
cd ..
