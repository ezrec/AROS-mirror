# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

def format( credits ):
    result  = '=======\n' \
            + 'Credits\n' \
            + '======='
              
    for area in credits:
        result += '\n\n' + area[0] + '\n' + '=' * (len( area[0] ) + 3) + '\n\n' 
        
        for name in area[1]:
            result += '+ ' + name + '\n'

    return result
