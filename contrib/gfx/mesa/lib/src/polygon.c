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
 * Revision 3.3  1998/08/21 02:43:30  brianp
 * implemented true packing/unpacking of polygon stipples
 *
 * Revision 3.2  1998/07/29 04:08:31  brianp
 * implemented glGetPolygonStipple()
 *
 * Revision 3.1  1998/03/27 04:33:17  brianp
 * fixed G++ warnings
 *
 * Revision 3.0  1998/01/31 21:01:27  brianp
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
#include "polygon.h"
#include "types.h"
#endif



void gl_CullFace( GLcontext *ctx, GLenum mode )
{
   if (mode!=GL_FRONT && mode!=GL_BACK && mode!=GL_FRONT_AND_BACK) {
      gl_error( ctx, GL_INVALID_ENUM, "glCullFace" );
      return;
   }
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glCullFace" );
      return;
   }
   ctx->Polygon.CullFaceMode = mode;
   ctx->NewState |= NEW_POLYGON;
}



void gl_FrontFace( GLcontext *ctx, GLenum mode )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glFrontFace" );
      return;
   }
   if (mode!=GL_CW && mode!=GL_CCW) {
      gl_error( ctx, GL_INVALID_ENUM, "glFrontFace" );
      return;
   }
   ctx->Polygon.FrontFace = mode;
}



void gl_PolygonMode( GLcontext *ctx, GLenum face, GLenum mode )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glPolygonMode" );
      return;
   }
   if (face!=GL_FRONT && face!=GL_BACK && face!=GL_FRONT_AND_BACK) {
      gl_error( ctx, GL_INVALID_ENUM, "glPolygonMode(face)" );
      return;
   }
   else if (mode!=GL_POINT && mode!=GL_LINE && mode!=GL_FILL) {
      gl_error( ctx, GL_INVALID_ENUM, "glPolygonMode(mode)" );
      return;
   }

   if (face==GL_FRONT || face==GL_FRONT_AND_BACK) {
      ctx->Polygon.FrontMode = mode;
   }
   if (face==GL_BACK || face==GL_FRONT_AND_BACK) {
      ctx->Polygon.BackMode = mode;
   }

   /* Compute a handy "shortcut" value: */
   if (ctx->Polygon.FrontMode!=GL_FILL || ctx->Polygon.BackMode!=GL_FILL) {
      ctx->Polygon.Unfilled = GL_TRUE;
   }
   else {
      ctx->Polygon.Unfilled = GL_FALSE;
   }

   ctx->NewState |= NEW_POLYGON;
}



/*
 * NOTE:  stipple pattern has already been unpacked.
 */
void gl_PolygonStipple( GLcontext *ctx, const GLuint pattern[32] )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glPolygonStipple" );
      return;
   }

   MEMCPY( ctx->PolygonStipple, pattern, 32 * 4 );

   if (ctx->Polygon.StippleFlag) {
      ctx->NewState |= NEW_RASTER_OPS;
   }
}



void gl_GetPolygonStipple( GLcontext *ctx, GLubyte *dest )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glPolygonOffset" );
      return;
   }

   gl_pack_polygon_stipple( ctx, ctx->PolygonStipple, dest );
}



void gl_PolygonOffset( GLcontext *ctx,
                       GLfloat factor, GLfloat units )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glPolygonOffset" );
      return;
   }
   ctx->Polygon.OffsetFactor = factor;
   ctx->Polygon.OffsetUnits = units;
}

