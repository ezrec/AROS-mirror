import os.path
import Image

def makeThumbnailPath( originalPath ):
    head, tail = os.path.split( originalPath )
    return os.path.join( head, 'thumbnails', tail )

def makeThumbnail( src, dst, size ):
    image = Image.open( src )
    image = image.convert( 'RGB' )
    image.thumbnail( size, Image.ANTIALIAS )
    image.save( dst )

