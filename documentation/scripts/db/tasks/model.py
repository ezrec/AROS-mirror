# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

class CategoryItem:
    STAT_Completed   = 2
    STAT_NeedsSomeWork = 1
    STAT_NotImplemented   = 0
    STAT_Skipped = 3
    STAT_ArchitectureDependant = 4
    STAT_AmigaOnly = 5

    ARCH_Generic = 1
    ARCH_i386 = 2

    API_OS31 = 1
    API_AROS = 2

    def __init__( self, id, description, category, status, architecture, apiversion ):
        self.id             =   id
        self.description    =   description
        self.category       =   category
        self.status         =   status
        self.architecture   =   architecture
        self.apiversion     =   apiversion
    
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
        
        self.completed                  = 0
        self.needssomework              = 0
        self.notimplemented             = 0
        self.skipped                    = 0
        self.architecturedependant      = 0
        self.amigaonly                  = 0
        
        self.total                      = 0

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

                if   item.status == CategoryItem.STAT_Completed:
                    self.completed              += 1
                elif item.status == CategoryItem.STAT_NeedsSomeWork:
                    self.needssomework          += 1
                elif item.status == CategoryItem.STAT_NotImplemented:
                    self.notimplemented         += 1
                elif item.status == CategoryItem.STAT_Skipped:
                    self.skipped                += 1
                elif item.status == CategoryItem.STAT_ArchitectureDependant:
                    self.architecturedependant  += 1
                elif item.status == CategoryItem.STAT_AmigaOnly:
                    self.amigaonly              += 1

            else:
                item.recalculate()
                
                self.completed              += item.completed
                self.needssomework          += item.needssomework
                self.notimplemented         += item.notimplemented
                self.skipped                += item.skipped
                self.architecturedependant  += item.architecturedependant
                self.amigaonly              += item.amigaonly
                self.total                  += item.total

