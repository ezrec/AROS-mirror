# -*- coding: iso-8859-1 -*-
# Copyright © 2002-2007, The AROS Development Team. All rights reserved.
# $Id$

import os, shutil
from build.settings import *

def newer( src, dst ):
    """
        srcs - list of source paths
        dst  - dst

        Returns true if any path in srcs is newer than dst
        or if dst does not exist.
    """

    if not os.path.exists( dst ): return True

    if not isinstance( src, list ): src = [ src ]

    dstmtime = os.path.getmtime( dst )
    for path in src:
        srcmtime = os.path.getmtime( path )
        if srcmtime > dstmtime:
            return True

    return False


def ignore( path ):
    path = os.path.normpath( path )
    filename = os.path.split( path )[1]

    if filename in IGNORE_ALL or filename[0] == '.' or path in IGNORE_ABS:
        return True
    else:
        return False


def makedir( path ):
    if not os.path.exists( path ):
        os.makedirs( path )


def copy( src, dst ):
    if not isinstance( src, list ): src = [ src ]
     
    for path in src:
        if os.path.isdir( dst ):
            rdst = os.path.join( dst, os.path.basename( path ) )
        else:
            rdst = dst

        if newer( path, rdst ):
            shutil.copy( path, rdst )


def pathscopy( entries, srcpath, dstpath):
    """
        entries - entry or list of entries to copy
        srcpath - source to copy from
        dstpath - destination to copy to

        Note that the entries can themselves include paths.
    """
    if not isinstance( entries, list ): entries = [ entries ]

    pathentries= map( (lambda e: os.path.join( srcpath, e)), entries)
    copy( pathentries, dstpath)


def remove( path ):
    if os.path.exists( path ):
        os.remove( path )


# similar shutil.copytree, but ignores .svn directory
def copytree(src, dst, symlinks=0):
    names = os.listdir(src)
    os.mkdir(dst)
    for name in names:
        srcname = os.path.join(src, name)
        dstname = os.path.join(dst, name)
        try:
            if symlinks and os.path.islink(srcname):
                linkto = os.readlink(srcname)
                os.symlink(linkto, dstname)
            elif os.path.isdir(srcname):
                if os.path.basename(srcname) != ".svn":
                    copytree(srcname, dstname, symlinks)
            else:
                if newer( srcname, dstname ):
                    shutil.copy( srcname, dstname )
        except (IOError, os.error), why:
            print "Can't copy %s to %s: %s" % (`srcname`, `dstname`, str(why))


def reportSkipping( message ):
    print '\033[1m\033[32m*\033[30m', message, '\033[0m'


def reportBuilding( message ):
    print '\033[1m\033[33m*\033[30m', message, '\033[0m'
  
