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
 * Revision 3.4  1998/07/30 02:40:41  brianp
 * always feedback texture coords, even when texturing disabled
 *
 * Revision 3.3  1998/02/20 04:50:44  brianp
 * implemented GL_SGIS_multitexture
 *
 * Revision 3.2  1998/02/08 20:20:08  brianp
 * added assert.h
 *
 * Revision 3.1  1998/02/02 03:09:34  brianp
 * added GL_LIGHT_MODEL_COLOR_CONTROL (separate specular color interpolation)
 *
 * Revision 3.0  1998/01/31 21:02:29  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <assert.h>
#include "clip.h"
#include "feedback.h"
#include "light.h"
#include "macros.h"
#include "matrix.h"
#include "mmath.h"
#include "shade.h"
#include "types.h"
#include "xform.h"
#endif


/*
 * Caller:  context->API.RasterPos4f
 */
void gl_RasterPos4f( GLcontext *ctx,
                     GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   GLfloat v[4], eye[4], clip[4], ndc[3], d;

   ASSIGN_4V( v, x, y, z, w );

   if (ctx->NewModelViewMatrix) {
      gl_analyze_modelview_matrix(ctx);
   }
   if (ctx->NewProjectionMatrix) {
      gl_analyze_projection_matrix(ctx);
   }
   if (ctx->NewTextureMatrix) {
      gl_analyze_texture_matrix(ctx);
   }

   /* transform v to eye coords:  eye = ModelView * v */
   TRANSFORM_POINT( eye, ctx->ModelViewMatrix, v );

   /* raster color */
   if (ctx->Light.Enabled) {
      GLfloat eyenorm[3];
      TRANSFORM_NORMAL( eyenorm[0], eyenorm[1], eyenorm[2], ctx->Current.Normal,
                        ctx->ModelViewInv );
      if (ctx->Visual->RGBAflag) {
         GLubyte color[4];
         gl_shade_rgba( ctx, 0, 1, &eye, &eyenorm, &color );
         ctx->Current.RasterColor[0] = color[0] * (1.0F / 255.0F);
         ctx->Current.RasterColor[1] = color[1] * (1.0F / 255.0F);
         ctx->Current.RasterColor[2] = color[2] * (1.0F / 255.0F);
         ctx->Current.RasterColor[3] = color[3] * (1.0F / 255.0F);
      }
      else {
	 gl_shade_ci( ctx, 0, 1, &eye, &eyenorm, &ctx->Current.RasterIndex );
      }
   }
   else {
      /* use current color or index */
      if (ctx->Visual->RGBAflag) {
         GLfloat *rc = ctx->Current.RasterColor;
         rc[0] = ctx->Current.ByteColor[0] * (1.0F / 255.0F);
         rc[1] = ctx->Current.ByteColor[1] * (1.0F / 255.0F);
         rc[2] = ctx->Current.ByteColor[2] * (1.0F / 255.0F);
         rc[3] = ctx->Current.ByteColor[3] * (1.0F / 255.0F);
      }
      else {
	 ctx->Current.RasterIndex = ctx->Current.Index;
      }
   }

   /* clip to user clipping planes */
   if (gl_userclip_point(ctx, eye)==0) {
      ctx->Current.RasterPosValid = GL_FALSE;
      return;
   }

   /* compute raster distance */
   ctx->Current.RasterDistance =
                      GL_SQRT( eye[0]*eye[0] + eye[1]*eye[1] + eye[2]*eye[2] );

   /* apply projection matrix:  clip = Proj * eye */
   TRANSFORM_POINT( clip, ctx->ProjectionMatrix, eye );

   /* clip to view volume */
   if (gl_viewclip_point( clip )==0) {
      ctx->Current.RasterPosValid = GL_FALSE;
      return;
   }

   /* ndc = clip / W */
   ASSERT( clip[3]!=0.0 );
   d = 1.0F / clip[3];
   ndc[0] = clip[0] * d;
   ndc[1] = clip[1] * d;
   ndc[2] = clip[2] * d;

   ctx->Current.RasterPos[0] = ndc[0] * ctx->Viewport.Sx + ctx->Viewport.Tx;
   ctx->Current.RasterPos[1] = ndc[1] * ctx->Viewport.Sy + ctx->Viewport.Ty;
   ctx->Current.RasterPos[2] = (ndc[2] * ctx->Viewport.Sz + ctx->Viewport.Tz)
                               / DEPTH_SCALE;
   ctx->Current.RasterPos[3] = clip[3];
   ctx->Current.RasterPosValid = GL_TRUE;

   /* FOG??? */

   {
      GLuint texSet;
      for (texSet=0; texSet<MAX_TEX_SETS; texSet++) {
         COPY_4V( ctx->Current.RasterMultiTexCoord[texSet],
                  ctx->Current.MultiTexCoord[texSet] );
      }
   }

   if (ctx->RenderMode==GL_SELECT) {
      gl_update_hitflag( ctx, ctx->Current.RasterPos[2] );
   }

}



/*
 * This is a MESA extension function.  Pretty much just like glRasterPos
 * except we don't apply the modelview or projection matrices; specify a
 * window coordinate directly.
 * Caller:  context->API.WindowPos4fMESA pointer.
 */
void gl_windowpos( GLcontext *ctx, GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   /* set raster position */
   ctx->Current.RasterPos[0] = x;
   ctx->Current.RasterPos[1] = y;
   ctx->Current.RasterPos[2] = CLAMP( z, 0.0F, 1.0F );
   ctx->Current.RasterPos[3] = w;

   ctx->Current.RasterPosValid = GL_TRUE;

   /* raster color */
   if (ctx->Light.Enabled) {
      GLfloat eye[4];
      GLfloat eyenorm[3];
      COPY_4V( eye, ctx->Current.RasterPos );
      if (ctx->NewModelViewMatrix) {
	 gl_analyze_modelview_matrix(ctx);
      }
      TRANSFORM_NORMAL( eyenorm[0], eyenorm[1], eyenorm[2],
                        ctx->Current.Normal,
                        ctx->ModelViewInv );
      if (ctx->Visual->RGBAflag) {
         GLubyte color[4];
         gl_shade_rgba( ctx, 0, 1, &eye, &eyenorm, &color );
         ASSIGN_4V( ctx->Current.RasterColor, 
                    (GLfloat) color[0] * (1.0F / 255.0F),
                    (GLfloat) color[1] * (1.0F / 255.0F),
                    (GLfloat) color[2] * (1.0F / 255.0F),
                    (GLfloat) color[3] * (1.0F / 255.0F) );
      }
      else {
	 gl_shade_ci( ctx, 0, 1, &eye, &eyenorm, &ctx->Current.RasterIndex );
      }
   }
   else {
      /* use current color or index */
      if (ctx->Visual->RGBAflag) {
         ASSIGN_4V( ctx->Current.RasterColor,
                    ctx->Current.ByteColor[0] * (1.0F / 255.0F),
                    ctx->Current.ByteColor[1] * (1.0F / 255.0F),
                    ctx->Current.ByteColor[2] * (1.0F / 255.0F),
                    ctx->Current.ByteColor[3] * (1.0F / 255.0F) );
      }
      else {
	 ctx->Current.RasterIndex = ctx->Current.Index;
      }
   }

   ctx->Current.RasterDistance = 0.0;

   {
      GLuint texSet;
      for (texSet=0; texSet<MAX_TEX_SETS; texSet++) {
         COPY_4V( ctx->Current.RasterMultiTexCoord[texSet],
                  ctx->Current.MultiTexCoord[texSet] );
      }
   }

   if (ctx->RenderMode==GL_SELECT) {
      gl_update_hitflag( ctx, ctx->Current.RasterPos[2] );
   }
}
