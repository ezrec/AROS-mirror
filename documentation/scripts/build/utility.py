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
    if newer( src, dst ):
        shutil.copy( src, dst )

def reportSkipping( message ):
    print '\033[1m\033[32m*\033[30m', message, '\033[0m'

def reportBuilding( message ):
    print '\033[1m\033[33m*\033[30m', message, '\033[0m'
  
