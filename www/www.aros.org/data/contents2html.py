
import contents, sys, os, os.path, string
import xml2html, xmlsupport
from util import arosdir, DefinitionList, Text, Href, Heading, \
    MyRawText, Paragraph

def list2html (list):
    s = ''
    for item in list:
	s = s + str (item)
	
    return MyRawText (s)

def convertParagraph (page, list):
    if not isinstance (list[0], xmlsupport.Tag) or \
	list[0].name != 'p':
	p = xmlsupport.Tag ('p')
	p.setContents (list)
	list = p
    page.extend (xml2html.elementToHtml (list))

def convertItem (Page, page, c, dirlist, item):
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
	    
	    convertContents (Page, child, dirlist+(dir,), item)

	list.append (Text (', '))

    for file in item.files:
	list.append (Text (file))
	list.append (Text (', '))

    if list:
	del list[-1]

    page.append (Heading (2, list2html (list)))

    list = item.description[:]
    if list:
	convertParagraph (page, list)
    else:
	page.append (Paragraph ('No description for this entry available.'))

def convertContents (Page, c, dirlist, parent):
    if dirlist:
	partPath = apply (os.path.join, dirlist)
    else:
	partPath = ''
    path = os.path.join ('contents', partPath)
    filename = os.path.join (path, 'contents.html')

    if not os.path.exists (path):
	os.makedirs (path)

    relRoot = ['..'] * len (dirlist)
    if relRoot:
	relPath = apply (os.path.join, relRoot)
	rootUrl = os.path.join (relPath, 'contents.html')
    else:
	relPath = '.'
	rootUrl = 'contents.html'
    
    nodeName = 'Documentation/Contents'
    if dirlist:
	nodeName = nodeName + '/' + string.join (dirlist, '/')
    page = Page (nodeName, filename)

    #print filename, relPath
    
    list = [Href (rootUrl, 'AROS'), Text (' / ')]
    path = relPath
    for dir in dirlist:
	path = os.path.dirname (path)
	list.append (Href (os.path.join (path, 'contents.html'), dir))
	list.append (Text (' / '))

    page.append (Heading (1, list2html (list)))

    if c.description:
	convertParagraph (page, c.description)
    elif parent:
	convertParagraph (page, parent.description)

    for item in c.items:
	convertItem (Page, page, c, dirlist, item)
    
    page.write ()
    
def gen (Page):
    c = contents.Contents (os.path.join (arosdir, 'contents.xml'))

    convertContents (Page, c, (), None)

if __name__ == '__main__':
    gen ()
