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
 * Revision 3.4  1998/08/21 02:41:39  brianp
 * added gl_pack/unpack_polygon_stipple()
 *
 * Revision 3.3  1998/07/26 17:24:18  brianp
 * replaced const with CONST because of IRIX cc warning
 *
 * Revision 3.2  1998/07/17 03:24:53  brianp
 * added gl_pack_rgba_span()
 *
 * Revision 3.1  1998/02/08 20:21:22  brianp
 * added gl_unpack_bitmap()
 *
 * Revision 3.0  1998/01/31 20:54:19  brianp
 * initial rev
 *
 */


#ifndef IMAGE_H
#define IMAGE_H


#include "types.h"


extern void gl_flip_bytes( GLubyte *p, GLuint n );


extern void gl_swap2( GLushort *p, GLuint n );

extern void gl_swap4( GLuint *p, GLuint n );


extern GLint gl_sizeof_type( GLenum type );


extern GLint gl_components_in_format( GLenum format );


extern GLvoid *gl_pixel_addr_in_image(
                                const struct gl_pixelstore_attrib *packing,
                                const GLvoid *image, GLsizei width,
                                GLsizei height, GLenum format, GLenum type,
                                GLint img, GLint row, GLint column );


extern struct gl_image *gl_unpack_bitmap( GLcontext *ctx,
                                          GLsizei width, GLsizei height,
                                          const GLubyte *bitmap );


extern void gl_unpack_polygon_stipple( const GLcontext *ctx,
                                       const GLubyte *pattern,
                                       GLuint dest[32] );


extern void gl_pack_polygon_stipple( const GLcontext *ctx,
                                     const GLuint pattern[32],
                                     GLubyte *dest );


extern struct gl_image *gl_unpack_image( GLcontext *ctx,
                                  GLint width, GLint height,
                                  GLenum srcFormat, GLenum srcType,
                                  const GLvoid *pixels );


struct gl_image *gl_unpack_image3D( GLcontext *ctx,
                                    GLint width, GLint height,GLint depth,
                                    GLenum srcFormat, GLenum srcType,
                                    const GLvoid *pixels );


extern void gl_pack_rgba_span( const GLcontext *ctx,
                               GLuint n, CONST GLubyte rgba[][4],
                               GLenum format, GLenum type, GLvoid *dest);


extern void gl_free_image( struct gl_image *image );


extern GLboolean gl_image_error_test( GLcontext *ctx,
                                      const struct gl_image *image,
                                      const char *msg );


#endif
