# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

from db.tasks.model import *
from data import *

def parse( file ):
    tasks = {}
    size  = 0

    for line in file:
        words = line.strip().split( ';' )
    
        id          = words[0]
        description = words[3]
        category    = words[0][:-5]
        status      = int(words[1])
        developers  = words[2].split( ',' )
        
        tasks[id] = Task( id, description, category, status ) 
    

    cats = {}        
    
    while True:
        bubbled = False
    
        for task in tasks.copy().itervalues():
            if task.category:
                category = task.category
            
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
                    tasks[c.id] = c
                
                cats[category].append( task )
                del tasks[task.id]
                bubbled = True
            
        if not bubbled:
            break

    everything = tasks['everything']
    everything.recalculate()    
    everything.sort()
    
    return everything
