#!/bin/bash

if [ "x$1" = "x" ] ; then
    echo "Usage: $0 [-s savedir] package..."
    exit 0
fi

# Check for path in AROS. Note that the variable has a "../" more,
# because we cd into the package dir later
if [ -d ../../bin/contrib/gnu ] ; then
    PKGDIR="$PKGDIR ../../../bin/contrib/gnu"
fi
# Check for standard paths used by other systems
# Gentoo:
if [ -d /usr/portage/distfiles ] ; then
    PKGDIR="$PKGDIR /usr/portage/distfiles"
fi
# Also look in the subdir where the sources will be installed
PKGDIR=". $PKGDIR"

# Set a directory where the user has write access (for internet fetches)
if [ "x$1" = "x-s" ] ; then
    SAVEDIR=$2
    shift
    shift
fi
# If none was specified, then CWD should do it...
if [ "x$SAVEDIR" = "x" ] ; then
    SAVEDIR=.
fi

# Now search for each argument the .description
while [ ! "x$1" = "x" ] ; do
    if [ -e $1/.description ] ; then
	pname=`cut -d' ' -f2 $1/.description`
	aname=`cut -d' ' -f3 $1/.description`
	location=`cut -d' ' -f4 $1/.description`
	pdiff=`cut -d' ' -f5 $1/.description`
	if [ -d "$1/${pname}" ] ; then
	    echo Directory $1/${pname} already exists! Skipping...
	else
	    cd $1
	    PROGFOUND=""
	    for i in $PKGDIR ; do
		if [ "x$PROGFOUND" = "x" ] ; then
		    echo Looking in $i
		    if [ -e $i/$aname ] ; then
			PROGFOUND=$i/$aname
		    fi
		fi
	    done
	    if [ "x$PROGFOUND" = "x" ] ; then
		echo Archive not found, trying to fetch from internet...
		olddir=`pwd`
		cd $SAVEDIR
		fetch $location/$aname
		PROGFOUND=$SAVEDIR/$aname
		cd $olddir
	    fi
	    if [ -e "$PROGFOUND" ] ; then
		echo "Uncompressing $PROGFOUND"
		tar xfz $PROGFOUND
		cd $pname
		patch -p1 -s < ../$pdiff
		cd ..
	    else
		echo "Could not install $pname!"
	    fi
	    cd ..
	fi
    else
	echo Package $1 not found.
    fi

    # Get next package to install
    shift
done
