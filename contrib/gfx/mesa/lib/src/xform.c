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
 * Revision 1.1  2005/01/11 14:58:33  NicJA
 * AROSMesa 3.0
 *
 * - Based on the official mesa 3 code with major patches to the amigamesa driver code to get it working.
 * - GLUT not yet started (ive left the _old_ mesaaux, mesatk and demos in for this reason)
 * - Doesnt yet work - the _db functions seem to be writing the data incorrectly, and color picking also seems broken somewhat - giving most things a blue tinge (those that are currently working)
 *
 * Revision 3.3  1998/08/20 04:15:48  brianp
 * added prototype 3.1 transformation functions
 *
 * Revision 3.2  1998/04/18 05:00:56  brianp
 * renamed USE_ASM to USE_X86_ASM
 *
 * Revision 3.1  1998/02/01 16:37:19  brianp
 * added GL_EXT_rescale_normal extension
 *
 * Revision 3.0  1998/01/31 21:08:31  brianp
 * initial rev
 *
 */


/*
 * Matrix/vertex/vector transformation stuff
 *
 *
 * NOTES:
 * 1. 4x4 transformation matrices are stored in memory in column major order.
 * 2. Points/vertices are to be thought of as column vectors.
 * 3. Transformation of a point p by a matrix M is: p' = M * p
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <math.h>
#include "context.h"
#include "mmath.h"
#include "types.h"
#include "xform.h"
#endif



/*
 * Apply a transformation matrix to an array of [X Y Z W] coordinates:
 *   for i in 0 to n-1 do   q[i] = m * p[i]
 * where p[i] and q[i] are 4-element column vectors and m is a 16-element
 * transformation matrix.
 */
void gl_xform_points_4fv( GLuint n, GLfloat q[][4], const GLfloat m[16],
                          GLfloat p[][4] )
{
   /* This function has been carefully crafted to maximize register usage
    * and use loop unrolling with IRIX 5.3's cc.  Hopefully other compilers
    * will like this code too.
    */
   {
      GLuint i;
      GLfloat m0 = m[0],  m4 = m[4],  m8 = m[8],  m12 = m[12];
      GLfloat m1 = m[1],  m5 = m[5],  m9 = m[9],  m13 = m[13];
      if (m12==0.0F && m13==0.0F) {
         /* common case */
         for (i=0;i<n;i++) {
            GLfloat p0 = p[i][0], p1 = p[i][1], p2 = p[i][2];
            q[i][0] = m0 * p0 + m4  * p1 + m8 * p2;
            q[i][1] = m1 * p0 + m5  * p1 + m9 * p2;
         }
      }
      else {
         /* general case */
         for (i=0;i<n;i++) {
            GLfloat p0 = p[i][0], p1 = p[i][1], p2 = p[i][2], p3 = p[i][3];
            q[i][0] = m0 * p0 + m4  * p1 + m8 * p2 + m12 * p3;
            q[i][1] = m1 * p0 + m5  * p1 + m9 * p2 + m13 * p3;
         }
      }
   }
   {
      GLuint i;
      GLfloat m2 = m[2],  m6 = m[6],  m10 = m[10],  m14 = m[14];
      GLfloat m3 = m[3],  m7 = m[7],  m11 = m[11],  m15 = m[15];
      if (m3==0.0F && m7==0.0F && m11==0.0F && m15==1.0F) {
         /* common case */
         for (i=0;i<n;i++) {
            GLfloat p0 = p[i][0], p1 = p[i][1], p2 = p[i][2], p3 = p[i][3];
            q[i][2] = m2 * p0 + m6 * p1 + m10 * p2 + m14 * p3;
            q[i][3] = p3;
         }
      }
      else {
         /* general case */
         for (i=0;i<n;i++) {
            GLfloat p0 = p[i][0], p1 = p[i][1], p2 = p[i][2], p3 = p[i][3];
            q[i][2] = m2 * p0 + m6 * p1 + m10 * p2 + m14 * p3;
            q[i][3] = m3 * p0 + m7 * p1 + m11 * p2 + m15 * p3;
         }
      }
   }
}



/*
 * Apply a transformation matrix to an array of [X Y Z] coordinates:
 *   for i in 0 to n-1 do   q[i] = m * p[i]
 */
void gl_xform_points_3fv( GLuint n, GLfloat q[][4], const GLfloat m[16],
                          GLfloat p[][3] )
{
   /* This function has been carefully crafted to maximize register usage
    * and use loop unrolling with IRIX 5.3's cc.  Hopefully other compilers
    * will like this code too.
    */
   {
      GLuint i;
      GLfloat m0 = m[0],  m4 = m[4],  m8 = m[8],  m12 = m[12];
      GLfloat m1 = m[1],  m5 = m[5],  m9 = m[9],  m13 = m[13];
      for (i=0;i<n;i++) {
         GLfloat p0 = p[i][0], p1 = p[i][1], p2 = p[i][2];
         q[i][0] = m0 * p0 + m4  * p1 + m8 * p2 + m12;
         q[i][1] = m1 * p0 + m5  * p1 + m9 * p2 + m13;
      }
   }
   {
      GLuint i;
      GLfloat m2 = m[2],  m6 = m[6],  m10 = m[10],  m14 = m[14];
      GLfloat m3 = m[3],  m7 = m[7],  m11 = m[11],  m15 = m[15];
      if (m3==0.0F && m7==0.0F && m11==0.0F && m15==1.0F) {
         /* common case */
         for (i=0;i<n;i++) {
            GLfloat p0 = p[i][0], p1 = p[i][1], p2 = p[i][2];
            q[i][2] = m2 * p0 + m6 * p1 + m10 * p2 + m14;
            q[i][3] = 1.0F;
         }
      }
      else {
         /* general case */
         for (i=0;i<n;i++) {
            GLfloat p0 = p[i][0], p1 = p[i][1], p2 = p[i][2];
            q[i][2] = m2 * p0 + m6 * p1 + m10 * p2 + m14;
            q[i][3] = m3 * p0 + m7 * p1 + m11 * p2 + m15;
         }
      }
   }
}



#ifndef USE_X86_ASM
/*
 * Apply a transformation matrix to an array of normal vectors:
 *   for i in 0 to n-1 do  v[i] = u[i] * m
 * where u[i] and v[i] are 3-element row vectors and m is a 16-element
 * transformation matrix.
 * If the normalize flag is true the normals will be scaled to length 1.
 * If the rescale flag is true then do normal rescaling.
 */
void gl_xform_normals_3fv( GLuint n, GLfloat v[][3], const GLfloat m[16],
                           GLfloat u[][3], GLboolean normalize,
                           GLboolean rescale )
{
   if (normalize) {
      if (rescale) {
         /* Transform normals, rescale and normalize */
         GLuint i;
         GLfloat m0 = m[0],  m4 = m[4],  m8 = m[8];
         GLfloat m1 = m[1],  m5 = m[5],  m9 = m[9];
         GLfloat m2 = m[2],  m6 = m[6],  m10 = m[10];
         GLfloat f = GL_SQRT( m2*m2 + m6*m6 + m10*m10 );
         f = (f == 0.0F) ? 1.0F : (1.0F / f);
         for (i=0;i<n;i++) {
            GLdouble tx, ty, tz;
            {
               GLfloat ux = u[i][0],  uy = u[i][1],  uz = u[i][2];
               tx = f * (ux * m0 + uy * m1 + uz * m2);
               ty = f * (ux * m4 + uy * m5 + uz * m6);
               tz = f * (ux * m8 + uy * m9 + uz * m10);
            }
            {
               GLdouble len, scale;
               len = GL_SQRT( tx*tx + ty*ty + tz*tz );
               scale = (len>1E-30) ? (1.0 / len) : 1.0;
               v[i][0] = tx * scale;
               v[i][1] = ty * scale;
               v[i][2] = tz * scale;
            }
         }
      }
      else {
         /* Transform and normalize */
      GLuint i;
      GLfloat m0 = m[0],  m4 = m[4],  m8 = m[8];
      GLfloat m1 = m[1],  m5 = m[5],  m9 = m[9];
      GLfloat m2 = m[2],  m6 = m[6],  m10 = m[10];
      for (i=0;i<n;i++) {
         GLdouble tx, ty, tz;
         {
            GLfloat ux = u[i][0],  uy = u[i][1],  uz = u[i][2];
            tx = ux * m0 + uy * m1 + uz * m2;
            ty = ux * m4 + uy * m5 + uz * m6;
            tz = ux * m8 + uy * m9 + uz * m10;
         }
         {
            GLdouble len, scale;
            len = GL_SQRT( tx*tx + ty*ty + tz*tz );
            scale = (len>1E-30) ? (1.0 / len) : 1.0;
            v[i][0] = tx * scale;
            v[i][1] = ty * scale;
            v[i][2] = tz * scale;
         }
      }
   }
   }
   else {
      if (rescale) {
         /* Transform and rescale */
         GLuint i;
         GLfloat m0 = m[0],  m4 = m[4],  m8 = m[8];
         GLfloat m1 = m[1],  m5 = m[5],  m9 = m[9];
         GLfloat m2 = m[2],  m6 = m[6],  m10 = m[10];
         GLfloat f = GL_SQRT( m2*m2 + m6*m6 + m10*m10 );
         f = (f == 0.0F) ? 1.0F : (1.0F / f);
         for (i=0;i<n;i++) {
            GLfloat ux = u[i][0],  uy = u[i][1],  uz = u[i][2];
            v[i][0] = f * (ux * m0 + uy * m1 + uz * m2);
            v[i][1] = f * (ux * m4 + uy * m5 + uz * m6);
            v[i][2] = f * (ux * m8 + uy * m9 + uz * m10);
         }
      }
   else {
         /* Just transform */
      GLuint i;
      GLfloat m0 = m[0],  m4 = m[4],  m8 = m[8];
      GLfloat m1 = m[1],  m5 = m[5],  m9 = m[9];
      GLfloat m2 = m[2],  m6 = m[6],  m10 = m[10];
      for (i=0;i<n;i++) {
         GLfloat ux = u[i][0],  uy = u[i][1],  uz = u[i][2];
         v[i][0] = ux * m0 + uy * m1 + uz * m2;
         v[i][1] = ux * m4 + uy * m5 + uz * m6;
         v[i][2] = ux * m8 + uy * m9 + uz * m10;
      }
   }
   }
}
#endif


/*
 * Transform a 4-element row vector (1x4 matrix) by a 4x4 matrix.  This
 * function is used for transforming clipping plane equations and spotlight
 * directions.
 * Mathematically,  u = v * m.
 * Input:  v - input vector
 *         m - transformation matrix
 * Output:  u - transformed vector
 */
void gl_transform_vector( GLfloat u[4], const GLfloat v[4], const GLfloat m[16] )
{
   GLfloat v0=v[0], v1=v[1], v2=v[2], v3=v[3];
#define M(row,col)  m[col*4+row]
   u[0] = v0 * M(0,0) + v1 * M(1,0) + v2 * M(2,0) + v3 * M(3,0);
   u[1] = v0 * M(0,1) + v1 * M(1,1) + v2 * M(2,1) + v3 * M(3,1);
   u[2] = v0 * M(0,2) + v1 * M(1,2) + v2 * M(2,2) + v3 * M(3,2);
   u[3] = v0 * M(0,3) + v1 * M(1,3) + v2 * M(2,3) + v3 * M(3,3);
#undef M
}



/**********************************************************************
 *
 * Mesa 3.1 prototype transformation code
 *
 * Objectives:
 *     Move all functions which may be asm-accelerated out of vbxform.c
 *       into this file.
 *
 *     Add support for arbitrary strides in source coordinates to better
 *       support vertex arrays.
 *
 **********************************************************************/



/*
 * Use the current modelview matrix to transform XYZ vertices from object
 * to eye coordinates.
 * Input:  ctx - the context
 *         n - number of vertices to transform
 *         stride - stride in bytes between subsequent vObj vertices
 *         vObj - pointer to first vertex (in object coordinates)
 * Output;  vEye - array [n][4] of eye coordinates
 */
void
gl_transform_points3( const GLcontext *ctx, GLuint n, GLuint stride,
                      const GLfloat *vObj, GLfloat vEye[][4] )
{
   ASSERT((stride & 0x3) == 0);  /* multiple of 4 bytes */

   switch (ctx->ModelViewMatrixType) {
      case MATRIX_GENERAL:
         {
            const GLfloat *m = ctx->ModelViewMatrix;
            const GLfloat m0 = m[0],  m4 = m[4],  m8 = m[8],  m12 = m[12];
            const GLfloat m1 = m[1],  m5 = m[5],  m9 = m[9],  m13 = m[13];
            const GLfloat m2 = m[2],  m6 = m[6],  m10 = m[10],  m14 = m[14];
            const GLfloat m3 = m[3],  m7 = m[7],  m11 = m[11],  m15 = m[15];
            GLuint i;
            for (i=0;i<n;i++) {
               const GLfloat ox = vObj[0], oy = vObj[1], oz = vObj[2];
               vEye[i][0] = m0 * ox + m4 * oy + m8  * oz + m12;
               vEye[i][1] = m1 * ox + m5 * oy + m9  * oz + m13;
               vEye[i][2] = m2 * ox + m6 * oy + m10 * oz + m14;
               vEye[i][3] = m3 * ox + m7 * oy + m11 * oz + m15;
               vObj = (const GLfloat *) ((GLubyte *) vObj + stride);
            }
         }
         break;
      case MATRIX_IDENTITY:
         {
            GLuint i;
            for (i=0;i<n;i++) {
               vEye[i][0] = vObj[0];
               vEye[i][1] = vObj[1];
               vEye[i][2] = vObj[2];
               vEye[i][3] = 1.0F;
               vObj = (const GLfloat *) ((GLubyte *) vObj + stride);
            }
         }
         break;
      case MATRIX_2D:
         {
            const GLfloat *m = ctx->ModelViewMatrix;
            const GLfloat m0 = m[0], m1 = m[1], m4 = m[4], m5 = m[5];
            const GLfloat m12 = m[12], m13 = m[13];
            GLuint i;
            for (i=0;i<n;i++) {
               const GLfloat ox = vObj[0], oy = vObj[1], oz = vObj[2];
               vEye[i][0] = m0 * ox + m4 * oy            + m12       ;
               vEye[i][1] = m1 * ox + m5 * oy            + m13       ;
               vEye[i][2] =                   +       oz             ;
               vEye[i][3] =                                      1.0F;
               vObj = (const GLfloat *) ((GLubyte *) vObj + stride);
            }
         }
         break;
      case MATRIX_2D_NO_ROT:
         {
            const GLfloat *m = ctx->ModelViewMatrix;
            const GLfloat m0 = m[0], m5 = m[5], m12 = m[12], m13 = m[13];
            GLuint i;
            for (i=0;i<n;i++) {
               const GLfloat ox = vObj[0], oy = vObj[1], oz = vObj[2];
               vEye[i][0] = m0 * ox                      + m12       ;
               vEye[i][1] =           m5 * oy            + m13       ;
               vEye[i][2] =                   +       oz             ;
               vEye[i][3] =                                      1.0F;
               vObj = (const GLfloat *) ((GLubyte *) vObj + stride);
            }
         }
         break;
      case MATRIX_3D:
         {
            const GLfloat *m = ctx->ModelViewMatrix;
            const GLfloat m0 = m[0], m1 = m[1], m2 = m[2], m4 = m[4];
            const GLfloat m5 = m[5], m6 = m[6], m8 = m[8], m9 = m[9];
            const GLfloat m10 = m[10], m12 = m[12], m13 = m[13], m14 = m[14];
            GLuint i;
            for (i=0;i<n;i++) {
               const GLfloat ox = vObj[0], oy = vObj[1], oz = vObj[2];
               vEye[i][0] = m0 * ox + m4 * oy +  m8 * oz + m12       ;
               vEye[i][1] = m1 * ox + m5 * oy +  m9 * oz + m13       ;
               vEye[i][2] = m2 * ox + m6 * oy + m10 * oz + m14       ;
               vEye[i][3] =                                      1.0F;
               vObj = (const GLfloat *) ((GLubyte *) vObj + stride);
            }
         }
         break;
      default:
         /* should never get here */
         gl_problem( NULL, "invalid matrix type in gl_transform_points3()" );
   }
}


/*
 * Use the current modelview matrix to transform XYZW vertices from object
 * to eye coordinates.
 * Input:  ctx - the context
 *         n - number of vertices to transform
 *         stride - stride in bytes between subsequent vObj vertices
 *         vObj - pointer to first vertex (in object coordinates)
 * Output;  vEye - array [n][4] of eye coordinates
 */
void
gl_transform_points4( const GLcontext *ctx, GLuint n, GLuint stride,
                      const GLfloat *vObj, GLfloat vEye[][4] )
{
   ASSERT((stride & 0x3) == 0);  /* multiple of 4 bytes */

   switch (ctx->ModelViewMatrixType) {
      case MATRIX_GENERAL:
         {
            const GLfloat *m = ctx->ModelViewMatrix;
            const GLfloat m0 = m[0],  m4 = m[4],  m8 = m[8],  m12 = m[12];
            const GLfloat m1 = m[1],  m5 = m[5],  m9 = m[9],  m13 = m[13];
            const GLfloat m2 = m[2],  m6 = m[6],  m10 = m[10],  m14 = m[14];
            const GLfloat m3 = m[3],  m7 = m[7],  m11 = m[11],  m15 = m[15];
            GLuint i;
            for (i=0;i<n;i++) {
               const GLfloat ox = vObj[0], oy = vObj[1];
               const GLfloat oz = vObj[2], ow = vObj[3];
               vEye[i][0] = m0 * ox + m4 * oy + m8  * oz + m12 * ow;
               vEye[i][1] = m1 * ox + m5 * oy + m9  * oz + m13 * ow;
               vEye[i][2] = m2 * ox + m6 * oy + m10 * oz + m14 * ow;
               vEye[i][3] = m3 * ox + m7 * oy + m11 * oz + m15 * ow;
               vObj = (const GLfloat *) ((GLubyte *) vObj + stride);
            }
         }
         break;
      case MATRIX_IDENTITY:
         {
            GLuint i;
            for (i=0;i<n;i++) {
               vEye[i][0] = vObj[0];
               vEye[i][1] = vObj[1];
               vEye[i][2] = vObj[2];
               vEye[i][3] = vObj[3];
               vObj = (const GLfloat *) ((GLubyte *) vObj + stride);
            }
         }
         break;
      case MATRIX_2D:
         {
            const GLfloat *m = ctx->ModelViewMatrix;
            const GLfloat m0 = m[0], m1 = m[1], m4 = m[4], m5 = m[5];
            const GLfloat m12 = m[12], m13 = m[13];
            GLuint i;
            for (i=0;i<n;i++) {
               const GLfloat ox = vObj[0], oy = vObj[1];
               const GLfloat oz = vObj[2], ow = vObj[3];
               vEye[i][0] = m0 * ox + m4 * oy            + m12 * ow;
               vEye[i][1] = m1 * ox + m5 * oy            + m13 * ow;
               vEye[i][2] =                   +       oz           ;
               vEye[i][3] =                                      ow;
               vObj = (const GLfloat *) ((GLubyte *) vObj + stride);
            }
         }
         break;
      case MATRIX_2D_NO_ROT:
         {
            const GLfloat *m = ctx->ModelViewMatrix;
            const GLfloat m0 = m[0], m5 = m[5], m12 = m[12], m13 = m[13];
            GLuint i;
            for (i=0;i<n;i++) {
               const GLfloat ox = vObj[0], oy = vObj[1];
               const GLfloat oz = vObj[2], ow = vObj[3];
               vEye[i][0] = m0 * ox                      + m12 * ow;
               vEye[i][1] =           m5 * oy            + m13 * ow;
               vEye[i][2] =                   +       oz           ;
               vEye[i][3] =                                      ow;
               vObj = (const GLfloat *) ((GLubyte *) vObj + stride);
            }
         }
         break;
      case MATRIX_3D:
         {
            const GLfloat *m = ctx->ModelViewMatrix;
            const GLfloat m0 = m[0], m1 = m[1], m2 = m[2], m4 = m[4];
            const GLfloat m5 = m[5], m6 = m[6], m8 = m[8], m9 = m[9];
            const GLfloat m10 = m[10], m12 = m[12], m13 = m[13], m14 = m[14];
            GLuint i;
            for (i=0;i<n;i++) {
               const GLfloat ox = vObj[0], oy = vObj[1];
               const GLfloat oz = vObj[2], ow = vObj[3];
               vEye[i][0] = m0 * ox + m4 * oy +  m8 * oz + m12 * ow;
               vEye[i][1] = m1 * ox + m5 * oy +  m9 * oz + m13 * ow;
               vEye[i][2] = m2 * ox + m6 * oy + m10 * oz + m14 * ow;
               vEye[i][3] =                                      ow;
               vObj = (const GLfloat *) ((GLubyte *) vObj + stride);
            }
         }
         break;
      default:
         /* should never get here */
         gl_problem( NULL, "invalid matrix type in gl_transform_points4()" );
   }
}


