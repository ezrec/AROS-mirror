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
 * Revision 3.0  1998/01/31 21:04:38  brianp
 * initial rev
 *
 */


#ifndef TEXTOBJ_H
#define TEXTOBJ_H


#include "types.h"


/*
 * Internal functions
 */

extern struct gl_texture_object *
gl_alloc_texture_object( struct gl_shared_state *shared, GLuint name,
                         GLuint dimensions );


extern void gl_free_texture_object( struct gl_shared_state *shared,
                                    struct gl_texture_object *t );


extern void gl_test_texture_object_completeness( struct gl_texture_object *t );


/*
 * API functions
 */

extern void gl_GenTextures( GLcontext *ctx, GLsizei n, GLuint *textures );


extern void gl_DeleteTextures( GLcontext *ctx,
                               GLsizei n, const GLuint *textures);


extern void gl_BindTexture( GLcontext *ctx, GLenum target, GLuint texture );


extern void gl_PrioritizeTextures( GLcontext *ctx,
                                   GLsizei n, const GLuint *textures,
                                   const GLclampf *priorities );


extern GLboolean gl_AreTexturesResident( GLcontext *ctx, GLsizei n,
                                         const GLuint *textures,
                                         GLboolean *residences );


extern GLboolean gl_IsTexture( GLcontext *ctx, GLuint texture );


#endif
