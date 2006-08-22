# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

# The core framework classes
# ==========================

class Node:
    def __init__( self ):
        self.parent = None


class Container( Node, list ):
    def __init__( self, contents=None ):
        Node.__init__( self )
        list.__init__( self )

        if contents is None: return
        
        if contents.__class__ is list:
            for child in contents:
                if child.__class__ is str:
                    child = String( child )

                child.parent = self
                self.append( child )
        else:
            self.append( contents )

    def __str__( self ):
        result = ''
        for child in self:
            result += child.__str__()

        return result


class String( Node ):
    def __init__( self, value ):
        Node.__init__( self )
        self.value = value

    def __str__( self ):
        return self.value

class Standard( Container ):
    def __init__( self, name, attributes, contents=None, defaults=None ):
        Container.__init__( self, contents )

        self.name       = name
        self.attributes = attributes

        if self.attributes.has_key( 'CLASS' ):
            self.attributes['class'] = self.attributes['CLASS']
            del self.attributes['CLASS']
            
        if defaults:
            for key in defaults.keys():
                if key not in self.attributes: 
                    self.attributes[key] = defaults[key]

    def __str__( self ):
        result  = '<' + self.name
        for name in self.attributes.keys():
            result += ' ' + name + '="' + str( self.attributes[name] ) + '"'
        result += '>'
        result += Container.__str__( self )
        result += '</' + self.name + '>'

        return result

class StandardEmpty( Node ):
    def __init__( self, name, attributes, defaults=None ):
        Node.__init__( self )

        self.name       = name
        self.attributes = attributes

        if self.attributes.has_key( 'CLASS' ):
            self.attributes['class'] = self.attributes['CLASS']
            del self.attributes['CLASS']
        
        if defaults:
            for key in defaults.keys():
                if key not in self.attributes:
                    self.attributes[key] = defaults[key]
                    
    def __str__( self ):
        result  = '<' + self.name
        for name in self.attributes.keys():
            result += ' ' + name + '="' + str( self.attributes[name] ) + '"'
        result += '>'

        return result


# Document related classes
# ========================

class HTML( Standard ):
    def __init__( self, contents=None, **attributes ):
        Standard.__init__( self, 'html', attributes, contents )

    def __str__( self ):
        result  = '<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">'
        result += Standard.__str__( self )

        return result 

class Head( Standard ):
    def __init__( self, contents=None, **attributes ):
        Standard.__init__( self, 'head', attributes, contents )

class Title( Standard ):
    def __init__( self, contents=None, **attributes ):
        Standard.__init__( self, 'title', attributes, contents )

class Meta( StandardEmpty ):
    def __init__( self, **attributes ):
        StandardEmpty.__init__( self, 'meta', attributes )

class Charset( StandardEmpty ):
    def __init__( self, charset ):
        self.charset = charset

    def __str__( self ):
        result  = '<' + 'meta'
        result += ' http-equiv="Content-Type"'
        result += ' content="text/html; charset=' + self.charset + '">'

        return result

class Link( StandardEmpty ):
    def __init__( self, **attributes ):
        StandardEmpty.__init__( self, 'link', attributes )

class Body( Standard ):
    def __init__( self, contents=None, **attributes ):
        Standard.__init__( self, 'body', attributes, contents )


# Style related classes
# =====================

class B( Standard ):
    def __init__( self, contents=None, **attributes ):
        Standard.__init__( self, 'b', attributes, contents )

class I( Standard ):
    def __init__( self, contents=None, **attributes ):  
        Standard.__init__( self, 'i', attributes, contents )

class Font( Standard ):
    def __init__( self, contents=None, **attributes ):
        Standard.__init__( self, 'font', attributes, contents )


# Table related classes
# =====================

class Table( Standard ):
    def __init__( self, contents=None, **attributes ):
        Standard.__init__( 
            self, 'table', attributes, contents,
            { 'border' : 0, 'class' : 'layout' }
        )

class TR( Standard ):
    def __init__( self, contents=None, **attributes ):
        Standard.__init__( 
            self, 'tr', attributes, contents,
            { 'class' : 'layout' }
        )

class TD( Standard ):
    def __init__( self, contents=None, **attributes ):
        Standard.__init__(
            self, 'td', attributes, contents,
            { 'valign': 'top', 'class' : 'layout' }
        )


# Form related classes
# ====================

class Form( Standard ):
    def __init__( self, contents=None, **attributes ):
        Standard.__init__( self, 'form', attributes, contents )

class Input( Standard ):
    def __init__( self, contents=None, **attributes ):
        Standard.__init__( self, 'input', attributes, contents )


# Miscellaneous classes
# ====================

class A( Standard ):
    def __init__( self, contents=None, **attributes ):
        Standard.__init__( self, 'a', attributes, contents )

class Img( Standard ):
    def __init__( self, contents=None, **attributes ):
        Standard.__init__( self, 'img', attributes, contents )

class P( Standard ):
    def __init__( self, contents=None, **attributes ):
        Standard.__init__( self, 'p', attributes, contents )

class BR( Node ):
    def __str__( self ):
        return '<br>'
