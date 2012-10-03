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
        if ( item.total - item.amigaonly ) != 0:
            return ( 100 * ( item.completed + item.skipped ) + 50 * item.needssomework ) \
                    / ( item.total - item.amigaonly )
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
        TD( A( href = item.category + extension, contents = item.description ) )
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

def formatReturnLink( parent, extension ):
    if parent is not None:
        return 'Return to ' + str( A( href = parent.category + extension, contents = parent.description ) ) +'<br/>'
    else:
        return ''

def formatLegend( ):
    legend = Table \
             (
                contents =
                [
                TR \
                (
                    contents =
                    [
                    TD( formatLegendColorTable( C_Completed ) ), TD( ' - Completed,' ),
                    TD( formatLegendColorTable( C_NeedsSomeWork ) ), TD( ' - Needs some work,' ),
                    TD( formatLegendColorTable( C_NotImplemented ) ), TD( ' - Not implemented,' ),
                    TD( formatLegendColorTable( C_AmigaOnly ) ), TD( ' - Amiga only' )
                    ]
                ),
                TR \
                (
                    contents = 
                    [
                    TD \
                    (
                        colspan = 8,
                        contents = Table \
                        (
                            TR \
                            (
                                contents =
                                [
                                TD
                                (
                                    contents = Table \
                                    ( 
                                        bgcolor = '#999999',
                                        contents = TR \
                                        (
                                            bgcolor = '#dddddd',
                                            contents = TD \
                                            (
                                                contents = '100% (2008-07-31)'
                                            )
                                        )
                                    )
                                ),
                                TD
                                (
                                    style = 'padding-top: 4px',
                                    contents = '- 100% = completeness score, 2008-07-31 = date of last update'
                                )
                                ]
                            )
                        )
                    )
                    ]
                )
                ]
            )             

    return legend

def format( root, directory, template, lang, extension, parent = None ):
    
    # First, format this category.

    content_CategoryItemsOriginalAPI = Table( bgcolor = '#999999', width = '98%', cellpadding = 2 )
    content_CategoryItemsExtensionAPI = Table( bgcolor = '#999999', width = '98%', cellpadding = 2 )
    #FIXME: GRRR: how do you create UL in html_?
    content_Comments = '<ul>'

    # Format category items
    for item in root.categoryitems:
        row = formatRowCategoryItem( item, extension )
        if item.apiversion == CategoryItem.API_Original:
            content_CategoryItemsOriginalAPI.append( row )
        elif item.apiversion == CategoryItem.API_Extension:
            content_CategoryItemsExtensionAPI.append( row )

    # Format category comments                
    for item in root.comments:
       content_Comments += '<li>' + item.comment + '</li>'
    content_Comments += '</ul>'

    contentstr = '<h2 align="center" style="font-size:large">AROS implementation status</h2>'

    if parent is not None:
        contentstr += str( formatReturnLink( parent, extension ) )

        contentstr += '\n<br/>' + str( formatHeader( root ) )

        contentstr += '\n<br/>' + str( formatLegend( ) )

        content_Categories = Table( bgcolor = '#999999', width = '98%', cellpadding = 2 )

        # Format subcategories
        for item in root.subcategories:
            row = formatRowCategory( item, extension )
            content_Categories.append( row )
        
        if len( content_Categories ) > 0:
            contentstr += '\n<br>' + str ( content_Categories )
            
    else:
        # Different formatting for main page (show two levels of subcategories)
        
        # Format subcategories

        for item in root.subcategories:
            contentstr += '\n<br/>' + str( formatHeader( item ) )
            content_Categories = Table( bgcolor = '#999999', width = '98%', cellpadding = 2 )

            for subitem in item.subcategories:
                row = formatRowCategory( subitem, extension )
                content_Categories.append( row )
            
            if len( content_Categories ) > 0:
                contentstr += '\n<br>' + str ( content_Categories )

    # Add category items
    if len( content_CategoryItemsOriginalAPI ) > 0:
        contentstr += '\n<h2 align="center">Original API</h2>' + str( content_CategoryItemsOriginalAPI )

    if len( content_CategoryItemsExtensionAPI ) > 0:
        contentstr += '\n<h2 align="center">Extensions API</h2>' + str( content_CategoryItemsExtensionAPI )


    # Add comments
    if content_Comments != '<ul></ul>':
        contentstr += '\n<h2 align="left">Comments</h2>' + str( content_Comments )



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

    output = file( os.path.join( directory, root.category + extension ), 'w' )
    output.write( template % strings )
    output.close()

    # Second, recurse down.
    for item in root.subcategories:
        format( item, directory, template, lang, extension, root )


