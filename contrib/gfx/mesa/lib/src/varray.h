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
 * Revision 3.3  1998/06/07 22:18:52  brianp
 * implemented GL_EXT_multitexture extension
 *
 * Revision 3.2  1998/02/20 04:53:07  brianp
 * implemented GL_SGIS_multitexture
 *
 * Revision 3.1  1998/02/01 20:05:10  brianp
 * added glDrawRangeElements()
 *
 * Revision 3.0  1998/01/31 21:06:45  brianp
 * initial rev
 *
 */


#ifndef VARRAY_H
#define VARRAY_H


#include "types.h"


extern void gl_VertexPointer( GLcontext *ctx,
                              GLint size, GLenum type, GLsizei stride,
                              const GLvoid *ptr );


extern void gl_NormalPointer( GLcontext *ctx,
                              GLenum type, GLsizei stride, const GLvoid *ptr );


extern void gl_ColorPointer( GLcontext *ctx,
                             GLint size, GLenum type, GLsizei stride,
                             const GLvoid *ptr );


extern void gl_IndexPointer( GLcontext *ctx,
                                GLenum type, GLsizei stride,
                                const GLvoid *ptr );


extern void gl_TexCoordPointer( GLcontext *ctx,
                                GLint size, GLenum type, GLsizei stride,
                                const GLvoid *ptr );

/* GL_SGIS_multitexture */
extern void gl_MultiTexCoordPointer( GLcontext *ctx, GLenum target,
                                     GLint size, GLenum type, GLsizei stride,
                                     const GLvoid *ptr );


/* GL_EXT_multitexture */
extern void gl_InterleavedTextureCoordSets( GLcontext *ctx, GLint );


extern void gl_EdgeFlagPointer( GLcontext *ctx,
                                GLsizei stride, const GLboolean *ptr );


extern void gl_GetPointerv( GLcontext *ctx, GLenum pname, GLvoid **params );


extern void gl_ArrayElement( GLcontext *ctx, GLint i );

extern void gl_save_ArrayElement( GLcontext *ctx, GLint i );


extern void gl_DrawArrays( GLcontext *ctx,
                           GLenum mode, GLint first, GLsizei count );

extern void gl_save_DrawArrays( GLcontext *ctx,
                                GLenum mode, GLint first, GLsizei count );


extern void gl_DrawElements( GLcontext *ctx,
                             GLenum mode, GLsizei count,
                             GLenum type, const GLvoid *indices );

extern void gl_save_DrawElements( GLcontext *ctx,
                                  GLenum mode, GLsizei count,
                                  GLenum type, const GLvoid *indices );


extern void gl_InterleavedArrays( GLcontext *ctx,
                                  GLenum format, GLsizei stride,
                                  const GLvoid *pointer );

extern void gl_save_InterleavedArrays( GLcontext *ctx,
                                       GLenum format, GLsizei stride,
                                       const GLvoid *pointer );


extern void gl_DrawRangeElements( GLcontext *ctx, GLenum mode, GLuint start,
                                  GLuint end, GLsizei count, GLenum type,
                                  const GLvoid *indices );

extern void gl_save_DrawRangeElements( GLcontext *ctx, GLenum mode,
                                       GLuint start, GLuint end, GLsizei count,
                                       GLenum type, const GLvoid *indices );


#endif



