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
 * Revision 3.3  1998/08/21 01:50:01  brianp
 * added gl_max_texture_coord_sets()
 *
 * Revision 3.2  1998/06/07 22:18:52  brianp
 * implemented GL_EXT_multitexture extension
 *
 * Revision 3.1  1998/02/20 04:53:37  brianp
 * implemented GL_SGIS_multitexture
 *
 * Revision 3.0  1998/01/31 21:04:38  brianp
 * initial rev
 *
 */


#ifndef TEXSTATE_H
#define TEXSTATE_H


#include "types.h"


/*** Called from API ***/

extern void gl_GetTexEnvfv( GLcontext *ctx,
                            GLenum target, GLenum pname, GLfloat *params );

extern void gl_GetTexEnviv( GLcontext *ctx,
                            GLenum target, GLenum pname, GLint *params );

extern void gl_GetTexGendv( GLcontext *ctx,
                            GLenum coord, GLenum pname, GLdouble *params );

extern void gl_GetTexGenfv( GLcontext *ctx,
                            GLenum coord, GLenum pname, GLfloat *params );

extern void gl_GetTexGeniv( GLcontext *ctx,
                            GLenum coord, GLenum pname, GLint *params );

extern void gl_GetTexLevelParameterfv( GLcontext *ctx,
                                       GLenum target, GLint level,
                                       GLenum pname, GLfloat *params );

extern void gl_GetTexLevelParameteriv( GLcontext *ctx,
                                       GLenum target, GLint level,
                                       GLenum pname, GLint *params );

extern void gl_GetTexParameterfv( GLcontext *ctx, GLenum target,
                                  GLenum pname, GLfloat *params );

extern void gl_GetTexParameteriv( GLcontext *ctx,
                                  GLenum target, GLenum pname, GLint *params );


extern void gl_TexEnvfv( GLcontext *ctx,
                         GLenum target, GLenum pname, const GLfloat *param );


extern void gl_TexParameterfv( GLcontext *ctx, GLenum target, GLenum pname,
                               const GLfloat *params );


extern void gl_TexGenfv( GLcontext *ctx,
                         GLenum coord, GLenum pname, const GLfloat *params );


/*
 * GL_SGIS_multitexture / GL_EXT_multitexture
 */

extern void gl_SelectTextureSGIS( GLcontext *ctx, GLenum target );

extern void gl_SelectTexture( GLcontext *ctx, GLenum target );

extern void gl_SelectTextureCoordSet( GLcontext *ctx, GLenum target );

extern void gl_SelectTextureTransform( GLcontext *ctx, GLenum target );



/*** Internal functions ***/

extern void gl_update_texture_state( GLcontext *ctx );

extern GLint gl_max_texture_coord_sets( const GLcontext *ctx );


#endif

