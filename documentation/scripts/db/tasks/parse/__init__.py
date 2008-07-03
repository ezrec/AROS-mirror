# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

from db.tasks.model import *
from data import *

def parse( file ):
    categoryitems = {}
    size  = 0

    for line in file:
        words = line.strip().split( ';' )
        
        # TODO: Add support for architectures different than generic
        architecture = int(words[2])
        if architecture != CategoryItem.ARCH_Generic:
            continue

        id          = words[0] + words[1] + words[2] + words[3]
        description = words[1]
        category    = words[0]
        status      = int(words[4])
        apiversion = int(words[3])
        
        categoryitems[id] = CategoryItem( id, description, category, status, architecture, apiversion ) 
    

    cats = {}        
    
    while True:
        bubbled = False
    
        for categoryitem in categoryitems.copy().itervalues():
            if categoryitem.category:
                category = categoryitem.category
            
                if category not in cats:
                    supercategory = None
                    for key, value in categories.iteritems():
                        if category in value:
                            supercategory = key 
                
                    if abbreviations.has_key( category ):
                        longy = abbreviations[category]
                    else:
                        longy = category
                    
                    c = Category( category, longy, supercategory )
                    cats[c.id] = c
                    categoryitems[c.id] = c
                
                cats[category].append( categoryitem )
                del categoryitems[categoryitem.id]
                bubbled = True
            
        if not bubbled:
            break

    everything = categoryitems['everything']
    everything.recalculate()    
    everything.sort()
    
    return everything
