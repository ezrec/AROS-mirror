# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

# Not used. Just keep it here because I have an idea about usage... :)

class Borg:
    __hive_mind = {}

    def __init__( self ):
        self.__dict__ = self.__hive_mind


