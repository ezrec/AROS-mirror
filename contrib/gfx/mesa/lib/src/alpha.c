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
 * Revision 3.3  1998/07/29 04:02:30  brianp
 * fixed rounding error in computing integer alpha reference value
 *
 * Revision 3.2  1998/03/28 03:57:13  brianp
 * added CONST macro to fix IRIX compilation problems
 *
 * Revision 3.1  1998/02/13 03:23:04  brianp
 * AlphaRef is now a GLubyte
 *
 * Revision 3.0  1998/01/31 20:44:19  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include "alpha.h"
#include "context.h"
#include "types.h"
#include "macros.h"
#endif



void gl_AlphaFunc( GLcontext* ctx, GLenum func, GLclampf ref )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glAlphaFunc" );
      return;
   }
   switch (func) {
      case GL_NEVER:
      case GL_LESS:
      case GL_EQUAL:
      case GL_LEQUAL:
      case GL_GREATER:
      case GL_NOTEQUAL:
      case GL_GEQUAL:
      case GL_ALWAYS:
         ctx->Color.AlphaFunc = func;
         ctx->Color.AlphaRef = (GLubyte) (CLAMP(ref, 0.0F, 1.0F) * 255.0F + 0.5F);
         break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glAlphaFunc(func)" );
         break;
   }
}




/*
 * Apply the alpha test to a span of pixels.
 * In:  rgba - array of pixels
 * In/Out:  mask - current pixel mask.  Pixels which fail the alpha test
 *                 will set the corresponding mask flag to 0.
 * Return:  0 = all pixels in the span failed the alpha test.
 *          1 = one or more pixels passed the alpha test.
 */
GLint gl_alpha_test( const GLcontext* ctx,
                     GLuint n, CONST GLubyte rgba[][4], GLubyte mask[] )
{
   GLuint i;
   GLubyte ref = ctx->Color.AlphaRef;

   /* switch cases ordered from most frequent to less frequent */
   switch (ctx->Color.AlphaFunc) {
      case GL_LESS:
         for (i=0;i<n;i++) {
	    mask[i] &= (rgba[i][ACOMP] < ref);
	 }
	 return 1;
      case GL_LEQUAL:
         for (i=0;i<n;i++) {
	    mask[i] &= (rgba[i][ACOMP] <= ref);
	 }
	 return 1;
      case GL_GEQUAL:
         for (i=0;i<n;i++) {
	    mask[i] &= (rgba[i][ACOMP] >= ref);
	 }
	 return 1;
      case GL_GREATER:
         for (i=0;i<n;i++) {
	    mask[i] &= (rgba[i][ACOMP] > ref);
	 }
	 return 1;
      case GL_NOTEQUAL:
         for (i=0;i<n;i++) {
	    mask[i] &= (rgba[i][ACOMP] != ref);
	 }
	 return 1;
      case GL_EQUAL:
         for (i=0;i<n;i++) {
	    mask[i] &= (rgba[i][ACOMP] == ref);
	 }
	 return 1;
      case GL_ALWAYS:
	 /* do nothing */
	 return 1;
      case GL_NEVER:
         /* caller should check for zero! */
	 return 0;
      default:
	 gl_problem( ctx, "Invalid alpha test in gl_alpha_test" );
         return 0;
   }
   /* Never get here */
   /*return 1;*/
}
