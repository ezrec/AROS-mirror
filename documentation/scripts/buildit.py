# -*- coding: iso-8859-1 -*-
# Copyright © 2002-2012, The AROS Development Team. All rights reserved.
# $Id$

import os, sys, shutil, glob

import db.credits.parse
import db.credits.format.rest
import db.tasks.parse
import db.tasks.format.html

from docutils.core import Publisher
from docutils.io import NullOutput

from build.utility import *
from build.thumbnail import *

from template.www import makeTemplates
from template.www.gallery import *

import autodoc


# Setup
DEFAULTLANG= 'en'
SRCROOT    = os.path.abspath( '.' )
DSTROOT    = os.path.abspath( '../bin/documentation' )

TEMPLATE   = 'targets/www/template.html.'

TEMPLATE_DATA = {}

# languages supported by docutils:
LANGUAGES  = [ 'en', 'de', 'cs', 'el', 'es', 'fi', 'fr', 'it', 'nl', 'pl', 'pt', 'ru', 'sv' ]
# languages not supported by docutils yet (but that we have files written in):
# 'no'
# Languages to build (defaults to LANGUAGES):
languages= []


# altLang
# -------
# Returns the translation of the specified base name into the specified
# language, if it exists. Otherwise, returns the file name for DEFAULTLANG.

def altLang( base, lang, path='.'):
    langfile= base +'.' +lang
    if not(os.path.exists( os.path.join( path, langfile))):
        langfile= base +'.' +DEFAULTLANG

    return langfile


# pathAltLang
# -------
# Joins the result of altLang with the specified path.

def pathAltLang( base, lang, path='.'):
    return  os.path.join( path, altLang( base, lang, path))


def recurse( function, path='.', depth=0 ):
    for name in os.listdir( path ):
        name = os.path.join( path, name )

        if not ignore( name ):
            if os.path.isdir( name ):
                recurse( function, name, depth + 1 )
            else:
                function( name, depth )


def processPicture( src, depth ):
    FORMATS = [ 'jpg', 'jpeg', 'png' ]

    extension  = os.path.splitext( src )[1][1:]
    if extension not in FORMATS: return

    src     = os.path.normpath( src )
    dst_abs = os.path.normpath( os.path.join( DSTROOT, src ) )
    src_abs = os.path.normpath( os.path.join( SRCROOT, src ) )
    dst_dir = os.path.dirname( dst_abs )

    tn_dst     = makeThumbnailPath( src )
    tn_dst_abs = os.path.normpath( os.path.join( DSTROOT, tn_dst ) )
    tn_dst_dir = os.path.dirname( tn_dst_abs )

    # Make sure the destination directories exist.
    makedir( dst_dir )
    makedir( tn_dst_dir )

    # Copy the original image over.
    copy( src_abs, dst_abs )

    # Create the thumbnail.
    if newer( [ src_abs ], tn_dst_abs ):
        print '» Thumbnailing', src
        makeThumbnail( src_abs, tn_dst_abs, (200, 200) )


# makePictures
# ------------
# Creates picture galleries.

def makePictures():
    DIRECTORIES = [ 
        'pictures/developers', 
        'pictures/developers-together', 
        'pictures/screenshots' 
    ]

    options = [ '--report=none' ]

    # First, copy the pictures and generate thumbnails
    for root in DIRECTORIES:
        recurse( processPicture, root )

    # Second, create the galleries
    for root in DIRECTORIES:
        for lang in languages:
            output = convertWWW( pathAltLang( 'index', lang, root ), lang, options )

            names = os.listdir( root )
            names.sort()
            if root == 'pictures/screenshots':
                names.reverse()

            for name in names:
                path = os.path.join( root, name )
                if name == '.svn' or not os.path.isdir( path ): continue

                output += '<a name=%s>\n' % name
                output += convertWWW( pathAltLang( 'overview', lang, path), lang, options )

                for pictureName in os.listdir( path ):
                    picturePath = os.path.join( path, pictureName )
                    pictureFormat = os.path.splitext( pictureName )[1][1:]
                    if pictureName == '.svn' or os.path.isdir( picturePath ): continue
                    if pictureFormat not in [ 'jpg', 'jpeg', 'png' ]: continue

                    output += makePicture( 
                        picturePath,
                        convertWWW( pathAltLang( os.path.splitext( picturePath )[0], lang), lang, options ),
                        lang
                    )

                output += '</a>'

            if lang == DEFAULTLANG:
                strings = {
                    'ROOT'    : '../../',
                    'BASE'    : '../../',
                    'CONTENT' : output
                }
            else:
                strings = {
                    'ROOT'    : '../../../',
                    'BASE'    : '../../../',
                    'CONTENT' : output
                }

            filename = 'index.php'
            if lang == DEFAULTLANG:
                dst = os.path.join( DSTROOT, root )
            else:
                dst = os.path.join( DSTROOT, lang, root )
            if not os.path.exists( dst ):
                makedir( dst )
            file( 
		os.path.join( dst, filename ), 'w'
	    ).write( TEMPLATE_DATA[lang] % strings )


# makeStatus
# ----------
# Creates graphs of component implementation status.

def makeStatus( extension = '.php' ):
    tasks  = db.tasks.parse.parse( file( 'db/status', 'r' ) )
    for lang in languages:
        dstdir = 'introduction/status'
        if lang == DEFAULTLANG:
            dstdir = os.path.join( DSTROOT, dstdir )
        else:
            dstdir = os.path.join( DSTROOT, lang, dstdir )
        makedir( dstdir )
        db.tasks.format.html.format( tasks, dstdir, TEMPLATE_DATA[lang], lang, extension )


# makeNews
# --------
# Creates ReST files for current and archived news.
# Returns True is files were created or False if no correct news files were found

def makeNews():
    NEWS_SRC_DIR = os.path.join( SRCROOT, 'news/data' )
    NEWS_DST_DIR = os.path.join( SRCROOT, 'news/data' )
    NEWS_SRC_INDX= os.path.join( SRCROOT, 'news/index.' )
    NEWS_SRC_ARCH= os.path.join( SRCROOT, 'news/archive/')
    NOOFITEMS=     5

    news     = {}  # Lists of news, per language, to determine the recent news
    archives = {}  # Lists of news, per year per language, to list news per year

    # Get list of news and archive items for each language
    # Initialise the languages
    for lang in languages:
        news[lang] = []
        archives[lang] = {}

    # Do all the year directories
    for yeardirname in os.listdir( NEWS_SRC_DIR ):
        yeardirpath= os.path.join( NEWS_SRC_DIR, yeardirname)
        for lang in languages:
            archives[ lang][yeardirname]= list()

        # Do all the news item files (originals) in a specific year directory
        for filename in os.listdir( yeardirpath):
            date, ext= os.path.splitext( filename)
            if ext == '.en' and len( date ) == 8 and date.isdigit():
                year = date[:4]
                if year != yeardirname:
                    print 'Error: News item "' +date +'" found in news year "' +yeardirname +'".'

                # Generate a recent news source list and year news source lists for each language
                for lang in languages:
                    lang_filepath = os.path.join( yeardirpath, altLang( date, lang, yeardirpath))
                    news[lang].append( lang_filepath)
                    archives[lang][year].append( lang_filepath)
                    # Disadvantage of this approach is that sorting the lists will have to compare entire paths.

    # Check whether we actually found any news (we test for DEFAULTLANG, but all news lists have the same length)
    if not( len( news[ DEFAULTLANG])):
        return False

    # Generate news and archive ReST files
    for lang in languages:
        news[lang].sort( reverse= True)
        current = news[lang][:NOOFITEMS]
        _dst = NEWS_SRC_INDX +lang

        # Set up translated title dictionary
        config = ConfigParser()
        config.read( os.path.join( 'targets/www/template/languages', lang ) )
        _T = {}
        for option in config.options( 'titles' ):
            _T[option] = config.get( 'titles', option )

        # Create a recent news page
        if newer( current, _dst ):
            output  = file( _dst, 'w' )
            output.write( titleReST( _T['news']))
            for filepath in current:
                output.write( htmlReST( '   <a name="%s"></a>\n' % filepath[-11:-3])) # Not ideal, but at least legal HTML
                output.write( drctReST( 'include', filepath))
            output.close()

        # Create year news pages
        for year in archives[lang].keys():
            if len( archives[ lang][ year]):
                archives[lang][year].sort( reverse= True)
                _dst = os.path.join( NEWS_SRC_ARCH +year +'.' +lang)

                if newer( archives[lang][year], _dst ):
                    output = file( _dst, 'w' )
                    output.write( titleReST( _T[ 'news-archive-for'] +' ' + year))
                    for filepath in archives[lang][year]:
                        output.write( htmlReST( '   <a name="%s"></a>\n' % filepath[-11:-3])) # At least legal HTML
                        output.write( drctReST( 'include', filepath))
                    output.close()

    return True


# makeCredits
# -----------
# Creates ReST file for credits.

def makeCredits():
    if (not os.path.exists( 'credits.en' ) ) \
        or (os.path.getmtime( 'db/credits' ) > os.path.getmtime( 'credits.en' )):
        CREDITS_DATA = db.credits.format.rest.format( 
            db.credits.parse.parse( file( 'db/credits', 'r' ) )
        )
        file( 'credits.en', 'w' ).write( CREDITS_DATA )


# convertWWW
# ----------
# Converts a source file into an HTML string.

def convertWWW( src, language, options=None ):
    if language == 'el':
        encoding = 'iso-8859-7'
    elif language == 'pl':
        encoding = 'iso-8859-2'
    elif language == 'ru':
        encoding = 'windows-1251'
    elif language == 'cs':
        encoding = 'iso-8859-2'
    else:
        encoding = 'iso-8859-15'

    arguments = [
        '--no-generator',   '--language=' + language,
        '--no-source-link', '--no-datestamp',
        '--input-encoding=' + encoding,
        '--output-encoding=' + encoding,
        '--target-suffix=' + 'php',
        src, '' ]

    if options:
        for option in options:
            arguments.insert( 0, option )

    publisher = Publisher( destination_class = NullOutput )
    publisher.set_reader( 'standalone', None, 'restructuredtext' )
    publisher.set_writer( 'html' )
    publisher.publish( argv = arguments )

    return ''.join(
        publisher.writer.body_pre_docinfo +
        publisher.writer.body
    ).encode( encoding )


# processWWW
# ----------
# If src is an English source file, builds the corresponding web page for
# every supported language, using the translated source files. If a
# translation is unavailable for a particular language, the English version
# is used, but with the translated template. If src is not an English source
# file, nothing is done.

def processWWW( src, depth ):
    src     = os.path.normpath( src )

    prefix = os.path.splitext( src )[0]
    suffix = os.path.splitext( src )[1][1:]
    if suffix != DEFAULTLANG: return

    for lang in languages:
        if lang == DEFAULTLANG:
            dst = prefix + '.php'
            dst_depth = depth
        else:
            dst = lang + '/' + prefix + '.php'
            dst_depth = depth + 1
        src = altLang( prefix, lang )
        dst_abs = os.path.normpath( os.path.join( DSTROOT, dst ) )
        src_abs = os.path.normpath( os.path.join( SRCROOT, src ) )
        dst_dir = os.path.dirname( dst_abs )

        makedir( dst_dir )

        if newer( [ TEMPLATE + lang, src_abs ], dst_abs ):
            reportBuilding( dst )
            strings = {
                'ROOT'    : '../' * dst_depth,
                'BASE'    : '../' * dst_depth,
                'CONTENT' : convertWWW( src_abs, lang )
            }
            file( dst_abs, 'w').write( TEMPLATE_DATA[lang] % strings )
        else:
            reportSkipping( dst )


def processHTML( src, depth ):
    src    = os.path.normpath( src )

    prefix = os.path.splitext( src )[0]
    suffix = os.path.splitext( src )[1][1:]
    if suffix != DEFAULTLANG: return

    dst     = prefix + '.html' #.' + suffix
    dst_abs = os.path.normpath( os.path.join( DSTROOT, dst ) )
    src_abs = os.path.normpath( os.path.join( SRCROOT, src ) )
    dst_dir = os.path.dirname( dst_abs )

    makedir( dst_dir )

    if newer( [ src_abs ], dst_abs ):
        reportBuilding( src )
        arguments = [
            '--no-generator',   '--language=' + suffix,
            '--no-source-link', '--no-datestamp',
            '--output-encoding=iso-8859-15',
            '--target-suffix=html',
            '--stylesheet=' + '../' * depth + 'aros.css',
            '--link-stylesheet',
            src_abs, dst_abs
        ]

        publisher = Publisher()
        publisher.set_reader( 'standalone', None, 'restructuredtext' )
        publisher.set_writer( 'html' )
        publisher.publish( argv = arguments )
    else:
        reportSkipping( dst )


def copyImages():
    imagepath = 'documentation/developers/ui/images'
    dstpath   = os.path.join( DSTROOT, imagepath )
    srcpath   = imagepath
    
    makedir( dstpath )
    
    pathscopy(
        [
            'windows-prefs-titlebar.png',
            'windows-prefs-buttons.png'
        ],
        srcpath,
        dstpath
    )
    
    imagepath = 'documentation/developers/zune-dev/images'
    dstpath   = os.path.join( DSTROOT, imagepath )
    srcpath   = imagepath
    
    makedir( dstpath )
    
    pathscopy( 'hello.png', srcpath, dstpath)

    imagepath = 'images'
    dstpath   = os.path.join( DSTROOT, imagepath )
    srcpath   = imagepath
    
    makedir( dstpath )

    pathscopy(
        [
            'aros-banner.gif',
            'aros-banner2.png',
            'aros-banner-blue.png',
            'aros-banner-pb2.png',
            'aros-banner-peta.png',
            'aros-sigbar-user.png',
            'aros-sigbar-coder.png',
            'genesi.gif',
            'trustec.png',
            'sourceforge.png',
            'phoenix.jpeg',
            'bttr.jpeg',
            'icaroslive_logo.png',
            'aspireos_logo.png'
        ],
        srcpath,
        dstpath
    )


def copySamples():
    srcpath = os.path.join( "documentation", "developers", "samplecode")
    dstpath = os.path.join(DSTROOT, srcpath)
    shutil.rmtree(dstpath, True)
    copytree(srcpath, dstpath)


def copyHeaders():
    srcpath = os.path.join( "documentation", "developers", "headerfiles")
    dstpath = os.path.join(DSTROOT, srcpath)
    shutil.rmtree(dstpath, True)
    copytree(srcpath, dstpath)


def buildClean():
    shutil.rmtree( DSTROOT, True )

    filenames = glob.glob( 'credits.??' ) \
        + glob.glob( 'news/index.??' ) \
        + glob.glob( 'news/archive/20[0-9][0-9].??' )
    for filename in filenames:
        remove( filename )


def buildWWW():
    global DSTROOT ; DSTROOT = os.path.join( DSTROOT, 'www' )

    # Hack to get around dependency problems
    for lang in languages:
        if lang == DEFAULTLANG:
            dstpath = DSTROOT
        else:
            dstpath = os.path.join( DSTROOT, lang )
        remove( os.path.join( dstpath, 'index.php' ) )
        remove( os.path.join( dstpath, 'introduction/index.php' ) )
        remove( os.path.join( dstpath, 'download.php' ) )

    makeNews()
    makeTemplates()

    for lang in languages:
        TEMPLATE_DATA[lang] = file( TEMPLATE + lang, 'r' ).read()

    makePictures()
    makeStatus()

    recurse( processWWW )

    copy( 'license.html', DSTROOT )

    imagepath = os.path.join( DSTROOT, 'images' )
    makedir( imagepath )
    srcpath= 'targets/www/images'

    pathscopy(
        [
            'trustec-small.png',
            'genesi-small.gif',
            'noeupatents-small.png',
            'bullet.gif',
            'toplogomenu.png',
            'toplogomenu.gif',
            'kittymascot.png',
            'kittymascot.gif',
            'backgroundtop.png',
            'disk.png',
            'arosthubmain.png',
            'bgcolormain.png',
            'mainpagespacer.png',
            'rsfeed.gif',
            'sidespacer.png',
            'textdocu.gif',
            'archivedownloadicon.gif',
            'archivedownloadicon.png',
            'bgcolorright.png',
            'bountyicon1.gif',
            'bountyicon1.png',
            'bountyicon2.gif',
            'bountyicon2.png',
            'communityicon.gif',
            'communityicon.png',
            'directdownloadicon.gif',
            'directdownloadicon.png',
            'czechlogo.png',
            'englishlogo.png',
            'finlandlogo.png',
            'francelogo.png',
            'germanylogo.png',
            'greecelogo.png',
            'italylogo.png',
            'netherlandslogo.png',
            'polandlogo.png',
            'portugallogo.png',
            'rssicon1.gif',
            'rssicon1.png',
            'russialogo.png',
            'swedenlogo.png',
            'spanishlogo.png',            
            'pointer.png'
        ],
        srcpath,
        imagepath
    )

    copyImages()
    copySamples()
    copyHeaders()

    srcpath= 'targets/www'
    pathscopy(
        [
            'docutils.css',
            'aros.css',
        ],
        srcpath,
        DSTROOT
    )

    dbpath = os.path.join( DSTROOT, 'db' )
    makedir( dbpath )
    copy( 'db/quotes', dbpath )

    makedir( os.path.join( dbpath, 'download-descriptions' ) )
    for lang in languages:
        desc_file = os.path.join( 'db/download-descriptions', lang )
        if os.path.exists( desc_file ):
            copy( desc_file, os.path.join( dbpath, 'download-descriptions' ) )

    cgi_dest = os.path.join( DSTROOT, 'cgi-bin' )
    if os.path.exists( cgi_dest ):
        shutil.rmtree( cgi_dest )
    copytree( 'targets/www/cgi-bin', cgi_dest )

    thub_dest = os.path.join( DSTROOT, 'images/thubs' )
    if os.path.exists( thub_dest ):
       shutil.rmtree ( thub_dest )
    copytree( 'targets/www/images/thubs', thub_dest )


    rsfeed_dest = os.path.join( DSTROOT, 'rsfeed' )
    if os.path.exists( rsfeed_dest ):
       shutil.rmtree ( rsfeed_dest )
    copytree( 'targets/www/rsfeed', rsfeed_dest )   

    toolpath = os.path.join( DSTROOT, 'tools' )
    makedir( toolpath )

    srcpath= 'targets/www/tools'
    pathscopy( 
        [ 
            'password.html',
            'password.php', 
            'redirect.php' 
        ],
        srcpath,
        toolpath 
    )

    # Remove index-offline.php
    remove( os.path.join( DSTROOT, 'index-offline.php' ) )

    os.system( 'chmod -R go+r %s' % DSTROOT )


def buildHTML():
    global DSTROOT ; DSTROOT = os.path.join( DSTROOT, 'html' )
    global languages ; languages = [ DEFAULTLANG ]
    TEMPLATE_DATA[DEFAULTLANG] = file( 'targets/html/template.html.en', 'r' ).read()

    makeNews();

    if not os.path.exists( 'news/index.en' ):
        file( 'news/index.en', 'w' ).write( '' )
    recurse( processHTML )

    copyImages()
    copySamples()
    copyHeaders()

    srcpath= 'targets/www'
    pathscopy(
        [
            'docutils.css',
            'aros.css'
        ],
        srcpath,
        DSTROOT
    )

    copy( 'license.html', DSTROOT )

    # Make status
    makeStatus( '.html' )

    # Use index-offline as index
    remove( os.path.join( DSTROOT, 'index.html' ) )
    os.rename( os.path.join( DSTROOT, 'index-offline.html' ), os.path.join( DSTROOT, 'index.html' ) )

    os.system( 'chmod -R go+r %s' % DSTROOT )


TARGETS = {
    'clean':     buildClean,
    'www':       buildWWW,
    'html':      buildHTML,
    'alldocs':   autodoc.create_all_docs,
    'shelldocs': autodoc.create_shell_docs,
    'moduledocs':autodoc.create_module_docs,
    'appsdocs':  autodoc.create_apps_docs
}


# Usage: build {target|language}
# Target is  www, html, clean, alldocs, shelldocs, libdocs.
# The latter 3 update the documentation which is extracted from the source code.
# Specifying one or more languages only effects the www target. Only the
# specified language(s) is built. If no language is specified, all
# languages are built.

if __name__ == '__main__':
    targets= []
    valid= 1

    # Interpret arguments
    for arg in sys.argv[ 1:]:
        if   arg in LANGUAGES:
            languages.append( arg)
        elif TARGETS.has_key( arg):
            targets.append( arg)
        else:
            print 'Error: Unrecognised argument, "' + arg + '".'
            valid= 0

    if valid:
        # Fill in defaults if necessary
        if len( languages) == 0:
            languages= list( LANGUAGES)
        if len( targets) == 0:
            targets.append( 'www')

        makeCredits() # FIXME: Should probably be called for at most www and html only.

        # Build each target
        for target in targets:
            TARGETS[target]()

