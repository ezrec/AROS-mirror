from html import *

class Tree( Container ):
    INDENT = '10'
    
    def __init__( self, contents=None ):
        Container.__init__( self, contents )
        
    def __str__( self ):
        depth    = self.getDepth()
        contents = self.buildLevel( self, 0, depth )
        table    = Table( contents )

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
                    tr.append( TD( Img( src='%(ROOT)simages/bullet.png' ), width=Tree.INDENT ) )
                
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
    BORDER  = '#000000'
    TITLE   = '#aaaaaa'
    CONTENT = '#dddddd'
    
    def __init__( self, contents=None, title=None ):
        Container.__init__( self, contents )
        
        self.title = title
        
    def __str__( self ):
        contents = TD( align = 'center', bgcolor = Box.CONTENT )
        for child in self:
            contents.append( child )
        
#        table = Table \
#        ( [
#            TR \
#            ( [
#                TD( bgcolor=Box.BORDER, width=10 ),
#                TD( bgcolor=Box.BORDER, width='*' ),
#                TD( bgcolor=Box.BORDER, width=10 )
#              ], height=10 
#            ),
#            TR \
#            ( [
#                TD( bgcolor=Box.BORDER ),
#                TD( bgcolor=Box.TITLE, contents=B(self.title), align='center' ),
#                TD( bgcolor=Box.BORDER )
#            ] ),
#            TR \
#            ( [
#                TD( bgcolor=Box.BORDER ),
#                contents,
#                TD( bgcolor=Box.BORDER ),
#            ] ),
#            TR \
#            ( [ 
#                TD( bgcolor=Box.BORDER ),
#                TD( bgcolor='yellow' ),
#                TD( bgcolor=Box.BORDER )
#              ], height=10 
#            )
#        ], width='100%' )
        
        table = Table \
        (
            width    = '100%%', cellspacing = 1, cellpadding = 3, 
            bgcolor  = Box.BORDER, 
            contents = \
            [
                TR( TD( bgcolor = Box.TITLE, align='center', contents = B(self.title) ) ),
                TR( contents )
            ]
        )
        
        return table.__str__() 
    
