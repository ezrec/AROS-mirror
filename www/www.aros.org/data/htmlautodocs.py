
import string, re, os
from util import PageMeat, arosdir, DefinitionList, Text, Href, BR, TT, Dummy, \
    Name, SeriesDocument, MyRawText, Heading, Paragraph, \
    NonBulletList, TableLite, TR, TD, relpath, RawText, \
    mainLinks2
import code2html, xml2html

codeConverter = code2html.AROSCodeConverter ('..')

class MyCode:
    def __init__ (self, text):
	self.text = text
	
    def __str__ (self):
	return codeConverter.convert (self.text)

autodocdir = 'autodocs'
includedir = 'includes'
sourcedir = 'sources'

includePattern = re.compile (r'\s*#\s*include\s*<([^>]+)>')

def getFilename (lib, func):
    return os.path.join (autodocdir,
	'%s_%s.html' % (
	    string.lower (lib.name),
	    string.lower (func.name),
	)
    )

class AutoDocPage (PageMeat):
    def __init__ (self, lib, func):
	PageMeat.__init__ (self, 'Documentation/AutoDocs/%s/%s' % (
		lib.longName, func.name,
	    ),
	    getFilename (lib, func)
	)

    def createLinkBox (self):
	#print 'createLinkBox',self
	linkBox = []
	node = mainLinks2.getNode (self)
	#print node.parent
	library = node.parent
	libraries = library.parent

	parentList = []
	current = libraries
	while current.parent:
	    parentList.insert (0, current)
	    current = current.parent
	
	indent = 0
	for node in parentList:
	    linkBox.extend (self.nodeToUrl (node, indent))
	    indent = indent + 1
	
	for lib in libraries.order:
	    linkBox.extend (self.nodeToUrl (lib, indent))

	    if lib == library:
		for func in lib.order:
		    linkBox.extend (self.nodeToUrl (func, indent+1))

	return linkBox

class AutoDocItem:
    def __init__ (self, contents):
	self.contents = contents

    def __str__ (self):
	if type (self.contents) == type ([]):
	    text = ''
	    for item in self.contents:
		text = text + str (item)
	elif type (self.contents) == type (''):
	    text = str (MyRawText (string.strip (self.contents)))
	else:
	    text = str (self.contents)
	
	text = text + '\n<P>\n'
	return text

class AutoDocTextItem:
    def __init__ (self, contents):
	self.contents = contents

    def __str__ (self):
	return xml2html.xmlStringToHtmlString (self.contents)

def makelibcell (func, absolute=os.path.abspath(autodocdir)):
    return TD (
	Href (relpath (absolute, os.path.abspath (func.url)),
	    func.name
	),
	width="33%",
	align="LEFT",
    )

def processFuncList (funcs, meat):
    table = TableLite (
	width="100%",
	border="0",
	cellspacing="0",
	cellpadding="0",
    )
    meat.append (table)

    i, n = 0, len (funcs)
    while i < n:
    
	left = funcs[i]
	middle = right = None
	if i+1 < n:
	    middle = funcs[i+1]
	if i+2 < n:
	    right = funcs[i+2]
	i = i + 3
	
	list = [makelibcell (left)]
	if middle:
	    list.append (makelibcell (middle))
	if right:
	    list.append (makelibcell (right))

	row = TR ()
	row = row + list
	table.append (row)
    
def genPage (db, lib, func):
    codeConverter.push ()
    try:
	page = AutoDocPage (lib, func)

	page.title = 'AROS - %s/%s' % (lib.longName, func.name)

	dl = DefinitionList ()
	page.append (dl)

	list = []
	hasIncludes = 0
	for isLocal, line in func.header:
	    #print isLocal, line
	    if isLocal:
		continue
	    
	    match = includePattern.match (line)
	    if match:
		include = match.group (1)
		text = MyCode ('#include <' + include + '>\n')
		#list.append (Text ('#include <'))
		#href = Href (
		#    os.path.join ('include', include),
		#    include
		#)
		#page.linksToFix.append (href)
		#list.append (href)
		#list.append (Text ('>'))
		#list.append (BR ())
		list.append (text)
		hasIncludes = 1
	
	if hasIncludes:
	    list.append (BR ())
	
	#list.append (TT ('%s %s ()' % (func.result.type, func.name)))
	#list.append (BR ())
	list.append (MyCode ('%s %s ()\n' % (func.result.type, func.name)))
	#list.append (Nbsp ())

	dl.append (('NAME', AutoDocItem (list)))

	for parameter in func.parameters:
	    codeConverter.addIdentifier (parameter.name, '#%s' % parameter.name)

	list = []
	for parameter in func.parameters:
	    #list.append (TT ('%s ' % parameter.type))
	    list.append (MyCode ('%s %s\n' % (parameter.type, parameter.name)))

	dl.append (('SYNOPSIS', AutoDocItem (list)))

	#print `func.section['FUNCTION']`
	#print func.section.keys ()
	dl.append (('FUNCTION', AutoDocTextItem (func.section['FUNCTION'])))
	parList = DefinitionList ()
	for parameter in func.parameters:
	    parList.append ((Dummy ([
		    Name (parameter.name),
		    parameter.name,
		]), RawText (xml2html.xmlStringToHtmlString (parameter.explanation))))
	dl.append (('INPUTS', AutoDocItem (parList)))
	dl.append (('RESULT', AutoDocTextItem (func.section['RESULT'])))
	dl.append (('NOTES', AutoDocTextItem (func.section['NOTES'])))
	dl.append (('EXAMPLE', AutoDocItem (MyCode (func.section['EXAMPLE']))))
	dl.append (('BUGS', AutoDocTextItem (func.section['BUGS'])))

	# Render SEE ALSO. The first item in the list is always
	# the library
	list = []
	href = Href (
	    os.path.join (autodocdir, 'index.html#%s' % lib.longName),
	    lib.longName
	)
	page.linksToFix.append (href)
	list.append (href)
	# Now render the rest
	for item in string.split (func.section['SEE ALSO']):
	    item = string.strip (item)
	    if item[0] == '@':
		dict = {}
		for f in db.seeAlso[item]:
		    if f != func:
			dict[f.name] = f
		flist = dict.keys ()
		flist.sort ()
		for key in flist:
		    f = dict[key]
		    href = Href (getFilename (f.lib, f),
			f.name+'()'
		    )
		    page.linksToFix.append (href)
		    list.append (Text(', '))
		    list.append (href)
	    else:
		list.append (Text (', '+item))
		
	#print list
	dl.append (('SEE ALSO', AutoDocItem (list)))
	
	dl.append (('INTERNALS', AutoDocTextItem (func.section['INTERNALS'])))
	#dl.append (('HISTORY', AutoDocItem (func.section['HISTORY'])))

	func.url = page.url
	page.write ()
    except:
	print 'Error in %s/%s' % (lib.name, func.name)
	raise
    codeConverter.pop ()

def gen ():
    if not os.path.exists (autodocdir):
	os.makedirs (autodocdir)
	
    import archtool

    archtool.setArosdir (arosdir)

    archives = archtool.findArchives (
	os.path.join (arosdir, 'rom'),
	os.path.join (arosdir, 'workbench'),
	os.path.join (arosdir, 'contrib'),
    )
    archives.insert (0, os.path.join (arosdir, 'rom/exec/lists.c'))

    adocs = archtool.Archive (archives)

    libs = adocs.libs.values ()

    #print adocs.seeAlso.keys ()

    # Create autodocs
    for lib in libs:
	#print lib
	for func in lib.functions.values ():
	    genPage (adocs, lib, func)

    page = PageMeat ('Documentation/AutoDocs', 
	os.path.join ('autodocs', 'index.html')
    )

    meat = []
    # --- Create table of libraries -----------------------------------------
    meat.append (Heading (1, 'Libraries sorted by Name'))

    list = []
    libs.sort ()
    libAnchors = {}
    for lib in libs:
	key = string.upper (lib.longName[0])
	if not libAnchors.has_key (key):
	    libAnchors[key] = Href ('#%s' % lib.longName, key)

    keys = libAnchors.keys ()
    keys.sort ()
    keylist = []
    for key in keys:
	keylist.append (libAnchors[key])
	keylist.append (Text (' '))
    meat.append (apply (Paragraph, keylist))

    # --- Create table of libraries -----------------------------------------
    meat.append (Name ('Functions sorted by Library'))
    meat.append (Heading (1, 'Functions sorted by Library'))

    for lib in libs:
	list.append (Href ('#%s' % lib.longName, lib.longName))

    meat.append (NonBulletList (list))

    allFunctions = []

    for lib in libs:
	meat.append (Name (lib.longName))
	meat.append (Heading (2, lib.longName))

	funcs = lib.functions.values ()
	funcs.sort ()
	allFunctions = allFunctions + funcs

	libNode = mainLinks2.findNode ('Documentation/AutoDocs/' + lib.longName)
	libNode.data = libNode.order[0].data

	processFuncList (funcs, meat)

    # --- Create table of functions -----------------------------------------
    meat.append (Heading (1, 'Functions sorted by Name'))
    
    allFunctions.sort ()
    funcAnchors = {}

    for func in allFunctions:
	key = string.upper (func.name[0])
	if not funcAnchors.has_key (key):
	    funcAnchors[key] = Href ('#%s' % func.name, Text (key))

    keys = funcAnchors.keys ()
    keys.sort ()
    keylist = []
    for key in keys:
	keylist.append (funcAnchors[key])
	keylist.append (Text (' '))
    meat.append (apply (Paragraph, keylist))

    key = ''
    list = []
    for func in allFunctions:
	if key != string.upper (func.name[0]):
	    if list:
		processFuncList (list, meat)
		list = []

	    key = string.upper (func.name[0])
	    meat.append (Name (func.name))
	    meat.append (Heading (2, key))
	
	list.append (func)

    if list:
	processFuncList (list, meat)

    page.extend (meat)
    page.write ()

