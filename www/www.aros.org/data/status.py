
import string, os
from util import Page, Heading, Paragraph, MyDataList, MyStackedBarChart, \
    Paragraph, Href, Image, lightBlue, darkBlue, WHITE, BR, Name, RawText, \
    HR
import util

def cmpNoCase (s1, s2):
    return cmp (string.lower (s1), string.lower (s2))

def gen (Page):
    '''Create the status page. This page contains two images (jpg&png)
    with graphs of the sizes of repositories.'''

    import jobs, time

    page = Page ('NEWS/Status', 'status.html')
    page.append (
	Href ('#Status', '1. Status'),
	BR (),
	Href ('#SizeAROS', '2. Size of AROS sources'),
	BR (),
	Href ('#SizeContrib', '3. Size of contributed sources'),
	BR (), BR(), HR(), BR(),
	Name ('Status'),
	RawText ('\n'),
	Heading (2, '1. Status'),
	RawText ('\n'),
	Paragraph (
	    'There is a total of %d functions of which %d (%5.2f%%) are finished,'
	    ' %d (%5.2f%%) are being worked on and %d (%5.2f%%) are still open.'
	    ' This status was generated %s.\n' % (
		jobs.jobsTotal, jobs.jobsDone, jobs.status[2],
		jobs.jobsInWork, jobs.status[1],
		jobs.jobsTodo, jobs.status[0],
		time.strftime ('%d. %B %Y', time.localtime (time.time ()))
	    ),
	)
    )

    data = []
    data.append (('Total',
	jobs.jobsTotal,
	jobs.status[2], jobs.status[1], jobs.status[0])
    )

    keys = jobs.jobs.keys ()
    keys.sort (cmpNoCase)
    for key in keys:
	lib = jobs.jobs[key]
	data.append ((lib.longName,
	    lib.jobsTotal,
	    lib.status[2], lib.status[1], lib.status[0])
	)

    dataList = MyDataList ()
    dataList.segment_names = ('Done', 'In Work', 'Todo')
    dataList.load_tuples (data)
    #dataList.sort ()
    chart = MyStackedBarChart (dataList)
    chart.label_shade = lightBlue
    chart.value_shade = darkBlue
    chart.bar_shade = WHITE
    chart.title = 'AROS Status'
    
    page.append (chart)
    page.append (BR ())
    page.append (Name ('SizeAROS'))
    page.append (Heading (2, '2. Size of AROS sources'))

    page.append (Paragraph (Image ('aros_size.png')))
    page.append (Paragraph (Href ('aros_size.jpg', 'Also as JPEG')))
    page.append (Paragraph (Image ('aros_allsize.png')))
    page.append (Paragraph (Href ('aros_allsize.jpg', 'Also as JPEG')))

    page.append (Name ('SizeContrib'))
    page.append (Heading (2, 'Size of contributed sources'))

    page.append (Paragraph (Image ('contrib_size.png')))
    page.append (Paragraph (Href ('contrib_size.jpg', 'Also as JPEG')))
    page.append (Paragraph (Image ('contrib_allsize.png')))
    page.append (Paragraph (Href ('contrib_allsize.jpg', 'Also as JPEG')))

    page.write ()

    #from docs.src.credits import credits
    from credits import credits
    page = Page ('NEWS/Credits', 'credits.html')
    page.append (Heading (2, 'Credits'))
    for area, names in credits:
	page.append (
	    Heading (3, area),
	    Paragraph (string.join (names, ', '))
	)

    # Fix paths
    path = os.getcwd ()
    for color in chart.colors:
	colorFile = os.path.abspath (chart.barfiles[color])
	chart.barfiles[color] = util.relurl (path, colorFile)
	
    page.write ()


