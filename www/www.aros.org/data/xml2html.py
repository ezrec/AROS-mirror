'''Routines to convert XML to HTML.'''

# Remember path because importing util will change it
import os
CWD = os.getcwd ()

import util
import xmlsupport, code2html, cStringIO, string, time

from util import TableLite, TR, TD, Heading, RawText, Page, arosRC, \
	Developers, cvsrootdir, exampleBorderColor, exampleBGColor, \
	DefinitionList, Href, Name, HR, BR, Para

def writeVerbatim (p, xmlfile, item):
    if isinstance (item, xmlsupport.Tag):
	# Give a hint if a tag might be unsupported
	if not item.name in ('p', 'ul', 'li', 'strong', 'b', 'i', 'a', 'img', 'tt', 'ol', 'br', 'table', 'tr', 'th', 'td'):
	    print item.name
	p.fh.write ('<%s' % item.name)
	for attr, value in item.attr.items ():
	    p.fh.write (' %s="%s"' % (attr, value))
	p.fh.write ('>')
	xmlfile.processRecursive (p, item.content)
	p.fh.write ('</%s>' % item.name)
    else:
	p.fh.write (item.text)

def addToc (p, level, item):
    '''level = 1 (chapter), 2 (section), 3 (subsection), etc.'''

    p.toccount[level-1] = p.toccount[level-1] + 1
    p.toccount = p.toccount[:level] + [0,0,0,0,0]
    label = ''
    for cnt in p.toccount[:level]:
	label = label + `cnt` + '.'
    
    # Strip last dot if not a chapter
    if level != 1:
	label = label[:-1]
    
    title = label + ' ' + item.attr['title']
    #print title
    
    p.fh.write (str (Name (label)) + '\n')
    p.fh.write (str (Heading (level, RawText (title)))+'\n')
    mtime = item.attr.get ('mtime', '')
    new = item.attr.get ('new', '')
    if mtime:
	new = time.strftime ('%d.%m.%Y', time.localtime (time.time ()))
    if new:
	p.fh.write ('<font size="-1">Last Update: %s</font><br>\n\n' % new)

	new = ' (%s)' % new

    p.toc.append (Href ('#%s' % label, RawText (title + new)))
    p.toc.append (BR ())

def chapterToHtml (p, xmlfile, item):
    addToc (p, 1, item)
    xmlfile.processRecursive (p, item.content)

def sectionToHtml (p, xmlfile, item):
    addToc (p, 2, item)
    xmlfile.processRecursive (p, item.content)

def subsectionToHtml (p, xmlfile, item):
    addToc (p, 3, item)
    xmlfile.processRecursive (p, item.content)

def emailToHtml (p, xmlfile, item):
    email = item.content[0].text
    p.fh.write ('<A HREF="mailto:%s">%s</A>' % (email, email))
    subject = item.attr.get ('subject', None)
    if subject:
	p.fh.write (' with the subject "%s"' % subject)
    body = item.attr.get ('body', None)
    if body:
	if subject:
	    p.fh.write (' and')
	p.fh.write ('"%s" in the body' % body)

def exampleToHtml (p, xmlfile, item):
    outerTable = TableLite (
	border="0",
	width="804",
	#align="left",
	bgcolor=exampleBorderColor,
	cellpadding=2,
	cellspacing=0,
    )
    row = TR ()
    outerTable.append (row)
    td = TD ()
    row = row + [td]
    table = TableLite (
	border="0",
	width="800",
	#align="left",
	bgcolor=exampleBGColor,
	cellpadding=2,
	cellspacing=0,
    )
    td = td + [table]
    oldFH = p.fh
    for line in item.content:
	if not isinstance (line, xmlsupport.Tag) or line.name != 'line':
	    continue
	
	row = TR ()
	table.append (row)
	td = TD ()
	row = row + [td]

	if line.content:
	    p.fh = cStringIO.StringIO ()
	    xmlfile.processRecursive (p, line.content)
	    td = td + [RawText (p.fh.getvalue ())]
	    p.fh.close ()
	else:
	    td = td + [RawText ('&nbsp;')]
    p.fh = oldFH
    p.fh.write (str (outerTable))

def userToHtml (p, xmlfile, item):
    p.fh.write ('<strong><i>')
    xmlfile.processRecursive (p, item.content)
    p.fh.write ('</i></strong>')
    
def promptToHtml (p, xmlfile, item):
    p.fh.write ('&gt;\xA0')
    
def shellToHtml (p, xmlfile, item):
    p.fh.write ('<tt>')
    xmlfile.processRecursive (p, item.content)
    p.fh.write ('</tt>')
    
def codeToHtml (p, xmlfile, item):
    p.fh.write ('<pre>')
    xmlfile.processRecursive (p, item.content)
    p.fh.write ('</pre>')
    
def preToHtml (p, xmlfile, item):
    p.fh.write ('<pre>')
    xmlfile.processRecursive (p, item.content)
    p.fh.write ('</pre>')
    
def filenameToHtml (p, xmlfile, item):
    p.fh.write ('<tt>')
    xmlfile.processRecursive (p, item.content)
    p.fh.write ('</tt>')

def classToHtml (p, xmlfile, item):
    p.fh.write ('<strong><tt>')
    xmlfile.processRecursive (p, item.content)
    p.fh.write ('</tt></strong>')

def ideaToHtml (p, xmlfile, item):
    addToc (p, 2, item)
    p.fh.write ('\n<p><strong>By %s</strong></p>\n\n' % item.attr['by'])
    xmlfile.processRecursive (p, item.content)

def commentToHtml (p, xmlfile, item):
    p.fh.write (str (Heading (3, 'Comment by %s' % item.attr['by'])))
    xmlfile.processRecursive (p, item.content)

def codeToHtml (p, xmlfile, item):
    c = code2html.AROSCodeConverter ('.')
    p.fh.write (c.convert (item.content[0].text))

def descriptionToHtml (p, xmlfile, item):
    p.fh.write ('<p><dl>\n')
    for line in item.content:
	if not isinstance (line, xmlsupport.Tag) or line.name != 'li':
	    continue
	
	i = 0
	while i < len (line.content):
	    itemTitle = line.content[i]
	    if not isinstance (itemTitle, xmlsupport.Text):
		break
	    i = i + 1
	
	if not isinstance (itemTitle, xmlsupport.Tag) or itemTitle.name != 'item':
	    line.dump (0)
	    raise 'Expected <item>, got ' + `itemTitle`
	
	p.fh.write ('<dt><strong>')
	xmlfile.processRecursive (p, itemTitle.content)
	p.fh.write ('</strong></dt>\n<dd>')
	xmlfile.processRecursive (p, line.content[i+1:])
	p.fh.write ('<dd>\n')

    p.fh.write ('</dl></p>\n')

extraInfo = {
}

def insertHtml (p, xmlfile, item):
    text = item.content[0].text

    if text == 'devlist':
	developers = Developers (cvsrootdir)
	p.fh.write ('<ul>\n')
	for dev in developers.developers:
	    p.fh.write ('<li>%s</li>\n' % (dev.name))
	p.fh.write ('</ul>')
    else:
	print 'Inserting',item.content[0].text
	p.fh.write (p.extraInfo[item.content[0].text])


def recursiveTreeItemToHtml (p, xmlfile, item, level, hasSuccessor):
    p.fh.write ('<TABLE BORDER="0" CELLPADDING="0" CELLSPACING="0"><TR>')
    for l in level:
	l = (l != ' ')
	p.fh.write ('<TD WIDTH="16" ALIGN="CENTER"><IMG SRC="pics/parent%d.png" ALT="%s"></TD>' % (
	    l, ' |'[l]
	))
    subtree, descr = None, None
    for child in item.content:
	if isinstance (child, xmlsupport.Tag):
	    if child.name == 'tree':
		assert subtree == None
		subtree = child
	    elif child.name == 'description':
		assert descr == None
		descr = child
	    else:
		raise 'Element %s not allowed in tree item' % child.name

    p.fh.write ('<TD WIDTH="16" ALIGN="CENTER"><IMG SRC="pics/child%d%d.png" ALT="%s"</TD>' % (
	(subtree != None), (hasSuccessor != 0), '+'
    ))
    p.fh.write ('<TD>%s</TD>\n' % item.attr['title'])
    if descr:
	p.fh.write ('<TD>')
	xmlfile.processRecursive (p, descr.content)
	p.fh.write ('</TD>\n')
    p.fh.write ('</TABLE>\n')
    if subtree:
	if hasSuccessor:
	    level = level + '|'
	else:
	    level = level + ' '
	recursiveTreeToHtml (p, xmlfile, subtree, level)

def recursiveTreeToHtml (p, xmlfile, item, level):
    children = []
    for child in item.content:
	if isinstance (child, xmlsupport.Tag):
	    if child.name == 'item':
		children.append (child)
	    else:
		raise 'Element %s not allowed in tree' % child.name
    
    if children:
	for child in children[:-1]:
	    recursiveTreeItemToHtml (p, xmlfile, child, level, 1)
	recursiveTreeItemToHtml (p, xmlfile, children[-1], level, 0)

def treeToHtml (p, xmlfile, item):
    '''The structure of a tree is 
	<tree><item title="...">...</item>...</tree>
	where every <tree> element can contain any number of <item>
	elements and an item can contain exactly one <tree> and one
	<description> element.
    '''
    recursiveTreeToHtml (p, xmlfile, item, '')

def adocInputToHtml (p, xmlfile, item):
    '''<input><parameter name="...">description</parameter>...</input>'''
    
    dl = DefinitionList ()
    
    for child in item.content:
	if isinstance (child, xmlsupport.Tag):
	    if child.name != 'parameter':
		raise 'Unexpected element <%s> in <input> (%s)' % (child.name, `item`)
	    
	    name = child.attr['name']

	    p.fh = cStringIO.StringIO ()
	    xmlfile.processRecursive (p, child.content)
	    s = p.fh.getvalue ()
	    p.fh.close ()
	    dl.append ((name, s))

    return str (dl)

def adocSeeAlsoToHtml (p, xmlfile, item):
    '''<seealso><item>description</item>...</seealso>'''
    
    list = []
    
    for child in item.content:
	if isinstance (child, xmlsupport.Tag):
	    if child.name != 'item':
		raise 'Unexpected element <%s> in <seealso> (%s)' % (child.name, `item`)
	    
	    p.fh = cStringIO.StringIO ()
	    xmlfile.processRecursive (p, child.content)
	    s = p.fh.getvalue ()
	    p.fh.close ()
	    list.append (s)

    return string.join (list, ', ')

def adocAutodocToHtml (p, xmlfile, dict, key):
    name = string.upper (key)
    item = dict[key]
    if key == 'input':
	s = adocInputToHtml (p, xmlfile, item)
    elif key == 'seealso':
	s = adocSeeAlsoToHtml (p, xmlfile, item)
    else:
	list = item.content
	if not isinstance (list[0], xmlsupport.Tag) or \
	    list[0].name != 'p':
	    par = xmlsupport.Tag ('p')
	    par.setContents (list)
	    list = [par]

	p.fh = cStringIO.StringIO ()
	xmlfile.processRecursive (p, item.content)
	s = p.fh.getvalue ()
	p.fh.close ()

    return name, s

def autodocToHtml (p, xmlfile, item):
    '''An autodoc element contains these children:

	name, function, input, result

    and these optional children:
    
	notes, bugs, seealso

    '''
    dict = {}

    for child in item.content:
	if isinstance (child, xmlsupport.Tag):
	    if dict.has_key (child.name):
		raise 'Found element %s twice (%s)' % (child.name, `child`)

	    dict[child.name] = child
    
    oldFH = p.fh
    dl = DefinitionList ()

    for key in ('name', 'function', 'input', 'result'):
	if not dict.has_key (key):
	    raise '<autodoc> element is missing the mandatory element <%s> (%s)' % (key, `item`)
	
	dl.append ((adocAutodocToHtml (p, xmlfile, dict, key)))

    for key in ('notes', 'bugs', 'seealso'):
	if not dict.has_key (key):
	    continue
	
	dl.append ((adocAutodocToHtml (p, xmlfile, dict, key)))

    p.fh = oldFH
    p.fh.write (str (dl))

class Xml2HtmlProcessor (xmlsupport.Processor):
    def __init__ (self, **kw):
	xmlsupport.Processor.__init__ (self,
	    chapter=chapterToHtml,
	    section=sectionToHtml,
	    subsection=subsectionToHtml,
	    email=emailToHtml,
	    insert=insertHtml,
	    example=exampleToHtml,
	    user=userToHtml,
	    shell=shellToHtml,
	    filename=filenameToHtml,
	    description=descriptionToHtml,
	    tree=treeToHtml,
	    code=codeToHtml,
	    pre=preToHtml,
	    idea=ideaToHtml,
	    comment=commentToHtml,
	    autodoc=autodocToHtml,
	    prompt=promptToHtml,
	)
	self.add ('class', classToHtml)
	self.processors.update (kw)
	self.setDefault (writeVerbatim)
	self.extraInfo = extraInfo
        self.fh = cStringIO.StringIO ()

class XmlPage (Page):
    def __init__ (self, xmlfilename, rcFile=arosRC, linkBoxItem='', **kw):
	apply (Page.__init__, (self, rcFile, linkBoxItem,), kw)

	xmlfile = xmlsupport.AROSXmlFile ()

	XML2HTML = Xml2HtmlProcessor ()
	XML2HTML.toccount = [0, 0, 0, 0, 0]
	XML2HTML.toc = []

	xmlfile.parse (xmlfilename)
	xmlfile.process (XML2HTML)
	if XML2HTML.toc:
	    self.meat = self.meat + XML2HTML.toc + [BR(), HR (), BR()]
	self.meat = self.meat + [RawText (XML2HTML.fh.getvalue ())]
	XML2HTML.fh.close ()

	self.mtime = xmlfile.mtime

def elementToHtml (element):
    result = []

    xmlfile = xmlsupport.AROSXmlFile ()

    XML2HTML = Xml2HtmlProcessor ()
    XML2HTML.toccount = [0, 0, 0, 0, 0]
    XML2HTML.toc = []

    if type (element) != type ([]) and type (element) != type (()):
	element = [element]
    xmlfile.tree = element
    xmlfile.process (XML2HTML)
    if XML2HTML.toc:
	result = result + XML2HTML.toc + [BR(), HR (), BR()]
	
    result = result + [RawText (XML2HTML.fh.getvalue ())]
    XML2HTML.fh.close ()

    return result

def xmlStringToHtmlString (s):
    xmlfile = xmlsupport.AROSXmlFile ()

    XML2HTML = Xml2HtmlProcessor ()
    XML2HTML.toccount = [0, 0, 0, 0, 0]
    XML2HTML.toc = []

    xmlfile.parseString (s)
    xmlfile.process (XML2HTML)
    # TODO There is no support for toc :-/
	
    s = XML2HTML.fh.getvalue ()
    XML2HTML.fh.close ()

    return s

if __name__ == '__main__':
    import sys

    xf = XmlPage (os.path.join (CWD, sys.argv[1]))
    print 'Writing to ',os.path.join (os.getcwd(), sys.argv[2])
    xf.write (sys.argv[2])
