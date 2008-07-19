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
C_AmigaOnly             = 'tan'

def calculateCategoryScore( item ):
    if isinstance( item, Category):
        if item.total != 0:
            return ( 100 * ( item.completed + item.skipped ) + 50 * item.needssomework ) \
                    / (item.total - item.amigaonly)
    return 0;

def formatRowCategoryStatus(array, count, total, color):
    if count != 0:
        array.append( TD( bgcolor = color, width = `int ( round( count * 100.0 / total, 0 ) )` + '%' ) )

def formatRowCategory( item, extension ):

    row = TR( bgcolor = '#dddddd' )

    # Dynamically build category status chart
    # The 'if' are needed here, because when sum(width != 100%) and there are
    # entries with 0%, they are still beeing drawn
    dyncontents = []

    formatRowCategoryStatus( dyncontents, item.completed, item.total, C_Completed)
    formatRowCategoryStatus( dyncontents, item.skipped, item.total, C_Skipped)
    formatRowCategoryStatus( dyncontents, item.amigaonly, item.total, C_AmigaOnly)
    formatRowCategoryStatus( dyncontents, item.needssomework, item.total, C_NeedsSomeWork)
    formatRowCategoryStatus( dyncontents, item.notimplemented, item.total, C_NotImplemented)

    row.append \
    ( 
        TD( A( href = item.id + extension, contents = item.description ) )
    )
    row.append \
    (
        TD \
        ( width = '15%', height = '100%', align = 'right', \
          contents = str( calculateCategoryScore( item ) ) + '% (' + item.lastupdated + ')' 
        )
    )
    row.append \
    (
        TD \
        (
            width = '65%', height = '100%',
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

    return row

def formatRowCategoryItem( item, extension ):
    
    row = TR( bgcolor = '#dddddd' )

    if   item.status == CategoryItem.STAT_Completed:   color = C_Completed
    elif item.status == CategoryItem.STAT_NeedsSomeWork: color = C_NeedsSomeWork
    elif item.status == CategoryItem.STAT_NotImplemented:   color = C_NotImplemented
    elif item.status == CategoryItem.STAT_Skipped: color = C_Skipped
    elif item.status == CategoryItem.STAT_AmigaOnly: color = C_AmigaOnly
    
    description = item.description
    if item.architecture == CategoryItem.ARCH_i386:
        description = description + ' (<i>i386</i>)'
        
    row.append( TD( description ) )
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
    
    return row

def formatHeader( root ):

    header = Table \
             (
                bgcolor = '#999999', width = '40%', cellpadding = 2, \
                contents = TR \
                (
                    bgcolor = 'lightblue', style='font-size:medium;font-weight:bold',
                    contents = [
                    TD \
                    (
                        contents = 'Status: ' + root.description
                    ),
                    TD \
                    (
                        width = '15%', align = 'right',
                        contents = str( calculateCategoryScore( root ) ) + '%'
                    )
                    ]
                )
             )
    return header

def formatLegendColorTable( color ):

    legendColorTable = Table \
                     ( 
                        bgcolor = '#999999', width = '20px', height = '20px', cellpadding = 2,
                        contents = TR \
                        (
                            bgcolor = '#dddddd',
                            contents = TD \
                            (
                                Table \
                                (
                                    bgcolor = 'black', width = '100%', height = '100%',
                                    cellspacing = 2, contents = TR \
                                    (
                                        height = '100%',
                                        contents = TD \
                                        (
                                            bgcolor = color,
                                        )
                                    )
                                )
                            )
                        )
                     )

    return legendColorTable

def formatLegend( ):
    legend = Table \
             (
                TR \
                (
                    contents =
                    [
                    TD( formatLegendColorTable( C_Completed ) ), TD( ' - Completed,' ),
                    TD( formatLegendColorTable( C_NeedsSomeWork ) ), TD( ' - Needs some work,' ),
                    TD( formatLegendColorTable( C_NotImplemented ) ), TD( ' - Not implemented,' ),
                    TD( formatLegendColorTable( C_AmigaOnly ) ), TD( ' - Amiga only' )
                    ]
                )
            )             

    return legend

def format( root, directory, template, lang, extension ):
    
    # First, format this category.
    content_Categories = Table( bgcolor = '#999999', width = '100%', cellpadding = 2 )
    content_CategoryItemsAmigaOSAPI = Table( bgcolor = '#999999', width = '100%', cellpadding = 2 )
    content_CategoryItemsAROSAPI = Table( bgcolor = '#999999', width = '100%', cellpadding = 2 )

    for item in root:
        
        if isinstance( item, Category ):
            row = formatRowCategory( item, extension )
            content_Categories.append(row)
        else:
            row = formatRowCategoryItem( item, extension )
            if item.apiversion == CategoryItem.API_AmigaOS:
                content_CategoryItemsAmigaOSAPI.append( row )
            elif item.apiversion == CategoryItem.API_AROS:
                content_CategoryItemsAROSAPI.append( row )
                



    contentstr = '<br/>' + str( formatHeader( root ) )

    contentstr += '<br/>' + str( formatLegend( ) )
    
    if len( content_Categories ) > 0:
        contentstr += '<h2 align="center">Categories</h2>' + str ( content_Categories )
    
    if len( content_CategoryItemsAmigaOSAPI ) > 0:
        contentstr += '<h2 align="center">AmigaOS API</h2>' + str( content_CategoryItemsAmigaOSAPI )

    if len( content_CategoryItemsAROSAPI ) > 0:
        contentstr += '<h2 align="center">AROS Extensions API</h2>' + str( content_CategoryItemsAROSAPI )

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


