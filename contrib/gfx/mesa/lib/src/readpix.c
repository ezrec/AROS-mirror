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
 * Revision 1.1  2005/01/11 14:58:32  NicJA
 * AROSMesa 3.0
 *
 * - Based on the official mesa 3 code with major patches to the amigamesa driver code to get it working.
 * - GLUT not yet started (ive left the _old_ mesaaux, mesatk and demos in for this reason)
 * - Doesnt yet work - the _db functions seem to be writing the data incorrectly, and color picking also seems broken somewhat - giving most things a blue tinge (those that are currently working)
 *
 * Revision 3.8  1998/08/04 02:47:49  brianp
 * clipping in read_fast_rgba_pixels() off by one (Randy Frank)
 *
 * Revision 3.7  1998/07/18 03:34:24  brianp
 * GL_ALPHA format was missing
 *
 * Revision 3.6  1998/07/17 03:24:16  brianp
 * added Pixel.ScaleOrBiasRGBA field
 *
 * Revision 3.5  1998/03/27 04:17:52  brianp
 * fixed G++ warnings
 *
 * Revision 3.4  1998/03/15 18:50:25  brianp
 * added GL_EXT_abgr extension
 *
 * Revision 3.3  1998/02/08 20:22:14  brianp
 * LOTS of clean-up and rewriting
 *
 * Revision 3.2  1998/02/01 22:29:09  brianp
 * added support for packed pixel formats
 *
 * Revision 3.1  1998/02/01 20:47:42  brianp
 * added GL_BGR and GL_BGRA pixel formats
 *
 * Revision 3.0  1998/01/31 21:02:29  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "context.h"
#include "depth.h"
#include "feedback.h"
#include "macros.h"
#include "image.h"
#include "pixel.h"
#include "readpix.h"
#include "span.h"
#include "stencil.h"
#include "types.h"
#endif




/*
 * Read a block of color index pixels.
 */
static void read_index_pixels( GLcontext *ctx,
                               GLint x, GLint y,
			       GLsizei width, GLsizei height,
			       GLenum type, GLvoid *pixels )
{
   GLint i, j;

   /* error checking */
   if (ctx->Visual->RGBAflag) {
      gl_error( ctx, GL_INVALID_OPERATION, "glReadPixels" );
      return;
   }

   /* process image row by row */
   for (j=0;j<height;j++,y++) {
      GLuint index[MAX_WIDTH];
      GLvoid *dest;

      (*ctx->Driver.ReadCI32Span)( ctx, width, x, y, index );

      if (ctx->Pixel.IndexShift!=0 || ctx->Pixel.IndexOffset!=0) {
         gl_shift_and_offset_ci( ctx, width, index);
      }

      if (ctx->Pixel.MapColorFlag) {
         gl_map_ci(ctx, width, index);
      }

      dest = gl_pixel_addr_in_image(&ctx->Pack, pixels,
                         width, height, GL_COLOR_INDEX, type, 0, j, 0);

      switch (type) {
	 case GL_UNSIGNED_BYTE:
	    {
               GLubyte *dst = (GLubyte *) dest;
	       for (i=0;i<width;i++) {
		  *dst++ = (GLubyte) index[i];
	       }
	    }
	    break;
	 case GL_BYTE:
	    {
               GLbyte *dst = (GLbyte *) dest;
	       for (i=0;i<width;i++) {
		  dst[i] = (GLbyte) index[i];
	       }
	    }
	    break;
	 case GL_UNSIGNED_SHORT:
	    {
               GLushort *dst = (GLushort *) dest;
	       for (i=0;i<width;i++) {
		  dst[i] = (GLushort) index[i];
	       }
	       if (ctx->Pack.SwapBytes) {
		  gl_swap2( (GLushort *) dst, width );
	       }
	    }
	    break;
	 case GL_SHORT:
	    {
               GLshort *dst = (GLshort *) dest;
	       for (i=0;i<width;i++) {
		  dst[i] = (GLshort) index[i];
	       }
	       if (ctx->Pack.SwapBytes) {
		  gl_swap2( (GLushort *) dst, width );
	       }
	    }
	    break;
	 case GL_UNSIGNED_INT:
	    {
               GLuint *dst = (GLuint *) dest;
	       for (i=0;i<width;i++) {
		  dst[i] = (GLuint) index[i];
	       }
	       if (ctx->Pack.SwapBytes) {
		  gl_swap4( (GLuint *) dst, width );
	       }
	    }
	    break;
	 case GL_INT:
	    {
               GLint *dst = (GLint *) dest;
	       for (i=0;i<width;i++) {
		  dst[i] = (GLint) index[i];
	       }
	       if (ctx->Pack.SwapBytes) {
		  gl_swap4( (GLuint *) dst, width );
	       }
	    }
	    break;
	 case GL_FLOAT:
	    {
               GLfloat *dst = (GLfloat *) dest;
	       for (i=0;i<width;i++) {
		  dst[i] = (GLfloat) index[i];
	       }
	       if (ctx->Pack.SwapBytes) {
		  gl_swap4( (GLuint *) dst, width );
	       }
	    }
	    break;
         default:
            gl_error( ctx, GL_INVALID_ENUM, "glReadPixels(type)" );
      }
   }
}



static void read_depth_pixels( GLcontext *ctx,
                               GLint x, GLint y,
			       GLsizei width, GLsizei height,
			       GLenum type, GLvoid *pixels )
{
   GLint i, j;
   GLboolean bias_or_scale;

   /* Error checking */
   if (ctx->Visual->DepthBits <= 0) {
      /* No depth buffer */
      gl_error( ctx, GL_INVALID_OPERATION, "glReadPixels" );
      return;
   }

   bias_or_scale = ctx->Pixel.DepthBias!=0.0 || ctx->Pixel.DepthScale!=1.0;

   if (type==GL_UNSIGNED_SHORT && sizeof(GLdepth)==sizeof(GLushort)
       && !bias_or_scale && !ctx->Pack.SwapBytes) {
      /* Special case: directly read 16-bit unsigned depth values. */
      for (j=0;j<height;j++,y++) {
         GLushort *dst = (GLushort*) gl_pixel_addr_in_image(&ctx->Pack, pixels,
                         width, height, GL_DEPTH_COMPONENT, type, 0, j, 0);
         (*ctx->Driver.ReadDepthSpanInt)( ctx, width, x, y, (GLdepth*) dst);
      }
   }
   else if (type==GL_UNSIGNED_INT && sizeof(GLdepth)==sizeof(GLuint)
            && !bias_or_scale && !ctx->Pack.SwapBytes) {
      /* Special case: directly read 32-bit unsigned depth values. */
      /* Compute shift value to scale depth values up to 32-bit uints. */
      GLuint shift = 0;
      GLuint max = MAX_DEPTH;
      while ((max&0x80000000)==0) {
         max = max << 1;
         shift++;
      }
      for (j=0;j<height;j++,y++) {
         GLuint *dst = (GLuint *) gl_pixel_addr_in_image(&ctx->Pack, pixels,
                         width, height, GL_DEPTH_COMPONENT, type, 0, j, 0);
         (*ctx->Driver.ReadDepthSpanInt)( ctx, width, x, y, (GLdepth*) dst);
         for (i=0;i<width;i++) {
            dst[i] = dst[i] << shift;
         }
      }
   }
   else {
      /* General case (slow) */
      for (j=0;j<height;j++,y++) {
         GLfloat depth[MAX_WIDTH];
         GLvoid *dest;

         (*ctx->Driver.ReadDepthSpanFloat)( ctx, width, x, y, depth );

         if (bias_or_scale) {
            for (i=0;i<width;i++) {
               GLfloat d;
               d = depth[i] * ctx->Pixel.DepthScale + ctx->Pixel.DepthBias;
               depth[i] = CLAMP( d, 0.0F, 1.0F );
            }
         }

         dest = gl_pixel_addr_in_image(&ctx->Pack, pixels,
                         width, height, GL_DEPTH_COMPONENT, type, 0, j, 0);

         switch (type) {
            case GL_UNSIGNED_BYTE:
               {
                  GLubyte *dst = (GLubyte *) dest;
                  for (i=0;i<width;i++) {
                     dst[i] = FLOAT_TO_UBYTE( depth[i] );
                  }
               }
               break;
            case GL_BYTE:
               {
                  GLbyte *dst = (GLbyte *) dest;
                  for (i=0;i<width;i++) {
                     dst[i] = FLOAT_TO_BYTE( depth[i] );
                  }
               }
               break;
            case GL_UNSIGNED_SHORT:
               {
                  GLushort *dst = (GLushort *) dest;
                  for (i=0;i<width;i++) {
                     dst[i] = FLOAT_TO_USHORT( depth[i] );
                  }
                  if (ctx->Pack.SwapBytes) {
                     gl_swap2( (GLushort *) dst, width );
                  }
               }
               break;
            case GL_SHORT:
               {
                  GLshort *dst = (GLshort *) dest;
                  for (i=0;i<width;i++) {
                     dst[i] = FLOAT_TO_SHORT( depth[i] );
                  }
                  if (ctx->Pack.SwapBytes) {
                     gl_swap2( (GLushort *) dst, width );
                  }
               }
               break;
            case GL_UNSIGNED_INT:
               {
                  GLuint *dst = (GLuint *) dest;
                  for (i=0;i<width;i++) {
                     dst[i] = FLOAT_TO_UINT( depth[i] );
                  }
                  if (ctx->Pack.SwapBytes) {
                     gl_swap4( (GLuint *) dst, width );
                  }
               }
               break;
            case GL_INT:
               {
                  GLint *dst = (GLint *) dest;
                  for (i=0;i<width;i++) {
                     dst[i] = FLOAT_TO_INT( depth[i] );
                  }
                  if (ctx->Pack.SwapBytes) {
                     gl_swap4( (GLuint *) dst, width );
                  }
               }
               break;
            case GL_FLOAT:
               {
                  GLfloat *dst = (GLfloat *) dest;
                  for (i=0;i<width;i++) {
                     dst[i] = depth[i];
                  }
                  if (ctx->Pack.SwapBytes) {
                     gl_swap4( (GLuint *) dst, width );
                  }
               }
               break;
            default:
               gl_error( ctx, GL_INVALID_ENUM, "glReadPixels(type)" );
         }
      }
   }
}




static void read_stencil_pixels( GLcontext *ctx,
                                 GLint x, GLint y,
				 GLsizei width, GLsizei height,
				 GLenum type, GLvoid *pixels )
{
   GLboolean shift_or_offset;
   GLint i, j;

   if (ctx->Visual->StencilBits<=0) {
      /* No stencil buffer */
      gl_error( ctx, GL_INVALID_OPERATION, "glReadPixels" );
      return;
   }

   shift_or_offset = ctx->Pixel.IndexShift!=0 || ctx->Pixel.IndexOffset!=0;

   /* process image row by row */
   for (j=0;j<height;j++,y++) {
      GLvoid *dest;
      GLubyte stencil[MAX_WIDTH];

      gl_read_stencil_span( ctx, width, x, y, stencil );

      if (shift_or_offset) {
         gl_shift_and_offset_stencil( ctx, width, stencil );
      }

      if (ctx->Pixel.MapStencilFlag) {
         gl_map_stencil( ctx, width, stencil );
      }

      dest = gl_pixel_addr_in_image(&ctx->Pack, pixels,
                          width, height, GL_STENCIL_INDEX, type, 0, j, 0);

      switch (type) {
	 case GL_UNSIGNED_BYTE:
            MEMCPY( dest, stencil, width );
	    break;
	 case GL_BYTE:
            MEMCPY( dest, stencil, width );
	    break;
	 case GL_UNSIGNED_SHORT:
	    {
               GLushort *dst = (GLushort *) dest;
	       for (i=0;i<width;i++) {
		  dst[i] = (GLushort) stencil[i];
	       }
	       if (ctx->Pack.SwapBytes) {
		  gl_swap2( (GLushort *) dst, width );
	       }
	    }
	    break;
	 case GL_SHORT:
	    {
               GLshort *dst = (GLshort *) dest;
	       for (i=0;i<width;i++) {
		  dst[i] = (GLshort) stencil[i];
	       }
	       if (ctx->Pack.SwapBytes) {
		  gl_swap2( (GLushort *) dst, width );
	       }
	    }
	    break;
	 case GL_UNSIGNED_INT:
	    {
               GLuint *dst = (GLuint *) dest;
	       for (i=0;i<width;i++) {
		  dst[i] = (GLuint) stencil[i];
	       }
	       if (ctx->Pack.SwapBytes) {
		  gl_swap4( (GLuint *) dst, width );
	       }
	    }
	    break;
	 case GL_INT:
	    {
               GLint *dst = (GLint *) dest;
	       for (i=0;i<width;i++) {
		  *dst++ = (GLint) stencil[i];
	       }
	       if (ctx->Pack.SwapBytes) {
		  gl_swap4( (GLuint *) dst, width );
	       }
	    }
	    break;
	 case GL_FLOAT:
	    {
               GLfloat *dst = (GLfloat *) dest;
	       for (i=0;i<width;i++) {
		  dst[i] = (GLfloat) stencil[i];
	       }
	       if (ctx->Pack.SwapBytes) {
		  gl_swap4( (GLuint *) dst, width );
	       }
	    }
	    break;
         default:
            gl_error( ctx, GL_INVALID_ENUM, "glReadPixels(type)" );
      }
   }
}



/*
 * Optimized glReadPixels for particular pixel formats:
 *   GL_UNSIGNED_BYTE, GL_RGBA
 * when pixel scaling, biasing and mapping are disabled.
 */
static GLboolean read_fast_rgba_pixels( GLcontext *ctx,
                                        GLint x, GLint y,
                                        GLsizei width, GLsizei height,
                                        GLenum format, GLenum type,
                                        GLvoid *pixels )
{
   /* can't do scale, bias or mapping */
   if (ctx->Pixel.ScaleOrBiasRGBA || ctx->Pixel.MapColorFlag)
       return GL_FALSE;

   /* can't do fancy pixel packing */
   if (ctx->Pack.Alignment != 1 || ctx->Pack.SwapBytes || ctx->Pack.LsbFirst)
      return GL_FALSE;

   {
      GLint srcX = x;
      GLint srcY = y;
      GLint readWidth = width;           /* actual width read */
      GLint readHeight = height;         /* actual height read */
      GLint skipPixels = ctx->Pack.SkipPixels;
      GLint skipRows = ctx->Pack.SkipRows;
      GLint rowLength;

      if (ctx->Pack.RowLength > 0)
         rowLength = ctx->Pack.RowLength;
      else
         rowLength = width;

      /* horizontal clipping */
      if (srcX < ctx->Buffer->Xmin) {
         skipPixels += (ctx->Buffer->Xmin - srcX);
         readWidth  -= (ctx->Buffer->Xmin - srcX);
         srcX = ctx->Buffer->Xmin;
      }
      if (srcX + readWidth > ctx->Buffer->Xmax)
         readWidth -= (srcX + readWidth - ctx->Buffer->Xmax - 1);
      if (readWidth <= 0)
         return GL_TRUE;

      /* vertical clipping */
      if (srcY < ctx->Buffer->Ymin) {
         skipRows   += (ctx->Buffer->Ymin - srcY);
         readHeight -= (ctx->Buffer->Ymin - srcY);
         srcY = ctx->Buffer->Ymin;
      }
      if (srcY + readHeight > ctx->Buffer->Ymax)
         readHeight -= (srcY + readHeight - ctx->Buffer->Ymax - 1);
      if (readHeight <= 0)
         return GL_TRUE;

      /*
       * Ready to read!
       * The window region at (destX, destY) of size (drawWidth, drawHeight)
       * will be read back.
       * We'll write pixel data to buffer pointed to by "pixels" but we'll
       * skip "skipRows" rows and skip "skipPixels" pixels/row.
 */
      if (format==GL_RGBA && type==GL_UNSIGNED_BYTE) {
         GLubyte *dest = (GLubyte *) pixels
                         + (skipRows * rowLength + skipPixels) * 4;
         GLint row;
         for (row=0; row<readHeight; row++) {
            (*ctx->Driver.ReadRGBASpan)(ctx, readWidth, srcX, srcY,
                                        (void *) dest);
            dest += rowLength * 4;
            srcY++;
         }
         return GL_TRUE;
      }
      else {
         /* can't do this format/type combination */
         return GL_FALSE;
   }
   }
}



/*
 * Read R, G, B, A, RGB, L, or LA pixels.
 */
static void read_rgba_pixels( GLcontext *ctx,
                               GLint x, GLint y,
			       GLsizei width, GLsizei height,
			       GLenum format, GLenum type, GLvoid *pixels )
{
   GLint i, j, n, s;
   DEFARRAY(GLfloat, red, MAX_WIDTH);
   DEFARRAY(GLfloat, green, MAX_WIDTH);
   DEFARRAY(GLfloat, blue, MAX_WIDTH);
   DEFARRAY(GLfloat, alpha, MAX_WIDTH);
   DEFARRAY(GLfloat, luminance, MAX_WIDTH);

   /* number of components */
   n = gl_components_in_format(format);
   if (n <= 0) {
      gl_error(ctx, GL_INVALID_ENUM, "glReadPixels(format)");
         UNDEFARRAY( red );
         UNDEFARRAY( green );
         UNDEFARRAY( blue );
         UNDEFARRAY( alpha );
         UNDEFARRAY( luminance );
         return;
   }

   /* Size of each component */
   s = gl_sizeof_type( type );
   if (s <= 0) {
      gl_error( ctx, GL_INVALID_ENUM, "glReadPixels(type)" );
      UNDEFARRAY( red );
      UNDEFARRAY( green );
      UNDEFARRAY( blue );
      UNDEFARRAY( alpha );
      UNDEFARRAY( luminance );
      return;
   }

   /* process image row by row */
   for (j=0;j<height;j++,y++) {

      /*
       * Read the pixels from frame buffer
       */
      if (ctx->Visual->RGBAflag) {
         GLubyte rgba[MAX_WIDTH][4];
	 GLfloat rscale = (1.0F / 255.0F);
	 GLfloat gscale = (1.0F / 255.0F);
	 GLfloat bscale = (1.0F / 255.0F);
	 GLfloat ascale = (1.0F / 255.0F);

	 /* read colors and convert to floats */
         gl_read_rgba_span(ctx, width, x, y, rgba );
	 for (i=0;i<width;i++) {
	    red[i]   = rgba[i][RCOMP] * rscale;
	    green[i] = rgba[i][GCOMP] * gscale;
	    blue[i]  = rgba[i][BCOMP] * bscale;
	    alpha[i] = rgba[i][ACOMP] * ascale;
	 }

	 if (ctx->Pixel.ScaleOrBiasRGBA) {
	    gl_scale_and_bias_color( ctx, width, red, green, blue, alpha );
	 }
	 if (ctx->Pixel.MapColorFlag) {
	    gl_map_color( ctx, width, red, green, blue, alpha );
	 }
      }
      else {
	 /* convert CI values to RGBA */
	 GLuint index[MAX_WIDTH];
	 (*ctx->Driver.ReadCI32Span)( ctx, width, x, y, index );

	 if (ctx->Pixel.IndexShift!=0 || ctx->Pixel.IndexOffset!=0) {
            gl_map_ci( ctx, width, index );
	 }

         gl_map_ci_to_color(ctx, width, index, red, green, blue, alpha);
      }

      if (format==GL_LUMINANCE || format==GL_LUMINANCE_ALPHA) {
         for (i=0;i<width;i++) {
            GLfloat sum = red[i] + green[i] + blue[i];
            luminance[i] = CLAMP( sum, 0.0F, 1.0F );
         }
      }

      /*
       * Pack/transfer/store the pixels
       */

      /* XXX
       * XXX rewrite this to use the new gl_pack_rgba_span function!!!
       * XXX
       */

      switch (type) {
	 case GL_UNSIGNED_BYTE:
	    {
               GLubyte *dst = (GLubyte *) gl_pixel_addr_in_image(&ctx->Pack,
                              pixels, width, height, format, type, 0, j, 0);
               switch (format) {
                  case GL_RED:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_UBYTE(red[i]);
                     break;
                  case GL_GREEN:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_UBYTE(green[i]);
                     break;
                  case GL_BLUE:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_UBYTE(blue[i]);
                     break;
                  case GL_ALPHA:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_UBYTE(alpha[i]);
                     break;
                  case GL_LUMINANCE:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_UBYTE(luminance[i]);
                     break;
                  case GL_LUMINANCE_ALPHA:
                     for (i=0;i<width;i++) {
                        dst[i*2+0] = FLOAT_TO_UBYTE(luminance[i]);
                        dst[i*2+1] = FLOAT_TO_UBYTE(alpha[i]);
                     }
                     break;
                  case GL_RGB:
                     for (i=0;i<width;i++) {
                        dst[i*3+0] = FLOAT_TO_UBYTE(red[i]);
                        dst[i*3+1] = FLOAT_TO_UBYTE(green[i]);
                        dst[i*3+2] = FLOAT_TO_UBYTE(blue[i]);
                     }
                     break;
                  case GL_RGBA:
                     for (i=0;i<width;i++) {
                        dst[i*4+0] = FLOAT_TO_UBYTE(red[i]);
                        dst[i*4+1] = FLOAT_TO_UBYTE(green[i]);
                        dst[i*4+2] = FLOAT_TO_UBYTE(blue[i]);
                        dst[i*4+3] = FLOAT_TO_UBYTE(alpha[i]);
                     }
                     break;
                  case GL_BGR:
	       for (i=0;i<width;i++) {
                        dst[i*3+0] = FLOAT_TO_UBYTE(blue[i]);
                        dst[i*3+1] = FLOAT_TO_UBYTE(green[i]);
                        dst[i*3+2] = FLOAT_TO_UBYTE(red[i]);
                     }
                     break;
                  case GL_BGRA:
                     for (i=0;i<width;i++) {
                        dst[i*4+0] = FLOAT_TO_UBYTE(blue[i]);
                        dst[i*4+1] = FLOAT_TO_UBYTE(green[i]);
                        dst[i*4+2] = FLOAT_TO_UBYTE(red[i]);
                        dst[i*4+3] = FLOAT_TO_UBYTE(alpha[i]);
                     }
                     break;
                  case GL_ABGR_EXT:
                     for (i=0;i<width;i++) {
                        dst[i*4+0] = FLOAT_TO_UBYTE(alpha[i]);
                        dst[i*4+1] = FLOAT_TO_UBYTE(blue[i]);
                        dst[i*4+2] = FLOAT_TO_UBYTE(green[i]);
                        dst[i*4+3] = FLOAT_TO_UBYTE(red[i]);
                     }
                     break;
                  default:
                     gl_problem(ctx, "bad format in glReadPixels\n");
	       }
	    }
	    break;
	 case GL_BYTE:
	    {
               GLbyte *dst = (GLbyte *) gl_pixel_addr_in_image(&ctx->Pack,
                             pixels, width, height, format, type, 0, j, 0);
               switch (format) {
                  case GL_RED:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_BYTE(red[i]);
                     break;
                  case GL_GREEN:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_BYTE(green[i]);
                     break;
                  case GL_BLUE:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_BYTE(blue[i]);
                     break;
                  case GL_ALPHA:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_BYTE(alpha[i]);
                     break;
                  case GL_LUMINANCE:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_BYTE(luminance[i]);
                     break;
                  case GL_LUMINANCE_ALPHA:
                     for (i=0;i<width;i++) {
                        dst[i*2+0] = FLOAT_TO_BYTE(luminance[i]);
                        dst[i*2+1] = FLOAT_TO_BYTE(alpha[i]);
                     }
                     break;
                  case GL_RGB:
	       for (i=0;i<width;i++) {
                        dst[i*3+0] = FLOAT_TO_BYTE(red[i]);
                        dst[i*3+1] = FLOAT_TO_BYTE(green[i]);
                        dst[i*3+2] = FLOAT_TO_BYTE(blue[i]);
                     }
                     break;
                  case GL_RGBA:
                     for (i=0;i<width;i++) {
                        dst[i*4+0] = FLOAT_TO_BYTE(red[i]);
                        dst[i*4+1] = FLOAT_TO_BYTE(green[i]);
                        dst[i*4+2] = FLOAT_TO_BYTE(blue[i]);
                        dst[i*4+3] = FLOAT_TO_BYTE(alpha[i]);
                     }
                     break;
                  case GL_BGR:
                     for (i=0;i<width;i++) {
                        dst[i*3+0] = FLOAT_TO_BYTE(blue[i]);
                        dst[i*3+1] = FLOAT_TO_BYTE(green[i]);
                        dst[i*3+2] = FLOAT_TO_BYTE(red[i]);
                     }
                     break;
                  case GL_BGRA:
                     for (i=0;i<width;i++) {
                        dst[i*4+0] = FLOAT_TO_BYTE(blue[i]);
                        dst[i*4+1] = FLOAT_TO_BYTE(green[i]);
                        dst[i*4+2] = FLOAT_TO_BYTE(red[i]);
                        dst[i*4+3] = FLOAT_TO_BYTE(alpha[i]);
                     }
                  case GL_ABGR_EXT:
                     for (i=0;i<width;i++) {
                        dst[i*4+0] = FLOAT_TO_BYTE(alpha[i]);
                        dst[i*4+1] = FLOAT_TO_BYTE(blue[i]);
                        dst[i*4+2] = FLOAT_TO_BYTE(green[i]);
                        dst[i*4+3] = FLOAT_TO_BYTE(red[i]);
                     }
                     break;
                  default:
                     gl_problem(ctx, "bad format in glReadPixels\n");
	       }
	    }
	    break;
	 case GL_UNSIGNED_SHORT:
	    {
               GLushort *dst = (GLushort *) gl_pixel_addr_in_image(&ctx->Pack,
                               pixels, width, height, format, type, 0, j, 0);
               switch (format) {
                  case GL_RED:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_USHORT(red[i]);
                     break;
                  case GL_GREEN:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_USHORT(green[i]);
                     break;
                  case GL_BLUE:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_USHORT(blue[i]);
                     break;
                  case GL_ALPHA:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_USHORT(alpha[i]);
                     break;
                  case GL_LUMINANCE:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_USHORT(luminance[i]);
                     break;
                  case GL_LUMINANCE_ALPHA:
                     for (i=0;i<width;i++) {
                        dst[i*2+0] = FLOAT_TO_USHORT(luminance[i]);
                        dst[i*2+1] = FLOAT_TO_USHORT(alpha[i]);
                     }
                     break;
                  case GL_RGB:
                     for (i=0;i<width;i++) {
                        dst[i*3+0] = FLOAT_TO_USHORT(red[i]);
                        dst[i*3+1] = FLOAT_TO_USHORT(green[i]);
                        dst[i*3+2] = FLOAT_TO_USHORT(blue[i]);
                     }
                     break;
                  case GL_RGBA:
                     for (i=0;i<width;i++) {
                        dst[i*4+0] = FLOAT_TO_USHORT(red[i]);
                        dst[i*4+1] = FLOAT_TO_USHORT(green[i]);
                        dst[i*4+2] = FLOAT_TO_USHORT(blue[i]);
                        dst[i*4+3] = FLOAT_TO_USHORT(alpha[i]);
                     }
                     break;
                  case GL_BGR:
                     for (i=0;i<width;i++) {
                        dst[i*3+0] = FLOAT_TO_USHORT(blue[i]);
                        dst[i*3+1] = FLOAT_TO_USHORT(green[i]);
                        dst[i*3+2] = FLOAT_TO_USHORT(red[i]);
                     }
                     break;
                  case GL_BGRA:
	       for (i=0;i<width;i++) {
                        dst[i*4+0] = FLOAT_TO_USHORT(blue[i]);
                        dst[i*4+1] = FLOAT_TO_USHORT(green[i]);
                        dst[i*4+2] = FLOAT_TO_USHORT(red[i]);
                        dst[i*4+3] = FLOAT_TO_USHORT(alpha[i]);
	       }
                     break;
                  case GL_ABGR_EXT:
                     for (i=0;i<width;i++) {
                        dst[i*4+0] = FLOAT_TO_USHORT(alpha[i]);
                        dst[i*4+1] = FLOAT_TO_USHORT(blue[i]);
                        dst[i*4+2] = FLOAT_TO_USHORT(green[i]);
                        dst[i*4+3] = FLOAT_TO_USHORT(red[i]);
                     }
                     break;
                  default:
                     gl_problem(ctx, "bad format in glReadPixels\n");
	    }
	    if (ctx->Pack.SwapBytes) {
                  gl_swap2( (GLushort *) dst, width*n );
               }
	    }
	    break;
	 case GL_SHORT:
	    {
               GLshort *dst = (GLshort *) gl_pixel_addr_in_image(&ctx->Pack,
                              pixels, width, height, format, type, 0, j, 0);
               switch (format) {
                  case GL_RED:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_SHORT(red[i]);
                     break;
                  case GL_GREEN:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_SHORT(green[i]);
                     break;
                  case GL_BLUE:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_SHORT(blue[i]);
                     break;
                  case GL_ALPHA:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_SHORT(alpha[i]);
                     break;
                  case GL_LUMINANCE:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_SHORT(luminance[i]);
                     break;
                  case GL_LUMINANCE_ALPHA:
	       for (i=0;i<width;i++) {
                        dst[i*2+0] = FLOAT_TO_SHORT(luminance[i]);
                        dst[i*2+1] = FLOAT_TO_SHORT(alpha[i]);
                     }
                     break;
                  case GL_RGB:
                     for (i=0;i<width;i++) {
                        dst[i*3+0] = FLOAT_TO_SHORT(red[i]);
                        dst[i*3+1] = FLOAT_TO_SHORT(green[i]);
                        dst[i*3+2] = FLOAT_TO_SHORT(blue[i]);
                     }
                     break;
                  case GL_RGBA:
                     for (i=0;i<width;i++) {
                        dst[i*4+0] = FLOAT_TO_SHORT(red[i]);
                        dst[i*4+1] = FLOAT_TO_SHORT(green[i]);
                        dst[i*4+2] = FLOAT_TO_SHORT(blue[i]);
                        dst[i*4+3] = FLOAT_TO_SHORT(alpha[i]);
                     }
                     break;
                  case GL_BGR:
                     for (i=0;i<width;i++) {
                        dst[i*3+0] = FLOAT_TO_SHORT(blue[i]);
                        dst[i*3+1] = FLOAT_TO_SHORT(green[i]);
                        dst[i*3+2] = FLOAT_TO_SHORT(red[i]);
                     }
                     break;
                  case GL_BGRA:
                     for (i=0;i<width;i++) {
                        dst[i*4+0] = FLOAT_TO_SHORT(blue[i]);
                        dst[i*4+1] = FLOAT_TO_SHORT(green[i]);
                        dst[i*4+2] = FLOAT_TO_SHORT(red[i]);
                        dst[i*4+3] = FLOAT_TO_SHORT(alpha[i]);
                     }
                  case GL_ABGR_EXT:
                     for (i=0;i<width;i++) {
                        dst[i*4+0] = FLOAT_TO_SHORT(alpha[i]);
                        dst[i*4+1] = FLOAT_TO_SHORT(blue[i]);
                        dst[i*4+2] = FLOAT_TO_SHORT(green[i]);
                        dst[i*4+3] = FLOAT_TO_SHORT(red[i]);
                     }
                     break;
                  default:
                     gl_problem(ctx, "bad format in glReadPixels\n");
	       }
	       if (ctx->Pack.SwapBytes) {
                  gl_swap2( (GLushort *) dst, width*n );
	       }
	    }
	    break;
	 case GL_UNSIGNED_INT:
	    {
               GLuint *dst = (GLuint *) gl_pixel_addr_in_image(&ctx->Pack,
                             pixels, width, height, format, type, 0, j, 0);
               switch (format) {
                  case GL_RED:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_UINT(red[i]);
                     break;
                  case GL_GREEN:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_UINT(green[i]);
                     break;
                  case GL_BLUE:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_UINT(blue[i]);
                     break;
                  case GL_ALPHA:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_UINT(alpha[i]);
                     break;
                  case GL_LUMINANCE:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_UINT(luminance[i]);
                     break;
                  case GL_LUMINANCE_ALPHA:
	       for (i=0;i<width;i++) {
                        dst[i*2+0] = FLOAT_TO_UINT(luminance[i]);
                        dst[i*2+1] = FLOAT_TO_UINT(alpha[i]);
                     }
                     break;
                  case GL_RGB:
                     for (i=0;i<width;i++) {
                        dst[i*3+0] = FLOAT_TO_UINT(red[i]);
                        dst[i*3+1] = FLOAT_TO_UINT(green[i]);
                        dst[i*3+2] = FLOAT_TO_UINT(blue[i]);
                     }
                     break;
                  case GL_RGBA:
                     for (i=0;i<width;i++) {
                        dst[i*4+0] = FLOAT_TO_UINT(red[i]);
                        dst[i*4+1] = FLOAT_TO_UINT(green[i]);
                        dst[i*4+2] = FLOAT_TO_UINT(blue[i]);
                        dst[i*4+3] = FLOAT_TO_UINT(alpha[i]);
                     }
                     break;
                  case GL_BGR:
                     for (i=0;i<width;i++) {
                        dst[i*3+0] = FLOAT_TO_UINT(blue[i]);
                        dst[i*3+1] = FLOAT_TO_UINT(green[i]);
                        dst[i*3+2] = FLOAT_TO_UINT(red[i]);
                     }
                     break;
                  case GL_BGRA:
                     for (i=0;i<width;i++) {
                        dst[i*4+0] = FLOAT_TO_UINT(blue[i]);
                        dst[i*4+1] = FLOAT_TO_UINT(green[i]);
                        dst[i*4+2] = FLOAT_TO_UINT(red[i]);
                        dst[i*4+3] = FLOAT_TO_UINT(alpha[i]);
                     }
                     break;
                  case GL_ABGR_EXT:
                     for (i=0;i<width;i++) {
                        dst[i*4+0] = FLOAT_TO_UINT(alpha[i]);
                        dst[i*4+1] = FLOAT_TO_UINT(blue[i]);
                        dst[i*4+2] = FLOAT_TO_UINT(green[i]);
                        dst[i*4+3] = FLOAT_TO_UINT(red[i]);
                     }
                     break;
                  default:
                     gl_problem(ctx, "bad format in glReadPixels\n");
	       }
	       if (ctx->Pack.SwapBytes) {
                  gl_swap4( (GLuint *) dst, width*n );
	       }
	    }
	    break;
	 case GL_INT:
	    {
               GLint *dst = (GLint *) gl_pixel_addr_in_image(&ctx->Pack,
                            pixels, width, height, format, type, 0, j, 0);
               switch (format) {
                  case GL_RED:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_INT(red[i]);
                     break;
                  case GL_GREEN:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_INT(green[i]);
                     break;
                  case GL_BLUE:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_INT(blue[i]);
                     break;
                  case GL_ALPHA:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_INT(alpha[i]);
                     break;
                  case GL_LUMINANCE:
                     for (i=0;i<width;i++)
                        dst[i] = FLOAT_TO_INT(luminance[i]);
                     break;
                  case GL_LUMINANCE_ALPHA:
                     for (i=0;i<width;i++) {
                        dst[i*2+0] = FLOAT_TO_INT(luminance[i]);
                        dst[i*2+1] = FLOAT_TO_INT(alpha[i]);
                     }
                     break;
                  case GL_RGB:
                     for (i=0;i<width;i++) {
                        dst[i*3+0] = FLOAT_TO_INT(red[i]);
                        dst[i*3+1] = FLOAT_TO_INT(green[i]);
                        dst[i*3+2] = FLOAT_TO_INT(blue[i]);
                     }
                     break;
                  case GL_RGBA:
                     for (i=0;i<width;i++) {
                        dst[i*4+0] = FLOAT_TO_INT(red[i]);
                        dst[i*4+1] = FLOAT_TO_INT(green[i]);
                        dst[i*4+2] = FLOAT_TO_INT(blue[i]);
                        dst[i*4+3] = FLOAT_TO_INT(alpha[i]);
                     }
                     break;
                  case GL_BGR:
	       for (i=0;i<width;i++) {
                        dst[i*3+0] = FLOAT_TO_INT(blue[i]);
                        dst[i*3+1] = FLOAT_TO_INT(green[i]);
                        dst[i*3+2] = FLOAT_TO_INT(red[i]);
                     }
                     break;
                  case GL_BGRA:
                     for (i=0;i<width;i++) {
                        dst[i*4+0] = FLOAT_TO_INT(blue[i]);
                        dst[i*4+1] = FLOAT_TO_INT(green[i]);
                        dst[i*4+2] = FLOAT_TO_INT(red[i]);
                        dst[i*4+3] = FLOAT_TO_INT(alpha[i]);
                     }
                     break;
                  case GL_ABGR_EXT:
                     for (i=0;i<width;i++) {
                        dst[i*4+0] = FLOAT_TO_INT(alpha[i]);
                        dst[i*4+1] = FLOAT_TO_INT(blue[i]);
                        dst[i*4+2] = FLOAT_TO_INT(green[i]);
                        dst[i*4+3] = FLOAT_TO_INT(red[i]);
                     }
                     break;
                  default:
                     gl_problem(ctx, "bad format in glReadPixels\n");
	       }
	       if (ctx->Pack.SwapBytes) {
		  gl_swap4( (GLuint *) dst, width*n );
	       }
	    }
	    break;
	 case GL_FLOAT:
	    {
               GLfloat *dst = (GLfloat *) gl_pixel_addr_in_image(&ctx->Pack,
                              pixels, width, height, format, type, 0, j, 0);
               switch (format) {
                  case GL_RED:
                     for (i=0;i<width;i++)
                        dst[i] = red[i];
                     break;
                  case GL_GREEN:
                     for (i=0;i<width;i++)
                        dst[i] = green[i];
                     break;
                  case GL_BLUE:
                     for (i=0;i<width;i++)
                        dst[i] = blue[i];
                     break;
                  case GL_ALPHA:
                     for (i=0;i<width;i++)
                        dst[i] = alpha[i];
                     break;
                  case GL_LUMINANCE:
                     for (i=0;i<width;i++)
                        dst[i] = luminance[i];
                     break;
                  case GL_LUMINANCE_ALPHA:
                     for (i=0;i<width;i++) {
                        dst[i*2+0] = luminance[i];
                        dst[i*2+1] = alpha[i];
                     }
                     break;
                  case GL_RGB:
                     for (i=0;i<width;i++) {
                        dst[i*3+0] = red[i];
                        dst[i*3+1] = green[i];
                        dst[i*3+2] = blue[i];
                     }
                     break;
                  case GL_RGBA:
                     for (i=0;i<width;i++) {
                        dst[i*4+0] = red[i];
                        dst[i*4+1] = green[i];
                        dst[i*4+2] = blue[i];
                        dst[i*4+3] = alpha[i];
                     }
                     break;
                  case GL_BGR:
                     for (i=0;i<width;i++) {
                        dst[i*3+0] = blue[i];
                        dst[i*3+1] = green[i];
                        dst[i*3+2] = red[i];
                     }
                     break;
                  case GL_BGRA:
                     for (i=0;i<width;i++) {
                        dst[i*4+0] = blue[i];
                        dst[i*4+1] = green[i];
                        dst[i*4+2] = red[i];
                        dst[i*4+3] = alpha[i];
                     }
                     break;
                  case GL_ABGR_EXT:
	       for (i=0;i<width;i++) {
                        dst[i*4+0] = alpha[i];
                        dst[i*4+1] = blue[i];
                        dst[i*4+2] = green[i];
                        dst[i*4+3] = red[i];
                     }
                     break;
                  default:
                     gl_problem(ctx, "bad format in glReadPixels\n");
	       }
	       if (ctx->Pack.SwapBytes) {
		  gl_swap4( (GLuint *) dst, width*n );
	       }
	    }
	    break;
         case GL_UNSIGNED_BYTE_3_3_2:
            if (format == GL_RGB) {
               GLubyte *dst = (GLubyte *) gl_pixel_addr_in_image(&ctx->Pack,
                              pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (red[i]   * 7.0F)) << 5)
                         | (((GLint) (green[i] * 7.0F)) << 2)
                         | (((GLint) (blue[i]  * 3.0F))     );
               }
            }
            break;
         case GL_UNSIGNED_BYTE_2_3_3_REV:
            if (format == GL_RGB) {
               GLubyte *dst = (GLubyte *) gl_pixel_addr_in_image(&ctx->Pack,
                              pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (red[i]   * 7.0F))     )
                         | (((GLint) (green[i] * 7.0F)) << 3)
                         | (((GLint) (blue[i]  * 3.0F)) << 5);
               }
            }
            break;
         case GL_UNSIGNED_SHORT_5_6_5:
            if (format == GL_RGB) {
               GLushort *dst = (GLushort *) gl_pixel_addr_in_image(&ctx->Pack,
                               pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (red[i]   * 31.0F)) << 11)
                         | (((GLint) (green[i] * 63.0F)) <<  5)
                         | (((GLint) (blue[i]  * 31.0F))      );
               }
            }
            break;
         case GL_UNSIGNED_SHORT_5_6_5_REV:
            if (format == GL_RGB) {
               GLushort *dst = (GLushort *) gl_pixel_addr_in_image(&ctx->Pack,
                               pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (red[i]   * 31.0F))      )
                         | (((GLint) (green[i] * 63.0F)) <<  5)
                         | (((GLint) (blue[i]  * 31.0F)) << 11);
               }
            }
            break;
         case GL_UNSIGNED_SHORT_4_4_4_4:
            if (format == GL_RGB) {
               GLushort *dst = (GLushort *) gl_pixel_addr_in_image(&ctx->Pack,
                               pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (red[i]   * 15.0F)) << 12)
                         | (((GLint) (green[i] * 15.0F)) <<  8)
                         | (((GLint) (blue[i]  * 15.0F)) <<  4)
                         | (((GLint) (alpha[i] * 15.0F))      );
               }
            }
            break;
         case GL_UNSIGNED_SHORT_4_4_4_4_REV:
            if (format == GL_RGB) {
               GLushort *dst = (GLushort *) gl_pixel_addr_in_image(&ctx->Pack,
                               pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (red[i]   * 15.0F))      )
                         | (((GLint) (green[i] * 15.0F)) <<  4)
                         | (((GLint) (blue[i]  * 15.0F)) <<  8)
                         | (((GLint) (alpha[i] * 15.0F)) << 12);
               }
            }
            break;
         case GL_UNSIGNED_SHORT_5_5_5_1:
            if (format == GL_RGB) {
               GLushort *dst = (GLushort *) gl_pixel_addr_in_image(&ctx->Pack,
                               pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (red[i]   * 31.0F)) << 11)
                         | (((GLint) (green[i] * 31.0F)) <<  6)
                         | (((GLint) (blue[i]  * 31.0F)) <<  1)
                         | (((GLint) (alpha[i] *  1.0F))      );
               }
            }
            break;
         case GL_UNSIGNED_SHORT_1_5_5_5_REV:
            if (format == GL_RGB) {
               GLushort *dst = (GLushort *) gl_pixel_addr_in_image(&ctx->Pack,
                               pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (red[i]   * 31.0F))      )
                         | (((GLint) (green[i] * 31.0F)) <<  5)
                         | (((GLint) (blue[i]  * 31.0F)) << 10)
                         | (((GLint) (alpha[i] *  1.0F)) << 15);
               }
            }
            break;
         case GL_UNSIGNED_INT_8_8_8_8:
            if (format == GL_RGBA) {
               GLuint *dst = (GLuint *) gl_pixel_addr_in_image(&ctx->Pack,
                             pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (red[i]   * 255.0F)) << 24)
                         | (((GLint) (green[i] * 255.0F)) << 16)
                         | (((GLint) (blue[i]  * 255.0F)) <<  8)
                         | (((GLint) (alpha[i] * 255.0F))      );
               }
            }
            else if (format == GL_BGRA) {
               GLushort *dst = (GLushort *) gl_pixel_addr_in_image(&ctx->Pack,
                               pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (blue[i]  * 255.0F)) << 24)
                         | (((GLint) (green[i] * 255.0F)) << 16)
                         | (((GLint) (red[i]   * 255.0F)) <<  8)
                         | (((GLint) (alpha[i] * 255.0F))      );
               }
            }
            else if (format == GL_ABGR_EXT) {
               GLushort *dst = (GLushort *) gl_pixel_addr_in_image(&ctx->Pack,
                               pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (alpha[i] * 255.0F)) << 24)
                         | (((GLint) (blue[i]  * 255.0F)) << 16)
                         | (((GLint) (green[i] * 255.0F)) <<  8)
                         | (((GLint) (red[i]   * 255.0F))      );
               }
            }
            break;
         case GL_UNSIGNED_INT_8_8_8_8_REV:
            if (format == GL_RGBA) {
               GLuint *dst = (GLuint *) gl_pixel_addr_in_image(&ctx->Pack,
                             pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (red[i]   * 255.0F))      )
                         | (((GLint) (green[i] * 255.0F)) <<  8)
                         | (((GLint) (blue[i]  * 255.0F)) << 16)
                         | (((GLint) (alpha[i] * 255.0F)) << 24);
               }
            }
            else if (format == GL_BGRA) {
               GLushort *dst = (GLushort *) gl_pixel_addr_in_image(&ctx->Pack,
                               pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (blue[i]  * 255.0F))      )
                         | (((GLint) (green[i] * 255.0F)) <<  8)
                         | (((GLint) (red[i]   * 255.0F)) << 16)
                         | (((GLint) (alpha[i] * 255.0F)) << 24);
               }
            }
            else if (format == GL_ABGR_EXT) {
               GLushort *dst = (GLushort *) gl_pixel_addr_in_image(&ctx->Pack,
                               pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (alpha[i] * 255.0F))      )
                         | (((GLint) (blue[i]  * 255.0F)) <<  8)
                         | (((GLint) (green[i] * 255.0F)) << 16)
                         | (((GLint) (red[i]   * 255.0F)) << 24);
               }
            }
            break;
         case GL_UNSIGNED_INT_10_10_10_2:
            if (format == GL_RGBA) {
               GLuint *dst = (GLuint *) gl_pixel_addr_in_image(&ctx->Pack,
                             pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (red[i]   * 1023.0F)) << 22)
                         | (((GLint) (green[i] * 1023.0F)) << 12)
                         | (((GLint) (blue[i]  * 1023.0F)) <<  2)
                         | (((GLint) (alpha[i] *    3.0F))      );
               }
            }
            else if (format == GL_BGRA) {
               GLushort *dst = (GLushort *) gl_pixel_addr_in_image(&ctx->Pack,
                               pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (blue[i]  * 1023.0F)) << 22)
                         | (((GLint) (green[i] * 1023.0F)) << 12)
                         | (((GLint) (red[i]   * 1023.0F)) <<  2)
                         | (((GLint) (alpha[i] *    3.0F))      );
               }
            }
            else if (format == GL_ABGR_EXT) {
               GLushort *dst = (GLushort *) gl_pixel_addr_in_image(&ctx->Pack,
                               pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (alpha[i] * 1023.0F)) << 22)
                         | (((GLint) (blue[i]  * 1023.0F)) << 12)
                         | (((GLint) (green[i] * 1023.0F)) <<  2)
                         | (((GLint) (red[i]   *    3.0F))      );
               }
            }
            break;
         case GL_UNSIGNED_INT_2_10_10_10_REV:
            if (format == GL_RGBA) {
               GLuint *dst = (GLuint *) gl_pixel_addr_in_image(&ctx->Pack,
                             pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (red[i]   * 1023.0F))      )
                         | (((GLint) (green[i] * 1023.0F)) << 10)
                         | (((GLint) (blue[i]  * 1023.0F)) << 20)
                         | (((GLint) (alpha[i] *    3.0F)) << 30);
               }
            }
            else if (format == GL_BGRA) {
               GLushort *dst = (GLushort *) gl_pixel_addr_in_image(&ctx->Pack,
                               pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (blue[i]  * 1023.0F))      )
                         | (((GLint) (green[i] * 1023.0F)) << 10)
                         | (((GLint) (red[i]   * 1023.0F)) << 20)
                         | (((GLint) (alpha[i] *    3.0F)) << 30);
               }
            }
            else if (format == GL_ABGR_EXT) {
               GLushort *dst = (GLushort *) gl_pixel_addr_in_image(&ctx->Pack,
                               pixels, width, height, format, type, 0, j, 0);
               for (i=0;i<width;i++) {
                  dst[i] = (((GLint) (alpha[i] * 1023.0F))      )
                         | (((GLint) (blue[i]  * 1023.0F)) << 10)
                         | (((GLint) (green[i] * 1023.0F)) << 20)
                         | (((GLint) (red[i]   *    3.0F)) << 30);
	       }
	    }
	    break;
         default:
            gl_error( ctx, GL_INVALID_ENUM, "glReadPixels(type)" );
      }
   }
   UNDEFARRAY( red );
   UNDEFARRAY( green );
   UNDEFARRAY( blue );
   UNDEFARRAY( alpha );
   UNDEFARRAY( luminance );
}



void gl_ReadPixels( GLcontext *ctx,
                    GLint x, GLint y, GLsizei width, GLsizei height,
		    GLenum format, GLenum type, GLvoid *pixels )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glReadPixels" );
      return;
   }

   if (!pixels) {
      gl_error( ctx, GL_INVALID_VALUE, "glReadPixels(pixels)" );
      return;
   }

   (void) (*ctx->Driver.SetBuffer)( ctx, ctx->Pixel.ReadBuffer );

   switch (format) {
      case GL_COLOR_INDEX:
         read_index_pixels( ctx, x, y, width, height, type, pixels );
	 break;
      case GL_STENCIL_INDEX:
	 read_stencil_pixels( ctx, x, y, width, height, type, pixels );
         break;
      case GL_DEPTH_COMPONENT:
	 read_depth_pixels( ctx, x, y, width, height, type, pixels );
	 break;
      case GL_RED:
      case GL_GREEN:
      case GL_BLUE:
      case GL_ALPHA:
      case GL_RGB:
      case GL_LUMINANCE:
      case GL_LUMINANCE_ALPHA:
      case GL_RGBA:
      case GL_BGR:
      case GL_BGRA:
      case GL_ABGR_EXT:
         if (!read_fast_rgba_pixels( ctx, x, y, width, height,
                                     format, type, pixels))
            read_rgba_pixels( ctx, x, y, width, height, format, type, pixels );
	 break;
      default:
	 gl_error( ctx, GL_INVALID_ENUM, "glReadPixels(format)" );
   }

   (void) (*ctx->Driver.SetBuffer)( ctx, ctx->Color.DrawBuffer );
}
