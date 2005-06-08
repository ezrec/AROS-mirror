# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
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
    
    if os.path.split( path )[1] in IGNORE_ALL or path in IGNORE_ABS:
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

def remove( path ):
    if os.path.exists( path ):
        os.remove( path )

def reportSkipping( message ):
    print '\033[1m\033[32m*\033[30m', message, '\033[0m'

def reportBuilding( message ):
    print '\033[1m\033[33m*\033[30m', message, '\033[0m'
  
