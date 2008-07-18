# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

from db.tasks.model import *
from data import *

def parse( file ):
    categoryitems = {}

    for line in file:
        words = line.strip().split( ';' )
        
        id          = words[0] + words[1] + words[3]
        description = words[1]
        category    = words[0]
        status      = int(words[4])
        apiversion = int(words[3])
        architecture = int(words[2])

        
        categoryitems[id] = CategoryItem( id, description, category, status, architecture, apiversion ) 
    

    # Create categories  
    cats = {}
    c = Category( 'everything', 'Everything', None )
    cats[c.id] = c

    #TODO: Load from file
    for key, value in categories.iteritems():
        for category in value:
            if abbreviations.has_key( category ):
                longy = abbreviations[ category ]
            else:
                longy = category
        
            c = Category( category, longy, key )
            cats[c.id] = c

    # Link categories
    for key, value in cats.iteritems():
        if value.category != None:
            if value.category in cats:
                cats[value.category].append( value )
            else:
                cats['everything'].append( value )

    # Assign category items to categories
    for categoryitem in categoryitems.copy().itervalues():

        category = categoryitem.category

        # Fallback for not existing categories            
        if category not in cats:
            category = 'everything'
            
        cats[category].append( categoryitem )
        del categoryitems[categoryitem.id]
            


    everything = cats['everything']
    everything.recalculate()    
    everything.sort()
    
    return everything
