# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

from html import *

class Tree( Container ):
    INDENT = '10'
    
    def __init__( self, contents=None ):
        Container.__init__( self, contents )
        
    def __str__( self ):
        depth    = self.getDepth()
        table    = Table( 
            border = 0, cellpadding = 0, cellspacing = 2, CLASS = 'tree',
            contents = self.buildLevel( self, 0, depth )
        )

        return table.__str__()
                        
    def buildLevel( self, container, currentDepth, maxDepth ):
        contents = list()
        
        for child in container:
            if isinstance( child, Tree ):
                contents.extend( self.buildLevel( child, currentDepth + 1, maxDepth ) )
            else:
                tr = TR()
                colspan = maxDepth - currentDepth
                if currentDepth >= 1:
                    if maxDepth >= 3:
                        for i in range( currentDepth - 1 ):
                            tr.append( TD( '', width=Tree.INDENT ) )

                    #tr.append( TD( '+', width=Tree.INDENT ) )
                    tr.append( 
                        TD( 
                            Img( src='%(ROOT)simages/bullet.gif' ), 
                            align = 'right', valign = 'middle', width=Tree.INDENT 
                        ) 
                    )
                
                if colspan >= 2:
                    tr.append( TD( child, colspan=`colspan` ) )
                else:
                    tr.append( TD( child ) )

                contents.append( tr )

        return contents
        
    def getDepth( self ):
        def _depth( container ):
            max = 0
            for child in container:
                if isinstance( child, Container ):
                    childDepth = _depth( child )
                    if childDepth > max:
                        max = childDepth

            return max + 1 # Count this container.
            
        return _depth( self )


class Box( Container ):
    # Colors
    TITLE_BG = '#c97f30'
    TITLE_FG = '#000000'
    
    def __init__( self, contents=None, title=None ):
        Container.__init__( self, contents )
        
        self.title = title
        
    def __str__( self ):
        contents = TD( align = 'center' )
        for child in self:
            contents.append( child )
        
        table = Table(
            width = '100%%', cellspacing = 4, cellpadding = 0, border = 0,
            contents = [
                TR( 
                    TD( 
                        bgcolor = self.TITLE_BG, align = 'center', 
                        contents = Font(
                            size = '-1', color = self.TITLE_FG,
                            contents = B( self.title ) 
                        ) 
                    )
                ),
                TR( contents )                    
            ]
        )
        
        return table.__str__() 
    
