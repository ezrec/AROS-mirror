'''Routines to convert XML to HTML.'''

import xmlsupport, code2html, cStringIO

from util import TableLite, TR, TD, Heading, RawText, Page, arosRC, \
	Developers, cvsrootdir, exampleBorderColor, exampleBGColor

def writeVerbatim (p, xmlfile, item):
    if isinstance (item, xmlsupport.Tag):
	# Give a hint if a tag might be unsupported
	if not item.name in ('p', 'ul', 'li', 'strong', 'i', 'a', 'img', 'tt', 'ol'):
	    print item.name
	p.fh.write ('<%s' % item.name)
	for attr, value in item.attr.items ():
	    p.fh.write (' %s="%s"' % (attr, value))
	p.fh.write ('>')
	xmlfile.processRecursive (p, item.content)
	p.fh.write ('</%s>' % item.name)
    else:
	p.fh.write (item.text)

def chapterToHtml (p, xmlfile, item):
    p.fh.write (str (Heading (1, item.attr['title'])))
    xmlfile.processRecursive (p, item.content)

def sectionToHtml (p, xmlfile, item):
    p.fh.write (str (Heading (2, item.attr['title'])))
    xmlfile.processRecursive (p, item.content)

def subsectionToHtml (p, xmlfile, item):
    p.fh.write (str (Heading (3, item.attr['title'])))
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
    
def shellToHtml (p, xmlfile, item):
    p.fh.write ('<tt>')
    xmlfile.processRecursive (p, item.content)
    p.fh.write ('</tt>')
    
def codeToHtml (p, xmlfile, item):
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
    p.fh.write (str (Heading (2, item.attr['title'])))
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
	    idea=ideaToHtml,
	    comment=commentToHtml,
	)
	self.add ('class', classToHtml)
	self.processors.update (kw)
	self.setDefault (writeVerbatim)
	self.extraInfo = extraInfo
        self.fh = cStringIO.StringIO ()

class XmlPage (Page):
    def __init__ (self, xmlfilename, rcFile=arosRC, linkBoxItem='', **kw):
	apply (Page.__init__, (self, rcFile, linkBoxItem,), kw)

	xmlfile = xmlsupport.XmlFile ()
	xmlfile.addEntity ('AROS', 'AROS')
	xmlfile.addEntity ('AMIGA', 'Amiga®')
	xmlfile.addEntity ('AMIGAOS', 'AmigaOS')

	XML2HTML = Xml2HtmlProcessor ()

	xmlfile.parse (xmlfilename)
	xmlfile.process (XML2HTML)
	self.meat = self.meat + [RawText (XML2HTML.fh.getvalue ())]
	XML2HTML.fh.close ()

def elementToHtml (element, page):
    xmlfile = xmlsupport.XmlFile ()

    xmlfile.addEntity ('AROS', 'AROS')
    xmlfile.addEntity ('AMIGA', 'Amiga®')
    xmlfile.addEntity ('AMIGAOS', 'AmigaOS')

    XML2HTML = Xml2HtmlProcessor ()

    if type (element) != type ([]) and type (element) != type (()):
	element = [element]
    xmlfile.tree = element
    xmlfile.process (XML2HTML)
    page.meat = page.meat + [RawText (XML2HTML.fh.getvalue ())]
    XML2HTML.fh.close ()

