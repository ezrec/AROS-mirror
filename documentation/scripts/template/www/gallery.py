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
        CLASS = "gallery",
        contents = \
        ( \
            [ \
                A \
                ( \
                    href = path,
                    contents = Img( src = thumbnail),
                ),
                Div( contents = description, CLASS = "gallerydesc"),
            ]
        ),
    )
    
    return str( result )
