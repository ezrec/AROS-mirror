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
 * Revision 3.0  1998/01/31 21:03:42  brianp
 * initial rev
 *
 */


#ifndef STENCIL_H
#define STENCIL_H


#include "types.h"


extern void gl_ClearStencil( GLcontext *ctx, GLint s );


extern void gl_StencilFunc( GLcontext *ctx, GLenum func,
                            GLint ref, GLuint mask );


extern void gl_StencilMask( GLcontext *ctx, GLuint mask );


extern void gl_StencilOp( GLcontext *ctx, GLenum fail,
                          GLenum zfail, GLenum zpass );



extern GLint gl_stencil_span( GLcontext *ctx,
                              GLuint n, GLint x, GLint y, GLubyte mask[] );


extern void gl_depth_stencil_span( GLcontext *ctx, GLuint n, GLint x, GLint y,
				   const GLdepth z[], GLubyte mask[] );


extern GLint gl_stencil_pixels( GLcontext *ctx,
                                GLuint n, const GLint x[], const GLint y[],
			        GLubyte mask[] );


extern void gl_depth_stencil_pixels( GLcontext *ctx,
                                     GLuint n, const GLint x[],
				     const GLint y[], const GLdepth z[],
				     GLubyte mask[] );


extern void gl_read_stencil_span( GLcontext *ctx,
                                  GLuint n, GLint x, GLint y,
				  GLubyte stencil[] );


extern void gl_write_stencil_span( GLcontext *ctx,
                                   GLuint n, GLint x, GLint y,
				   const GLubyte stencil[] );


extern void gl_alloc_stencil_buffer( GLcontext *ctx );


extern void gl_clear_stencil_buffer( GLcontext *ctx );


#endif
