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
 * Revision 3.7  1998/08/23 22:18:18  brianp
 * added Driver.Viewport and Driver.DepthRange function pointers
 *
 * Revision 3.6  1998/06/07 22:18:52  brianp
 * implemented GL_EXT_multitexture extension
 *
 * Revision 3.5  1998/05/07 00:12:57  brianp
 * new invert_matrix() function from Jacques Leroy
 *
 * Revision 3.4  1998/03/27 04:17:31  brianp
 * fixed G++ warnings
 *
 * Revision 3.3  1998/03/03 02:39:29  brianp
 * fixed divide by zero problem in gl_LoadMatrixf() (Tim Rowley)
 *
 * Revision 3.2  1998/02/20 04:50:44  brianp
 * implemented GL_SGIS_multitexture
 *
 * Revision 3.1  1998/02/08 20:19:12  brianp
 * removed unneeded headers
 *
 * Revision 3.0  1998/01/31 20:58:46  brianp
 * initial rev
 *
 */


/*
 * Matrix operations
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "context.h"
#include "macros.h"
#include "matrix.h"
#include "mmath.h"
#include "types.h"
#endif



static GLfloat Identity[16] = {
   1.0, 0.0, 0.0, 0.0,
   0.0, 1.0, 0.0, 0.0,
   0.0, 0.0, 1.0, 0.0,
   0.0, 0.0, 0.0, 1.0
};




#if 0
static void print_matrix( const GLfloat m[16] )
{
   int i;

   for (i=0;i<4;i++) {
      printf("%f %f %f %f\n", m[i], m[4+i], m[8+i], m[12+i] );
   }
}
#endif



/*
 * Perform a 4x4 matrix multiplication  (product = a x b).
 * Input:  a, b - matrices to multiply
 * Output:  product - product of a and b
 * WARNING: (product != b) assumed
 * NOTE:    (product == a) allowed    
 */
static void matmul( GLfloat *product, const GLfloat *a, const GLfloat *b )
{
   /* This matmul was contributed by Thomas Malik */
   GLint i;

#define A(row,col)  a[(col<<2)+row]
#define B(row,col)  b[(col<<2)+row]
#define P(row,col)  product[(col<<2)+row]

   /* i-te Zeile */
   for (i = 0; i < 4; i++) {
      GLfloat ai0=A(i,0),  ai1=A(i,1),  ai2=A(i,2),  ai3=A(i,3);
      P(i,0) = ai0 * B(0,0) + ai1 * B(1,0) + ai2 * B(2,0) + ai3 * B(3,0);
      P(i,1) = ai0 * B(0,1) + ai1 * B(1,1) + ai2 * B(2,1) + ai3 * B(3,1);
      P(i,2) = ai0 * B(0,2) + ai1 * B(1,2) + ai2 * B(2,2) + ai3 * B(3,2);
      P(i,3) = ai0 * B(0,3) + ai1 * B(1,3) + ai2 * B(2,3) + ai3 * B(3,3);
   }

#undef A
#undef B
#undef P
}



/*
 * Compute inverse of 4x4 transformation matrix.
 * Code contributed by Jacques Leroy jle@star.be
 * Return GL_TRUE for success, GL_FALSE for failure (singular matrix)
 */
static GLboolean invert_matrix( const GLfloat *m, GLfloat *out )
{
/* NB. OpenGL Matrices are COLUMN major. */
#define SWAP_ROWS(a, b) { GLfloat *_tmp = a; (a)=(b); (b)=_tmp; }
#define MAT(m,r,c) (m)[(c)*4+(r)]

 GLfloat wtmp[4][8];
 GLfloat m0, m1, m2, m3, s;
 GLfloat *r0, *r1, *r2, *r3;

 r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

 r0[0] = MAT(m,0,0), r0[1] = MAT(m,0,1),
 r0[2] = MAT(m,0,2), r0[3] = MAT(m,0,3),
 r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,

 r1[0] = MAT(m,1,0), r1[1] = MAT(m,1,1),
 r1[2] = MAT(m,1,2), r1[3] = MAT(m,1,3),
 r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,

 r2[0] = MAT(m,2,0), r2[1] = MAT(m,2,1),
 r2[2] = MAT(m,2,2), r2[3] = MAT(m,2,3),
 r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,

 r3[0] = MAT(m,3,0), r3[1] = MAT(m,3,1),
 r3[2] = MAT(m,3,2), r3[3] = MAT(m,3,3),
 r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;

 /* choose pivot - or die */
 if (fabs(r3[0])>fabs(r2[0])) SWAP_ROWS(r3, r2);
 if (fabs(r2[0])>fabs(r1[0])) SWAP_ROWS(r2, r1);
 if (fabs(r1[0])>fabs(r0[0])) SWAP_ROWS(r1, r0);
 if (0.0 == r0[0])  return GL_FALSE;

 /* eliminate first variable     */
 m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
 s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
 s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
 s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
 s = r0[4];
 if (s != 0.0) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
 s = r0[5];
 if (s != 0.0) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
 s = r0[6];
 if (s != 0.0) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
 s = r0[7];
 if (s != 0.0) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }

 /* choose pivot - or die */
 if (fabs(r3[1])>fabs(r2[1])) SWAP_ROWS(r3, r2);
 if (fabs(r2[1])>fabs(r1[1])) SWAP_ROWS(r2, r1);
 if (0.0 == r1[1])  return GL_FALSE;

 /* eliminate second variable */
 m2 = r2[1]/r1[1]; m3 = r3[1]/r1[1];
 r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
 r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
 s = r1[4]; if (0.0 != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
 s = r1[5]; if (0.0 != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
 s = r1[6]; if (0.0 != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
 s = r1[7]; if (0.0 != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }

 /* choose pivot - or die */
 if (fabs(r3[2])>fabs(r2[2])) SWAP_ROWS(r3, r2);
 if (0.0 == r2[2])  return GL_FALSE;

 /* eliminate third variable */
 m3 = r3[2]/r2[2];
 r3[3] -= m3 * r2[3], r3[4] -= m3 * r2[4],
 r3[5] -= m3 * r2[5], r3[6] -= m3 * r2[6],
 r3[7] -= m3 * r2[7];

 /* last check */
 if (0.0 == r3[3]) return GL_FALSE;

 s = 1.0/r3[3];              /* now back substitute row 3 */
 r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;

 m2 = r2[3];                 /* now back substitute row 2 */
 s  = 1.0/r2[2];
 r2[4] = s * (r2[4] - r3[4] * m2), r2[5] = s * (r2[5] - r3[5] * m2),
 r2[6] = s * (r2[6] - r3[6] * m2), r2[7] = s * (r2[7] - r3[7] * m2);
 m1 = r1[3];
 r1[4] -= r3[4] * m1, r1[5] -= r3[5] * m1,
 r1[6] -= r3[6] * m1, r1[7] -= r3[7] * m1;
 m0 = r0[3];
 r0[4] -= r3[4] * m0, r0[5] -= r3[5] * m0,
 r0[6] -= r3[6] * m0, r0[7] -= r3[7] * m0;

 m1 = r1[2];                 /* now back substitute row 1 */
 s  = 1.0/r1[1];
 r1[4] = s * (r1[4] - r2[4] * m1), r1[5] = s * (r1[5] - r2[5] * m1),
 r1[6] = s * (r1[6] - r2[6] * m1), r1[7] = s * (r1[7] - r2[7] * m1);
 m0 = r0[2];
 r0[4] -= r2[4] * m0, r0[5] -= r2[5] * m0,
 r0[6] -= r2[6] * m0, r0[7] -= r2[7] * m0;

 m0 = r0[1];                 /* now back substitute row 0 */
 s  = 1.0/r0[0];
 r0[4] = s * (r0[4] - r1[4] * m0), r0[5] = s * (r0[5] - r1[5] * m0),
 r0[6] = s * (r0[6] - r1[6] * m0), r0[7] = s * (r0[7] - r1[7] * m0);

 MAT(out,0,0) = r0[4]; MAT(out,0,1) = r0[5],
 MAT(out,0,2) = r0[6]; MAT(out,0,3) = r0[7],
 MAT(out,1,0) = r1[4]; MAT(out,1,1) = r1[5],
 MAT(out,1,2) = r1[6]; MAT(out,1,3) = r1[7],
 MAT(out,2,0) = r2[4]; MAT(out,2,1) = r2[5],
 MAT(out,2,2) = r2[6]; MAT(out,2,3) = r2[7],
 MAT(out,3,0) = r3[4]; MAT(out,3,1) = r3[5],
 MAT(out,3,2) = r3[6]; MAT(out,3,3) = r3[7]; 

 return GL_TRUE;

#undef MAT
#undef SWAP_ROWS
}



/*
 * Determine if the given matrix is the identity matrix.
 */
static GLboolean is_identity( const GLfloat m[16] )
{
   if (   m[0]==1.0F && m[4]==0.0F && m[ 8]==0.0F && m[12]==0.0F
       && m[1]==0.0F && m[5]==1.0F && m[ 9]==0.0F && m[13]==0.0F
       && m[2]==0.0F && m[6]==0.0F && m[10]==1.0F && m[14]==0.0F
       && m[3]==0.0F && m[7]==0.0F && m[11]==0.0F && m[15]==1.0F) {
      return GL_TRUE;
   }
   else {
      return GL_FALSE;
   }
}


/*
 * Examine the current modelview matrix to determine its type.
 * Later we use the matrix type to optimize vertex transformations.
 */
void gl_analyze_modelview_matrix( GLcontext *ctx )
{
   const GLfloat *m = ctx->ModelViewMatrix;
   if (is_identity(m)) {
      ctx->ModelViewMatrixType = MATRIX_IDENTITY;
   }
   else if (                 m[4]==0.0F && m[ 8]==0.0F               
            && m[1]==0.0F               && m[ 9]==0.0F
            && m[2]==0.0F && m[6]==0.0F && m[10]==1.0F && m[14]==0.0F
            && m[3]==0.0F && m[7]==0.0F && m[11]==0.0F && m[15]==1.0F) {
      ctx->ModelViewMatrixType = MATRIX_2D_NO_ROT;
   }
   else if (                               m[ 8]==0.0F               
            &&                             m[ 9]==0.0F
            && m[2]==0.0F && m[6]==0.0F && m[10]==1.0F && m[14]==0.0F
            && m[3]==0.0F && m[7]==0.0F && m[11]==0.0F && m[15]==1.0F) {
      ctx->ModelViewMatrixType = MATRIX_2D;
   }
   else if (m[3]==0.0F && m[7]==0.0F && m[11]==0.0F && m[15]==1.0F) {
      ctx->ModelViewMatrixType = MATRIX_3D;
   }
   else {
      ctx->ModelViewMatrixType = MATRIX_GENERAL;
   }

   if (!invert_matrix( ctx->ModelViewMatrix, ctx->ModelViewInv )) {
      MEMCPY( ctx->ModelViewInv, Identity, 16 * sizeof(GLfloat) );
   }

   ctx->NewModelViewMatrix = GL_FALSE;
}



/*
 * Examine the current projection matrix to determine its type.
 * Later we use the matrix type to optimize vertex transformations.
 */
void gl_analyze_projection_matrix( GLcontext *ctx )
{
   /* look for common-case ortho and perspective matrices */
   const GLfloat *m = ctx->ProjectionMatrix;
   if (is_identity(m)) {
      ctx->ProjectionMatrixType = MATRIX_IDENTITY;
   }
   else if (                 m[4]==0.0F && m[8] ==0.0F
            && m[1]==0.0F               && m[9] ==0.0F
            && m[2]==0.0F && m[6]==0.0F
            && m[3]==0.0F && m[7]==0.0F && m[11]==0.0F && m[15]==1.0F) {
      ctx->ProjectionMatrixType = MATRIX_ORTHO;
   }
   else if (                 m[4]==0.0F                 && m[12]==0.0F
            && m[1]==0.0F                               && m[13]==0.0F
            && m[2]==0.0F && m[6]==0.0F
            && m[3]==0.0F && m[7]==0.0F && m[11]==-1.0F && m[15]==0.0F) {
      ctx->ProjectionMatrixType = MATRIX_PERSPECTIVE;
   }
   else {
      ctx->ProjectionMatrixType = MATRIX_GENERAL;
   }

   ctx->NewProjectionMatrix = GL_FALSE;
}



/*
 * Examine the current texture matrix to determine its type.
 * Later we use the matrix type to optimize texture coordinate transformations.
 */
void gl_analyze_texture_matrix( GLcontext *ctx )
{
   GLuint texSet;
   for (texSet = 0; texSet<MAX_TEX_COORD_SETS; texSet++) {
      const GLfloat *m = ctx->TextureMatrix[texSet];
   if (is_identity(m)) {
         ctx->TextureMatrixType[texSet] = MATRIX_IDENTITY;
   }
   else if (                               m[ 8]==0.0F               
            &&                             m[ 9]==0.0F
       && m[2]==0.0F && m[6]==0.0F && m[10]==1.0F && m[14]==0.0F
       && m[3]==0.0F && m[7]==0.0F && m[11]==0.0F && m[15]==1.0F) {
         ctx->TextureMatrixType[texSet] = MATRIX_2D;
   }
   else if (m[3]==0.0F && m[7]==0.0F && m[11]==0.0F && m[15]==1.0F) {
         ctx->TextureMatrixType[texSet] = MATRIX_3D;
   }
   else {
         ctx->TextureMatrixType[texSet] = MATRIX_GENERAL;
      }
   }
   ctx->NewTextureMatrix = GL_FALSE;
}



void gl_Frustum( GLcontext *ctx,
                 GLdouble left, GLdouble right,
	 	 GLdouble bottom, GLdouble top,
		 GLdouble nearval, GLdouble farval )
{
   GLfloat x, y, a, b, c, d;
   GLfloat m[16];

   if (nearval<=0.0 || farval<=0.0) {
      gl_error( ctx,  GL_INVALID_VALUE, "glFrustum(near or far)" );
   }

   x = (2.0*nearval) / (right-left);
   y = (2.0*nearval) / (top-bottom);
   a = (right+left) / (right-left);
   b = (top+bottom) / (top-bottom);
   c = -(farval+nearval) / ( farval-nearval);
   d = -(2.0*farval*nearval) / (farval-nearval);  /* error? */

#define M(row,col)  m[col*4+row]
   M(0,0) = x;     M(0,1) = 0.0F;  M(0,2) = a;      M(0,3) = 0.0F;
   M(1,0) = 0.0F;  M(1,1) = y;     M(1,2) = b;      M(1,3) = 0.0F;
   M(2,0) = 0.0F;  M(2,1) = 0.0F;  M(2,2) = c;      M(2,3) = d;
   M(3,0) = 0.0F;  M(3,1) = 0.0F;  M(3,2) = -1.0F;  M(3,3) = 0.0F;
#undef M

   gl_MultMatrixf( ctx, m );


   /* Need to keep a stack of near/far values in case the user push/pops
    * the projection matrix stack so that we can call Driver.NearFar()
    * after a pop.
    */
   ctx->NearFarStack[ctx->ProjectionStackDepth][0] = nearval;
   ctx->NearFarStack[ctx->ProjectionStackDepth][1] = farval;

   if (ctx->Driver.NearFar) {
      (*ctx->Driver.NearFar)( ctx, nearval, farval );
   }
}


void gl_Ortho( GLcontext *ctx,
               GLdouble left, GLdouble right,
               GLdouble bottom, GLdouble top,
               GLdouble nearval, GLdouble farval )
{
   GLfloat x, y, z;
   GLfloat tx, ty, tz;
   GLfloat m[16];

   x = 2.0 / (right-left);
   y = 2.0 / (top-bottom);
   z = -2.0 / (farval-nearval);
   tx = -(right+left) / (right-left);
   ty = -(top+bottom) / (top-bottom);
   tz = -(farval+nearval) / (farval-nearval);

#define M(row,col)  m[col*4+row]
   M(0,0) = x;     M(0,1) = 0.0F;  M(0,2) = 0.0F;  M(0,3) = tx;
   M(1,0) = 0.0F;  M(1,1) = y;     M(1,2) = 0.0F;  M(1,3) = ty;
   M(2,0) = 0.0F;  M(2,1) = 0.0F;  M(2,2) = z;     M(2,3) = tz;
   M(3,0) = 0.0F;  M(3,1) = 0.0F;  M(3,2) = 0.0F;  M(3,3) = 1.0F;
#undef M

   gl_MultMatrixf( ctx, m );

   if (ctx->Driver.NearFar) {
      (*ctx->Driver.NearFar)( ctx, nearval, farval );
   }
}


void gl_MatrixMode( GLcontext *ctx, GLenum mode )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx,  GL_INVALID_OPERATION, "glMatrixMode" );
      return;
   }
   switch (mode) {
      case GL_MODELVIEW:
      case GL_PROJECTION:
      case GL_TEXTURE:
         ctx->Transform.MatrixMode = mode;
         break;
      default:
         gl_error( ctx,  GL_INVALID_ENUM, "glMatrixMode" );
   }
}



void gl_PushMatrix( GLcontext *ctx )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx,  GL_INVALID_OPERATION, "glPushMatrix" );
      return;
   }
   switch (ctx->Transform.MatrixMode) {
      case GL_MODELVIEW:
         if (ctx->ModelViewStackDepth>=MAX_MODELVIEW_STACK_DEPTH-1) {
            gl_error( ctx,  GL_STACK_OVERFLOW, "glPushMatrix");
            return;
         }
         MEMCPY( ctx->ModelViewStack[ctx->ModelViewStackDepth],
                 ctx->ModelViewMatrix,
                 16*sizeof(GLfloat) );
         ctx->ModelViewStackDepth++;
         break;
      case GL_PROJECTION:
         if (ctx->ProjectionStackDepth>=MAX_PROJECTION_STACK_DEPTH) {
            gl_error( ctx,  GL_STACK_OVERFLOW, "glPushMatrix");
            return;
         }
         MEMCPY( ctx->ProjectionStack[ctx->ProjectionStackDepth],
                 ctx->ProjectionMatrix,
                 16*sizeof(GLfloat) );
         ctx->ProjectionStackDepth++;

         /* Save near and far projection values */
         ctx->NearFarStack[ctx->ProjectionStackDepth][0]
            = ctx->NearFarStack[ctx->ProjectionStackDepth-1][0];
         ctx->NearFarStack[ctx->ProjectionStackDepth][1]
            = ctx->NearFarStack[ctx->ProjectionStackDepth-1][1];
         break;
      case GL_TEXTURE:
         {
            GLuint texSet = ctx->Texture.CurrentTransformSet;
            if (ctx->TextureStackDepth[texSet] >= MAX_TEXTURE_STACK_DEPTH) {
            gl_error( ctx,  GL_STACK_OVERFLOW, "glPushMatrix");
            return;
         }
            MEMCPY( ctx->TextureStack[texSet][ctx->TextureStackDepth[texSet]],
                    ctx->TextureMatrix[texSet],
                 16*sizeof(GLfloat) );
            ctx->TextureStackDepth[texSet]++;
         }
         break;
      default:
         gl_problem(ctx, "Bad matrix mode in gl_PushMatrix");
   }
}



void gl_PopMatrix( GLcontext *ctx )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx,  GL_INVALID_OPERATION, "glPopMatrix" );
      return;
   }
   switch (ctx->Transform.MatrixMode) {
      case GL_MODELVIEW:
         if (ctx->ModelViewStackDepth==0) {
            gl_error( ctx,  GL_STACK_UNDERFLOW, "glPopMatrix");
            return;
         }
         ctx->ModelViewStackDepth--;
         MEMCPY( ctx->ModelViewMatrix,
                 ctx->ModelViewStack[ctx->ModelViewStackDepth],
                 16*sizeof(GLfloat) );
         ctx->NewModelViewMatrix = GL_TRUE;
         break;
      case GL_PROJECTION:
         if (ctx->ProjectionStackDepth==0) {
            gl_error( ctx,  GL_STACK_UNDERFLOW, "glPopMatrix");
            return;
         }
         ctx->ProjectionStackDepth--;
         MEMCPY( ctx->ProjectionMatrix,
                 ctx->ProjectionStack[ctx->ProjectionStackDepth],
                 16*sizeof(GLfloat) );
         ctx->NewProjectionMatrix = GL_TRUE;

         /* Device driver near/far values */
         {
            GLfloat nearVal = ctx->NearFarStack[ctx->ProjectionStackDepth][0];
            GLfloat farVal  = ctx->NearFarStack[ctx->ProjectionStackDepth][1];
            if (ctx->Driver.NearFar) {
               (*ctx->Driver.NearFar)( ctx, nearVal, farVal );
            }
         }
         break;
      case GL_TEXTURE:
         {
            GLuint texSet = ctx->Texture.CurrentTransformSet;
            if (ctx->TextureStackDepth[texSet]==0) {
            gl_error( ctx,  GL_STACK_UNDERFLOW, "glPopMatrix");
            return;
         }
            ctx->TextureStackDepth[texSet]--;
            MEMCPY( ctx->TextureMatrix[texSet],
                    ctx->TextureStack[texSet][ctx->TextureStackDepth[texSet]],
                 16*sizeof(GLfloat) );
         ctx->NewTextureMatrix = GL_TRUE;
         }
         break;
      default:
         gl_problem(ctx, "Bad matrix mode in gl_PopMatrix");
   }
}



void gl_LoadIdentity( GLcontext *ctx )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx,  GL_INVALID_OPERATION, "glLoadIdentity" );
      return;
   }
   switch (ctx->Transform.MatrixMode) {
      case GL_MODELVIEW:
         MEMCPY( ctx->ModelViewMatrix, Identity, 16*sizeof(GLfloat) );
         MEMCPY( ctx->ModelViewInv, Identity, 16*sizeof(GLfloat) );
         ctx->ModelViewMatrixType = MATRIX_IDENTITY;
	 ctx->NewModelViewMatrix = GL_FALSE;
	 break;
      case GL_PROJECTION:
	 MEMCPY( ctx->ProjectionMatrix, Identity, 16*sizeof(GLfloat) );
         ctx->ProjectionMatrixType = MATRIX_IDENTITY;
	 ctx->NewProjectionMatrix = GL_FALSE;
	 break;
      case GL_TEXTURE:
         {
            GLuint texSet = ctx->Texture.CurrentTransformSet;
            MEMCPY( ctx->TextureMatrix[texSet], Identity, 16*sizeof(GLfloat) );
            ctx->TextureMatrixType[texSet] = MATRIX_IDENTITY;
	 ctx->NewTextureMatrix = GL_FALSE;
         }
	 break;
      default:
         gl_problem(ctx, "Bad matrix mode in gl_LoadIdentity");
   }
}


void gl_LoadMatrixf( GLcontext *ctx, const GLfloat *m )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx,  GL_INVALID_OPERATION, "glLoadMatrix" );
      return;
   }
   switch (ctx->Transform.MatrixMode) {
      case GL_MODELVIEW:
         MEMCPY( ctx->ModelViewMatrix, m, 16*sizeof(GLfloat) );
	 ctx->NewModelViewMatrix = GL_TRUE;
	 break;
      case GL_PROJECTION:
	 MEMCPY( ctx->ProjectionMatrix, m, 16*sizeof(GLfloat) );
	 ctx->NewProjectionMatrix = GL_TRUE;
         {
            GLfloat n, f, c, d;

#define M(row,col)  m[col*4+row]
            c = M(2,2);
            d = M(2,3);
#undef M
            if (c == 1.0)
               n = 0.0;
            else
               n = d / (c - 1.0);

            if (c == -1.0)
               f = 1.0;
            else
               f = d / (c + 1.0);

            /* Need to keep a stack of near/far values in case the user
             * push/pops the projection matrix stack so that we can call
             * Driver.NearFar() after a pop.
             */
            ctx->NearFarStack[ctx->ProjectionStackDepth][0] = n;
            ctx->NearFarStack[ctx->ProjectionStackDepth][1] = f;

            if (ctx->Driver.NearFar) {
               (*ctx->Driver.NearFar)( ctx, n, f );
            }
         }
	 break;
      case GL_TEXTURE:
         {
            GLuint texSet = ctx->Texture.CurrentTransformSet;
            MEMCPY( ctx->TextureMatrix[texSet], m, 16*sizeof(GLfloat) );
	 ctx->NewTextureMatrix = GL_TRUE;
         }
	 break;
      default:
         gl_problem(ctx, "Bad matrix mode in gl_LoadMatrixf");
   }
}



void gl_MultMatrixf( GLcontext *ctx, const GLfloat *m )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx,  GL_INVALID_OPERATION, "glMultMatrix" );
      return;
   }
   switch (ctx->Transform.MatrixMode) {
      case GL_MODELVIEW:
         matmul( ctx->ModelViewMatrix, ctx->ModelViewMatrix, m );
	 ctx->NewModelViewMatrix = GL_TRUE;
	 break;
      case GL_PROJECTION:
	 matmul( ctx->ProjectionMatrix, ctx->ProjectionMatrix, m );
	 ctx->NewProjectionMatrix = GL_TRUE;
	 break;
      case GL_TEXTURE:
         {
            GLuint texSet = ctx->Texture.CurrentTransformSet;
            matmul( ctx->TextureMatrix[texSet], ctx->TextureMatrix[texSet], m );
	 ctx->NewTextureMatrix = GL_TRUE;
         }
	 break;
      default:
         gl_problem(ctx, "Bad matrix mode in gl_MultMatrixf");
   }
}



/*
 * Generate a 4x4 transformation matrix from glRotate parameters.
 */
void gl_rotation_matrix( GLfloat angle, GLfloat x, GLfloat y, GLfloat z,
                         GLfloat m[] )
{
   /* This function contributed by Erich Boleyn (erich@uruk.org) */
   GLfloat mag, s, c;
   GLfloat xx, yy, zz, xy, yz, zx, xs, ys, zs, one_c;

   s = sin( angle * DEG2RAD );
   c = cos( angle * DEG2RAD );

   mag = GL_SQRT( x*x + y*y + z*z );

   if (mag == 0.0) {
      /* generate an identity matrix and return */
      MEMCPY(m, Identity, sizeof(GLfloat)*16);
      return;
   }

   x /= mag;
   y /= mag;
   z /= mag;

#define M(row,col)  m[col*4+row]

   /*
    *     Arbitrary axis rotation matrix.
    *
    *  This is composed of 5 matrices, Rz, Ry, T, Ry', Rz', multiplied
    *  like so:  Rz * Ry * T * Ry' * Rz'.  T is the final rotation
    *  (which is about the X-axis), and the two composite transforms
    *  Ry' * Rz' and Rz * Ry are (respectively) the rotations necessary
    *  from the arbitrary axis to the X-axis then back.  They are
    *  all elementary rotations.
    *
    *  Rz' is a rotation about the Z-axis, to bring the axis vector
    *  into the x-z plane.  Then Ry' is applied, rotating about the
    *  Y-axis to bring the axis vector parallel with the X-axis.  The
    *  rotation about the X-axis is then performed.  Ry and Rz are
    *  simply the respective inverse transforms to bring the arbitrary
    *  axis back to it's original orientation.  The first transforms
    *  Rz' and Ry' are considered inverses, since the data from the
    *  arbitrary axis gives you info on how to get to it, not how
    *  to get away from it, and an inverse must be applied.
    *
    *  The basic calculation used is to recognize that the arbitrary
    *  axis vector (x, y, z), since it is of unit length, actually
    *  represents the sines and cosines of the angles to rotate the
    *  X-axis to the same orientation, with theta being the angle about
    *  Z and phi the angle about Y (in the order described above)
    *  as follows:
    *
    *  cos ( theta ) = x / sqrt ( 1 - z^2 )
    *  sin ( theta ) = y / sqrt ( 1 - z^2 )
    *
    *  cos ( phi ) = sqrt ( 1 - z^2 )
    *  sin ( phi ) = z
    *
    *  Note that cos ( phi ) can further be inserted to the above
    *  formulas:
    *
    *  cos ( theta ) = x / cos ( phi )
    *  sin ( theta ) = y / sin ( phi )
    *
    *  ...etc.  Because of those relations and the standard trigonometric
    *  relations, it is pssible to reduce the transforms down to what
    *  is used below.  It may be that any primary axis chosen will give the
    *  same results (modulo a sign convention) using thie method.
    *
    *  Particularly nice is to notice that all divisions that might
    *  have caused trouble when parallel to certain planes or
    *  axis go away with care paid to reducing the expressions.
    *  After checking, it does perform correctly under all cases, since
    *  in all the cases of division where the denominator would have
    *  been zero, the numerator would have been zero as well, giving
    *  the expected result.
    */

   xx = x * x;
   yy = y * y;
   zz = z * z;
   xy = x * y;
   yz = y * z;
   zx = z * x;
   xs = x * s;
   ys = y * s;
   zs = z * s;
   one_c = 1.0F - c;

   M(0,0) = (one_c * xx) + c;
   M(0,1) = (one_c * xy) - zs;
   M(0,2) = (one_c * zx) + ys;
   M(0,3) = 0.0F;

   M(1,0) = (one_c * xy) + zs;
   M(1,1) = (one_c * yy) + c;
   M(1,2) = (one_c * yz) - xs;
   M(1,3) = 0.0F;

   M(2,0) = (one_c * zx) - ys;
   M(2,1) = (one_c * yz) + xs;
   M(2,2) = (one_c * zz) + c;
   M(2,3) = 0.0F;

   M(3,0) = 0.0F;
   M(3,1) = 0.0F;
   M(3,2) = 0.0F;
   M(3,3) = 1.0F;

#undef M
}



void gl_Rotatef( GLcontext *ctx,
                 GLfloat angle, GLfloat x, GLfloat y, GLfloat z )
{
   GLfloat m[16];
   gl_rotation_matrix( angle, x, y, z, m );
   gl_MultMatrixf( ctx, m );
}



/*
 * Execute a glScale call
 */
void gl_Scalef( GLcontext *ctx, GLfloat x, GLfloat y, GLfloat z )
{
   GLfloat *m;

   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx,  GL_INVALID_OPERATION, "glScale" );
      return;
   }
   switch (ctx->Transform.MatrixMode) {
      case GL_MODELVIEW:
         m = ctx->ModelViewMatrix;
	 ctx->NewModelViewMatrix = GL_TRUE;
	 break;
      case GL_PROJECTION:
         m = ctx->ProjectionMatrix;
	 ctx->NewProjectionMatrix = GL_TRUE;
	 break;
      case GL_TEXTURE:
         {
            GLuint texSet = ctx->Texture.CurrentTransformSet;
            m = ctx->TextureMatrix[texSet];
	 ctx->NewTextureMatrix = GL_TRUE;
         }
	 break;
      default:
         gl_problem(ctx, "Bad matrix mode in gl_Scalef");
         return;
   }
   m[0] *= x;   m[4] *= y;   m[8]  *= z;
   m[1] *= x;   m[5] *= y;   m[9]  *= z;
   m[2] *= x;   m[6] *= y;   m[10] *= z;
   m[3] *= x;   m[7] *= y;   m[11] *= z;
}



/*
 * Execute a glTranslate call
 */
void gl_Translatef( GLcontext *ctx, GLfloat x, GLfloat y, GLfloat z )
{
   GLfloat *m;
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glTranslate" );
      return;
   }
   switch (ctx->Transform.MatrixMode) {
      case GL_MODELVIEW:
         m = ctx->ModelViewMatrix;
	 ctx->NewModelViewMatrix = GL_TRUE;
	 break;
      case GL_PROJECTION:
         m = ctx->ProjectionMatrix;
	 ctx->NewProjectionMatrix = GL_TRUE;
	 break;
      case GL_TEXTURE:
         {
            GLuint texSet = ctx->Texture.CurrentTransformSet;
            m = ctx->TextureMatrix[texSet];
	 ctx->NewTextureMatrix = GL_TRUE;
         }
	 break;
      default:
         gl_problem(ctx, "Bad matrix mode in gl_Translatef");
         return;
   }

   m[12] = m[0] * x + m[4] * y + m[8]  * z + m[12];
   m[13] = m[1] * x + m[5] * y + m[9]  * z + m[13];
   m[14] = m[2] * x + m[6] * y + m[10] * z + m[14];
   m[15] = m[3] * x + m[7] * y + m[11] * z + m[15];
}




/*
 * Define a new viewport and reallocate auxillary buffers if the size of
 * the window (color buffer) has changed.
 */
void gl_Viewport( GLcontext *ctx,
                  GLint x, GLint y, GLsizei width, GLsizei height )
{
   if (width<0 || height<0) {
      gl_error( ctx,  GL_INVALID_VALUE, "glViewport" );
      return;
   }
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx,  GL_INVALID_OPERATION, "glViewport" );
      return;
   }

   /* clamp width, and height to implementation dependent range */
   width  = CLAMP( width,  1, MAX_WIDTH );
   height = CLAMP( height, 1, MAX_HEIGHT );

   /* Save viewport */
   ctx->Viewport.X = x;
   ctx->Viewport.Width = width;
   ctx->Viewport.Y = y;
   ctx->Viewport.Height = height;

   /* compute scale and bias values */
   ctx->Viewport.Sx = (GLfloat) width / 2.0F;
   ctx->Viewport.Tx = ctx->Viewport.Sx + x;
   ctx->Viewport.Sy = (GLfloat) height / 2.0F;
   ctx->Viewport.Ty = ctx->Viewport.Sy + y;

   ctx->NewState |= NEW_ALL;   /* just to be safe */

   /* Check if window/buffer has been resized and if so, reallocate the
    * ancillary buffers.
    */
   gl_ResizeBuffersMESA(ctx);

   if (ctx->Driver.Viewport) {
      (*ctx->Driver.Viewport)( ctx, x, y, width, height );
   }
}



void gl_DepthRange( GLcontext *ctx, GLclampd nearval, GLclampd farval )
{
   /*
    * nearval - specifies mapping of the near clipping plane to window
    *   coordinates, default is 0
    * farval - specifies mapping of the far clipping plane to window
    *   coordinates, default is 1
    *
    * After clipping and div by w, z coords are in -1.0 to 1.0,
    * corresponding to near and far clipping planes.  glDepthRange
    * specifies a linear mapping of the normalized z coords in
    * this range to window z coords.
    */
   GLfloat n, f;

   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glDepthRange" );
      return;
   }

   n = (GLfloat) CLAMP( nearval, 0.0, 1.0 );
   f = (GLfloat) CLAMP( farval, 0.0, 1.0 );

   ctx->Viewport.Near = n;
   ctx->Viewport.Far = f;
   ctx->Viewport.Sz = DEPTH_SCALE * ((f - n) / 2.0);
   ctx->Viewport.Tz = DEPTH_SCALE * ((f - n) / 2.0 + n);

   if (ctx->Driver.DepthRange) {
      (*ctx->Driver.DepthRange)( ctx, nearval, farval );
   }
}
