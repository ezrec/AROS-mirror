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
 * Revision 3.1  1998/03/05 02:59:05  brianp
 * [RGBA]COMP wasn't being used in a few places
 *
 * Revision 3.0  1998/01/31 21:00:28  brianp
 * initial rev
 *
 */


#ifndef PB_H
#define PB_H


#include "types.h"



/*
 * Pixel buffer size, must be larger than MAX_WIDTH.
 */
#define PB_SIZE (3*MAX_WIDTH)


struct pixel_buffer {
	GLint x[PB_SIZE];	/* X window coord in [0,MAX_WIDTH) */
	GLint y[PB_SIZE];	/* Y window coord in [0,MAX_HEIGHT) */
	GLdepth z[PB_SIZE];	/* Z window coord in [0,MAX_DEPTH] */
	GLubyte rgba[PB_SIZE][4];/* Colors */
	GLuint i[PB_SIZE];	/* Index */
	GLfloat s[PB_SIZE];	/* Texture S coordinate */
	GLfloat t[PB_SIZE];	/* Texture T coordinate */
	GLfloat u[PB_SIZE];	/* Texture R coordinate */
	GLfloat lambda[PB_SIZE];/* Texture lambda values */
	GLint color[4];		/* Mono color, integers! */
	GLuint index;		/* Mono index */
	GLuint count;		/* Number of pixels in buffer */
	GLboolean mono;		/* Same color or index for all pixels? */
	GLenum primitive;	/* GL_POINT, GL_LINE, GL_POLYGON or GL_BITMAP*/
};


/*
 * Initialize the Pixel Buffer, specifying the type of primitive being drawn.
 */
#define PB_INIT( PB, PRIM )		\
	(PB)->count = 0;		\
	(PB)->mono = GL_FALSE;		\
	(PB)->primitive = (PRIM);



/*
 * Set the color used for all subsequent pixels in the buffer.
 */
#define PB_SET_COLOR( CTX, PB, R, G, B, A )		\
	if ((PB)->color[RCOMP]!=(R) || (PB)->color[GCOMP]!=(G)	\
	 || (PB)->color[BCOMP]!=(B) || (PB)->color[ACOMP]!=(A)	\
	 || !(PB)->mono) {				\
		gl_flush_pb( ctx );			\
	}						\
	(PB)->color[RCOMP] = R;					\
	(PB)->color[GCOMP] = G;					\
	(PB)->color[BCOMP] = B;					\
	(PB)->color[ACOMP] = A;					\
	(PB)->mono = GL_TRUE;


/*
 * Set the color index used for all subsequent pixels in the buffer.
 */
#define PB_SET_INDEX( CTX, PB, I )		\
	if ((PB)->index!=(I) || !(PB)->mono) {	\
		gl_flush_pb( CTX );		\
	}					\
	(PB)->index = I;			\
	(PB)->mono = GL_TRUE;


/*
 * "write" a pixel using current color or index
 */
#define PB_WRITE_PIXEL( PB, X, Y, Z )		\
	(PB)->x[(PB)->count] = X;		\
	(PB)->y[(PB)->count] = Y;		\
	(PB)->z[(PB)->count] = Z;		\
	(PB)->count++;


/*
 * "write" an RGBA pixel
 */
#define PB_WRITE_RGBA_PIXEL( PB, X, Y, Z, R, G, B, A )	\
	(PB)->x[(PB)->count] = X;			\
	(PB)->y[(PB)->count] = Y;			\
	(PB)->z[(PB)->count] = Z;			\
	(PB)->rgba[(PB)->count][RCOMP] = R;		\
	(PB)->rgba[(PB)->count][GCOMP] = G;		\
	(PB)->rgba[(PB)->count][BCOMP] = B;		\
	(PB)->rgba[(PB)->count][ACOMP] = A;		\
	(PB)->count++;

/*
 * "write" a color-index pixel
 */
#define PB_WRITE_CI_PIXEL( PB, X, Y, Z, I )	\
	(PB)->x[(PB)->count] = X;		\
	(PB)->y[(PB)->count] = Y;		\
	(PB)->z[(PB)->count] = Z;		\
	(PB)->i[(PB)->count] = I;		\
	(PB)->count++;


/*
 * "write" an RGBA pixel with texture coordinates
 */
#define PB_WRITE_TEX_PIXEL( PB, X, Y, Z, R, G, B, A, S, T, U )	\
	(PB)->x[(PB)->count] = X;				\
	(PB)->y[(PB)->count] = Y;				\
	(PB)->z[(PB)->count] = Z;				\
	(PB)->rgba[(PB)->count][RCOMP] = R;			\
	(PB)->rgba[(PB)->count][GCOMP] = G;			\
	(PB)->rgba[(PB)->count][BCOMP] = B;			\
	(PB)->rgba[(PB)->count][ACOMP] = A;			\
	(PB)->s[(PB)->count] = S;				\
	(PB)->t[(PB)->count] = T;				\
	(PB)->u[(PB)->count] = U;				\
	(PB)->count++;


/*
 * Call this function at least every MAX_WIDTH pixels:
 */
#define PB_CHECK_FLUSH( CTX, PB )		\
	if ((PB)->count>=PB_SIZE-MAX_WIDTH) {	\
	   gl_flush_pb( CTX );			\
	}


extern struct pixel_buffer *gl_alloc_pb(void);

extern void gl_flush_pb( GLcontext *ctx );

#endif
