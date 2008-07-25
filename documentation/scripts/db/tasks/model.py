# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

class CategoryItem:
    STAT_Completed   = 2
    STAT_NeedsSomeWork = 1
    STAT_NotImplemented   = 0
    STAT_Skipped = 3
#    STAT_ArchitectureDependant = 4 - Removed, ARCH_i386 status is shown instead of ARCH_Generic/STAT_ArchitectureDependant
    STAT_AmigaOnly = 5

    ARCH_Generic = 1
    ARCH_i386 = 2

    API_Original = 1
    API_AROS = 2

    def __init__( self, description, parentcategory, status, architecture, apiversion ):
        self.description            =   description
        self.parentcategory         =   parentcategory
        self.status                 =   status
        self.architecture           =   architecture
        self.apiversion             =   apiversion
    
    def __lt__( self, other ): return self.description <  other.description 
    def __le__( self, other ): return self.description <= other.description 
    def __eq__( self, other ): return self.description == other.description 
    def __ne__( self, other ): return self.description != other.description 
    def __gt__( self, other ): return self.description >  other.description 
    def __ge__( self, other ): return self.description >= other.description 

class Category( list ):

    TYPE_General    =   0
    TYPE_AmigaOS    =   1
    TYPE_External   =   2

    def __init__( self, category, description, parentcategory, categorytype, lastupdated ):
        list.__init__( self )
        
        self.category           =   category
        self.description        =   description
        self.parentcategory     =   parentcategory
        self.lastupdated        =   lastupdated
        self.categorytype       =   categorytype
        
        self.completed                  = 0
        self.needssomework              = 0
        self.notimplemented             = 0
        self.skipped                    = 0
        self.amigaonly                  = 0
        
        self.total                      = 0

    def __lt__( self, other ): return self.category <  other.category 
    def __le__( self, other ): return self.category <= other.category 
    def __eq__( self, other ): return self.category == other.category 
    def __ne__( self, other ): return self.category != other.category 
    def __gt__( self, other ): return self.category >  other.category 
    def __ge__( self, other ): return self.category >= other.category 

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
                elif item.status == CategoryItem.STAT_AmigaOnly:
                    self.amigaonly              += 1

            else:
                item.recalculate()
                
                self.completed              += item.completed
                self.needssomework          += item.needssomework
                self.notimplemented         += item.notimplemented
                self.skipped                += item.skipped
                self.amigaonly              += item.amigaonly
                self.total                  += item.total

