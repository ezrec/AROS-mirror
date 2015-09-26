# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

import os
from html import *
from ConfigParser import ConfigParser

def makePicture( path, description, language ):
    LANG_DIR   = 'targets/www/template/languages'

    if language == 'en':
        root = '../../'
    else:
        root = '../../../'

    config = ConfigParser()
    config.read( os.path.join( LANG_DIR, language ) )

    filename  = os.path.basename( path )
    directory = os.path.dirname( path )
    thumbnail = root + os.path.join( directory, 'thumbnails', filename )
    path      = root + path

    result = Div \
    ( \
        style = "margin: 4px; padding: 2px; border: 1px solid #000000; height: 400; width: 320; float: left; text-align: center;",
        contents = \
        ( \
            [ \
                A \
                ( \
                    href = path,
                    contents = Img( src = thumbnail, style = "display: inline; margin: 5px; border: 1px solid #ffffff"),
                ),
                Div( contents = description, style = "text-align: center; font-weight: normal; width: auto; margin: 5px;"),
            ]
        ),
    )
    
    return str( result )
