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
            A( _N['news'], hclass='almost', href='%(ROOT)snews' ),
            Tree ( A( _N['archive'], hclass='done', href='%(ROOT)snews/archive' ) ),
            A( _N['introduction'], hclass='done', href='%(ROOT)sintroduction' ),
            Tree \
            ( [
                _N['status'],
                A( _N['screenshots'], hclass='done', href='%(ROOT)spictures/screenshots' ),
                A( _N['ports'], hclass='done', href='%(ROOT)sintroduction/ports' )
            ] ),
            A( _N['download'], hclass='almost', href='%(ROOT)sdownload' ),
            _N['documentation'],
            Tree \
            ( [
                _N['users'],
                Tree \
                ( [
                    A( _N['installation'], hclass='done', href='%(ROOT)sdocumentation/users/installation' ),
                    A( _N['using'], hclass='nope', href='%(ROOT)sdocumentation/users/using' ),
                    A( _N['faq'], hclass='done', href='%(ROOT)sdocumentation/users/faq' ),
                    _N['ports'],
                    A( _N['links'], hclass='done', href='%(ROOT)sdocumentation/users/links' )
                ] ),
                _N['developers'],
                Tree \
                ( [
                    A( _N['contribute'], hclass='done', href='%(ROOT)sdocumentation/developers/contribute' ),
                    A( _N['working-with-cvs'], hclass='done', href='%(ROOT)sdocumentation/developers/cvs' ),
                    A( _N['compiling'], hclass='nope', href='%(ROOT)sdocumentation/developers/compiling' ),
                    A( _N['application-development-manual'], hclass='nope', href='%(ROOT)sdocumentation/developers/application-development' ),
                    A( _N['system-development-manual'], hclass='almost', href='%(ROOT)sdocumentation/developers/system-development' ),
                    A( _N['reference'], hclass='nope', href='%(ROOT)sdocumentation/developers/reference' ),
                    A( _N['specifications'], hclass='nope', href='%(ROOT)sdocumentation/developers/specifications' ),
                    A( _N['ui-style-guide'], hclass='nope', href='%(ROOT)sdocumentation/developers/ui' ),
                    A( _N['documenting'], hclass='nope', href='%(ROOT)sdocumentation/developers/documenting' ),
                    A( _N['translating'], hclass='nope', href='%(ROOT)sdocumentation/developers/translating' ),
                    A( _N['summaries'], hclass='nope', href='%(ROOT)sdocumentation/developers/summaries' ),
                    A( _N['links'], hclass='done', href='%(ROOT)sdocumentation/developers/links' )
                ] )
            ] ),
            A( _N['contact'], hclass='done', href='%(ROOT)scontact' ),
            Tree \
            ( [
                A( _N['mailing-lists'], hclass='done', href='%(ROOT)scontact#mailing-lists' ),
                A( _N['forums'], hclass='done', href='%(ROOT)scontact#forums' ),
                A( _N['irc-channels'], hclass='done', href='%(ROOT)scontact#irc-channels' )
            ] ),
            A( _N['credits'], hclass='done', href='%(ROOT)scredits' ),
            _N['pictures'],
            Tree \
            ( [
                A( _N['developers'], hclass='done', href='%(ROOT)spictures/developers' ),
                A( _N['developers-together'], hclass='done', href='%(ROOT)spictures/developers-together' )
            ] ),
            A( _N['sponsors'], hclass='done', href='%(ROOT)ssponsors' ),
            A( _N['links'], hclass='done', href='%(ROOT)slinks' )
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
            TR( TD( colspan = 2, align = 'center', contents = 'Image a spiffy logo here.' ) ),
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
        Body( bgcolor = '#EFEFEF', contents = layout )
    ] )

    return str( page )


