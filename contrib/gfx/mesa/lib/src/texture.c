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
 * Revision 3.8  1998/06/11 02:04:41  brianp
 * fixed texture clamp problem when coord==1 (bad sampling)
 *
 * Revision 3.7  1998/03/28 03:58:16  brianp
 * added CONST macro to fix IRIX compilation problems
 *
 * Revision 3.6  1998/03/27 04:26:44  brianp
 * fixed G++ warnings
 *
 * Revision 3.5  1998/03/15 18:12:37  brianp
 * fixed sampling bug when filter=NEAREST and wrap=CLAMP
 *
 * Revision 3.4  1998/03/01 20:19:26  brianp
 * fixed a few sampling functions to prevent access of missing mipmap levels
 *
 * Revision 3.3  1998/02/20 04:53:37  brianp
 * implemented GL_SGIS_multitexture
 *
 * Revision 3.2  1998/02/03 04:27:54  brianp
 * added texture lod clamping
 *
 * Revision 3.1  1998/02/01 19:39:09  brianp
 * added GL_CLAMP_TO_EDGE texture wrap mode
 *
 * Revision 3.0  1998/01/31 21:04:38  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <math.h>
#include <stdlib.h>
#include "context.h"
#include "macros.h"
#include "mmath.h"
#include "pb.h"
#include "texture.h"
#include "types.h"
#endif



/*
 * Perform automatic texture coordinate generation.
 * Input:  ctx - the context
 *         n - number of texture coordinates to generate
 *         obj - array of vertexes in object coordinate system
 *         eye - array of vertexes in eye coordinate system
 *         normal - array of normal vectores in eye coordinate system
 * Output:  texcoord - array of resuling texture coordinates
 */
void gl_texgen( GLcontext *ctx, GLint n,
                GLfloat obj[][4], GLfloat eye[][4],
                GLfloat normal[][3], GLfloat texcoord[][4],
                GLuint textureSet)
{
   struct gl_texture_set *texSet = &ctx->Texture.Set[textureSet];

   /* special case: S and T sphere mapping */
   if (texSet->TexGenEnabled==(S_BIT|T_BIT)
       && texSet->GenModeS==GL_SPHERE_MAP
       && texSet->GenModeT==GL_SPHERE_MAP) {
      GLint i;
      for (i=0;i<n;i++) {
         GLfloat u[3], two_nu, m, fx, fy, fz;
         COPY_3V( u, eye[i] );
         NORMALIZE_3FV( u );
         two_nu = 2.0F * DOT3(normal[i],u);
         fx = u[0] - normal[i][0] * two_nu;
         fy = u[1] - normal[i][1] * two_nu;
         fz = u[2] - normal[i][2] * two_nu;
         m = 2.0F * GL_SQRT( fx*fx + fy*fy + (fz+1.0F)*(fz+1.0F) );
         if (m==0.0F) {
            texcoord[i][0] = 0.5F;
            texcoord[i][1] = 0.5F;
         }
         else {
            GLfloat mInv = 1.0F / m;
            texcoord[i][0] = fx * mInv + 0.5F;
            texcoord[i][1] = fy * mInv + 0.5F;
         }
      }
      return;
   }

   /* general solution */
   if (texSet->TexGenEnabled & S_BIT) {
      GLint i;
      switch (texSet->GenModeS) {
	 case GL_OBJECT_LINEAR:
            for (i=0;i<n;i++) {
               texcoord[i][0] = DOT4( obj[i], texSet->ObjectPlaneS );
            }
	    break;
	 case GL_EYE_LINEAR:
            for (i=0;i<n;i++) {
               texcoord[i][0] = DOT4( eye[i], texSet->EyePlaneS );
            }
	    break;
	 case GL_SPHERE_MAP:
            for (i=0;i<n;i++) {
               GLfloat u[3], two_nu, m, fx, fy, fz;
               COPY_3V( u, eye[i] );
               NORMALIZE_3FV( u );
               two_nu = 2.0*DOT3(normal[i],u);
               fx = u[0] - normal[i][0] * two_nu;
               fy = u[1] - normal[i][1] * two_nu;
               fz = u[2] - normal[i][2] * two_nu;
               m = 2.0F * GL_SQRT( fx*fx + fy*fy + (fz+1.0)*(fz+1.0) );
               if (m==0.0F) {
                  texcoord[i][0] = 0.5F;
               }
               else {
                  texcoord[i][0] = fx / m + 0.5F;
               }
            }
	    break;
         default:
            gl_problem(ctx, "Bad S texgen");
            return;
      }
   }

   if (texSet->TexGenEnabled & T_BIT) {
      GLint i;
      switch (texSet->GenModeT) {
	 case GL_OBJECT_LINEAR:
            for (i=0;i<n;i++) {
               texcoord[i][1] = DOT4( obj[i], texSet->ObjectPlaneT );
            }
	    break;
	 case GL_EYE_LINEAR:
            for (i=0;i<n;i++) {
               texcoord[i][1] = DOT4( eye[i], texSet->EyePlaneT );
            }
	    break;
	 case GL_SPHERE_MAP:
            for (i=0;i<n;i++) {
               GLfloat u[3], two_nu, m, fx, fy, fz;
               COPY_3V( u, eye[i] );
               NORMALIZE_3FV( u );
               two_nu = 2.0*DOT3(normal[i],u);
               fx = u[0] - normal[i][0] * two_nu;
               fy = u[1] - normal[i][1] * two_nu;
               fz = u[2] - normal[i][2] * two_nu;
               m = 2.0F * GL_SQRT( fx*fx + fy*fy + (fz+1.0)*(fz+1.0) );
               if (m==0.0F) {
                  texcoord[i][1] = 0.5F;
               }
               else {
                  texcoord[i][1] = fy / m + 0.5F;
               }
            }
	    break;
         default:
            gl_problem(ctx, "Bad T texgen");
            return;
      }
   }

   if (texSet->TexGenEnabled & R_BIT) {
      GLint i;
      switch (texSet->GenModeR) {
	 case GL_OBJECT_LINEAR:
            for (i=0;i<n;i++) {
               texcoord[i][2] = DOT4( obj[i], texSet->ObjectPlaneR );
            }
	    break;
	 case GL_EYE_LINEAR:
            for (i=0;i<n;i++) {
               texcoord[i][2] = DOT4( eye[i], texSet->EyePlaneR );
            }
	    break;
         default:
            gl_problem(ctx, "Bad R texgen");
            return;
      }
   }

   if (texSet->TexGenEnabled & Q_BIT) {
      GLint i;
      switch (texSet->GenModeQ) {
	 case GL_OBJECT_LINEAR:
            for (i=0;i<n;i++) {
               texcoord[i][3] = DOT4( obj[i], texSet->ObjectPlaneQ );
            }
	    break;
	 case GL_EYE_LINEAR:
            for (i=0;i<n;i++) {
               texcoord[i][3] = DOT4( eye[i], texSet->EyePlaneQ );
            }
	    break;
         default:
            gl_problem(ctx, "Bad Q texgen");
            return;
      }
   }
}



/*
 * Paletted texture sampling.
 * Input:  tObj - the texture object
 *         index - the palette index (8-bit only)
 * Output:  red, green, blue, alpha - the texel color
 */
static void palette_sample(const struct gl_texture_object *tObj,
                           GLubyte index, GLubyte rgba[4] )
{
   GLcontext *ctx = gl_get_current_context();  /* THIS IS A HACK */
   GLint i = index;
   const GLubyte *palette;

   if (ctx->Texture.SharedPalette) {
      palette = ctx->Texture.Palette;
   }
   else {
      palette = tObj->Palette;
   }

   switch (tObj->PaletteFormat) {
      case GL_ALPHA:
         rgba[ACOMP] = tObj->Palette[index];
         return;
      case GL_LUMINANCE:
      case GL_INTENSITY:
         rgba[RCOMP] = palette[index];
         return;
      case GL_LUMINANCE_ALPHA:
         rgba[RCOMP] = palette[(index << 1) + 0];
         rgba[ACOMP] = palette[(index << 1) + 1];
         return;
      case GL_RGB:
         rgba[RCOMP] = palette[index * 3 + 0];
         rgba[GCOMP] = palette[index * 3 + 1];
         rgba[BCOMP] = palette[index * 3 + 2];
         return;
      case GL_RGBA:
         rgba[RCOMP] = palette[(i << 2) + 0];
         rgba[GCOMP] = palette[(i << 2) + 1];
         rgba[BCOMP] = palette[(i << 2) + 2];
         rgba[ACOMP] = palette[(i << 2) + 3];
         return;
      default:
         gl_problem(NULL, "Bad palette format in palette_sample");
   }
}




/**********************************************************************/
/*                    1-D Texture Sampling Functions                  */
/**********************************************************************/


/*
 * Return the fractional part of x.
 */
#define frac(x) ((GLfloat)(x)-floor((GLfloat)x))



/*
 * Given 1-D texture image and an (i) texel column coordinate, return the
 * texel color.
 */
static void get_1d_texel( const struct gl_texture_object *tObj,
                          const struct gl_texture_image *img, GLint i,
                          GLubyte rgba[4] )
{
   GLubyte *texel;

#ifdef DEBUG
   GLint width = img->Width;
   if (i<0 || i>=width)  abort();
#endif

   switch (img->Format) {
      case GL_COLOR_INDEX:
         {
            GLubyte index = img->Data[i];
            palette_sample(tObj, index, rgba);
            return;
         }
      case GL_ALPHA:
         rgba[ACOMP] = img->Data[ i ];
         return;
      case GL_LUMINANCE:
      case GL_INTENSITY:
         rgba[RCOMP] = img->Data[ i ];
         return;
      case GL_LUMINANCE_ALPHA:
         texel = img->Data + i * 2;
         rgba[RCOMP] = texel[0];
         rgba[ACOMP] = texel[1];
         return;
      case GL_RGB:
         texel = img->Data + i * 3;
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         return;
      case GL_RGBA:
         texel = img->Data + i * 4;
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         rgba[ACOMP] = texel[3];
         return;
      default:
         gl_problem(NULL, "Bad format in get_1d_texel");
         return;
   }
}



/*
 * Return the texture sample for coordinate (s) using GL_NEAREST filter.
 */
static void sample_1d_nearest( const struct gl_texture_object *tObj,
                               const struct gl_texture_image *img,
                               GLfloat s, GLubyte rgba[4] )
{
   GLint width = img->Width2;  /* without border, power of two */
   GLint i;
   GLubyte *texel;

   /* Clamp/Repeat S and convert to integer texel coordinate */
   if (tObj->WrapS==GL_REPEAT) {
      /* s limited to [0,1) */
      /* i limited to [0,width-1] */
      i = (GLint) (s * width);
      if (s<0.0F)  i -= 1;
      i &= (width-1);
   }
   else if (tObj->WrapS==GL_CLAMP_TO_EDGE) {
      GLfloat min = 1.0F / (2.0F * width);
      GLfloat max = 1.0F - min;
      if (s < min)
         i = 0;
      else if (s > max)
         i = width-1;
      else
         i = (GLint) (s * width);
   }
   else {
      ASSERT(tObj->WrapS==GL_CLAMP);
      /* s limited to [0,1] */
      /* i limited to [0,width-1] */
      if (s<=0.0F)       i = 0;
      else if (s>=1.0F)  i = width-1;
      else               i = (GLint) (s * width);
   }

   /* skip over the border, if any */
   i += img->Border;

   /* Get the texel */
   switch (img->Format) {
      case GL_COLOR_INDEX:
         {
            GLubyte index = img->Data[i];
            palette_sample(tObj, index, rgba );
            return;
         }
      case GL_ALPHA:
         rgba[ACOMP] = img->Data[i];
         return;
      case GL_LUMINANCE:
      case GL_INTENSITY:
         rgba[RCOMP] = img->Data[i];
         return;
      case GL_LUMINANCE_ALPHA:
         texel = img->Data + i * 2;
         rgba[RCOMP] = texel[0];
         rgba[ACOMP] = texel[1];
         return;
      case GL_RGB:
         texel = img->Data + i * 3;
         rgba[RCOMP] = texel[0];
         rgba[RCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         return;
      case GL_RGBA:
         texel = img->Data + i * 4;
         rgba[RCOMP] = texel[0];
         rgba[RCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         rgba[ACOMP] = texel[3];
         return;
      default:
         gl_problem(NULL, "Bad format in sample_1d_nearest");
   }
}



/*
 * Return the texture sample for coordinate (s) using GL_LINEAR filter.
 */
static void sample_1d_linear( const struct gl_texture_object *tObj,
                              const struct gl_texture_image *img,
                              GLfloat s,
                              GLubyte rgba[4] )
{
   GLint width = img->Width2;
   GLint i0, i1;
   GLfloat u;
   GLint i0border, i1border;

   u = s * width;
   if (tObj->WrapS==GL_REPEAT) {
      i0 = ((GLint) floor(u - 0.5F)) % width;
      i1 = (i0 + 1) & (width-1);
      i0border = i1border = 0;
   }
   else if (tObj->WrapS==GL_CLAMP_TO_EDGE) {
      GLfloat min = 1.0F / (2.0F * width);
      GLfloat max = 1.0F - min;
      if (s < min)
         i0 = 0;
      else if (s > max)
         i0 = width-1;
      else
         i0 = (GLint) (s * width);
      i1 = i0 + 1;
      if (i1 >= width)
         i1 = width - 1;
      i0border = i1border = 0;
   }
   else {
      ASSERT(tObj->WrapS==GL_CLAMP);
      if (u < 0.0F)  u = 0.0F;
      else if (u > width)  u = width;
      i0 = (GLint) floor(u - 0.5F);
      i1 = i0 + 1;
      i0border = (i0<0) | (i0>=width);
      i1border = (i1<0) | (i1>=width);
   }

   if (img->Border) {
      i0 += img->Border;
      i1 += img->Border;
      i0border = i1border = 0;
   }
   else {
      i0 &= (width-1);
   }

   {
      GLfloat a = frac(u - 0.5F);

      GLint w0 = (GLint) ((1.0F-a) * 256.0F);
      GLint w1 = (GLint) (      a  * 256.0F);

      GLubyte t0[4], t1[4];  /* texels */

      if (i0border) {
         t0[RCOMP] = tObj->BorderColor[0];
         t0[GCOMP] = tObj->BorderColor[1];
         t0[BCOMP] = tObj->BorderColor[2];
         t0[ACOMP] = tObj->BorderColor[3];
      }
      else {
         get_1d_texel( tObj, img, i0, t0 );
      }
      if (i1border) {
         t1[RCOMP] = tObj->BorderColor[0];
         t1[GCOMP] = tObj->BorderColor[1];
         t1[BCOMP] = tObj->BorderColor[2];
         t1[ACOMP] = tObj->BorderColor[3];
      }
      else {
         get_1d_texel( tObj, img, i1, t1 );
      }

      /* MMX */
      rgba[0] = (w0 * t0[0] + w1 * t1[0]) >> 8;
      rgba[1] = (w0 * t0[1] + w1 * t1[1]) >> 8;
      rgba[2] = (w0 * t0[2] + w1 * t1[2]) >> 8;
      rgba[3] = (w0 * t0[3] + w1 * t1[3]) >> 8;
   }
}


static void
sample_1d_nearest_mipmap_nearest( const struct gl_texture_object *tObj,
                                  GLfloat s, GLfloat lambda,
                                  GLubyte rgba[4] )
{
   GLint level;
   if (lambda <= 0.5F)
      lambda = 0.0F;
   else if (lambda > tObj->M + 0.4999F)
      lambda = tObj->M + 0.4999F;
   level = (GLint) (tObj->BaseLevel + lambda + 0.5F);
   if (level > tObj->P)
      level = tObj->P;

   sample_1d_nearest( tObj, tObj->Image[level], s, rgba );
}


static void
sample_1d_linear_mipmap_nearest( const struct gl_texture_object *tObj,
                                 GLfloat s, GLfloat lambda,
                                 GLubyte rgba[4] )
{
   GLint level;
   if (lambda <= 0.5F)
      lambda = 0.0F;
   else if (lambda > tObj->M + 0.4999F)
      lambda = tObj->M + 0.4999F;
   level = (GLint) (tObj->BaseLevel + lambda + 0.5F);
   if (level > tObj->P)
      level = tObj->P;

   sample_1d_linear( tObj, tObj->Image[level], s, rgba );
}



static void
sample_1d_nearest_mipmap_linear( const struct gl_texture_object *tObj,
                                 GLfloat s, GLfloat lambda,
                                 GLubyte rgba[4] )
{
   GLint level;
   if (lambda < 0.0F)
      lambda = 0.0F;
   else if (lambda > tObj->M)
      lambda = tObj->M;
   level = (GLint) (tObj->BaseLevel + lambda);

   if (level >= tObj->P) {
      sample_1d_nearest( tObj, tObj->Image[tObj->P], s, rgba );
   }
   else {
      GLubyte t0[4], t1[4];
      GLfloat f = frac(lambda);
      sample_1d_nearest( tObj, tObj->Image[level  ], s, t0 );
      sample_1d_nearest( tObj, tObj->Image[level+1], s, t1 );
      rgba[RCOMP] = (GLint) ((1.0F-f) * t0[RCOMP] + f * t1[RCOMP]);
      rgba[RCOMP] = (GLint) ((1.0F-f) * t0[GCOMP] + f * t1[GCOMP]);
      rgba[BCOMP] = (GLint) ((1.0F-f) * t0[BCOMP] + f * t1[BCOMP]);
      rgba[ACOMP] = (GLint) ((1.0F-f) * t0[ACOMP] + f * t1[ACOMP]);
   }
}



static void
sample_1d_linear_mipmap_linear( const struct gl_texture_object *tObj,
                                GLfloat s, GLfloat lambda,
                                GLubyte rgba[4] )
{
   GLint level;
   if (lambda < 0.0F)
      lambda = 0.0F;
   else if (lambda > tObj->M)
      lambda = tObj->M;
   level = (GLint) (tObj->BaseLevel + lambda);

   if (level >= tObj->P) {
      sample_1d_linear( tObj, tObj->Image[tObj->P], s, rgba );
   }
   else {
      GLubyte t0[4], t1[4];
      GLfloat f = frac(lambda);
      sample_1d_linear( tObj, tObj->Image[level  ], s, t0 );
      sample_1d_linear( tObj, tObj->Image[level+1], s, t1 );
      rgba[RCOMP] = (GLint) ((1.0F-f) * t0[RCOMP] + f * t1[RCOMP]);
      rgba[RCOMP] = (GLint) ((1.0F-f) * t0[GCOMP] + f * t1[GCOMP]);
      rgba[BCOMP] = (GLint) ((1.0F-f) * t0[BCOMP] + f * t1[BCOMP]);
      rgba[ACOMP] = (GLint) ((1.0F-f) * t0[ACOMP] + f * t1[ACOMP]);
   }
}



static void sample_nearest_1d( const struct gl_texture_object *tObj, GLuint n,
                               const GLfloat s[], const GLfloat t[],
                               const GLfloat u[], const GLfloat lambda[],
                               GLubyte rgba[][4] )
{
   GLuint i;
   struct gl_texture_image *image = tObj->Image[tObj->BaseLevel];
   (void) t;
   (void) u;
   (void) lambda;
   for (i=0;i<n;i++) {
      sample_1d_nearest( tObj, image, s[i], rgba[i] );
   }
}



static void sample_linear_1d( const struct gl_texture_object *tObj, GLuint n,
                              const GLfloat s[], const GLfloat t[],
                              const GLfloat u[], const GLfloat lambda[],
                              GLubyte rgba[][4] )
{
   GLuint i;
   struct gl_texture_image *image = tObj->Image[tObj->BaseLevel];
   (void) t;
   (void) u;
   (void) lambda;
   for (i=0;i<n;i++) {
      sample_1d_linear( tObj, image, s[i], rgba[i] );
   }
}


/*
 * Given an (s) texture coordinate and lambda (level of detail) value,
 * return a texture sample.
 *
 */
static void sample_lambda_1d( const struct gl_texture_object *tObj, GLuint n,
                              const GLfloat s[], const GLfloat t[],
                              const GLfloat u[], const GLfloat lambda[],
                              GLubyte rgba[][4] )
{
   GLuint i;

   (void) t;
   (void) u;

   for (i=0;i<n;i++) {
      if (lambda[i] > tObj->MinMagThresh) {
      /* minification */
         switch (tObj->MinFilter) {
         case GL_NEAREST:
               sample_1d_nearest( tObj, tObj->Image[tObj->BaseLevel], s[i], rgba[i] );
            break;
         case GL_LINEAR:
               sample_1d_linear( tObj, tObj->Image[tObj->BaseLevel], s[i], rgba[i] );
            break;
         case GL_NEAREST_MIPMAP_NEAREST:
               sample_1d_nearest_mipmap_nearest( tObj, lambda[i], s[i], rgba[i] );
            break;
         case GL_LINEAR_MIPMAP_NEAREST:
               sample_1d_linear_mipmap_nearest( tObj, s[i], lambda[i], rgba[i] );
            break;
         case GL_NEAREST_MIPMAP_LINEAR:
               sample_1d_nearest_mipmap_linear( tObj, s[i], lambda[i], rgba[i] );
            break;
         case GL_LINEAR_MIPMAP_LINEAR:
               sample_1d_linear_mipmap_linear( tObj, s[i], lambda[i], rgba[i] );
            break;
         default:
               gl_problem(NULL, "Bad min filter in sample_1d_texture");
               return;
      }
   }
   else {
      /* magnification */
         switch (tObj->MagFilter) {
         case GL_NEAREST:
               sample_1d_nearest( tObj, tObj->Image[tObj->BaseLevel], s[i], rgba[i] );
            break;
         case GL_LINEAR:
               sample_1d_linear( tObj, tObj->Image[tObj->BaseLevel], s[i], rgba[i] );
            break;
         default:
               gl_problem(NULL, "Bad mag filter in sample_1d_texture");
               return;
         }
      }
   }
}




/**********************************************************************/
/*                    2-D Texture Sampling Functions                  */
/**********************************************************************/


/*
 * Given a texture image and an (i,j) integer texel coordinate, return the
 * texel color.
 */
static void get_2d_texel( const struct gl_texture_object *tObj,
                          const struct gl_texture_image *img, GLint i, GLint j,
                          GLubyte rgba[4] )
{
   GLint width = img->Width;    /* includes border */
   GLubyte *texel;

#ifdef DEBUG
   GLint height = img->Height;  /* includes border */
   if (i<0 || i>=width)  abort();
   if (j<0 || j>=height)  abort();
#endif

   switch (img->Format) {
      case GL_COLOR_INDEX:
         {
            GLubyte index = img->Data[ width *j + i ];
            palette_sample(tObj, index, rgba );
            return;
         }
      case GL_ALPHA:
         rgba[ACOMP] = img->Data[ width * j + i ];
         return;
      case GL_LUMINANCE:
      case GL_INTENSITY:
         rgba[RCOMP] = img->Data[ width * j + i ];
         return;
      case GL_LUMINANCE_ALPHA:
         texel = img->Data + (width * j + i) * 2;
         rgba[RCOMP] = texel[0];
         rgba[ACOMP] = texel[1];
         return;
      case GL_RGB:
         texel = img->Data + (width * j + i) * 3;
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         return;
      case GL_RGBA:
         texel = img->Data + (width * j + i) * 4;
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         rgba[ACOMP] = texel[3];
         return;
      default:
         gl_problem(NULL, "Bad format in get_2d_texel");
   }
}



/*
 * Return the texture sample for coordinate (s,t) using GL_NEAREST filter.
 */
static void sample_2d_nearest( const struct gl_texture_object *tObj,
                               const struct gl_texture_image *img,
                               GLfloat s, GLfloat t,
                               GLubyte rgba[] )
{
   GLint imgWidth = img->Width;  /* includes border */
   GLint width = img->Width2;    /* without border, power of two */
   GLint height = img->Height2;  /* without border, power of two */
   GLint i, j;
   GLubyte *texel;

   /* Clamp/Repeat S and convert to integer texel coordinate */
   if (tObj->WrapS==GL_REPEAT) {
      /* s limited to [0,1) */
      /* i limited to [0,width-1] */
      i = (GLint) (s * width);
      if (s<0.0F)  i -= 1;
      i &= (width-1);
   }
   else if (tObj->WrapS==GL_CLAMP_TO_EDGE) {
      GLfloat min = 1.0F / (2.0F * width);
      GLfloat max = 1.0F - min;
      if (s < min)
         i = 0;
      else if (s > max)
         i = width-1;
      else
         i = (GLint) (s * width);
   }
   else {
      ASSERT(tObj->WrapS==GL_CLAMP);
      /* s limited to [0,1] */
      /* i limited to [0,width-1] */
      if (s<=0.0F)      i = 0;
      else if (s>=1.0F) i = width-1;
      else              i = (GLint) (s * width);
   }

   /* Clamp/Repeat T and convert to integer texel coordinate */
   if (tObj->WrapT==GL_REPEAT) {
      /* t limited to [0,1) */
      /* j limited to [0,height-1] */
      j = (GLint) (t * height);
      if (t<0.0F)  j -= 1;
      j &= (height-1);
   }
   else if (tObj->WrapT==GL_CLAMP_TO_EDGE) {
      GLfloat min = 1.0F / (2.0F * height);
      GLfloat max = 1.0F - min;
      if (t < min)
         j = 0;
      else if (t > max)
         j = height-1;
      else
         j = (GLint) (t * height);
   }
   else {
      ASSERT(tObj->WrapT==GL_CLAMP);
      /* t limited to [0,1] */
      /* j limited to [0,height-1] */
      if (t<=0.0F)      j = 0;
      else if (t>=1.0F) j = height-1;
      else              j = (GLint) (t * height);
   }

   /* skip over the border, if any */
   i += img->Border;
   j += img->Border;

   switch (img->Format) {
      case GL_COLOR_INDEX:
         {
            GLubyte index = img->Data[ j * imgWidth + i ];
            palette_sample(tObj, index, rgba);
            return;
         }
      case GL_ALPHA:
         rgba[ACOMP] = img->Data[ j * imgWidth + i ];
         return;
      case GL_LUMINANCE:
      case GL_INTENSITY:
         rgba[RCOMP] = img->Data[ j * imgWidth + i ];
         return;
      case GL_LUMINANCE_ALPHA:
         texel = img->Data + ((j * imgWidth + i) << 1);
         rgba[RCOMP] = texel[0];
         rgba[ACOMP] = texel[1];
         return;
      case GL_RGB:
         texel = img->Data + (j * imgWidth + i) * 3;
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         return;
      case GL_RGBA:
         texel = img->Data + ((j * imgWidth + i) << 2);
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         rgba[ACOMP] = texel[3];
         return;
      default:
         gl_problem(NULL, "Bad format in sample_2d_nearest");
   }
}



/*
 * Return the texture sample for coordinate (s,t) using GL_LINEAR filter.
 */
static void sample_2d_linear( const struct gl_texture_object *tObj,
                              const struct gl_texture_image *img,
                              GLfloat s, GLfloat t,
                              GLubyte rgba[] )
{
   GLint width = img->Width2;
   GLint height = img->Height2;
   GLint i0, j0, i1, j1;
   GLint i0border, j0border, i1border, j1border;
   GLfloat u, v;

   u = s * width;
   if (tObj->WrapS==GL_REPEAT) {
      i0 = ((GLint) floor(u - 0.5F)) % width;
      i1 = (i0 + 1) & (width-1);
      i0border = i1border = 0;
   }
   else if (tObj->WrapS==GL_CLAMP_TO_EDGE) {
      GLfloat min = 1.0F / (2.0F * width);
      GLfloat max = 1.0F - min;
      if (s < min)
         i0 = 0;
      else if (s > max)
         i0 = width-1;
      else
         i0 = (GLint) (s * width);
      i1 = i0 + 1;
      if (i1 >= width)
         i1 = width - 1;
      i0border = i1border = 0;
   }
   else {
      ASSERT(tObj->WrapS==GL_CLAMP);
      if (u < 0.0F)  u = 0.0F;
      else if (u > width)  u = width;
      i0 = (GLint) floor(u - 0.5F);
      i1 = i0 + 1;
      i0border = (i0<0) | (i0>=width);
      i1border = (i1<0) | (i1>=width);
   }

   v = t * height;
   if (tObj->WrapT==GL_REPEAT) {
      j0 = ((GLint) floor(v - 0.5F)) % height;
      j1 = (j0 + 1) & (height-1);
      j0border = j1border = 0;
   }
   else if (tObj->WrapT==GL_CLAMP_TO_EDGE) {
      GLfloat min = 1.0F / (2.0F * height);
      GLfloat max = 1.0F - min;
      if (t < min)
         j0 = 0;
      else if (t > max)
         j0 = height-1;
      else
         j0 = (GLint) (t * height);
      j1 = j0 + 1;
      if (j1 >= height)
         j1 = height - 1;
      j0border = j1border = 0;
   }
   else {
      ASSERT(tObj->WrapT==GL_CLAMP);
      if (v < 0.0F)  v = 0.0F;
      else if (v > height)  v = height;
      j0 = (GLint) floor(v - 0.5F );
      j1 = j0 + 1;
      j0border = (j0<0) | (j0>=height);
      j1border = (j1<0) | (j1>=height);
   }

   if (img->Border) {
      i0 += img->Border;
      i1 += img->Border;
      j0 += img->Border;
      j1 += img->Border;
      i0border = i1border = 0;
      j0border = j1border = 0;
   }
   else {
      i0 &= (width-1);
      j0 &= (height-1);
   }

   {
      GLfloat a = frac(u - 0.5F);
      GLfloat b = frac(v - 0.5F);

      GLint w00 = (GLint) ((1.0F-a)*(1.0F-b) * 256.0F);
      GLint w10 = (GLint) (      a *(1.0F-b) * 256.0F);
      GLint w01 = (GLint) ((1.0F-a)*      b  * 256.0F);
      GLint w11 = (GLint) (      a *      b  * 256.0F);

      GLubyte t00[4];
      GLubyte t10[4];
      GLubyte t01[4];
      GLubyte t11[4];

      if (i0border | j0border) {
         t00[RCOMP] = tObj->BorderColor[0];
         t00[GCOMP] = tObj->BorderColor[1];
         t00[BCOMP] = tObj->BorderColor[2];
         t00[ACOMP] = tObj->BorderColor[3];
      }
      else {
         get_2d_texel( tObj, img, i0, j0, t00 );
      }
      if (i1border | j0border) {
         t10[RCOMP] = tObj->BorderColor[0];
         t10[GCOMP] = tObj->BorderColor[1];
         t10[BCOMP] = tObj->BorderColor[2];
         t10[ACOMP] = tObj->BorderColor[3];
      }
      else {
         get_2d_texel( tObj, img, i1, j0, t10 );
      }
      if (i0border | j1border) {
         t01[RCOMP] = tObj->BorderColor[0];
         t01[GCOMP] = tObj->BorderColor[1];
         t01[BCOMP] = tObj->BorderColor[2];
         t01[ACOMP] = tObj->BorderColor[3];
      }
      else {
         get_2d_texel( tObj, img, i0, j1, t01 );
      }
      if (i1border | j1border) {
         t11[RCOMP] = tObj->BorderColor[0];
         t11[GCOMP] = tObj->BorderColor[1];
         t11[BCOMP] = tObj->BorderColor[2];
         t11[ACOMP] = tObj->BorderColor[3];
      }
      else {
         get_2d_texel( tObj, img, i1, j1, t11 );
      }

      /* MMX */
      rgba[0] = (w00 * t00[0] + w10 * t10[0] + w01 * t01[0] + w11 * t11[0]) >> 8;
      rgba[1] = (w00 * t00[1] + w10 * t10[1] + w01 * t01[1] + w11 * t11[1]) >> 8;
      rgba[2] = (w00 * t00[2] + w10 * t10[2] + w01 * t01[2] + w11 * t11[2]) >> 8;
      rgba[3] = (w00 * t00[3] + w10 * t10[3] + w01 * t01[3] + w11 * t11[3]) >> 8;
   }
}



static void
sample_2d_nearest_mipmap_nearest( const struct gl_texture_object *tObj,
                                  GLfloat s, GLfloat t, GLfloat lambda,
                                  GLubyte rgba[4] )
{
   GLint level;
   if (lambda <= 0.5F)
      lambda = 0.0F;
   else if (lambda > tObj->M + 0.4999F)
      lambda = tObj->M + 0.4999F;
   level = (GLint) (tObj->BaseLevel + lambda + 0.5F);
   if (level > tObj->P)
      level = tObj->P;

   sample_2d_nearest( tObj, tObj->Image[level], s, t, rgba );
}



static void
sample_2d_linear_mipmap_nearest( const struct gl_texture_object *tObj,
                                 GLfloat s, GLfloat t, GLfloat lambda,
                                 GLubyte rgba[4] )
{
   GLint level;
   if (lambda <= 0.5F)
      lambda = 0.0F;
   else if (lambda > tObj->M + 0.4999F)
      lambda = tObj->M + 0.4999F;
   level = (GLint) (tObj->BaseLevel + lambda + 0.5F);
   if (level > tObj->P)
      level = tObj->P;

   sample_2d_linear( tObj, tObj->Image[level], s, t, rgba );
}



static void
sample_2d_nearest_mipmap_linear( const struct gl_texture_object *tObj,
                                 GLfloat s, GLfloat t, GLfloat lambda,
                                 GLubyte rgba[4] )
{
   GLint level;
   if (lambda < 0.0F)
      lambda = 0.0F;
   else if (lambda > tObj->M)
      lambda = tObj->M;
   level = (GLint) (tObj->BaseLevel + lambda);

   if (level >= tObj->P) {
      sample_2d_nearest( tObj, tObj->Image[tObj->P], s, t, rgba );
   }
   else {
      GLubyte t0[4], t1[4];  /* texels */
      GLfloat f = frac(lambda);
      sample_2d_nearest( tObj, tObj->Image[level  ], s, t, t0 );
      sample_2d_nearest( tObj, tObj->Image[level+1], s, t, t1 );
      rgba[RCOMP] = (GLint) ((1.0F-f) * t0[RCOMP] + f * t1[RCOMP]);
      rgba[GCOMP] = (GLint) ((1.0F-f) * t0[GCOMP] + f * t1[GCOMP]);
      rgba[BCOMP] = (GLint) ((1.0F-f) * t0[BCOMP] + f * t1[BCOMP]);
      rgba[ACOMP] = (GLint) ((1.0F-f) * t0[ACOMP] + f * t1[ACOMP]);
   }
}



static void
sample_2d_linear_mipmap_linear( const struct gl_texture_object *tObj,
                                GLfloat s, GLfloat t, GLfloat lambda,
                                GLubyte rgba[4] )
{
   GLint level;
   if (lambda < 0.0F)
      lambda = 0.0F;
   else if (lambda > tObj->M)
      lambda = tObj->M;
   level = (GLint) (tObj->BaseLevel + lambda);

   if (level >= tObj->P) {
      sample_2d_linear( tObj, tObj->Image[tObj->P], s, t, rgba );
   }
   else {
      GLubyte t0[4], t1[4];  /* texels */
      GLfloat f = frac(lambda);
      sample_2d_linear( tObj, tObj->Image[level  ], s, t, t0 );
      sample_2d_linear( tObj, tObj->Image[level+1], s, t, t1 );
      rgba[RCOMP] = (GLint) ((1.0F-f) * t0[RCOMP] + f * t1[RCOMP]);
      rgba[GCOMP] = (GLint) ((1.0F-f) * t0[GCOMP] + f * t1[GCOMP]);
      rgba[BCOMP] = (GLint) ((1.0F-f) * t0[BCOMP] + f * t1[BCOMP]);
      rgba[ACOMP] = (GLint) ((1.0F-f) * t0[ACOMP] + f * t1[ACOMP]);
   }
}



static void sample_nearest_2d( const struct gl_texture_object *tObj, GLuint n,
                               const GLfloat s[], const GLfloat t[],
                               const GLfloat u[], const GLfloat lambda[],
                               GLubyte rgba[][4] )
{
   GLuint i;
   struct gl_texture_image *image = tObj->Image[tObj->BaseLevel];
   (void) u;
   (void) lambda;
   for (i=0;i<n;i++) {
      sample_2d_nearest( tObj, image, s[i], t[i], rgba[i] );
   }
}



static void sample_linear_2d( const struct gl_texture_object *tObj, GLuint n,
                              const GLfloat s[], const GLfloat t[],
                              const GLfloat u[], const GLfloat lambda[],
                              GLubyte rgba[][4] )
{
   GLuint i;
   struct gl_texture_image *image = tObj->Image[tObj->BaseLevel];
   (void) u;
   (void) lambda;
   for (i=0;i<n;i++) {
      sample_2d_linear( tObj, image, s[i], t[i], rgba[i] );
   }
}


/*
 * Given an (s,t) texture coordinate and lambda (level of detail) value,
 * return a texture sample.
 */
static void sample_lambda_2d( const struct gl_texture_object *tObj,
                              GLuint n,
                              const GLfloat s[], const GLfloat t[],
                              const GLfloat u[], const GLfloat lambda[],
                              GLubyte rgba[][4] )
{
   GLuint i;
   (void) u;
   for (i=0;i<n;i++) {
      if (lambda[i] > tObj->MinMagThresh) {
      /* minification */
         switch (tObj->MinFilter) {
         case GL_NEAREST:
               sample_2d_nearest( tObj, tObj->Image[tObj->BaseLevel], s[i], t[i], rgba[i] );
            break;
         case GL_LINEAR:
               sample_2d_linear( tObj, tObj->Image[tObj->BaseLevel], s[i], t[i], rgba[i] );
            break;
         case GL_NEAREST_MIPMAP_NEAREST:
               sample_2d_nearest_mipmap_nearest( tObj, s[i], t[i], lambda[i], rgba[i] );
            break;
         case GL_LINEAR_MIPMAP_NEAREST:
               sample_2d_linear_mipmap_nearest( tObj, s[i], t[i], lambda[i], rgba[i] );
            break;
         case GL_NEAREST_MIPMAP_LINEAR:
               sample_2d_nearest_mipmap_linear( tObj, s[i], t[i], lambda[i], rgba[i] );
            break;
         case GL_LINEAR_MIPMAP_LINEAR:
               sample_2d_linear_mipmap_linear( tObj, s[i], t[i], lambda[i], rgba[i] );
            break;
         default:
               gl_problem(NULL, "Bad min filter in sample_2d_texture");
               return;
      }
   }
   else {
      /* magnification */
         switch (tObj->MagFilter) {
         case GL_NEAREST:
               sample_2d_nearest( tObj, tObj->Image[tObj->BaseLevel], s[i], t[i], rgba[i] );
            break;
         case GL_LINEAR:
               sample_2d_linear( tObj, tObj->Image[tObj->BaseLevel], s[i], t[i], rgba[i] );
            break;
         default:
               gl_problem(NULL, "Bad mag filter in sample_2d_texture");
         }
      }
   }
}


/*
 * Optimized 2-D texture sampling:
 *    S and T wrap mode == GL_REPEAT
 *    No border
 *    Format = GL_RGB
 */
static void opt_sample_rgb_2d( const struct gl_texture_object *tObj,
                               GLuint n, const GLfloat s[], const GLfloat t[],
                               const GLfloat u[], const GLfloat lambda[],
                               GLubyte rgba[][4] )
{
   const struct gl_texture_image *img = tObj->Image[tObj->BaseLevel];
   GLfloat width = img->Width, height = img->Height;
   GLint colMask = img->Width-1, rowMask = img->Height-1;
   GLint shift = img->WidthLog2;
   GLuint k;
   (void) u;
   (void) lambda;
   ASSERT(tObj->WrapS==GL_REPEAT);
   ASSERT(tObj->WrapT==GL_REPEAT);
   ASSERT(img->Border==0);
   ASSERT(img->Format==GL_RGB);

   for (k=0;k<n;k++) {
      GLint i = (GLint) (s[k] * width) & colMask;
      GLint j = (GLint) (t[k] * height) & rowMask;
      GLint pos = (j << shift) | i;
      GLubyte *texel = img->Data + pos + pos + pos;  /* pos*3 */
      rgba[k][RCOMP] = texel[0];
      rgba[k][GCOMP] = texel[1];
      rgba[k][BCOMP] = texel[2];
      }
}


/*
 * Optimized 2-D texture sampling:
 *    S and T wrap mode == GL_REPEAT
 *    No border
 *    Format = GL_RGBA
 */
static void opt_sample_rgba_2d( const struct gl_texture_object *tObj,
                                GLuint n, const GLfloat s[], const GLfloat t[],
                                const GLfloat u[], const GLfloat lambda[],
                                GLubyte rgba[][4] )
{
   const struct gl_texture_image *img = tObj->Image[tObj->BaseLevel];
   GLfloat width = img->Width, height = img->Height;
   GLint colMask = img->Width-1, rowMask = img->Height-1;
   GLint shift = img->WidthLog2;
   GLuint k;
   (void) u;
   (void) lambda;

   ASSERT(tObj->WrapS==GL_REPEAT);
   ASSERT(tObj->WrapT==GL_REPEAT);
   ASSERT(img->Border==0);
   ASSERT(img->Format==GL_RGBA);

   for (k=0;k<n;k++) {
      GLint i = (GLint) (s[k] * width) & colMask;
      GLint j = (GLint) (t[k] * height) & rowMask;
      GLint pos = (j << shift) | i;
      GLubyte *texel = img->Data + (pos << 2);    /* pos*4 */
      rgba[k][RCOMP] = texel[0];
      rgba[k][GCOMP] = texel[1];
      rgba[k][BCOMP] = texel[2];
      rgba[k][ACOMP] = texel[3];
   }
}



/**********************************************************************/
/*                    3-D Texture Sampling Functions                  */
/**********************************************************************/

/*
 * Given a texture image and an (i,j,k) integer texel coordinate, return the
 * texel color.
 */
static void get_3d_texel( const struct gl_texture_object *tObj,
                          const struct gl_texture_image *img,
                          GLint i, GLint j, GLint k,
                          GLubyte rgba[4] )
{
   GLint width = img->Width;    /* includes border */
   GLint height = img->Height;  /* includes border */
   GLint depth = img->Depth;    /* includes border */
   GLint rectarea;              /* = width * heigth */
   GLubyte *texel;

   rectarea = width*height;

#ifdef DEBUG
   if (i<0 || i>=width)  abort();
   if (j<0 || j>=height)  abort();
   if (k<0 || k>=depth)  abort();
#else
   (void) depth;
#endif

   switch (img->Format) {
      case GL_COLOR_INDEX:
         {
            GLubyte index = img->Data[ rectarea * k +  width * j + i ];
            palette_sample(tObj, index, rgba );
            return;
         }
      case GL_ALPHA:
         rgba[ACOMP] = img->Data[ rectarea * k +  width * j + i ];
         return;
      case GL_LUMINANCE:
      case GL_INTENSITY:
         rgba[RCOMP] = img->Data[ rectarea * k +  width * j + i ];
         return;
      case GL_LUMINANCE_ALPHA:
         texel = img->Data + ( rectarea * k + width * j + i) * 2;
         rgba[RCOMP] = texel[0];
         rgba[ACOMP] = texel[1];
         return;
      case GL_RGB:
         texel = img->Data + (rectarea * k + width * j + i) * 3;
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         return;
      case GL_RGBA:
         texel = img->Data + (rectarea * k + width * j + i) * 4;
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         rgba[ACOMP] = texel[3];
         return;
      default:
         gl_problem(NULL, "Bad format in get_3d_texel");
   }
}


/*
 * Return the texture sample for coordinate (s,t,r) using GL_NEAREST filter.
 */
static void sample_3d_nearest( const struct gl_texture_object *tObj,
                               const struct gl_texture_image *img,
                               GLfloat s, GLfloat t, GLfloat r,
                               GLubyte rgba[4] )
{
   GLint imgWidth = img->Width;   /* includes border, if any */
   GLint imgHeight = img->Height; /* includes border, if any */
   GLint width = img->Width2;     /* without border, power of two */
   GLint height = img->Height2;   /* without border, power of two */
   GLint depth = img->Depth2;     /* without border, power of two */
   GLint rectarea;                /* = width * height */
   GLint i, j, k;
   GLubyte *texel;

   rectarea = imgWidth * imgHeight;

   /* Clamp/Repeat S and convert to integer texel coordinate */
   if (tObj->WrapS==GL_REPEAT) {
      /* s limited to [0,1) */
      /* i limited to [0,width-1] */
      i = (GLint) (s * width);
      if (s<0.0F)  i -= 1;
      i &= (width-1);
   }
   else if (tObj->WrapS==GL_CLAMP_TO_EDGE) {
      GLfloat min = 1.0F / (2.0F * width);
      GLfloat max = 1.0F - min;
      if (s < min)
         i = 0;
      else if (s > max)
         i = width-1;
      else
         i = (GLint) (s * width);
   }
   else {
      ASSERT(tObj->WrapS==GL_CLAMP);
      /* s limited to [0,1] */
      /* i limited to [0,width-1] */
      if (s<=0.0F)      i = 0;
      else if (s>=1.0F) i = width-1;
      else              i = (GLint) (s * width);
   }

   /* Clamp/Repeat T and convert to integer texel coordinate */
   if (tObj->WrapT==GL_REPEAT) {
      /* t limited to [0,1) */
      /* j limited to [0,height-1] */
      j = (GLint) (t * height);
      if (t<0.0F)  j -= 1;
      j &= (height-1);
   }
   else if (tObj->WrapT==GL_CLAMP_TO_EDGE) {
      GLfloat min = 1.0F / (2.0F * height);
      GLfloat max = 1.0F - min;
      if (t < min)
         j = 0;
      else if (t > max)
         j = height-1;
      else
         j = (GLint) (t * height);
   }
   else {
      ASSERT(tObj->WrapT==GL_CLAMP);
      /* t limited to [0,1] */
      /* j limited to [0,height-1] */
      if (t<=0.0F)      j = 0;
      else if (t>=1.0F) j = height-1;
      else              j = (GLint) (t * height);
   }

   /* Clamp/Repeat R and convert to integer texel coordinate */
   if (tObj->WrapR==GL_REPEAT) {
      /* r limited to [0,1) */
      /* k limited to [0,depth-1] */
      k = (GLint) (r * depth);
      if (r<0.0F)  k -= 1;
      k &= (depth-1);
   }
   else if (tObj->WrapR==GL_CLAMP_TO_EDGE) {
      GLfloat min = 1.0F / (2.0F * depth);
      GLfloat max = 1.0F - min;
      if (r < min)
         k = 0;
      else if (r > max)
         k = depth-1;
      else
         k = (GLint) (t * depth);
   }
   else {
      ASSERT(tObj->WrapR==GL_CLAMP);
      /* r limited to [0,1] */
      /* k limited to [0,depth-1] */
      if (r<=0.0F)      k = 0;
      else if (r>=1.0F) k = depth-1;
      else              k = (GLint) (r * depth);
   }

   switch (tObj->Image[0]->Format) {
      case GL_COLOR_INDEX:
         {
            GLubyte index = img->Data[ rectarea * k + j * imgWidth + i ];
            palette_sample(tObj, index, rgba );
            return;
         }
      case GL_ALPHA:
         rgba[ACOMP] = img->Data[ rectarea * k + j * imgWidth + i ];
         return;
      case GL_LUMINANCE:
      case GL_INTENSITY:
         rgba[RCOMP] = img->Data[ rectarea * k + j * imgWidth + i ];
         return;
      case GL_LUMINANCE_ALPHA:
         texel  = img->Data + ((rectarea * k + j * imgWidth + i) << 1);
         rgba[RCOMP] = texel[0];
         rgba[ACOMP] = texel[1];
         return;
      case GL_RGB:
         texel = img->Data + ( rectarea * k + j * imgWidth + i) * 3;
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         return;
      case GL_RGBA:
         texel = img->Data + ((rectarea * k + j * imgWidth + i) << 2);
         rgba[RCOMP] = texel[0];
         rgba[GCOMP] = texel[1];
         rgba[BCOMP] = texel[2];
         rgba[ACOMP] = texel[3];
         return;
      default:
         gl_problem(NULL, "Bad format in sample_3d_nearest");
   }
}


/*
 * Return the texture sample for coordinate (s,t,r) using GL_LINEAR filter.
 */
static void sample_3d_linear( const struct gl_texture_object *tObj,
                              const struct gl_texture_image *img,
                              GLfloat s, GLfloat t, GLfloat r,
                              GLubyte rgba[4] )
{
   GLint width = img->Width2;
   GLint height = img->Height2;
   GLint depth = img->Depth2;
   GLint i0, j0, k0, i1, j1, k1;
   GLint i0border, j0border, k0border, i1border, j1border, k1border;
   GLfloat u, v, w;

   u = s * width;
   if (tObj->WrapS==GL_REPEAT) {
      i0 = ((GLint) floor(u - 0.5F)) % width;
      i1 = (i0 + 1) & (width-1);
      i0border = i1border = 0;
   }
   else if (tObj->WrapS==GL_CLAMP_TO_EDGE) {
      GLfloat min = 1.0F / (2.0F * width);
      GLfloat max = 1.0F - min;
      if (s < min)
         i0 = 0;
      else if (s > max)
         i0 = width-1;
      else
         i0 = (GLint) (s * width);
      i1 = i0 + 1;
      if (i1 >= width)
         i1 = width - 1;
      i0border = i1border = 0;
   }
   else {
      ASSERT(tObj->WrapS==GL_CLAMP);
      if (u < 0.0F)  u = 0.0F;
      else if (u > width)  u = width;
      i0 = (GLint) floor(u - 0.5F);
      i1 = i0 + 1;
      i0border = (i0<0) | (i0>=width);
      i1border = (i1<0) | (i1>=width);
   }

   v = t * height;
   if (tObj->WrapT==GL_REPEAT) {
      j0 = ((GLint) floor(v - 0.5F)) % height;
      j1 = (j0 + 1) & (height-1);
      j0border = j1border = 0;
   }
   else if (tObj->WrapT==GL_CLAMP_TO_EDGE) {
      GLfloat min = 1.0F / (2.0F * height);
      GLfloat max = 1.0F - min;
      if (t < min)
         j0 = 0;
      else if (t > max)
         j0 = height-1;
      else
         j0 = (GLint) (t * height);
      j1 = j0 + 1;
      if (j1 >= height)
         j1 = height - 1;
      j0border = j1border = 0;
   }
   else {
      ASSERT(tObj->WrapT==GL_CLAMP);
      if (v < 0.0F)  v = 0.0F;
      else if (v > height)  v = height;
      j0 = (GLint) floor(v - 0.5F);
      j1 = j0 + 1;
      j0border = (j0<0) | (j0>=height);
      j1border = (j1<0) | (j1>=height);
   }

   w = r * depth;
   if (tObj->WrapR==GL_REPEAT) {
      k0 = ((GLint) floor(w - 0.5F)) % depth;
      k1 = (k0 + 1) & (depth-1);
      k0border = k1border = 0;
   }
   else if (tObj->WrapR==GL_CLAMP_TO_EDGE) {
      GLfloat min = 1.0F / (2.0F * depth);
      GLfloat max = 1.0F - min;
      if (r < min)
         k0 = 0;
      else if (r > max)
         k0 = depth-1;
      else
         k0 = (GLint) (r * depth);
      k1 = k0 + 1;
      if (k1 >= depth)
         k1 = depth - 1;
      k0border = k1border = 0;
   }
   else {
      ASSERT(tObj->WrapR==GL_CLAMP);
      if (r < 0.0F)  r = 0.0F;
      else if (r > depth)  r = depth;
      k0 = (GLint) floor(r - 0.5F);
      k1 = k0 + 1;
      k0border = (k0<0) | (k0>=depth);
      k1border = (k1<0) | (k1>=depth);
   }

   if (img->Border) {
      i0 += img->Border;
      i1 += img->Border;
      j0 += img->Border;
      j1 += img->Border;
      k0 += img->Border;
      k1 += img->Border;
      i0border = i1border = 0;
      j0border = j1border = 0;
      k0border = k1border = 0;
   }
   else {
      i0 &= (width-1);
      j0 &= (height-1);
      k0 &= (depth-1);
   }

   {
      GLfloat a = frac(u - 0.5F);
      GLfloat b = frac(v - 0.5F);
      GLfloat c = frac(w - 0.5F);

      GLint w000 = (GLint) ((1.0F-a)*(1.0F-b) * (1.0F-c) * 256.0F);
      GLint w010 = (GLint) (      a *(1.0F-b) * (1.0F-c) * 256.0F);
      GLint w001 = (GLint) ((1.0F-a)*      b  * (1.0F-c) * 256.0F);
      GLint w011 = (GLint) (      a *      b  * (1.0F-c) * 256.0F);
      GLint w100 = (GLint) ((1.0F-a)*(1.0F-b) * c * 256.0F);
      GLint w110 = (GLint) (      a *(1.0F-b) * c * 256.0F);
      GLint w101 = (GLint) ((1.0F-a)*      b  * c * 256.0F);
      GLint w111 = (GLint) (      a *      b  * c * 256.0F);


      GLubyte t000[4], t010[4], t001[4], t011[4];      /* texels */
      GLubyte t100[4], t110[4], t101[4], t111[4];

      if (k0border | i0border | j0border ) {
         t000[RCOMP] = tObj->BorderColor[0];
         t000[GCOMP] = tObj->BorderColor[1];
         t000[BCOMP] = tObj->BorderColor[2];
         t000[ACOMP] = tObj->BorderColor[3];
      }
      else {
         get_3d_texel( tObj, img, i0, j0, k0, t000 );
      }
      if (k0border | i1border | j0border) {
         t010[RCOMP] = tObj->BorderColor[0];
         t010[GCOMP] = tObj->BorderColor[1];
         t010[BCOMP] = tObj->BorderColor[2];
         t010[ACOMP] = tObj->BorderColor[3];
      }
      else {
         get_3d_texel( tObj, img, i1, j0, k0, t010 );
      }
      if (k0border | i0border | j1border) {
         t001[RCOMP] = tObj->BorderColor[0];
         t001[GCOMP] = tObj->BorderColor[1];
         t001[BCOMP] = tObj->BorderColor[2];
         t001[ACOMP] = tObj->BorderColor[3];
      }
      else {
         get_3d_texel( tObj, img, i0, j1, k0, t001 );
      }
      if (k0border | i1border | j1border) {
         t011[RCOMP] = tObj->BorderColor[0];
         t011[GCOMP] = tObj->BorderColor[1];
         t011[BCOMP] = tObj->BorderColor[2];
         t011[ACOMP] = tObj->BorderColor[3];
      }
      else {
         get_3d_texel( tObj, img, i1, j1, k0, t011 );
      }

      if (k1border | i0border | j0border ) {
         t100[RCOMP] = tObj->BorderColor[0];
         t100[GCOMP] = tObj->BorderColor[1];
         t100[BCOMP] = tObj->BorderColor[2];
         t100[ACOMP] = tObj->BorderColor[3];
      }
      else {
         get_3d_texel( tObj, img, i0, j0, k1, t100 );
      }
      if (k1border | i1border | j0border) {
         t110[RCOMP] = tObj->BorderColor[0];
         t110[GCOMP] = tObj->BorderColor[1];
         t110[BCOMP] = tObj->BorderColor[2];
         t110[ACOMP] = tObj->BorderColor[3];
      }
      else {
         get_3d_texel( tObj, img, i1, j0, k1, t110 );
      }
      if (k1border | i0border | j1border) {
         t101[RCOMP] = tObj->BorderColor[0];
         t101[GCOMP] = tObj->BorderColor[1];
         t101[BCOMP] = tObj->BorderColor[2];
         t101[ACOMP] = tObj->BorderColor[3];
      }
      else {
         get_3d_texel( tObj, img, i0, j1, k1, t101 );
      }
      if (k1border | i1border | j1border) {
         t111[RCOMP] = tObj->BorderColor[0];
         t111[GCOMP] = tObj->BorderColor[1];
         t111[BCOMP] = tObj->BorderColor[2];
         t111[ACOMP] = tObj->BorderColor[3];
      }
      else {
         get_3d_texel( tObj, img, i1, j1, k1, t111 );
      }

      /* MMX */
      rgba[0] = (w000*t000[0] + w010*t010[0] + w001*t001[0] + w011*t011[0] +
                 w100*t100[0] + w110*t110[0] + w101*t101[0] + w111*t111[0]  )
                >> 8;
      rgba[1] = (w000*t000[1] + w010*t010[1] + w001*t001[1] + w011*t011[1] +
                 w100*t100[1] + w110*t110[1] + w101*t101[1] + w111*t111[1] )
                >> 8;
      rgba[2] = (w000*t000[2] + w010*t010[2] + w001*t001[2] + w011*t011[2] +
                 w100*t100[2] + w110*t110[2] + w101*t101[2] + w111*t111[2] )
                >> 8;
      rgba[3] = (w000*t000[3] + w010*t010[3] + w001*t001[3] + w011*t011[3] +
                 w100*t100[3] + w110*t110[3] + w101*t101[3] + w111*t111[3] )
                >> 8;
   }
}


static void
sample_3d_nearest_mipmap_nearest( const struct gl_texture_object *tObj,
                                  GLfloat s, GLfloat t, GLfloat r,
                                  GLfloat lambda, GLubyte rgba[4] )
{
   GLint level;
   if (lambda <= 0.5F)
      lambda = 0.0F;
   else if (lambda > tObj->M + 0.4999F)
      lambda = tObj->M + 0.4999F;
   level = (GLint) (tObj->BaseLevel + lambda + 0.5F);
   if (level > tObj->P)
      level = tObj->P;

   sample_3d_nearest( tObj, tObj->Image[level], s, t, r, rgba );
}


static void
sample_3d_linear_mipmap_nearest( const struct gl_texture_object *tObj,
                                 GLfloat s, GLfloat t, GLfloat r,
                                 GLfloat lambda, GLubyte rgba[4] )
{
   GLint level;
   if (lambda <= 0.5F)
      lambda = 0.0F;
   else if (lambda > tObj->M + 0.4999F)
      lambda = tObj->M + 0.4999F;
   level = (GLint) (tObj->BaseLevel + lambda + 0.5F);
   if (level > tObj->P)
      level = tObj->P;

   sample_3d_linear( tObj, tObj->Image[level], s, t, r, rgba );
}


static void
sample_3d_nearest_mipmap_linear( const struct gl_texture_object *tObj,
                                 GLfloat s, GLfloat t, GLfloat r,
                                 GLfloat lambda, GLubyte rgba[4] )
{
   GLint level;
   if (lambda < 0.0F)
      lambda = 0.0F;
   else if (lambda > tObj->M)
      lambda = tObj->M;
   level = (GLint) (tObj->BaseLevel + lambda);

   if (level >= tObj->P) {
      sample_3d_nearest( tObj, tObj->Image[tObj->P], s, t, r, rgba );
   }
   else {
      GLubyte t0[4], t1[4];  /* texels */
      GLfloat f = frac(lambda);
      sample_3d_nearest( tObj, tObj->Image[level  ], s, t, r, t0 );
      sample_3d_nearest( tObj, tObj->Image[level+1], s, t, r, t1 );
      rgba[RCOMP] = (GLint) ((1.0F-f) * t0[RCOMP] + f * t1[RCOMP]);
      rgba[GCOMP] = (GLint) ((1.0F-f) * t0[GCOMP] + f * t1[GCOMP]);
      rgba[BCOMP] = (GLint) ((1.0F-f) * t0[BCOMP] + f * t1[BCOMP]);
      rgba[ACOMP] = (GLint) ((1.0F-f) * t0[ACOMP] + f * t1[ACOMP]);
   }
}


static void
sample_3d_linear_mipmap_linear( const struct gl_texture_object *tObj,
                                GLfloat s, GLfloat t, GLfloat r,
                                GLfloat lambda, GLubyte rgba[4] )
{
   GLint level;
   if (lambda < 0.0F)
      lambda = 0.0F;
   else if (lambda > tObj->M)
      lambda = tObj->M;
   level = (GLint) (tObj->BaseLevel + lambda);

   if (level >= tObj->P) {
      sample_3d_linear( tObj, tObj->Image[tObj->P], s, t, r, rgba );
   }
   else {
      GLubyte t0[4], t1[4];  /* texels */
      GLfloat f = frac(lambda);
      sample_3d_linear( tObj, tObj->Image[level  ], s, t, r, t0 );
      sample_3d_linear( tObj, tObj->Image[level+1], s, t, r, t1 );
      rgba[RCOMP] = (GLint) ((1.0F-f) * t0[RCOMP] + f * t1[RCOMP]);
      rgba[GCOMP] = (GLint) ((1.0F-f) * t0[GCOMP] + f * t1[GCOMP]);
      rgba[BCOMP] = (GLint) ((1.0F-f) * t0[BCOMP] + f * t1[BCOMP]);
      rgba[ACOMP] = (GLint) ((1.0F-f) * t0[ACOMP] + f * t1[ACOMP]);
   }
}


static void sample_nearest_3d( const struct gl_texture_object *tObj, GLuint n,
                               const GLfloat s[], const GLfloat t[],
                               const GLfloat u[], const GLfloat lambda[],
                               GLubyte rgba[][4] )
{
   GLuint i;
   struct gl_texture_image *image = tObj->Image[tObj->BaseLevel];
   (void) lambda;
   for (i=0;i<n;i++) {
      sample_3d_nearest( tObj, image, s[i], t[i], u[i], rgba[i] );
   }
}



static void sample_linear_3d( const struct gl_texture_object *tObj, GLuint n,
                              const GLfloat s[], const GLfloat t[],
                              const GLfloat u[], const GLfloat lambda[],
                              GLubyte rgba[][4] )
{
   GLuint i;
   struct gl_texture_image *image = tObj->Image[tObj->BaseLevel];
   (void) lambda;
   for (i=0;i<n;i++) {
      sample_3d_linear( tObj, image, s[i], t[i], u[i], rgba[i] );
   }
}


/*
 * Given an (s,t,r) texture coordinate and lambda (level of detail) value,
 * return a texture sample.
 */
static void sample_lambda_3d( const struct gl_texture_object *tObj, GLuint n,
                              const GLfloat s[], const GLfloat t[],
                              const GLfloat u[], const GLfloat lambda[],
                              GLubyte rgba[][4] )
{
   GLuint i;

   for (i=0;i<n;i++) {

      if (lambda[i] > tObj->MinMagThresh) {
      /* minification */
         switch (tObj->MinFilter) {
         case GL_NEAREST:
               sample_3d_nearest( tObj, tObj->Image[tObj->BaseLevel], s[i], t[i], u[i], rgba[i] );
            break;
         case GL_LINEAR:
               sample_3d_linear( tObj, tObj->Image[tObj->BaseLevel], s[i], t[i], u[i], rgba[i] );
            break;
         case GL_NEAREST_MIPMAP_NEAREST:
               sample_3d_nearest_mipmap_nearest( tObj, s[i], t[i], u[i], lambda[i], rgba[i] );
            break;
         case GL_LINEAR_MIPMAP_NEAREST:
               sample_3d_linear_mipmap_nearest( tObj, s[i], t[i], u[i], lambda[i], rgba[i] );
            break;
         case GL_NEAREST_MIPMAP_LINEAR:
               sample_3d_nearest_mipmap_linear( tObj, s[i], t[i], u[i], lambda[i], rgba[i] );
            break;
         case GL_LINEAR_MIPMAP_LINEAR:
               sample_3d_linear_mipmap_linear( tObj, s[i], t[i], u[i], lambda[i], rgba[i] );
            break;
         default:
               gl_problem(NULL, "Bad min filterin sample_3d_texture");
      }
   }
   else {
      /* magnification */
         switch (tObj->MagFilter) {
         case GL_NEAREST:
               sample_3d_nearest( tObj, tObj->Image[tObj->BaseLevel], s[i], t[i], u[i], rgba[i] );
            break;
         case GL_LINEAR:
               sample_3d_linear( tObj, tObj->Image[tObj->BaseLevel], s[i], t[i], u[i], rgba[i] );
               break;
            default:
               gl_problem(NULL, "Bad mag filter in sample_3d_texture");
         }
      }
   }
}



/**********************************************************************/
/*                       Texture Sampling Setup                       */
/**********************************************************************/


/*
 * Setup the texture sampling function for this texture object.
 */
void gl_set_texture_sampler( struct gl_texture_object *t )
{
   if (!t->Complete) {
      t->SampleFunc = NULL;
   }
   else {
      GLboolean needLambda = (t->MinFilter != t->MagFilter);

      if (needLambda) {
         /* Compute min/mag filter threshold */
         if (t->MagFilter==GL_LINEAR
             && (t->MinFilter==GL_NEAREST_MIPMAP_NEAREST ||
                 t->MinFilter==GL_LINEAR_MIPMAP_NEAREST)) {
            t->MinMagThresh = 0.5F;
         }
         else {
            t->MinMagThresh = 0.0F;
         }
      }

      switch (t->Dimensions) {
         case 1:
            if (needLambda) {
               t->SampleFunc = sample_lambda_1d;
            }
            else if (t->MinFilter==GL_LINEAR) {
               t->SampleFunc = sample_linear_1d;
            }
            else {
               ASSERT(t->MinFilter==GL_NEAREST);
               t->SampleFunc = sample_nearest_1d;
            }
            break;
         case 2:
            if (needLambda) {
               t->SampleFunc = sample_lambda_2d;
            }
            else if (t->MinFilter==GL_LINEAR) {
               t->SampleFunc = sample_linear_2d;
            }
            else {
               ASSERT(t->MinFilter==GL_NEAREST);
               if (t->WrapS==GL_REPEAT && t->WrapT==GL_REPEAT
                   && t->Image[0]->Border==0 && t->Image[0]->Format==GL_RGB) {
                  t->SampleFunc = opt_sample_rgb_2d;
               }
               else if (t->WrapS==GL_REPEAT && t->WrapT==GL_REPEAT
                   && t->Image[0]->Border==0 && t->Image[0]->Format==GL_RGBA) {
                  t->SampleFunc = opt_sample_rgba_2d;
               }
               else
                  t->SampleFunc = sample_nearest_2d;
            }
            break;
         case 3:
            if (needLambda) {
               t->SampleFunc = sample_lambda_3d;
            }
            else if (t->MinFilter==GL_LINEAR) {
               t->SampleFunc = sample_linear_3d;
            }
            else {
               ASSERT(t->MinFilter==GL_NEAREST);
               t->SampleFunc = sample_nearest_3d;
            }
            break;
         default:
            gl_problem(NULL, "invalid dimensions in gl_set_texture_sampler");
      }
   }
}



/**********************************************************************/
/*                      Texture Application                           */
/**********************************************************************/


/*
 * Combine incoming fragment color with texel color to produce output color.
 * Input:  textureSet - pointer to texture set/stage to apply
 *         format - base internal texture format
 *         n - number of fragments
 *         texels - array of texel colors
 * InOut:  rgba - incoming fragment colors modified by texel colors
 *                according to the texture environment mode.
 */
static void apply_texture( const GLcontext *ctx,
                           const struct gl_texture_set *texSet,
                           GLuint n,
                           GLubyte rgba[][4], CONST GLubyte texel[][4] )
{
   GLuint i;
   GLint Rc, Gc, Bc, Ac;
   GLenum format;

   ASSERT(texSet);
   ASSERT(texSet->Current);
   ASSERT(texSet->Current->Image[0]);

   format = texSet->Current->Image[0]->Format;

/*
 * Use (A*(B+1)) >> 8 as a fast approximation of (A*B)/255 for A
 * and B in [0,255]
 */
#define PROD(A,B)   (((GLint)(A) * ((GLint)(B)+1)) >> 8)

   if (format==GL_COLOR_INDEX) {
      format = GL_RGBA;  /* XXXX a hack! */
   }

   switch (texSet->EnvMode) {
      case GL_REPLACE:
	 switch (format) {
	    case GL_ALPHA:
	       for (i=0;i<n;i++) {
		  /* Cv = Cf */
                  /* Av = At */
                  rgba[i][ACOMP] = texel[i][ACOMP];
	       }
	       break;
	    case GL_LUMINANCE:
	       for (i=0;i<n;i++) {
		  /* Cv = Lt */
                  GLint Lt = texel[i][RCOMP];
                  rgba[i][RCOMP] = rgba[i][GCOMP] = rgba[i][BCOMP] = Lt;
                  /* Av = Af */
	       }
	       break;
	    case GL_LUMINANCE_ALPHA:
	       for (i=0;i<n;i++) {
                  GLint Lt = texel[i][RCOMP];
		  /* Cv = Lt */
		  rgba[i][RCOMP] = rgba[i][GCOMP] = rgba[i][BCOMP] = Lt;
		  /* Av = At */
		  rgba[i][ACOMP] = texel[i][ACOMP];
	       }
	       break;
	    case GL_INTENSITY:
	       for (i=0;i<n;i++) {
		  /* Cv = It */
                  GLint It = texel[i][RCOMP];
                  rgba[i][RCOMP] = rgba[i][GCOMP] = rgba[i][BCOMP] = It;
                  /* Av = It */
                  rgba[i][ACOMP] = It;
	       }
	       break;
	    case GL_RGB:
	       for (i=0;i<n;i++) {
		  /* Cv = Ct */
		  rgba[i][RCOMP] = texel[i][RCOMP];
		  rgba[i][GCOMP] = texel[i][GCOMP];
		  rgba[i][BCOMP] = texel[i][BCOMP];
		  /* Av = Af */
	       }
	       break;
	    case GL_RGBA:
	       for (i=0;i<n;i++) {
		  /* Cv = Ct */
		  rgba[i][RCOMP] = texel[i][RCOMP];
		  rgba[i][GCOMP] = texel[i][GCOMP];
		  rgba[i][BCOMP] = texel[i][BCOMP];
		  /* Av = At */
		  rgba[i][ACOMP] = texel[i][ACOMP];
	       }
	       break;
            default:
               gl_problem(ctx, "Bad format in apply_texture");
               return;
	 }
	 break;

      case GL_MODULATE:
         switch (format) {
	    case GL_ALPHA:
	       for (i=0;i<n;i++) {
		  /* Cv = Cf */
		  /* Av = AfAt */
		  rgba[i][ACOMP] = PROD( rgba[i][ACOMP], texel[i][ACOMP] );
	       }
	       break;
	    case GL_LUMINANCE:
	       for (i=0;i<n;i++) {
		  /* Cv = LtCf */
                  GLint Lt = texel[i][RCOMP];
		  rgba[i][RCOMP] = PROD( rgba[i][RCOMP], Lt );
		  rgba[i][GCOMP] = PROD( rgba[i][GCOMP], Lt );
		  rgba[i][BCOMP] = PROD( rgba[i][BCOMP], Lt );
		  /* Av = Af */
	       }
	       break;
	    case GL_LUMINANCE_ALPHA:
	       for (i=0;i<n;i++) {
		  /* Cv = CfLt */
                  GLint Lt = texel[i][RCOMP];
		  rgba[i][RCOMP] = PROD( rgba[i][RCOMP], Lt );
		  rgba[i][GCOMP] = PROD( rgba[i][GCOMP], Lt );
		  rgba[i][BCOMP] = PROD( rgba[i][BCOMP], Lt );
		  /* Av = AfAt */
		  rgba[i][ACOMP] = PROD( rgba[i][ACOMP], texel[i][ACOMP] );
	       }
	       break;
	    case GL_INTENSITY:
	       for (i=0;i<n;i++) {
		  /* Cv = CfIt */
                  GLint It = texel[i][RCOMP];
		  rgba[i][RCOMP] = PROD( rgba[i][RCOMP], It );
		  rgba[i][GCOMP] = PROD( rgba[i][GCOMP], It );
		  rgba[i][BCOMP] = PROD( rgba[i][BCOMP], It );
		  /* Av = AfIt */
		  rgba[i][ACOMP] = PROD( rgba[i][ACOMP], It );
	       }
	       break;
	    case GL_RGB:
	       for (i=0;i<n;i++) {
		  /* Cv = CfCt */
		  rgba[i][RCOMP] = PROD( rgba[i][RCOMP], texel[i][RCOMP] );
		  rgba[i][GCOMP] = PROD( rgba[i][GCOMP], texel[i][GCOMP] );
		  rgba[i][BCOMP] = PROD( rgba[i][BCOMP], texel[i][BCOMP] );
		  /* Av = Af */
	       }
	       break;
	    case GL_RGBA:
	       for (i=0;i<n;i++) {
		  /* Cv = CfCt */
		  rgba[i][RCOMP] = PROD( rgba[i][RCOMP], texel[i][RCOMP] );
		  rgba[i][GCOMP] = PROD( rgba[i][GCOMP], texel[i][GCOMP] );
		  rgba[i][BCOMP] = PROD( rgba[i][BCOMP], texel[i][BCOMP] );
		  /* Av = AfAt */
		  rgba[i][ACOMP] = PROD( rgba[i][ACOMP], texel[i][ACOMP] );
	       }
	       break;
            default:
               gl_problem(ctx, "Bad format (2) in apply_texture");
               return;
	 }
	 break;

      case GL_DECAL:
         switch (format) {
            case GL_ALPHA:
            case GL_LUMINANCE:
            case GL_LUMINANCE_ALPHA:
            case GL_INTENSITY:
               /* undefined */
               break;
	    case GL_RGB:
	       for (i=0;i<n;i++) {
		  /* Cv = Ct */
		  rgba[i][RCOMP] = texel[i][RCOMP];
		  rgba[i][GCOMP] = texel[i][GCOMP];
		  rgba[i][BCOMP] = texel[i][BCOMP];
		  /* Av = Af */
	       }
	       break;
	    case GL_RGBA:
	       for (i=0;i<n;i++) {
		  /* Cv = Cf(1-At) + CtAt */
		  GLint t = texel[i][ACOMP], s = 255 - t;
		  rgba[i][RCOMP] = PROD(rgba[i][RCOMP], s) + PROD(texel[i][RCOMP],t);
		  rgba[i][GCOMP] = PROD(rgba[i][GCOMP], s) + PROD(texel[i][GCOMP],t);
		  rgba[i][BCOMP] = PROD(rgba[i][BCOMP], s) + PROD(texel[i][BCOMP],t);
		  /* Av = Af */
	       }
	       break;
            default:
               gl_problem(ctx, "Bad format (3) in apply_texture");
               return;
	 }
	 break;

      case GL_BLEND:
         Rc = (GLint) (texSet->EnvColor[0] * 255.0F);
         Gc = (GLint) (texSet->EnvColor[1] * 255.0F);
         Bc = (GLint) (texSet->EnvColor[2] * 255.0F);
         Ac = (GLint) (texSet->EnvColor[3] * 255.0F);
	 switch (format) {
	    case GL_ALPHA:
	       for (i=0;i<n;i++) {
		  /* Cv = Cf */
		  /* Av = AfAt */
                  rgba[i][ACOMP] = PROD(rgba[i][ACOMP], texel[i][ACOMP]);
	       }
	       break;
            case GL_LUMINANCE:
	       for (i=0;i<n;i++) {
		  /* Cv = Cf(1-Lt) + CcLt */
		  GLint Lt = texel[i][RCOMP], s = 255 - Lt;
		  rgba[i][RCOMP] = PROD(rgba[i][RCOMP], s) + PROD(Rc, Lt);
		  rgba[i][GCOMP] = PROD(rgba[i][GCOMP], s) + PROD(Gc, Lt);
		  rgba[i][BCOMP] = PROD(rgba[i][BCOMP], s) + PROD(Bc, Lt);
		  /* Av = Af */
	       }
	       break;
	    case GL_LUMINANCE_ALPHA:
	       for (i=0;i<n;i++) {
		  /* Cv = Cf(1-Lt) + CcLt */
		  GLint Lt = texel[i][RCOMP], s = 255 - Lt;
		  rgba[i][RCOMP] = PROD(rgba[i][RCOMP], s) + PROD(Rc, Lt);
		  rgba[i][GCOMP] = PROD(rgba[i][GCOMP], s) + PROD(Gc, Lt);
		  rgba[i][BCOMP] = PROD(rgba[i][BCOMP], s) + PROD(Bc, Lt);
		  /* Av = AfAt */
		  rgba[i][ACOMP] = PROD(rgba[i][ACOMP],texel[i][ACOMP]);
	       }
	       break;
            case GL_INTENSITY:
	       for (i=0;i<n;i++) {
		  /* Cv = Cf(1-It) + CcLt */
		  GLint It = texel[i][RCOMP], s = 255 - It;
		  rgba[i][RCOMP] = PROD(rgba[i][RCOMP], s) + PROD(Rc, It);
		  rgba[i][GCOMP] = PROD(rgba[i][GCOMP], s) + PROD(Gc, It);
		  rgba[i][BCOMP] = PROD(rgba[i][BCOMP], s) + PROD(Bc, It);
                  /* Av = Af(1-It) + Ac*It */
                  rgba[i][ACOMP] = PROD(rgba[i][ACOMP], s) + PROD(Ac, It);
               }
               break;
	    case GL_RGB:
	       for (i=0;i<n;i++) {
		  /* Cv = Cf(1-Ct) + CcCt */
		  rgba[i][RCOMP] = PROD(rgba[i][RCOMP], (255-texel[i][RCOMP])) + PROD(Rc,texel[i][RCOMP]);
		  rgba[i][GCOMP] = PROD(rgba[i][GCOMP], (255-texel[i][GCOMP])) + PROD(Gc,texel[i][GCOMP]);
		  rgba[i][BCOMP] = PROD(rgba[i][BCOMP], (255-texel[i][BCOMP])) + PROD(Bc,texel[i][BCOMP]);
		  /* Av = Af */
	       }
	       break;
	    case GL_RGBA:
	       for (i=0;i<n;i++) {
		  /* Cv = Cf(1-Ct) + CcCt */
		  rgba[i][RCOMP] = PROD(rgba[i][RCOMP], (255-texel[i][RCOMP])) + PROD(Rc,texel[i][RCOMP]);
		  rgba[i][GCOMP] = PROD(rgba[i][GCOMP], (255-texel[i][GCOMP])) + PROD(Gc,texel[i][GCOMP]);
		  rgba[i][BCOMP] = PROD(rgba[i][BCOMP], (255-texel[i][BCOMP])) + PROD(Bc,texel[i][BCOMP]);
		  /* Av = AfAt */
		  rgba[i][ACOMP] = PROD(rgba[i][ACOMP],texel[i][ACOMP]);
	       }
	       break;
            default:
               gl_problem(ctx, "Bad format (4) in apply_texture");
               return;
	 }
	 break;

      default:
         gl_problem(ctx, "Bad env mode in apply_texture");
         return;
   }
#undef PROD
}



/*
 * Apply a stage/set of texture mapping to the incoming fragments.
 */
void gl_texture_pixels( GLcontext *ctx, GLuint texSet, GLuint n,
                        const GLfloat s[], const GLfloat t[],
                        const GLfloat r[], GLfloat lambda[],
                        GLubyte rgba[][4] )
{
   GLuint mask = (TEXTURE0_1D | TEXTURE0_2D | TEXTURE0_3D) << (texSet * 4);
   if (ctx->Texture.Enabled & mask) {
      const struct gl_texture_set *textureSet = &ctx->Texture.Set[texSet];
      if (textureSet->Current && textureSet->Current->SampleFunc) {

         GLubyte texel[PB_SIZE][4];

         if (textureSet->Current->MinLod != -1000.0
             || textureSet->Current->MaxLod != 1000.0) {
            /* apply LOD clamping to lambda */
            GLfloat min = textureSet->Current->MinLod;
            GLfloat max = textureSet->Current->MaxLod;
            GLuint i;
            for (i=0;i<n;i++) {
               GLfloat l = lambda[i];
               lambda[i] = CLAMP(l, min, max);
            }
         }

         /* Sample the texture. */
         (*textureSet->Current->SampleFunc)( textureSet->Current, n,
                                             s, t, r, lambda, texel );

         apply_texture( ctx, textureSet, n, rgba, texel );
      }
   }
}
