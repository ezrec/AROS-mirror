#!/bin/sh

# the arguments of this script are the compiler name and flags

compiler=`echo $* | sed -e 's/-static//g'`
if [ `basename $1` = "gcc" ] && ($compiler -v) >/dev/null 2>&1 ; then
 gccversion=`$compiler -v 2>&1 | sed -e "/version/!d"`
else
 gccversion="$1 (unknown version)"
fi

libcversion="unknown version"
if ($* hello.c -o hello) >/dev/null 2>&1; then
 if (ldd hello) > /dev/null 2>/dev/null; then
    libcversion=`ldd hello 2>&1 | sed -e 's/.*static.*/static/'`
    if [ ! "$libcversion" = "static" ] ; then
     libcversion="unknown version"
    else
      libcversion="unknown version"
      if ($compiler hello.c -o hello) >/dev/null 2>&1; then
       if (ldd hello) >/dev/null 2>/dev/null; then
      	  libcversion=`ldd hello 2>&1`
      	  libcversion=`echo $libcversion | sed -e '/libc/!d'\
      		       -e 's/^[ 	]*//' -e 's/.*=>[ 	][ 	]*\([^ 	]*\).*/\1/'`
      	  # remember the current directory
      	  current=`pwd`
      	  while [ -L $libcversion ] && [ ! "$libcversion" = "unknown version" ] ; do
      	      libcitself=`basename $libcversion`
      	      libpath=`echo $libcversion | sed -e "s/$libcitself$//"`
      	      if [ -d $libpath ] ; then
      		cd $libpath
      	      fi
      	      if ls $libcitself >/dev/null 2>/dev/null ; then
      		libcversion=`ls -l $libcitself | \
      			     sed -e 's/.*->[ 	][ 	]*\(.*\)$/\1/'`
      	      else
      		# something must have gone wrong, let's bail out
      		libcversion="unknown version"
      	      fi
      	    done
      	  # return to the current directory
      	  cd $current
       fi
      fi
    fi
  fi
fi

rm -f sysinfo.crm sysinfoc.c hello

# this bombs out on Ultrix which expect "cut -d"

compsystem=`uname -a | cut -b 1-78`
compdate=`date|cut -b1-55`

# let's hope that ctrl-c is not part of any string here
# this also will barf later if " is in any of the strings

for i in sysinfo.c sysinfoc.c ; do
 sed -e "s%CCVERSION%$gccversion" -e "s%LIBCVERSION%$libcversion"\
     -e "s%SYSTEM%$compsystem" -e "s%DATE%$compdate"\
   ${i}.template > $i
done