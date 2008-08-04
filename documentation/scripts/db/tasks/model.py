# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

class Sortable:
    def __init__( self, sortkey ):
        self.sortkey = sortkey

    def __lt__( self, other ): return self.sortkey <  other.sortkey 
    def __le__( self, other ): return self.sortkey <= other.sortkey 
    def __eq__( self, other ): return self.sortkey == other.sortkey 
    def __ne__( self, other ): return self.sortkey != other.sortkey 
    def __gt__( self, other ): return self.sortkey >  other.sortkey 
    def __ge__( self, other ): return self.sortkey >= other.sortkey 

class CategoryItem( Sortable ):
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
        Sortable.__init__( self, description )
        self.description            =   description
        self.parentcategory         =   parentcategory
        self.status                 =   status
        self.architecture           =   architecture
        self.apiversion             =   apiversion

class Comment( Sortable ):

    def __init__( self, comment, parentcategory ):
        Sortable.__init__( self, comment )
        self.comment        =   comment
        self.parentcategory =   parentcategory    

class Category( Sortable ):

    TYPE_General    =   0
    TYPE_AmigaOS    =   1
    TYPE_Extensions =   2

    def __init__( self, category, description, parentcategory, categorytype, lastupdated ):
        Sortable.__init__( self, category )
       
        self.category           =   category
        self.description        =   description
        self.parentcategory     =   parentcategory
        self.lastupdated        =   lastupdated
        self.categorytype       =   categorytype
        
        self.completed          = 0
        self.needssomework      = 0
        self.notimplemented     = 0
        self.skipped            = 0
        self.amigaonly          = 0
        
        self.total              = 0

        self.comments           = []
        self.categoryitems      = []
        self.subcategories      = []


    def sort( self ):
        for item in self.subcategories:
            item.sort()
        self.comments.sort()
        self.categoryitems.sort()       
        self.subcategories.sort()
    
    def recalculate( self ):
        for item in self.categoryitems:
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

        for item in self.subcategories:
            item.recalculate()
            
            self.completed              += item.completed
            self.needssomework          += item.needssomework
            self.notimplemented         += item.notimplemented
            self.skipped                += item.skipped
            self.amigaonly              += item.amigaonly
            self.total                  += item.total

