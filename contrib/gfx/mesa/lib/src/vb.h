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
 * Revision 3.4  1998/06/22 03:16:00  brianp
 * moved VB_MAX definition to config.h
 *
 * Revision 3.3  1998/06/21 02:02:23  brianp
 * added another comment
 *
 * Revision 3.2  1998/02/20 04:53:07  brianp
 * implemented GL_SGIS_multitexture
 *
 * Revision 3.1  1998/02/02 03:09:34  brianp
 * added GL_LIGHT_MODEL_COLOR_CONTROL (separate specular color interpolation)
 *
 * Revision 3.0  1998/01/31 21:06:45  brianp
 * initial rev
 *
 */


/*
 * OVERVIEW:  The vertices defined between glBegin() and glEnd()
 * are accumulated in the vertex buffer.  When either the
 * vertex buffer becomes filled or glEnd() is called, we must flush
 * the buffer.  That is, we apply the vertex transformations, compute
 * lighting, fog, texture coordinates etc.  Then, we can render the
 * vertices as points, lines or polygons by calling the gl_render_vb()
 * function in render.c
 *
 * When we're outside of a glBegin/glEnd pair the information in this
 * structure is irrelevant.
 */


#ifndef VB_H
#define VB_H


#include "types.h"



/*
 * Actual vertex buffer size.
 * Arrays must also accomodate new vertices from clipping.
 */
#define VB_SIZE  (VB_MAX + 2 * (6 + MAX_CLIP_PLANES))


/* Bit values for VertexSizeMask, below. */
/*#define VERTEX2_BIT  1*/
#define VERTEX3_BIT  2
#define VERTEX4_BIT  4



struct vertex_buffer {
        GLfloat Obj[VB_SIZE][4];        /* Object coords */
	GLfloat Eye[VB_SIZE][4];	/* Eye coords */
	GLfloat Clip[VB_SIZE][4];	/* Clip coords */
	GLfloat Win[VB_SIZE][3];	/* Window coords */

        GLfloat Normal[VB_SIZE][3];     /* Normal vectors */

   GLubyte Fcolor[VB_SIZE][4];	/* Front colors (RGBA) */
   GLubyte Bcolor[VB_SIZE][4];	/* Back colors (RGBA) */
   GLubyte (*Color)[4];		/* == Fcolor or Bcolor */

   GLubyte Fspec[VB_SIZE][4];	/* Front specular color (RGBA) */
   GLubyte Bspec[VB_SIZE][4];	/* Back specular color (RGBA) */
   GLubyte (*Specular)[4];	/* == Fspec or Bspec */

	GLuint Findex[VB_SIZE];         /* Front color indexes */
	GLuint Bindex[VB_SIZE];         /* Back color indexes */
	GLuint *Index;			/* == Findex or Bindex */

   GLboolean Edgeflag[VB_SIZE];	/* Polygon edge flags */

   GLfloat (*TexCoord)[4];	/* Points to one of the MultiTexCoord sets */
   GLfloat MultiTexCoord[MAX_TEX_SETS][VB_SIZE][4];/* Texture coords */

   GLubyte ClipMask[VB_SIZE];	/* bitwise-OR of CLIP_* values, below */
   GLubyte ClipOrMask;		/* bitwise-OR of all ClipMask[] values */
   GLubyte ClipAndMask;		/* bitwise-AND of all ClipMask[] values */

	GLuint Start;			/* First vertex to process */
	GLuint Count;			/* Number of vertexes in buffer */
	GLuint Free;			/* Next empty position for clipping */

   GLuint VertexSizeMask;	/* Bitwise-or of VERTEX[234]_BIT */
   GLuint TexCoordSize;		/* Either 2 or 4 */
   GLboolean MonoColor;		/* Do all vertices have same color? */
   GLboolean MonoNormal;	/* Do all vertices have same normal? */
   GLboolean MonoMaterial;	/* Do all vertices have same material? */

        /* to handle glMaterial calls inside glBegin/glEnd: */
   GLuint MaterialMask[VB_SIZE];	/* Which material values to change */
   struct gl_material Material[VB_SIZE][2]; /* New material values */
};



/* Vertex buffer clipping flags */
#define CLIP_RIGHT_BIT   0x01
#define CLIP_LEFT_BIT    0x02
#define CLIP_TOP_BIT     0x04
#define CLIP_BOTTOM_BIT  0x08
#define CLIP_NEAR_BIT    0x10
#define CLIP_FAR_BIT     0x20
#define CLIP_USER_BIT    0x40
#define CLIP_ALL_BITS    0x3f

#define CLIP_ALL   1
#define CLIP_NONE  2
#define CLIP_SOME  3


extern struct vertex_buffer *gl_alloc_vb(void);


#endif
