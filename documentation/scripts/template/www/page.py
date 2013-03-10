# -*- coding: iso-8859-1 -*-
# Copyright (C) 2002-2012, The AROS Development Team. All rights reserved.
# $Id$

import os

from html import *
from components import *

def makePage( _T, _N, _M, MIRRORS_DATA, lang, charset ):
    navigation = Tree \
    ( [    
    P ( contents = [
    Img( src = '%(ROOT)simages/pointer.png', alt = 'pointer'),
    A( _N['home'], href=makeURL( '.', lang ))]
    ),
        Tree \
        ( [
        P ( contents = [
            Img( src = '%(ROOT)simages/czechlogo.png', width = 16, height = 10, alt = 'czech logo'),
            A( '&#268;esky', href='%(BASE)scs/' )]),
        P ( contents = [
            Img( src = '%(ROOT)simages/germanylogo.png', width = 16, height = 10, alt = 'germany logo'),
            A( 'Deutsch', href='%(BASE)sde/' )]),
            P ( contents = [
            Img( src = '%(ROOT)simages/greecelogo.png', width = 16, height = 10, alt = 'greece logo'),
            A( '&#917;&#955;&#955;&#951;&#965;&#953;&#954;&#940;', href='%(BASE)sel/' )]),
        P ( contents = [
            Img( src = '%(ROOT)simages/englishlogo.png', width = 16, height = 10, alt = 'english logo'),
            A( 'English', href='%(BASE)s.' )]),
        P ( contents = [
            Img( src = '%(ROOT)simages/spanishlogo.png', width = 16, height = 10, alt = 'spanish logo'),
            A( 'Espa&#241ol', href='%(BASE)ses/' )]),
            P ( contents = [
            Img( src = '%(ROOT)simages/francelogo.png', width = 16, height = 10, alt = 'france logo'),
            A( 'Fran&#231;ais', href='%(BASE)sfr/' )]),
            P ( contents = [
            Img( src = '%(ROOT)simages/italylogo.png', width = 16, height = 10, alt = 'italy logo'),
            A( 'Italiano', href='%(BASE)sit/' )]),
            P ( contents = [
            Img( src = '%(ROOT)simages/netherlandslogo.png', width = 16, height = 10, alt = 'netherlands logo'),
            A( 'Nederlands', href='%(BASE)snl/' )]),
        P ( contents = [
            Img( src = '%(ROOT)simages/polandlogo.png', width = 16, height = 10, alt = 'poland logo'),
            A( 'Polski', href='%(BASE)spl/' )]),
            P ( contents = [
            Img( src = '%(ROOT)simages/portugallogo.png', width = 16, height = 10, alt = 'portugal logo'),
            A( 'Portugu&#234;s', href='%(BASE)spt/' )]),
            P ( contents = [
            Img( src = '%(ROOT)simages/russialogo.png', width = 16, height = 10, alt = 'russian logo'),
            A( '&#1056;&#1091;&#1089;&#1089;&#1082;&#1080;&#1081;', href='%(BASE)sru/' )]),
        P ( contents = [
            Img( src = '%(ROOT)simages/finlandlogo.png', width = 16, height = 10, alt = 'finland logo'),
            A( 'Suomi', href='%(BASE)sfi/' )]),
        P ( contents = [
            Img( src = '%(ROOT)simages/swedenlogo.png', width = 16, height = 10, alt = 'sweden logo'),
            A( 'Svenska', href='%(BASE)ssv/' )])
        ] ),
        BR(),
    P ( contents = [
        Img( src = '%(ROOT)simages/pointer.png', alt = 'pointer' ),    
        A( _N['news'], href=makeURL( 'news/', lang ) )]),
        Tree ( A( _N['archive'], href=makeURL( 'news/archive/', lang ) ) ),
        BR(),
    P ( contents = [
        Img( src = '%(ROOT)simages/pointer.png', alt = 'pointer' ),
        A( _N['introduction'], href=makeURL( 'introduction/', lang ) ) ]),
    
        Tree \
        ( [
            A( _N['status'], href=makeURL('introduction/status/everything', lang ) ),
            A( _N['screenshots'], href=makeURL( 'pictures/screenshots/', lang) ),
            A( _N['ports'], href=makeURL( 'introduction/ports', lang ) ),
            A( _N['license'], href='%(BASE)slicense.html' )
        ] ),
        BR(),
    P ( contents = [
        Img( src = '%(ROOT)simages/pointer.png', alt = 'pointer' ),
        A(  _N['download'], href=makeURL( 'download', lang ) )]),
        BR(),
    P ( contents = [
        Img( src = '%(ROOT)simages/pointer.png', alt = 'pointer' ),
        _N['documentation'] ]),
        Tree \
        ( [
            A( _N['users'], href=makeURL( 'documentation/users/', lang ) ),
            Tree \
            ( [
                A( _N['installation'], href=makeURL( 'documentation/users/installation', lang ) ),
                A( _N['using'], href=makeURL( 'documentation/users/using', lang ) ),
                A( _N['shell'], href=makeURL( 'documentation/users/shell/index', lang ) ),
                A( _N['applications'], href=makeURL( 'documentation/users/applications/index', lang ) ),
                A( _N['faq'], href=makeURL( 'documentation/users/faq', lang ) ),
                A( _N['howto'], href=makeURL( 'documentation/users/howto', lang ) ),
                A( _N['hwcompat'], href='http://en.wikibooks.org/wiki/Aros/Platforms/x86_support'),
            ] ),
            A( _N['developers'], href=makeURL( 'documentation/developers/index', lang ) ),
            Tree \
            ( [
                A( _N['contribute'], href=makeURL( 'documentation/developers/contribute', lang ) ),
                A( _N['roadmap'], href=makeURL( 'documentation/developers/roadmap', lang ) ),
                A( _N['bug-tracker'], href='http://sourceforge.net/tracker/?atid=439463&amp;group_id=43586&amp;func=browse' ),
                A( _N['feature-requests'], href='http://sourceforge.net/tracker/?group_id=43586&amp;atid=439466' ),
                A( _N['working-with-subversion'], href=makeURL( 'documentation/developers/svn', lang ) ),
                A( _N['compiling'],  href=makeURL( 'documentation/developers/compiling', lang ) ),
                A( _N['application-development-manual'], href=makeURL( 'documentation/developers/app-dev/index', lang ) ),
                A( _N['zune-application-development-manual'], href=makeURL( 'documentation/developers/zune-dev/index', lang ) ),
                A( _N['system-development-manual'], href=makeURL( 'documentation/developers/sys-dev/index', lang ) ),
                A( _N['debugging-manual'], href=makeURL( 'documentation/developers/debugging', lang ) ),
                A( _N['reference'], href=makeURL( 'documentation/developers/autodocs/index', lang ) ),
                A( _N['specifications'], href=makeURL( 'documentation/developers/specifications/', lang ) ),
                A( _N['ui-style-guide'], href=makeURL( 'documentation/developers/ui', lang ) ),
                A( _N['documenting'], href=makeURL( 'documentation/developers/documenting', lang ) ),
                A( _N['porting'], href=makeURL( 'documentation/developers/porting', lang ) ),
                A( _N['translating'], href=makeURL( 'documentation/developers/translating', lang ) ),
                A( _N['summaries'], href=makeURL( 'documentation/developers/summaries/', lang ) ),
                A( _N['links'], href=makeURL( 'documentation/developers/links', lang ) )
            ] )
        ] ),
        BR(),
    P ( contents = [
        Img( src = '%(ROOT)simages/pointer.png', alt = 'pointer' ),
        A( _N['contact'], href=makeURL( 'contact', lang ) )]),
        Tree \
        ( [
            A( _N['mailing-lists'], href=makeURL( 'contact', lang, 'mailing-lists' ) ),
            #A( _N['forums'], href=makeURL( 'contact', lang, 'forums' ) ),
            A( _N['irc-channels'], href=makeURL( 'contact', lang, 'irc-channels' ) )
        ] ),
        BR(),
    P ( contents = [
        Img( src = '%(ROOT)simages/pointer.png', alt = 'pointer' ),
        A(  _N['credits'], href=makeURL( 'credits', lang ) )]),
     P ( contents = [
        Img( src = '%(ROOT)simages/pointer.png', alt = 'pointer' ),
        A( _N['acknowledgements'], href=makeURL( 'acknowledgements', lang ) )]),
        BR(),
    P ( contents = [
        Img( src = '%(ROOT)simages/pointer.png', alt = 'pointer' ),
        _N['pictures']]),
        Tree \
        ( [
            A( _N['developers'], href=makeURL( 'pictures/developers/', lang ) ),
            A( _N['developers-together'], href=makeURL( 'pictures/developers-together/', lang ) )
        ] ),
        BR(),
    P ( contents = [
        Img( src = '%(ROOT)simages/pointer.png', alt = 'pointer' ),
        A( _N['sponsors'], href=makeURL( 'sponsors', lang ) )]),
    P ( contents = [
        Img( src = '%(ROOT)simages/pointer.png', alt = 'pointer' ),
        A( _N['linking'], href=makeURL( 'linking', lang ) )]),
    P ( contents = [
        Img( src = '%(ROOT)simages/pointer.png', alt = 'pointer' ),
        A( _N['links'], href=makeURL( 'links', lang ) )])
    ] )

    counter = Img( 
    src = 'http://www.hepe.com/cgi-bin/wwwcount.cgi?df=aros.dat&amp;dd=E&amp;ft=0', alt = 'www.hepe.com' 
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
                        Img( src = '%(ROOT)simages/trustec-small.png', border = 0, alt = 'Trustsec' ),
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
                        Img( src = '%(ROOT)simages/genesi-small.gif', border = 0, alt = 'Genesi USA' ), 
                        href = 'http://www.genesi-tech.com/' 
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
                            src = 'http://sflogo.sourceforge.net/sflogo.php?group_id=43586&amp;type=10', 
                            width = 88, height = 16, border = 0, alt = 'Get AROS Research Operating System at SourceForge.net. '
                                'Fast, secure and Free Open Source software downloads'
                        ),
                        href = 'http://sourceforge.net/projects/aros/'
                    )
                )
            )
        ] 
    )
    
    bar = Table(
        border = 0, cellpadding = 2, cellspacing = 2, width = 171,
        contents = [
            TR( 
                valign = 'top', contents = [
                    TD( rowspan = 8, width=15 ),
                    TD()
                ]
            ),
            TR( valign = 'top', contents = TD( navigation ) ),
            TR( TD() ),
            TR( valign = 'top', contents = TD( align = 'center', contents = counter ) ),
            TR( TD() ),
            TR( valign = 'top', contents = TD( align = 'center', contents = sponsors ) ),
            TR( TD()),
            TR \
            (
                valign = 'top', contents = TD \
                (
                    align = 'center', contents = A \
                    (
                        Img \
                        (
                            src = '%(ROOT)simages/noeupatents-small.png', alt = 'No EU patents',
                            border = 0
                        ),
                        href = 'http://stopsoftwarepatents.eu/'
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
              echo "<area shape=\\"rect\\" coords=\\"25,78,85,95\\" alt=\\"http://www.aros.org\\" href=\\"http://www.aros.org\\">";
              echo "<area shape=\\"rect\\" coords=\\"100,78,168,95\\" alt=\\"AROS-Exec\\" href=\\"http://aros-exec.org\\">";
              echo "<area shape=\\"rect\\" coords=\\"180,78,240,95\\" alt=\\"AROS-Exec Archives\\" href=\\"http://archives.aros-exec.org\\">";
              echo "<area shape=\\"rect\\" coords=\\"260,78,350,95\\" alt=\\"Power2People\\" href=\\"http://www.power2people.org\\">";
              echo "</map>"; 
    ?>
    '''
    statsPHP4 = '''
    <?php
        echo "<table width=\\"100%%\\"><tr><td>";
        echo "<div style=\\"text-align: right;\\">";
        echo "<font color=\\"#aaaaaa\\" size=\\"-1\\">";        
    ?>
    '''
    statsPHP6 = '''
    <?php
        echo "</font></div>";
        echo "</td></tr></table>";
    ?>
    '''
    statsPHP5= '''
    <?php
        include( '%(ROOT)srsfeed/browserdetect.php'); 
        $win_ie56 = (browser_detection('browser') == 'ie' ) &&

          (browser_detection('number') >= 5 ) &&

                (browser_detection('number') < 7  );
    if ($win_ie56) { 

    echo \"<img src=\\"/images/kittymascot.gif\\"
        alt=\\"kitty mascot\\"
        style=\\"float:right\\" border=\\"0\\"><img 
        src=\\"/images/toplogomenu.gif\\" border=\\"0\\" 
        alt=\\"top logo menu\\"
        usemap=\\"#map\\">";

        }
        else {
        echo \"<img src=\\"/images/kittymascot.png\\"  
        alt=\\"kitty mascot\\"
        style=\\"float:right\\" 
        border=\\"0\\"><img src=\\"/images/toplogomenu.png\\" 
        alt=\\"top logo menu\\"
        border=\\"0\\" usemap=\\"#map\\">";
        } ?>
    '''
    page = HTML( [
        Head( [
            Charset( charset ),
            Title( 'AROS Research Operating System' ),
            Link( href = '%(ROOT)saros.css', type = 'text/css', rel = 'stylesheet' ),
            Link( href = '%(ROOT)sprint.css', type = 'text/css', rel = 'stylesheet', media = 'print' ),
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
                    style="background-image:url('%(ROOT)simages/backgroundtop.png'); background-repeat:repeat-x;", 
                    width = '100%%', contents = [
                        TR( [
                            TD( 
                                valign = 'top', width = '100%%', height = 109,
                                contents = statsPHP5)

#                Map(name = 'map')[,
#                Area(shape ='rect', coords='5,84,87,104', alt = 'AROS.org', href="http://www.aros.org"),
#                Area(shape ='rect', coords='96,83,177,104', alt = 'AROS-Exec', href="http://www.aros-exec.org"),
#                Area(shape ='rect', coords='183,84,263,104', alt = 'Team AROS', href="http://www.teamaros.org"),
#                Area(shape ='rect', coords='271,82,353,104', alt = 'AROS-Exec Archives', href="http://archives.aros-exec.org") ],
                            
                        ] ),
            
                        TR(
                            TD( 
                            Table(
                                border = 0, cellspacing = 0, cellpadding = 0,
                                width = '100%%', contents = [
                                    TR( contents = [
                                        TD(
                                            width = 171, contents = [ bar ], id = 'menusidebar'
                                        ),
                                        TD( width="100%%",
                                            contents =  '%(CONTENT)s'
                                        ),
                                    ]),
                                ]
                            )
                            )
                        ), 
                        TR( [
                            TD(
                                width = '100%%', valign = 'bottom', align = 'center',
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
    if file != '.' and file[-1] != '/':
        file += '.php'
    if lang != 'en':
        file = lang + '/' + file
    url = '%(BASE)s' + file

    # Add the section, if any
    if section != '':
        url += '#' + section

    return url

