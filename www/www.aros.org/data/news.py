
import glob, os
from util import TR, TD, Page, TableLite, Paragraph, Name, MyRawText, Href, \
	newsColor, Center, Text, Image

def prepareNewsItem (filename):
    '''Convert a single news item into HTML.'''

    row = TR ()
    td = TD (bgcolor=newsColor, valign='TOP')
    row = row + [td]
    str = os.path.basename (filename)
    date = '%d.%d.%d' % (
	int (str[6:8]),
	int (str[4:6]),
	int (str[0:4]),
    )
    td = td + [Name (str), date]

    td = TD (valign='TOP')
    row = row + [td]

    fh = open (filename, 'r')
    body = fh.read ()
    fh.close ()

    td = td + [MyRawText (body)]
    
    return row
	    
def genPage (items, filename, linkBoxItem):
    '''Convert a list of news items into an HTML page and save
    it in filename. items must be a list of files with newsitems.
    The filenames must be dates in the form YYYYMMDD.'''

    page = Page (linkBoxItem=linkBoxItem)

    if filename == 'index.html':
	page.meat = page.meat + [
	    Paragraph (
		Text ('You are visitor no. '),
		Image (('/cgi-bin/wwwcount.cgi?df=aros.dat&dd=B&comma=Y&pad=Y&md=7', 94, 30),
		    alt='[picture of a counter]',
		),
		Text (' since 12. October 1999.'),
		align="center",
	    )
	]
	
    table = TableLite ()
    page.meat = page.meat + [table,]
    mtime = 0

    for file in items:
	item = prepareNewsItem (file)
	fmtime = os.path.getmtime (file)
	if mtime < fmtime
	    mtime = fmtime
	table.append (item)

    if filename == 'index.html':
	p = Paragraph ()
	p.append (Href ('oldnews.html', 'Older News'))
	page.meat = page.meat + [p]

    page.mtime = fmtime
    page.write (filename)

def gen (datadir):
    '''Create the news page (index.html, oldnews.html).'''

    # Search all known news items, and sort them in reverse order
    list = glob.glob (os.path.join (datadir, 'news', '*'))
    list.remove (os.path.join (datadir, 'news', 'CVS'))
    list.sort ()
    list.reverse ()
    
    # Create a main page with the Top 5 news items and another page
    # with the rest.
    genPage (list[:5], 'index.html', 'NEWS')
    genPage (list[5:], 'oldnews.html', 'Old News')

