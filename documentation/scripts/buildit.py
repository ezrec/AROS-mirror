# Settings

SRCROOT    = '.'
DSTROOT    = '../bin/documentation'

TEMPLATE   = 'targets/www/template.html.en'

# Languages not supported by docutils yet (but that we have files written in):
# 'no', 'sv'
LANGUAGES  = [ 'en' ]

# Setup global environment

import os, sys, shutil

import db.credits.parse
import db.credits.format.rest
import db.tasks.parse
import db.tasks.format.html

from docutils.core import publish
from docutils.core import Publisher
from docutils.io import NullOutput

from build.utility import *
from build.thumbnail import *

from template.www import makeTemplates

SRCROOT = os.path.abspath( SRCROOT )
DSTROOT = os.path.abspath( DSTROOT )

TEMPLATE_DATA = None

# FIXME: Move this somewhere else...
if (not os.path.exists( 'credits.en' ) ) \
    or (os.path.getmtime( 'db/credits' ) > os.path.getmtime( 'credits.en' )):
    CREDITS_DATA = db.credits.format.rest.format( 
        db.credits.parse.parse( file( 'db/credits', 'r' ) )
    )
    file( 'credits.en', 'w' ).write( CREDITS_DATA )


def extractAbstract( file ):
    result = '' 
    output = False

    for line in file:
        stripped = line.strip()
    
        if stripped == '.. START ABSTRACT':
            output = True
        elif stripped == '.. END ABSTRACT':
            output = False
        elif output:
            result += line

    return result

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
    DIRECTORIES = [ 'pictures/developers', 'pictures/screenshots' ]
    
    for path in DIRECTORIES:
        recurse( processPicture, path ) 
    

def makeStatus():
    dstdir = os.path.join( DSTROOT, 'introduction/status' )
    makedir( dstdir )

    tasks  = db.tasks.parse.parse( file( 'db/tasks', 'r' ) )
    db.tasks.format.html.format( tasks, dstdir )

    for path in os.listdir( dstdir ):
        strings = {
            'ROOT'    : '../../',
            'CONTENT' : file( os.path.join( dstdir, path ), 'r' ).read()
        }
        
        file( os.path.join( dstdir, '_' + path ), 'w' ).write( TEMPLATE_DATA % strings )
       

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
            output.write( file( filename, 'r' ).read() )
            output.write( '\n\n' )
        output.close()

    for year in archives.keys():
        archives[year].sort()
        archives[year].reverse()
        _dst = os.path.join( SRCROOT, 'news/archive/' + year + '.en' )
        
        if newer( archives[year], _dst ):
            output = file( _dst, 'w' )
            output.write( 'News archive for ' + year + '\n=====================\n\n' )
            for filename in archives[year]:
                output.write( file( filename, 'r' ).read() )
                output.write( '\n\n' )
            output.close()


def processWWW( src, depth ):
    src     = os.path.normpath( src )
    
    prefix = os.path.splitext( src )[0]
    suffix = os.path.splitext( src )[1][1:]
    if suffix not in LANGUAGES: return
    
    dst     = prefix + '.html.' + suffix
    dst_abs = os.path.normpath( os.path.join( DSTROOT, dst ) )
    src_abs = os.path.normpath( os.path.join( SRCROOT, src ) )
    dst_dir = os.path.dirname( dst_abs )
    
    makedir( dst_dir )
    
    _tmp_src = os.path.join( SRCROOT, 'targets/www/htaccess' )
    _tmp_dst = os.path.join( dst_dir, '.htaccess' )
    copy( _tmp_src, _tmp_dst )
    
    if newer( [ TEMPLATE, src_abs ], dst_abs ):
        reportBuilding( src )
        arguments = [
            '--no-generator',   '--language=' + suffix,
            '--no-source-link', '--no-datestamp', 
            '--output-encoding=iso-8859-1',
            src_abs, dst_abs ]
        publisher = Publisher( destination_class = NullOutput )
        publisher.set_reader( 'standalone', None, 'restructuredtext' )
        publisher.set_writer( 'html' )
        publisher.publish( argv = arguments )
        
        strings = {
            'ROOT'    : '../' * depth,
            'CONTENT' : ''.join \
            ( 
                publisher.writer.body_pre_docinfo + 
                publisher.writer.body 
            ).encode( 'iso-8859-1' )
        }
        
        file( dst_abs, 'w').write( TEMPLATE_DATA % strings )
        
    else:
        reportSkipping( dst )


def processHTML( src, depth ):
    src    = os.path.normpath( src )

    prefix = os.path.splitext( src )[0]
    suffix = os.path.splitext( src )[1][1:]
    if suffix not in LANGUAGES: return

    dst     = prefix + '.html.' + suffix
    dst_abs = os.path.normpath( os.path.join( DSTROOT, dst ) )
    src_abs = os.path.normpath( os.path.join( SRCROOT, src ) )
    dst_dir = os.path.dirname( dst_abs )
    
    makedir( dst_dir )
    
    if newer( [ src_abs ], dst_abs ):
        reportBuilding( src )
        arguments = \
        [
            '--no-generator',   '--language=' + suffix,
            '--no-source-link', '--no-datestamp',
            '--output-encoding=iso-8859-1',
            '--stylesheet=' + '../' * depth + 'aros.css',
            src_abs, dst_abs 
        ]

        publisher = Publisher()
        publisher.set_reader( 'standalone', None, 'restructuredtext' )
        publisher.set_writer( 'html' )
        publisher.publish( argv = arguments )
    else:
        reportSkipping( dst )


def buildClean():
    shutil.rmtree( DSTROOT )

def buildWWW():
    global DSTROOT ; DSTROOT = os.path.join( DSTROOT, 'www' )

    makePictures()
    makeNews()
    makeTemplates()
    
    global TEMPLATE_DATA
    TEMPLATE_DATA = file( TEMPLATE, 'r' ).read()

    makeStatus()

    _src_1 = os.path.join( SRCROOT, 'introduction/index.en' )
    _src_2 = os.path.join( SRCROOT, 'documentation/developers/contribute.en' )
    _src_3 = os.path.join( SRCROOT, 'news/index.en' )
    _dst   = os.path.join( SRCROOT, 'index.en' )

    if newer( [ _src_1, _src_2, _src_3 ], _dst ):
        text  = extractAbstract( file( _src_1, 'r' ) )
        text += '\n`Read more...`__\n\n__ introduction\n\n'
        text += extractAbstract( file( _src_2, 'r' ) )
        text += '\n`Read more...`__\n\n__ documentation/developers/contribute\n\n'
        text += file( _src_3, 'r' ).read()
        file( _dst, 'w' ).write( text )

    recurse( processWWW )

    imagepath = os.path.join( DSTROOT, 'images' ) 
    makedir( imagepath )

    copy
    ( [ \
        'images/trustec.png',
        'images/sourceforge.png',
        'images/amigados-online-reference-manual.png', )
        'targets/www/images/trustec-small.png',
        'targets/www/images/bullet.png', 
        'targets/www/images/aros.png'
      ], 
      imagepath 
    )
    
    copy( 'targets/www/aros.css', DSTROOT )
    
    toolpath = os.path.join( DSTROOT, 'tools' )
    makedir( toolpath )

    copy 
    ( [ \
        'targets/www/tools/password.html',
        'targets/www/tools/password.php', 
        'targets/www/tools/redirect.php' 
      ], 
      toolpath 
    )


def buildHTML():
    global DSTROOT ; DSTROOT = os.path.join( DSTROOT, 'html' )

    recurse( processHTML )

    copy( 'targets/html/aros.css', DSTROOT )


targets = \
{
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
