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
 * Revision 3.0  1998/01/31 20:58:06  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <stdlib.h>
#include "alphabuf.h"
#include "context.h"
#include "logic.h"
#include "macros.h"
#include "pb.h"
#include "span.h"
#include "types.h"
#endif



void gl_LogicOp( GLcontext *ctx, GLenum opcode )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glLogicOp" );
      return;
   }
   switch (opcode) {
      case GL_CLEAR:
      case GL_SET:
      case GL_COPY:
      case GL_COPY_INVERTED:
      case GL_NOOP:
      case GL_INVERT:
      case GL_AND:
      case GL_NAND:
      case GL_OR:
      case GL_NOR:
      case GL_XOR:
      case GL_EQUIV:
      case GL_AND_REVERSE:
      case GL_AND_INVERTED:
      case GL_OR_REVERSE:
      case GL_OR_INVERTED:
         ctx->Color.LogicOp = opcode;
         ctx->NewState |= NEW_RASTER_OPS;
	 return;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glLogicOp" );
	 return;
   }
}



/*
 * Apply logic op to array of CI pixels.
 */
static void index_logicop( GLcontext *ctx, GLuint n,
                           GLuint index[], const GLuint dest[],
                           const GLubyte mask[] )
{
   GLuint i;
   switch (ctx->Color.LogicOp) {
      case GL_CLEAR:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = 0;
	    }
	 }
	 break;
      case GL_SET:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = 1;
	    }
	 }
	 break;
      case GL_COPY:
	 /* do nothing */
	 break;
      case GL_COPY_INVERTED:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = ~index[i];
	    }
	 }
	 break;
      case GL_NOOP:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = dest[i];
	    }
	 }
	 break;
      case GL_INVERT:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = ~dest[i];
	    }
	 }
	 break;
      case GL_AND:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] &= dest[i];
	    }
	 }
	 break;
      case GL_NAND:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = ~(index[i] & dest[i]);
	    }
	 }
	 break;
      case GL_OR:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] |= dest[i];
	    }
	 }
	 break;
      case GL_NOR:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = ~(index[i] | dest[i]);
	    }
	 }
	 break;
      case GL_XOR:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] ^= dest[i];
	    }
	 }
	 break;
      case GL_EQUIV:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = ~(index[i] ^ dest[i]);
	    }
	 }
	 break;
      case GL_AND_REVERSE:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = index[i] & ~dest[i];
	    }
	 }
	 break;
      case GL_AND_INVERTED:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = ~index[i] & dest[i];
	    }
	 }
	 break;
      case GL_OR_REVERSE:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = index[i] | ~dest[i];
	    }
	 }
	 break;
      case GL_OR_INVERTED:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
	       index[i] = ~index[i] | dest[i];
	    }
	 }
	 break;
      default:
	 gl_error( ctx, GL_INVALID_ENUM, "gl_logic error" );
   }
}



/*
 * Apply the current logic operator to a span of CI pixels.  This is only
 * used if the device driver can't do logic ops.
 */
void gl_logicop_ci_span( GLcontext *ctx, GLuint n, GLint x, GLint y,
                         GLuint index[], GLubyte mask[] )
{
   GLuint dest[MAX_WIDTH];
   /* Read dest values from frame buffer */
   (*ctx->Driver.ReadCI32Span)( ctx, n, x, y, dest );
   index_logicop( ctx, n, index, dest, mask );
}



/*
 * Apply the current logic operator to an array of CI pixels.  This is only
 * used if the device driver can't do logic ops.
 */
void gl_logicop_ci_pixels( GLcontext *ctx,
                           GLuint n, const GLint x[], const GLint y[],
                           GLuint index[], GLubyte mask[] )
{
   GLuint dest[PB_SIZE];
   /* Read dest values from frame buffer */
   (*ctx->Driver.ReadCI32Pixels)( ctx, n, x, y, dest, mask );
   index_logicop( ctx, n, index, dest, mask );
}



/*
 * Apply logic operator to rgba pixels.
 * Input:  ctx - the context
 *         n - number of pixels
 *         mask - pixel mask array
 * In/Out:  src - incoming pixels which will be modified
 * Input:  dest - frame buffer values
 *
 * Note:  Since the R, G, B, and A channels are all treated the same we
 * process them as 4-byte GLuints instead of four GLubytes.
 */
static void rgba_logicop( const GLcontext *ctx, GLuint n,
                          const GLubyte mask[],
                          GLuint src[], const GLuint dest[] )
{
   GLuint i;
   switch (ctx->Color.LogicOp) {
      case GL_CLEAR:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
               src[i] = 0;
	    }
	 }
	 break;
      case GL_SET:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
               src[i] = 0xffffffff;
	    }
	 }
	 break;
      case GL_COPY:
	 /* do nothing */
	 break;
      case GL_COPY_INVERTED:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
               src[i] = ~src[i];
	    }
	 }
	 break;
      case GL_NOOP:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
               src[i] = dest[i];
	    }
	 }
	 break;
      case GL_INVERT:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
               src[i] = ~dest[i];
	    }
	 }
	 break;
      case GL_AND:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
               src[i] &= dest[i];
	    }
	 }
	 break;
      case GL_NAND:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
               src[i] = ~(src[i] & src[i]);
	    }
	 }
	 break;
      case GL_OR:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
               src[i]|= dest[i];
	    }
	 }
	 break;
      case GL_NOR:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
               src[i] = ~(src[i] | dest[i]);
	    }
	 }
	 break;
      case GL_XOR:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
               src[i] ^= dest[i];
	    }
	 }
	 break;
      case GL_EQUIV:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
               src[i] = ~(src[i] ^ dest[i]);
	    }
	 }
	 break;
      case GL_AND_REVERSE:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
               src[i] = src[i] & ~dest[i];
	    }
	 }
	 break;
      case GL_AND_INVERTED:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
               src[i] = ~src[i] & dest[i];
	    }
	 }
	 break;
      case GL_OR_REVERSE:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
               src[i] = src[i] | ~dest[i];
	    }
	 }
	 break;
      case GL_OR_INVERTED:
         for (i=0;i<n;i++) {
	    if (mask[i]) {
               src[i] = ~src[i] | dest[i];
	    }
	 }
	 break;
      default:
         /* should never happen */
         gl_problem(ctx, "Bad function in rgba_logicop");
   }
}



/*
 * Apply the current logic operator to a span of RGBA pixels.
 * This is only used if the device driver can't do logic ops.
 */
void gl_logicop_rgba_span( GLcontext *ctx,
                           GLuint n, GLint x, GLint y,
                           GLubyte rgba[][4], const GLubyte mask[] )
{
   GLubyte dest[MAX_WIDTH][4];
   gl_read_rgba_span( ctx, n, x, y, dest );
   rgba_logicop( ctx, n, mask, (GLuint *) rgba, (const GLuint *) dest );
}



/*
 * Apply the current logic operator to an array of RGBA pixels.
 * This is only used if the device driver can't do logic ops.
 */
void gl_logicop_rgba_pixels( GLcontext *ctx,
                             GLuint n, const GLint x[], const GLint y[],
                             GLubyte rgba[][4], const GLubyte mask[] )
{
   GLubyte dest[PB_SIZE][4];
   (*ctx->Driver.ReadRGBAPixels)( ctx, n, x, y, dest, mask );
   if (ctx->RasterMask & ALPHABUF_BIT) {
      gl_read_alpha_pixels( ctx, n, x, y, dest, mask );
   }
   rgba_logicop( ctx, n, mask, (GLuint *) rgba, (const GLuint *) dest );
}
