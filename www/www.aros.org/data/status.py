
import string, os
from util import Page, Heading, Paragraph, MyDataList, MyStackedBarChart, \
    Paragraph, Href, Image, lightBlue, darkBlue, WHITE, BR

def cmpNoCase (s1, s2):
    return cmp (string.lower (s1), string.lower (s2))

def gen ():
    '''Create the status page. This page contains two images (jpg&png)
    with graphs of the sizes of repositories.'''

    import jobs, time

    page = Page (linkBoxItem='Status')
    body = []
    body.append (Heading (2, 'Status'))
    body.append (Paragraph (
	'There is a total of %d functions of which %d (%5.2f%%) are finished,'
	' %d (%5.2f%%) are being worked on and %d (%5.2f%%) are still open.'
	' This status was generated %s' % (
	    jobs.jobsTotal, jobs.jobsDone, jobs.status[2],
	    jobs.jobsInWork, jobs.status[1],
	    jobs.jobsTodo, jobs.status[0],
	    time.strftime ('%d. %B %Y', time.localtime (time.time ()))
	)
    ))

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
    
    body.append (chart)
    body.append (BR ())
    body.append (Heading (2, 'Size of AROS sources'))

    body.append (Paragraph (Image ('aros_size.png')))
    body.append (Paragraph (Href ('aros_size.jpg', 'Also as JPEG')))
    body.append (Paragraph (Image ('aros_allsize.png')))
    body.append (Paragraph (Href ('aros_allsize.jpg', 'Also as JPEG')))

    body.append (Heading (2, 'Size of contributed sources'))

    body.append (Paragraph (Image ('contrib_size.png')))
    body.append (Paragraph (Href ('contrib_size.jpg', 'Also as JPEG')))
    body.append (Paragraph (Image ('contrib_allsize.png')))
    body.append (Paragraph (Href ('contrib_allsize.jpg', 'Also as JPEG')))

    page.meat = page.meat + body
    page.write ('status.html')

    #from docs.src.credits import credits
    from credits import credits
    page = Page (linkBoxItem='Credits')
    body = []
    body.append (Heading (2, 'Credits'))
    for area, names in credits:
	body.append (Heading (3, area))
	body.append (Paragraph (string.join (names, ', ')))
    page.meat = page.meat + body

    # Fix paths
    path = os.getcwd ()
    for color in chart.colors:
	colorFile = os.path.abspath (chart.barfiles[color])
	chart.barfiles[color] = page.relurl (path, colorFile)
	
    page.write ('credits.html')


