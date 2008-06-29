# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

import sys, os
from db.tasks.model import *
from html_ import *

# Settings

C_Completed             = 'green'
C_NeedsSomeWork         = 'yellow'
C_NotImplemented        = 'red'
C_Skipped               = 'green'
C_ArchitectureDependant = 'lightseagreen'
C_AmigaOnly             = 'lightgray'

def format( root, directory, template, lang, extension ):
    # First, format this category.
    content = Table( bgcolor = '#999999', width = '100%', cellpadding = 2 )

    for item in root:
        row = TR( bgcolor = '#dddddd' )
        
        if isinstance( item, Category ):

            # Calculate category score
            categoryscore = \
                ( 100 * ( item.completed + item.skipped ) + 50 * item.needssomework ) \
                / (item.total - item.architecturedependant - item.amigaonly)

            # Dynamically build category status chart
            # The 'if' are needed here, because when sum(width != 100%) and there are
            # entries with 0%, they are still beeing drawn
            dyncontents = []

            if item.completed != 0:
                dyncontents.append(TD \
                             ( 
                             bgcolor = C_Completed, 
                             width = `int (round( item.completed * 100.0 / item.total, 0 ) )` + '%'
                             ) )

            if item.skipped != 0:
                dyncontents.append(TD \
                             ( 
                             bgcolor = C_Skipped, 
                             width = `int (round( item.skipped * 100.0 / item.total, 0 ) )` + '%'
                             ) )

            if item.architecturedependant != 0:
                dyncontents.append(TD \
                             ( 
                             bgcolor = C_ArchitectureDependant, 
                             width = `int (round( item.architecturedependant * 100.0 / item.total, 0 ) )` + '%'
                             ) )

            if item.amigaonly != 0:
                dyncontents.append(TD \
                             ( 
                             bgcolor = C_AmigaOnly, 
                             width = `int (round( item.amigaonly * 100.0 / item.total, 0 ) )` + '%'
                             ) ) 
           
            if item.needssomework != 0:
                dyncontents.append(TD \
                             ( 
                             bgcolor = C_NeedsSomeWork, 
                             width = `int (round( item.needssomework * 100.0 / item.total, 0 ) )` + '%'
                             ) )

            if item.notimplemented != 0:
                dyncontents.append(TD \
                             ( 
                             bgcolor = C_NotImplemented, 
                             width = `int (round( item.notimplemented * 100.0 / item.total, 0 ) )` + '%'
                             ) )
            row.append \
            ( 
                TD( A( href = item.id + extension, contents = item.description ) )
            )
            row.append \
            (
                TD( width = '5%', height = '100%', align = 'right', contents = str( categoryscore ) + '%' )
            )
            row.append \
            (
                TD \
                (
                    width = '75%', height = '100%',
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
                                        contents = dyncontents
                                    )
                                )
                            )
                        )
                    )
                )
            )
        else:
            if   item.status == CategoryItem.STAT_Completed:   color = C_Completed
            elif item.status == CategoryItem.STAT_NeedsSomeWork: color = C_NeedsSomeWork
            elif item.status == CategoryItem.STAT_NotImplemented:   color = C_NotImplemented
            elif item.status == CategoryItem.STAT_Skipped: color = C_Skipped
            elif item.status == CategoryItem.STAT_ArchitectureDependant:   color = C_ArchitectureDependant
            elif item.status == CategoryItem.STAT_AmigaOnly: color = C_AmigaOnly
            
            row.append( TD( item.description ) )
            row.append \
            ( 
                TD \
                (
                    width = '75%', height = '100%',
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

    contentstr = '<h1>Status: ' + root.description + '</h1>' + str( content )
    if lang == 'en':
        strings = {
            'ROOT'    : '../../',
            'BASE'    : '../../',
            'CONTENT' : contentstr
        }
    else:
        strings = {
            'ROOT'    : '../../../',
            'BASE'    : '../../../',
            'CONTENT' : contentstr
        }

    output = file( os.path.join( directory, root.id + extension ), 'w' )
    output.write( template % strings )
    output.close()

    # Second, recurse down.
    for item in root:
        if isinstance( item, Category ):
            format( item, directory, template, lang, extension )


