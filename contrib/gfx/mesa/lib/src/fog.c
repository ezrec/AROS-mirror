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
 * Revision 3.2  1998/03/27 03:37:40  brianp
 * fixed G++ warnings
 *
 * Revision 3.1  1998/02/08 20:18:41  brianp
 * removed unneeded headers
 *
 * Revision 3.0  1998/01/31 20:52:49  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <math.h>
#include <stdlib.h>
#include "context.h"
#include "fog.h"
#include "macros.h"
#include "types.h"
#endif



void gl_Fogfv( GLcontext *ctx, GLenum pname, const GLfloat *params )
{
   GLenum m;

   switch (pname) {
      case GL_FOG_MODE:
         m = (GLenum) (GLint) *params;
	 if (m==GL_LINEAR || m==GL_EXP || m==GL_EXP2) {
	    ctx->Fog.Mode = m;
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glFog" );
	 }
	 break;
      case GL_FOG_DENSITY:
	 if (*params<0.0) {
	    gl_error( ctx, GL_INVALID_VALUE, "glFog" );
	 }
	 else {
	    ctx->Fog.Density = *params;
	 }
	 break;
      case GL_FOG_START:
#if 0
         /* Prior to OpenGL 1.1, this was an error */
         if (*params<0.0F) {
            gl_error( ctx, GL_INVALID_VALUE, "glFog(GL_FOG_START)" );
            return;
         }
#endif
	 ctx->Fog.Start = *params;
	 break;
      case GL_FOG_END:
#if 0
         /* Prior to OpenGL 1.1, this was an error */
         if (*params<0.0F) {
            gl_error( ctx, GL_INVALID_VALUE, "glFog(GL_FOG_END)" );
            return;
         }
#endif
	 ctx->Fog.End = *params;
	 break;
      case GL_FOG_INDEX:
	 ctx->Fog.Index = *params;
	 break;
      case GL_FOG_COLOR:
	 ctx->Fog.Color[0] = params[0];
	 ctx->Fog.Color[1] = params[1];
	 ctx->Fog.Color[2] = params[2];
	 ctx->Fog.Color[3] = params[3];
         break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glFog" );
   }
}




/*
 * Compute the fogged color for an array of vertices.
 * Input:  n - number of vertices
 *         v - array of vertices
 *         color - the original vertex colors
 * Output:  color - the fogged colors
 */
void gl_fog_rgba_vertices( const GLcontext *ctx,
                            GLuint n, GLfloat v[][4], GLubyte color[][4] )
{
   GLuint i;
   GLfloat d;
   GLfloat rFog = ctx->Fog.Color[0] * 255.0F;
   GLfloat gFog = ctx->Fog.Color[1] * 255.0F;
   GLfloat bFog = ctx->Fog.Color[2] * 255.0F;
   GLfloat end = ctx->Fog.End;

   switch (ctx->Fog.Mode) {
      case GL_LINEAR:
         d = 1.0F / (ctx->Fog.End - ctx->Fog.Start);
         for (i=0;i<n;i++) {
            GLfloat f = (end - ABSF(v[i][2])) * d;
            f = CLAMP( f, 0.0F, 1.0F );
            color[i][0] = (GLint) (f * color[i][0] + (1.0F-f) * rFog);
            color[i][1] = (GLint) (f * color[i][1] + (1.0F-f) * gFog);
            color[i][2] = (GLint) (f * color[i][2] + (1.0F-f) * bFog);
         }
	 break;
      case GL_EXP:
         d = -ctx->Fog.Density;
         for (i=0;i<n;i++) {
            GLfloat f = exp( d * ABSF(v[i][2]) );
            f = CLAMP( f, 0.0F, 1.0F );
            color[i][0] = (GLint) (f * color[i][0] + (1.0F-f) * rFog);
            color[i][1] = (GLint) (f * color[i][1] + (1.0F-f) * gFog);
            color[i][2] = (GLint) (f * color[i][2] + (1.0F-f) * bFog);
         }
	 break;
      case GL_EXP2:
         d = -(ctx->Fog.Density*ctx->Fog.Density);
         for (i=0;i<n;i++) {
            GLfloat z = ABSF(v[i][2]);
            GLfloat f = exp( d * z*z );
            f = CLAMP( f, 0.0F, 1.0F );
            color[i][0] = (GLint) (f * color[i][0] + (1.0F-f) * rFog);
            color[i][1] = (GLint) (f * color[i][1] + (1.0F-f) * gFog);
            color[i][2] = (GLint) (f * color[i][2] + (1.0F-f) * bFog);
         }
	 break;
      default:
         gl_problem(ctx, "Bad fog mode in gl_fog_rgba_vertices");
         return;
   }
}



/*
 * Compute the fogged color indexes for an array of vertices.
 * Input:  n - number of vertices
 *         v - array of vertices
 * In/Out: indx - array of vertex color indexes
 */
void gl_fog_ci_vertices( const GLcontext *ctx,
                            GLuint n, GLfloat v[][4], GLuint indx[] )
{
   /* NOTE: the extensive use of casts generates better/faster code for MIPS */
   switch (ctx->Fog.Mode) {
      case GL_LINEAR:
         {
            GLfloat d = 1.0F / (ctx->Fog.End - ctx->Fog.Start);
            GLfloat fogindex = ctx->Fog.Index;
            GLfloat fogend = ctx->Fog.End;
            GLuint i;
            for (i=0;i<n;i++) {
               GLfloat f = (fogend - ABSF(v[i][2])) * d;
               f = CLAMP( f, 0.0F, 1.0F );
               indx[i] = (GLint)
                         ((GLfloat) (GLint) indx[i] + (1.0F-f) * fogindex);
            }
         }
	 break;
      case GL_EXP:
         {
            GLfloat d = -ctx->Fog.Density;
            GLfloat fogindex = ctx->Fog.Index;
            GLuint i;
            for (i=0;i<n;i++) {
               GLfloat f = exp( d * ABSF(v[i][2]) );
               f = CLAMP( f, 0.0F, 1.0F );
               indx[i] = (GLint)
                         ((GLfloat) (GLint) indx[i] + (1.0F-f) * fogindex);
            }
         }
	 break;
      case GL_EXP2:
         {
            GLfloat d = -(ctx->Fog.Density*ctx->Fog.Density);
            GLfloat fogindex = ctx->Fog.Index;
            GLuint i;
            for (i=0;i<n;i++) {
               GLfloat z = ABSF(v[i][2]);
               GLfloat f = exp( -d * z*z );
               f = CLAMP( f, 0.0F, 1.0F );
               indx[i] = (GLint)
                         ((GLfloat) (GLint) indx[i] + (1.0F-f) * fogindex);
            }
         }
	 break;
      default:
         gl_problem(ctx, "Bad fog mode in gl_fog_ci_vertices");
         return;
   }
}




/*
 * Apply fog to an array of RGBA pixels.
 * Input:  n - number of pixels
 *         z - array of integer depth values
 *         red, green, blue, alpha - pixel colors
 * Output:  red, green, blue, alpha - fogged pixel colors
 */
void gl_fog_rgba_pixels( const GLcontext *ctx,
                         GLuint n, const GLdepth z[], GLubyte rgba[][4] )
{
   GLfloat c = ctx->ProjectionMatrix[10];
   GLfloat d = ctx->ProjectionMatrix[14];
   GLuint i;

   GLfloat rFog = ctx->Fog.Color[0] * 255.0F;
   GLfloat gFog = ctx->Fog.Color[1] * 255.0F;
   GLfloat bFog = ctx->Fog.Color[2] * 255.0F;

   GLfloat tz = ctx->Viewport.Tz;
   GLfloat szInv = 1.0F / ctx->Viewport.Sz;

   switch (ctx->Fog.Mode) {
      case GL_LINEAR:
         {
            GLfloat fogEnd = ctx->Fog.End;
            GLfloat fogScale = 1.0F / (ctx->Fog.End - ctx->Fog.Start);
            for (i=0;i<n;i++) {
               GLfloat ndcz = ((GLfloat) z[i] - tz) * szInv;
               GLfloat eyez = -d / (c+ndcz);
               GLfloat f, g;
               if (eyez < 0.0)  eyez = -eyez;
               f = (fogEnd - eyez) * fogScale;
               f = CLAMP( f, 0.0F, 1.0F );
               g = 1.0F - f;
               rgba[i][RCOMP] = (GLint) (f * rgba[i][RCOMP] + g * rFog);
               rgba[i][GCOMP] = (GLint) (f * rgba[i][GCOMP] + g * gFog);
               rgba[i][BCOMP] = (GLint) (f * rgba[i][BCOMP] + g * bFog);
            }
         }
	 break;
      case GL_EXP:
	 for (i=0;i<n;i++) {
	    GLfloat ndcz = ((GLfloat) z[i] - tz) * szInv;
	    GLfloat eyez = -d / (c+ndcz);
            GLfloat f, g;
	    if (eyez < 0.0)  eyez = -eyez;
	    f = exp( -ctx->Fog.Density * eyez );
	    f = CLAMP( f, 0.0F, 1.0F );
            g = 1.0F - f;
            rgba[i][RCOMP] = (GLint) (f * rgba[i][RCOMP] + g * rFog);
            rgba[i][GCOMP] = (GLint) (f * rgba[i][GCOMP] + g * gFog);
            rgba[i][BCOMP] = (GLint) (f * rgba[i][BCOMP] + g * bFog);
	 }
	 break;
      case GL_EXP2:
         {
            GLfloat negDensitySquared = -ctx->Fog.Density * ctx->Fog.Density;
            for (i=0;i<n;i++) {
               GLfloat ndcz = ((GLfloat) z[i] - tz) * szInv;
               GLfloat eyez = -d / (c+ndcz);
               GLfloat f, g;
               if (eyez < 0.0)  eyez = -eyez;
               f = exp( negDensitySquared * eyez*eyez );
               f = CLAMP( f, 0.0F, 1.0F );
               g = 1.0F - f;
               rgba[i][RCOMP] = (GLint) (f * rgba[i][RCOMP] + g * rFog);
               rgba[i][GCOMP] = (GLint) (f * rgba[i][GCOMP] + g * gFog);
               rgba[i][BCOMP] = (GLint) (f * rgba[i][BCOMP] + g * bFog);
            }
         }
	 break;
      default:
         gl_problem(ctx, "Bad fog mode in gl_fog_rgba_pixels");
         return;
   }
}




/*
 * Apply fog to an array of color index pixels.
 * Input:  n - number of pixels
 *         z - array of integer depth values
 *         index - pixel color indexes
 * Output:  index - fogged pixel color indexes
 */
void gl_fog_ci_pixels( const GLcontext *ctx,
                          GLuint n, const GLdepth z[], GLuint index[] )
{
   GLfloat c = ctx->ProjectionMatrix[10];
   GLfloat d = ctx->ProjectionMatrix[14];
   GLuint i;

   GLfloat tz = ctx->Viewport.Tz;
   GLfloat szInv = 1.0F / ctx->Viewport.Sz;

   switch (ctx->Fog.Mode) {
      case GL_LINEAR:
         {
            GLfloat fogEnd = ctx->Fog.End;
            GLfloat fogScale = 1.0F / (ctx->Fog.End - ctx->Fog.Start);
            for (i=0;i<n;i++) {
               GLfloat ndcz = ((GLfloat) z[i] - tz) * szInv;
               GLfloat eyez = -d / (c+ndcz);
               GLfloat f;
               if (eyez < 0.0)  eyez = -eyez;
               f = (fogEnd - eyez) * fogScale;
               f = CLAMP( f, 0.0F, 1.0F );
               index[i] = (GLuint) ((GLfloat) index[i] + (1.0F-f) * ctx->Fog.Index);
            }
	 }
	 break;
      case GL_EXP:
         for (i=0;i<n;i++) {
	    GLfloat ndcz = ((GLfloat) z[i] - tz) * szInv;
	    GLfloat eyez = -d / (c+ndcz);
            GLfloat f;
	    if (eyez < 0.0)  eyez = -eyez;
	    f = exp( -ctx->Fog.Density * eyez );
	    f = CLAMP( f, 0.0F, 1.0F );
	    index[i] = (GLuint) ((GLfloat) index[i] + (1.0F-f) * ctx->Fog.Index);
	 }
	 break;
      case GL_EXP2:
         {
            GLfloat negDensitySquared = -ctx->Fog.Density * ctx->Fog.Density;
            for (i=0;i<n;i++) {
               GLfloat ndcz = ((GLfloat) z[i] - tz) * szInv;
               GLfloat eyez = -d / (c+ndcz);
               GLfloat f;
               if (eyez < 0.0)  eyez = -eyez;
               f = exp( negDensitySquared * eyez*eyez );
               f = CLAMP( f, 0.0F, 1.0F );
               index[i] = (GLuint) ((GLfloat) index[i] + (1.0F-f) * ctx->Fog.Index);
            }
	 }
	 break;
      default:
         gl_problem(ctx, "Bad fog mode in gl_fog_ci_pixels");
         return;
   }
}

