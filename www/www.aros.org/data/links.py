'''Small Python script to define external links.'''

from HTMLgen import *

class Link:
    '''Helper class to store all relevant information about a link.
    
    url - The URL of the link.
    label - The label which should be used in the HTML page.
	    If not specified, then url is used.
    text - Text explaining the link
    logo - Optional logo for this URL.
    '''
    def __init__ (self, url, label=None, text=None, logo=None):
	if not label:
	    label = url

	self.href = Href (url, label)
	self.text, self.logo = text, logo

# More related to the AROS project
moreRelated = (
    Link ('ftp://ftp.aros.org/pub/aros/', 'The official AROS FTP server',
	'You can find releases, nightly snapshots and other'
	' stuff on this server.',
    ),
    Link ('ftp://ftp.aros.org/pub/aros/snapshots/', 
	'Snapshots',
	'Here, you find daily snapshots of AROS sources and binaries'
	' incl. build logs.',
    ),
    ( Link ('/mailman/listinfo/',
	'Mailing lists',
	'There are a couple of mailing lists for AROS. Follow the links'
	' to subscribe. Note that you must be member of the lists to be'
	' able to post.',
	),
	Link ('/mailman/listinfo/aros-user/',
	    'AROS User',
	    'This list is for users of AROS. Since there are not many'
	    ' users of AROS at this time, the volume on this list is'
	    ' very low. :-)',
	),
	Link ('/mailman/listinfo/aros-announce/',
	    'AROS Announce',
	    'Subscribe to this list to be notified about all important'
	    ' news regarding AROS. The things posted here are the same'
	    ' as in the NEWS section of this site. Posting on this list'
	    ' is disabled.',
	),
	Link ('/mailman/listinfo/aros-dev/',
	    'AROS Dev',
	    'The developer mailing list. Subscribe here if you want to'
	    ' join the development team. The volume is usually a couple'
	    ' of mails per day. Note that subscription on this list'
	    ' is not automatic. Every subscription request will be'
	    ' verified by the list admin.',
	),
    ),
    Link ('/autodocs/index.html',
	'AutoDocs',
	'Amiga AutoDocs in HTML format (automatically generated from'
	' AROS sources.',
    ),
    Link ('ftp://ftp.aros.org/pub/aros/crypt.c',
	'Source for the crypt tool',
	(   'You need this tool to create a password for CVS. If you'
	    ' trust us, here is a ',
	    Href ('ftp://ftp.aros.org/pub/aros/AROS-crypt.lha', 
		'link to a binary for the Amiga'),
	    '. Otherwise, download the'
	    ' source and compile for yourself.',
	),
    ),
    Link ('http://www.xs4all.nl/~ldp/aros/index.html',
	'AROS for Amiga',
	'Here you can find a binary for the Amiga. You should not'
	' expect too much from these. These binaries are quite old'
	' and AROS behaves exactly like the AmigaOS, so you should'
	' not notice any difference when you install them.',
    ),
    Link ('http://www.freiburg.linux.de/~uae/',
	'UAE',
	'Here you can find the famous UAE which can run Amiga binaries'
	' (AROS can only run Amiga software for which you have the'
	' sources. Open Source rulez !).',
    ),
    Link ('http://olis.north.de/~indy/Speicherschutz-FAQ.html',
	'FAQ about memory protection (in german).',
	(   '(',
	    Href ('Speicherschutz-FAQ.html', 'Local copy'),
	    ')',
	)
    ),
    Link ('http://www.ninemoons.com/GG/',
	'GeekGadgets',
	'Tools to compile AROS on the Amiga.',
    ),
)

articles = (
    Link ('http://amiga.eden.it/dossier/aros/aros_eng.html',
	'Article by Amiga Life',
	(   'Also ',
	    Href ('http://amiga.eden.it/dossier/aros/aros.html', 'in italian'),
	    ', ',
	    Href ('http://amiga.eden.it/dossier/aros/aros_ger.html', 'in german'),
	    ' and ',
	    Href ('http://amiga.eden.it/dossier/aros/aros_jpn.html', 'in japanese'),
	    '.'
	),
    ),
    Link ('http://elwoodb.free.fr/AROS/',
	'Interview with A. Digulla (Head of Development)',
    ),
)

homepages = (
)

lessRelated = (
)

