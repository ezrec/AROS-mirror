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
 * Revision 3.9  1998/08/16 14:45:44  brianp
 * fixed clipping problem in gl_read_rgba_span() (Karl Schultz)
 *
 * Revision 3.8  1998/06/07 22:18:52  brianp
 * implemented GL_EXT_multitexture extension
 *
 * Revision 3.7  1998/03/28 03:57:13  brianp
 * added CONST macro to fix IRIX compilation problems
 *
 * Revision 3.6  1998/03/27 04:17:31  brianp
 * fixed G++ warnings
 *
 * Revision 3.5  1998/03/15 17:55:54  brianp
 * added FogMode to context struct
 *
 * Revision 3.4  1998/03/10 01:27:47  brianp
 * fixed bugs in the backup of color arrays
 *
 * Revision 3.3  1998/02/20 04:50:44  brianp
 * implemented GL_SGIS_multitexture
 *
 * Revision 3.2  1998/02/03 04:26:07  brianp
 * removed const from lambda[] passed to gl_write_texture_span()
 *
 * Revision 3.1  1998/02/02 03:09:34  brianp
 * added GL_LIGHT_MODEL_COLOR_CONTROL (separate specular color interpolation)
 *
 * Revision 3.0  1998/01/31 21:03:42  brianp
 * initial rev
 *
 */


/*
 * pixel span rasterization:
 * These functions simulate the rasterization pipeline.
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <assert.h>
#include <string.h>
#include "alpha.h"
#include "alphabuf.h"
#include "blend.h"
#include "depth.h"
#include "fog.h"
#include "logic.h"
#include "macros.h"
#include "masking.h"
#include "scissor.h"
#include "span.h"
#include "stencil.h"
#include "texture.h"
#include "types.h"
#endif




/*
 * Apply the current polygon stipple pattern to a span of pixels.
 */
static void stipple_polygon_span( GLcontext *ctx,
                                  GLuint n, GLint x, GLint y, GLubyte mask[] )
{
   register GLuint i, m, stipple, highbit=0x80000000;

   stipple = ctx->PolygonStipple[y % 32];
   m = highbit >> (GLuint) (x % 32);

   for (i=0;i<n;i++) {
      if ((m & stipple)==0) {
	 mask[i] = 0;
      }
      m = m >> 1;
      if (m==0) {
	 m = 0x80000000;
      }
   }
}



/*
 * Clip a pixel span to the current buffer/window boundaries.
 * Return:  0 = all pixels clipped
 *          1 = at least one pixel is visible
 */
static GLuint clip_span( GLcontext *ctx,
                         GLint n, GLint x, GLint y, GLubyte mask[] )
{
   GLint i;

   /* Clip to top and bottom */
   if (y<0 || y>=ctx->Buffer->Height) {
      return 0;
   }

   /* Clip to left and right */
   if (x>=0 && x+n<=ctx->Buffer->Width) {
      /* no clipping needed */
      return 1;
   }
   else if (x+n<=0) {
      /* completely off left side */
      return 0;
   }
   else if (x>=ctx->Buffer->Width) {
      /* completely off right side */
      return 0;
   }
   else {
      /* clip-test each pixel, this could be done better */
      for (i=0;i<n;i++) {
         if (x+i<0 || x+i>=ctx->Buffer->Width) {
            mask[i] = 0;
         }
      }
      return 1;
   }
}



/*
 * Write a horizontal span of color index pixels to the frame buffer.
 * Stenciling, Depth-testing, etc. are done as needed.
 * Input:  n - number of pixels in the span
 *         x, y - location of leftmost pixel in the span
 *         z - array of [n] z-values
 *         index - array of [n] color indexes
 *         primitive - either GL_POINT, GL_LINE, GL_POLYGON, or GL_BITMAP
 */
void gl_write_index_span( GLcontext *ctx,
                          GLuint n, GLint x, GLint y, const GLdepth z[],
			  GLuint indexIn[], GLenum primitive )
{
   GLubyte mask[MAX_WIDTH];
   GLuint indexBackup[MAX_WIDTH];
   GLuint *index;  /* points to indexIn or indexBackup */

   /* init mask to 1's (all pixels are to be written) */
   MEMSET(mask, 1, n);

   if ((ctx->RasterMask & WINCLIP_BIT) || primitive==GL_BITMAP) {
      if (clip_span(ctx,n,x,y,mask)==0) {
	 return;
      }
   }

   if ((primitive==GL_BITMAP && ctx->MutablePixels)
       || (ctx->RasterMask & FRONT_AND_BACK_BIT)) {
      /* Make copy of color indexes */
      MEMCPY( indexBackup, indexIn, n * sizeof(GLuint) );
      index = indexBackup;
   }
   else {
      index = indexIn;
   }

   /* Per-pixel fog */
   if (ctx->Fog.Enabled && (primitive==GL_BITMAP || ctx->FogMode == FOG_FRAGMENT)) {
      gl_fog_ci_pixels( ctx, n, z, index );
   }

   /* Do the scissor test */
   if (ctx->Scissor.Enabled) {
      if (gl_scissor_span( ctx, n, x, y, mask )==0) {
	 return;
      }
   }

   /* Polygon Stippling */
   if (ctx->Polygon.StippleFlag && primitive==GL_POLYGON) {
      stipple_polygon_span( ctx, n, x, y, mask );
   }

   if (ctx->Stencil.Enabled) {
      /* first stencil test */
      if (gl_stencil_span( ctx, n, x, y, mask )==0) {
	 return;
      }
      /* depth buffering w/ stencil */
      gl_depth_stencil_span( ctx, n, x, y, z, mask );
   }
   else if (ctx->Depth.Test) {
      /* regular depth testing */
      if ((*ctx->Driver.DepthTestSpan)( ctx, n, x, y, z, mask )==0)  return;
   }

   if (ctx->RasterMask & NO_DRAW_BIT) {
      /* write no pixels */
      return;
   }

   if (ctx->Color.SWLogicOpEnabled) {
      gl_logicop_ci_span( ctx, n, x, y, index, mask );
   }
   if (ctx->Color.SWmasking) {
      gl_mask_index_span( ctx, n, x, y, index );
   }

   /* write pixels */
   (*ctx->Driver.WriteCI32Span)( ctx, n, x, y, index, mask );


   if (ctx->RasterMask & FRONT_AND_BACK_BIT) {
      /*** Also draw to back buffer ***/
      (*ctx->Driver.SetBuffer)( ctx, GL_BACK );
      MEMCPY( indexBackup, indexIn, n * sizeof(GLuint) );
      assert( index == indexBackup );
      if (ctx->Color.SWLogicOpEnabled) {
         gl_logicop_ci_span( ctx, n, x, y, index, mask );
      }
      if (ctx->Color.SWmasking) {
         gl_mask_index_span( ctx, n, x, y, index );
      }
      (*ctx->Driver.WriteCI32Span)( ctx, n, x, y, index, mask );
      (*ctx->Driver.SetBuffer)( ctx, GL_FRONT );
   }
}




void gl_write_monoindex_span( GLcontext *ctx,
                              GLuint n, GLint x, GLint y, const GLdepth z[],
			      GLuint index, GLenum primitive )
{
   GLuint i;
   GLubyte mask[MAX_WIDTH];
   GLuint indexBackup[MAX_WIDTH];

   /* init mask to 1's (all pixels are to be written) */
   MEMSET(mask, 1, n);

   if ((ctx->RasterMask & WINCLIP_BIT) || primitive==GL_BITMAP) {
      if (clip_span( ctx, n, x, y, mask)==0) {
	 return;
      }
   }

   /* Do the scissor test */
   if (ctx->Scissor.Enabled) {
      if (gl_scissor_span( ctx, n, x, y, mask )==0) {
	 return;
      }
   }

   /* Polygon Stippling */
   if (ctx->Polygon.StippleFlag && primitive==GL_POLYGON) {
      stipple_polygon_span( ctx, n, x, y, mask );
   }

   if (ctx->Stencil.Enabled) {
      /* first stencil test */
      if (gl_stencil_span( ctx, n, x, y, mask )==0) {
	 return;
      }
      /* depth buffering w/ stencil */
      gl_depth_stencil_span( ctx, n, x, y, z, mask );
   }
   else if (ctx->Depth.Test) {
      /* regular depth testing */
      if ((*ctx->Driver.DepthTestSpan)( ctx, n, x, y, z, mask )==0)  return;
   }

   if (ctx->RasterMask & NO_DRAW_BIT) {
      /* write no pixels */
      return;
   }

   if ((ctx->Fog.Enabled && (primitive==GL_BITMAP || ctx->FogMode==FOG_FRAGMENT))
        || ctx->Color.SWLogicOpEnabled || ctx->Color.SWmasking) {
      GLuint ispan[MAX_WIDTH];
      /* index may change, replicate single index into an array */
      for (i=0;i<n;i++) {
	 ispan[i] = index;
      }

      if (ctx->Fog.Enabled && (primitive==GL_BITMAP || ctx->FogMode==FOG_FRAGMENT)) {
	 gl_fog_ci_pixels( ctx, n, z, ispan );
      }

      if (ctx->RasterMask & FRONT_AND_BACK_BIT) {
         MEMCPY( indexBackup, ispan, n * sizeof(GLuint) );
      }

      if (ctx->Color.SWLogicOpEnabled) {
	 gl_logicop_ci_span( ctx, n, x, y, ispan, mask );
      }

      if (ctx->Color.SWmasking) {
         gl_mask_index_span( ctx, n, x, y, ispan );
      }

      (*ctx->Driver.WriteCI32Span)( ctx, n, x, y, ispan, mask );

      if (ctx->RasterMask & FRONT_AND_BACK_BIT) {
         /*** Also draw to back buffer ***/
         MEMCPY( ispan, indexBackup, n * sizeof(GLuint) );
         (*ctx->Driver.SetBuffer)( ctx, GL_BACK );
         for (i=0;i<n;i++) {
            ispan[i] = index;
         }
         if (ctx->Color.SWLogicOpEnabled) {
            gl_logicop_ci_span( ctx, n, x, y, ispan, mask );
         }
         if (ctx->Color.SWmasking) {
            gl_mask_index_span( ctx, n, x, y, ispan );
         }
         (*ctx->Driver.WriteCI32Span)( ctx, n, x, y, ispan, mask );
         (*ctx->Driver.SetBuffer)( ctx, GL_FRONT );
      }
   }
   else {
      (*ctx->Driver.WriteMonoCISpan)( ctx, n, x, y, mask );

      if (ctx->RasterMask & FRONT_AND_BACK_BIT) {
         /*** Also draw to back buffer ***/
         (*ctx->Driver.SetBuffer)( ctx, GL_BACK );
         (*ctx->Driver.WriteMonoCISpan)( ctx, n, x, y, mask );
         (*ctx->Driver.SetBuffer)( ctx, GL_FRONT );
      }
   }
}



void gl_write_rgba_span( GLcontext *ctx,
                         GLuint n, GLint x, GLint y, const GLdepth z[],
                         GLubyte rgbaIn[][4],
			  GLenum primitive )
{
   GLubyte mask[MAX_WIDTH];
   GLboolean write_all = GL_TRUE;
   GLubyte rgbaBackup[MAX_WIDTH][4];
   GLubyte (*rgba)[4];
   const GLubyte *Null = 0;

   /* init mask to 1's (all pixels are to be written) */
   MEMSET(mask, 1, n);

   if ((ctx->RasterMask & WINCLIP_BIT) || primitive==GL_BITMAP) {
      if (clip_span( ctx,n,x,y,mask)==0) {
	 return;
      }
      write_all = GL_FALSE;
   }

   if ((primitive==GL_BITMAP && ctx->MutablePixels)
       || (ctx->RasterMask & FRONT_AND_BACK_BIT)) {
      /* must make a copy of the colors since they may be modified */
      MEMCPY( rgbaBackup, rgbaIn, 4 * n * sizeof(GLubyte) );
      rgba = rgbaBackup;
   }
   else {
      rgba = rgbaIn;
   }

   /* Per-pixel fog */
   if (ctx->Fog.Enabled && (primitive==GL_BITMAP || ctx->FogMode==FOG_FRAGMENT)) {
      gl_fog_rgba_pixels( ctx, n, z, rgba );
   }

   /* Do the scissor test */
   if (ctx->Scissor.Enabled) {
      if (gl_scissor_span( ctx, n, x, y, mask )==0) {
	 return;
      }
      write_all = GL_FALSE;
   }

   /* Polygon Stippling */
   if (ctx->Polygon.StippleFlag && primitive==GL_POLYGON) {
      stipple_polygon_span( ctx, n, x, y, mask );
      write_all = GL_FALSE;
   }

   /* Do the alpha test */
   if (ctx->Color.AlphaEnabled) {
      if (gl_alpha_test( ctx, n, rgba, mask )==0) {
	 return;
      }
      write_all = GL_FALSE;
   }

   if (ctx->Stencil.Enabled) {
      /* first stencil test */
      if (gl_stencil_span( ctx, n, x, y, mask )==0) {
	 return;
      }
      /* depth buffering w/ stencil */
      gl_depth_stencil_span( ctx, n, x, y, z, mask );
      write_all = GL_FALSE;
   }
   else if (ctx->Depth.Test) {
      /* regular depth testing */
      GLuint m = (*ctx->Driver.DepthTestSpan)( ctx, n, x, y, z, mask );
      if (m==0) {
         return;
      }
      if (m<n) {
         write_all = GL_FALSE;
      }
   }

   if (ctx->RasterMask & NO_DRAW_BIT) {
      /* write no pixels */
      return;
   }

   /* logic op or blending */
   if (ctx->Color.SWLogicOpEnabled) {
      gl_logicop_rgba_span( ctx, n, x, y, rgba, mask );
   }
   else if (ctx->Color.BlendEnabled) {
      gl_blend_span( ctx, n, x, y, rgba, mask );
   }

   /* Color component masking */
   if (ctx->Color.SWmasking) {
      gl_mask_rgba_span( ctx, n, x, y, rgba );
   }

   /* write pixels */
   (*ctx->Driver.WriteRGBASpan)( ctx, n, x, y, rgba, write_all ? Null : mask );
   if (ctx->RasterMask & ALPHABUF_BIT) {
      gl_write_alpha_span( ctx, n, x, y, rgba, write_all ? Null : mask );
   }

   if (ctx->RasterMask & FRONT_AND_BACK_BIT) {
      /*** Also render to back buffer ***/
      MEMCPY( rgbaBackup, rgbaIn, 4 * n * sizeof(GLubyte) );
      assert( rgba == rgbaBackup );
      (*ctx->Driver.SetBuffer)( ctx, GL_BACK );
      if (ctx->Color.SWLogicOpEnabled) {
         gl_logicop_rgba_span( ctx, n, x, y, rgba, mask );
      }
      else  if (ctx->Color.BlendEnabled) {
         gl_blend_span( ctx, n, x, y, rgba, mask );
      }
      if (ctx->Color.SWmasking) {
         gl_mask_rgba_span( ctx, n, x, y, rgba );
      }
      (*ctx->Driver.WriteRGBASpan)( ctx, n, x, y, rgba, write_all ? Null : mask );
      if (ctx->RasterMask & ALPHABUF_BIT) {
         ctx->Buffer->Alpha = ctx->Buffer->BackAlpha;
         gl_write_alpha_span( ctx, n, x, y, rgba, write_all ? Null : mask );
         ctx->Buffer->Alpha = ctx->Buffer->FrontAlpha;
      }
      (*ctx->Driver.SetBuffer)( ctx, GL_FRONT );
   }

}



/*
 * Write a horizontal span of color pixels to the frame buffer.
 * The color is initially constant for the whole span.
 * Alpha-testing, stenciling, depth-testing, and blending are done as needed.
 * Input:  n - number of pixels in the span
 *         x, y - location of leftmost pixel in the span
 *         z - array of [n] z-values
 *         r, g, b, a - the color of the pixels
 *         primitive - either GL_POINT, GL_LINE, GL_POLYGON or GL_BITMAP.
 */
void gl_write_monocolor_span( GLcontext *ctx,
                              GLuint n, GLint x, GLint y, const GLdepth z[],
			      GLint r, GLint g, GLint b, GLint a,
                              GLenum primitive )
{
   GLuint i;
   GLubyte mask[MAX_WIDTH];
   GLboolean write_all = GL_TRUE;
   GLubyte rgba[MAX_WIDTH][4];
   const GLubyte *Null = 0;

   /* init mask to 1's (all pixels are to be written) */
   MEMSET(mask, 1, n);

   if ((ctx->RasterMask & WINCLIP_BIT) || primitive==GL_BITMAP) {
      if (clip_span( ctx,n,x,y,mask)==0) {
	 return;
      }
      write_all = GL_FALSE;
   }

   /* Do the scissor test */
   if (ctx->Scissor.Enabled) {
      if (gl_scissor_span( ctx, n, x, y, mask )==0) {
	 return;
      }
      write_all = GL_FALSE;
   }

   /* Polygon Stippling */
   if (ctx->Polygon.StippleFlag && primitive==GL_POLYGON) {
      stipple_polygon_span( ctx, n, x, y, mask );
      write_all = GL_FALSE;
   }

   /* Do the alpha test */
   if (ctx->Color.AlphaEnabled) {
      for (i=0;i<n;i++) {
         rgba[i][ACOMP] = a;
      }
      if (gl_alpha_test( ctx, n, rgba, mask )==0) {
	 return;
      }
      write_all = GL_FALSE;
   }

   if (ctx->Stencil.Enabled) {
      /* first stencil test */
      if (gl_stencil_span( ctx, n, x, y, mask )==0) {
	 return;
      }
      /* depth buffering w/ stencil */
      gl_depth_stencil_span( ctx, n, x, y, z, mask );
      write_all = GL_FALSE;
   }
   else if (ctx->Depth.Test) {
      /* regular depth testing */
      GLuint m = (*ctx->Driver.DepthTestSpan)( ctx, n, x, y, z, mask );
      if (m==0) {
         return;
      }
      if (m<n) {
         write_all = GL_FALSE;
      }
   }

   if (ctx->RasterMask & NO_DRAW_BIT) {
      /* write no pixels */
      return;
   }

   if (ctx->Color.BlendEnabled || ctx->Color.SWLogicOpEnabled
       || ctx->Color.SWmasking) {
      /* assign same color to each pixel */
      for (i=0;i<n;i++) {
	 if (mask[i]) {
	    rgba[i][RCOMP] = r;
	    rgba[i][GCOMP] = g;
	    rgba[i][BCOMP] = b;
	    rgba[i][ACOMP] = a;
	 }
      }

      if (ctx->Color.SWLogicOpEnabled) {
         gl_logicop_rgba_span( ctx, n, x, y, rgba, mask );
      }
      else if (ctx->Color.BlendEnabled) {
         gl_blend_span( ctx, n, x, y, rgba, mask );
      }

      /* Color component masking */
      if (ctx->Color.SWmasking) {
         gl_mask_rgba_span( ctx, n, x, y, rgba );
      }

      /* write pixels */
      (*ctx->Driver.WriteRGBASpan)( ctx, n, x, y, rgba, write_all ? Null : mask );
      if (ctx->RasterMask & ALPHABUF_BIT) {
         gl_write_alpha_span( ctx, n, x, y, rgba, write_all ? Null : mask );
      }

      if (ctx->RasterMask & FRONT_AND_BACK_BIT) {
         /*** Also draw to back buffer ***/
         for (i=0;i<n;i++) {
            if (mask[i]) {
               rgba[i][RCOMP] = r;
               rgba[i][GCOMP] = g;
               rgba[i][BCOMP] = b;
               rgba[i][ACOMP] = a;
            }
         }
         (*ctx->Driver.SetBuffer)( ctx, GL_BACK );
         if (ctx->Color.SWLogicOpEnabled) {
            gl_logicop_rgba_span( ctx, n, x, y, rgba, mask);
         }
         else if (ctx->Color.BlendEnabled) {
            gl_blend_span( ctx, n, x, y, rgba, mask );
         }
         if (ctx->Color.SWmasking) {
            gl_mask_rgba_span( ctx, n, x, y, rgba );
         }
         (*ctx->Driver.WriteRGBASpan)( ctx, n, x, y, rgba, write_all ? Null : mask );
         (*ctx->Driver.SetBuffer)( ctx, GL_FRONT );
         if (ctx->RasterMask & ALPHABUF_BIT) {
            ctx->Buffer->Alpha = ctx->Buffer->BackAlpha;
            gl_write_alpha_span( ctx, n, x, y, rgba,
                                 write_all ? Null : mask );
            ctx->Buffer->Alpha = ctx->Buffer->FrontAlpha;
         }
      }
   }
   else {
      (*ctx->Driver.WriteMonoRGBASpan)( ctx, n, x, y, mask );
      if (ctx->RasterMask & ALPHABUF_BIT) {
         gl_write_mono_alpha_span( ctx, n, x, y, a, write_all ? Null : mask );
      }
      if (ctx->RasterMask & FRONT_AND_BACK_BIT) {
         /* Also draw to back buffer */
         (*ctx->Driver.SetBuffer)( ctx, GL_BACK );
         (*ctx->Driver.WriteMonoRGBASpan)( ctx, n, x, y, mask );
         (*ctx->Driver.SetBuffer)( ctx, GL_FRONT );
         if (ctx->RasterMask & ALPHABUF_BIT) {
            ctx->Buffer->Alpha = ctx->Buffer->BackAlpha;
            gl_write_mono_alpha_span( ctx, n, x, y, a,
                                      write_all ? Null : mask );
            ctx->Buffer->Alpha = ctx->Buffer->FrontAlpha;
         }
      }
   }
}



/*
 * Add specular color to base color.  This is used only when
 * GL_LIGHT_MODEL_COLOR_CONTROL = GL_SEPARATE_SPECULAR_COLOR.
 */
static void add_colors(GLuint n, GLubyte rgba[][4], CONST GLubyte specular[][4] )
{
   GLuint i;
   for (i=0; i<n; i++) {
      GLint r = rgba[i][RCOMP] + specular[i][RCOMP];
      GLint g = rgba[i][GCOMP] + specular[i][GCOMP];
      GLint b = rgba[i][BCOMP] + specular[i][BCOMP];
      rgba[i][RCOMP] = MIN2(r, 255);
      rgba[i][GCOMP] = MIN2(g, 255);
      rgba[i][BCOMP] = MIN2(b, 255);
   }
}


/*
 * Write a horizontal span of textured pixels to the frame buffer.
 * The color of each pixel is different.
 * Alpha-testing, stenciling, depth-testing, and blending are done
 * as needed.
 * Input:  n - number of pixels in the span
 *         x, y - location of leftmost pixel in the span
 *         z - array of [n] z-values
 *         s, t - array of (s,t) texture coordinates for each pixel
 *         lambda - array of texture lambda values
 *         rgba - array of [n] color components
 *         primitive - either GL_POINT, GL_LINE, GL_POLYGON or GL_BITMAP.
 */
void gl_write_texture_span( GLcontext *ctx,
                            GLuint n, GLint x, GLint y, const GLdepth z[],
			    const GLfloat s[], const GLfloat t[],
                            const GLfloat u[], GLfloat lambda[],
			    GLubyte rgbaIn[][4], CONST GLubyte spec[][4],
			    GLenum primitive )
{
   GLubyte mask[MAX_WIDTH];
   GLboolean write_all = GL_TRUE;
   GLubyte rgbaBackup[MAX_WIDTH][4];
   GLubyte (*rgba)[4];   /* points to either rgbaIn or rgbaBackup */
   const GLubyte *Null = 0;

   /* init mask to 1's (all pixels are to be written) */
   MEMSET(mask, 1, n);

   if ((ctx->RasterMask & WINCLIP_BIT) || primitive==GL_BITMAP) {
      if (clip_span(ctx, n, x, y, mask)==0) {
	 return;
      }
      write_all = GL_FALSE;
   }


   if (primitive==GL_BITMAP || (ctx->RasterMask & FRONT_AND_BACK_BIT)) {
      /* must make a copy of the colors since they may be modified */
      MEMCPY(rgbaBackup, rgbaIn, 4 * sizeof(GLubyte));
      rgba = rgbaBackup;
   }
   else {
      rgba = rgbaIn;
   }

   /* Texture */
   ASSERT(ctx->Texture.Enabled);
   gl_texture_pixels( ctx, 0, n, s, t, u, lambda, rgba );

   /* Add base and specular colors */
   if (spec && ctx->Light.Model.ColorControl == GL_SEPARATE_SPECULAR_COLOR)
      add_colors( n, rgba, spec );   /* rgba = rgba + spec */

   /* Per-pixel fog */
   if (ctx->Fog.Enabled && (primitive==GL_BITMAP || ctx->FogMode==FOG_FRAGMENT)) {
      gl_fog_rgba_pixels( ctx, n, z, rgba );
   }

   /* Do the scissor test */
   if (ctx->Scissor.Enabled) {
      if (gl_scissor_span( ctx, n, x, y, mask )==0) {
	 return;
      }
      write_all = GL_FALSE;
   }

   /* Polygon Stippling */
   if (ctx->Polygon.StippleFlag && primitive==GL_POLYGON) {
      stipple_polygon_span( ctx, n, x, y, mask );
      write_all = GL_FALSE;
   }

   /* Do the alpha test */
   if (ctx->Color.AlphaEnabled) {
      if (gl_alpha_test( ctx, n, rgba, mask )==0) {
	 return;
      }
      write_all = GL_FALSE;
   }

   if (ctx->Stencil.Enabled) {
      /* first stencil test */
      if (gl_stencil_span( ctx, n, x, y, mask )==0) {
	 return;
      }
      /* depth buffering w/ stencil */
      gl_depth_stencil_span( ctx, n, x, y, z, mask );
      write_all = GL_FALSE;
   }
   else if (ctx->Depth.Test) {
      /* regular depth testing */
      GLuint m = (*ctx->Driver.DepthTestSpan)( ctx, n, x, y, z, mask );
      if (m==0) {
         return;
      }
      if (m<n) {
         write_all = GL_FALSE;
      }
   }

   if (ctx->RasterMask & NO_DRAW_BIT) {
      /* write no pixels */
      return;
   }

   if (ctx->RasterMask & FRONT_AND_BACK_BIT) {
      /* make backup of fragment colors */
      MEMCPY( rgbaBackup, rgba, 4 * n * sizeof(GLubyte) );
   }

   /* blending */
   if (ctx->Color.SWLogicOpEnabled) {
      gl_logicop_rgba_span( ctx, n, x, y, rgba, mask );
   }
   else  if (ctx->Color.BlendEnabled) {
      gl_blend_span( ctx, n, x, y, rgba, mask );
   }

   if (ctx->Color.SWmasking) {
      gl_mask_rgba_span( ctx, n, x, y, rgba );
   }

   /* write pixels */
   (*ctx->Driver.WriteRGBASpan)( ctx, n, x, y, rgba, write_all ? Null : mask );
   if (ctx->RasterMask & ALPHABUF_BIT) {
      gl_write_alpha_span( ctx, n, x, y, rgba, write_all ? Null : mask );
   }

   if (ctx->RasterMask & FRONT_AND_BACK_BIT) {
      /* Also draw to back buffer */
      (*ctx->Driver.SetBuffer)( ctx, GL_BACK );
      if (ctx->Color.SWLogicOpEnabled) {
         gl_logicop_rgba_span( ctx, n, x, y, rgba, mask );
      }
      else if (ctx->Color.BlendEnabled) {
         gl_blend_span( ctx, n, x, y, rgba, mask );
      }
      if (ctx->Color.SWmasking) {
         gl_mask_rgba_span( ctx, n, x, y, rgba );
      }
      (*ctx->Driver.WriteRGBASpan)( ctx, n, x, y, rgba, write_all ? Null : mask );
      (*ctx->Driver.SetBuffer)( ctx, GL_FRONT );
      if (ctx->RasterMask & ALPHABUF_BIT) {
         ctx->Buffer->Alpha = ctx->Buffer->BackAlpha;
         gl_write_alpha_span( ctx, n, x, y, rgba, write_all ? Null : mask );
         ctx->Buffer->Alpha = ctx->Buffer->FrontAlpha;
      }
   }
}



/*
 * As above but perform multiple stages of texture application.
 * Input:  texSets - number of texture sets to apply
 */
void gl_write_multitexture_span( GLcontext *ctx, GLuint texSets,
                                 GLuint n, GLint x, GLint y, const GLdepth z[],
                                 CONST GLfloat s[][MAX_WIDTH],
                                 CONST GLfloat t[][MAX_WIDTH],
                                 CONST GLfloat u[][MAX_WIDTH],
                                 GLfloat lambda[][MAX_WIDTH],
                                 GLubyte rgbaIn[][4], CONST GLubyte spec[][4],
			    GLenum primitive )
{
   GLubyte mask[MAX_WIDTH];
   GLboolean write_all = GL_TRUE;
   GLubyte rgbaBackup[MAX_WIDTH][4];
   GLubyte (*rgba)[4];   /* points to either rgbaIn or rgbaBackup */
   GLuint i;
   const GLubyte *Null = 0;

   /* init mask to 1's (all pixels are to be written) */
   MEMSET(mask, 1, n);

   if ((ctx->RasterMask & WINCLIP_BIT) || primitive==GL_BITMAP) {
      if (clip_span(ctx, n, x, y, mask)==0) {
	 return;
      }
      write_all = GL_FALSE;
   }


   if (primitive==GL_BITMAP || (ctx->RasterMask & FRONT_AND_BACK_BIT)) {
      /* must make a copy of the colors since they may be modified */
      MEMCPY(rgbaBackup, rgbaIn, 4 * sizeof(GLubyte));
      rgba = rgbaBackup;
   }
   else {
      rgba = rgbaIn;
   }

   /* Texture */
   ASSERT(ctx->Texture.Enabled);
   ASSERT(texSets <= MAX_TEX_SETS);
   for (i=0;i<texSets;i++) {
      GLuint j = ctx->Texture.Set[i].TexCoordSet;
      /* Evaluate i_th texture environment using the j_th set of
       * texture coords
       */
      gl_texture_pixels( ctx, i, n, s[j], t[j], u[j], lambda[j], rgba );
   }


   /* Add base and specular colors */
   if (spec && ctx->Light.Model.ColorControl == GL_SEPARATE_SPECULAR_COLOR)
      add_colors( n, rgba, spec );   /* rgba = rgba + spec */

   /* Per-pixel fog */
   if (ctx->Fog.Enabled && (primitive==GL_BITMAP || ctx->FogMode==FOG_FRAGMENT)) {
      gl_fog_rgba_pixels( ctx, n, z, rgba );
   }

   /* Do the scissor test */
   if (ctx->Scissor.Enabled) {
      if (gl_scissor_span( ctx, n, x, y, mask )==0) {
	 return;
      }
      write_all = GL_FALSE;
   }

   /* Polygon Stippling */
   if (ctx->Polygon.StippleFlag && primitive==GL_POLYGON) {
      stipple_polygon_span( ctx, n, x, y, mask );
      write_all = GL_FALSE;
   }

   /* Do the alpha test */
   if (ctx->Color.AlphaEnabled) {
      if (gl_alpha_test( ctx, n, rgba, mask )==0) {
	 return;
      }
      write_all = GL_FALSE;
   }

   if (ctx->Stencil.Enabled) {
      /* first stencil test */
      if (gl_stencil_span( ctx, n, x, y, mask )==0) {
	 return;
      }
      /* depth buffering w/ stencil */
      gl_depth_stencil_span( ctx, n, x, y, z, mask );
      write_all = GL_FALSE;
   }
   else if (ctx->Depth.Test) {
      /* regular depth testing */
      GLuint m = (*ctx->Driver.DepthTestSpan)( ctx, n, x, y, z, mask );
      if (m==0) {
         return;
      }
      if (m<n) {
         write_all = GL_FALSE;
      }
   }

   if (ctx->RasterMask & NO_DRAW_BIT) {
      /* write no pixels */
      return;
   }

   if (ctx->RasterMask & FRONT_AND_BACK_BIT) {
      /* make backup of fragment colors */
      MEMCPY( rgbaBackup, rgba, 4 * n * sizeof(GLubyte) );
   }

   /* blending */
   if (ctx->Color.SWLogicOpEnabled) {
      gl_logicop_rgba_span( ctx, n, x, y, rgba, mask );
   }
   else  if (ctx->Color.BlendEnabled) {
      gl_blend_span( ctx, n, x, y, rgba, mask );
   }

   if (ctx->Color.SWmasking) {
      gl_mask_rgba_span( ctx, n, x, y, rgba );
   }

   /* write pixels */
   (*ctx->Driver.WriteRGBASpan)( ctx, n, x, y, rgba, write_all ? Null : mask );
   if (ctx->RasterMask & ALPHABUF_BIT) {
      gl_write_alpha_span( ctx, n, x, y, rgba, write_all ? Null : mask );
   }

   if (ctx->RasterMask & FRONT_AND_BACK_BIT) {
      /* Also draw to back buffer */
      (*ctx->Driver.SetBuffer)( ctx, GL_BACK );
      if (ctx->Color.SWLogicOpEnabled) {
         gl_logicop_rgba_span( ctx, n, x, y, rgba, mask );
      }
      else if (ctx->Color.BlendEnabled) {
         gl_blend_span( ctx, n, x, y, rgba, mask );
      }
      if (ctx->Color.SWmasking) {
         gl_mask_rgba_span( ctx, n, x, y, rgba );
      }
      (*ctx->Driver.WriteRGBASpan)( ctx, n, x, y, rgba, write_all ? Null : mask );
      (*ctx->Driver.SetBuffer)( ctx, GL_FRONT );
      if (ctx->RasterMask & ALPHABUF_BIT) {
         ctx->Buffer->Alpha = ctx->Buffer->BackAlpha;
         gl_write_alpha_span( ctx, n, x, y, rgba, write_all ? Null : mask );
         ctx->Buffer->Alpha = ctx->Buffer->FrontAlpha;
      }
   }
}



/*
 * Read RGBA pixels from frame buffer.  Clipping will be done to prevent
 * reading ouside the buffer's boundaries.
 */
void gl_read_rgba_span( GLcontext *ctx,
                         GLuint n, GLint x, GLint y,
                        GLubyte rgba[][4] )
{
   if (y<0 || y>=ctx->Buffer->Height || x>=ctx->Buffer->Width) {
      /* completely above, below, or right */
      MEMSET( rgba, 0, 4 * n * sizeof(GLubyte)); /*XXX maybe leave undefined?*/
   }
   else {
      GLint skip, length;
      if (x < 0) {
         /* left edge clippping */
         skip = -x;
         length = (GLint) n - skip;
         if (length < 0) {
            /* completely left of window */
            return;
         }
         if (length > ctx->Buffer->Width) {
            length = ctx->Buffer->Width;
         }
      }
      else if ((GLint) (x + n) > ctx->Buffer->Width) {
         /* right edge clipping */
         skip = 0;
         length = ctx->Buffer->Width - x;
         if (length < 0) {
            /* completely to right of window */
            return;
         }
      }
      else {
         /* no clipping */
         skip = 0;
         length = (GLint) n;
	    }

      (*ctx->Driver.ReadRGBASpan)( ctx, length, x + skip, y, rgba + skip );
         if (ctx->RasterMask & ALPHABUF_BIT) {
         gl_read_alpha_span( ctx, length, x + skip, y, rgba + skip );
      }
   }
}




/*
 * Read CI pixels from frame buffer.  Clipping will be done to prevent
 * reading ouside the buffer's boundaries.
 */
void gl_read_index_span( GLcontext *ctx,
                         GLuint n, GLint x, GLint y, GLuint indx[] )
{
   register GLuint i;

   if (y<0 || y>=ctx->Buffer->Height || x>=ctx->Buffer->Width) {
      /* completely above, below, or right */
      for (i=0;i<n;i++) {
	 indx[i] = 0;
      }
   }
   else {
      GLint skip, length;
      if (x < 0) {
         /* left edge clippping */
         skip = -x;
         length = (GLint) n - skip;
         if (length < 0) {
            /* completely left of window */
            return;
         }
         if (length > ctx->Buffer->Width) {
            length = ctx->Buffer->Width;
         }
      }
      else if ((GLint) (x + n) > ctx->Buffer->Width) {
         /* right edge clipping */
         skip = 0;
         length = ctx->Buffer->Width - x;
         if (length < 0) {
            /* completely to right of window */
            return;
         }
      }
      else {
         /* no clipping */
         skip = 0;
         length = (GLint) n;
      }

      (*ctx->Driver.ReadCI32Span)( ctx, length, skip + x, y, indx + skip );
   }
}
