# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

def format( mirrors ):
    output  = '<form name="mirrors" action="http://aros.sourceforge.net/tools/redirect.php" method="get">'
    output += '<select name="url" onChange="mirrors.submit()">'

    for mirror in mirrors:
        output += '<option value="' + mirror[1] + '">' + mirror[0] + '</option>'

    output += '</select>'
    output += '<noscript><input type="submit" value="Go"></noscript>'
    output += '</form>'

    return output
