# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

class Task:
    DONE   = 2
    INWORK = 1
    TODO   = 0

    def __init__( self, id, description, category, status ):
        self.id          = id
        self.description = description
        self.category    = category
        self.status      = status
    
    def __lt__( self, other ): return self.id <  other.id 
    def __le__( self, other ): return self.id <= other.id 
    def __eq__( self, other ): return self.id == other.id 
    def __ne__( self, other ): return self.id != other.id 
    def __gt__( self, other ): return self.id >  other.id 
    def __ge__( self, other ): return self.id >= other.id 

class Category( list ):
    def __init__( self, id, description, category=None ):
        list.__init__( self )
        
        self.id          = id
        self.description = description
        self.category    = category
        
        self.done        = 0
        self.inwork      = 0
        self.todo        = 0
        
        self.total       = 0

    def __lt__( self, other ): return self.id <  other.id 
    def __le__( self, other ): return self.id <= other.id 
    def __eq__( self, other ): return self.id == other.id 
    def __ne__( self, other ): return self.id != other.id 
    def __gt__( self, other ): return self.id >  other.id 
    def __ge__( self, other ): return self.id >= other.id 

    def sort( self ):
        for item in self:
            if item.__class__ == Category:
                item.sort()
                
        list.sort( self )
    
    def recalculate( self ):
        for item in self:
            if item.__class__ != Category:
                self.total += 1

                if   item.status == Task.DONE:
                    self.done   += 1
                elif item.status == Task.INWORK:
                    self.inwork += 1
                elif item.status == Task.TODO:
                    self.todo   += 1

            else:
                item.recalculate()
                
                self.done   += item.done
                self.inwork += item.inwork
                self.todo   += item.todo
                self.total  += item.total

