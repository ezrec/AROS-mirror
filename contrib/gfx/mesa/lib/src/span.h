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
 * Revision 3.4  1998/03/28 03:57:13  brianp
 * added CONST macro to fix IRIX compilation problems
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


#ifndef SPAN_H
#define SPAN_H


#include "types.h"


extern void gl_write_index_span( GLcontext *ctx,
                                 GLuint n, GLint x, GLint y, const GLdepth z[],
				 GLuint index[], GLenum primitive );


extern void gl_write_monoindex_span( GLcontext *ctx,
                                     GLuint n, GLint x, GLint y,
                                     const GLdepth z[],
				     GLuint index, GLenum primitive );


extern void gl_write_rgba_span( GLcontext *ctx,
                                GLuint n, GLint x, GLint y, const GLdepth z[],
                                GLubyte rgba[][4], GLenum primitive );


extern void gl_write_monocolor_span( GLcontext *ctx,
                                     GLuint n, GLint x, GLint y,
                                     const GLdepth z[],
				     GLint r, GLint g, GLint b, GLint a,
                                     GLenum primitive );


extern void gl_write_texture_span( GLcontext *ctx,
                                   GLuint n, GLint x, GLint y,
                                   const GLdepth z[],
				   const GLfloat s[], const GLfloat t[],
                                   const GLfloat u[], GLfloat lambda[],
				   GLubyte rgba[][4], CONST GLubyte spec[][4],
                                   GLenum primitive );


extern void gl_write_multitexture_span( GLcontext *ctx, GLuint texSets,
                                        GLuint n, GLint x, GLint y,
                                        const GLdepth z[],
                                        CONST GLfloat s[][MAX_WIDTH],
                                        CONST GLfloat t[][MAX_WIDTH],
                                        CONST GLfloat u[][MAX_WIDTH],
                                        GLfloat lambda[][MAX_WIDTH],
                                        GLubyte rgba[][4],
                                        CONST GLubyte spec[][4],
				   GLenum primitive );


extern void gl_read_rgba_span( GLcontext *ctx,
                                GLuint n, GLint x, GLint y,
                               GLubyte rgba[][4] );


extern void gl_read_index_span( GLcontext *ctx,
                                GLuint n, GLint x, GLint y, GLuint indx[] );


#endif
