# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

def parse( file ):
    mirrors = []
    
    for line in file:
        line = line.strip()

        if line != '':
            words = line.split( ';' )
            mirrors.append( words )

    return mirrors
