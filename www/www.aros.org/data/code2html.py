'''Support module which converts C code into HTML.'''

import string, re, sys

identifierPattern = re.compile (r'[_a-zA-Z][_a-zA-Z0-9]*')
ltPattern = re.compile (r'<')
gtPattern = re.compile (r'>')
ampPattern = re.compile (r'&')
commentBeginPattern = re.compile (r'(\/\*)')
commentEndPattern = re.compile (r'(\*\/)')
cppPattern = re.compile (r'\#*(?P<cpp>include|define|undef|ifdef|ifndef|if|else|endif)\b')
whiteSpace = re.compile (r'(\s+)')

class EOF: pass

def escape (s):
    s = ampPattern.sub ('&amp;', s)
    s = ltPattern.sub ('&lt;', s)
    s = gtPattern.sub ('&gt;', s)
    return s


class Token:
    def __init__ (self, text):
	self.text = text

    def dump (self, level):
	sys.stdout.write ('    '*level)
	print self.__class__.__name__

    def __str__ (self):
	return escape (self.text)

    def __repr__ (self):
	return self.__class__.__name__

class Identifier (Token):
    def __init__ (self, text, link=None):
	Token.__init__ (self, text)
	if link == 1:
	    link = None
	self.link = link

    def __str__ (self):
	if self.link:
	    return '<a href="%s">%s</a>' % (self.link, self.text)

	return self.text

spacePattern = re.compile (' ')
nlPattern = re.compile ('\n')

class WhiteSpace (Token):
    def __str__ (self):
	s = string.expandtabs (self.text)
	s = spacePattern.sub ('&nbsp;', s)
	s = nlPattern.sub ('<BR>\n', s)
	return s

class Constant (Identifier):
    def __str__ (self):
	if self.link:
	    return '<a href="%s">%s</a>' % (self.link, self.text)

	return '<font color="Magenta">%s</font>' % self.text

class Keyword (Identifier):
    def __str__ (self):
	if self.link:
	    return '<strong><a href="%s">%s</a></strong>' % (self.link, self.text)

	return '<strong><font color="Brown">%s</font></strong>' % self.text

class Type (Identifier):
    def __str__ (self):
	if self.link:
	    return '<strong><a href="%s">%s</a></strong>' % (self.link, self.text)
	return '<strong><font color="DarkGreen">%s</font></strong>' % self.text

class Include (Token):
    def __init__ (self, text, delim, file, link):
	self.text, self.delim, self.file, self.link = \
		text, delim, file, link
	    
    def __str__ (self):
	result = '<strong><font color="Purple">'
	result = result + str (WhiteSpace (escape (self.text)))
	if self.delim:
	    result = result + str (WhiteSpace (escape (self.delim)))
	if self.link:
	    result = result + '<a href="%s">%s</a>' % (self.link, self.file)
	else:
	    result = result + self.file
	if self.delim == '<':
	    result = result + '&gt;'
	elif self.delim:
	    result = result + self.delim
	return result + '</font></strong>'
	
class CPP (Token):
    def __init__ (self, cmd, text, list=()):
	Token.__init__ (self, text)
	self.list = list

    def __str__ (self):
	result = '<font color="Purple">'
	
	s = escape (self.text)
	result = result + str (WhiteSpace (s))
	for item in self.list:
	    if isinstance (item, Token):
		result = result + str (item)
	    else:
		s = escape (item)
		result = result + str (WhiteSpace (s))
	return result + '</font>'

class Comment (Token):
    def __str__ (self):
	s = escape (self.text)
	s = str (WhiteSpace (s))
	return '<font color="DarkBlue">%s</font>' % s

include1Pattern = re.compile (r'<(?P<file>[^>]*)>')
include2Pattern = re.compile (r'"(?P<file>[^"]*)"')
include3Pattern = re.compile (r'(?P<file>\S*)')
uncontinuedPattern = re.compile ('[^\\\\]\n')

class Reader:
    def __init__ (self, s, conv):
	self.string, self.pos = s, 0
	self.converter = conv
	
    def readCpp (self, match):
	# if(n?def) cond...
	# define cmd
	# define cmd(args,...)
	# define cmd(args,...) ...
	# undef arg
	# include file

	cmd = match.group ('cpp')
	text = match.group ()
	if cmd in ('undef', 'ifdef', 'ifndef'):
	    self.pos = match.end ()
	    list = []
	    while 1:
		token = self.read ()
		if token == EOF:
		    break
		list.append (token)
		
		if isinstance (token, Identifier):
		    break

	    return CPP (cmd, text, list)
	    
	if cmd == 'include':
	    match = whiteSpace.match (self.string, match.end ())
	    #print 'match:',`match.group ()`
	    text = text + match.group ()
	    ws = match.group ()
	    pos = match.end ()
	    if self.string[pos] == '<':
		delim = '<'
		match = include1Pattern.match (self.string, pos)
	    elif self.string[pos] == '"':
		delim = '"'
		match = include2Pattern.match (self.string, pos)
	    else:
		delim = None
		match = include3Pattern.match (self.string, pos)

	    file = match.group ('file')
	    self.pos = match.end ()
	    #print delim,'file:',`file`
	    link = self.converter.findInclude (file)
	    return Include (text, delim, file, link)
	
	# Read until we encounter a line that doesn't end with '\'
	pos = match.end ()
	while 1:
	    pos = string.find (self.string, '\n', pos)
	    if pos == -1:
		pos = len (self.string)
		break
	    if self.string[pos-1] != '\\':
		break
	    pos = pos + 1
	text = self.string[self.pos:pos]
	#print cmd, `text`
	cpp = CPP (cmd, text)
	self.pos = pos
	return cpp

    def readIdentifier (self, match):
	text = match.group ()
	#print 'readIdentifier',`text`
	
	link = self.converter.findType (text)
	if link:
	    return Type (text, link)

	link = self.converter.findConstant (text)
	if link:
	    return Constant (text, link)

	link = self.converter.findKeyword (text)
	if link:
	    return Keyword (text, link)

	link = self.converter.findIdentifier (text)
	if link:
	    return Identifier (text, link)
	
	return Identifier (text)

    def read (self):
	#print 'read:',`self.string[self.pos:self.pos+10]`
	if self.pos >= len (self.string):
	    return EOF

	match = whiteSpace.match (self.string, self.pos)
	if match:
	    self.pos = match.end ()
	    return WhiteSpace (match.group ())
	
	if self.string[self.pos:self.pos+2] == '/*':
	    match = commentEndPattern.search (self.string, self.pos)
	    if not match:
		pos = len (self.string)
	    else:
		pos = match.end ()
	    
	    text = self.string[self.pos:pos]
	    self.pos = pos
	    return Comment (text)
	
	if self.string[self.pos:self.pos+2] == '//':
	    pos = string.find (self.string, '\n', self.pos)
	    text = self.string[self.pos:pos]
	    self.pos = pos
	    return Comment (text)

	match = cppPattern.match (self.string, self.pos)
	if match:
	    return self.readCpp (match)

	match = identifierPattern.match (self.string, self.pos)
	if match:
	    self.pos = match.end ()
	    return self.readIdentifier (match)

	text = self.string[self.pos]
	self.pos = self.pos + 1
	return Token (text)

class Converter:
    def __init__ (self):
	self.stack = []
	self.includes = {}
	self.identifiers = {}

	self.keywords = {
	    'goto': 1,
	    'break': 1,
	    'return': 1,
	    'continue': 1,
	    'asm': 1,
	    'case': 1,
	    'default': 1,
	    'if': 1,
	    'else': 1,
	    'switch': 1,
	    'while': 1,
	    'for': 1,
	    'do': 1,
	}

	self.types = {
	    'int': 1,
	    'long': 1,
	    'short': 1,
	    'char': 1,
	    'void': 1,
	    'signed': 1,
	    'unsigned': 1,
	    'float': 1,
	    'double': 1,
	    'struct': 1,
	    'union': 1,
	    'enum': 1,
	    'typedef': 1,
	    'static': 1,
	    'register': 1,
	    'auto': 1,
	    'volatile': 1,
	    'extern': 1,
	    'const': 1,
	}

	self.constants = {
	    'NULL': 1,
	    'TRUE': 1,
	    'FALSE': 1,
	}

	self.push ()

    def push (self):
	self.stack.insert (0, (self.includes, self.identifiers, self.keywords,
	    self.types, self.constants))
	self.includes = {}
	self.identifiers = {}
	self.keywords = {}
	self.types = {}
	self.constants = {}

    def pop (self):
	self.includes, self.identifiers, self.keywords, \
	    self.types, self.constants = self.stack.pop (0)

    def searchStack (self, col, key):
	for row in self.stack:
	    link = row[col].get (key, None)
	    if link:
		return link
	
	return None

    def findInclude (self, key):
	return self.searchStack (0, key)
	
    def findIdentifier (self, key):
	return self.searchStack (1, key)
	
    def findKeyword (self, key):
	return self.searchStack (2, key)
	
    def findType (self, key):
	return self.searchStack (3, key)
	
    def findConstant (self, key):
	return self.searchStack (4, key)
	
    def addInclude (self, name, link):
	self.includes[name] = link

    def addType (self, name, link):
	self.types[name] = link

    def addConstant (self, name, link):
	self.constants[name] = link

    def addIdentifier (self, name, link):
	self.identifiers[name] = link

    def convert (self, s):
	self.push ()

	#print 'Convert:'
	#for row in self.stack:
	#    print row
	
	result = '<tt>'
	reader = Reader (s, self)
	while 1:
	    token = reader.read ()
	    #sys.stderr.write (`token`)
	    #sys.stderr.write (' ')
	    #sys.stderr.write (str (token))
	    #sys.stderr.write ('\n')

	    if token == EOF:
		break

	    result = result + str (token)
	
	self.pop ()
	return result + '</tt>'

class AROSCodeConverter (Converter):
    def __init__ (self, path):
	Converter.__init__ (self)

	# FIXME prepent path to all paths
	self.types = {
	    'UBYTE': 'include/exec/types.html#UBYTE',
	    'BYTE': 'include/exec/types.html#BYTE',
	    'UWORD': 'include/exec/types.html#UWORD',
	    'WORD': 'include/exec/types.html#WORD',
	    'ULONG': 'include/exec/types.html#ULONG',
	    'LONG': 'include/exec/types.html#LONG',
	    'APTR': 'include/exec/types.html#APTR',
	    'IPTR': 'include/exec/types.html#IPTR',
	    'List': 'include/exec/lists.html#List',
	    'Node': 'include/exec/lists.html#Node',
	}

	self.includes = {
	    'exec/types.h': 'include/exec/types.html',
	}
	
	self.identifiers = {
	    'FindName': 'autodocs/exec_findname.html',
	}
	
	self.push ()

def convert (s):
    c = AROSCodeConverter ('.')
    return c.convert (s)

if __name__ == '__main__':
    c = AROSCodeConverter ('.')
    c.addType ('Node', 'types/node.html')
    c.addConstant ('DEMOCONST', 'const/democonst.html')
    c.addIdentifier ('main', 'idents/main.html')
    c.addInclude ('with/link.h', 'include/with/link.html')
    c.push ()
    c.addIdentifier ('a', 'test')
    print c.convert ('''
/* Das ist ein test. */

int a;

/* Test includes */
#include <some/file.h>
#include <with/link.h>
#include "with/link.h"
#include "without/link.h"
#include DEFINE

/* test defines */
#define a
#define a   1
#define a(x)   1
#define a(x)   x
#define a(x)   \\
    b \\
    c \\
    x
/* Test if(n)def */
#ifdef xxx
#endif /* xxx */
#ifndef xxx
#endif /* xxx */
#undef xxx

int main (int argc, char ** argv)
{
    int a = DEMOCONST;
    void * b = NULL;
    Node * node;
    a = TRUE;

    return 0;
}

''')
