
from HTMLgen import Href

# This is a list of links which is used to generate the linkbox
# at the beginning of every page. The format is as follows:
#
#  ( 
#    ( main-page, sub-page-1, sub-page-2, ... ),
#    ( next-main-page, ... ),
#    ( ... ),
#  )
#
# All items in the lists must be objects of the class Href.
#
# Whenever a page is generated, you must pass the title of the main
# page (second parameter to Href()) to Page() in the parameter
# linkBoxItem. This will then "open" this item (ie. the subitems
# will be shown).
mainLinks = (
    ( Href ('index.html', 'NEWS'),
	Href ('status.html', 'Status'),
	Href ('credits.html', 'Credits'),
	Href ('oldnews.html', 'Old News'),
    ),
    ( Href ('documentation.html', 'Documentation'),
	Href ('background.html', 'Background'),
	Href ('faq.html', 'FAQ'),
	Href ('cvs.html', 'CVS'),
	Href ('mmake.html', 'MetaMake'),
	Href ('style.html', 'Coding Style'),
	Href ('filesys.html', 'Filesystems'),
	Href ('hidd-intro.html', 'HIDD Intro'),
	Href ('hidd-model.html', 'HIDD Model'),
	Href ('contents/contents.html', 'Contents'),
	Href ('autodocs/index.html', 'AutoDocs'),
	Href ('ideas.html', 'Random Ideas'),
    ),
    ( Href ('screenshots.html', 'Screenshots'),
	Href ('screenshots.html#Pictures around AROS', 'Pictures'),
	Href ('screenshots.html#Screenshots', 'Screenshots'),
    ),
    ( Href ('download.html', 'Download'),
	Href ('ftp://ftp.aros.org/pub/aros/', 'FTP Server'),
	Href ('ftp://ftp.aros.org/pub/aros/DiskImages/', 'Disk Images'),
	Href ('http://www.bloodline.freeserve.co.uk/aros/AROS-ibmpc-bin.zip', 'Matts DI'),
	Href ('http://wh2-315.st.uni-magdeburg.de/~sheutlin/aros/', 'Sebastians DIs'),
	Href ('snapshots.html', 'Snapshots'),
    ),
    ( Href ('links.html', 'Links'),
	Href ('links.html#More Related', 'More Related'),
	Href ('links.html#Articles', 'Articles'),
	Href ('links.html#Homepages', 'Homepages'),
	Href ('links.html#Less Related', 'Less Related'),
    ),
)

