# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

"""
    Synopsis
    --------

    credits = parse( file )

    Function
    --------

    Parses AROS/docs/src/credits into a list 'credits' of work areas and
    lists of their associated contributors. Ie, something like this:

    [ [ 'Area1', [ 'Name1', 'Name2' ]], [ 'Area2', [ 'Name3' ]] ]

    See below for an example on how to use it.

    Input
    -----

    file

    Output
    ------

    See above. :)

    Example
    -------

    # The following program will print the credits in the same
    # format as the input file is in.

    import sys, os.path
    sys.path.append (os.path.join (AROSDIR, 'docs', 'src'))

    from credits import credits

    for area, names in credits:
        print '\n' + area + ':'

        for name in names:
            print '    ' + name
"""

#FIXME: Write better documentation.

import os, sys, string

def parse( file ):
    credits = []
    names   = []

    for line in file:
        line = line.strip()

        if ':' in line:
	    if len( names ) > 0:
	        credits.append( [ area, names ] )

	    area = line[:-1]

	    names = []

        elif line != '':
	    names.append( line )

    if len( names ) > 0:
        credits.append( [ area, names ] )

    return credits
