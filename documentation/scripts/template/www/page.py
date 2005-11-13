# -*- coding: iso-8859-1 -*-
# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

from html import *
from components import *

def makePage( _T, _N, _M, MIRRORS_DATA ):
    navigation = Tree \
    ( [
        A( 'Home', href='%(BASE)sindex.php' ),
        BR(),
        A( _N['news'], href='%(BASE)snews/' ),
        Tree ( A( _N['archive'], href='%(BASE)snews/archive/' ) ),
        BR(),
        A( _N['introduction'], href='%(BASE)sintroduction/' ),
        Tree \
        ( [
            #A( _N['status'], href='%(BASE)sintroduction/status/everything.html' ),
            A( _N['screenshots'], href='%(BASE)spictures/screenshots/' ),
            A( _N['ports'], href='%(BASE)sintroduction/ports.php' ),
            A( _N['license'], href='%(BASE)slicense.html' )
        ] ),
        BR(),
        A( _N['download'], href='%(BASE)sdownload.php' ),
        BR(),
        _N['documentation'],
        Tree \
        ( [
            A( _N['users'], href='%(BASE)sdocumentation/users/index.php' ),
            Tree \
            ( [
                A( _N['installation'], href='%(BASE)sdocumentation/users/installation.php' ),
                A( _N['using'], href='%(BASE)sdocumentation/users/using.php' ),
                A( _N['faq'], href='%(BASE)sdocumentation/users/faq.php' ),
                #_N['ports'],
                #A( _N['links'], href='%(BASE)sdocumentation/users/links.php' )
            ] ),
            A( _N['developers'], href='%(BASE)sdocumentation/developers/index.php' ),
            Tree \
            ( [
                A( _N['contribute'], href='%(BASE)sdocumentation/developers/contribute.php' ),
                A( 'Roadmap', href='%(BASE)sdocumentation/developers/roadmap.php' ),
                A( _N['bug-tracker'], href='http://sourceforge.net/tracker/?atid=439463&group_id=43586&func=browse' ),
                A( _N['working-with-subversion'], href='%(BASE)sdocumentation/developers/svn.php' ),
                A( _N['compiling'],  href='%(BASE)sdocumentation/developers/compiling.php' ),
                A( _N['application-development-manual'], href='%(BASE)sdocumentation/developers/application-development.php' ),
                A( _N['zune-application-development-manual'], href='%(BASE)sdocumentation/developers/zune-application-development.php' ),
                A( _N['system-development-manual'], href='%(BASE)sdocumentation/developers/system-development.php' ),
                A( _N['debugging-manual'], href='%(BASE)sdocumentation/developers/debugging.php' ),
                #A( _N['reference'], href='%(BASE)sdocumentation/developers/reference' ),
                A( _N['specifications'], href='%(BASE)sdocumentation/developers/specifications/' ),
                A( _N['ui-style-guide'], href='%(BASE)sdocumentation/developers/ui.php' ),
                A( _N['documenting'], href='%(BASE)sdocumentation/developers/documenting.php' ),
                #A( _N['translating'], href='%(BASE)sdocumentation/developers/translating' ),
                A( _N['summaries'], href='%(BASE)sdocumentation/developers/summaries/' ),
                A( _N['links'], href='%(BASE)sdocumentation/developers/links.php' )
            ] )
        ] ),
        BR(),
        A( _N['contact'], href='%(BASE)scontact.php' ),
        Tree \
        ( [
            A( _N['mailing-lists'], href='%(BASE)scontact.php#mailing-lists' ),
            #A( _N['forums'], href='%(BASE)scontact.php#forums' ),
            A( _N['irc-channels'], href='%(BASE)scontact.php#irc-channels' )
        ] ),
        BR(),
        A( _N['credits'], href='%(BASE)scredits.php' ),
        A( 'Acknowledgements', href='%(BASE)sacknowledgements.php' ),
        BR(),
        _N['pictures'],
        Tree \
        ( [
            A( _N['developers'], href='%(BASE)spictures/developers/' ),
            A( _N['developers-together'], href='%(BASE)spictures/developers-together/' )
        ] ),
        BR(),
        A( _N['sponsors'],href='%(BASE)ssponsors.php' ),
        A( _N['linking'], href='%(BASE)slinking.php' ),
        A( _N['links'], href='%(BASE)slinks.php' )
    ] )

    counter = Img(
        src = 'http://sbb.hepe.com/cgi-bin/wwwcount.cgi?df=aros.dat&dd=C&ft=0'
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
        border = 0, cellpadding = 0, cellspacing = 0, valign = 'top',
        contents = [
            TR( 
                valign = 'top', contents = [
                    TD( rowspan = 8, width = 10 ),
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
    page = HTML( [
        Head( [
            Title( 'AROS: Amiga® Research Operating System' ),
            Link( href = '%(ROOT)saros.css', type = 'text/css', rel = 'stylesheet' ),
            Meta(
                name    = 'keywords',
                content = 'AROS, Amiga, OS, operating system, research, open source, portage'
            )
        ] ),
        Body( 
            style = 'margin: 0px;',
            bgcolor = '#ffffff', contents = [
                Table(
                    border = 0, cellspacing = 0, cellpadding = 0, 
                    width = '100%%', height = '100%%', contents = [
                        TR( [
                            TD( 
                                valign = 'top', width = 184, rowspan = 4,
                                contents = Img(
                                    width = 184, height = 190,
                                    src = '%(ROOT)simages/kitty_1.png'
                                )
                            ),
                            TD(
                                valign = 'top', width = 65, rowspan = 4,
                                contents = Img(
                                    width = 65, height = 190,
                                    src = '%(ROOT)simages/kitty_2.png'
                                )
                            ),
                            TD(
                                valign = 'top', width = 53, rowspan = 4,
                                contents = Img(
                                    width = 53, height = 190,
                                    src = '%(ROOT)simages/kitty_3.png'
                                )
                            ),
                            TD(
                                valign = 'top', align = 'left', width = 344, 
                                background = '%(ROOT)simages/dark_bg.png',
                                contents = Img(
                                    width = 344, height = 81,
                                    src = '%(ROOT)simages/aros.png'
                                )
                            ),
                            TD(
                                valign = 'top', align = 'right', width = '100%%',
                                background = '%(ROOT)simages/dark_bg.png',
                                #contents = Table(
                                #    border = 0, cellpadding = 0, cellspacing = 2,
                                #    contents = [
                                #        TR( 
                                #            TD( Font( size = '-2', color = '#ffffff', contents = B( _T['mirrors'] ) ) )
                                #        ),
                                #        TR(
                                #            TD( MIRRORS_DATA )
                                #        )
                                #    ]
                                #)
                                contents = Img(
                                    width = '100%%', height = 1,
                                    src = '%(ROOT)simages/spacer.gif'
                                )
                            )
                        ] ),
                        TR(
                            TD(
                                background = '%(ROOT)simages/dark_bg.png',
                                align = 'center', colspan = 2, contents = Img(
                                    width = 367, height = 36,
                                    src = '%(ROOT)simages/aros_text.png'
                                )
                            )
                        ),
                        TR( [
                            TD(
                                background = '%(ROOT)simages/horizontal_border.png',
                                width = 344, contents = Img(
                                    width = 344, height = 33,
                                    src = '%(ROOT)simages/h_arc.png'
                                )
                            ),
                            TD( background = '%(ROOT)simages/horizontal_border.png' )
                        ] ),
                        TR( 
                            TD(
                                width = 1, height = 40, colspan = 2,
                                bgcolor = '#ffffff', contents = Font(
                                    size = '-1', color = '#aaaaaa',
                                    contents = Img(
                                        width = '100%%', height = 40,
                                        src = '%(ROOT)simages/spacer.gif'
                                    )
                                )
                            )
                        ),
                        TR( [
                            TD(
                                rowspan = 2, valign = 'top', width = 184,
                                height = '100%%',
                                background = '%(ROOT)simages/bright_bg.png',
                                style = 'width: 184px; height: 100%%',
                                contents = [ bar ]
                            ),
                            TD(
                                background = '%(ROOT)simages/vertical_border.png',
                                width = 65, height = 393, contents = Img(
                                    width = 65, height = 393, 
                                    src = '%(ROOT)simages/v_arc.png'
                                )
                            ),
                            TD(
                                colspan = 3, rowspan = 1, valign = 'top',
                                height = '100%%',
                                contents = [
                                    Img(
                                        width = '100%%', height = 1,
                                        src = '%(ROOT)simages/spacer.gif'
                                    ),
                                    BR(),
                                    Table(
                                        border = 0, cellspacing = 0, 
                                        cellpadding = 0, width = '100%%',
                                        bgcolor = '#ffffff',
                                        contents = TR( TD( '%(CONTENT)s' ) )
                                    )
                                ]
                            )
                        ] ),
                        TR( [
                            TD(
                                width = 65, height = '100%%', style = 'height: 100%%',
                                background = '%(ROOT)simages/vertical_border.png',
                                contents = Img(
                                    width = 1, height = '100%%',
                                    src = '%(ROOT)simages/spacer.gif'
                                )
                            ),
                            TD(
                                colspan = 3, valign = 'bottom', align = 'center',
                                contents = Font(
                                    size = '-1', color = '#aaaaaa',
                                    contents = [
                                        BR(),
                                        _M['copyright'],
                                        BR(),
                                        _M['trademarks'],
                                        BR(), BR()
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
