
import contents, sys, os, os.path
import xml2html, xmlsupport
from util import Page, arosdir, DefinitionList, Text, Href, Heading, \
    MyRawText, Paragraph

def list2html (list):
    s = ''
    for item in list:
	s = s + str (item)
	
    return MyRawText (s)

def convertItem (page, c, dirlist, item):
    list = ('contents',) + dirlist
    path = apply (os.path.join, list)

    list = []
    
    for dir in item.dirs:
	if type (dir) == type (''):
	    list.append (Text ('%s/' % dir))
	else:
	    dir, child = dir
	    list.append (Href (os.path.join (dir, 'contents.html'),
		'%s/' % dir))
	    
	    convertContents (child, dirlist+(dir,), c)

	list.append (Text (', '))

    for file in item.files:
	list.append (Text (file))
	list.append (Text (', '))

    if list:
	del list[-1]

    page.meat = page.meat + [Heading (2, list2html (list))]

    list = item.description[:]
    if list:
	if not isinstance (list[0], xmlsupport.Tag) or \
	    list[0].name != 'p':
	    p = xmlsupport.Tag ('p')
	    p.setContents (list)
	    list = p
	xml2html.elementToHtml (list, page)
    else:
	page.meat = page.meat + [Paragraph ('No description for this entry available.')]

def convertContents (c, dirlist, parent):
    list = ('contents',) + dirlist
    path = apply (os.path.join, list)
    filename = os.path.join (path, 'contents.html')

    if not os.path.exists (path):
	os.makedirs (path)

    page = Page (linkBoxItem='Contents')

    list = ['..'] * len (dirlist)
    if list:
	relPath = apply (os.path.join, list)
	rootUrl = os.path.join (relPath, 'contents.html')
    else:
	relPath = '.'
	rootUrl = 'contents.html'
    
    print filename, relPath
    
    list = [Href (rootUrl, 'AROS'), Text (' / ')]
    path = relPath
    for dir in dirlist:
	path = os.path.dirname (path)
	list.append (Href (os.path.join (path, 'contents.html'), dir))
	list.append (Text (' / '))

    page.meat = page.meat + [Heading (1, list2html (list))]

    if c.description:
	xml2html.elementToHtml (c.description, page)
    elif parent:
	xml2html.elementToHtml (parent.description, page)

    for item in c.items:
	convertItem (page, c, dirlist, item)
    
    page.write (filename)
    
def gen ():
    c = contents.Contents (os.path.join (arosdir, 'contents.xml'))

    convertContents (c, (), None)

