# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

import os
from html import *

def makePicture( path, description, languages ):
    root = '../../'

    got_it = False
    for lang in languages:
        if os.path.exists( os.getcwd()+'/scripts/template/www/'+lang+'.py' ):
            gallery = __import__( lang, globals(), locals())
            got_it = True
            break
    if not got_it:
        print 'Error - no gallery language file found?!'
        raise KeyError()

    filename  = os.path.basename( path )
    directory = os.path.dirname( path )
    thumbnail = root + os.path.join( directory, 'thumbnails', filename )
    path      = root + path

    result = Table \
    (
        cellpadding = 4, cellspacing = 0, border = 1, width = '100%%',
        contents = TR \
        ( [
            TD( width = '328', contents = A( href = path, contents = Img( src = thumbnail ) ) ),
            TD \
            ( 
                width = '*', contents =
                [ 
                    description, BR(), '&nbsp;&nbsp;&nbsp;&nbsp;', 
                    A( href = path, contents = gallery.largeImage ) 
                ] 
            )
        ] )
    )
    
    return str( result )

