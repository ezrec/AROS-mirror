/* $Id$ */

/*
 * Mesa 3-D graphics library
 * Version:  3.0
 * Copyright (C) 1995-1998  Brian Paul
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


/*
 * $Log$
 * Revision 1.1  2005/01/11 14:58:31  NicJA
 * AROSMesa 3.0
 *
 * - Based on the official mesa 3 code with major patches to the amigamesa driver code to get it working.
 * - GLUT not yet started (ive left the _old_ mesaaux, mesatk and demos in for this reason)
 * - Doesnt yet work - the _db functions seem to be writing the data incorrectly, and color picking also seems broken somewhat - giving most things a blue tinge (those that are currently working)
 *
 * Revision 3.15  1998/08/21 02:41:39  brianp
 * added gl_pack/unpack_polygon_stipple()
 *
 * Revision 3.14  1998/08/06 01:38:47  brianp
 * removed DEFARRAY/UNDEFARRAY from gl_pack_rgba_span()
 *
 * Revision 3.13  1998/08/01 04:53:23  brianp
 * bitmap unpacking didn't distinguish GL_COLOR_INDEX from GL_STENCIL_INDEX
 *
 * Revision 3.12  1998/07/26 17:24:18  brianp
 * replaced const with CONST because of IRIX cc warning
 *
 * Revision 3.11  1998/07/18 03:36:41  brianp
 * removed some debugging code
 *
 * Revision 3.10  1998/07/18 03:33:17  brianp
 * GL_INT type wasn't implemented
 *
 * Revision 3.9  1998/07/17 03:24:53  brianp
 * added gl_pack_rgba_span()
 *
 * Revision 3.8  1998/06/14 15:23:08  brianp
 * don't bit-flip bytes for GLubyte images
 *
 * Revision 3.7  1998/05/05 00:19:06  brianp
 * added GL_COLOR_INDEXxx_EXT cases to gl_components_in_format()
 *
 * Revision 3.6  1998/03/28 03:57:58  brianp
 * fixed minor IRIX cc warning
 *
 * Revision 3.5  1998/03/27 03:37:40  brianp
 * fixed G++ warnings
 *
 * Revision 3.4  1998/03/15 18:50:25  brianp
 * added GL_EXT_abgr extension
 *
 * Revision 3.3  1998/02/08 20:21:09  brianp
 * fixed bitmap unpacking error
 *
 * Revision 3.2  1998/02/01 22:29:09  brianp
 * added support for packed pixel formats
 *
 * Revision 3.1  1998/02/01 20:47:42  brianp
 * added GL_BGR and GL_BGRA pixel formats
 *
 * Revision 3.0  1998/01/31 20:54:19  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "context.h"
#include "image.h"
#include "macros.h"
#include "pixel.h"
#include "types.h"
#endif



/*
 * Flip the 8 bits in each byte of the given array.
 */
void gl_flip_bytes( GLubyte *p, GLuint n )
{
   register GLuint i, a, b;

   for (i=0;i<n;i++) {
      b = (GLuint) p[i];
      a = ((b & 0x01) << 7) |
	  ((b & 0x02) << 5) |
	  ((b & 0x04) << 3) |
	  ((b & 0x08) << 1) |
	  ((b & 0x10) >> 1) |
	  ((b & 0x20) >> 3) |
	  ((b & 0x40) >> 5) |
	  ((b & 0x80) >> 7);
      p[i] = (GLubyte) a;
   }
}


/*
 * Flip the order of the 2 bytes in each word in the given array.
 */
void gl_swap2( GLushort *p, GLuint n )
{
   register GLuint i;

   for (i=0;i<n;i++) {
      p[i] = (p[i] >> 8) | ((p[i] << 8) & 0xff00);
   }
}



/*
 * Flip the order of the 4 bytes in each word in the given array.
 */
void gl_swap4( GLuint *p, GLuint n )
{
   register GLuint i, a, b;

   for (i=0;i<n;i++) {
      b = p[i];
      a =  (b >> 24)
	| ((b >> 8) & 0xff00)
	| ((b << 8) & 0xff0000)
	| ((b << 24) & 0xff000000);
      p[i] = a;
   }
}




/*
 * Return the size, in bytes, of the given GL datatype.
 * Return 0 if GL_BITMAP.
 * Return -1 if invalid type enum.
 */
GLint gl_sizeof_type( GLenum type )
{
   switch (type) {
      case GL_BITMAP:
	 return 0;
      case GL_UNSIGNED_BYTE:
         return sizeof(GLubyte);
      case GL_BYTE:
	 return sizeof(GLbyte);
      case GL_UNSIGNED_SHORT:
	 return sizeof(GLushort);
      case GL_SHORT:
	 return sizeof(GLshort);
      case GL_UNSIGNED_INT:
	 return sizeof(GLuint);
      case GL_INT:
	 return sizeof(GLint);
      case GL_FLOAT:
	 return sizeof(GLfloat);
      default:
         return -1;
   }
}


/*
 * Same as gl_sizeof_packed_type() but we also accept the
 * packed pixel format datatypes.
 */
GLint gl_sizeof_packed_type( GLenum type )
{
   switch (type) {
      case GL_BITMAP:
	 return 0;
      case GL_UNSIGNED_BYTE:
         return sizeof(GLubyte);
      case GL_BYTE:
	 return sizeof(GLbyte);
      case GL_UNSIGNED_SHORT:
	 return sizeof(GLushort);
      case GL_SHORT:
	 return sizeof(GLshort);
      case GL_UNSIGNED_INT:
	 return sizeof(GLuint);
      case GL_INT:
	 return sizeof(GLint);
      case GL_FLOAT:
	 return sizeof(GLfloat);
      case GL_UNSIGNED_BYTE_3_3_2:
         return sizeof(GLubyte);
      case GL_UNSIGNED_BYTE_2_3_3_REV:
         return sizeof(GLubyte);
      case GL_UNSIGNED_SHORT_5_6_5:
         return sizeof(GLshort);
      case GL_UNSIGNED_SHORT_5_6_5_REV:
         return sizeof(GLshort);
      case GL_UNSIGNED_SHORT_4_4_4_4:
         return sizeof(GLshort);
      case GL_UNSIGNED_SHORT_4_4_4_4_REV:
         return sizeof(GLshort);
      case GL_UNSIGNED_SHORT_5_5_5_1:
         return sizeof(GLshort);
      case GL_UNSIGNED_SHORT_1_5_5_5_REV:
         return sizeof(GLshort);
      case GL_UNSIGNED_INT_8_8_8_8:
         return sizeof(GLuint);
      case GL_UNSIGNED_INT_8_8_8_8_REV:
         return sizeof(GLuint);
      case GL_UNSIGNED_INT_10_10_10_2:
         return sizeof(GLuint);
      case GL_UNSIGNED_INT_2_10_10_10_REV:
         return sizeof(GLuint);
      default:
         return -1;
   }
}



/*
 * Return the number of components in a GL enum pixel type.
 * Return -1 if bad format.
 */
GLint gl_components_in_format( GLenum format )
{
   switch (format) {
      case GL_COLOR_INDEX:
      case GL_COLOR_INDEX1_EXT:
      case GL_COLOR_INDEX2_EXT:
      case GL_COLOR_INDEX4_EXT:
      case GL_COLOR_INDEX8_EXT:
      case GL_COLOR_INDEX12_EXT:
      case GL_COLOR_INDEX16_EXT:
      case GL_STENCIL_INDEX:
      case GL_DEPTH_COMPONENT:
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_LUMINANCE:
         return 1;
      case GL_LUMINANCE_ALPHA:
	 return 2;
      case GL_RGB:
	 return 3;
      case GL_RGBA:
	 return 4;
      case GL_BGR:
	 return 3;
      case GL_BGRA:
	 return 4;
      case GL_ABGR_EXT:
         return 4;
      /* SPECIAL CASES: */
      case GL_UNSIGNED_BYTE_3_3_2:
      case GL_UNSIGNED_BYTE_2_3_3_REV:
      case GL_UNSIGNED_SHORT_5_6_5:
      case GL_UNSIGNED_SHORT_5_6_5_REV:
      case GL_UNSIGNED_SHORT_4_4_4_4:
      case GL_UNSIGNED_SHORT_4_4_4_4_REV:
      case GL_UNSIGNED_SHORT_5_5_5_1:
      case GL_UNSIGNED_SHORT_1_5_5_5_REV:
      case GL_UNSIGNED_INT_8_8_8_8:
      case GL_UNSIGNED_INT_8_8_8_8_REV:
      case GL_UNSIGNED_INT_10_10_10_2:
      case GL_UNSIGNED_INT_2_10_10_10_REV:
         return 1;
      default:
         return -1;
   }
}


/*
 * Return the address of a pixel in an image (actually a volume).
 * Pixel unpacking/packing parameters are observed according to 'packing'.
 * Input:  image - start of image data
 *         width, height - size of image
 *         format - image format
 *         type - pixel component type
 *         packing - GL_TRUE = use packing params
 *                   GL_FALSE = use unpacking params.
 *         img - which image in the volume (0 for 2-D images)
 *         row, column - location of pixel in the image
 * Return:  address of pixel at (image,row,column) in image or NULL if error.
 */
GLvoid *gl_pixel_addr_in_image( const struct gl_pixelstore_attrib *packing,
                                const GLvoid *image, GLsizei width,
                                GLsizei height, GLenum format, GLenum type,
                                GLint img, GLint row, GLint column )
{
   GLint bytes_per_comp;   /* bytes per component */
   GLint comp_per_pixel;   /* components per pixel */
   GLint comps_per_row;    /* components per row */
   GLint pixels_per_row;   /* pixels per row */
   GLint bytes_per_image;
   GLint rows_per_image;
   GLint alignment;        /* 1, 2 or 4 */
   GLint skiprows;
   GLint skippixels;
   GLint skipimages;       /* for 3-D */
   GLubyte *pixel_addr;

   /* Compute bytes per component */
   bytes_per_comp = gl_sizeof_packed_type( type );
   if (bytes_per_comp<0) {
      return NULL;
   }

   /* Compute number of components per pixel */
   comp_per_pixel = gl_components_in_format( format );
   if (comp_per_pixel<0 && type != GL_BITMAP) {
      return NULL;
   }

   alignment = packing->Alignment;
   if (packing->RowLength > 0) {
      pixels_per_row = packing->RowLength;
   }
   else {
      pixels_per_row = width;
   }
   if (packing->ImageHeight>0) {
      rows_per_image = packing->ImageHeight;
   }
   else {
      rows_per_image = height;
   }
   skiprows = packing->SkipRows;
   skippixels = packing->SkipPixels;
   skipimages = packing->SkipImages;

   if (type==GL_BITMAP) {
      /* BITMAP data */
      GLint bytes_per_row;

      bytes_per_row = alignment
                    * CEILING( comp_per_pixel*pixels_per_row, 8*alignment );

      bytes_per_image = bytes_per_row * rows_per_image;

      pixel_addr = (GLubyte *) image
                 + (skipimages + img) * bytes_per_image
                 + (skiprows + row) * bytes_per_row
                 + (skippixels + column) / 8;
   }
   else {
      /* Non-BITMAP data */

      if (bytes_per_comp>=alignment) {
	 comps_per_row = comp_per_pixel * pixels_per_row;
      }
      else {
         GLint bytes_per_row = bytes_per_comp * comp_per_pixel
                             * pixels_per_row;

	 comps_per_row = alignment / bytes_per_comp
                       * CEILING( bytes_per_row, alignment );
      }

      bytes_per_image = bytes_per_comp * comps_per_row * rows_per_image;

      /* Copy/unpack pixel data to buffer */
      pixel_addr = (GLubyte *) image
                 + (skipimages + img) * bytes_per_image
                 + (skiprows + row) * bytes_per_comp * comps_per_row
                 + (skippixels + column) * bytes_per_comp * comp_per_pixel;
   }

   return (GLvoid *) pixel_addr;
}



/*
 * Allocate a new gl_image.  All fields are initialized to zero.
 */
static struct gl_image *alloc_image( void )
{ 
   return (struct gl_image *) calloc(sizeof(struct gl_image), 1);
}



/*
 * Allocate a new gl_image with the error flag set.
 */
static struct gl_image *alloc_error_image( GLint width, GLint height,
                                           GLint depth, GLenum format,
                                           GLenum type )
{
   struct gl_image *image = alloc_image();
   if (image) {
      image->Width = width;
      image->Height = height;
      image->Depth = depth;
      image->Format = format;
      image->Type = type;
      image->ErrorFlag = GL_TRUE;
   }
   return image;
}



/* 
 * Free a gl_image.
 */
void gl_free_image( struct gl_image *image )
{
   if (image->Data) {
      free(image->Data);
   }
   free(image);
}



/*
 * Do error checking on an image.  If there's an error, register it and
 * return GL_TRUE, else return GL_FALSE.
 */
GLboolean gl_image_error_test( GLcontext *ctx, const struct gl_image *image,
                               const char *msg )
{
   assert(image);
   if (image->Width <= 0 || image->Height <= 0 || image->Depth <= 0) {
      gl_error( ctx, GL_INVALID_VALUE, msg );
      return GL_TRUE;
   }
   else {
      return GL_FALSE;
   }
}



/*
 * Unpack a depth-buffer image storing values as GLshort, GLuint, or GLfloats.
 * Input:  type - datatype of src depth image
 * Return pointer to a new gl_image structure.
 *
 * Notes:  if the source image type is GLushort then the gl_image will
 * also store GLushorts.  If the src image type is GLuint then the gl_image
 * will also store GLuints.  For all other src image types the gl_image
 * will store GLfloats.  The integer cases can later be optimized.
 */
static struct gl_image *unpack_depth_image( GLcontext *ctx, GLenum type,
                                            GLint width, GLint height,
                                            const GLvoid *pixels )
{
      struct gl_image *image;
   GLfloat *fDst;
   GLushort *sDst;
   GLuint *iDst;
   GLint i, j;

   image = alloc_image();
   if (image) {
      image->Width = width;
      image->Height = height;
      image->Depth = 1;
      image->Components = 1;
      image->Format = GL_DEPTH_COMPONENT;
      if (type==GL_UNSIGNED_SHORT) {
         image->Type = GL_UNSIGNED_SHORT;
         image->Data = malloc( width * height * sizeof(GLushort));
      }
      else if (type==GL_UNSIGNED_INT) {
         image->Type = GL_UNSIGNED_INT;
         image->Data = malloc( width * height * sizeof(GLuint));
      }
      else {
         image->Type = GL_FLOAT;
         image->Data = malloc( width * height * sizeof(GLfloat));
      }
      image->RefCount = 0;
      if (!image->Data)
         return image;
   }
   else {
            return NULL;
         }

   fDst = (GLfloat *) image->Data;
   sDst = (GLushort *) image->Data;
   iDst = (GLuint *) image->Data;

   for (i=0;i<height;i++) {
               GLvoid *src = gl_pixel_addr_in_image( &ctx->Unpack, pixels,
                                                     width, height,
                                            GL_DEPTH_COMPONENT, type,
                                            0, i, 0 );
               if (!src) {
         return image;
      }

      switch (type) {
         case GL_BYTE:
            assert(image->Type == GL_FLOAT);
            for (j=0; j<width; j++) {
               *fDst++ = BYTE_TO_FLOAT(((GLbyte*)src)[j]);
               }
            break;
         case GL_UNSIGNED_BYTE:
            assert(image->Type == GL_FLOAT);
            for (j=0; j<width; j++) {
               *fDst++ = UBYTE_TO_FLOAT(((GLubyte*)src)[j]);
            }
            break;
         case GL_UNSIGNED_SHORT:
            assert(image->Type == GL_UNSIGNED_SHORT);
            MEMCPY( sDst, src, width * sizeof(GLushort) );
            if (ctx->Unpack.SwapBytes) {
               gl_swap2( sDst, width );
         }
            sDst += width;
            break;
         case GL_SHORT:
            assert(image->Type == GL_FLOAT);
            if (ctx->Unpack.SwapBytes) {
               for (j=0;j<width;j++) {
                  GLshort value = ((GLshort*)src)[j];
                  value = ((value >> 8) & 0xff) | ((value&0xff) << 8);
                  *fDst++ = SHORT_TO_FLOAT(value);
         }
      }
      else {
               for (j=0;j<width;j++) {
                  *fDst++ = SHORT_TO_FLOAT(((GLshort*)src)[j]);
      }
      }
            break;
         case GL_INT:
            assert(image->Type == GL_FLOAT);
            if (ctx->Unpack.SwapBytes) {
               for (j=0;j<width;j++) {
                  GLint value = ((GLint*)src)[j];
                  value = ((value >> 24) & 0x000000ff) |
                          ((value >> 8)  & 0x0000ff00) |
                          ((value << 8)  & 0x00ff0000) |
                          ((value << 24) & 0xff000000);
                  *fDst++ = INT_TO_FLOAT(value);
      }
   }
            else {
               for (j=0;j<width;j++) {
                  *fDst++ = INT_TO_FLOAT(((GLint*)src)[j]);
   }
   }
            iDst += width;
            break;
         case GL_UNSIGNED_INT:
            assert(image->Type == GL_UNSIGNED_INT);
            MEMCPY( iDst, src, width * sizeof(GLuint) );
            if (ctx->Unpack.SwapBytes) {
               gl_swap4( iDst, width );
      }
            iDst += width;
            break;
         case GL_FLOAT:
            assert(image->Type == GL_FLOAT);
            MEMCPY( fDst, src, width * sizeof(GLfloat) );
            if (ctx->Unpack.SwapBytes) {
               gl_swap4( (GLuint*) fDst, width );
            }
            fDst += width;
            break;
         default:
            gl_problem(ctx, "unpack_depth_image type" );
            return image;
         }
      }

   return image;
}



/*
 * Unpack a stencil image.  Store as GLubytes in a gl_image structure.
 * Return:  pointer to new gl_image structure.
 */
static struct gl_image *unpack_stencil_image( GLcontext *ctx, GLenum type,
                                              GLint width, GLint height,
                                              const GLvoid *pixels )
{
   struct gl_image *image;
   GLubyte *dst;
   GLint i, j;

   assert(sizeof(GLstencil) == sizeof(GLubyte));

   image = alloc_image();
      if (image) {
         image->Width = width;
         image->Height = height;
      image->Depth = 1;
      image->Components = 1;
      image->Format = GL_STENCIL_INDEX;
         image->Type = GL_UNSIGNED_BYTE;
      image->Data = malloc( width * height * sizeof(GLubyte));
         image->RefCount = 0;
      if (!image->Data)
         return image;
      }
      else {
         return NULL;
      }

   dst = (GLubyte *) image->Data;

         for (i=0;i<height;i++) {
            GLvoid *src = gl_pixel_addr_in_image( &ctx->Unpack, pixels,
                                                  width, height,
                                            GL_STENCIL_INDEX, type,
                                            0, i, 0 );
            if (!src) {
         return image;
            }

      switch (type) {
               case GL_UNSIGNED_BYTE:
         case GL_BYTE:
            MEMCPY( dst, src, width * sizeof(GLubyte) );
            dst += width * sizeof(GLubyte);
            break;
         case GL_UNSIGNED_SHORT:
         case GL_SHORT:
            if (ctx->Unpack.SwapBytes) {
               /* grab upper byte */
               for (j=0; j < width; j++) {
                  *dst++ = (((GLushort*)src)[j] & 0xff00) >> 8;
                     }
                  }
                  else {
               for (j=0; j < width; j++) {
                  *dst++ = (((GLushort*)src)[j]) & 0xff;
                  }
                  }
                  break;
         case GL_INT:
            if (ctx->Unpack.SwapBytes) {
               /* grab upper byte */
               for (j=0; j < width; j++) {
                  *dst++ = (((GLuint*)src)[j] & 0xff000000) >> 8;
                     }
                  }
                  else {
               for (j=0; j < width; j++) {
                  *dst++ = (((GLuint*)src)[j]) & 0xff;
                  }
                  }
                  break;
         case GL_UNSIGNED_INT:
                  if (ctx->Unpack.SwapBytes) {
               /* grab upper byte */
               for (j=0; j < width; j++) {
                  *dst++ = (((GLuint*)src)[j] & 0xff000000) >> 8;
               }
                        }
                        else {
               for (j=0; j < width; j++) {
                  *dst++ = (((GLuint*)src)[j]) & 0xff;
               }
            }
            break;
         case GL_FLOAT:
            if (ctx->Unpack.SwapBytes) {
               for (j=0; j < width; j++) {
                  GLfloat fvalue;
                  GLint value = ((GLuint*)src)[j];
                  value = ((value & 0xff000000) >> 24)
                     | ((value & 0x00ff0000) >> 8)
                     | ((value & 0x0000ff00) << 8)
                     | ((value & 0x000000ff) << 24);
                  fvalue = *((GLfloat*) &value);
                  *dst++ = ((GLint) fvalue) & 0xff;
               }
            }
            else {
               for (j=0; j < width; j++) {
                  GLfloat fvalue = ((GLfloat *)src)[j];
                  *dst++ = ((GLint) fvalue) & 0xff;
               }
            }
            break;
         default:
            gl_problem(ctx, "unpack_stencil_image type" );
            return image;
      }
   }

   return image;
}



/*
 * Unpack a bitmap, return a new gl_image struct.
 */
static struct gl_image *unpack_bitmap( GLcontext *ctx, GLenum format,
                                       GLint width, GLint height,
                                       const GLvoid *pixels )
{
   struct gl_image *image;
   GLint bytes, i, width_in_bytes;
   GLubyte *buffer, *dst;

   assert(format == GL_COLOR_INDEX || format == GL_STENCIL_INDEX);

   /* Alloc dest storage */
   bytes = ((width+7)/8 * height);
   if (bytes>0 && pixels!=NULL) {
      buffer = (GLubyte *) malloc( bytes );
      if (!buffer) {
         return NULL;
      }
      /* Copy/unpack pixel data to buffer */
      width_in_bytes = CEILING( width, 8 );
      dst = buffer;
      for (i=0; i<height; i++) {
         GLvoid *src = gl_pixel_addr_in_image( &ctx->Unpack, pixels,
                                               width, height,
                                               GL_COLOR_INDEX, GL_BITMAP,
                                               0, i, 0 );
         if (!src) {
            free(buffer);
            return NULL;
         }
         MEMCPY( dst, src, width_in_bytes );
         dst += width_in_bytes;
      }
      /* Bit flipping */
      if (ctx->Unpack.LsbFirst) {
         gl_flip_bytes( buffer, bytes );
      }
   }
   else {
      /* a 'null' bitmap */
      buffer = NULL;
   }
   
   image = alloc_image();
   if (image) {
      image->Width = width;
      image->Height = height;
      image->Depth = 1;
      image->Components = 0;
      image->Format = format;
      image->Type = GL_BITMAP;
      image->Data = buffer;
      image->RefCount = 0;
   }
   else {
      free( buffer );
      return NULL;
   }

   return image;
}



/*
 * Unpack a 32x32 pixel polygon stipple from user memory using the
 * current pixel unpack settings.
 */
void gl_unpack_polygon_stipple( const GLcontext *ctx,
                                const GLubyte *pattern, GLuint dest[32] )
{
   GLint i;
   for (i = 0; i < 32; i++) {
      GLubyte *src = (GLubyte *) gl_pixel_addr_in_image( &ctx->Unpack, pattern,
                                  32, 32, GL_COLOR_INDEX, GL_BITMAP, 0, i, 0 );
      dest[i] = (src[0] << 24)
              | (src[1] << 16)
              | (src[2] <<  8)
              | (src[3]      );
   }

   /* Bit flipping within each byte */
   if (ctx->Unpack.LsbFirst) {
      gl_flip_bytes( (GLubyte *) dest, 32 * 4 );
   }
}



/*
 * Pack polygon stipple into user memory given current pixel packing
 * settings.
 */
void gl_pack_polygon_stipple( const GLcontext *ctx,
                              const GLuint pattern[32],
                              GLubyte *dest )
{
   GLint i;
   for (i = 0; i < 32; i++) {
      GLubyte *dst = (GLubyte *) gl_pixel_addr_in_image( &ctx->Pack, dest,
                                  32, 32, GL_COLOR_INDEX, GL_BITMAP, 0, i, 0 );
      dst[0] = (pattern[i] >> 24) & 0xff;
      dst[1] = (pattern[i] >> 16) & 0xff;
      dst[2] = (pattern[i] >>  8) & 0xff;
      dst[3] = (pattern[i]      ) & 0xff;

      /* Bit flipping within each byte */
      if (ctx->Pack.LsbFirst) {
         gl_flip_bytes( (GLubyte *) dst, 4 );
      }
   }
}



/*
 * Unpack an RGBA or CI image and store it as unsigned bytes
 */
static struct gl_image *unpack_ubyte_image( GLcontext *ctx,
                                            GLint width, GLint height,
                                            GLint depth,
                                            GLenum format,
                                            const GLvoid *pixels )
{
   struct gl_image *image;
   GLint width_in_bytes;
   GLint components;
   GLubyte *buffer, *dst;
   GLint i, d;

   components = gl_components_in_format( format );

   width_in_bytes = width * components * sizeof(GLubyte);
   buffer = (GLubyte *) malloc( height * width_in_bytes * depth );
   if (!buffer) {
      return NULL;
   }

   /* Copy/unpack pixel data to buffer */
   dst = buffer;
   for (d=0; d<depth; d++ ) {
      for (i=0;i<height;i++) {
         GLubyte *src = (GLubyte *) gl_pixel_addr_in_image( &ctx->Unpack,
                       pixels, width, height, format, GL_UNSIGNED_BYTE,
                       d, i, 0 );
         if (!src) {
            free(buffer);
            return NULL;
         }
         MEMCPY( dst, src, width_in_bytes );
         dst += width_in_bytes;
      }
   }
   
   if (format == GL_BGR) {
      /* swap order of every ubyte triplet from BGR to RGB */
      for (i=0; i<width*height; i++) {
         GLubyte b = buffer[i*3+0];
         GLubyte r = buffer[i*3+2];
         buffer[i*3+0] = r;
         buffer[i*3+2] = b;
      }
   }
   else if (format == GL_BGRA) {
      /* swap order of every ubyte quadruplet from BGRA to RGBA */
      for (i=0; i<width*height; i++) {
         GLubyte b = buffer[i*4+0];
         GLubyte r = buffer[i*4+2];
         buffer[i*4+0] = r;
         buffer[i*4+2] = b;
      }
   }
   else if (format == GL_ABGR_EXT) {
      /* swap order of every ubyte quadruplet from ABGR to RGBA */
      for (i=0; i<width*height; i++) {
         GLubyte a = buffer[i*4+0];
         GLubyte b = buffer[i*4+1];
         GLubyte g = buffer[i*4+2];
         GLubyte r = buffer[i*4+3];
         buffer[i*4+0] = r;
         buffer[i*4+1] = g;
         buffer[i*4+2] = b;
         buffer[i*4+3] = a;
      }
   }


   image = alloc_image();
   if (image) {
      image->Width = width;
      image->Height = height;
      image->Depth = depth;
      image->Components = components;
      if (format == GL_BGR)
         image->Format = GL_RGB;
      else if (format == GL_BGRA)
         image->Format = GL_RGBA;
      else if (format == GL_ABGR_EXT)
         image->Format = GL_RGBA;
      else
         image->Format = format;
      image->Type = GL_UNSIGNED_BYTE;
      image->Data = buffer;
      image->RefCount = 0;
   }
   else {
      free( buffer );
   }

   return image;
}



/*
 * Unpack an RGBA image storing image as GLfloats
 */
static struct gl_image *unpack_float_image( GLcontext *ctx,
                                            GLint width, GLint height,
                                            GLint depth,
                                            GLenum format, GLenum type,
                                            const GLvoid *pixels )
{
   struct gl_image *image;
   GLfloat *dst;
   GLint elems_per_row;
   GLint components;
   GLint i, j, d;
   GLboolean normalize;

   assert(type != GL_BITMAP);

   components = gl_components_in_format( format );

   elems_per_row = width * components;

   image = alloc_image();
   if (image) {
      image->Width = width;
      image->Height = height;
      image->Depth = depth;
      image->Components = components;
      if (format == GL_BGR)
         image->Format = GL_RGB;
      else if (format == GL_BGRA)
         image->Format = GL_RGBA;
      else if (format == GL_ABGR_EXT)
         image->Format = GL_RGBA;
      else
         image->Format = format;
      image->Type = GL_FLOAT;
      image->Data = malloc( elems_per_row * height * depth * sizeof(GLfloat));
      image->RefCount = 0;
      if (!image->Data)
         return image;
   }
   else {
      return NULL;
   }

   normalize = (format != GL_COLOR_INDEX) && (format != GL_STENCIL_INDEX);

   dst = (GLfloat *) image->Data;

   for (d=0; d<depth; d++) {
      for (i=0;i<height;i++) {
         GLvoid *src = gl_pixel_addr_in_image( &ctx->Unpack, pixels,
                                               width, height,
                                               format, type,
                                               d, i, 0 );
         if (!src) {
            return image;
         }

         switch (type) {
            case GL_UNSIGNED_BYTE:
               {
                  GLubyte *ubsrc = (GLubyte *) src;
                  if (normalize) {
                     for (j=0;j<elems_per_row;j++) {
                        *dst++ = UBYTE_TO_FLOAT(ubsrc[j]);
                     }
                  }
                  else {
                     for (j=0;j<elems_per_row;j++) {
                        *dst++ = (GLfloat) ubsrc[j];
                     }
                  }
               }
               break;
            case GL_BYTE:
               if (normalize) {
                  for (j=0;j<elems_per_row;j++) {
                     *dst++ = BYTE_TO_FLOAT(((GLbyte*)src)[j]);
                  }
               }
               else {
                  for (j=0;j<elems_per_row;j++) {
                     *dst++ = (GLfloat) ((GLbyte*)src)[j];
                  }
               }
               break;
            case GL_UNSIGNED_SHORT:
               if (ctx->Unpack.SwapBytes) {
                  for (j=0;j<elems_per_row;j++) {
                     GLushort value = ((GLushort*)src)[j];
                     value = ((value >> 8) & 0xff) | ((value&0xff) << 8);
                     if (normalize) {
                        *dst++ = USHORT_TO_FLOAT(value);
                     }
                     else {
                        *dst++ = (GLfloat) value;
                     }
                  }
                  }
                  else {
                     if (normalize) {
                        for (j=0;j<elems_per_row;j++) {
                           *dst++ = USHORT_TO_FLOAT(((GLushort*)src)[j]);
                        }
                     }
                  else {
                     for (j=0;j<elems_per_row;j++) {
                        *dst++ = (GLfloat) ((GLushort*)src)[j];
                     }
                  }
                  }
                  break;
               case GL_SHORT:
                  if (ctx->Unpack.SwapBytes) {
                     for (j=0;j<elems_per_row;j++) {
                        GLshort value = ((GLshort*)src)[j];
                        value = ((value >> 8) & 0xff) | ((value&0xff) << 8);
                        if (normalize) {
                           *dst++ = SHORT_TO_FLOAT(value);
                        }
                        else {
                        *dst++ = (GLfloat) value;
                     }
                  }
                  }
                  else {
                     if (normalize) {
                        for (j=0;j<elems_per_row;j++) {
                           *dst++ = SHORT_TO_FLOAT(((GLshort*)src)[j]);
                        }
                     }
                  else {
                     for (j=0;j<elems_per_row;j++) {
                        *dst++ = (GLfloat) ((GLshort*)src)[j];
                     }
                  }
                  }
                  break;
               case GL_UNSIGNED_INT:
                  if (ctx->Unpack.SwapBytes) {
                     GLuint value;
                     for (j=0;j<elems_per_row;j++) {
                        value = ((GLuint*)src)[j];
                        value = ((value & 0xff000000) >> 24)
                              | ((value & 0x00ff0000) >> 8)
                              | ((value & 0x0000ff00) << 8)
                              | ((value & 0x000000ff) << 24);
                        if (normalize) {
                           *dst++ = UINT_TO_FLOAT(value);
                        }
                        else {
                        *dst++ = (GLfloat) value;
                     }
                  }
                  }
                  else {
                     if (normalize) {
                        for (j=0;j<elems_per_row;j++) {
                           *dst++ = UINT_TO_FLOAT(((GLuint*)src)[j]);
                        }
                     }
                  else {
                     for (j=0;j<elems_per_row;j++) {
                        *dst++ = (GLfloat) ((GLuint*)src)[j];
                     }
                  }
                  }
                  break;
               case GL_INT:
                  if (ctx->Unpack.SwapBytes) {
                     GLint value;
                     for (j=0;j<elems_per_row;j++) {
                        value = ((GLint*)src)[j];
                        value = ((value & 0xff000000) >> 24)
                              | ((value & 0x00ff0000) >> 8)
                              | ((value & 0x0000ff00) << 8)
                              | ((value & 0x000000ff) << 24);
                        if (normalize) {
                           *dst++ = INT_TO_FLOAT(value);
                        }
                        else {
                        *dst++ = (GLfloat) value;
                     }
                  }
                  }
                  else {
                     if (normalize) {
                        for (j=0;j<elems_per_row;j++) {
                           *dst++ = INT_TO_FLOAT(((GLint*)src)[j]);
                        }
                     }
                  else {
                     for (j=0;j<elems_per_row;j++) {
                        *dst++ = (GLfloat) ((GLint*)src)[j];
                     }
                  }
                  }
                  break;
               case GL_FLOAT:
                  if (ctx->Unpack.SwapBytes) {
                     GLint value;
                     for (j=0;j<elems_per_row;j++) {
                        value = ((GLuint*)src)[j];
                        value = ((value & 0xff000000) >> 24)
                              | ((value & 0x00ff0000) >> 8)
                              | ((value & 0x0000ff00) << 8)
                              | ((value & 0x000000ff) << 24);
                        *dst++ = *((GLfloat*) &value);
                     }
                  }
                  else {
                     MEMCPY( dst, src, elems_per_row*sizeof(GLfloat) );
                     dst += elems_per_row;
                  }
                  break;
            case GL_UNSIGNED_BYTE_3_3_2:
               {
                  GLubyte *ubsrc = (GLubyte *) src;
                  for (j=0;j<elems_per_row;j++) {
                     GLubyte p = ubsrc[j];
                     *dst++ = ((p >> 5)      ) * (1.0F / 7.0F); /* red */
                     *dst++ = ((p >> 2) & 0x7) * (1.0F / 7.0F); /* green */
                     *dst++ = ((p     ) & 0x3) * (1.0F / 3.0F); /* blue */
                  }
               }
               break;
            case GL_UNSIGNED_BYTE_2_3_3_REV:
               {
                  GLubyte *ubsrc = (GLubyte *) src;
                  for (j=0;j<elems_per_row;j++) {
                     GLubyte p = ubsrc[j];
                     *dst++ = ((p     ) & 0x7) * (1.0F / 7.0F); /* red */
                     *dst++ = ((p >> 3) & 0x7) * (1.0F / 7.0F); /* green */
                     *dst++ = ((p >> 6)      ) * (1.0F / 3.0F); /* blue */
                  }
      }
               break;
            case GL_UNSIGNED_SHORT_5_6_5:
               {
                  GLushort *ussrc = (GLushort *) src;
                  for (j=0;j<elems_per_row;j++) {
                     GLushort p = ussrc[j];
                     *dst++ = ((p >> 11)       ) * (1.0F / 31.0F); /* red */
                     *dst++ = ((p >>  5) & 0x3f) * (1.0F / 63.0F); /* green */
                     *dst++ = ((p      ) & 0x1f) * (1.0F / 31.0F); /* blue */
      }
   }
               break;
            case GL_UNSIGNED_SHORT_5_6_5_REV:
               {
                  GLushort *ussrc = (GLushort *) src;
                  for (j=0;j<elems_per_row;j++) {
                     GLushort p = ussrc[j];
                     *dst++ = ((p      ) & 0x1f) * (1.0F / 31.0F); /* red */
                     *dst++ = ((p >>  5) & 0x3f) * (1.0F / 63.0F); /* green */
                     *dst++ = ((p >> 11)       ) * (1.0F / 31.0F); /* blue */
                  }
               }
               break;
	    case GL_UNSIGNED_SHORT_4_4_4_4:
               {
                  GLushort *ussrc = (GLushort *) src;
                  for (j=0;j<elems_per_row;j++) {
                     GLushort p = ussrc[j];
                     *dst++ = ((p >> 12)      ) * (1.0F / 15.0F); /* red */
                     *dst++ = ((p >>  8) & 0xf) * (1.0F / 15.0F); /* green */
                     *dst++ = ((p >>  4) & 0xf) * (1.0F / 15.0F); /* blue */
                     *dst++ = ((p      ) & 0xf) * (1.0F / 15.0F); /* alpha */
                  }
               }
               break;
	    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
               {
                  GLushort *ussrc = (GLushort *) src;
                  for (j=0;j<elems_per_row;j++) {
                     GLushort p = ussrc[j];
                     *dst++ = ((p      ) & 0xf) * (1.0F / 15.0F); /* red */
                     *dst++ = ((p >>  4) & 0xf) * (1.0F / 15.0F); /* green */
                     *dst++ = ((p >>  8) & 0xf) * (1.0F / 15.0F); /* blue */
                     *dst++ = ((p >> 12)      ) * (1.0F / 15.0F); /* alpha */
                  }
               }
               break;
	    case GL_UNSIGNED_SHORT_5_5_5_1:
               {
                  GLushort *ussrc = (GLushort *) src;
                  for (j=0;j<elems_per_row;j++) {
                     GLushort p = ussrc[j];
                     *dst++ = ((p >> 11)       ) * (1.0F / 31.0F); /* red */
                     *dst++ = ((p >>  6) & 0x1f) * (1.0F / 31.0F); /* green */
                     *dst++ = ((p >>  1) & 0x1f) * (1.0F / 31.0F); /* blue */
                     *dst++ = ((p      ) & 0x1)  * (1.0F /  1.0F); /* alpha */
                  }
               }
               break;
	    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
               {
                  GLushort *ussrc = (GLushort *) src;
                  for (j=0;j<elems_per_row;j++) {
                     GLushort p = ussrc[j];
                     *dst++ = ((p      ) & 0x1f) * (1.0F / 31.0F); /* red */
                     *dst++ = ((p >>  5) & 0x1f) * (1.0F / 31.0F); /* green */
                     *dst++ = ((p >> 10) & 0x1f) * (1.0F / 31.0F); /* blue */
                     *dst++ = ((p >> 15)       ) * (1.0F /  1.0F); /* alpha */
                  }
               }
               break;
	    case GL_UNSIGNED_INT_8_8_8_8:
               {
                  GLuint *uisrc = (GLuint *) src;
                  for (j=0;j<elems_per_row;j++) {
                     GLuint p = uisrc[j];
                     *dst++ = ((p >> 24)       ) * (1.0F / 255.0F); /* red */
                     *dst++ = ((p >> 16) & 0xff) * (1.0F / 255.0F); /* green */
                     *dst++ = ((p >>  8) & 0xff) * (1.0F / 255.0F); /* blue */
                     *dst++ = ((p      ) & 0xff) * (1.0F /   1.0F); /* alpha */
                  }
               }
               break;
	    case GL_UNSIGNED_INT_8_8_8_8_REV:
               {
                  GLuint *uisrc = (GLuint *) src;
                  for (j=0;j<elems_per_row;j++) {
                     GLuint p = uisrc[j];
                     *dst++ = ((p      ) & 0xff) * (1.0F /   1.0F); /* red */
                     *dst++ = ((p >>  8) & 0xff) * (1.0F / 255.0F); /* green */
                     *dst++ = ((p >> 16) & 0xff) * (1.0F / 255.0F); /* blue */
                     *dst++ = ((p >> 24)       ) * (1.0F / 255.0F); /* alpha */
                  }
               }
               break;
	    case GL_UNSIGNED_INT_10_10_10_2:
               {
                  GLuint *uisrc = (GLuint *) src;
                  for (j=0;j<elems_per_row;j++) {
                     GLuint p = uisrc[j];
                     *dst++ = ((p >> 22)        ) * (1.0F / 1023.0F); /* red */
                     *dst++ = ((p >> 12) & 0x3ff) * (1.0F / 1023.0F); /* green */
                     *dst++ = ((p >>  2) & 0x3ff) * (1.0F / 1023.0F); /* blue */
                     *dst++ = ((p      ) & 0x3  ) * (1.0F /    3.0F); /* alpha */
                  }
               }
               break;
	    case GL_UNSIGNED_INT_2_10_10_10_REV:
               {
                  GLuint *uisrc = (GLuint *) src;
                  for (j=0;j<elems_per_row;j++) {
                     GLuint p = uisrc[j];
                     *dst++ = ((p      ) & 0x3ff) * (1.0F / 1023.0F); /* red */
                     *dst++ = ((p >> 10) & 0x3ff) * (1.0F / 1023.0F); /* green */
                     *dst++ = ((p >> 20) & 0x3ff) * (1.0F / 1023.0F); /* blue */
                     *dst++ = ((p >> 30)        ) * (1.0F /    3.0F); /* alpha */
                  }
               }
               break;
            default:
               gl_problem(ctx, "unpack_float_image type" );
               return image;
         }
      }
   }

   if (format == GL_BGR) {
      /* swap order of every float triplet from BGR to RGBA */
      GLfloat *buffer = (GLfloat *) image->Data;
      for (i=0; i<width*height*depth; i++) {
         GLfloat b = buffer[i*3+0];
         GLfloat r = buffer[i*3+2];
         buffer[i*3+0] = r;
         buffer[i*3+2] = b;
      }
   }
   else if (format == GL_BGRA) {
      /* swap order of every float quadruplet from BGRA to RGBA */
      GLfloat *buffer = (GLfloat *) image->Data;
      for (i=0; i<width*height*depth; i++) {
         GLfloat b = buffer[i*4+0];
         GLfloat r = buffer[i*4+2];
         buffer[i*4+0] = r;
         buffer[i*4+2] = b;
      }
   }
   else if (format == GL_ABGR_EXT) {
      /* swap order of every float quadruplet from ABGR to RGBA */
      GLfloat *buffer = (GLfloat *) image->Data;
      for (i=0; i<width*height*depth; i++) {
         GLfloat a = buffer[i*4+0];
         GLfloat b = buffer[i*4+1];
         GLfloat g = buffer[i*4+2];
         GLfloat r = buffer[i*4+3];
         buffer[i*4+0] = r;
         buffer[i*4+1] = g;
         buffer[i*4+2] = b;
         buffer[i*4+3] = a;
      }
   }

   return image;
}



/*
 * Unpack a bitmap image, making a new gl_image.
 */
struct gl_image *gl_unpack_bitmap( GLcontext *ctx,
                                   GLsizei width, GLsizei height,
                                   const GLubyte *bitmap )
{
   return gl_unpack_image( ctx, width, height,
                           GL_COLOR_INDEX, GL_BITMAP, bitmap );
}



/*
 * Unpack a 2-D image from user-supplied address, returning a pointer to
 * a new gl_image struct.
 *
 * Input:  width, height - size in pixels
 *         format - format of incoming pixel data, ignored if
 *                     srcType BITMAP.
 *         type - GL_UNSIGNED_BYTE .. GL_FLOAT
 *         pixels - pointer to unpacked image in client memory space.
 */
struct gl_image *gl_unpack_image( GLcontext *ctx,
                                  GLint width, GLint height,
                                  GLenum format, GLenum type,
                                  const GLvoid *pixels )
{ 
   return gl_unpack_image3D( ctx, width, height, 1,
                             format, type, pixels );
}



/* 
 * Unpack a 1, 2 or 3-D image from user-supplied address, returning a
 * pointer to a new gl_image struct.
 * This function is always called by a higher-level unpack function such
 * as gl_unpack_texsubimage() or gl_unpack_bitmap().
 *
 * Input:  width, height, depth - size in pixels
 *         format - format of incoming pixel data
 *         type - GL_UNSIGNED_BYTE .. GL_FLOAT
 *         pixels - pointer to unpacked image.
 */
struct gl_image *gl_unpack_image3D( GLcontext *ctx,
                                    GLint width, GLint height, GLint depth,
                                    GLenum format, GLenum type,
                                    const GLvoid *pixels )
{
   if (width <= 0 || height <= 0 || depth <= 0) {
      return alloc_error_image(width, height, depth, format, type);
   }

   if (type==GL_BITMAP) {
      if (format != GL_COLOR_INDEX && format != GL_STENCIL_INDEX) {
         return alloc_error_image(width, height, depth, format, type);
      }
      else {
         return unpack_bitmap( ctx, format, width, height, pixels );
      }
   }
   else if (format==GL_DEPTH_COMPONENT) {
      /* TODO: pack as GLdepth values (GLushort or GLuint) */
      return unpack_depth_image( ctx, type, width, height, pixels);
   }
   else if (format==GL_STENCIL_INDEX) {
      /* TODO: pack as GLstencil (GLubyte or GLushort) */
      return unpack_stencil_image( ctx, type, width, height, pixels);
   }
   else if (type==GL_UNSIGNED_BYTE) {
      /* upack, convert to GLubytes */
      return unpack_ubyte_image( ctx, width, height, depth, format, pixels );
   }
   else {
      /* upack, convert to floats */
      return unpack_float_image( ctx, width, height, depth,
                                 format, type, pixels );
   }

   /* never get here */
   /*return NULL;*/
}


/*
 * Apply pixel-transfer operations (scale, bias, mapping) to a single row
 * of a gl_image.  Put resulting color components into result array.
 */
void gl_scale_bias_map_image_data( const GLcontext *ctx,
                                   const struct gl_image *image,
                                   GLint row, GLubyte result[] )
{
   GLint start, i;

   assert(ctx);
   assert(image);
   assert(result);
   assert(row >= 0);

   start = row * image->Width * image->Components;

   for (i=0; i < image->Width; i++) {
      GLint pos = start+i;
      GLfloat red, green, blue, alpha;
      if (image->Type == GL_UNSIGNED_BYTE) {
         const GLubyte *data = (GLubyte *) image->Data;
         switch (image->Format) {
            case GL_RED:
               red   = data[pos] * (1.0F/255.0F);
               green = 0;
               blue  = 0;
               alpha = 0;
               break;
            case GL_RGB:
               red   = data[pos*3+0] * (1.0F/255.0F);
               green = data[pos*3+1] * (1.0F/255.0F);
               blue  = data[pos*3+2] * (1.0F/255.0F);
               alpha = 0;
               break;
            default:
               gl_problem(ctx, "bad image format in gl_scale...image_data");
               return;
         }
      }
      else if (image->Type == GL_FLOAT) {
         const GLubyte *data = (GLubyte *) image->Data;
         switch (image->Format) {
            case GL_RED:
               red   = data[pos];
               green = 0;
               blue  = 0;
               alpha = 0;
               break;
            case GL_RGB:
               red   = data[pos*3+0];
               green = data[pos*3+1];
               blue  = data[pos*3+2];
               alpha = 0;
               break;
            default:
               gl_problem(ctx, "bad image format in gl_scale...image_data");
               return;
         }
      }
      else {
         gl_problem(ctx, "Bad image type in gl_scale_...image_data");
         return;
      }

      assert(red   >= 0.0 && red   <= 1.0);
      assert(green >= 0.0 && green <= 1.0);
      assert(blue  >= 0.0 && blue  <= 1.0);
      assert(alpha >= 0.0 && alpha <= 1.0);

      /*
      if (scale or bias) {


      }
      if (mapping) {

      }
      */

      result[i*4+0] = (GLubyte) (red   * 255.0);
      result[i*4+1] = (GLubyte) (green * 255.0);
      result[i*4+2] = (GLubyte) (blue  * 255.0);
      result[i*4+3] = (GLubyte) (alpha * 255.0);
   }
}



/*
 * Pack the given RGBA span into client memory at 'dest' address
 * in the given pixel format and type.  Apply all enabled pixel
 * transfer and packing parameters.  This is used by glReadPixels (NOT YET)
 * and glGetTexImage?D()
 */
void gl_pack_rgba_span( const GLcontext *ctx,
                        GLuint n, CONST GLubyte rgba[][4],
                        GLenum format, GLenum type, GLvoid *destination)
{
   /* Test for optimized case first */
   if (!ctx->Pixel.ScaleOrBiasRGBA && !ctx->Pixel.MapColorFlag &&
       format == GL_RGBA && type == GL_UNSIGNED_BYTE) {
      /* simple case */
      MEMCPY( destination, rgba, n * 4 * sizeof(GLubyte) );
   }
   else {
      GLfloat red[MAX_WIDTH], green[MAX_WIDTH], blue[MAX_WIDTH];
      GLfloat alpha[MAX_WIDTH], luminance[MAX_WIDTH];
      GLfloat rscale = 1.0F / 255.0F;
      GLfloat gscale = 1.0F / 255.0F;
      GLfloat bscale = 1.0F / 255.0F;
      GLfloat ascale = 1.0F / 255.0F;
      GLuint i;

      assert( n < MAX_WIDTH );

      /* convert color components to floating point */
      for (i=0;i<n;i++) {
         red[i]   = rgba[i][RCOMP] * rscale;
         green[i] = rgba[i][GCOMP] * gscale;
         blue[i]  = rgba[i][BCOMP] * bscale;
         alpha[i] = rgba[i][ACOMP] * ascale;
      }

      /*
       * Apply scale, bias and mappings if enabled.
       */
      if (ctx->Pixel.ScaleOrBiasRGBA) {
         gl_scale_and_bias_color( ctx, n, red, green, blue, alpha );
      }
      if (ctx->Pixel.MapColorFlag) {
         gl_map_color( ctx, n, red, green, blue, alpha );
      }
      if (format==GL_LUMINANCE || format==GL_LUMINANCE_ALPHA) {
         for (i=0;i<n;i++) {
            GLfloat sum = red[i] + green[i] + blue[i];
            luminance[i] = CLAMP( sum, 0.0F, 1.0F );
         }
      }

      /*
       * Pack/store the pixels.  Ugh!  Lots of cases!!!
       */
      switch (type) {
         case GL_UNSIGNED_BYTE:
            {
               GLubyte *dst = (GLubyte *) destination;
               switch (format) {
                  case GL_RED:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_UBYTE(red[i]);
                     break;
                  case GL_GREEN:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_UBYTE(green[i]);
                     break;
                  case GL_BLUE:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_UBYTE(blue[i]);
                     break;
                  case GL_LUMINANCE:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_UBYTE(luminance[i]);
                     break;
                  case GL_LUMINANCE_ALPHA:
                     for (i=0;i<n;i++) {
                        dst[i*2+0] = FLOAT_TO_UBYTE(luminance[i]);
                        dst[i*2+1] = FLOAT_TO_UBYTE(alpha[i]);
                     }
                     break;
                  case GL_RGB:
                     for (i=0;i<n;i++) {
                        dst[i*3+0] = FLOAT_TO_UBYTE(red[i]);
                        dst[i*3+1] = FLOAT_TO_UBYTE(green[i]);
                        dst[i*3+2] = FLOAT_TO_UBYTE(blue[i]);
                     }
                     break;
                  case GL_RGBA:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = FLOAT_TO_UBYTE(red[i]);
                        dst[i*4+1] = FLOAT_TO_UBYTE(green[i]);
                        dst[i*4+2] = FLOAT_TO_UBYTE(blue[i]);
                        dst[i*4+3] = FLOAT_TO_UBYTE(alpha[i]);
                     }
                     break;
                  case GL_BGR:
                     for (i=0;i<n;i++) {
                        dst[i*3+0] = FLOAT_TO_UBYTE(blue[i]);
                        dst[i*3+1] = FLOAT_TO_UBYTE(green[i]);
                        dst[i*3+2] = FLOAT_TO_UBYTE(red[i]);
                     }
                     break;
                  case GL_BGRA:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = FLOAT_TO_UBYTE(blue[i]);
                        dst[i*4+1] = FLOAT_TO_UBYTE(green[i]);
                        dst[i*4+2] = FLOAT_TO_UBYTE(red[i]);
                        dst[i*4+3] = FLOAT_TO_UBYTE(alpha[i]);
                     }
                     break;
                  case GL_ABGR_EXT:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = FLOAT_TO_UBYTE(alpha[i]);
                        dst[i*4+1] = FLOAT_TO_UBYTE(blue[i]);
                        dst[i*4+2] = FLOAT_TO_UBYTE(green[i]);
                        dst[i*4+3] = FLOAT_TO_UBYTE(red[i]);
                     }
                     break;
                  default:
                     gl_problem(ctx, "bad format in gl_pack_rgba_span\n");
               }
	    }
	    break;
	 case GL_BYTE:
            {
               GLbyte *dst = (GLbyte *) destination;
               switch (format) {
                  case GL_RED:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_BYTE(red[i]);
                     break;
                  case GL_GREEN:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_BYTE(green[i]);
                     break;
                  case GL_BLUE:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_BYTE(blue[i]);
                     break;
                  case GL_LUMINANCE:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_BYTE(luminance[i]);
                     break;
                  case GL_LUMINANCE_ALPHA:
                     for (i=0;i<n;i++) {
                        dst[i*2+0] = FLOAT_TO_BYTE(luminance[i]);
                        dst[i*2+1] = FLOAT_TO_BYTE(alpha[i]);
                     }
                     break;
                  case GL_RGB:
                     for (i=0;i<n;i++) {
                        dst[i*3+0] = FLOAT_TO_BYTE(red[i]);
                        dst[i*3+1] = FLOAT_TO_BYTE(green[i]);
                        dst[i*3+2] = FLOAT_TO_BYTE(blue[i]);
                     }
                     break;
                  case GL_RGBA:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = FLOAT_TO_BYTE(red[i]);
                        dst[i*4+1] = FLOAT_TO_BYTE(green[i]);
                        dst[i*4+2] = FLOAT_TO_BYTE(blue[i]);
                        dst[i*4+3] = FLOAT_TO_BYTE(alpha[i]);
                     }
                     break;
                  case GL_BGR:
                     for (i=0;i<n;i++) {
                        dst[i*3+0] = FLOAT_TO_BYTE(blue[i]);
                        dst[i*3+1] = FLOAT_TO_BYTE(green[i]);
                        dst[i*3+2] = FLOAT_TO_BYTE(red[i]);
                     }
                     break;
                  case GL_BGRA:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = FLOAT_TO_BYTE(blue[i]);
                        dst[i*4+1] = FLOAT_TO_BYTE(green[i]);
                        dst[i*4+2] = FLOAT_TO_BYTE(red[i]);
                        dst[i*4+3] = FLOAT_TO_BYTE(alpha[i]);
                     }
                  case GL_ABGR_EXT:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = FLOAT_TO_BYTE(alpha[i]);
                        dst[i*4+1] = FLOAT_TO_BYTE(blue[i]);
                        dst[i*4+2] = FLOAT_TO_BYTE(green[i]);
                        dst[i*4+3] = FLOAT_TO_BYTE(red[i]);
                     }
                     break;
                  default:
                     gl_problem(ctx, "bad format in gl_pack_rgba_span\n");
               }
            }
	    break;
	 case GL_UNSIGNED_SHORT:
            {
               GLushort *dst = (GLushort *) destination;
               switch (format) {
                  case GL_RED:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_USHORT(red[i]);
                     break;
                  case GL_GREEN:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_USHORT(green[i]);
                     break;
                  case GL_BLUE:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_USHORT(blue[i]);
                     break;
                  case GL_LUMINANCE:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_USHORT(luminance[i]);
                     break;
                  case GL_LUMINANCE_ALPHA:
                     for (i=0;i<n;i++) {
                        dst[i*2+0] = FLOAT_TO_USHORT(luminance[i]);
                        dst[i*2+1] = FLOAT_TO_USHORT(alpha[i]);
                     }
                     break;
                  case GL_RGB:
                     for (i=0;i<n;i++) {
                        dst[i*3+0] = FLOAT_TO_USHORT(red[i]);
                        dst[i*3+1] = FLOAT_TO_USHORT(green[i]);
                        dst[i*3+2] = FLOAT_TO_USHORT(blue[i]);
                     }
                     break;
                  case GL_RGBA:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = FLOAT_TO_USHORT(red[i]);
                        dst[i*4+1] = FLOAT_TO_USHORT(green[i]);
                        dst[i*4+2] = FLOAT_TO_USHORT(blue[i]);
                        dst[i*4+3] = FLOAT_TO_USHORT(alpha[i]);
                     }
                     break;
                  case GL_BGR:
                     for (i=0;i<n;i++) {
                        dst[i*3+0] = FLOAT_TO_USHORT(blue[i]);
                        dst[i*3+1] = FLOAT_TO_USHORT(green[i]);
                        dst[i*3+2] = FLOAT_TO_USHORT(red[i]);
                     }
                     break;
                  case GL_BGRA:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = FLOAT_TO_USHORT(blue[i]);
                        dst[i*4+1] = FLOAT_TO_USHORT(green[i]);
                        dst[i*4+2] = FLOAT_TO_USHORT(red[i]);
                        dst[i*4+3] = FLOAT_TO_USHORT(alpha[i]);
                     }
                     break;
                  case GL_ABGR_EXT:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = FLOAT_TO_USHORT(alpha[i]);
                        dst[i*4+1] = FLOAT_TO_USHORT(blue[i]);
                        dst[i*4+2] = FLOAT_TO_USHORT(green[i]);
                        dst[i*4+3] = FLOAT_TO_USHORT(red[i]);
                     }
                     break;
                  default:
                     gl_problem(ctx, "bad format in gl_pack_rgba_span\n");
               }
               if (ctx->Pack.SwapBytes) {
                  gl_swap2( (GLushort *) dst, n*n );
               }
            }
	    break;
	 case GL_SHORT:
            {
               GLshort *dst = (GLshort *) destination;
               switch (format) {
                  case GL_RED:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_SHORT(red[i]);
                     break;
                  case GL_GREEN:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_SHORT(green[i]);
                     break;
                  case GL_BLUE:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_SHORT(blue[i]);
                     break;
                  case GL_LUMINANCE:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_SHORT(luminance[i]);
                     break;
                  case GL_LUMINANCE_ALPHA:
                     for (i=0;i<n;i++) {
                        dst[i*2+0] = FLOAT_TO_SHORT(luminance[i]);
                        dst[i*2+1] = FLOAT_TO_SHORT(alpha[i]);
                     }
                     break;
                  case GL_RGB:
                     for (i=0;i<n;i++) {
                        dst[i*3+0] = FLOAT_TO_SHORT(red[i]);
                        dst[i*3+1] = FLOAT_TO_SHORT(green[i]);
                        dst[i*3+2] = FLOAT_TO_SHORT(blue[i]);
                     }
                     break;
                  case GL_RGBA:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = FLOAT_TO_SHORT(red[i]);
                        dst[i*4+1] = FLOAT_TO_SHORT(green[i]);
                        dst[i*4+2] = FLOAT_TO_SHORT(blue[i]);
                        dst[i*4+3] = FLOAT_TO_SHORT(alpha[i]);
                     }
                     break;
                  case GL_BGR:
                     for (i=0;i<n;i++) {
                        dst[i*3+0] = FLOAT_TO_SHORT(blue[i]);
                        dst[i*3+1] = FLOAT_TO_SHORT(green[i]);
                        dst[i*3+2] = FLOAT_TO_SHORT(red[i]);
                     }
                     break;
                  case GL_BGRA:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = FLOAT_TO_SHORT(blue[i]);
                        dst[i*4+1] = FLOAT_TO_SHORT(green[i]);
                        dst[i*4+2] = FLOAT_TO_SHORT(red[i]);
                        dst[i*4+3] = FLOAT_TO_SHORT(alpha[i]);
                     }
                  case GL_ABGR_EXT:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = FLOAT_TO_SHORT(alpha[i]);
                        dst[i*4+1] = FLOAT_TO_SHORT(blue[i]);
                        dst[i*4+2] = FLOAT_TO_SHORT(green[i]);
                        dst[i*4+3] = FLOAT_TO_SHORT(red[i]);
                     }
                     break;
                  default:
                     gl_problem(ctx, "bad format in gl_pack_rgba_span\n");
               }
               if (ctx->Pack.SwapBytes) {
                  gl_swap2( (GLushort *) dst, n*n );
               }
            }
	    break;
	 case GL_UNSIGNED_INT:
            {
               GLuint *dst = (GLuint *) destination;
               switch (format) {
                  case GL_RED:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_UINT(red[i]);
                     break;
                  case GL_GREEN:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_UINT(green[i]);
                     break;
                  case GL_BLUE:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_UINT(blue[i]);
                     break;
                  case GL_LUMINANCE:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_UINT(luminance[i]);
                     break;
                  case GL_LUMINANCE_ALPHA:
                     for (i=0;i<n;i++) {
                        dst[i*2+0] = FLOAT_TO_UINT(luminance[i]);
                        dst[i*2+1] = FLOAT_TO_UINT(alpha[i]);
                     }
                     break;
                  case GL_RGB:
                     for (i=0;i<n;i++) {
                        dst[i*3+0] = FLOAT_TO_UINT(red[i]);
                        dst[i*3+1] = FLOAT_TO_UINT(green[i]);
                        dst[i*3+2] = FLOAT_TO_UINT(blue[i]);
                     }
                     break;
                  case GL_RGBA:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = FLOAT_TO_UINT(red[i]);
                        dst[i*4+1] = FLOAT_TO_UINT(green[i]);
                        dst[i*4+2] = FLOAT_TO_UINT(blue[i]);
                        dst[i*4+3] = FLOAT_TO_UINT(alpha[i]);
                     }
                     break;
                  case GL_BGR:
                     for (i=0;i<n;i++) {
                        dst[i*3+0] = FLOAT_TO_UINT(blue[i]);
                        dst[i*3+1] = FLOAT_TO_UINT(green[i]);
                        dst[i*3+2] = FLOAT_TO_UINT(red[i]);
                     }
                     break;
                  case GL_BGRA:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = FLOAT_TO_UINT(blue[i]);
                        dst[i*4+1] = FLOAT_TO_UINT(green[i]);
                        dst[i*4+2] = FLOAT_TO_UINT(red[i]);
                        dst[i*4+3] = FLOAT_TO_UINT(alpha[i]);
                     }
                     break;
                  case GL_ABGR_EXT:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = FLOAT_TO_UINT(alpha[i]);
                        dst[i*4+1] = FLOAT_TO_UINT(blue[i]);
                        dst[i*4+2] = FLOAT_TO_UINT(green[i]);
                        dst[i*4+3] = FLOAT_TO_UINT(red[i]);
                     }
                     break;
                  default:
                     gl_problem(ctx, "bad format in gl_pack_rgba_span\n");
               }
               if (ctx->Pack.SwapBytes) {
                  gl_swap4( (GLuint *) dst, n*n );
               }
            }
	    break;
	 case GL_INT:
	    {
               GLint *dst = (GLint *) destination;
               switch (format) {
                  case GL_RED:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_INT(red[i]);
                     break;
                  case GL_GREEN:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_INT(green[i]);
                     break;
                  case GL_BLUE:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_INT(blue[i]);
                     break;
                  case GL_LUMINANCE:
                     for (i=0;i<n;i++)
                        dst[i] = FLOAT_TO_INT(luminance[i]);
                     break;
                  case GL_LUMINANCE_ALPHA:
                     for (i=0;i<n;i++) {
                        dst[i*2+0] = FLOAT_TO_INT(luminance[i]);
                        dst[i*2+1] = FLOAT_TO_INT(alpha[i]);
                     }
                     break;
                  case GL_RGB:
                     for (i=0;i<n;i++) {
                        dst[i*3+0] = FLOAT_TO_INT(red[i]);
                        dst[i*3+1] = FLOAT_TO_INT(green[i]);
                        dst[i*3+2] = FLOAT_TO_INT(blue[i]);
                     }
                     break;
                  case GL_RGBA:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = FLOAT_TO_INT(red[i]);
                        dst[i*4+1] = FLOAT_TO_INT(green[i]);
                        dst[i*4+2] = FLOAT_TO_INT(blue[i]);
                        dst[i*4+3] = FLOAT_TO_INT(alpha[i]);
                     }
                     break;
                  case GL_BGR:
                     for (i=0;i<n;i++) {
                        dst[i*3+0] = FLOAT_TO_INT(blue[i]);
                        dst[i*3+1] = FLOAT_TO_INT(green[i]);
                        dst[i*3+2] = FLOAT_TO_INT(red[i]);
                     }
                     break;
                  case GL_BGRA:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = FLOAT_TO_INT(blue[i]);
                        dst[i*4+1] = FLOAT_TO_INT(green[i]);
                        dst[i*4+2] = FLOAT_TO_INT(red[i]);
                        dst[i*4+3] = FLOAT_TO_INT(alpha[i]);
                     }
                     break;
                  case GL_ABGR_EXT:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = FLOAT_TO_INT(alpha[i]);
                        dst[i*4+1] = FLOAT_TO_INT(blue[i]);
                        dst[i*4+2] = FLOAT_TO_INT(green[i]);
                        dst[i*4+3] = FLOAT_TO_INT(red[i]);
                     }
                     break;
                  default:
                     gl_problem(ctx, "bad format in gl_pack_rgba_span\n");
               }
	       if (ctx->Pack.SwapBytes) {
		  gl_swap4( (GLuint *) dst, n*n );
	       }
	    }
	    break;
	 case GL_FLOAT:
	    {
               GLfloat *dst = (GLfloat *) destination;
               switch (format) {
                  case GL_RED:
                     for (i=0;i<n;i++)
                        dst[i] = red[i];
                     break;
                  case GL_GREEN:
                     for (i=0;i<n;i++)
                        dst[i] = green[i];
                     break;
                  case GL_BLUE:
                     for (i=0;i<n;i++)
                        dst[i] = blue[i];
                     break;
                  case GL_LUMINANCE:
                     for (i=0;i<n;i++)
                        dst[i] = luminance[i];
                     break;
                  case GL_LUMINANCE_ALPHA:
                     for (i=0;i<n;i++) {
                        dst[i*2+0] = luminance[i];
                        dst[i*2+1] = alpha[i];
                     }
                     break;
                  case GL_RGB:
                     for (i=0;i<n;i++) {
                        dst[i*3+0] = red[i];
                        dst[i*3+1] = green[i];
                        dst[i*3+2] = blue[i];
                     }
                     break;
                  case GL_RGBA:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = red[i];
                        dst[i*4+1] = green[i];
                        dst[i*4+2] = blue[i];
                        dst[i*4+3] = alpha[i];
                     }
                     break;
                  case GL_BGR:
                     for (i=0;i<n;i++) {
                        dst[i*3+0] = blue[i];
                        dst[i*3+1] = green[i];
                        dst[i*3+2] = red[i];
                     }
                     break;
                  case GL_BGRA:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = blue[i];
                        dst[i*4+1] = green[i];
                        dst[i*4+2] = red[i];
                        dst[i*4+3] = alpha[i];
                     }
                     break;
                  case GL_ABGR_EXT:
                     for (i=0;i<n;i++) {
                        dst[i*4+0] = alpha[i];
                        dst[i*4+1] = blue[i];
                        dst[i*4+2] = green[i];
                        dst[i*4+3] = red[i];
                     }
                     break;
                  default:
                     gl_problem(ctx, "bad format in gl_pack_rgba_span\n");
               }
	       if (ctx->Pack.SwapBytes) {
		  gl_swap4( (GLuint *) dst, n*n );
	       }
	    }
	    break;
         case GL_UNSIGNED_BYTE_3_3_2:
            if (format == GL_RGB) {
               GLubyte *dst = (GLubyte *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (red[i]   * 7.0F)) << 5)
                         | (((GLint) (green[i] * 7.0F)) << 2)
                         | (((GLint) (blue[i]  * 3.0F))     );
               }
            }
            break;
         case GL_UNSIGNED_BYTE_2_3_3_REV:
            if (format == GL_RGB) {
               GLubyte *dst = (GLubyte *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (red[i]   * 7.0F))     )
                         | (((GLint) (green[i] * 7.0F)) << 3)
                         | (((GLint) (blue[i]  * 3.0F)) << 5);
               }
            }
            break;
         case GL_UNSIGNED_SHORT_5_6_5:
            if (format == GL_RGB) {
               GLushort *dst = (GLushort *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (red[i]   * 31.0F)) << 11)
                         | (((GLint) (green[i] * 63.0F)) <<  5)
                         | (((GLint) (blue[i]  * 31.0F))      );
               }
            }
            break;
         case GL_UNSIGNED_SHORT_5_6_5_REV:
            if (format == GL_RGB) {
               GLushort *dst = (GLushort *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (red[i]   * 31.0F))      )
                         | (((GLint) (green[i] * 63.0F)) <<  5)
                         | (((GLint) (blue[i]  * 31.0F)) << 11);
               }
            }
            break;
         case GL_UNSIGNED_SHORT_4_4_4_4:
            if (format == GL_RGB) {
               GLushort *dst = (GLushort *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (red[i]   * 15.0F)) << 12)
                         | (((GLint) (green[i] * 15.0F)) <<  8)
                         | (((GLint) (blue[i]  * 15.0F)) <<  4)
                         | (((GLint) (alpha[i] * 15.0F))      );
               }
            }
            break;
         case GL_UNSIGNED_SHORT_4_4_4_4_REV:
            if (format == GL_RGB) {
               GLushort *dst = (GLushort *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (red[i]   * 15.0F))      )
                         | (((GLint) (green[i] * 15.0F)) <<  4)
                         | (((GLint) (blue[i]  * 15.0F)) <<  8)
                         | (((GLint) (alpha[i] * 15.0F)) << 12);
               }
            }
            break;
         case GL_UNSIGNED_SHORT_5_5_5_1:
            if (format == GL_RGB) {
               GLushort *dst = (GLushort *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (red[i]   * 31.0F)) << 11)
                         | (((GLint) (green[i] * 31.0F)) <<  6)
                         | (((GLint) (blue[i]  * 31.0F)) <<  1)
                         | (((GLint) (alpha[i] *  1.0F))      );
               }
            }
            break;
         case GL_UNSIGNED_SHORT_1_5_5_5_REV:
            if (format == GL_RGB) {
               GLushort *dst = (GLushort *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (red[i]   * 31.0F))      )
                         | (((GLint) (green[i] * 31.0F)) <<  5)
                         | (((GLint) (blue[i]  * 31.0F)) << 10)
                         | (((GLint) (alpha[i] *  1.0F)) << 15);
               }
            }
            break;
         case GL_UNSIGNED_INT_8_8_8_8:
            if (format == GL_RGBA) {
               GLuint *dst = (GLuint *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (red[i]   * 255.0F)) << 24)
                         | (((GLint) (green[i] * 255.0F)) << 16)
                         | (((GLint) (blue[i]  * 255.0F)) <<  8)
                         | (((GLint) (alpha[i] * 255.0F))      );
               }
            }
            else if (format == GL_BGRA) {
               GLushort *dst = (GLushort *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (blue[i]  * 255.0F)) << 24)
                         | (((GLint) (green[i] * 255.0F)) << 16)
                         | (((GLint) (red[i]   * 255.0F)) <<  8)
                         | (((GLint) (alpha[i] * 255.0F))      );
               }
            }
            else if (format == GL_ABGR_EXT) {
               GLushort *dst = (GLushort *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (alpha[i] * 255.0F)) << 24)
                         | (((GLint) (blue[i]  * 255.0F)) << 16)
                         | (((GLint) (green[i] * 255.0F)) <<  8)
                         | (((GLint) (red[i]   * 255.0F))      );
               }
            }
            break;
         case GL_UNSIGNED_INT_8_8_8_8_REV:
            if (format == GL_RGBA) {
               GLuint *dst = (GLuint *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (red[i]   * 255.0F))      )
                         | (((GLint) (green[i] * 255.0F)) <<  8)
                         | (((GLint) (blue[i]  * 255.0F)) << 16)
                         | (((GLint) (alpha[i] * 255.0F)) << 24);
               }
            }
            else if (format == GL_BGRA) {
               GLushort *dst = (GLushort *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (blue[i]  * 255.0F))      )
                         | (((GLint) (green[i] * 255.0F)) <<  8)
                         | (((GLint) (red[i]   * 255.0F)) << 16)
                         | (((GLint) (alpha[i] * 255.0F)) << 24);
               }
            }
            else if (format == GL_ABGR_EXT) {
               GLushort *dst = (GLushort *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (alpha[i] * 255.0F))      )
                         | (((GLint) (blue[i]  * 255.0F)) <<  8)
                         | (((GLint) (green[i] * 255.0F)) << 16)
                         | (((GLint) (red[i]   * 255.0F)) << 24);
               }
            }
            break;
         case GL_UNSIGNED_INT_10_10_10_2:
            if (format == GL_RGBA) {
               GLuint *dst = (GLuint *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (red[i]   * 1023.0F)) << 22)
                         | (((GLint) (green[i] * 1023.0F)) << 12)
                         | (((GLint) (blue[i]  * 1023.0F)) <<  2)
                         | (((GLint) (alpha[i] *    3.0F))      );
               }
            }
            else if (format == GL_BGRA) {
               GLushort *dst = (GLushort *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (blue[i]  * 1023.0F)) << 22)
                         | (((GLint) (green[i] * 1023.0F)) << 12)
                         | (((GLint) (red[i]   * 1023.0F)) <<  2)
                         | (((GLint) (alpha[i] *    3.0F))      );
               }
            }
            else if (format == GL_ABGR_EXT) {
               GLushort *dst = (GLushort *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (alpha[i] * 1023.0F)) << 22)
                         | (((GLint) (blue[i]  * 1023.0F)) << 12)
                         | (((GLint) (green[i] * 1023.0F)) <<  2)
                         | (((GLint) (red[i]   *    3.0F))      );
               }
            }
            break;
         case GL_UNSIGNED_INT_2_10_10_10_REV:
            if (format == GL_RGBA) {
               GLuint *dst = (GLuint *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (red[i]   * 1023.0F))      )
                         | (((GLint) (green[i] * 1023.0F)) << 10)
                         | (((GLint) (blue[i]  * 1023.0F)) << 20)
                         | (((GLint) (alpha[i] *    3.0F)) << 30);
               }
            }
            else if (format == GL_BGRA) {
               GLushort *dst = (GLushort *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (blue[i]  * 1023.0F))      )
                         | (((GLint) (green[i] * 1023.0F)) << 10)
                         | (((GLint) (red[i]   * 1023.0F)) << 20)
                         | (((GLint) (alpha[i] *    3.0F)) << 30);
               }
            }
            else if (format == GL_ABGR_EXT) {
               GLushort *dst = (GLushort *) destination;
               for (i=0;i<n;i++) {
                  dst[i] = (((GLint) (alpha[i] * 1023.0F))      )
                         | (((GLint) (blue[i]  * 1023.0F)) << 10)
                         | (((GLint) (green[i] * 1023.0F)) << 20)
                         | (((GLint) (red[i]   *    3.0F)) << 30);
               }
            }
            break;
         default:
            gl_problem( ctx, "bad type in gl_pack_rgba_span" );
      }
   }
}
