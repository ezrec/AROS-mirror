
import string, re, os
from util import Page, arosdir, DefinitionList, Text, Href, BR, TT, Dummy, \
    Name, SeriesDocument, MyRawText, Heading, Paragraph, \
    NonBulletList, TableLite, TR, TD, relpath
import code2html

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

class AutoDocPage (Page):
    def __init__ (self, lib, func):
	Page.__init__ (self)
	
	self.filename = getFilename (lib, func)

#    def relurl (self, dest):
#	absdest = os.path.abspath (dest)
#	url = relpath (self.absname, absdest)
#	#print "AD::relurl(%s)->%s" % (dest, url)
#	return url

    def write (self):
	self.gotop = self.relurl (os.getcwd (),
	    os.path.join (autodocdir, 'index.html'))
	self.background = self.relurl (autodocdir, self.background)

	self.createNavBar ()

	for link in self.linksToFix:
	    #print link
	    link.url = self.relurl (autodocdir, link.url)
	    #print link

	for img in self.imagesToFix:
	    #print img
	    self.calc_rel_path (img, self.filename)
	    #print img

	SeriesDocument.write (self, self.filename)

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
	page.meat = page.meat + [dl]

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
	dl.append (('FUNCTION', AutoDocItem (func.section['FUNCTION'])))
	parList = DefinitionList ()
	for parameter in func.parameters:
	    parList.append ((Dummy ([
		    Name (parameter.name),
		    parameter.name,
		]), parameter.explanation))
	dl.append (('INPUTS', AutoDocItem (parList)))
	dl.append (('RESULT', AutoDocItem (func.section['RESULT'])))
	dl.append (('NOTES', AutoDocItem (func.section['NOTES'])))
	dl.append (('EXAMPLE', AutoDocItem (MyCode (func.section['EXAMPLE']))))
	dl.append (('BUGS', AutoDocItem (func.section['BUGS'])))

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
		for flib, f in db.seeAlso[item]:
		    if f != func:
			dict[f.name] = (flib, f)
		flist = dict.keys ()
		flist.sort ()
		for key in flist:
		    flib, f = dict[key]
		    href = Href (getFilename (flib, f),
			f.name+'()'
		    )
		    page.linksToFix.append (href)
		    list.append (Text(', '))
		    list.append (href)
	    else:
		list.append (Text (', '+item))
		
	#print list
	dl.append (('SEE ALSO', AutoDocItem (list)))
	
	dl.append (('INTERNALS', AutoDocItem (func.section['INTERNALS'])))
	#dl.append (('HISTORY', AutoDocItem (func.section['HISTORY'])))

	func.url = page.filename
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

    page = Page (linkBoxItem='AutoDocs')

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

    page.meat = page.meat + meat
    page.write (os.path.join ('autodocs', 'index.html'))

