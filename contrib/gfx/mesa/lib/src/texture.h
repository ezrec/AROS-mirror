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
 * Revision 3.2  1998/02/20 04:53:37  brianp
 * implemented GL_SGIS_multitexture
 *
 * Revision 3.1  1998/02/03 04:27:54  brianp
 * added texture lod clamping
 *
 * Revision 3.0  1998/01/31 21:04:38  brianp
 * initial rev
 *
 */


#ifndef TEXTURE_H
#define TEXTURE_H


#include "types.h"


extern void gl_texgen( GLcontext *ctx, GLint n,
                       GLfloat obj[][4], GLfloat eye[][4],
                       GLfloat normal[][3], GLfloat texcoord[][4],
                       GLuint textureSet);



extern void gl_set_texture_sampler( struct gl_texture_object *t );



extern void gl_texture_pixels( GLcontext *ctx, GLuint texSet, GLuint n,
                               const GLfloat s[], const GLfloat t[],
                               const GLfloat r[], GLfloat lambda[],
                               GLubyte rgba[][4] );


#endif

