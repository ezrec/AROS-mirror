# -*- coding: iso-8859-1 -*-
# Copyright © 2002-2003, The AROS Development Team. All rights reserved.
# $Id$

import os, sys, shutil

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

# Setup
SRCROOT    = os.path.abspath( '.' )
DSTROOT    = os.path.abspath( '../bin/documentation' )

TEMPLATE   = 'targets/www/template.html.en'

TEMPLATE_DATA = None

# Languages not supported by docutils yet (but that we have files written in):
# 'no'
LANGUAGES = [ 'en' ]
#LANGUAGES  = [ 'en', 'sv', 'de' ]

# FIXME: Move this somewhere else...
if (not os.path.exists( 'credits.en' ) ) \
    or (os.path.getmtime( 'db/credits' ) > os.path.getmtime( 'credits.en' )):
    CREDITS_DATA = db.credits.format.rest.format( 
        db.credits.parse.parse( file( 'db/credits', 'r' ) )
    )
    file( 'credits.en', 'w' ).write( CREDITS_DATA )

def recurse( function, path='.', depth=0 ):
    for name in os.listdir( path ):
        name = os.path.join( path, name )

        if not ignore( name ):
            if os.path.isdir( name ):
                recurse( function, name, depth + 1 )
            else:
                function( name, depth )

def processPicture( src, depth ):
    FORMATS = [ 'jpeg', 'png' ]
    
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
        output = convertWWW( os.path.join( root, 'index.en' ), 'en', options )
        
        names = os.listdir( root )
        names.sort()
        if root == 'pictures/screenshots':
            names.reverse() 
        
        for name in names:
            path = os.path.join( root, name )
            if name == 'CVS' or not os.path.isdir( path ): continue 

            output += convertWWW( os.path.join( path, 'overview.en' ), 'en', options )

            for pictureName in os.listdir( path ):
                picturePath = os.path.join( path, pictureName )
                pictureFormat = os.path.splitext( pictureName )[1][1:]
                if pictureName == 'CVS' or os.path.isdir( picturePath ): continue 
                if pictureFormat not in [ 'png', 'jpeg' ]: continue
                
                output += makePicture( 
                    picturePath, 
                    convertWWW( os.path.splitext( picturePath )[0] + '.en', 'en', options )
                )
        
        strings = {
            'ROOT'    : '../../',
            'BASE'    : '../../',
            'CONTENT' : output
        }
        
        file( 
            os.path.join( DSTROOT, root, 'index.php' ), 'w'
        ).write( TEMPLATE_DATA % strings )


def makeStatus():
    dstdir = os.path.join( DSTROOT, 'introduction/status' )
    makedir( dstdir )

    tasks  = db.tasks.parse.parse( file( 'db/tasks', 'r' ) )
    db.tasks.format.html.format( tasks, dstdir, TEMPLATE_DATA )


def makeNews():
    NEWS_SRC_DIR = os.path.join( SRCROOT, 'news/data' )
    NEWS_DST_DIR = os.path.join( SRCROOT, 'news/data' )

    news     = []
    archives = {}

    for filename in os.listdir( NEWS_SRC_DIR ):
        if len( filename ) >= 4:
            language = filename[-2:]
            date     = filename[:-3]

            if language in LANGUAGES and len( date ) == 8 and date.isdigit():
                news.append( os.path.join( NEWS_SRC_DIR, filename ) )
                year = date[:4]

                if not archives.has_key( year ):
                    archives[year] = list()

                archives[year].append( os.path.join( NEWS_SRC_DIR, filename ) )

    news.sort()
    news.reverse()
    current = news[:5]
    _dst = os.path.join( SRCROOT, 'news/index.en' )

    if newer( current, _dst ):
        output  = file( _dst, 'w' )
        output.write( 'News\n====\n\n' )
        for filename in current:
            output.write( '.. raw:: html\n\n   <a name="%s">\n' % filename[-11:-3])
            output.write( '.. include:: %s\n' % filename )
            output.write( '.. raw:: html\n\n   </a>\n\n\n')
        output.close()

    for year in archives.keys():
        archives[year].sort()
        archives[year].reverse()
        _dst = os.path.join( SRCROOT, 'news/archive/' + year + '.en' )

        if newer( archives[year], _dst ):
            output = file( _dst, 'w' )
            output.write( 'News archive for ' + year + '\n=====================\n\n' )
            for filename in archives[year]:
                output.write( '.. raw:: html\n\n   <a name="%s">\n' % filename[-11:-3])
                output.write( '.. include:: %s\n' % filename )
                output.write( '.. raw:: html\n\n   </a>\n\n\n')
            output.close()

def convertWWW( src, language, options=None ):
    arguments = [
        '--no-generator',   '--language=' + language,
        '--no-source-link', '--no-datestamp',
        '--output-encoding=iso-8859-1',
        '--target-suffix=php',
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
    ).encode( 'iso-8859-1' )

def processWWW( src, depth ):
    src     = os.path.normpath( src )

    prefix = os.path.splitext( src )[0]
    suffix = os.path.splitext( src )[1][1:]
    if suffix not in LANGUAGES: return

    dst     = prefix + '.php'
    dst_abs = os.path.normpath( os.path.join( DSTROOT, dst ) )
    src_abs = os.path.normpath( os.path.join( SRCROOT, src ) )
    dst_dir = os.path.dirname( dst_abs )

    makedir( dst_dir )

    if newer( [ TEMPLATE, src_abs ], dst_abs ):
        reportBuilding( src )
        strings = {
            'ROOT'    : '../' * depth,
            'BASE'    : '../' * depth,
            'CONTENT' : convertWWW( src_abs, suffix )
        }

        file( dst_abs, 'w').write( TEMPLATE_DATA % strings )

    else:
        reportSkipping( dst )


def processHTML( src, depth ):
    src    = os.path.normpath( src )

    prefix = os.path.splitext( src )[0]
    suffix = os.path.splitext( src )[1][1:]
    if suffix not in LANGUAGES: return

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
            '--output-encoding=iso-8859-1',
            '--target-suffix=html',
            '--stylesheet=' + '../' * depth + 'aros.css',
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
    
    copy(
        [
            os.path.join( srcpath, 'windows-prefs-titlebar.png' ),
            os.path.join( srcpath, 'windows-prefs-buttons.png' )
        ],
        dstpath
    )
    
    imagepath = 'images'
    dstpath   = os.path.join( DSTROOT, imagepath )
    srcpath   = imagepath
    
    makedir( dstpath )

    copy(
        [
            os.path.join( srcpath, 'aros-banner.gif' ),
            os.path.join( srcpath, 'genesi.gif' ),
            os.path.join( srcpath, 'trustec.png' ),
            os.path.join( srcpath, 'sourceforge.png' ),
            os.path.join( srcpath, 'phoenix.jpeg' ),
            os.path.join( srcpath, 'bttr.jpeg' ),
            os.path.join( srcpath, 'amigados-online-reference-manual.png' )
        ],
        dstpath
    )
            

def buildClean():
    shutil.rmtree( DSTROOT )

def buildWWW():
    global DSTROOT ; DSTROOT = os.path.join( DSTROOT, 'www' )

    makeNews()
    makeTemplates()

    global TEMPLATE_DATA
    TEMPLATE_DATA = file( TEMPLATE, 'r' ).read()

    makePictures()
    #makeStatus()

    recurse( processWWW )

    copy( 'license.html', DSTROOT )

    imagepath = os.path.join( DSTROOT, 'images' )
    makedir( imagepath )

    copy(
        [
            'targets/www/images/trustec-small.png',
            'targets/www/images/genesi-small.gif',
            'targets/www/images/noeupatents-small.png',
            'targets/www/images/bullet.gif',
            'targets/www/images/aros.png',
            'targets/www/images/aros.png',
            'targets/www/images/aros_text.png',
            'targets/www/images/bright_bg.png',
            'targets/www/images/dark_bg.png',
            'targets/www/images/h_arc.png',
            'targets/www/images/horizontal_border.png',
            'targets/www/images/kitty_1.png',
            'targets/www/images/kitty_2.png',
            'targets/www/images/kitty_3.png',
            'targets/www/images/v_arc.png',
            'targets/www/images/vertical_border.png',
            'targets/www/images/spacer.gif',
            'targets/www/images/disk.png'
        ],
        imagepath
    )
    
    copyImages()
    
    copy( 'targets/www/docutils.css', DSTROOT )
    copy( 'targets/www/aros.css', DSTROOT )

    dbpath = os.path.join( DSTROOT, 'db' )
    makedir( dbpath )
    
    copy( 'db/quotes', dbpath )

    toolpath = os.path.join( DSTROOT, 'tools' )
    makedir( toolpath )

    copy( 
        [ 
            'targets/www/tools/password.html',
            'targets/www/tools/password.php', 
            'targets/www/tools/redirect.php' 
        ], 
        toolpath 
    )
    
    os.system( 'chmod -R go+r %s' % DSTROOT )

def buildHTML():
    global DSTROOT ; DSTROOT = os.path.join( DSTROOT, 'html' )

    recurse( processHTML )

    copyImages()

    copy( 'targets/www/docutils.css', DSTROOT )
    copy( 'targets/html/aros.css', DSTROOT )
    
    copy( 'license.html', DSTROOT )

    os.system( 'chmod -R go+r %s' % DSTROOT )


targets = {
    'clean': buildClean,
    'www':   buildWWW,
    'html':  buildHTML
}

if __name__ == '__main__':
    target = 'www' # Set default target
    if len( sys.argv ) > 1:
        target = sys.argv[1]

    if targets.has_key( target ):
        targets[target]()
    else:
        print 'Error: No such target, "' + target + '".'
