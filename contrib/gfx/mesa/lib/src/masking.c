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
 * Revision 3.1  1998/02/08 20:19:41  brianp
 * ColorMask is now GLubyte[4] instead of GLuint
 *
 * Revision 3.0  1998/01/31 20:58:46  brianp
 * initial rev
 *
 */


/*
 * Implement the effect of glColorMask and glIndexMask in software.
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <string.h>
#include "alphabuf.h"
#include "context.h"
#include "macros.h"
#include "masking.h"
#include "pb.h"
#include "span.h"
#include "types.h"
#endif



void gl_IndexMask( GLcontext *ctx, GLuint mask )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glIndexMask" );
      return;
   }
   ctx->Color.IndexMask = mask;
   ctx->NewState |= NEW_RASTER_OPS;
}



void gl_ColorMask( GLcontext *ctx, GLboolean red, GLboolean green,
                   GLboolean blue, GLboolean alpha )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glColorMask" );
      return;
   }
   ctx->Color.ColorMask[RCOMP] = red    ? 0xff : 0x0;
   ctx->Color.ColorMask[GCOMP] = green  ? 0xff : 0x0;
   ctx->Color.ColorMask[BCOMP] = blue   ? 0xff : 0x0;
   ctx->Color.ColorMask[ACOMP] = alpha  ? 0xff : 0x0;

   ctx->NewState |= NEW_RASTER_OPS;
}




/*
 * Apply glColorMask to a span of RGBA pixels.
 */
void gl_mask_rgba_span( GLcontext *ctx,
                        GLuint n, GLint x, GLint y, GLubyte rgba[][4] )
{
   GLubyte dest[MAX_WIDTH][4];
   GLuint srcMask = *((GLuint*)ctx->Color.ColorMask);
   GLuint dstMask = ~srcMask;
   GLuint *rgba32 = (GLuint *) rgba;
   GLuint *dest32 = (GLuint *) dest;
   GLuint i;

   gl_read_rgba_span( ctx, n, x, y, dest );

   for (i=0; i<n; i++) {
      rgba32[i] = (rgba32[i] & srcMask) | (dest32[i] & dstMask);
   }
}



/*
 * Apply glColorMask to an array of RGBA pixels.
 */
void gl_mask_rgba_pixels( GLcontext *ctx,
                           GLuint n, const GLint x[], const GLint y[],
                          GLubyte rgba[][4], const GLubyte mask[] )
{
   GLubyte dest[PB_SIZE][4];
   GLuint srcMask = *((GLuint*)ctx->Color.ColorMask);
   GLuint dstMask = ~srcMask;
   GLuint *rgba32 = (GLuint *) rgba;
   GLuint *dest32 = (GLuint *) dest;
   GLuint i;

   (*ctx->Driver.ReadRGBAPixels)( ctx, n, x, y, dest, mask );

   for (i=0; i<n; i++) {
      rgba32[i] = (rgba32[i] & srcMask) | (dest32[i] & dstMask);
   }
}



/*
 * Apply glIndexMask to a span of CI pixels.
 */
void gl_mask_index_span( GLcontext *ctx,
                         GLuint n, GLint x, GLint y, GLuint index[] )
{
   GLuint i;
   GLuint fbindexes[MAX_WIDTH];
   GLuint msrc, mdest;

   gl_read_index_span( ctx, n, x, y, fbindexes );

   msrc = ctx->Color.IndexMask;
   mdest = ~msrc;

   for (i=0;i<n;i++) {
      index[i] = (index[i] & msrc) | (fbindexes[i] & mdest);
   }
}



/*
 * Apply glIndexMask to an array of CI pixels.
 */
void gl_mask_index_pixels( GLcontext *ctx,
                           GLuint n, const GLint x[], const GLint y[],
                           GLuint index[], const GLubyte mask[] )
{
   GLuint i;
   GLuint fbindexes[PB_SIZE];
   GLuint msrc, mdest;

   (*ctx->Driver.ReadCI32Pixels)( ctx, n, x, y, fbindexes, mask );

   msrc = ctx->Color.IndexMask;
   mdest = ~msrc;

   for (i=0;i<n;i++) {
      index[i] = (index[i] & msrc) | (fbindexes[i] & mdest);
   }
}

