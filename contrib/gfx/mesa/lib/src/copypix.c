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
 * Revision 1.1  2005/01/11 14:58:30  NicJA
 * AROSMesa 3.0
 *
 * - Based on the official mesa 3 code with major patches to the amigamesa driver code to get it working.
 * - GLUT not yet started (ive left the _old_ mesaaux, mesatk and demos in for this reason)
 * - Doesnt yet work - the _db functions seem to be writing the data incorrectly, and color picking also seems broken somewhat - giving most things a blue tinge (those that are currently working)
 *
 * Revision 3.8  1998/07/17 03:24:16  brianp
 * added Pixel.ScaleOrBiasRGBA field
 *
 * Revision 3.7  1998/05/31 23:50:36  brianp
 * cleaned up a few Solaris compiler warnings
 *
 * Revision 3.6  1998/04/01 02:58:52  brianp
 * applied Miklos Fazekas's 3-31-98 Macintosh changes
 *
 * Revision 3.5  1998/03/27 03:30:36  brianp
 * fixed G++ warnings
 *
 * Revision 3.4  1998/02/15 01:31:41  brianp
 * removed unused variables
 *
 * Revision 3.3  1998/02/08 20:22:14  brianp
 * LOTS of clean-up and rewriting
 *
 * Revision 3.2  1998/02/04 00:33:45  brianp
 * fixed a few cast problems for Amiga StormC compiler
 *
 * Revision 3.1  1998/02/01 22:16:34  brianp
 * include different headers (zooming)
 *
 * Revision 3.0  1998/01/31 20:49:24  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <string.h>
#include "context.h"
#include "copypix.h"
#include "depth.h"
#include "feedback.h"
#include "macros.h"
#include "pixel.h"
#include "span.h"
#include "stencil.h"
#include "types.h"
#include "zoom.h"
#endif



static void copy_rgb_pixels( GLcontext* ctx,
                             GLint srcx, GLint srcy, GLint width, GLint height,
                             GLint destx, GLint desty )
{
   GLdepth zspan[MAX_WIDTH];
   GLubyte rgba[MAX_WIDTH][4];
   GLboolean quick_draw;
   GLint sy, dy, stepy;
   GLint i, j;
   GLboolean setbuffer;
   const GLboolean zoom = ctx->Pixel.ZoomX!=1.0F || ctx->Pixel.ZoomY!=1.0F;

   /* Determine if copy should be done bottom-to-top or top-to-bottom */
   if (srcy<desty) {
      /* top-down  max-to-min */
      sy = srcy + height - 1;
      dy = desty + height - 1;
      stepy = -1;
   }
   else {
      /* bottom-up  min-to-max */
      sy = srcy;
      dy = desty;
      stepy = 1;
   }

   if (ctx->Depth.Test) {
      /* fill in array of z values */
      GLint z = (GLint) (ctx->Current.RasterPos[2] * DEPTH_SCALE);
      for (i=0;i<width;i++) {
         zspan[i] = z;
      }
   }

   if (ctx->RasterMask==0 && !zoom
       && destx>=0 && destx+width<=ctx->Buffer->Width) {
      quick_draw = GL_TRUE;
   }
   else {
      quick_draw = GL_FALSE;
   }

   /* If read and draw buffer are different we must do buffer switching */
   setbuffer = ctx->Pixel.ReadBuffer!=ctx->Color.DrawBuffer;

   for (j=0; j<height; j++, sy+=stepy, dy+=stepy) {
      if (setbuffer) {
         (*ctx->Driver.SetBuffer)( ctx, ctx->Pixel.ReadBuffer );
      }
      gl_read_rgba_span( ctx, width, srcx, sy, rgba );

      if (ctx->Pixel.ScaleOrBiasRGBA) {
         gl_scale_and_bias_rgba( ctx, width, rgba );
      }
      if (ctx->Pixel.MapColorFlag) {
         gl_map_rgba( ctx, width, rgba );
      }

      if (setbuffer) {
         (*ctx->Driver.SetBuffer)( ctx, ctx->Color.DrawBuffer );
      }
      if (quick_draw && dy>=0 && dy<ctx->Buffer->Height) {
         (*ctx->Driver.WriteRGBASpan)( ctx, width, destx, dy, rgba, NULL );
      }
      else if (zoom) {
         gl_write_zoomed_rgba_span( ctx, width, destx, dy, zspan, rgba, desty);
      }
      else {
         gl_write_rgba_span( ctx, width, destx, dy, zspan, rgba, GL_BITMAP );
      }
   }
}



static void copy_ci_pixels( GLcontext* ctx,
                            GLint srcx, GLint srcy, GLint width, GLint height,
                            GLint destx, GLint desty )
{
   GLdepth zspan[MAX_WIDTH];
   GLint sy, dy, stepy;
   GLint i, j;
   GLboolean setbuffer;
   const GLboolean zoom = ctx->Pixel.ZoomX!=1.0F || ctx->Pixel.ZoomY!=1.0F;
   const GLboolean shift_or_offset = ctx->Pixel.IndexShift || ctx->Pixel.IndexOffset;

   /* Determine if copy should be bottom-to-top or top-to-bottom */
   if (srcy<desty) {
      /* top-down  max-to-min */
      sy = srcy + height - 1;
      dy = desty + height - 1;
      stepy = -1;
   }
   else {
      /* bottom-up  min-to-max */
      sy = srcy;
      dy = desty;
      stepy = 1;
   }

   if (ctx->Depth.Test) {
      /* fill in array of z values */
      GLint z = (GLint) (ctx->Current.RasterPos[2] * DEPTH_SCALE);
      for (i=0;i<width;i++) {
         zspan[i] = z;
      }
   }

   /* If read and draw buffer are different we must do buffer switching */
   setbuffer = ctx->Pixel.ReadBuffer!=ctx->Color.DrawBuffer;

   for (j=0; j<height; j++, sy+=stepy, dy+=stepy) {
      GLuint indexes[MAX_WIDTH];
      if (setbuffer) {
         (*ctx->Driver.SetBuffer)( ctx, ctx->Pixel.ReadBuffer );
      }
      gl_read_index_span( ctx, width, srcx, sy, indexes );

      if (shift_or_offset) {
         gl_shift_and_offset_ci( ctx, width, indexes );
         }
      if (ctx->Pixel.MapColorFlag) {
         gl_map_ci( ctx, width, indexes );
      }

      if (setbuffer) {
         (*ctx->Driver.SetBuffer)( ctx, ctx->Color.DrawBuffer );
      }
      if (zoom) {
         gl_write_zoomed_index_span( ctx, width, destx, dy, zspan, indexes, desty );
      }
      else {
         gl_write_index_span( ctx, width, destx, dy, zspan, indexes, GL_BITMAP );
      }
   }
}



/*
 * TODO: Optimize!!!!
 */
static void copy_depth_pixels( GLcontext* ctx, GLint srcx, GLint srcy,
                               GLint width, GLint height,
                               GLint destx, GLint desty )
{
   GLfloat depth[MAX_WIDTH];
   GLdepth zspan[MAX_WIDTH];
   GLuint indexes[MAX_WIDTH];
   GLubyte rgba[MAX_WIDTH][4];
   GLint sy, dy, stepy;
   GLint i, j;
   const GLboolean zoom = ctx->Pixel.ZoomX!=1.0F || ctx->Pixel.ZoomY!=1.0F;

   if (!ctx->Buffer->Depth) {
      gl_error( ctx, GL_INVALID_OPERATION, "glCopyPixels" );
      return;
   }

   /* Determine if copy should be bottom-to-top or top-to-bottom */
   if (srcy<desty) {
      /* top-down  max-to-min */
      sy = srcy + height - 1;
      dy = desty + height - 1;
      stepy = -1;
   }
   else {
      /* bottom-up  min-to-max */
      sy = srcy;
      dy = desty;
      stepy = 1;
   }

   /* setup colors or indexes */
   if (ctx->Visual->RGBAflag) {
      GLuint *rgba32 = (GLuint *) rgba;
      GLuint color = *((GLuint *) ctx->Current.ByteColor);
      for (i=0; i<width; i++) {
         rgba32[i] = color;
      }
   }
   else {
      for (i=0;i<width;i++) {
         indexes[i] = ctx->Current.Index;
      }
   }

   for (j=0; j<height; j++, sy+=stepy, dy+=stepy) {
      (*ctx->Driver.ReadDepthSpanFloat)( ctx, width, srcx, sy, depth );

      for (i=0;i<width;i++) {
         GLfloat d = depth[i] * ctx->Pixel.DepthScale + ctx->Pixel.DepthBias;
         zspan[i] = (GLint) (CLAMP( d, 0.0F, 1.0F ) * DEPTH_SCALE);
      }

      if (ctx->Visual->RGBAflag) {
         if (zoom) {
            gl_write_zoomed_rgba_span( ctx, width, destx, dy, zspan, rgba, desty );
         }
         else {
            gl_write_rgba_span( ctx, width, destx, dy, zspan, rgba, GL_BITMAP);
         }
      }
      else {
         if (zoom) {
            gl_write_zoomed_index_span( ctx, width, destx, dy,
                                        zspan, indexes, desty );
         }
         else {
            gl_write_index_span( ctx, width, destx, dy,
                                 zspan, indexes, GL_BITMAP );
         }
      }
   }
}



static void copy_stencil_pixels( GLcontext* ctx, GLint srcx, GLint srcy,
                                 GLint width, GLint height,
                                 GLint destx, GLint desty )
{
   GLubyte stencil[MAX_WIDTH];
   GLint sy, dy, stepy;
   GLint j;
   const GLboolean zoom = (ctx->Pixel.ZoomX!=1.0F || ctx->Pixel.ZoomY!=1.0F);
   const GLboolean shift_or_offset = ctx->Pixel.IndexShift!=0 || ctx->Pixel.IndexOffset!=0;

   if (!ctx->Buffer->Stencil) {
      gl_error( ctx, GL_INVALID_OPERATION, "glCopyPixels" );
      return;
   }

   /* Determine if copy should be bottom-to-top or top-to-bottom */
   if (srcy<desty) {
      /* top-down  max-to-min */
      sy = srcy + height - 1;
      dy = desty + height - 1;
      stepy = -1;
   }
   else {
      /* bottom-up  min-to-max */
      sy = srcy;
      dy = desty;
      stepy = 1;
   }

   for (j=0; j<height; j++, sy+=stepy, dy+=stepy) {
      gl_read_stencil_span( ctx, width, srcx, sy, stencil );

      if (shift_or_offset) {
         gl_shift_and_offset_stencil( ctx, width, stencil );
         }
      if (ctx->Pixel.MapStencilFlag) {
         gl_map_stencil( ctx, width, stencil );
            }

      if (zoom) {
         gl_write_zoomed_stencil_span( ctx, width, destx, dy, stencil, desty );
      }
      else {
         gl_write_stencil_span( ctx, width, destx, dy, stencil );
      }
   }
}




void gl_CopyPixels( GLcontext* ctx, GLint srcx, GLint srcy, GLsizei width, GLsizei height,
		    GLenum type )
{
   GLint destx, desty;

   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glCopyPixels" );
      return;
   }

   if (width<0 || height<0) {
      gl_error( ctx, GL_INVALID_VALUE, "glCopyPixels" );
      return;
   }

   if (ctx->NewState) {
      gl_update_state(ctx);
   }

   if (ctx->RenderMode==GL_RENDER) {
      /* Destination of copy: */
      if (!ctx->Current.RasterPosValid) {
	 return;
      }
      destx = (GLint) (ctx->Current.RasterPos[0] + 0.5F);
      desty = (GLint) (ctx->Current.RasterPos[1] + 0.5F);

      if (type==GL_COLOR && ctx->Visual->RGBAflag) {
         copy_rgb_pixels( ctx, srcx, srcy, width, height, destx, desty );
      }
      else if (type==GL_COLOR && !ctx->Visual->RGBAflag) {
         copy_ci_pixels( ctx, srcx, srcy, width, height, destx, desty );
      }
      else if (type==GL_DEPTH) {
         copy_depth_pixels( ctx, srcx, srcy, width, height, destx, desty );
      }
      else if (type==GL_STENCIL) {
         copy_stencil_pixels( ctx, srcx, srcy, width, height, destx, desty );
      }
      else {
	 gl_error( ctx, GL_INVALID_ENUM, "glCopyPixels" );
      }
   }
   else if (ctx->RenderMode==GL_FEEDBACK) {
      GLfloat color[4];
      color[0] = (GLfloat) ctx->Current.ByteColor[0] / 255.0;
      color[1] = (GLfloat) ctx->Current.ByteColor[1] / 255.0;
      color[2] = (GLfloat) ctx->Current.ByteColor[2] / 255.0;
      color[3] = (GLfloat) ctx->Current.ByteColor[3] / 255.0;
      FEEDBACK_TOKEN( ctx, (GLfloat) (GLint) GL_COPY_PIXEL_TOKEN );
      gl_feedback_vertex( ctx, ctx->Current.RasterPos[0],
			  ctx->Current.RasterPos[1],
			  ctx->Current.RasterPos[2],
			  ctx->Current.RasterPos[3],
			  color, ctx->Current.Index,
			  ctx->Current.TexCoord );
   }
   else if (ctx->RenderMode==GL_SELECT) {
      gl_update_hitflag( ctx, ctx->Current.RasterPos[2] );
   }

}


