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
 * Revision 3.1  1998/03/28 04:05:52  brianp
 * added CONST macro
 *
 * Revision 3.0  1998/01/31 20:45:56  brianp
 * initial rev
 *
 */


#ifndef ASM_386_H
#define ASM_386_H


#include "GL/gl.h"
#include "macros.h"


/*
 * Prototypes for assembly functions.
 */


extern void asm_transform_points3_general( GLuint n, GLfloat d[][4],
                                        GLfloat m[16], CONST GLfloat s[][4] );

extern void asm_transform_points3_identity( GLuint n, GLfloat d[][4],
                                            CONST GLfloat s[][4] );

extern void asm_transform_points3_2d( GLuint n, GLfloat d[][4],
                                      GLfloat m[16], CONST GLfloat s[][4] );

extern void asm_transform_points3_2d_no_rot( GLuint n, GLfloat d[][4],
                                         GLfloat m[16], CONST GLfloat s[][4] );

extern void asm_transform_points3_3d( GLuint n, GLfloat d[][4], GLfloat m[16],
                                      CONST GLfloat s[][4] );

extern void asm_transform_points4_general( GLuint n, GLfloat d[][4],
                                       GLfloat m[16], CONST GLfloat s[][4] );

extern void asm_transform_points4_identity( GLuint n, GLfloat d[][4],
                                            CONST GLfloat s[][4] );

extern void asm_transform_points4_2d( GLuint n, GLfloat d[][4], GLfloat m[16],
                                      CONST GLfloat s[][4] );

extern void asm_transform_points4_2d_no_rot( GLuint n, GLfloat d[][4],
                                      GLfloat m[16], CONST GLfloat s[][4] );

extern void asm_transform_points4_3d( GLuint n, GLfloat d[][4], GLfloat m[16],
                                      CONST GLfloat s[][4] );

extern void asm_transform_points4_ortho( GLuint n, GLfloat d[][4],
                                         GLfloat m[16], CONST GLfloat s[][4] );

extern void asm_transform_points4_perspective( GLuint n, GLfloat d[][4],
                                         GLfloat m[16], CONST GLfloat s[][4] );

extern void asm_project_and_cliptest_general( GLuint n, GLfloat d[][4],
                                      GLfloat m[16],
                                      CONST GLfloat s[][4], GLubyte clipmask[],
                                      GLubyte *ormask, GLubyte *andmask );

extern void asm_project_and_cliptest_identity( GLuint n, GLfloat d[][4],
                                     CONST GLfloat s[][4], GLubyte clipmask[],
                                     GLubyte *ormask, GLubyte *andmask );

extern void asm_project_and_cliptest_ortho( GLuint n, GLfloat d[][4],
                                     GLfloat m[16],
                                     CONST GLfloat s[][4], GLubyte clipmask[],
                                     GLubyte *ormask, GLubyte *andmask );

extern void asm_project_and_cliptest_perspective( GLuint n, GLfloat d[][4],
                                      GLfloat m[16],
                                      CONST GLfloat s[][4], GLubyte clipmask[],
                                      GLubyte *ormask, GLubyte *andmask );

#endif
