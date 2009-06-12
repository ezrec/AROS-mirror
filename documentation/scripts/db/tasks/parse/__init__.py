# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

from db.tasks.model import *

def createExtensionCategoryName( category ):
    index = category.rfind("_")
    if index == -1:
        return category + "_ext"
    else:
        return category[ 0:index ] + "_ext"


def createCategories( file ):

    # Root categories ( append in specif order, not sorted )
    categories = {}
    root = Category( 'everything', 'Everything', None, None )
    categories[root.category] = root
    c = Category( 'amigaos', 'AmigaOS 3.1', 'everything', None )
    categories[c.category] = c
    root.subcategories.append ( c )
    c = Category( 'aros', 'AROS', 'everything', None )
    categories[c.category] = c
    root.subcategories.append ( c )
    c = Category( '3rdparty', '3rd Party', 'everything', None )
    categories[c.category] = c
    root.subcategories.append ( c )
    c = Category( 'extensions', 'Extensions', 'everything', None )
    categories[c.category] = c
    root.subcategories.append ( c )

    # Search for startup
    file.seek(0)   
    for line in file:
        if line.find( "CATEGORIES_BEGIN" ) != -1:
            break

    # Load categories
    for line in file:
        if line.find( "CATEGORIES_END" ) != -1:
            break

        # Skip commented out lines
        if line[0] == '#':
            continue

        words = line.strip().split( ';' )
        
        parentcategory      =   words[0]
        category            =   words[1]
        description         =   words[2]
        lastupdated         =   words[3]

        # Force recalculation of last updated based on child categories
        if lastupdated == "AUTO":
            lastupdated = None

        c = Category( category, description, parentcategory, lastupdated )
        categories[c.category] = c

        # For each normal category, create an empty, virtual extension category
        category = createExtensionCategoryName( category )

        if not category in categories:
            # Override top-most extension category name
            if ( parentcategory == "amigaos" ) or ( parentcategory == "aros" ) or ( parentcategory == "3rdparty" ):
                parentcategory = "extensions"
            else:
                parentcategory = createExtensionCategoryName( parentcategory )

            c = Category( category, description, parentcategory, lastupdated )
            categories[c.category] = c

    # Link categories
    for key, value in categories.iteritems():
        if value.parentcategory != None and value.parentcategory != 'everything':
            if value.parentcategory in categories:
                categories[value.parentcategory].subcategories.append( value )
            else:
                categories['everything'].subcategories.append( value )

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

        # Skip commented out lines
        if line[0] == '#':
            continue
    
        words = line.strip().split( ';' )
        
        description = words[1]
        category    = words[0]
        status      = int( words[4] )
        apiversion = int( words[3] )
        architecture = int( words[2] )

        categoryitems.append( CategoryItem( description, category, status, architecture, apiversion ) )

    return categoryitems

def createComments( file ):

    comments = []

    # Search for startup
    file.seek(0)   
    for line in file:
        if line.find( "COMMENTS_BEGIN" ) != -1:
            break

    # Load comments
    for line in file:
        if line.find( "COMMENTS_END" ) != -1:
            break

        # Skip commented out lines
        if line[0] == '#':
            continue
    
        words = line.strip().split( ';' )
        
        comment = words[1]
        category    = words[0]
        
        comments.append( Comment( comment, category ) )

    return comments

def parse( file ):

    # Create categories, category items and comments
    categories = createCategories( file )
    categoryitems = createCategoryItems( file )
    comments = createComments( file )

    # Assign category items to categories
    for categoryitem in categoryitems:

        category = categoryitem.parentcategory

        # Fallback for not existing categories            
        if category not in categories:
            category = 'everything'
        
        # If item is an Extension, assign item to extension category
        if categoryitem.apiversion == CategoryItem.API_Extension:
            category = createExtensionCategoryName( category )

        categories[category].categoryitems.append( categoryitem )            

    # Assign comments to categories
    for comment in comments:

        category = comment.parentcategory

        # Fallback for not existing categories            
        if category not in categories:
            category = 'everything'
            
        categories[category].comments.append( comment )  


    everything = categories[ 'everything' ]
    # Remove empty categories
    everything.removeempty( )
    # Recalculate count values
    everything.recalculate( )  
    # Sort items (don't sort top most level)
    category = categories[ 'amigaos' ]
    if category is not None:
        category.sort ( )
    category = categories[ 'aros' ]
    if category is not None:
        category.sort ( )
    category = categories[ '3rdparty' ]
    if category is not None:
        category.sort ( )
    category = categories[ 'extensions' ]
    if category is not None:
        category.sort ( )
    
    return everything
