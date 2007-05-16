# -*- coding: iso-8859-1 -*-
# Copyright © 2002-2006, The AROS Development Team. All rights reserved.
# $Id$

import os

from html import *
from components import *

def makePage( _T, _N, _M, MIRRORS_DATA, lang, charset ):
    navigation = Tree \
    ( [	
	P ( contents = [
	Img( src = '/images/pointer.png' ),
	A( _N['home'], href=makeURL( '', lang ))]
	),
        Tree \
        ( [
	    P ( contents = [
            Img( src = '/images/englishlogo.png' ),
            A( 'English', href='%(BASE)s' )]),
	    P ( contents = [
            Img( src = '/images/germanylogo.png' ),
            A( 'Deutsch', href='%(BASE)sde/' )]),
            P ( contents = [
            Img( src = '/images/francelogo.png' ),
            A( 'Fran&#231;ais', href='%(BASE)sfr/' )]),
            P ( contents = [
            Img( src = '/images/italylogo.png' ),
            A( 'Italiano', href='%(BASE)sit/' )]),
            P ( contents = [
            Img( src = '/images/netherlandslogo.png' ),
            A( 'Nederlands', href='%(BASE)snl/' )]),
	    P ( contents = [
            Img( src = '/images/polandlogo.png' ),
            A( 'Polski', href='%(BASE)spl/' )]),
            P ( contents = [
            Img( src = '/images/portugallogo.png' ),
            A( 'Portugu&#234;s', href='%(BASE)spt/' )]),
            P ( contents = [
            Img( src = '/images/russialogo.png' ),
            A( '&#1056;&#1091;&#1089;&#1089;&#1082;&#1080;&#1081;', href='%(BASE)sru/' )]),
	    P ( contents = [
            Img( src = '/images/finlandlogo.png' ),
            A( 'Suomi', href='%(BASE)sfi/' )]),
	    P ( contents = [
            Img( src = '/images/swedenlogo.png' ),
            A( 'Svenska', href='%(BASE)ssv/' )])
        ] ),
        BR(),
	P ( contents = [
        Img( src = '/images/pointer.png' ),	
        A( _N['news'], href=makeURL( 'news/', lang ) )]),
        Tree ( A( _N['archive'], href=makeURL( 'news/archive/', lang ) ) ),
        BR(),
	P ( contents = [
        Img( src = '/images/pointer.png' ),
        A( _N['introduction'], href=makeURL( 'introduction/', lang ) ) ]),
	
        Tree \
        ( [
            #A( _N['status'], href=makeURL('introduction/status/everything.html' ),
            A( _N['screenshots'], href=makeURL( 'pictures/screenshots/', lang) ),
            A( _N['ports'], href=makeURL( 'introduction/ports', lang ) ),
            A( _N['license'], href='%(BASE)slicense.html' )
        ] ),
        BR(),
	P ( contents = [
        Img( src = '/images/pointer.png' ),
        A(  _N['download'], href=makeURL( 'download', lang ) )]),
        BR(),
	P ( contents = [
        Img( src = '/images/pointer.png' ),
        _N['documentation'] ]),
        Tree \
        ( [
            A( _N['users'], href=makeURL( 'documentation/users/', lang ) ),
            Tree \
            ( [
                A( _N['installation'], href=makeURL( 'documentation/users/installation', lang ) ),
                A( _N['using'], href=makeURL( 'documentation/users/using', lang ) ),
                A( _N['shell'], href=makeURL( 'documentation/users/shell/index', lang ) ),
                A( _N['faq'], href=makeURL( 'documentation/users/faq', lang ) ),
                #_N['ports'],
                #A( _N['links'], href=makeURL( 'documentation/users/links', lang ) )
            ] ),
            A( _N['developers'], href=makeURL( 'documentation/developers/index', lang ) ),
            Tree \
            ( [
                A( _N['contribute'], href=makeURL( 'documentation/developers/contribute', lang ) ),
                A( 'Roadmap', href=makeURL( 'documentation/developers/roadmap', lang ) ),
                A( _N['bug-tracker'], href='http://sourceforge.net/tracker/?atid=439463&group_id=43586&func=browse' ),
                A( _N['working-with-subversion'], href=makeURL( 'documentation/developers/svn', lang ) ),
                A( _N['compiling'],  href=makeURL( 'documentation/developers/compiling', lang ) ),
                A( _N['application-development-manual'], href=makeURL( 'documentation/developers/app-dev/index', lang ) ),
                A( _N['zune-application-development-manual'], href=makeURL( 'documentation/developers/zune-application-development', lang ) ),
                A( _N['system-development-manual'], href=makeURL( 'documentation/developers/system-development', lang ) ),
                A( _N['debugging-manual'], href=makeURL( 'documentation/developers/debugging', lang ) ),
                A( _N['reference'], href=makeURL( 'documentation/developers/autodocs/index', lang ) ),
                A( _N['specifications'], href=makeURL( 'documentation/developers/specifications/', lang ) ),
                A( _N['ui-style-guide'], href=makeURL( 'documentation/developers/ui', lang ) ),
                A( _N['documenting'], href=makeURL( 'documentation/developers/documenting', lang ) ),
                A( _N['porting'], href=makeURL( 'documentation/developers/porting', lang ) ),
                #A( _N['translating'], href=makeURL( 'documentation/developers/translating', lang ) ),
                A( _N['summaries'], href=makeURL( 'documentation/developers/summaries/', lang ) ),
                A( _N['links'], href=makeURL( 'documentation/developers/links', lang ) )
            ] )
        ] ),
        BR(),
	P ( contents = [
        Img( src = '/images/pointer.png' ),
        A( _N['contact'], href=makeURL( 'contact', lang ) )]),
        Tree \
        ( [
            A( _N['mailing-lists'], href=makeURL( 'contact', lang, 'mailing-lists' ) ),
            #A( _N['forums'], href=makeURL( 'contact', lang, 'forums' ) ),
            A( _N['irc-channels'], href=makeURL( 'contact', lang, 'irc-channels' ) )
        ] ),
        BR(),
	P ( contents = [
        Img( src = '/images/pointer.png' ),
        A(  _N['credits'], href=makeURL( 'credits', lang ) )]),
 	P ( contents = [
        Img( src = '/images/pointer.png' ),
        A( 'Acknowledgements', href=makeURL( 'acknowledgements', lang ) )]),
        BR(),
	P ( contents = [
        Img( src = '/images/pointer.png' ),
        _N['pictures']]),
        Tree \
        ( [
            A( _N['developers'], href=makeURL( 'pictures/developers/', lang ) ),
            A( _N['developers-together'], href=makeURL( 'pictures/developers-together/', lang ) )
        ] ),
        BR(),
	P ( contents = [
        Img( src = '/images/pointer.png' ),
        A( _N['sponsors'], href=makeURL( 'sponsors', lang ) )]),
	P ( contents = [
        Img( src = '/images/pointer.png' ),
        A( _N['linking'], href=makeURL( 'linking', lang ) )]),
	P ( contents = [
        Img( src = '/images/pointer.png' ),
        A( _N['links'], href=makeURL( 'links', lang ) )])
    ] )

    counter = Img( 
	src = 'http://www.hepe.com/cgi-bin/wwwcount.cgi?df=aros.dat&dd=E&ft=0' 
    )
    
    sponsors = Table\
    ( 
        cellspacing = 5, cellpadding = 0,
        contents =
        [
            TR
            ( 
                TD
                ( 
                    A
                    ( 
                        Img( src = '%(ROOT)simages/trustec-small.png', border = 0 ),
                        href = 'http://www.trustsec.de/' 
                    )
                )
            ),
            TR
            ( 
                TD
                (
                    A
                    ( 
                        Img( src = '%(ROOT)simages/genesi-small.gif', border = 0 ), 
                        href = 'http://www.pegasosppc.com/' 
                    )
                )
            ),
            TR
            (
                TD
                (
                    A \
                    (
                        Img \
                        (
                            src = 'http://sflogo.sourceforge.net/sflogo.php?group_id=43586&type=1', 
                            width = 88, height = 31, border = 0, alt = 'SourceForge Logo'
                        ),
                        href = 'http://sourceforge.net/'
                    )
                )
            )
        ] 
    )
    
    bar = Table(
        border = 0, cellpadding = 2, cellspacing = 2, width = 171, valign = 'top',
        contents = [
            TR( 
                valign = 'top', contents = [
                    TD( rowspan = 8, width=15 ),
                    TD()
                ]
            ),
            TR( valign = 'top', contents = TD( navigation ) ),
            TR( TD(), height=15 ),
            TR( valign = 'top', contents = TD( align = 'center', contents = counter ) ),
            TR( TD(), height=15 ),
            TR( valign = 'top', contents = TD( align = 'center', contents = sponsors ) ),
            TR( TD(), height=30 ),
            TR \
            (
                valign = 'top', contents = TD \
                (
                    align = 'center', contents = A \
                    (
                        Img \
                        (
                            src = '%(ROOT)simages/noeupatents-small.png', 
                            border = 0
                        ),
                        href = 'http://petition.eurolinux.org/'
                    )
                )
            )
        ]
    )

    statsPHP = '''
        <?php 
            //define("_BBC_PAGE_NAME", "my page title");
            define("_BBCLONE_DIR", "%(ROOT)smybbclone/");
            define("COUNTER", _BBCLONE_DIR."index.php");
            if (file_exists(COUNTER)) include_once(COUNTER);
        ?>
    '''
    statsPHP2 = '''
	<?php echo date("m.d.y");
	?>
    '''
    statsPHP3 = '''
	<?php
		echo "<map name=\\"map\\">";
	      	echo "<area shape=\\"rect\\" coords=\\"11,80,82,95\\" alt=\\"http://www.aros.org\\" href=\\"http://www.aros.org\\">";
      		echo "<area shape=\\"rect\\" coords=\\"87,78,165,95\\" alt=\\"AROS-Exec\\" href=\\"http://www.aros-exec.org\\">";
      		echo "<area shape=\\"rect\\" coords=\\"244,77,323,95166,77,240,95\\" alt=\\"Team AROS\\" href=\\"http://www.teamaros.org\\">";
      		echo "<area shape=\\"rect\\" coords=\\"166,77,240,95\\" alt=\\"AROS-Exec Archives\\" href=\\"http://archives.aros-exec.org\\">";
	      	echo "</map>"; 
	?>
    '''
    statsPHP4 = '''
	<?php
		echo "<table width=\\"100%%\\"><tr><td>";
		echo "<div style=\\"text-align: right;\\">";
		echo "<font color=\\"#aaaaaa\\">";		
	?>
    '''
    statsPHP6 = '''
	<?php
		echo "</font></div>";
		echo "</p></tr></td></table>";
	?>
    '''
    statsPHP5= '''
	<?php
		include( '/home/groups/a/ar/aros/htdocs/rsfeed/browserdetect.php'); 
		$win_ie56 = (browser_detection('browser') == 'ie' ) &&

  		(browser_detection('number') >= 5 ) &&

            	(browser_detection('number') < 7  );
	if ($win_ie56) { 

	echo \"<img src=\\"/images/kittymascot.gif\\"  
        style=\\"float:right\\" border=\\"0\\"></img><img 
        src=\\"/images/toplogomenu.gif\\" border=\\"0\\" 
        usemap=\\"#map\\"></img>";

        }
        else {
        echo \"<img src=\\"/images/kittymascot.png\\"  
        style=\\"float:right\\" 
        border=\\"0\\"></img><img src=\\"/images/toplogomenu.png\\" 
        border=\\"0\\" usemap=\\"#map\\"></img>";
        } ?>
    '''
    page = HTML( [
        Head( [
            Charset( charset ),
            Title( 'AROS Research Operating System' ),
            Link( href = '%(ROOT)saros.css', type = 'text/css', rel = 'stylesheet' ),
            Meta(
                name    = 'keywords',
                content = 'AROS, OS, operating system, research, open source, portage'
            )
        ] ),
        Body( 
            style = 'margin: 0px;',
            bgcolor = '#ffffff', contents = [
		statsPHP3,
                Table(
                    border = 0, cellspacing = 0, cellpadding = 0, 
                    width = '100%%', contents = [
                        TR( [
                            TD( 
                                halign = 'top', width = '100%%', height = 109, background='/images/backgroundtop.png' ,rowspan = 4,
                                contents = statsPHP5)

#				Map(name = 'map')[,
#				Area(shape ='rect', coords='5,84,87,104', alt = 'AROS.org', href="http://www.aros.org"),
#				Area(shape ='rect', coords='96,83,177,104', alt = 'AROS-Exec', href="http://www.aros-exec.org"),
#				Area(shape ='rect', coords='183,84,263,104', alt = 'Team AROS', href="http://www.teamaros.org"),
#				Area(shape ='rect', coords='271,82,353,104', alt = 'AROS-Exec Archives', href="http://archives.aros-exec.org") ],
                            
                        ] ),
			
                        TR(
        	    Table(
		    border = 0, cellspacing = 0, cellpadding = 0, 
                    width = '100%%', contents = [
   			TR( contents = [
                            TD(
                               width = 171, border= 0, cellpadding="2", 
				cellspacing="2", contents = [ bar ]
                                )
                           ,
                            TD( border= 0, width="100%%", cellpadding="1", cellspacing="1", 
                                contents =  '%(CONTENT)s' 
                            ),
                        ]),
                        ] )), 
                        TR( [
                           
                            TD(
                                width = '100%%', colspan = 3, valign = 'bottom', align = 'center',
                                contents = Font(
                                    size = '-1', color = '#aaaaaa',
                                    contents = [
                                        BR(),
					statsPHP4,
                                        _M['copyright'],
                                        BR(),
                                        _M['trademarks'],
					statsPHP6,
                                        BR(),
                                        BR()
                                    ]
                                )
                            )                                
                        
			] )
                    ]
                ),
                statsPHP
            ]
        )
    ] )

    return str( page )


# makeURL
# -------
# Create an internal link.

def makeURL( file, lang, section='' ):

    # Create the URL
    if lang != 'en':
        file = lang + '/' + file
    url = '%(BASE)s' + file
    if file != '' and file[-1] != '/':
        url += '.php'
    if url == '':
        url = '/'

    # Add the section, if any
    if section != '':
        url += '#' + section

    return url

