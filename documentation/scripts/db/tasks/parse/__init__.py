# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

from db.tasks.model import *

def createCategories( file ):

    # Root category    
    categories = {}
    c = Category( 'everything', 'Everything', None, 0, None )
    categories[c.category] = c

    # Search for startup
    file.seek(0)   
    for line in file:
        if line.find( "CATEGORIES_BEGIN" ) != -1:
            break

    # Load categories
    for line in file:
        if line.find( "CATEGORIES_END" ) != -1:
            break

        words = line.strip().split( ';' )
        
        parentcategory      =   words[0]
        category            =   words[1]
        description         =   words[2]
        categorytype        =   int( words[3] )
        lastupdated         =   words[4]

        c = Category( category, description, parentcategory, categorytype, lastupdated )
        categories[c.category] = c

    # Link categories
    for key, value in categories.iteritems():
        if value.parentcategory != None:
            if value.parentcategory in categories:
                categories[value.parentcategory].append( value )
            else:
                categories['everything'].append( value )

    return categories

def createCategoryItems( file ):

    categoryitems = []

    # Search for startup
    file.seek(0)   
    for line in file:
        if line.find( "CATEGORY_ITEMS_BEGIN" ) != -1:
            break

    # Load category items
    for line in file:
        if line.find( "CATEGORY_ITEMS_END" ) != -1:
            break
    
        words = line.strip().split( ';' )
        
        description = words[1]
        category    = words[0]
        status      = int( words[4] )
        apiversion = int( words[3] )
        architecture = int( words[2] )

        
        categoryitems.append( CategoryItem( description, category, status, architecture, apiversion ) )

    return categoryitems

def parse( file ):

    # Create categories and category items
    categories = createCategories( file )
    categoryitems = createCategoryItems( file )

    # Assign category items to categories
    for categoryitem in categoryitems:

        category = categoryitem.parentcategory

        # Fallback for not existing categories            
        if category not in categories:
            category = 'everything'
            
        categories[category].append( categoryitem )            


    everything = categories[ 'everything' ]
    everything.recalculate()    
    everything.sort()
    
    return everything
