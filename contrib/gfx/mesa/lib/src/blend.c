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
 * Revision 3.8  1998/08/16 14:44:56  brianp
 * fixed arithmetic problems in blend_transparency() (Karl Schultz)
 *
 * Revision 3.7  1998/08/05 01:34:40  brianp
 * fixed round-off problem exposed by conformance testing
 *
 * Revision 3.6  1998/07/09 03:15:22  brianp
 * include asm_mmx.h instead of asm-mmx.h
 *
 * Revision 3.5  1998/04/22 00:53:02  brianp
 * fixed a few more IRIX compiler warnings
 *
 * Revision 3.4  1998/03/28 03:57:13  brianp
 * added CONST macro to fix IRIX compilation problems
 *
 * Revision 3.3  1998/03/28 02:03:22  brianp
 * hooked in MMX code
 *
 * Revision 3.2  1998/02/13 04:38:05  brianp
 * optimized blending functions called via BlendFunc function pointer
 *
 * Revision 3.1  1998/02/08 20:16:50  brianp
 * removed unneeded headers
 *
 * Revision 3.0  1998/01/31 20:47:46  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <assert.h>
#include <stdlib.h>
#include "alphabuf.h"
#include "asm_mmx.h"
#include "blend.h"
#include "context.h"
#include "macros.h"
#include "pb.h"
#include "span.h"
#include "types.h"
#endif



void gl_BlendFunc( GLcontext *ctx, GLenum sfactor, GLenum dfactor )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glBlendFunc" );
      return;
   }

   switch (sfactor) {
      case GL_ZERO:
      case GL_ONE:
      case GL_DST_COLOR:
      case GL_ONE_MINUS_DST_COLOR:
      case GL_SRC_ALPHA:
      case GL_ONE_MINUS_SRC_ALPHA:
      case GL_DST_ALPHA:
      case GL_ONE_MINUS_DST_ALPHA:
      case GL_SRC_ALPHA_SATURATE:
      case GL_CONSTANT_COLOR:
      case GL_ONE_MINUS_CONSTANT_COLOR:
      case GL_CONSTANT_ALPHA:
      case GL_ONE_MINUS_CONSTANT_ALPHA:
         ctx->Color.BlendSrc = sfactor;
         break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glBlendFunc(sfactor)" );
         return;
   }

   switch (dfactor) {
      case GL_ZERO:
      case GL_ONE:
      case GL_SRC_COLOR:
      case GL_ONE_MINUS_SRC_COLOR:
      case GL_SRC_ALPHA:
      case GL_ONE_MINUS_SRC_ALPHA:
      case GL_DST_ALPHA:
      case GL_ONE_MINUS_DST_ALPHA:
      case GL_CONSTANT_COLOR:
      case GL_ONE_MINUS_CONSTANT_COLOR:
      case GL_CONSTANT_ALPHA:
      case GL_ONE_MINUS_CONSTANT_ALPHA:
         ctx->Color.BlendDst = dfactor;
         break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glBlendFunc(dfactor)" );
         return;
   }

   ctx->Color.BlendFunc = NULL;
   ctx->NewState |= NEW_RASTER_OPS;
}



/* This is really an extension function! */
void gl_BlendEquation( GLcontext *ctx, GLenum mode )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glBlendEquation" );
      return;
   }

   switch (mode) {
      case GL_MIN_EXT:
      case GL_MAX_EXT:
      case GL_LOGIC_OP:
      case GL_FUNC_ADD_EXT:
      case GL_FUNC_SUBTRACT_EXT:
      case GL_FUNC_REVERSE_SUBTRACT_EXT:
         ctx->Color.BlendEquation = mode;
         break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glBlendEquation" );
         return;
   }

   /* This is needed to support 1.1's RGB logic ops AND
    * 1.0's blending logicops.
    */
   if (mode==GL_LOGIC_OP && ctx->Color.BlendEnabled) {
      ctx->Color.ColorLogicOpEnabled = GL_TRUE;
   }
   else {
      ctx->Color.ColorLogicOpEnabled = GL_FALSE;
   }

   ctx->Color.BlendFunc = NULL;
   ctx->NewState |= NEW_RASTER_OPS;
}



void gl_BlendColor( GLcontext *ctx, GLclampf red, GLclampf green,
		    GLclampf blue, GLclampf alpha )
{
   ctx->Color.BlendColor[0] = CLAMP( red,   0.0, 1.0 );
   ctx->Color.BlendColor[1] = CLAMP( green, 0.0, 1.0 );
   ctx->Color.BlendColor[2] = CLAMP( blue,  0.0, 1.0 );
   ctx->Color.BlendColor[3] = CLAMP( alpha, 0.0, 1.0 );
}



/*
 * Common transparency blending mode.
 */
static void blend_transparency( GLcontext *ctx, GLuint n, const GLubyte mask[],
                                GLubyte rgba[][4], CONST GLubyte dest[][4] )
{
   GLuint i;
   ASSERT(ctx->Color.BlendEquation==GL_FUNC_ADD_EXT);
   ASSERT(ctx->Color.BlendSrc==GL_SRC_ALPHA);
   ASSERT(ctx->Color.BlendDst==GL_ONE_MINUS_SRC_ALPHA);

      for (i=0;i<n;i++) {
	 if (mask[i]) {
         GLint t = rgba[i][ACOMP];  /* t in [0,255] */
         if (t == 0) {
            rgba[i][RCOMP] = dest[i][RCOMP];
            rgba[i][GCOMP] = dest[i][GCOMP];
            rgba[i][BCOMP] = dest[i][BCOMP];
            rgba[i][ACOMP] = dest[i][ACOMP];
	 }
         else if (t == 255) {
            /* no-op */
      }
         else {
            GLint s = 255 - t;
            GLint r = (rgba[i][RCOMP] * t + dest[i][RCOMP] * s) >> 8;
            GLint g = (rgba[i][GCOMP] * t + dest[i][GCOMP] * s) >> 8;
            GLint b = (rgba[i][BCOMP] * t + dest[i][BCOMP] * s) >> 8;
            GLint a = (rgba[i][ACOMP] * t + dest[i][ACOMP] * s) >> 8;
            ASSERT(r <= 255);
            ASSERT(g <= 255);
            ASSERT(b <= 255);
            ASSERT(a <= 255);
            rgba[i][RCOMP] = r;
            rgba[i][GCOMP] = g;
            rgba[i][BCOMP] = b;
            rgba[i][ACOMP] = a;
	    }
	 }
      }
}



/*
 * Add src and dest.
 */
static void blend_add( GLcontext *ctx, GLuint n, const GLubyte mask[],
                       GLubyte rgba[][4], CONST GLubyte dest[][4] )
{
   GLuint i;
   ASSERT(ctx->Color.BlendEquation==GL_FUNC_ADD_EXT);
   ASSERT(ctx->Color.BlendSrc==GL_ONE);
   ASSERT(ctx->Color.BlendDst==GL_ONE);
   (void) ctx;

   for (i=0;i<n;i++) {
	       if (mask[i]) {
         GLint r = rgba[i][RCOMP] + dest[i][RCOMP];
         GLint g = rgba[i][GCOMP] + dest[i][GCOMP];
         GLint b = rgba[i][BCOMP] + dest[i][BCOMP];
         GLint a = rgba[i][ACOMP] + dest[i][ACOMP];
         rgba[i][RCOMP] = MIN2( r, 255 );
         rgba[i][GCOMP] = MIN2( g, 255 );
         rgba[i][BCOMP] = MIN2( b, 255 );
         rgba[i][ACOMP] = MIN2( a, 255 );
	       }
	    }
}



/*
 * Blend min function  (for GL_EXT_blend_minmax)
 */
static void blend_min( GLcontext *ctx, GLuint n, const GLubyte mask[],
                       GLubyte rgba[][4], CONST GLubyte dest[][4] )
{
   GLuint i;
   ASSERT(ctx->Color.BlendEquation==GL_MIN_EXT);
   (void) ctx;

   for (i=0;i<n;i++) {
	       if (mask[i]) {
         rgba[i][RCOMP] = MIN2( rgba[i][RCOMP], dest[i][RCOMP] );
         rgba[i][GCOMP] = MIN2( rgba[i][GCOMP], dest[i][GCOMP] );
         rgba[i][BCOMP] = MIN2( rgba[i][BCOMP], dest[i][BCOMP] );
         rgba[i][ACOMP] = MIN2( rgba[i][ACOMP], dest[i][ACOMP] );
	       }
	    }
}



/*
 * Blend max function  (for GL_EXT_blend_minmax)
 */
static void blend_max( GLcontext *ctx, GLuint n, const GLubyte mask[],
                       GLubyte rgba[][4], CONST GLubyte dest[][4] )
{
   GLuint i;
   ASSERT(ctx->Color.BlendEquation==GL_MAX_EXT);
   (void) ctx;

   for (i=0;i<n;i++) {
      if (mask[i]) {
         rgba[i][RCOMP] = MAX2( rgba[i][RCOMP], dest[i][RCOMP] );
         rgba[i][GCOMP] = MAX2( rgba[i][GCOMP], dest[i][GCOMP] );
         rgba[i][BCOMP] = MAX2( rgba[i][BCOMP], dest[i][BCOMP] );
         rgba[i][ACOMP] = MAX2( rgba[i][ACOMP], dest[i][ACOMP] );
	    }
	    }
}



/*
 * Modulate:  result = src * dest
 */
static void blend_modulate( GLcontext *ctx, GLuint n, const GLubyte mask[],
                            GLubyte rgba[][4], CONST GLubyte dest[][4] )
{
   GLuint i;
   (void) ctx;

   for (i=0;i<n;i++) {
      if (mask[i]) {
         GLint r = (rgba[i][RCOMP] * dest[i][RCOMP]) >> 8;
         GLint g = (rgba[i][GCOMP] * dest[i][GCOMP]) >> 8;
         GLint b = (rgba[i][BCOMP] * dest[i][BCOMP]) >> 8;
         GLint a = (rgba[i][ACOMP] * dest[i][ACOMP]) >> 8;
         rgba[i][RCOMP] = r;
         rgba[i][GCOMP] = g;
         rgba[i][BCOMP] = b;
         rgba[i][ACOMP] = a;
      }
		  }
}



/*
 * General case blend pixels.
 * Input:  n - number of pixels
 *         mask - the usual write mask
 * In/Out:  rgba - the incoming and modified pixels
 * Input:  dest - the pixels from the dest color buffer
 */
static void blend_general( GLcontext *ctx, GLuint n, const GLubyte mask[],
                           GLubyte rgba[][4], CONST GLubyte dest[][4] )
{
   GLfloat rscale = 1.0F / 255.0F;
   GLfloat gscale = 1.0F / 255.0F;
   GLfloat bscale = 1.0F / 255.0F;
   GLfloat ascale = 1.0F / 255.0F;
   GLuint i;

      for (i=0;i<n;i++) {
	 if (mask[i]) {
	    GLint Rs, Gs, Bs, As;  /* Source colors */
	    GLint Rd, Gd, Bd, Ad;  /* Dest colors */
	    GLfloat sR, sG, sB, sA;  /* Source scaling */
	    GLfloat dR, dG, dB, dA;  /* Dest scaling */
	    GLfloat r, g, b, a;

	    /* Source Color */
         Rs = rgba[i][RCOMP];
         Gs = rgba[i][GCOMP];
         Bs = rgba[i][BCOMP];
         As = rgba[i][ACOMP];

	    /* Frame buffer color */
         Rd = dest[i][RCOMP];
         Gd = dest[i][GCOMP];
         Bd = dest[i][BCOMP];
         Ad = dest[i][ACOMP];

	    /* Source scaling */
	    switch (ctx->Color.BlendSrc) {
	       case GL_ZERO:
		  sR = sG = sB = sA = 0.0F;
		  break;
	       case GL_ONE:
		  sR = sG = sB = sA = 1.0F;
		  break;
	       case GL_DST_COLOR:
		  sR = (GLfloat) Rd * rscale;
		  sG = (GLfloat) Gd * gscale;
		  sB = (GLfloat) Bd * bscale;
		  sA = (GLfloat) Ad * ascale;
		  break;
	       case GL_ONE_MINUS_DST_COLOR:
		  sR = 1.0F - (GLfloat) Rd * rscale;
		  sG = 1.0F - (GLfloat) Gd * gscale;
		  sB = 1.0F - (GLfloat) Bd * bscale;
		  sA = 1.0F - (GLfloat) Ad * ascale;
		  break;
	       case GL_SRC_ALPHA:
		  sR = sG = sB = sA = (GLfloat) As * ascale;
		  break;
	       case GL_ONE_MINUS_SRC_ALPHA:
		  sR = sG = sB = sA = (GLfloat) 1.0F - (GLfloat) As * ascale;
		  break;
	       case GL_DST_ALPHA:
		  sR = sG = sB = sA =(GLfloat) Ad * ascale;
		  break;
	       case GL_ONE_MINUS_DST_ALPHA:
		  sR = sG = sB = sA = 1.0F - (GLfloat) Ad * ascale;
		  break;
	       case GL_SRC_ALPHA_SATURATE:
		  if (As < 1.0F - (GLfloat) Ad * ascale) {
		     sR = sG = sB = (GLfloat) As * ascale;
		  }
		  else {
		     sR = sG = sB = 1.0F - (GLfloat) Ad * ascale;
		  }
		  sA = 1.0;
		  break;
	       case GL_CONSTANT_COLOR:
		  sR = ctx->Color.BlendColor[0];
		  sG = ctx->Color.BlendColor[1];
		  sB = ctx->Color.BlendColor[2];
		  sA = ctx->Color.BlendColor[3];
		  break;
	       case GL_ONE_MINUS_CONSTANT_COLOR:
		  sR = 1.0F - ctx->Color.BlendColor[0];
		  sG = 1.0F - ctx->Color.BlendColor[1];
		  sB = 1.0F - ctx->Color.BlendColor[2];
		  sA = 1.0F - ctx->Color.BlendColor[3];
		  break;
	       case GL_CONSTANT_ALPHA:
		  sR = sG = sB = sA = ctx->Color.BlendColor[3];
		  break;
	       case GL_ONE_MINUS_CONSTANT_ALPHA:
		  sR = sG = sB = sA = 1.0F - ctx->Color.BlendColor[3];
		  break;
	       default:
                  /* this should never happen */
			gl_problem(ctx, "Bad blend source factor in do_blend");
	    }

	    /* Dest scaling */
	    switch (ctx->Color.BlendDst) {
	       case GL_ZERO:
		  dR = dG = dB = dA = 0.0F;
		  break;
	       case GL_ONE:
		  dR = dG = dB = dA = 1.0F;
		  break;
	       case GL_SRC_COLOR:
		  dR = (GLfloat) Rs * rscale;
		  dG = (GLfloat) Gs * gscale;
		  dB = (GLfloat) Bs * bscale;
		  dA = (GLfloat) As * ascale;
		  break;
	       case GL_ONE_MINUS_SRC_COLOR:
		  dR = 1.0F - (GLfloat) Rs * rscale;
		  dG = 1.0F - (GLfloat) Gs * gscale;
		  dB = 1.0F - (GLfloat) Bs * bscale;
		  dA = 1.0F - (GLfloat) As * ascale;
		  break;
	       case GL_SRC_ALPHA:
		  dR = dG = dB = dA = (GLfloat) As * ascale;
		  break;
	       case GL_ONE_MINUS_SRC_ALPHA:
		  dR = dG = dB = dA = (GLfloat) 1.0F - (GLfloat) As * ascale;
		  break;
	       case GL_DST_ALPHA:
		  dR = dG = dB = dA = (GLfloat) Ad * ascale;
		  break;
	       case GL_ONE_MINUS_DST_ALPHA:
		  dR = dG = dB = dA = 1.0F - (GLfloat) Ad * ascale;
		  break;
	       case GL_CONSTANT_COLOR:
		  dR = ctx->Color.BlendColor[0];
		  dG = ctx->Color.BlendColor[1];
		  dB = ctx->Color.BlendColor[2];
		  dA = ctx->Color.BlendColor[3];
		  break;
	       case GL_ONE_MINUS_CONSTANT_COLOR:
		  dR = 1.0F - ctx->Color.BlendColor[0];
		  dG = 1.0F - ctx->Color.BlendColor[1];
		  dB = 1.0F - ctx->Color.BlendColor[2];
		  dA = 1.0F - ctx->Color.BlendColor[3];
		  break;
	       case GL_CONSTANT_ALPHA:
		  dR = dG = dB = dA = ctx->Color.BlendColor[3];
		  break;
	       case GL_ONE_MINUS_CONSTANT_ALPHA:
		  dR = dG = dB = dA = 1.0F - ctx->Color.BlendColor[3] * ascale;
		  break;
	       default:
                  /* this should never happen */
			gl_problem(ctx, "Bad blend dest factor in do_blend");
	    }

         /* Due to round-off problems we have to clamp against zero. */
         /* Optimization: we don't have to do this for all src & dst factors */
         if (dA < 0.0F)  dA = 0.0F;
         if (dR < 0.0F)  dR = 0.0F;
         if (dG < 0.0F)  dG = 0.0F;
         if (dB < 0.0F)  dB = 0.0F;
         if (sA < 0.0F)  sA = 0.0F;
         if (sR < 0.0F)  sR = 0.0F;
         if (sG < 0.0F)  sG = 0.0F;
         if (sB < 0.0F)  sB = 0.0F;

         ASSERT( sR <= 1.0 );
         ASSERT( sG <= 1.0 );
         ASSERT( sB <= 1.0 );
         ASSERT( sA <= 1.0 );
         ASSERT( dR <= 1.0 );
         ASSERT( dG <= 1.0 );
         ASSERT( dB <= 1.0 );
         ASSERT( dA <= 1.0 );

	    /* compute blended color */
	    if (ctx->Color.BlendEquation==GL_FUNC_ADD_EXT) {
	       r = Rs * sR + Rd * dR;
	       g = Gs * sG + Gd * dG;
	       b = Bs * sB + Bd * dB;
	       a = As * sA + Ad * dA;
	    }
	    else if (ctx->Color.BlendEquation==GL_FUNC_SUBTRACT_EXT) {
	       r = Rs * sR - Rd * dR;
	       g = Gs * sG - Gd * dG;
	       b = Bs * sB - Bd * dB;
	       a = As * sA - Ad * dA;
	    }
	    else if (ctx->Color.BlendEquation==GL_FUNC_REVERSE_SUBTRACT_EXT) {
	       r = Rd * dR - Rs * sR;
	       g = Gd * dG - Gs * sG;
	       b = Bd * dB - Bs * sB;
	       a = Ad * dA - As * sA;
	    }

         /* final clamping */
         rgba[i][RCOMP] = (GLint) CLAMP( r, 0.0F, 255.0F );
         rgba[i][GCOMP] = (GLint) CLAMP( g, 0.0F, 255.0F );
         rgba[i][BCOMP] = (GLint) CLAMP( b, 0.0F, 255.0F );
         rgba[i][ACOMP] = (GLint) CLAMP( a, 0.0F, 255.0F );
	       }
	    }
}



/*
 * Analyze current blending parameters to pick fastest blending function.
 * Result: the ctx->Color.BlendFunc pointer is updated.
 */
static void set_blend_function(GLcontext *ctx)
{
   GLenum eq = ctx->Color.BlendEquation;
   GLenum src = ctx->Color.BlendSrc;
   GLenum dst = ctx->Color.BlendDst;

   if (eq==GL_FUNC_ADD_EXT && src==GL_SRC_ALPHA && dst==GL_ONE_MINUS_SRC_ALPHA) {
#ifdef MMX
      ctx->Color.BlendFunc = gl_mmx_blend_transparency;
#else
      ctx->Color.BlendFunc = blend_transparency;
#endif
	 }
   else if (eq==GL_FUNC_ADD_EXT && src==GL_ONE && dst==GL_ONE) {
      ctx->Color.BlendFunc = blend_add;
      }
   else if (((eq==GL_FUNC_ADD_EXT || eq==GL_FUNC_REVERSE_SUBTRACT_EXT)
             && (src==GL_ZERO && dst==GL_SRC_COLOR))
            ||
            ((eq==GL_FUNC_ADD_EXT || eq==GL_FUNC_SUBTRACT_EXT)
             && (src==GL_DST_COLOR && dst==GL_ZERO))) {
      ctx->Color.BlendFunc = blend_modulate;
   }
   else if (eq==GL_MIN_EXT) {
      ctx->Color.BlendFunc = blend_min;
	 }
   else if (eq==GL_MAX_EXT) {
      ctx->Color.BlendFunc = blend_max;
      }
   else {
      ctx->Color.BlendFunc = blend_general;
   }
}



/*
 * Apply the blending operator to a span of pixels.
 * Input:  n - number of pixels in span
 *         x, y - location of leftmost pixel in span in window coords.
 *         mask - boolean mask indicating which pixels to blend.
 * In/Out:  rgba - pixel values
 */
void gl_blend_span( GLcontext *ctx, GLuint n, GLint x, GLint y,
		    GLubyte rgba[][4], const GLubyte mask[] )
{
   GLubyte dest[MAX_WIDTH][4];

   /* Check if device driver can do the work */
   if (ctx->Color.BlendEquation==GL_LOGIC_OP && !ctx->Color.SWLogicOpEnabled) {
      return;
   }

   /* Read span of current frame buffer pixels */
   gl_read_rgba_span( ctx, n, x, y, dest );

   if (!ctx->Color.BlendFunc)
      set_blend_function(ctx);

   (*ctx->Color.BlendFunc)( ctx, n, mask, rgba, dest );
}





/*
 * Apply the blending operator to an array of pixels.
 * Input:  n - number of pixels in span
 *         x, y - array of pixel locations
 *         mask - boolean mask indicating which pixels to blend.
 * In/Out:  rgba - pixel values
 */
void gl_blend_pixels( GLcontext *ctx,
                      GLuint n, const GLint x[], const GLint y[],
		      GLubyte rgba[][4], const GLubyte mask[] )
{
   GLubyte dest[PB_SIZE][4];

   /* Check if device driver can do the work */
   if (ctx->Color.BlendEquation==GL_LOGIC_OP && !ctx->Color.SWLogicOpEnabled) {
      return;
   }

   /* Read pixels from current color buffer */
   (*ctx->Driver.ReadRGBAPixels)( ctx, n, x, y, dest, mask );

   if (ctx->RasterMask & ALPHABUF_BIT) {
      gl_read_alpha_pixels( ctx, n, x, y, dest, mask );
   }
   else {
      GLuint i;
      for (i=0; i<n; i++) {
         dest[i][ACOMP] = 255;
      }
   }

   if (!ctx->Color.BlendFunc)
      set_blend_function(ctx);

   (*ctx->Color.BlendFunc)( ctx, n, mask, rgba, dest );
}
