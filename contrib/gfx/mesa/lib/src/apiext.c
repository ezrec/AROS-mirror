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
 * Revision 3.2  1998/06/07 22:18:52  brianp
 * implemented GL_EXT_multitexture extension
 *
 * Revision 3.1  1998/03/27 03:30:36  brianp
 * fixed G++ warnings
 *
 * Revision 3.0  1998/02/20 04:45:50  brianp
 * implemented GL_SGIS_multitexture
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include "api.h"
#include "context.h"
#include "types.h"
#endif



/*
 * Extension API functions
 */



/*
 * GL_EXT_blend_minmax
 */

void APIENTRY glBlendEquationEXT( GLenum mode )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.BlendEquation)(CC, mode);
}




/*
 * GL_EXT_blend_color
 */

void APIENTRY glBlendColorEXT( GLclampf red, GLclampf green,
                               GLclampf blue, GLclampf alpha )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.BlendColor)(CC, red, green, blue, alpha);
}




/*
 * GL_EXT_vertex_array
 */

void APIENTRY glVertexPointerEXT( GLint size, GLenum type, GLsizei stride,
                                  GLsizei count, const GLvoid *ptr )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.VertexPointer)(CC, size, type, stride, ptr);
   (void) count;
}


void APIENTRY glNormalPointerEXT( GLenum type, GLsizei stride, GLsizei count,
                                  const GLvoid *ptr )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.NormalPointer)(CC, type, stride, ptr);
   (void) count;
}


void APIENTRY glColorPointerEXT( GLint size, GLenum type, GLsizei stride,
                                 GLsizei count, const GLvoid *ptr )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ColorPointer)(CC, size, type, stride, ptr);
   (void) count;
}


void APIENTRY glIndexPointerEXT( GLenum type, GLsizei stride,
                                 GLsizei count, const GLvoid *ptr )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.IndexPointer)(CC, type, stride, ptr);
   (void) count;
}


void APIENTRY glTexCoordPointerEXT( GLint size, GLenum type, GLsizei stride,
                                    GLsizei count, const GLvoid *ptr )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexCoordPointer)(CC, size, type, stride, ptr);
   (void) count;
}


void APIENTRY glEdgeFlagPointerEXT( GLsizei stride, GLsizei count,
                                    const GLboolean *ptr )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.EdgeFlagPointer)(CC, stride, ptr);
   (void) count;
}


void APIENTRY glGetPointervEXT( GLenum pname, GLvoid **params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.GetPointerv)(CC, pname, params);
}


void APIENTRY glArrayElementEXT( GLint i )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ArrayElement)(CC, i);
}


void APIENTRY glDrawArraysEXT( GLenum mode, GLint first, GLsizei count )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.DrawArrays)(CC, mode, first, count);
}




/*
 * GL_EXT_texture_object
 */

GLboolean APIENTRY glAreTexturesResidentEXT( GLsizei n, const GLuint *textures,
                                             GLboolean *residences )
{
   return glAreTexturesResident( n, textures, residences );
}


void APIENTRY glBindTextureEXT( GLenum target, GLuint texture )
{
   glBindTexture( target, texture );
}


void APIENTRY glDeleteTexturesEXT( GLsizei n, const GLuint *textures)
{
   glDeleteTextures( n, textures );
}


void APIENTRY glGenTexturesEXT( GLsizei n, GLuint *textures )
{
   glGenTextures( n, textures );
}


GLboolean APIENTRY glIsTextureEXT( GLuint texture )
{
   return glIsTexture( texture );
}


void APIENTRY glPrioritizeTexturesEXT( GLsizei n, const GLuint *textures,
                                       const GLclampf *priorities )
{
   glPrioritizeTextures( n, textures, priorities );
}




/*
 * GL_EXT_texture3D
 */

void APIENTRY glCopyTexSubImage3DEXT( GLenum target, GLint level, GLint xoffset,
                                      GLint yoffset, GLint zoffset,
                                      GLint x, GLint y, GLsizei width,
                                      GLsizei height )
{
   glCopyTexSubImage3D(target, level, xoffset, yoffset, zoffset,
                       x, y, width, height);
}



void APIENTRY glTexImage3DEXT( GLenum target, GLint level, GLenum internalformat,
                               GLsizei width, GLsizei height, GLsizei depth,
                               GLint border, GLenum format, GLenum type,
                               const GLvoid *pixels )
{
   glTexImage3D(target, level, internalformat, width, height, depth,
                border, format, type, pixels);
}


void APIENTRY glTexSubImage3DEXT( GLenum target, GLint level, GLint xoffset,
                                  GLint yoffset, GLint zoffset, GLsizei width,
                                  GLsizei height, GLsizei depth, GLenum format,
                                  GLenum type, const GLvoid *pixels )
{
   glTexSubImage3D(target, level, xoffset, yoffset, zoffset,
                   width, height, depth, format, type, pixels);
}




/*
 * GL_EXT_point_parameters
 */

void APIENTRY glPointParameterfEXT( GLenum pname, GLfloat param )
{
   GLfloat params[3];
   GET_CONTEXT;
   CHECK_CONTEXT;
   params[0] = param;
   params[1] = 0.0;
   params[2] = 0.0;
   (*CC->API.PointParameterfvEXT)(CC, pname, params);
}


void APIENTRY glPointParameterfvEXT( GLenum pname, const GLfloat *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PointParameterfvEXT)(CC, pname, params);
}




#ifdef GL_MESA_window_pos
/*
 * Mesa implementation of glWindowPos*MESA()
 */
void APIENTRY glWindowPos4fMESA( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.WindowPos4fMESA)( CC, x, y, z, w );
}
#else
/* Implementation in winpos.c is used */
#endif


void APIENTRY glWindowPos2iMESA( GLint x, GLint y )
{
   glWindowPos4fMESA( (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}

void APIENTRY glWindowPos2sMESA( GLshort x, GLshort y )
{
   glWindowPos4fMESA( (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}

void APIENTRY glWindowPos2fMESA( GLfloat x, GLfloat y )
{
   glWindowPos4fMESA( x, y, 0.0F, 1.0F );
}

void APIENTRY glWindowPos2dMESA( GLdouble x, GLdouble y )
{
   glWindowPos4fMESA( (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}

void APIENTRY glWindowPos2ivMESA( const GLint *p )
{
   glWindowPos4fMESA( (GLfloat) p[0], (GLfloat) p[1], 0.0F, 1.0F );
}

void APIENTRY glWindowPos2svMESA( const GLshort *p )
{
   glWindowPos4fMESA( (GLfloat) p[0], (GLfloat) p[1], 0.0F, 1.0F );
}

void APIENTRY glWindowPos2fvMESA( const GLfloat *p )
{
   glWindowPos4fMESA( p[0], p[1], 0.0F, 1.0F );
}

void APIENTRY glWindowPos2dvMESA( const GLdouble *p )
{
   glWindowPos4fMESA( (GLfloat) p[0], (GLfloat) p[1], 0.0F, 1.0F );
}

void APIENTRY glWindowPos3iMESA( GLint x, GLint y, GLint z )
{
   glWindowPos4fMESA( (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}

void APIENTRY glWindowPos3sMESA( GLshort x, GLshort y, GLshort z )
{
   glWindowPos4fMESA( (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}

void APIENTRY glWindowPos3fMESA( GLfloat x, GLfloat y, GLfloat z )
{
   glWindowPos4fMESA( x, y, z, 1.0F );
}

void APIENTRY glWindowPos3dMESA( GLdouble x, GLdouble y, GLdouble z )
{
   glWindowPos4fMESA( (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}

void APIENTRY glWindowPos3ivMESA( const GLint *p )
{
   glWindowPos4fMESA( (GLfloat) p[0], (GLfloat) p[1], (GLfloat) p[2], 1.0F );
}

void APIENTRY glWindowPos3svMESA( const GLshort *p )
{
   glWindowPos4fMESA( (GLfloat) p[0], (GLfloat) p[1], (GLfloat) p[2], 1.0F );
}

void APIENTRY glWindowPos3fvMESA( const GLfloat *p )
{
   glWindowPos4fMESA( p[0], p[1], p[2], 1.0F );
}

void APIENTRY glWindowPos3dvMESA( const GLdouble *p )
{
   glWindowPos4fMESA( (GLfloat) p[0], (GLfloat) p[1], (GLfloat) p[2], 1.0F );
}

void APIENTRY glWindowPos4iMESA( GLint x, GLint y, GLint z, GLint w )
{
   glWindowPos4fMESA( (GLfloat) x, (GLfloat) y, (GLfloat) z, (GLfloat) w );
}

void APIENTRY glWindowPos4sMESA( GLshort x, GLshort y, GLshort z, GLshort w )
{
   glWindowPos4fMESA( (GLfloat) x, (GLfloat) y, (GLfloat) z, (GLfloat) w );
}

void APIENTRY glWindowPos4dMESA( GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
   glWindowPos4fMESA( (GLfloat) x, (GLfloat) y, (GLfloat) z, (GLfloat) w );
}


void APIENTRY glWindowPos4ivMESA( const GLint *p )
{
   glWindowPos4fMESA( (GLfloat) p[0], (GLfloat) p[1],
                      (GLfloat) p[2], (GLfloat) p[3] );
}

void APIENTRY glWindowPos4svMESA( const GLshort *p )
{
   glWindowPos4fMESA( (GLfloat) p[0], (GLfloat) p[1],
                      (GLfloat) p[2], (GLfloat) p[3] );
}

void APIENTRY glWindowPos4fvMESA( const GLfloat *p )
{
   glWindowPos4fMESA( p[0], p[1], p[2], p[3] );
}

void APIENTRY glWindowPos4dvMESA( const GLdouble *p )
{
   glWindowPos4fMESA( (GLfloat) p[0], (GLfloat) p[1],
                      (GLfloat) p[2], (GLfloat) p[3] );
}




/*
 * GL_MESA_resize_buffers
 */

/*
 * Called by user application when window has been resized.
 */
void APIENTRY glResizeBuffersMESA( void )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ResizeBuffersMESA)( CC );
}



/*
 * GL_SGIS_multitexture
 */

void APIENTRY glMultiTexCoord1dSGIS(GLenum target, GLdouble s)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, 0.0, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord1dvSGIS(GLenum target, const GLdouble *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], 0.0, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord1fSGIS(GLenum target, GLfloat s)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, 0.0, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord1fvSGIS(GLenum target, const GLfloat *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], 0.0, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord1iSGIS(GLenum target, GLint s)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, 0.0, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord1ivSGIS(GLenum target, const GLint *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], 0.0, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord1sSGIS(GLenum target, GLshort s)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, 0.0, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord1svSGIS(GLenum target, const GLshort *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], 0.0, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord2dSGIS(GLenum target, GLdouble s, GLdouble t)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord2dvSGIS(GLenum target, const GLdouble *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord2fSGIS(GLenum target, GLfloat s, GLfloat t)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord2fvSGIS(GLenum target, const GLfloat *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord2iSGIS(GLenum target, GLint s, GLint t)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord2ivSGIS(GLenum target, const GLint *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord2sSGIS(GLenum target, GLshort s, GLshort t)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord2svSGIS(GLenum target, const GLshort *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord3dSGIS(GLenum target, GLdouble s, GLdouble t, GLdouble r)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, r, 1.0 );
}

void APIENTRY glMultiTexCoord3dvSGIS(GLenum target, const GLdouble *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], v[2], 1.0 );
}

void APIENTRY glMultiTexCoord3fSGIS(GLenum target, GLfloat s, GLfloat t, GLfloat r)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, r, 1.0 );
}

void APIENTRY glMultiTexCoord3fvSGIS(GLenum target, const GLfloat *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], v[2], 1.0 );
}

void APIENTRY glMultiTexCoord3iSGIS(GLenum target, GLint s, GLint t, GLint r)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, r, 1.0 );
}

void APIENTRY glMultiTexCoord3ivSGIS(GLenum target, const GLint *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], v[2], 1.0 );
}

void APIENTRY glMultiTexCoord3sSGIS(GLenum target, GLshort s, GLshort t, GLshort r)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, r, 1.0 );
}

void APIENTRY glMultiTexCoord3svSGIS(GLenum target, const GLshort *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], v[2], 1.0 );
}

void APIENTRY glMultiTexCoord4dSGIS(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, r, q );
}

void APIENTRY glMultiTexCoord4dvSGIS(GLenum target, const GLdouble *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], v[2], v[3] );
}

void APIENTRY glMultiTexCoord4fSGIS(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, r, q );
}

void APIENTRY glMultiTexCoord4fvSGIS(GLenum target, const GLfloat *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], v[2], v[3] );
}

void APIENTRY glMultiTexCoord4iSGIS(GLenum target, GLint s, GLint t, GLint r, GLint q)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, r, q );
}

void APIENTRY glMultiTexCoord4ivSGIS(GLenum target, const GLint *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], v[2], v[3] );
}

void APIENTRY glMultiTexCoord4sSGIS(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, r, q );
}

void APIENTRY glMultiTexCoord4svSGIS(GLenum target, const GLshort *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], v[2], v[3] );
}



void APIENTRY glMultiTexCoordPointerSGIS(GLenum target, GLint size, GLenum type,
                                         GLsizei stride, const GLvoid *ptr)
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.MultiTexCoordPointer)(CC, target, size, type, stride, ptr);
}



void APIENTRY glSelectTextureSGIS(GLenum target)
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.SelectTextureSGIS)(CC, target);
}



void APIENTRY glSelectTextureCoordSetSGIS(GLenum target)
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.SelectTextureCoordSet)(CC, target);
}




/*
 * GL_EXT_multitexture
 */

void APIENTRY glMultiTexCoord1dEXT(GLenum target, GLdouble s)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, 0.0, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord1dvEXT(GLenum target, const GLdouble *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], 0.0, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord1fEXT(GLenum target, GLfloat s)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, 0.0, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord1fvEXT(GLenum target, const GLfloat *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], 0.0, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord1iEXT(GLenum target, GLint s)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, 0.0, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord1ivEXT(GLenum target, const GLint *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], 0.0, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord1sEXT(GLenum target, GLshort s)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, 0.0, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord1svEXT(GLenum target, const GLshort *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], 0.0, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord2dEXT(GLenum target, GLdouble s, GLdouble t)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord2dvEXT(GLenum target, const GLdouble *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord2fEXT(GLenum target, GLfloat s, GLfloat t)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord2fvEXT(GLenum target, const GLfloat *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord2iEXT(GLenum target, GLint s, GLint t)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord2ivEXT(GLenum target, const GLint *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord2sEXT(GLenum target, GLshort s, GLshort t)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord2svEXT(GLenum target, const GLshort *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], 0.0, 1.0 );
}

void APIENTRY glMultiTexCoord3dEXT(GLenum target, GLdouble s, GLdouble t, GLdouble r)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, r, 1.0 );
}

void APIENTRY glMultiTexCoord3dvEXT(GLenum target, const GLdouble *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], v[2], 1.0 );
}

void APIENTRY glMultiTexCoord3fEXT(GLenum target, GLfloat s, GLfloat t, GLfloat r)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, r, 1.0 );
}

void APIENTRY glMultiTexCoord3fvEXT(GLenum target, const GLfloat *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], v[2], 1.0 );
}

void APIENTRY glMultiTexCoord3iEXT(GLenum target, GLint s, GLint t, GLint r)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, r, 1.0 );
}

void APIENTRY glMultiTexCoord3ivEXT(GLenum target, const GLint *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], v[2], 1.0 );
}

void APIENTRY glMultiTexCoord3sEXT(GLenum target, GLshort s, GLshort t, GLshort r)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, r, 1.0 );
}

void APIENTRY glMultiTexCoord3svEXT(GLenum target, const GLshort *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], v[2], 1.0 );
}

void APIENTRY glMultiTexCoord4dEXT(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, r, q );
}

void APIENTRY glMultiTexCoord4dvEXT(GLenum target, const GLdouble *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], v[2], v[3] );
}

void APIENTRY glMultiTexCoord4fEXT(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, r, q );
}

void APIENTRY glMultiTexCoord4fvEXT(GLenum target, const GLfloat *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], v[2], v[3] );
}

void APIENTRY glMultiTexCoord4iEXT(GLenum target, GLint s, GLint t, GLint r, GLint q)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, r, q );
}

void APIENTRY glMultiTexCoord4ivEXT(GLenum target, const GLint *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], v[2], v[3] );
}

void APIENTRY glMultiTexCoord4sEXT(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, s, t, r, q );
}

void APIENTRY glMultiTexCoord4svEXT(GLenum target, const GLshort *v)
{
   GET_CONTEXT;
   (*CC->API.MultiTexCoord4f)( CC, target, v[0], v[1], v[2], v[3] );
}



void APIENTRY glInterleavedTextureCoordSetsEXT( GLint factor )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.InterleavedTextureCoordSets)( CC, factor );
}



void APIENTRY glSelectTextureTransformEXT( GLenum target )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.SelectTextureTransform)( CC, target );
}



void APIENTRY glSelectTextureEXT( GLenum target )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.SelectTexture)( CC, target );
}



void APIENTRY glSelectTextureCoordSetEXT( GLenum target )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.SelectTextureCoordSet)( CC, target );
}

