# -*- coding: iso-8859-1 -*-
# Copyright © 2002-2006, The AROS Development Team. All rights reserved.
# $Id$

import os

from html import *
from components import *

def makePage( _T, _N, _M, MIRRORS_DATA, lang, charset ):
    navigation = Tree \
    ( [
        A( _N['home'], href=makeURL( '/', lang ) ),
        Tree \
        ( [
            A( 'English', href='%(BASE)s/' ),
            A( 'Deutsch', href='%(BASE)sde/' ),
            A( 'Fran&#231;ais', href='%(BASE)sfr/' ),
            A( 'Italiano', href='%(BASE)sit/' ),            
            A( 'Polski', href='%(BASE)spl/' ),
            A( 'Suomi', href='%(BASE)sfi/' ),
            A( '&#1056;&#1091;&#1089;&#1089;&#1082;&#1080;&#1081;', href='%(BASE)sru/' )
        ] ),
        BR(),
        A( _N['news'], href=makeURL( 'news/', lang ) ),
        Tree ( A( _N['archive'], href=makeURL( 'news/archive/', lang ) ) ),
        BR(),
        A( _N['introduction'], href=makeURL( 'introduction/', lang ) ),
        Tree \
        ( [
            #A( _N['status'], href=makeURL('introduction/status/everything.html' ),
            A( _N['screenshots'], href=makeURL( 'pictures/screenshots/', lang) ),
            A( _N['ports'], href=makeURL( 'introduction/ports', lang ) ),
            A( _N['license'], href='%(BASE)slicense.html' )
        ] ),
        BR(),
        A( _N['download'], href=makeURL( 'download', lang ) ),
        BR(),
        _N['documentation'],
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
                #A( _N['translating'], href=makeURL( 'documentation/developers/translating', lang ) ),
                A( _N['summaries'], href=makeURL( 'documentation/developers/summaries/', lang ) ),
                A( _N['links'], href=makeURL( 'documentation/developers/links', lang ) )
            ] )
        ] ),
        BR(),
        A( _N['contact'], href=makeURL( 'contact', lang ) ),
        Tree \
        ( [
            A( _N['mailing-lists'], href=makeURL( 'contact', lang, 'mailing-lists' ) ),
            #A( _N['forums'], href=makeURL( 'contact', lang, 'forums' ) ),
            A( _N['irc-channels'], href=makeURL( 'contact', lang, 'irc-channels' ) )
        ] ),
        BR(),
        A( _N['credits'], href=makeURL( 'credits', lang ) ),
        A( 'Acknowledgements', href=makeURL( 'acknowledgements', lang ) ),
        BR(),
        _N['pictures'],
        Tree \
        ( [
            A( _N['developers'], href=makeURL( 'pictures/developers/', lang ) ),
            A( _N['developers-together'], href=makeURL( 'pictures/developers-together/', lang ) )
        ] ),
        BR(),
        A( _N['sponsors'],href=makeURL( 'sponsors', lang ) ),
        A( _N['linking'], href=makeURL( 'linking', lang ) ),
        A( _N['links'], href=makeURL( 'links', lang ) )
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
            Charset( charset ),
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


# makeURL
# -------
# Create an internal link.

def makeURL( file, lang, section='' ):

    # Create the URL
    if lang != 'en':
        file = lang + '/' + file
    url = '%(BASE)s' + file
    if file[-1] != '/':
        url += '.php'

    # Add the section, if any
    if section != '':
        url += '#' + section

    return url

