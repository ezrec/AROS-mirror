# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

import sys, os
from db.tasks.model import *
from html_ import *

# Settings

C_DONE   = 'green'
C_INWORK = 'yellow'
C_TODO   = 'red'

def format( root, directory, template ):
    # First, format this category.
    content = Table( bgcolor = '#999999', width = '100%', cellpadding = 2 )

    for item in root:
        row = TR( bgcolor = '#dddddd' )
        
        if isinstance( item, Category ):
            row.append \
            ( 
                TD( A( href = item.id + '.html', contents = item.description ) )
            )
            row.append \
            (
                TD \
                (
                    width = '80%', height = '100%',
                    contents = Table \
                    (
                        bgcolor = 'black', width = '100%', height = '100%',
                        cellspacing = 2, contents = TR \
                        ( 
                            TD \
                            ( 
                                Table \
                                (
                                    bgcolor = 'white', width = '100%', height = '100%',
                                    cellspacing = 0, cellpadding = 0, contents = TR \
                                    (
                                        height = '100%',
                                        contents = \
                                        [
                                            TD \
                                            ( 
                                                bgcolor = C_DONE, 
                                                width = `item.done * 100 / item.total` + '%'
                                            ),
                                            TD \
                                            (
                                                bgcolor = C_INWORK,
                                                width = `item.inwork * 100 / item.total` + '%'
                                            ),
                                            TD \
                                            (
                                                bgcolor = C_TODO,
                                                width = `item.todo * 100 / item.total` + '%'
                                            )
                                        ]
                                    )
                                )
                            )
                        )
                    )
                )
            )
        else:
            if   item.status == Task.DONE:   color = C_DONE
            elif item.status == Task.INWORK: color = C_INWORK
            elif item.status == Task.TODO:   color = C_TODO
            
            row.append( TD( item.description ) )
            row.append \
            ( 
                TD \
                (
                    width = '80%', height = '100%',
                    contents = Table \
                    ( 
                        bgcolor = 'black', width = '100%', height = '100%',
                        cellspacing = 2,
                        contents = TR \
                        ( 
                            height = '100%',
                            contents = TD( height = '100%', bgcolor = color, width = '*' ) 
                        )
                    )
                )
            )


        content.append( row )

    strings = {
        'ROOT' : '../../',
        'BASE' : '../../',
        'CONTENT' : '<h1>Status: ' + root.description + '</h1>' + str( content )
    }

    output = file( os.path.join( directory, root.id + '.html' ), 'w' )
    output.write( template % strings )
    output.close()

    # Second, recurse down.
    for item in root:
        if isinstance( item, Category ):
            format( item, directory, template )


