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
 * Revision 3.2  1998/05/31 23:50:36  brianp
 * cleaned up a few Solaris compiler warnings
 *
 * Revision 3.1  1998/03/27 04:17:31  brianp
 * fixed G++ warnings
 *
 * Revision 3.0  1998/01/31 21:02:06  brianp
 * initial rev
 *
 */


/*
 * Quadrilateral rendering functions.
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include "types.h"
#include "quads.h"
#endif



/*
 * At this time there is no quadrilateral optimization.  Just call the
 * triangle function twice.
 * v0, v1, v2, v3 in CCW order = front facing.
 */
static void basic_quad( GLcontext *ctx,
                  GLuint v0, GLuint v1, GLuint v2, GLuint v3, GLuint pv )
{
   (*ctx->Driver.TriangleFunc)( ctx, v0, v1, v3, pv );
   (*ctx->Driver.TriangleFunc)( ctx, v1, v2, v3, pv );
}



/*
 * Draw nothing (NULL raster mode)
 */
static void null_quad( GLcontext *ctx,
                       GLuint v0, GLuint v1, GLuint v2, GLuint v3, GLuint pv )
{
   (void) ctx;
   (void) v0;
   (void) v1;
   (void) v2;
   (void) v3;
   (void) pv;
}



void gl_set_quad_function( GLcontext *ctx )
{
   if (ctx->RenderMode==GL_RENDER) {
      if (ctx->NoRaster) {
         ctx->Driver.QuadFunc = null_quad;
      }
      else if (ctx->Driver.QuadFunc) {
         /* Device driver will draw quads. */
      }
      else {
         ctx->Driver.QuadFunc = basic_quad;
      }
   }
   else {
      /* if in feedback or selection mode we can fall back to triangle code */
      ctx->Driver.QuadFunc = basic_quad;
   }      
}


