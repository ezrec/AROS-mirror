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
            A( _N['news'], hclass='almost', href='%(BASE)snews' ),
            Tree ( A( _N['archive'], hclass='done', href='%(BASE)snews/archive' ) ),
            A( _N['introduction'], hclass='done', href='%(BASE)sintroduction' ),
            Tree \
            ( [
                A( _N['status'], hclass='almost', href='%(BASE)sintroduction/status/everything.html' ),
                A( _N['screenshots'], hclass='done', href='%(BASE)spictures/screenshots' ),
                A( _N['ports'], hclass='done', href='%(BASE)sintroduction/ports' ),
                A( _N['license'], hclass='almost', href='%(BASE)slicense.html' )
            ] ),
            A( _N['download'], hclass='almost', href='%(BASE)sdownload' ),
            _N['documentation'],
            Tree \
            ( [
                _N['users'],
                Tree \
                ( [
                    A( _N['installation'], hclass='done', href='%(BASE)sdocumentation/users/installation' ),
                    A( _N['using'], hclass='nope', href='%(BASE)sdocumentation/users/using' ),
                    A( _N['faq'], hclass='done', href='%(BASE)sdocumentation/users/faq' ),
                    #_N['ports'],
                    A( _N['links'], hclass='done', href='%(BASE)sdocumentation/users/links' )
                ] ),
                _N['developers'],
                Tree \
                ( [
                    A( _N['contribute'], hclass='done', href='%(BASE)sdocumentation/developers/contribute' ),
                    A( _N['working-with-cvs'], hclass='done', href='%(BASE)sdocumentation/developers/cvs' ),
                    A( _N['compiling'], hclass='nope', href='%(BASE)sdocumentation/developers/compiling' ),
                    A( _N['application-development-manual'], hclass='nope', href='%(BASE)sdocumentation/developers/application-development' ),
                    A( _N['system-development-manual'], hclass='almost', href='%(BASE)sdocumentation/developers/system-development' ),
                    #A( _N['reference'], hclass='nope', href='%(BASE)sdocumentation/developers/reference' ),
                    A( _N['specifications'], hclass='nope', href='%(BASE)sdocumentation/developers/specifications' ),
                    #A( _N['ui-style-guide'], hclass='nope', href='%(BASE)sdocumentation/developers/ui' ),
                    A( _N['documenting'], hclass='nope', href='%(BASE)sdocumentation/developers/documenting' ),
                    #A( _N['translating'], hclass='nope', href='%(BASE)sdocumentation/developers/translating' ),
                    A( _N['summaries'], hclass='nope', href='%(BASE)sdocumentation/developers/summaries' ),
                    A( _N['links'], hclass='done', href='%(BASE)sdocumentation/developers/links' )
                ] )
            ] ),
            A( _N['contact'], hclass='done', href='%(BASE)scontact' ),
            Tree \
            ( [
                A( _N['mailing-lists'], hclass='done', href='%(BASE)scontact#mailing-lists' ),
                #A( _N['forums'], hclass='done', href='%(BASE)scontact#forums' ),
                A( _N['irc-channels'], hclass='done', href='%(BASE)scontact#irc-channels' )
            ] ),
            A( _N['credits'], hclass='done', href='%(BASE)scredits' ),
            _N['pictures'],
            Tree \
            ( [
                A( _N['developers'], hclass='done', href='%(BASE)spictures/developers' ),
                A( _N['developers-together'], hclass='done', href='%(BASE)spictures/developers-together' )
            ] ),
            A( _N['sponsors'], hclass='done', href='%(BASE)ssponsors' ),
            A( _N['linking'], hclass='done', href='%(BASE)slinking' ),
            A( _N['links'], hclass='done', href='%(BASE)slinks' )
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
                                    rowspan = 2, contents = Img \
                                    (
                                        src='%(BASE)simages/kitty_small.png',
                                        alt='Kitty, the AROS mascot'
                                    )
                                ),
                                TD \
                                ( 
                                    align = 'center', height='100%%', contents = Img \
                                    ( 
                                        src='%(BASE)simages/aros-logo.png' 
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
                    width = '100%%',
                    contents = Table \
                    (
                        width = '100%%', cellspacing = 1, cellpadding = 3,
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


