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
 * Revision 1.1  2005/01/11 14:58:30  NicJA
 * AROSMesa 3.0
 *
 * - Based on the official mesa 3 code with major patches to the amigamesa driver code to get it working.
 * - GLUT not yet started (ive left the _old_ mesaaux, mesatk and demos in for this reason)
 * - Doesnt yet work - the _db functions seem to be writing the data incorrectly, and color picking also seems broken somewhat - giving most things a blue tinge (those that are currently working)
 *
 * Revision 3.1  1998/08/23 22:17:42  brianp
 * moved gl_DepthRange() to matrix.c
 *
 * Revision 3.0  1998/01/31 20:50:23  brianp
 * initial rev
 *
 */


#ifndef DEPTH_H
#define DEPTH_H


#include "types.h"


/*
 * Return the address of the Z-buffer value for window coordinate (x,y):
 */
#define Z_ADDRESS( CTX, X, Y )  \
            ((CTX)->Buffer->Depth + (CTX)->Buffer->Width * (Y) + (X))




extern GLuint
gl_depth_test_span_generic( GLcontext* ctx, GLuint n, GLint x, GLint y,
                            const GLdepth z[], GLubyte mask[] );

extern GLuint
gl_depth_test_span_less( GLcontext* ctx, GLuint n, GLint x, GLint y,
                         const GLdepth z[], GLubyte mask[] );

extern GLuint
gl_depth_test_span_greater( GLcontext* ctx, GLuint n, GLint x, GLint y,
                            const GLdepth z[], GLubyte mask[] );



extern void
gl_depth_test_pixels_generic( GLcontext* ctx,
                              GLuint n, const GLint x[], const GLint y[],
                              const GLdepth z[], GLubyte mask[] );

extern void
gl_depth_test_pixels_less( GLcontext* ctx,
                           GLuint n, const GLint x[], const GLint y[],
                           const GLdepth z[], GLubyte mask[] );

extern void
gl_depth_test_pixels_greater( GLcontext* ctx,
                              GLuint n, const GLint x[], const GLint y[],
                              const GLdepth z[], GLubyte mask[] );


extern void gl_read_depth_span_float( GLcontext* ctx,
                                      GLuint n, GLint x, GLint y,
                                      GLfloat depth[] );


extern void gl_read_depth_span_int( GLcontext* ctx, GLuint n, GLint x, GLint y,
                                    GLdepth depth[] );


extern void gl_alloc_depth_buffer( GLcontext* ctx );


extern void gl_clear_depth_buffer( GLcontext* ctx );


extern void gl_ClearDepth( GLcontext* ctx, GLclampd depth );

extern void gl_DepthFunc( GLcontext* ctx, GLenum func );

extern void gl_DepthMask( GLcontext* ctx, GLboolean flag );

#endif
