# Copyright © 2002, The AROS Development Team. All rights reserved.
# $Id$

from html import *
from components import *

def makePage( _T, _N, _M, MIRRORS_DATA ):
    mirrors = Box \
    (
        title    = _T['mirrors'],
        contents = MIRRORS_DATA
    )

    navigation = Box \
    (
        title    = _T['contents'],
        contents = Tree \
        ( [
            A( _N['news'],  href='%(BASE)snews/' ),
            Tree ( A( _N['archive'], href='%(BASE)snews/archive/' ) ),
            A( _N['introduction'], href='%(BASE)sintroduction/' ),
            Tree \
            ( [
                A( _N['status'], href='%(BASE)sintroduction/status/everything.html' ),
                A( _N['screenshots'], href='%(BASE)spictures/screenshots/' ),
                A( _N['ports'], href='%(BASE)sintroduction/ports.html' ),
                A( _N['license'], href='%(BASE)slicense.html' )
            ] ),
            A( _N['download'], href='%(BASE)sdownload/' ),
            _N['documentation'],
            Tree \
            ( [
                _N['users'],
                Tree \
                ( [
                    A( _N['installation'], href='%(BASE)sdocumentation/users/installation.html' ),
                    A( _N['using'], href='%(BASE)sdocumentation/users/using.html' ),
                    A( _N['faq'], href='%(BASE)sdocumentation/users/faq.html' ),
                    #_N['ports'],
                    A( _N['links'], href='%(BASE)sdocumentation/users/links.html' )
                ] ),
                _N['developers'],
                Tree \
                ( [
                    A( _N['contribute'], href='%(BASE)sdocumentation/developers/contribute.html' ),
                    A( _N['working-with-cvs'], href='%(BASE)sdocumentation/developers/cvs.html' ),
                    A( _N['compiling'],  href='%(BASE)sdocumentation/developers/compiling.html' ),
                    A( _N['application-development-manual'], href='%(BASE)sdocumentation/developers/application-development.html' ),
                    A( _N['system-development-manual'], href='%(BASE)sdocumentation/developers/system-development.html' ),
                    #A( _N['reference'], href='%(BASE)sdocumentation/developers/reference' ),
                    A( _N['specifications'], href='%(BASE)sdocumentation/developers/specifications/' ),
                    #A( _N['ui-style-guide'], href='%(BASE)sdocumentation/developers/ui' ),
                    A( _N['documenting'], href='%(BASE)sdocumentation/developers/documenting.html' ),
                    #A( _N['translating'], href='%(BASE)sdocumentation/developers/translating' ),
                    A( _N['summaries'], href='%(BASE)sdocumentation/developers/summaries/' ),
                    A( _N['links'], href='%(BASE)sdocumentation/developers/links.html' )
                ] )
            ] ),
            A( _N['contact'], href='%(BASE)scontact.html' ),
            Tree \
            ( [
                A( _N['mailing-lists'], href='%(BASE)scontact.html#mailing-lists' ),
                #A( _N['forums'], href='%(BASE)scontact.html#forums' ),
                A( _N['irc-channels'], href='%(BASE)scontact.html#irc-channels' )
            ] ),
            A( _N['credits'], href='%(BASE)scredits.html' ),
            _N['pictures'],
            Tree \
            ( [
                A( _N['developers'], href='%(BASE)spictures/developers/' ),
                A( _N['developers-together'], href='%(BASE)spictures/developers-together/' )
            ] ),
            A( _N['sponsors'],href='%(BASE)ssponsors.html' ),
            A( _N['linking'], href='%(BASE)slinking.html' ),
            A( _N['links'], href='%(BASE)slinks.html' )
        ] )
    )

    search = Box \
    ( 
        title    = _T['search'],
        contents = Form \
        (
            action   = 'http://aros.sourceforge.net/tools/search', method='post',
            contents = Table \
            (
                cellspacing = 2, cellpadding = 0,
                contents = TR \
                ( [
                    TD( Input( type='text', name='text', size=17 ) ),
                    TD( Input( type='submit', value=_M['search'] ) )
                ] )
            )
        )
    )

    visitors = Box \
    (
        title    = _T['visitors'],
        contents = Img \
        (
            src = 'http://www.aros.org/cgi-bin/wwwcount.cgi?df=aros.dat&dd=B&comma=Y&pad=Y&md=7', 
            height = 30, width = 94
        )
    )

    sponsors = Box \
    (
        title    = _T['sponsors'],
        contents = Table\
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
                        A \
                        (
                            Img \
                            (
                                src = 'http://sourceforge.net/sflogo.php?group_id=43586&type=1', 
                                width = 88, height = 31, border = 0, alt = 'SourceForge Logo'
                            ),
                            href = 'http://sourceforge.net'
                        )
                    )
                )
            ] 
        )
    )

    bar = Table \
    ( 
        cellspacing = 0, cellpadding = 0, 
        contents = \
        [
            TR( TD( mirrors ) ),
            TR( TD(), height=15 ),
            TR( TD( navigation ) ),
            TR( TD(), height=15 ),
            #TR( TD( search ) ),
            #TR( TD(), height=15 ),
            TR( TD( visitors ) ),
            TR( TD(), height=15 ),
            TR( TD( sponsors ) )
        ] 
    )

    layout = Table \
    ( 
        cellspacing = 20,
        contents = \
        [
            TR \
            ( 
                TD \
                ( 
                    colspan = 2, align = 'center', 
                    contents = Table \
                    (
                        border = 0, contents = \
                        [
                            TR \
                            ( [
                                TD \
                                ( 
                                    align = 'center', height='100%%', contents = Img \
                                    ( 
                                        src='%(BASE)simages/aros-logo.png' 
                                    )
                                ),
                                TD \
                                (
                                    rowspan = 2, contents = Img \
                                    (
                                        src='%(BASE)simages/kitty_small.png',
                                        alt='Kitty, the AROS mascot'
                                    )
                                )
                            ] ),
                            TR \
                            (
                                height = '*', contents = TD \
                                ( 
                                    valign = 'top', align = 'center', 
                                    contents = B( 'Amiga Research OS' ) 
                                )
                            )
                        ]
                    )
                )
            ),
            TR \
            ( [ 
                TD( bar ),
                TD \
                (
                    align='center', width = '100%%',
                    contents = Table \
                    (
                        width = '100%%', cellspacing = 1, cellpadding = 1,
                        bgcolor = Box.BORDER,
                        contents = TR \
                        ( 
                            TD \
                            ( 
                                width = '100%%', bgcolor = Box.CONTENT, 
                                contents = '%(CONTENT)s' 
                            )
                        )
                    )
                )
            ] ),
            TR \
            ( 
                TD \
                ( 
                    colspan  = 2, align = 'center', 
                    contents = Font \
                    ( 
                        size = '-1',
                        contents = \
                        [
                            _M['copyright'], BR(), _M['trademarks']
                        ] 
                    ) 
                )
            )
        ]
    )

    page = HTML \
    ( [
        Head \
        ( [
            Title( 'The Amiga Research OS' ),
            Link( href='%(ROOT)saros.css', type='text/css', rel='stylesheet' ),
            Meta \
            ( 
                name    = 'keywords', 
                content = 'AROS, Amiga, OS, operating system, research, open source, portable' 
            )
        ] ),
        Body( bgcolor = '#FFFFFF', contents = layout )
    ] )

    return str( page )


