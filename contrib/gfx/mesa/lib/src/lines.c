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
 * Revision 3.12  1998/07/08 01:03:28  brianp
 * removed some unused variables
 *
 * Revision 3.11  1998/06/18 02:38:45  brianp
 * re-implemented INTERP_STUV code for textured lines
 *
 * Revision 3.10  1998/06/18 01:51:07  brianp
 * more work on AA texture coordinate interpolation
 *
 * Revision 3.9  1998/06/17 04:08:38  brianp
 * textured, AA lines didn't interpolate texture coords correctly
 *
 * Revision 3.8  1998/06/17 01:18:14  brianp
 * textured, AA lines test was backward (Marten Stromberg)
 *
 * Revision 3.7  1998/06/17 01:15:10  brianp
 * include assert.h
 *
 * Revision 3.6  1998/06/11 01:58:40  brianp
 * separate textured and non-textured AA line functions
 *
 * Revision 3.5  1998/06/09 03:23:55  brianp
 * implemented antialiased lines
 *
 * Revision 3.4  1998/06/07 22:18:52  brianp
 * implemented GL_EXT_multitexture extension
 *
 * Revision 3.3  1998/04/13 23:30:08  brianp
 * fixed PB overflow bug (Randy Frank)
 *
 * Revision 3.2  1998/03/27 04:17:52  brianp
 * fixed G++ warnings
 *
 * Revision 3.1  1998/02/04 00:44:29  brianp
 * fixed casts and conditional expression problems for Amiga StormC compiler
 *
 * Revision 3.0  1998/01/31 20:55:24  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <assert.h>
#include "context.h"
#include "depth.h"
#include "feedback.h"
#include "lines.h"
#include "macros.h"
#include "pb.h"
#include "texstate.h"
#include "types.h"
#include "vb.h"
#endif



void gl_LineWidth( GLcontext *ctx, GLfloat width )
{
   if (width<=0.0) {
      gl_error( ctx, GL_INVALID_VALUE, "glLineWidth" );
      return;
   }
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glLineWidth" );
      return;
   }
   ctx->Line.Width = width;
   ctx->NewState |= NEW_RASTER_OPS;
}



void gl_LineStipple( GLcontext *ctx, GLint factor, GLushort pattern )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glLineStipple" );
      return;
   }
   ctx->Line.StippleFactor = CLAMP( factor, 1, 256 );
   ctx->Line.StipplePattern = pattern;
   ctx->NewState |= NEW_RASTER_OPS;
}



/**********************************************************************/
/*****                    Rasterization                           *****/
/**********************************************************************/


/*
 * There are 4 pairs (RGBA, CI) of line drawing functions:
 *   1. simple:  width=1 and no special rasterization functions (fastest)
 *   2. flat:  width=1, non-stippled, flat-shaded, any raster operations
 *   3. smooth:  width=1, non-stippled, smooth-shaded, any raster operations
 *   4. general:  any other kind of line (slowest)
 */


/*
 * All line drawing functions have the same arguments:
 * v1, v2 - indexes of first and second endpoints into vertex buffer arrays
 * pv     - provoking vertex: which vertex color/index to use for flat shading.
 */



static void feedback_line( GLcontext *ctx, GLuint v1, GLuint v2, GLuint pv )
{
   struct vertex_buffer *VB = ctx->VB;
   GLfloat x1, y1, z1, w1;
   GLfloat x2, y2, z2, w2;
   GLfloat tex1[4], tex2[4], invq;
   GLuint texSet = ctx->Texture.CurrentTransformSet;

   x1 = VB->Win[v1][0];
   y1 = VB->Win[v1][1];
   z1 = VB->Win[v1][2] / DEPTH_SCALE;
   w1 = VB->Clip[v1][3];

   x2 = VB->Win[v2][0];
   y2 = VB->Win[v2][1];
   z2 = VB->Win[v2][2] / DEPTH_SCALE;
   w2 = VB->Clip[v2][3];

   invq = (VB->MultiTexCoord[texSet][v1][3]==0.0) ? 1.0 : (1.0F / VB->TexCoord[v1][3]);
   tex1[0] = VB->MultiTexCoord[texSet][v1][0] * invq;
   tex1[1] = VB->MultiTexCoord[texSet][v1][1] * invq;
   tex1[2] = VB->MultiTexCoord[texSet][v1][2] * invq;
   tex1[3] = VB->MultiTexCoord[texSet][v1][3];
   invq = (VB->MultiTexCoord[texSet][v2][3]==0.0) ? 1.0 : (1.0F / VB->TexCoord[v2][3]);
   tex2[0] = VB->MultiTexCoord[texSet][v2][0] * invq;
   tex2[1] = VB->MultiTexCoord[texSet][v2][1] * invq;
   tex2[2] = VB->MultiTexCoord[texSet][v2][2] * invq;
   tex2[3] = VB->MultiTexCoord[texSet][v2][3];

   if (ctx->StippleCounter==0) {
      FEEDBACK_TOKEN( ctx, (GLfloat) (GLint) GL_LINE_RESET_TOKEN );
   }
   else {
      FEEDBACK_TOKEN( ctx, (GLfloat) (GLint) GL_LINE_TOKEN );
   }

   {
      GLfloat color[4];
      /* convert color from integer to a float in [0,1] */
      color[0] = (GLfloat) VB->Color[pv][0] * (1.0F / 255.0F);
      color[1] = (GLfloat) VB->Color[pv][1] * (1.0F / 255.0F);
      color[2] = (GLfloat) VB->Color[pv][2] * (1.0F / 255.0F);
      color[3] = (GLfloat) VB->Color[pv][3] * (1.0F / 255.0F);
      gl_feedback_vertex( ctx, x1,y1,z1,w1, color,
                          (GLfloat) VB->Index[pv], tex1 );
      gl_feedback_vertex( ctx, x2,y2,z2,w2, color,
                          (GLfloat) VB->Index[pv], tex2 );
   }

   ctx->StippleCounter++;
}



static void select_line( GLcontext *ctx, GLuint v1, GLuint v2, GLuint pv )
{
   (void) pv;
   gl_update_hitflag( ctx, ctx->VB->Win[v1][2] / DEPTH_SCALE );
   gl_update_hitflag( ctx, ctx->VB->Win[v2][2] / DEPTH_SCALE );
}



#if MAX_WIDTH > MAX_HEIGHT
#  define MAXPOINTS MAX_WIDTH
#else
#  define MAXPOINTS MAX_HEIGHT
#endif


/* Flat, color index line */
static void flat_ci_line( GLcontext *ctx,
                          GLuint vert0, GLuint vert1, GLuint pvert )
{
   GLint count;
   GLint *pbx = ctx->PB->x;
   GLint *pby = ctx->PB->y;
   PB_SET_INDEX( ctx, ctx->PB, ctx->VB->Index[pvert] );
   count = ctx->PB->count;

#define INTERP_XY 1

#define PLOT(X,Y)		\
	pbx[count] = X;		\
	pby[count] = Y;		\
	count++;

#include "linetemp.h"

   ctx->PB->count = count;
   PB_CHECK_FLUSH( ctx, ctx->PB );
}



/* Flat, color index line with Z interpolation/testing */
static void flat_ci_z_line( GLcontext *ctx,
                            GLuint vert0, GLuint vert1, GLuint pvert )
{
   GLint count;
   GLint *pbx = ctx->PB->x;
   GLint *pby = ctx->PB->y;
   GLdepth *pbz = ctx->PB->z;
   PB_SET_INDEX( ctx, ctx->PB, ctx->VB->Index[pvert] );
   count = ctx->PB->count;

#define INTERP_XY 1
#define INTERP_Z 1

#define PLOT(X,Y)		\
	pbx[count] = X;		\
	pby[count] = Y;		\
	pbz[count] = Z;		\
	count++;

#include "linetemp.h"

   ctx->PB->count = count;
   PB_CHECK_FLUSH( ctx, ctx->PB );
}



/* Flat-shaded, RGBA line */
static void flat_rgba_line( GLcontext *ctx,
                            GLuint vert0, GLuint vert1, GLuint pvert )
{
   GLint count;
   GLint *pbx = ctx->PB->x;
   GLint *pby = ctx->PB->y;
   GLubyte *color = ctx->VB->Color[pvert];
   PB_SET_COLOR( ctx, ctx->PB, color[0], color[1], color[2], color[3] );
   count = ctx->PB->count;

#define INTERP_XY 1

#define PLOT(X,Y)		\
	pbx[count] = X;		\
	pby[count] = Y;		\
	count++;

#include "linetemp.h"

   ctx->PB->count = count;
   PB_CHECK_FLUSH( ctx, ctx->PB );
}



/* Flat-shaded, RGBA line with Z interpolation/testing */
static void flat_rgba_z_line( GLcontext *ctx,
                              GLuint vert0, GLuint vert1, GLuint pvert )
{
   GLint count;
   GLint *pbx = ctx->PB->x;
   GLint *pby = ctx->PB->y;
   GLdepth *pbz = ctx->PB->z;
   GLubyte *color = ctx->VB->Color[pvert];
   PB_SET_COLOR( ctx, ctx->PB, color[0], color[1], color[2], color[3] );
   count = ctx->PB->count;

#define INTERP_XY 1
#define INTERP_Z 1

#define PLOT(X,Y)	\
	pbx[count] = X;	\
	pby[count] = Y;	\
	pbz[count] = Z;	\
	count++;

#include "linetemp.h"

   ctx->PB->count = count;
   PB_CHECK_FLUSH( ctx, ctx->PB );
}



/* Smooth shaded, color index line */
static void smooth_ci_line( GLcontext *ctx,
                            GLuint vert0, GLuint vert1, GLuint pvert )
{
   GLint count = ctx->PB->count;
   GLint *pbx = ctx->PB->x;
   GLint *pby = ctx->PB->y;
   GLuint *pbi = ctx->PB->i;
   (void) pvert;

#define INTERP_XY 1
#define INTERP_INDEX 1

#define PLOT(X,Y)		\
	pbx[count] = X;		\
	pby[count] = Y;		\
	pbi[count] = I;		\
	count++;

#include "linetemp.h"

   ctx->PB->count = count;
   PB_CHECK_FLUSH( ctx, ctx->PB );
}



/* Smooth shaded, color index line with Z interpolation/testing */
static void smooth_ci_z_line( GLcontext *ctx,
                              GLuint vert0, GLuint vert1, GLuint pvert )
{
   GLint count = ctx->PB->count;
   GLint *pbx = ctx->PB->x;
   GLint *pby = ctx->PB->y;
   GLdepth *pbz = ctx->PB->z;
   GLuint *pbi = ctx->PB->i;
   (void) pvert;

#define INTERP_XY 1
#define INTERP_Z 1
#define INTERP_INDEX 1

#define PLOT(X,Y)		\
	pbx[count] = X;		\
	pby[count] = Y;		\
	pbz[count] = Z;		\
	pbi[count] = I;		\
	count++;

#include "linetemp.h"

   ctx->PB->count = count;
   PB_CHECK_FLUSH( ctx, ctx->PB );
}



/* Smooth-shaded, RGBA line */
static void smooth_rgba_line( GLcontext *ctx,
                       	      GLuint vert0, GLuint vert1, GLuint pvert )
{
   GLint count = ctx->PB->count;
   GLint *pbx = ctx->PB->x;
   GLint *pby = ctx->PB->y;
   GLubyte (*pbrgba)[4] = ctx->PB->rgba;
   (void) pvert;

#define INTERP_XY 1
#define INTERP_RGB 1
#define INTERP_ALPHA 1

#define PLOT(X,Y)			\
	pbx[count] = X;			\
	pby[count] = Y;			\
	pbrgba[count][RCOMP] = FixedToInt(r0);	\
	pbrgba[count][GCOMP] = FixedToInt(g0);	\
	pbrgba[count][BCOMP] = FixedToInt(b0);	\
	pbrgba[count][ACOMP] = FixedToInt(a0);	\
	count++;

#include "linetemp.h"

   ctx->PB->count = count;
   PB_CHECK_FLUSH( ctx, ctx->PB );
}



/* Smooth-shaded, RGBA line with Z interpolation/testing */
static void smooth_rgba_z_line( GLcontext *ctx,
                       	        GLuint vert0, GLuint vert1, GLuint pvert )
{
   GLint count = ctx->PB->count;
   GLint *pbx = ctx->PB->x;
   GLint *pby = ctx->PB->y;
   GLdepth *pbz = ctx->PB->z;
   GLubyte (*pbrgba)[4] = ctx->PB->rgba;
   (void) pvert;

#define INTERP_XY 1
#define INTERP_Z 1
#define INTERP_RGB 1
#define INTERP_ALPHA 1

#define PLOT(X,Y)			\
	pbx[count] = X;			\
	pby[count] = Y;			\
	pbz[count] = Z;			\
	pbrgba[count][RCOMP] = FixedToInt(r0);	\
	pbrgba[count][GCOMP] = FixedToInt(g0);	\
	pbrgba[count][BCOMP] = FixedToInt(b0);	\
	pbrgba[count][ACOMP] = FixedToInt(a0);	\
	count++;

#include "linetemp.h"

   ctx->PB->count = count;
   PB_CHECK_FLUSH( ctx, ctx->PB );
}


#define CHECK_FULL(count)			\
	if (count >= PB_SIZE-MAX_WIDTH) {	\
	   ctx->PB->count = count;		\
	   gl_flush_pb(ctx);			\
	   count = ctx->PB->count;		\
	    }



/* Smooth shaded, color index, any width, maybe stippled */
static void general_smooth_ci_line( GLcontext *ctx,
                           	    GLuint vert0, GLuint vert1, GLuint pvert )
{
   GLint count = ctx->PB->count;
   GLint *pbx = ctx->PB->x;
   GLint *pby = ctx->PB->y;
   GLdepth *pbz = ctx->PB->z;
   GLuint *pbi = ctx->PB->i;
   (void) pvert;

   if (ctx->Line.StippleFlag) {
      /* stippled */
#define INTERP_XY 1
#define INTERP_Z 1
#define INTERP_INDEX 1
#define WIDE 1
#define STIPPLE 1
#define PLOT(X,Y)		\
	pbx[count] = X;		\
	pby[count] = Y;		\
	pbz[count] = Z;		\
	pbi[count] = I;		\
	count++;		\
	CHECK_FULL(count);
#include "linetemp.h"
   }
   else {
      /* unstippled */
      if (ctx->Line.Width==2.0F) {
         /* special case: unstippled and width=2 */
#define INTERP_XY 1
#define INTERP_Z 1
#define INTERP_INDEX 1
#define XMAJOR_PLOT(X,Y)			\
	pbx[count] = X;  pbx[count+1] = X;	\
	pby[count] = Y;  pby[count+1] = Y+1;	\
	pbz[count] = Z;  pbz[count+1] = Z;	\
	pbi[count] = I;  pbi[count+1] = I;	\
	count += 2;				\
	CHECK_FULL(count);
#define YMAJOR_PLOT(X,Y)			\
	pbx[count] = X;  pbx[count+1] = X+1;	\
	pby[count] = Y;  pby[count+1] = Y;	\
	pbz[count] = Z;  pbz[count+1] = Z;	\
	pbi[count] = I;  pbi[count+1] = I;	\
	count += 2;				\
	CHECK_FULL(count);
#include "linetemp.h"
      }
      else {
         /* unstippled, any width */
#define INTERP_XY 1
#define INTERP_Z 1
#define INTERP_INDEX 1
#define WIDE 1
#define PLOT(X,Y)		\
	pbx[count] = X;		\
	pby[count] = Y;		\
	pbz[count] = Z;		\
	pbi[count] = I;		\
	count++;		\
	CHECK_FULL(count);
#include "linetemp.h"
      }
   }

   ctx->PB->count = count;
   PB_CHECK_FLUSH( ctx, ctx->PB );
}


/* Flat shaded, color index, any width, maybe stippled */
static void general_flat_ci_line( GLcontext *ctx,
                                  GLuint vert0, GLuint vert1, GLuint pvert )
{
   GLint count;
   GLint *pbx = ctx->PB->x;
   GLint *pby = ctx->PB->y;
   GLdepth *pbz = ctx->PB->z;
   PB_SET_INDEX( ctx, ctx->PB, ctx->VB->Index[pvert] );
   count = ctx->PB->count;

   if (ctx->Line.StippleFlag) {
      /* stippled, any width */
#define INTERP_XY 1
#define INTERP_Z 1
#define WIDE 1
#define STIPPLE 1
#define PLOT(X,Y)		\
	pbx[count] = X;		\
	pby[count] = Y;		\
	pbz[count] = Z;		\
	count++;		\
	CHECK_FULL(count);
#include "linetemp.h"
   }
   else {
      /* unstippled */
   if (ctx->Line.Width==2.0F) {
         /* special case: unstippled and width=2 */
#define INTERP_XY 1
#define INTERP_Z 1
#define XMAJOR_PLOT(X,Y)			\
	pbx[count] = X;  pbx[count+1] = X;	\
	pby[count] = Y;  pby[count+1] = Y+1;	\
	pbz[count] = Z;  pbz[count+1] = Z;	\
	count += 2;				\
	CHECK_FULL(count);
#define YMAJOR_PLOT(X,Y)			\
	pbx[count] = X;  pbx[count+1] = X+1;	\
	pby[count] = Y;  pby[count+1] = Y;	\
	pbz[count] = Z;  pbz[count+1] = Z;	\
	count += 2;				\
	CHECK_FULL(count);
#include "linetemp.h"
      }
      else {
         /* unstippled, any width */
#define INTERP_XY 1
#define INTERP_Z 1
#define WIDE 1
#define PLOT(X,Y)		\
	pbx[count] = X;		\
	pby[count] = Y;		\
	pbz[count] = Z;		\
	count++;		\
	CHECK_FULL(count);
#include "linetemp.h"
      }
   }

   ctx->PB->count = count;
   PB_CHECK_FLUSH( ctx, ctx->PB );
}



static void general_smooth_rgba_line( GLcontext *ctx,
                                      GLuint vert0, GLuint vert1, GLuint pvert)
{
   GLint count = ctx->PB->count;
   GLint *pbx = ctx->PB->x;
   GLint *pby = ctx->PB->y;
   GLdepth *pbz = ctx->PB->z;
   GLubyte (*pbrgba)[4] = ctx->PB->rgba;
   (void) pvert;

   if (ctx->Line.StippleFlag) {
      /* stippled */
#define INTERP_XY 1
#define INTERP_Z 1
#define INTERP_RGB 1
#define INTERP_ALPHA 1
#define WIDE 1
#define STIPPLE 1
#define PLOT(X,Y)			\
	pbx[count] = X;			\
	pby[count] = Y;			\
	pbz[count] = Z;			\
	pbrgba[count][RCOMP] = FixedToInt(r0);	\
	pbrgba[count][GCOMP] = FixedToInt(g0);	\
	pbrgba[count][BCOMP] = FixedToInt(b0);	\
	pbrgba[count][ACOMP] = FixedToInt(a0);	\
	count++;			\
	CHECK_FULL(count);
#include "linetemp.h"
   }
   else {
      /* unstippled */
      if (ctx->Line.Width==2.0F) {
         /* special case: unstippled and width=2 */
#define INTERP_XY 1
#define INTERP_Z 1
#define INTERP_RGB 1
#define INTERP_ALPHA 1
#define XMAJOR_PLOT(X,Y)						\
	pbx[count] = X;  pbx[count+1] = X;				\
	pby[count] = Y;  pby[count+1] = Y+1;				\
	pbz[count] = Z;  pbz[count+1] = Z;				\
	pbrgba[count][RCOMP] = FixedToInt(r0);		\
	pbrgba[count][GCOMP] = FixedToInt(g0);		\
	pbrgba[count][BCOMP] = FixedToInt(b0);		\
	pbrgba[count][ACOMP] = FixedToInt(a0);		\
	pbrgba[count+1][RCOMP] = FixedToInt(r0);	\
	pbrgba[count+1][GCOMP] = FixedToInt(g0);	\
	pbrgba[count+1][BCOMP] = FixedToInt(b0);	\
	pbrgba[count+1][ACOMP] = FixedToInt(a0);	\
	count += 2;					\
	CHECK_FULL(count);
#define YMAJOR_PLOT(X,Y)						\
	pbx[count] = X;  pbx[count+1] = X+1;				\
	pby[count] = Y;  pby[count+1] = Y;				\
	pbz[count] = Z;  pbz[count+1] = Z;				\
	pbrgba[count][RCOMP] = FixedToInt(r0);		\
	pbrgba[count][GCOMP] = FixedToInt(g0);		\
	pbrgba[count][BCOMP] = FixedToInt(b0);		\
	pbrgba[count][ACOMP] = FixedToInt(a0);		\
	pbrgba[count+1][RCOMP] = FixedToInt(r0);	\
	pbrgba[count+1][GCOMP] = FixedToInt(g0);	\
	pbrgba[count+1][BCOMP] = FixedToInt(b0);	\
	pbrgba[count+1][ACOMP] = FixedToInt(a0);	\
	count += 2;					\
	CHECK_FULL(count);
#include "linetemp.h"
      }
      else {
         /* unstippled, any width */
#define INTERP_XY 1
#define INTERP_Z 1
#define INTERP_RGB 1
#define INTERP_ALPHA 1
#define WIDE 1
#define PLOT(X,Y)			\
	pbx[count] = X;			\
	pby[count] = Y;			\
	pbz[count] = Z;			\
	pbrgba[count][RCOMP] = FixedToInt(r0);	\
	pbrgba[count][GCOMP] = FixedToInt(g0);	\
	pbrgba[count][BCOMP] = FixedToInt(b0);	\
	pbrgba[count][ACOMP] = FixedToInt(a0);	\
	count++;			\
	CHECK_FULL(count);
#include "linetemp.h"
      }
   }

   ctx->PB->count = count;
   PB_CHECK_FLUSH( ctx, ctx->PB );
}


static void general_flat_rgba_line( GLcontext *ctx,
                                    GLuint vert0, GLuint vert1, GLuint pvert )
{
   GLint count;
   GLint *pbx = ctx->PB->x;
   GLint *pby = ctx->PB->y;
   GLdepth *pbz = ctx->PB->z;
   GLubyte *color = ctx->VB->Color[pvert];
   PB_SET_COLOR( ctx, ctx->PB, color[0], color[1], color[2], color[3] );
   count = ctx->PB->count;

   if (ctx->Line.StippleFlag) {
      /* stippled */
#define INTERP_XY 1
#define INTERP_Z 1
#define WIDE 1
#define STIPPLE 1
#define PLOT(X,Y)			\
	pbx[count] = X;			\
	pby[count] = Y;			\
	pbz[count] = Z;			\
	count++;			\
	CHECK_FULL(count);
#include "linetemp.h"
   }
   else {
      /* unstippled */
   if (ctx->Line.Width==2.0F) {
         /* special case: unstippled and width=2 */
#define INTERP_XY 1
#define INTERP_Z 1
#define XMAJOR_PLOT(X,Y)			\
	pbx[count] = X;  pbx[count+1] = X;	\
	pby[count] = Y;  pby[count+1] = Y+1;	\
	pbz[count] = Z;  pbz[count+1] = Z;	\
	count += 2;				\
	CHECK_FULL(count);
#define YMAJOR_PLOT(X,Y)			\
	pbx[count] = X;  pbx[count+1] = X+1;	\
	pby[count] = Y;  pby[count+1] = Y;	\
	pbz[count] = Z;  pbz[count+1] = Z;	\
	count += 2;				\
	CHECK_FULL(count);
#include "linetemp.h"
      }
      else {
         /* unstippled, any width */
#define INTERP_XY 1
#define INTERP_Z 1
#define WIDE 1
#define PLOT(X,Y)			\
	pbx[count] = X;			\
	pby[count] = Y;			\
	pbz[count] = Z;			\
	count++;			\
	CHECK_FULL(count);
#include "linetemp.h"
	    }
	 }

   ctx->PB->count = count;
   PB_CHECK_FLUSH( ctx, ctx->PB );
}



/* Flat-shaded, textured, any width, maybe stippled */
static void flat_textured_line( GLcontext *ctx,
                                GLuint vert0, GLuint vert1, GLuint pv )
{
   GLint count;
   GLint *pbx = ctx->PB->x;
   GLint *pby = ctx->PB->y;
   GLdepth *pbz = ctx->PB->z;
   GLfloat *pbs = ctx->PB->s;
   GLfloat *pbt = ctx->PB->t;
   GLfloat *pbu = ctx->PB->u;
   GLubyte *color = ctx->VB->Color[pv];
   PB_SET_COLOR( ctx, ctx->PB, color[0], color[1], color[2], color[3] );
   count = ctx->PB->count;

   if (ctx->Line.StippleFlag) {
      /* stippled */
#define INTERP_XY 1
#define INTERP_Z 1
#define INTERP_STUV 1
#define WIDE 1
#define STIPPLE 1
#define PLOT(X,Y)			\
	{				\
	   GLfloat invQ = 1.0F / hv0;	\
	pbx[count] = X;			\
	pby[count] = Y;			\
	pbz[count] = Z;			\
	   pbs[count] = hs0 * invQ;	\
	   pbt[count] = ht0 * invQ;	\
	   pbu[count] = hu0 * invQ;	\
	count++;			\
	   CHECK_FULL(count);		\
	}
#include "linetemp.h"
   }
   else {
      /* unstippled */
#define INTERP_XY 1
#define INTERP_Z 1
#define INTERP_STUV 1
#define WIDE 1
#define PLOT(X,Y)			\
	{				\
	   GLfloat invQ = 1.0F / hv0;	\
	pbx[count] = X;			\
	pby[count] = Y;			\
	pbz[count] = Z;			\
	   pbs[count] = hs0 * invQ;	\
	   pbt[count] = ht0 * invQ;	\
	   pbu[count] = hu0 * invQ;	\
	count++;			\
	   CHECK_FULL(count);		\
	}
#include "linetemp.h"
   }

   ctx->PB->count = count;
   PB_CHECK_FLUSH( ctx, ctx->PB );
}



/* Smooth-shaded, textured, any width, maybe stippled */
static void smooth_textured_line( GLcontext *ctx,
                                  GLuint vert0, GLuint vert1, GLuint pvert )
{
   GLint count = ctx->PB->count;
   GLint *pbx = ctx->PB->x;
   GLint *pby = ctx->PB->y;
   GLdepth *pbz = ctx->PB->z;
   GLfloat *pbs = ctx->PB->s;
   GLfloat *pbt = ctx->PB->t;
   GLfloat *pbu = ctx->PB->u;
   GLubyte (*pbrgba)[4] = ctx->PB->rgba;
   (void) pvert;

   if (ctx->Line.StippleFlag) {
      /* stippled */
#define INTERP_XY 1
#define INTERP_Z 1
#define INTERP_RGB 1
#define INTERP_ALPHA 1
#define INTERP_STUV 1
#define WIDE 1
#define STIPPLE 1
#define PLOT(X,Y)			\
	{						\
	   GLfloat invQ = 1.0F / hv0;			\
	pbx[count] = X;			\
	pby[count] = Y;			\
	pbz[count] = Z;			\
	   pbs[count] = hs0 * invQ;			\
	   pbt[count] = ht0 * invQ;			\
	   pbu[count] = hu0 * invQ;			\
	   pbrgba[count][RCOMP] = FixedToInt(r0);	\
	   pbrgba[count][GCOMP] = FixedToInt(g0);	\
	   pbrgba[count][BCOMP] = FixedToInt(b0);	\
	   pbrgba[count][ACOMP] = FixedToInt(a0);	\
	count++;			\
	   CHECK_FULL(count);				\
	}
#include "linetemp.h"
   }
   else {
      /* unstippled */
#define INTERP_XY 1
#define INTERP_Z 1
#define INTERP_RGB 1
#define INTERP_ALPHA 1
#define INTERP_STUV 1
#define WIDE 1
#define PLOT(X,Y)			\
	{						\
	   GLfloat invQ = 1.0F / hv0;			\
	pbx[count] = X;			\
	pby[count] = Y;			\
	pbz[count] = Z;			\
	   pbs[count] = hs0 * invQ;			\
	   pbt[count] = ht0 * invQ;			\
	   pbu[count] = hu0 * invQ;			\
	   pbrgba[count][RCOMP] = FixedToInt(r0);	\
	   pbrgba[count][GCOMP] = FixedToInt(g0);	\
	   pbrgba[count][BCOMP] = FixedToInt(b0);	\
	   pbrgba[count][ACOMP] = FixedToInt(a0);	\
	count++;			\
	   CHECK_FULL(count);				\
	}
#include "linetemp.h"
   }

   ctx->PB->count = count;
   PB_CHECK_FLUSH( ctx, ctx->PB );
}



/*
 * Antialiased RGBA line
 *
 * This AA line function isn't terribly efficient but it's pretty
 * straight-forward to understand.  Also, it doesn't exactly conform
 * to the specification.
 */
static void aa_rgba_line( GLcontext *ctx,
                          GLuint vert0, GLuint vert1, GLuint pvert )
{
   struct vertex_buffer *VB = ctx->VB;
   struct pixel_buffer *pb = ctx->PB;
   GLfloat halfWidth = 0.5F * ctx->Line.Width;  /* 0.5 is a bit of a hack */
   GLboolean solid = !ctx->Line.StippleFlag;
   GLint x0 = (GLint) VB->Win[vert0][0], x1 = (GLint) VB->Win[vert1][0];
   GLint y0 = (GLint) VB->Win[vert0][1], y1 = (GLint) VB->Win[vert1][1];
   GLint dx = x1 - x0;
   GLint dy = y1 - y0;
   GLint xStep, yStep;
   GLint z0, z1;
   GLfixed fr, fg, fb, fa;      /* fixed-pt RGBA */
   GLfixed dfr, dfg, dfb, dfa;  /* fixed-pt RGBA deltas */

   if (dx == 0 && dy == 0)
      return;

#if DEPTH_BITS==16
   z0 = FloatToFixed(VB->Win[vert0][2]);
   z1 = FloatToFixed(VB->Win[vert1][2]);
#else
   z0 = (int) VB->Win[vert0][2];
   z1 = (int) VB->Win[vert1][2];
#endif

   if (ctx->Light.ShadeModel == GL_SMOOTH) {
      fr = IntToFixed(VB->Color[vert0][0]);
      fg = IntToFixed(VB->Color[vert0][1]);
      fb = IntToFixed(VB->Color[vert0][2]);
      fa = IntToFixed(VB->Color[vert0][3]);
   }
   else {
      fr = IntToFixed(VB->Color[pvert][0]);
      fg = IntToFixed(VB->Color[pvert][1]);
      fb = IntToFixed(VB->Color[pvert][2]);
      fa = IntToFixed(VB->Color[pvert][3]);
      dfr = dfg = dfb = dfa = 0;
   }

   if (dx < 0) {
      xStep = -1;
      dx = -dx;
   }
   else {
      xStep = 1;
   }

   if (dy < 0) {
      yStep = -1;
      dy = -dy;
   }
   else {
      yStep = 1;
   }

   if (dx > dy) {
      /* X-major line */
      GLint i;
      GLint x = x0;
      GLfloat y = VB->Win[vert0][1];
      GLfloat yStep = (VB->Win[vert1][1] - y) / (GLfloat) dx;
      GLint dz = (z1 - z0) / dx;
      if (ctx->Light.ShadeModel == GL_SMOOTH) {
         dfr = (IntToFixed(VB->Color[vert1][0]) - fr) / dx;
         dfg = (IntToFixed(VB->Color[vert1][1]) - fg) / dx;
         dfb = (IntToFixed(VB->Color[vert1][2]) - fb) / dx;
         dfa = (IntToFixed(VB->Color[vert1][3]) - fa) / dx;
      }
      for (i = 0; i < dx; i++) {
         if (solid || (ctx->Line.StipplePattern & (1 << ((ctx->StippleCounter/ctx->Line.StippleFactor) & 0xf)))) {

            GLfloat yTop = y + halfWidth;
            GLfloat yBot = y - halfWidth;
            GLint yTopi = (GLint) yTop;
            GLint yBoti = (GLint) yBot;
            GLint iy;
            GLint coverage;
            GLubyte red   = FixedToInt(fr);
            GLubyte green = FixedToInt(fg);
            GLubyte blue  = FixedToInt(fb);
            GLubyte alpha = FixedToInt(fa);
#if DEPTH_BITS==16
            GLdepth z = FixedToInt(z0);
#else
            GLdepth z = z0;
#endif
            ASSERT(yBoti < yTopi);

            {
               /* bottom pixel of swipe */
               coverage = (GLint) (alpha * (1.0F - (yBot - yBoti)));
               PB_WRITE_RGBA_PIXEL( pb, x, yBoti, z, red, green, blue, coverage );
               yBoti++;

               /* top pixel of swipe */
               coverage = (GLint) (alpha * (yTop - yTopi));
               PB_WRITE_RGBA_PIXEL( pb, x, yTopi, z, red, green, blue, coverage );
               yTopi--;

               /* pixels between top and bottom with 100% coverage */
               for (iy = yBoti; iy <= yTopi; iy++) {
                  PB_WRITE_RGBA_PIXEL( pb, x, iy, z, red, green, blue, alpha );
               }
            }

            PB_CHECK_FLUSH( ctx, pb );

         } /* if stippling */

         x += xStep;
         y += yStep;
         z0 += dz;
         fr += dfr;
         fg += dfg;
         fb += dfb;
         fa += dfa;
         if (!solid)
            ctx->StippleCounter++;
      }
   }
   else {
      /* Y-major line */
      GLint i;
      GLint y = y0;
      GLfloat x = VB->Win[vert0][0];
      GLfloat xStep = (VB->Win[vert1][0] - x) / (GLfloat) dy;
      GLint dz = (z1 - z0) / dy;
      if (ctx->Light.ShadeModel == GL_SMOOTH) {
         dfr = (IntToFixed(VB->Color[vert1][0]) - fr) / dy;
         dfg = (IntToFixed(VB->Color[vert1][1]) - fg) / dy;
         dfb = (IntToFixed(VB->Color[vert1][2]) - fb) / dy;
         dfa = (IntToFixed(VB->Color[vert1][3]) - fa) / dy;
      }
      for (i = 0; i < dy; i++) {
         if (solid || (ctx->Line.StipplePattern & (1 << ((ctx->StippleCounter/ctx->Line.StippleFactor) & 0xf)))) {
            GLfloat xRight = x + halfWidth;
            GLfloat xLeft = x - halfWidth;
            GLint xRighti = (GLint) xRight;
            GLint xLefti = (GLint) xLeft;
            GLint ix;
            GLint coverage;
            GLubyte red   = FixedToInt(fr);
            GLubyte green = FixedToInt(fg);
            GLubyte blue  = FixedToInt(fb);
            GLubyte alpha = FixedToInt(fa);
#if DEPTH_BITS==16
            GLdepth z = FixedToInt(z0);
#else
            GLdepth z = z0;
#endif

            ASSERT(xLefti < xRight);

            {
               /* left pixel of swipe */
               coverage = (GLint) (alpha * (1.0F - (xLeft - xLefti)));
               PB_WRITE_RGBA_PIXEL( pb, xLefti, y, z, red, green, blue, coverage );
               xLefti++;

               /* right pixel of swipe */
               coverage = (GLint) (alpha * (xRight - xRighti));
               PB_WRITE_RGBA_PIXEL( pb, xRighti, y, z, red, green, blue, coverage );
               xRighti--;

               /* pixels between top and bottom with 100% coverage */
               for (ix = xLefti; ix <= xRighti; ix++) {
                  PB_WRITE_RGBA_PIXEL( pb, ix, y, z, red, green, blue, alpha );
               }
            }

            PB_CHECK_FLUSH( ctx, pb );
         }

         x += xStep;
         y += yStep;
         z0 += dz;
         fr += dfr;
         fg += dfg;
         fb += dfb;
         fa += dfa;
         if (!solid)
            ctx->StippleCounter++;
      }
   }
}


/*
 * Antialiased Textured RGBA line
 *
 * This AA line function isn't terribly efficient but it's pretty
 * straight-forward to understand.  Also, it doesn't exactly conform
 * to the specification.
 */
static void aa_tex_rgba_line( GLcontext *ctx,
                              GLuint vert0, GLuint vert1, GLuint pvert )
{
   struct vertex_buffer *VB = ctx->VB;
   struct pixel_buffer *pb = ctx->PB;
   GLfloat halfWidth = 0.5F * ctx->Line.Width;  /* 0.5 is a bit of a hack */
   GLboolean solid = !ctx->Line.StippleFlag;
   GLint x0 = (GLint) VB->Win[vert0][0], x1 = (GLint) VB->Win[vert1][0];
   GLint y0 = (GLint) VB->Win[vert0][1], y1 = (GLint) VB->Win[vert1][1];
   GLint dx = x1 - x0;
   GLint dy = y1 - y0;
   GLint xStep, yStep;
   GLint z0, z1;
   GLfixed fr, dfr;
   GLfixed fg, dfg;
   GLfixed fb, dfb;
   GLfixed fa, dfa;
   GLfloat hs0, dhs;     /* h denotes hyperbolic */
   GLfloat ht0, dht;
   GLfloat hu0, dhu;
   GLfloat hv0, dhv;

   if (dx == 0 && dy == 0)
      return;

#if DEPTH_BITS==16
   z0 = FloatToFixed(VB->Win[vert0][2]);
   z1 = FloatToFixed(VB->Win[vert1][2]);
#else
   z0 = (int) VB->Win[vert0][2];
   z1 = (int) VB->Win[vert1][2];
#endif
   {
      GLfloat invw0 = 1.0F / VB->Clip[vert0][3];
      GLfloat invw1 = 1.0F / VB->Clip[vert1][3];
      hs0 = VB->TexCoord[vert0][0] * invw0;
      dhs = VB->TexCoord[vert1][0] * invw1 - hs0;
      ht0 = VB->TexCoord[vert0][1] * invw0;
      dht = VB->TexCoord[vert1][1] * invw1 - ht0;
      hu0 = VB->TexCoord[vert0][2] * invw0;
      dhu = VB->TexCoord[vert1][2] * invw1 - hu0;
      hv0 = VB->TexCoord[vert0][3] * invw0;
      dhv = VB->TexCoord[vert1][3] * invw1 - hv0;
   }

   if (ctx->Light.ShadeModel == GL_SMOOTH) {
      fr = IntToFixed(VB->Color[vert0][0]);
      fg = IntToFixed(VB->Color[vert0][1]);
      fb = IntToFixed(VB->Color[vert0][2]);
      fa = IntToFixed(VB->Color[vert0][3]);
   }
   else {
      fr = IntToFixed(VB->Color[pvert][0]);
      fg = IntToFixed(VB->Color[pvert][1]);
      fb = IntToFixed(VB->Color[pvert][2]);
      fa = IntToFixed(VB->Color[pvert][3]);
      dfr = dfg = dfb = dfa = 0;
   }

   if (dx < 0) {
      xStep = -1;
      dx = -dx;
   }
   else {
      xStep = 1;
   }

   if (dy < 0) {
      yStep = -1;
      dy = -dy;
   }
   else {
      yStep = 1;
   }

   if (dx > dy) {
      /* X-major line */
      GLint i;
      GLint x = x0;
      GLfloat y = VB->Win[vert0][1];
      GLfloat yStep = (VB->Win[vert1][1] - y) / (GLfloat) dx;
      GLint dz = (z1 - z0) / dx;
      GLfloat invDx = 1.0F / dx;
      if (ctx->Light.ShadeModel == GL_SMOOTH) {
         dfr = (IntToFixed(VB->Color[vert1][0]) - fr) / dx;
         dfg = (IntToFixed(VB->Color[vert1][1]) - fg) / dx;
         dfb = (IntToFixed(VB->Color[vert1][2]) - fb) / dx;
         dfa = (IntToFixed(VB->Color[vert1][3]) - fa) / dx;
      }
      dhs *= invDx;
      dht *= invDx;
      dhu *= invDx;
      dhv *= invDx;
      for (i = 0; i < dx; i++) {
         if (solid || (ctx->Line.StipplePattern & (1 << ((ctx->StippleCounter/ctx->Line.StippleFactor) & 0xf)))) {

            GLfloat yTop = y + halfWidth;
            GLfloat yBot = y - halfWidth;
            GLint yTopi = (GLint) yTop;
            GLint yBoti = (GLint) yBot;
            GLint iy;
            GLint coverage;
            GLubyte red   = FixedToInt(fr);
            GLubyte green = FixedToInt(fg);
            GLubyte blue  = FixedToInt(fb);
            GLubyte alpha = FixedToInt(fa);
#if DEPTH_BITS==16
            GLdepth z = FixedToInt(z0);
#else
            GLdepth z = z0;
#endif
            ASSERT(yBoti <= yTopi);

            {
               GLfloat invQ = 1.0F / hv0;
               GLfloat s = hs0 * invQ;
               GLfloat t = ht0 * invQ;
               GLfloat u = hu0 * invQ;

               /* bottom pixel of swipe */
               coverage = (GLint) (alpha * (1.0F - (yBot - yBoti)));
               PB_WRITE_TEX_PIXEL( pb, x, yBoti, z, red, green, blue, coverage,
                                   s, t, u );
               yBoti++;

               /* top pixel of swipe */
               coverage = (GLint) (alpha * (yTop - yTopi));
               PB_WRITE_TEX_PIXEL( pb, x, yTopi, z, red, green, blue, coverage,
                                   s, t, u );
               yTopi--;

               /* pixels between top and bottom with 100% coverage */
               for (iy = yBoti; iy <= yTopi; iy++) {
                  PB_WRITE_TEX_PIXEL( pb, x, iy, z, red, green, blue, alpha,
                                      s, t, u );
               }
            }
            PB_CHECK_FLUSH( ctx, pb );

         } /* if stippling */

         x += xStep;
         y += yStep;
         z0 += dz;
         fr += dfr;
         fg += dfg;
         fb += dfb;
         fa += dfa;
         hs0 += dhs;
         ht0 += dht;
         hu0 += dhu;
         hv0 += dhv;

         if (!solid)
            ctx->StippleCounter++;
      }
   }
   else {
      /* Y-major line */
      GLint i;
      GLint y = y0;
      GLfloat x = VB->Win[vert0][0];
      GLfloat xStep = (VB->Win[vert1][0] - x) / (GLfloat) dy;
      GLint dz = (z1 - z0) / dy;
      GLfloat invDy = 1.0F / dy;
      if (ctx->Light.ShadeModel == GL_SMOOTH) {
         dfr = (IntToFixed(VB->Color[vert1][0]) - fr) / dy;
         dfg = (IntToFixed(VB->Color[vert1][1]) - fg) / dy;
         dfb = (IntToFixed(VB->Color[vert1][2]) - fb) / dy;
         dfa = (IntToFixed(VB->Color[vert1][3]) - fa) / dy;
      }
      dhs *= invDy;
      dht *= invDy;
      dhu *= invDy;
      dhv *= invDy;
      for (i = 0; i < dy; i++) {
         if (solid || (ctx->Line.StipplePattern & (1 << ((ctx->StippleCounter/ctx->Line.StippleFactor) & 0xf)))) {
            GLfloat xRight = x + halfWidth;
            GLfloat xLeft = x - halfWidth;
            GLint xRighti = (GLint) xRight;
            GLint xLefti = (GLint) xLeft;
            GLint ix;
            GLint coverage;
            GLubyte red   = FixedToInt(fr);
            GLubyte green = FixedToInt(fg);
            GLubyte blue  = FixedToInt(fb);
            GLubyte alpha = FixedToInt(fa);
#if DEPTH_BITS==16
            GLdepth z = FixedToInt(z0);
#else
            GLdepth z = z0;
#endif

            ASSERT(xLefti < xRight);

            {
               GLfloat invQ = 1.0F / hv0;
               GLfloat s = hs0 * invQ;
               GLfloat t = ht0 * invQ;
               GLfloat u = hu0 * invQ;

               /* left pixel of swipe */
               coverage = (GLint) (alpha * (1.0F - (xLeft - xLefti)));
               PB_WRITE_TEX_PIXEL( pb, xLefti, y, z, red, green, blue, coverage,
                                   s, t, u );
               xLefti++;

               /* right pixel of swipe */
               coverage = (GLint) (alpha * (xRight - xRighti));
               PB_WRITE_TEX_PIXEL( pb, xRighti, y, z, red, green, blue, coverage,
                                   s, t, u );
               xRighti--;

               /* pixels between top and bottom with 100% coverage */
               for (ix = xLefti; ix <= xRighti; ix++) {
                  PB_WRITE_TEX_PIXEL( pb, ix, y, z, red, green, blue, alpha,
                                      s, t, u );
               }
            }
            PB_CHECK_FLUSH( ctx, pb );
         }

         x += xStep;
         y += yStep;
         z0 += dz;
         fr += dfr;
         fg += dfg;
         fb += dfb;
         fa += dfa;
         hs0 += dhs;
         ht0 += dht;
         hu0 += dhu;
         hv0 += dhv;
         if (!solid)
            ctx->StippleCounter++;
      }
   }
}




/*
 * Antialiased CI line.  Same comments for RGBA antialiased lines apply.
 */
static void aa_ci_line( GLcontext *ctx,
                        GLuint vert0, GLuint vert1, GLuint pvert )
{
   struct vertex_buffer *VB = ctx->VB;
   struct pixel_buffer *pb = ctx->PB;
   GLfloat halfWidth = 0.5F * ctx->Line.Width;  /* 0.5 is a bit of a hack */
   GLboolean solid = !ctx->Line.StippleFlag;
   GLint x0 = (GLint) VB->Win[vert0][0], x1 = (GLint) VB->Win[vert1][0];
   GLint y0 = (GLint) VB->Win[vert0][1], y1 = (GLint) VB->Win[vert1][1];
   GLint dx = x1 - x0;
   GLint dy = y1 - y0;
   GLint xStep, yStep;
   GLint z0, z1;
   GLfixed fi, dfi;

   if (dx == 0 && dy == 0)
      return;

#if DEPTH_BITS==16
   z0 = FloatToFixed(VB->Win[vert0][2]);
   z1 = FloatToFixed(VB->Win[vert1][2]);
#else
   z0 = (int) VB->Win[vert0][2];
   z1 = (int) VB->Win[vert1][2];
#endif

   if (ctx->Light.ShadeModel == GL_SMOOTH) {
      fi = IntToFixed(VB->Index[vert0]);
   }
   else {
      fi = IntToFixed(VB->Index[pvert]);
      dfi = 0;
   }

   if (dx < 0) {
      xStep = -1;
      dx = -dx;
   }
   else {
      xStep = 1;
   }

   if (dy < 0) {
      yStep = -1;
      dy = -dy;
   }
   else {
      yStep = 1;
   }

   if (dx > dy) {
      /* X-major line */
      GLint i;
      GLint x = x0;
      GLfloat y = VB->Win[vert0][1];
      GLfloat yStep = (VB->Win[vert1][1] - y) / (GLfloat) dx;
      GLint dz = (z1 - z0) / dx;
      if (ctx->Light.ShadeModel == GL_SMOOTH) {
         dfi = (IntToFixed(VB->Index[vert1]) - fi) / dx;
      }
      for (i = 0; i < dx; i++) {
         if (solid || (ctx->Line.StipplePattern & (1 << ((ctx->StippleCounter/ctx->Line.StippleFactor) & 0xf)))) {

            GLfloat yTop = y + halfWidth;
            GLfloat yBot = y - halfWidth;
            GLint yTopi = (GLint) yTop;
            GLint yBoti = (GLint) yBot;
            GLint iy;
            GLuint coverage;
            GLuint index = FixedToInt(fi) & 0xfffffff0;
#if DEPTH_BITS==16
            GLdepth z = FixedToInt(z0);
#else
            GLdepth z = z0;
#endif

            ASSERT(yBoti <= yTopi);

            /* bottom pixel of swipe */
            coverage = (GLuint) (15.0F * (1.0F - (yBot - yBoti)));
            PB_WRITE_CI_PIXEL( pb, x, yBoti, z, index + coverage );
            yBoti++;

            /* top pixel of swipe */
            coverage = (GLuint) (15.0F * (yTop - yTopi));
            PB_WRITE_CI_PIXEL( pb, x, yTopi, z, index + coverage );
            yTopi--;

            /* pixels between top and bottom with 100% coverage */
            for (iy = yBoti; iy <= yTopi; iy++) {
               PB_WRITE_CI_PIXEL( pb, x, iy, z, index + 15 );
            }
         }

         PB_CHECK_FLUSH( ctx, pb );

         x += xStep;
         y += yStep;
         z0 += dz;
         fi += dfi;
         if (!solid)
            ctx->StippleCounter++;
      }
   }
   else {
      /* Y-major line */
      GLint i;
      GLint y = y0;
      GLfloat x = VB->Win[vert0][0];
      GLfloat xStep = (VB->Win[vert1][0] - x) / (GLfloat) dy;
      GLint dz = (z1 - z0) / dy;
      if (ctx->Light.ShadeModel == GL_SMOOTH) {
         dfi = (IntToFixed(VB->Index[vert1]) - fi) / dy;
      }
      for (i = 0; i < dy; i++) {
         if (solid || (ctx->Line.StipplePattern & (1 << ((ctx->StippleCounter/ctx->Line.StippleFactor) & 0xf)))) {
            GLfloat xRight = x + halfWidth;
            GLfloat xLeft = x - halfWidth;
            GLint xRighti = (GLint) xRight;
            GLint xLefti = (GLint) xLeft;
            GLint ix;
            GLuint coverage;
            GLuint index = FixedToInt(fi) & 0xfffffff0;
#if DEPTH_BITS==16
            GLdepth z = FixedToInt(z0);
#else
            GLdepth z = z0;
#endif

            ASSERT(xLefti < xRight);

            /* left pixel of swipe */
            coverage = (GLuint) (15.0F * (1.0F - (xLeft - xLefti)));
            PB_WRITE_CI_PIXEL( pb, xLefti, y, z, index + coverage );
            xLefti++;

            /* right pixel of swipe */
            coverage = (GLuint) (15.0F * (xRight - xRighti));
            PB_WRITE_CI_PIXEL( pb, xRighti, y, z, index + coverage );
            xRighti--;

            /* pixels between top and bottom with 100% coverage */
            for (ix = xLefti; ix <= xRighti; ix++) {
               PB_WRITE_CI_PIXEL( pb, ix, y, z, index + 15 );
            }
         }

         PB_CHECK_FLUSH( ctx, pb );

         x += xStep;
         y += yStep;
         z0 += dz;
         fi += dfi;
         if (!solid)
            ctx->StippleCounter++;

      }
   }
}




/*
 * Null rasterizer for measuring transformation speed.
 */
static void null_line( GLcontext *ctx, GLuint v1, GLuint v2, GLuint pv )
{
   (void) ctx;
   (void) v1;
   (void) v2;
   (void) pv;
}



/*
 * Determine which line drawing function to use given the current
 * rendering context.
 */
void gl_set_line_function( GLcontext *ctx )
{
   GLboolean rgbmode = ctx->Visual->RGBAflag;
   /* TODO: antialiased lines */

   if (ctx->RenderMode==GL_RENDER) {
      if (ctx->NoRaster) {
         ctx->Driver.LineFunc = null_line;
         return;
      }
      if (ctx->Driver.LineFunc) {
         /* Device driver will draw lines. */
         ctx->Driver.LineFunc = ctx->Driver.LineFunc;
      }
      else if (ctx->Line.SmoothFlag) {
         /* antialiased lines */
         if (rgbmode) {
            if (ctx->Texture.Enabled)
               ctx->Driver.LineFunc = aa_tex_rgba_line;
            else
               ctx->Driver.LineFunc = aa_rgba_line;
         }
         else {
            ctx->Driver.LineFunc = aa_ci_line;
         }
      }
      else if (ctx->Texture.Enabled) {
         if (ctx->Light.ShadeModel==GL_SMOOTH) {
            ctx->Driver.LineFunc = smooth_textured_line;
         }
         else {
            ctx->Driver.LineFunc = flat_textured_line;
      }
      }
      else if (ctx->Line.Width!=1.0 || ctx->Line.StippleFlag
               || ctx->Line.SmoothFlag || ctx->Texture.Enabled) {
         if (ctx->Light.ShadeModel==GL_SMOOTH) {
            if (rgbmode)
               ctx->Driver.LineFunc = general_smooth_rgba_line;
            else
               ctx->Driver.LineFunc = general_smooth_ci_line;
         }
         else {
            if (rgbmode)
               ctx->Driver.LineFunc = general_flat_rgba_line;
            else
               ctx->Driver.LineFunc = general_flat_ci_line;
         }
      }
      else {
	 if (ctx->Light.ShadeModel==GL_SMOOTH) {
	    /* Width==1, non-stippled, smooth-shaded */
            if (ctx->Depth.Test
	        || (ctx->Fog.Enabled && ctx->Hint.Fog==GL_NICEST)) {
               if (rgbmode)
                  ctx->Driver.LineFunc = smooth_rgba_z_line;
               else
                  ctx->Driver.LineFunc = smooth_ci_z_line;
            }
            else {
               if (rgbmode)
                  ctx->Driver.LineFunc = smooth_rgba_line;
               else
                  ctx->Driver.LineFunc = smooth_ci_line;
            }
	 }
         else {
	    /* Width==1, non-stippled, flat-shaded */
            if (ctx->Depth.Test
                || (ctx->Fog.Enabled && ctx->Hint.Fog==GL_NICEST)) {
               if (rgbmode)
                  ctx->Driver.LineFunc = flat_rgba_z_line;
               else
                  ctx->Driver.LineFunc = flat_ci_z_line;
            }
            else {
               if (rgbmode)
                  ctx->Driver.LineFunc = flat_rgba_line;
               else
                  ctx->Driver.LineFunc = flat_ci_line;
            }
         }
      }
   }
   else if (ctx->RenderMode==GL_FEEDBACK) {
      ctx->Driver.LineFunc = feedback_line;
   }
   else {
      /* GL_SELECT mode */
      ctx->Driver.LineFunc = select_line;
   }
}

