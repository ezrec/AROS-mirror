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
 * Revision 3.4  1998/08/21 02:42:21  brianp
 * changed gl_save_PolygonStipple() param type to GLuint *
 *
 * Revision 3.3  1998/06/07 22:18:52  brianp
 * implemented GL_EXT_multitexture extension
 *
 * Revision 3.2  1998/02/20 04:50:09  brianp
 * implemented GL_SGIS_multitexture
 *
 * Revision 3.1  1998/02/08 20:23:09  brianp
 * removed const from gl_save_Bitmap()'s bitmap parameter
 *
 * Revision 3.0  1998/01/31 20:50:39  brianp
 * initial rev
 *
 */


#ifndef DLIST_H
#define DLIST_H


#include "types.h"



extern void gl_init_lists( void );

extern void gl_destroy_list( GLcontext *ctx, GLuint list );



extern void gl_CallList( GLcontext *ctx, GLuint list );

extern void gl_CallLists( GLcontext *ctx,
                          GLsizei n, GLenum type, const GLvoid *lists );

extern void gl_DeleteLists( GLcontext *ctx, GLuint list, GLsizei range );

extern void gl_EndList( GLcontext *ctx );

extern GLuint gl_GenLists( GLcontext *ctx, GLsizei range );

extern GLboolean gl_IsList( GLcontext *ctx, GLuint list );

extern void gl_ListBase( GLcontext *ctx, GLuint base );

extern void gl_NewList( GLcontext *ctx, GLuint list, GLenum mode );




extern void gl_save_Accum( GLcontext *ctx, GLenum op, GLfloat value );

extern void gl_save_AlphaFunc( GLcontext *ctx, GLenum func, GLclampf ref );

extern void gl_save_BlendColor( GLcontext *ctx, GLfloat red, GLfloat green,
                                GLfloat blue, GLfloat alpha );

extern void gl_save_BlendEquation( GLcontext *ctx, GLenum mode );

extern void gl_save_BlendFunc( GLcontext *ctx,
                               GLenum sfactor, GLenum dfactor );

extern void gl_save_Begin( GLcontext *ctx, GLenum mode );

extern void gl_save_BindTexture( GLcontext *ctx,
                                 GLenum target, GLuint texture );

extern void gl_save_Bitmap( GLcontext *ctx, GLsizei width, GLsizei height,
			    GLfloat xorig, GLfloat yorig,
			    GLfloat xmove, GLfloat ymove,
                            struct gl_image *bitmap );

extern void gl_save_CallList( GLcontext *ctx, GLuint list );

extern void gl_save_CallLists( GLcontext *ctx,
                               GLsizei n, GLenum type, const GLvoid *lists );

extern void gl_save_Clear( GLcontext *ctx, GLbitfield mask );

extern void gl_save_ClearAccum( GLcontext *ctx, GLfloat red, GLfloat green,
			        GLfloat blue, GLfloat alpha );

extern void gl_save_ClearColor( GLcontext *ctx, GLclampf red, GLclampf green,
			        GLclampf blue, GLclampf alpha );

extern void gl_save_ClearDepth( GLcontext *ctx, GLclampd depth );

extern void gl_save_ClearIndex( GLcontext *ctx, GLfloat c );

extern void gl_save_ClearStencil( GLcontext *ctx, GLint s );

extern void gl_save_ClipPlane( GLcontext *ctx,
                               GLenum plane, const GLfloat *equ );

extern void gl_save_Color3f( GLcontext *ctx, GLfloat r, GLfloat g, GLfloat b );

extern void gl_save_Color3fv( GLcontext *ctx, const GLfloat *c );

extern void gl_save_Color4f( GLcontext *ctx, GLfloat r, GLfloat g,
                             GLfloat b, GLfloat a );

extern void gl_save_Color4fv( GLcontext *ctx, const GLfloat *c );

extern void gl_save_Color4ub( GLcontext *ctx, GLubyte r, GLubyte g,
                              GLubyte b, GLubyte a );

extern void gl_save_Color4ubv( GLcontext *ctx, const GLubyte *c );

extern void gl_save_ColorMask( GLcontext *ctx, GLboolean red, GLboolean green,
			       GLboolean blue, GLboolean alpha );

extern void gl_save_ColorMaterial( GLcontext *ctx, GLenum face, GLenum mode );

extern void gl_save_ColorTable( GLcontext *ctx, GLenum target,
                                GLenum internalFormat,
                                struct gl_image *table );

extern void gl_save_ColorSubTable( GLcontext *ctx, GLenum target,
                                   GLsizei start, struct gl_image *data );

extern void gl_save_CopyPixels( GLcontext *ctx, GLint x, GLint y,
				GLsizei width, GLsizei height, GLenum type );

extern void gl_save_CopyTexImage1D( GLcontext *ctx,
                                    GLenum target, GLint level,
                                    GLenum internalformat,
                                    GLint x, GLint y, GLsizei width,
                                    GLint border );

extern void gl_save_CopyTexImage2D( GLcontext *ctx,
                                    GLenum target, GLint level,
                                    GLenum internalformat,
                                    GLint x, GLint y, GLsizei width,
                                    GLsizei height, GLint border );

extern void gl_save_CopyTexSubImage1D( GLcontext *ctx,
                                       GLenum target, GLint level,
                                       GLint xoffset, GLint x, GLint y,
                                       GLsizei width );

extern void gl_save_CopyTexSubImage2D( GLcontext *ctx,
                                       GLenum target, GLint level,
                                       GLint xoffset, GLint yoffset,
                                       GLint x, GLint y,
                                       GLsizei width, GLint height );

extern void gl_save_CopyTexSubImage3DEXT( GLcontext *ctx,
                                          GLenum target, GLint level,
                                          GLint xoffset, GLint yoffset, GLint zoffset,
                                          GLint x, GLint y,
                                          GLsizei width, GLint height );

extern void gl_save_CullFace( GLcontext *ctx, GLenum mode );

extern void gl_save_DepthFunc( GLcontext *ctx, GLenum func );

extern void gl_save_DepthMask( GLcontext *ctx, GLboolean mask );

extern void gl_save_DepthRange( GLcontext *ctx,
                                GLclampd nearval, GLclampd farval );

extern void gl_save_Disable( GLcontext *ctx, GLenum cap );

extern void gl_save_DrawBuffer( GLcontext *ctx, GLenum mode );

extern void gl_save_DrawPixels( GLcontext *ctx, struct gl_image *image );

extern void gl_save_EdgeFlag( GLcontext *ctx, GLboolean flag );

extern void gl_save_Enable( GLcontext *ctx, GLenum cap );

extern void gl_save_End( GLcontext *ctx );

extern void gl_save_EvalCoord1f( GLcontext *ctx, GLfloat u );

extern void gl_save_EvalCoord2f( GLcontext *ctx, GLfloat u, GLfloat v );

extern void gl_save_EvalMesh1( GLcontext *ctx,
                               GLenum mode, GLint i1, GLint i2 );

extern void gl_save_EvalMesh2( GLcontext *ctx, GLenum mode, GLint i1, GLint i2,
			       GLint j1, GLint j2 );

extern void gl_save_EvalPoint1( GLcontext *ctx, GLint i );

extern void gl_save_EvalPoint2( GLcontext *ctx, GLint i, GLint j );

extern void gl_save_Fogfv( GLcontext *ctx,
                           GLenum pname, const GLfloat *params );

extern void gl_save_FrontFace( GLcontext *ctx, GLenum mode );

extern void gl_save_Frustum( GLcontext *ctx, GLdouble left, GLdouble right,
                             GLdouble bottom, GLdouble top,
                             GLdouble nearval, GLdouble farval );

extern void gl_save_Hint( GLcontext *ctx, GLenum target, GLenum mode );

extern void gl_save_Indexf( GLcontext *ctx, GLfloat index );

extern void gl_save_Indexi( GLcontext *ctx, GLint index );

extern void gl_save_IndexMask( GLcontext *ctx, GLuint mask );

extern void gl_save_InitNames( GLcontext *ctx );

extern void gl_save_Lightfv( GLcontext *ctx, GLenum light, GLenum pname,
                             const GLfloat *params, GLint numparams );

extern void gl_save_LightModelfv( GLcontext *ctx, GLenum pname,
                                const GLfloat *params );

extern void gl_save_LineWidth( GLcontext *ctx, GLfloat width );

extern void gl_save_LineStipple( GLcontext *ctx, GLint factor,
                                 GLushort pattern );

extern void gl_save_ListBase( GLcontext *ctx, GLuint base );

extern void gl_save_LoadIdentity( GLcontext *ctx );

extern void gl_save_LoadMatrixf( GLcontext *ctx, const GLfloat *m );

extern void gl_save_LoadName( GLcontext *ctx, GLuint name );

extern void gl_save_LogicOp( GLcontext *ctx, GLenum opcode );

extern void gl_save_Map1f( GLcontext *ctx, GLenum target,
                           GLfloat u1, GLfloat u2, GLint stride,
			   GLint order, const GLfloat *points,
                           GLboolean retain );

extern void gl_save_Map2f( GLcontext *ctx, GLenum target,
			   GLfloat u1, GLfloat u2, GLint ustride, GLint uorder,
			   GLfloat v1, GLfloat v2, GLint vstride, GLint vorder,
			   const GLfloat *points,
                           GLboolean retain );

extern void gl_save_MapGrid1f( GLcontext *ctx, GLint un,
                               GLfloat u1, GLfloat u2 );

extern void gl_save_MapGrid2f( GLcontext *ctx,
                               GLint un, GLfloat u1, GLfloat u2,
                               GLint vn, GLfloat v1, GLfloat v2 );

extern void gl_save_Materialfv( GLcontext *ctx, GLenum face, GLenum pname,
                                const GLfloat *params );

extern void gl_save_MatrixMode( GLcontext *ctx, GLenum mode );

extern void gl_save_MultMatrixf( GLcontext *ctx, const GLfloat *m );

extern void gl_save_NewList( GLcontext *ctx, GLuint list, GLenum mode );

extern void gl_save_Normal3fv( GLcontext *ctx, const GLfloat n[3] );

extern void gl_save_Normal3f( GLcontext *ctx,
                              GLfloat nx, GLfloat ny, GLfloat nz );

extern void gl_save_Ortho( GLcontext *ctx, GLdouble left, GLdouble right,
                           GLdouble bottom, GLdouble top,
                           GLdouble nearval, GLdouble farval );

extern void gl_save_PassThrough( GLcontext *ctx, GLfloat token );

extern void gl_save_PixelMapfv( GLcontext *ctx, GLenum map, GLint mapsize,
                                const GLfloat *values );

extern void gl_save_PixelTransferf( GLcontext *ctx,
                                    GLenum pname, GLfloat param );

extern void gl_save_PixelZoom( GLcontext *ctx,
                               GLfloat xfactor, GLfloat yfactor );

extern void gl_save_PointParameterfvEXT( GLcontext *ctx, GLenum pname,
                                         const GLfloat *params );

extern void gl_save_PointSize( GLcontext *ctx, GLfloat size );

extern void gl_save_PolygonMode( GLcontext *ctx, GLenum face, GLenum mode );

extern void gl_save_PolygonStipple( GLcontext *ctx, const GLuint *pattern );

extern void gl_save_PolygonOffset( GLcontext *ctx,
                                   GLfloat factor, GLfloat units );

extern void gl_save_PopAttrib( GLcontext *ctx );

extern void gl_save_PopMatrix( GLcontext *ctx );

extern void gl_save_PopName( GLcontext *ctx );

extern void gl_save_PrioritizeTextures( GLcontext *ctx,
                                        GLsizei n, const GLuint *textures,
                                        const GLclampf *priorities );

extern void gl_save_PushAttrib( GLcontext *ctx, GLbitfield mask );

extern void gl_save_PushMatrix( GLcontext *ctx );

extern void gl_save_PushName( GLcontext *ctx, GLuint name );

extern void gl_save_RasterPos4f( GLcontext *ctx,
                                 GLfloat x, GLfloat y, GLfloat z, GLfloat w );

extern void gl_save_ReadBuffer( GLcontext *ctx, GLenum mode );

extern void gl_save_Rectf( GLcontext *ctx,
                           GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );

extern void gl_save_Rotatef( GLcontext *ctx, GLfloat angle,
                             GLfloat x, GLfloat y, GLfloat z );

extern void gl_save_Scalef( GLcontext *ctx, GLfloat x, GLfloat y, GLfloat z );

extern void gl_save_Scissor( GLcontext *ctx,
                             GLint x, GLint y, GLsizei width, GLsizei height );

extern void gl_save_ShadeModel( GLcontext *ctx, GLenum mode );

extern void gl_save_StencilFunc( GLcontext *ctx,
                                 GLenum func, GLint ref, GLuint mask );

extern void gl_save_StencilMask( GLcontext *ctx, GLuint mask );

extern void gl_save_StencilOp( GLcontext *ctx,
                               GLenum fail, GLenum zfail, GLenum zpass );

extern void gl_save_TexCoord2f( GLcontext *ctx, GLfloat s, GLfloat t );

extern void gl_save_TexCoord4f( GLcontext *ctx, GLfloat s, GLfloat t,
                                GLfloat r, GLfloat q );

extern void gl_save_TexEnvfv( GLcontext *ctx, GLenum target, GLenum pname,
                              const GLfloat *params );

extern void gl_save_TexParameterfv( GLcontext *ctx, GLenum target,
                                    GLenum pname, const GLfloat *params );

extern void gl_save_TexGenfv( GLcontext *ctx, GLenum coord, GLenum pname,
                              const GLfloat *params );

extern void gl_save_TexImage1D( GLcontext *ctx, GLenum target,
                                GLint level, GLint components,
			        GLsizei width, GLint border,
                                GLenum format, GLenum type,
                                struct gl_image *teximage );

extern void gl_save_TexImage2D( GLcontext *ctx, GLenum target,
                                GLint level, GLint components,
			        GLsizei width, GLsizei height, GLint border,
                                GLenum format, GLenum type,
                                struct gl_image *teximage );

extern void gl_save_TexImage3DEXT( GLcontext *ctx, GLenum target,
                                   GLint level, GLint components,
                                   GLsizei width, GLsizei height, GLsizei depth,
                                   GLint border,
                                   GLenum format, GLenum type,
                                   struct gl_image *teximage );

extern void gl_save_TexSubImage1D( GLcontext *ctx,
                                   GLenum target, GLint level,
                                   GLint xoffset, GLsizei width,
                                   GLenum format, GLenum type,
                                   struct gl_image *image );


extern void gl_save_TexSubImage2D( GLcontext *ctx,
                                   GLenum target, GLint level,
                                   GLint xoffset, GLint yoffset,
                                   GLsizei width, GLsizei height,
                                   GLenum format, GLenum type,
                                   struct gl_image *image );

extern void gl_save_TexSubImage3DEXT( GLcontext *ctx,
                                      GLenum target, GLint level,
                                      GLint xoffset, GLint yoffset, GLint zoffset,
                                      GLsizei width, GLsizei height, GLsizei depth,
                                      GLenum format, GLenum type,
                                      struct gl_image *image );

extern void gl_save_Translatef( GLcontext *ctx,
                                GLfloat x, GLfloat y, GLfloat z );

extern void gl_save_Vertex2f( GLcontext *ctx,
                              GLfloat x, GLfloat y );

extern void gl_save_Vertex3f( GLcontext *ctx,
                              GLfloat x, GLfloat y, GLfloat z );

extern void gl_save_Vertex4f( GLcontext *ctx,
                              GLfloat x, GLfloat y, GLfloat z, GLfloat w );

extern void gl_save_Vertex3fv( GLcontext *ctx, const GLfloat *v );

extern void gl_save_Viewport( GLcontext *ctx, GLint x, GLint y,
                              GLsizei width, GLsizei height );

extern void gl_save_WindowPos4fMESA( GLcontext *ctx,
                                     GLfloat x, GLfloat y,
                                     GLfloat z, GLfloat w );


/* GL_SGIS_multitexture */
extern void gl_save_MultiTexCoord4f( GLcontext *ctx, GLenum target,
                                     GLfloat s, GLfloat t,
                                     GLfloat r, GLfloat q );

extern void gl_save_SelectTextureSGIS( GLcontext *ctx, GLenum target );

extern void gl_save_SelectTexture( GLcontext *ctx, GLenum target );

extern void gl_save_SelectTextureCoordSet( GLcontext *ctx, GLenum target );

extern void gl_save_SelectTextureTransform( GLcontext *ctx, GLenum target );


#endif
