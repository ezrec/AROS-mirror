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
 * Revision 3.4  1998/07/17 03:23:47  brianp
 * added a bunch of const's
 *
 * Revision 3.3  1998/03/22 16:42:05  brianp
 * added 8-bit CI->RGBA pixel mapping tables
 *
 * Revision 3.2  1998/02/08 20:21:42  brianp
 * added a bunch of rgba, ci and stencil scaling, biasing and mapping functions
 *
 * Revision 3.1  1998/02/01 22:15:39  brianp
 * moved pixel zoom code into zoom.h
 *
 * Revision 3.0  1998/01/31 21:00:28  brianp
 * initial rev
 *
 */


#ifndef PIXEL_H
#define PIXEL_H


#include "types.h"


/*
 * API functions
 */


extern void gl_GetPixelMapfv( GLcontext *ctx, GLenum map, GLfloat *values );

extern void gl_GetPixelMapuiv( GLcontext *ctx, GLenum map, GLuint *values );

extern void gl_GetPixelMapusv( GLcontext *ctx, GLenum map, GLushort *values );


extern void gl_PixelMapfv( GLcontext *ctx,
                           GLenum map, GLint mapsize, const GLfloat *values );

extern void gl_PixelStorei( GLcontext *ctx, GLenum pname, GLint param );

extern void gl_PixelTransferf( GLcontext *ctx, GLenum pname, GLfloat param );

extern void gl_PixelZoom( GLcontext *ctx, GLfloat xfactor, GLfloat yfactor );


/*
 * Pixel processing functions
 */

extern void gl_scale_and_bias_color( const GLcontext *ctx, GLuint n,
                                     GLfloat red[], GLfloat green[],
                                     GLfloat blue[], GLfloat alpha[] );


extern void gl_scale_and_bias_rgba( const GLcontext *ctx, GLuint n,
                                    GLubyte rgba[][4] );


extern void gl_map_rgba( const GLcontext *ctx, GLuint n, GLubyte rgba[][4] );


extern void gl_map_color( const GLcontext *ctx, GLuint n,
                          GLfloat red[], GLfloat green[],
                          GLfloat blue[], GLfloat alpha[] );


extern void gl_shift_and_offset_ci( const GLcontext *ctx, GLuint n,
                                    GLuint indexes[] );


extern void gl_map_ci( const GLcontext *ctx, GLuint n, GLuint index[] );


extern void gl_map_ci_to_rgba( const GLcontext *ctx,
                               GLuint n, const GLuint index[],
                               GLubyte rgba[][4] );


extern void gl_map_ci8_to_rgba( const GLcontext *ctx,
                                GLuint n, const GLubyte index[],
                                GLubyte rgba[][4] );


extern void gl_map_ci_to_color( const GLcontext *ctx,
                                GLuint n, const GLuint index[],
                                GLfloat r[], GLfloat g[],
                                GLfloat b[], GLfloat a[] );


extern void gl_shift_and_offset_stencil( const GLcontext *ctx, GLuint n,
                                         GLstencil indexes[] );


extern void gl_map_stencil( const GLcontext *ctx, GLuint n, GLstencil index[] );


#endif
