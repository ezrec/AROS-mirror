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
    Link ('ftp://ftp.aros.org/pub/aros/DiskImages/', 
	'Disk Images',
	'Here, you can find current disk images for AROS Native/x86.'
    ),
    Link ('http://wh2-315.st.uni-magdeburg.de/~sheutlin/aros/',
	'Sebastians site with Disk Images',
	'The images here are mirrored on the AROS main site above.'
    ),
    Link ('http://www.bloodline.freeserve.co.uk/aros/index.html',
	"Matt 'Crazy' Parsons Disk Images",
	'The images on this page are usually tested on several different PCs,'
	' so chances that they work for you are much higher',
    ),
    ( Link ('http://www.aros.org/mailman/listinfo/',
	'Mailing lists',
	'There are a couple of mailing lists for AROS. Follow the links'
	' to subscribe. Note that you must be member of the lists to be'
	' able to post (to avoid spam and the like).',
	),
	Link ('http://www.aros.org/mailman/listinfo/aros-user/',
	    'AROS User',
	    'This list is for users of AROS. Since there are not many'
	    ' users of AROS at this time, the volume on this list is'
	    ' very low. :-)',
	),
	Link ('http://www.aros.org/mailman/listinfo/aros-announce/',
	    'AROS Announce',
	    'Subscribe to this list to be notified about all important'
	    ' news regarding AROS. The things posted here are the same'
	    ' as in the NEWS section of this site. Posting on this list'
	    ' is disabled.',
	),
	Link ('http://www.aros.org/mailman/listinfo/aros-dev/',
	    'AROS Dev',
	    'The developer mailing list. Subscribe here if you want to'
	    ' join the development team. The volume is usually a couple'
	    ' of mails per day. Note that subscription on this list'
	    ' is not automatic. Every subscription request will be'
	    ' verified by the list admin.',
	),
    ),
    Link ('autodocs/index.html',
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
    Link ('http://elwoodb.free.fr/articles/AROS/',
	'Interview with A. Digulla (Head of Development)',
    ),
)

homepages = (
    Link ('http://www.lake.de/home/lake/home/', 'Aaron "Optimizer" Digulla'),
    Link ('http://www.vmc.de/', 'Harald Frank (VMC)'),
    Link ('http://www.dtek.chalmers.se/~d95duvan/', 'Johan Alfredsson'),
    Link ('http://www.xs4all.nl/~ldp/', 'Lennard voor den Dag'),
    Link ('http://home.pages.de/~helios/', 'Martin Steigerwald'),
    Link ('http://www.bloodline.freeserve.co.uk/', "Matt 'Crazy' Parsons",),
    Link ('http://www.in-berlin.de/User/jroger/index.html', 'Sebastian Rittau'),
    Link ('http://www.twinklestar.demon.co.uk/', 'Wez Furlong'),
)

lessRelated = (
    Link ('http://www.trustsec.de/', 'trustsec',
	(   'These people provide us with web space. If you need Java'
	    'development or courses in Germany, contact them !',
	    BR(),
	    Href ('http://www.trustsec.de/', Image ('pics/trustsec-logo-slim-120dpi.png')),
	),
    ),
    Link ('http://www.amiga.com/', 'Amiga, Inc.'),
    Link ('http://www.ninemoons.com/GG/'
	'Geek Gadgets is an ongoing effort to port Unix tools to non-Unix'
	' systems led by the famous Fred Fish at',
	( Href ('http://www.ninemoons.com/', 'Cronus'), ),
    ),
    Link ('http://www.openamiga.org/', 'Open Amiga Foundation OAF'),
    Link ('http://www.satanicdreams.com/scalos/index.html', 'Official Scalos Site'),
    Link ('http://www.crosswinds.net/~osap/', 'Open Source Amiga Projects'),
    Link ('http://www.mentasm.com/~mramiga/dos1.shtml',
	'AmigaDOS Online Reference Manual.',
	(
	    'This site contains a *lot* of information for developers and users'
	    ' of the AmigaOS: Workbench, ARexx, system utilities and commands'
	    ' and lots more.',
	    BR(),
	    Href ('http://www.mentasm.com/~mramiga/dos1.shtml',
		Image ('pics/ados1.gif')
	    ),
	),
    ),
    Link ('http://www.amiga.org/', 'Amiga.org', '"Your Gateway to the Amiga"'),
    Link ('http://www.intercom.it/~fsoft/afc.html',
	'AFC - Amiga Foundation Classes',
	'A FreeWare and Public Domain library of Object Classes devoted to the'
	' software development on the Amiga and on future Amiga compatible'
	' machines.'
    ),
)

downloadLinks = (
    Link ('ftp://ftp.aros.org/pub/aros/', 'FTP Server',
	'The official main AROS FTP Server',
    ),
    Link ('ftp://ftp.aros.org/pub/aros/DiskImages/',
	'Disk images on our FTP server',
	'These images are copies of the ones made by Sebastian Heutling'
    ),
    Link ('http://www.bloodline.freeserve.co.uk/aros/index.html',
	'Disk Images by Matt Parsons',
	'Matt creates his own disk images and tries them on several'
	' computers before uploading them so they tend to work better'
	' than the official ones.'
    ),
    Link ('http://wh2-315.st.uni-magdeburg.de/~sheutlin/aros/',
	'Disk Images by Sebastian Heutling',
	'These are copied every night to the main site so please'
	' use the link above before draining his quota :-)'
    ),
)
