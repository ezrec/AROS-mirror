/* $Id$ */

/*
 * Mesa 3-D graphics library
 * Version:  2.0
 * Copyright (C) 1995-1996  Brian Paul
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
 * Revision 1.1  2003/08/09 00:23:15  chodorowski
 * Initial revision
 *
 * Revision 1.4  1996/09/27 01:23:50  brianp
 * added extra error checking when DEBUG is defined
 *
 * Revision 1.3  1996/09/26 22:52:12  brianp
 * added glInterleavedArrays
 *
 * Revision 1.2  1996/09/14 06:27:22  brianp
 * some functions didn't return needed values
 *
 * Revision 1.1  1996/09/13 01:38:16  brianp
 * Initial revision
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "bitmap.h"
#include "context.h"
#include "eval.h"
#include "image.h"
#include "macros.h"
#include "matrix.h"
#include "teximage.h"
#include "types.h"



#ifdef MULTI_THREADING

/* Get the context associated with the calling thread */
#define GET_CONTEXT	GLcontext* ctx = gl_get_thread_context()

#else

#ifdef DEBUG
#define GET_CONTEXT							\
	printf("GL Command executed 1:%s\n",__FUNC__);	\
	if (!CC) {									\
	   fprintf(stderr,"Mesa error:  no rendering context!\n");	\
	   abort();									\
	}

#else
/* CC is a global pointer for all threads in the address space */
#define GET_CONTEXT
#endif /*DEBUG*/

#endif /*MULTI_THREADED*/



#define SHORTCUT


void glAccum( GLenum op, GLfloat value )
{
   GET_CONTEXT;
   (*CC->API.Accum)(CC, op, value);
}


void glAlphaFunc( GLenum func, GLclampf ref )
{
   GET_CONTEXT;
   (*CC->API.AlphaFunc)(CC, func, ref);
}


GLboolean glAreTexturesResident( GLsizei n, const GLuint *textures,
                                 GLboolean *residences )
{
   GET_CONTEXT;
   return (*CC->API.AreTexturesResident)(CC, n, textures, residences);
}


void glArrayElement( GLint i )
{
   GET_CONTEXT;
   (*CC->API.ArrayElement)(CC, i);
}


void glBegin( GLenum mode )
{
   GET_CONTEXT;
   (*CC->API.Begin)( CC, mode );
}


void glBindTexture( GLenum target, GLuint texture )
{
   GET_CONTEXT;
   (*CC->API.BindTexture)(CC, target, texture);
}


void glBitmap( GLsizei width, GLsizei height,
               GLfloat xorig, GLfloat yorig,
               GLfloat xmove, GLfloat ymove,
               const GLubyte *bitmap )
{
   GET_CONTEXT;
   if (!CC->CompileFlag) {
      /* execute only, try optimized case where no unpacking needed */
      if (   CC->Unpack.LsbFirst==GL_FALSE
          && CC->Unpack.Alignment==1
          && CC->Unpack.RowLength==0
          && CC->Unpack.SkipPixels==0
          && CC->Unpack.SkipRows==0) {
         /* Special case: no unpacking needed */
         struct gl_image image;
         image.Width = width;
         image.Height = height;
         image.Components = 0;
         image.Type = GL_BITMAP;
         image.Data = (GLvoid *) bitmap;
         (*CC->Exec.Bitmap)( CC, width, height, xorig, yorig,
                             xmove, ymove, &image );
      }
      else {
         struct gl_image *image;
         image = gl_unpack_bitmap( CC, width, height, bitmap );
         (*CC->Exec.Bitmap)( CC, width, height, xorig, yorig,
                             xmove, ymove, image );
         gl_free_image( image );
      }
   }
   else {
      /* compile and maybe execute */
      struct gl_image *image;
      image = gl_unpack_bitmap( CC, width, height, bitmap );
      (*CC->API.Bitmap)(CC, width, height, xorig, yorig, xmove, ymove, image );
   }
}


void glBlendFunc( GLenum sfactor, GLenum dfactor )
{
   GET_CONTEXT;
   (*CC->API.BlendFunc)(CC, sfactor, dfactor);
}


void glCallList( GLuint list )
{
   GET_CONTEXT;
   (*CC->API.CallList)(CC, list);
}


void glCallLists( GLsizei n, GLenum type, const GLvoid *lists )
{
   GET_CONTEXT;
   (*CC->API.CallLists)(CC, n, type, lists);
}


void glClear( GLbitfield mask )
{
   GET_CONTEXT;
   (*CC->API.Clear)(CC, mask);
}


void glClearAccum( GLfloat red, GLfloat green,
			  GLfloat blue, GLfloat alpha )
{
   GET_CONTEXT;
   (*CC->API.ClearAccum)(CC, red, green, blue, alpha);
}



void glClearIndex( GLfloat c )
{
   GET_CONTEXT;
   (*CC->API.ClearIndex)(CC, c);
}


void glClearColor( GLclampf red,
			  GLclampf green,
			  GLclampf blue,
			  GLclampf alpha )
{
   GET_CONTEXT;
   (*CC->API.ClearColor)(CC, red, green, blue, alpha);
}


void glClearDepth( GLclampd depth )
{
   GET_CONTEXT;
   (*CC->API.ClearDepth)( CC, depth );
}


void glClearStencil( GLint s )
{
   GET_CONTEXT;
   (*CC->API.ClearStencil)(CC, s);
}


void glClipPlane( GLenum plane, const GLdouble *equation )
{
   GLfloat eq[4];
   GET_CONTEXT;
   eq[0] = (GLfloat) equation[0];
   eq[1] = (GLfloat) equation[1];
   eq[2] = (GLfloat) equation[2];
   eq[3] = (GLfloat) equation[3];
   (*CC->API.ClipPlane)(CC, plane, eq );
}


void glColor3b( GLbyte red, GLbyte green, GLbyte blue )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, BYTE_TO_FLOAT(red), BYTE_TO_FLOAT(green),
                           BYTE_TO_FLOAT(blue), 1.0F );
}


void glColor3d( GLdouble red, GLdouble green, GLdouble blue )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, (GLfloat) red, (GLfloat) green,
                           (GLfloat) blue, 1.0F );
}


void glColor3f( GLfloat red, GLfloat green, GLfloat blue )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, red, green, blue, 1.0F );
}


void glColor3i( GLint red, GLint green, GLint blue )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, INT_TO_FLOAT(red), INT_TO_FLOAT(green),
                           INT_TO_FLOAT(blue), 1.0F );
}


void glColor3s( GLshort red, GLshort green, GLshort blue )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, SHORT_TO_FLOAT(red), SHORT_TO_FLOAT(green),
                           SHORT_TO_FLOAT(blue), 1.0F );
}


void glColor3ub( GLubyte red, GLubyte green, GLubyte blue )
{
   GET_CONTEXT;
   (*CC->API.Color4ub)( CC, red, green, blue, 255 );
}


void glColor3ui( GLuint red, GLuint green, GLuint blue )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, UINT_TO_FLOAT(red), UINT_TO_FLOAT(green),
                           UINT_TO_FLOAT(blue), 1.0F );
}


void glColor3us( GLushort red, GLushort green, GLushort blue )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, USHORT_TO_FLOAT(red), USHORT_TO_FLOAT(green),
                           USHORT_TO_FLOAT(blue), 1.0F );
}


void glColor4b( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, BYTE_TO_FLOAT(red), BYTE_TO_FLOAT(green),
                           BYTE_TO_FLOAT(blue), BYTE_TO_FLOAT(alpha) );
}


void glColor4d( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, (GLfloat) red, (GLfloat) green,
                           (GLfloat) blue, (GLfloat) alpha );
}


void glColor4f( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, red, green, blue, alpha );
}

void glColor4i( GLint red, GLint green, GLint blue, GLint alpha )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, INT_TO_FLOAT(red), INT_TO_FLOAT(green),
                           INT_TO_FLOAT(blue), INT_TO_FLOAT(alpha) );
}


void glColor4s( GLshort red, GLshort green, GLshort blue, GLshort alpha )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, SHORT_TO_FLOAT(red), SHORT_TO_FLOAT(green),
                           SHORT_TO_FLOAT(blue), SHORT_TO_FLOAT(alpha) );
}

void glColor4ub( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha )
{
   GET_CONTEXT;
   (*CC->API.Color4ub)( CC, red, green, blue, alpha );
}

void glColor4ui( GLuint red, GLuint green, GLuint blue, GLuint alpha )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, UINT_TO_FLOAT(red), UINT_TO_FLOAT(green),
                           UINT_TO_FLOAT(blue), UINT_TO_FLOAT(alpha) );
}

void glColor4us( GLushort red, GLushort green, GLushort blue, GLushort alpha )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, USHORT_TO_FLOAT(red), USHORT_TO_FLOAT(green),
                           USHORT_TO_FLOAT(blue), USHORT_TO_FLOAT(alpha) );
}


void glColor3bv( const GLbyte *v )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, BYTE_TO_FLOAT(v[0]), BYTE_TO_FLOAT(v[1]),
                           BYTE_TO_FLOAT(v[2]), 1.0F );
}


void glColor3dv( const GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, (GLdouble) v[0], (GLdouble) v[1],
                           (GLdouble) v[2], 1.0F );
}


void glColor3fv( const GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, v[0],v [1], v[2], 1.0F );
}


void glColor3iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, INT_TO_FLOAT(v[0]), INT_TO_FLOAT(v[1]),
                           INT_TO_FLOAT(v[2]), 1.0F );
}


void glColor3sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, SHORT_TO_FLOAT(v[0]), SHORT_TO_FLOAT(v[1]),
                           SHORT_TO_FLOAT(v[2]), 1.0F );
}


void glColor3ubv( const GLubyte *v )
{
   GET_CONTEXT;
   (*CC->API.Color4ub)( CC, v[0], v[1], v[2], 255 );
}


void glColor3uiv( const GLuint *v )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, UINT_TO_FLOAT(v[0]), UINT_TO_FLOAT(v[1]),
                           UINT_TO_FLOAT(v[2]), 1.0F );
}


void glColor3usv( const GLushort *v )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, USHORT_TO_FLOAT(v[0]), USHORT_TO_FLOAT(v[1]),
                           USHORT_TO_FLOAT(v[2]), 1.0F );

}


void glColor4bv( const GLbyte *v )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, BYTE_TO_FLOAT(v[0]), BYTE_TO_FLOAT(v[1]),
                           BYTE_TO_FLOAT(v[2]), BYTE_TO_FLOAT(v[3]) );
}


void glColor4dv( const GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, (GLdouble) v[0], (GLdouble) v[1],
                           (GLdouble) v[2], (GLdouble) v[3] );
}


void glColor4fv( const GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, v[0], v[1], v[2], v[3] );
}


void glColor4iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, INT_TO_FLOAT(v[0]), INT_TO_FLOAT(v[1]),
                           INT_TO_FLOAT(v[2]), INT_TO_FLOAT(v[3]) );
}


void glColor4sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, SHORT_TO_FLOAT(v[0]), SHORT_TO_FLOAT(v[1]),
                           SHORT_TO_FLOAT(v[2]), SHORT_TO_FLOAT(v[3]) );
}


void glColor4ubv( const GLubyte *v )
{
   GET_CONTEXT;
   (*CC->API.Color4ub)( CC, v[0], v[1], v[2], v[3] );
}


void glColor4uiv( const GLuint *v )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, UINT_TO_FLOAT(v[0]), UINT_TO_FLOAT(v[1]),
                           UINT_TO_FLOAT(v[2]), UINT_TO_FLOAT(v[3]) );
}


void glColor4usv( const GLushort *v )
{
   GET_CONTEXT;
   (*CC->API.Color4f)( CC, USHORT_TO_FLOAT(v[0]), USHORT_TO_FLOAT(v[1]),
                           USHORT_TO_FLOAT(v[2]), USHORT_TO_FLOAT(v[3]) );
}


void glColorMask( GLboolean red, GLboolean green,
			 GLboolean blue, GLboolean alpha )
{
	GET_CONTEXT;
	(*CC->API.ColorMask)(CC, red, green, blue, alpha);
}


void glColorMaterial( GLenum face, GLenum mode )
{
	GET_CONTEXT;
	(*CC->API.ColorMaterial)(CC, face, mode);
}


void glColorPointer( GLint size, GLenum type, GLsizei stride,
                     const GLvoid *ptr )
{
	GET_CONTEXT;
	(*CC->API.ColorPointer)(CC, size, type, stride, ptr);
}


void glCopyPixels( GLint x, GLint y, GLsizei width, GLsizei height,
			  GLenum type )
{
	GET_CONTEXT;
	(*CC->API.CopyPixels)(CC, x, y, width, height, type);
}


void glCopyTexImage1D( GLenum target, GLint level,
                       GLenum internalformat,
                       GLint x, GLint y,
                       GLsizei width, GLint border )
{
   GET_CONTEXT;
   (*CC->API.CopyTexImage1D)( CC, target, level, internalformat,
                                 x, y, width, border );
}


void glCopyTexImage2D( GLenum target, GLint level,
                       GLenum internalformat,
                       GLint x, GLint y,
                       GLsizei width, GLsizei height, GLint border )
{
   GET_CONTEXT;
   (*CC->API.CopyTexImage2D)( CC, target, level, internalformat,
                              x, y, width, height, border );
}


void glCopyTexSubImage1D( GLenum target, GLint level,
                          GLint xoffset, GLint x, GLint y,
                          GLsizei width )
{
   GET_CONTEXT;
   (*CC->API.CopyTexSubImage1D)( CC, target, level, xoffset, x, y, width );
}


void glCopyTexSubImage2D( GLenum target, GLint level,
                          GLint xoffset, GLint yoffset,
                          GLint x, GLint y,
                          GLsizei width, GLsizei height )
{
   GET_CONTEXT;
   (*CC->API.CopyTexSubImage2D)( CC, target, level, xoffset, yoffset,
                                 x, y, width, height );
}



void glCullFace( GLenum mode )
{
   GET_CONTEXT;
   (*CC->API.CullFace)(CC, mode);
}


void glDepthFunc( GLenum func )
{
   GET_CONTEXT;
   (*CC->API.DepthFunc)( CC, func );
}


void glDepthMask( GLboolean flag )
{
   GET_CONTEXT;
   (*CC->API.DepthMask)( CC, flag );
}


void glDepthRange( GLclampd near_val, GLclampd far_val )
{
   GET_CONTEXT;
   (*CC->API.DepthRange)( CC, near_val, far_val );
}


void glDeleteLists( GLuint list, GLsizei range )
{
   GET_CONTEXT;
   (*CC->API.DeleteLists)(CC, list, range);
}


void glDeleteTextures( GLsizei n, const GLuint *textures)
{
   GET_CONTEXT;
   (*CC->API.DeleteTextures)(CC, n, textures);
}


void glDisable( GLenum cap )
{
   GET_CONTEXT;
   (*CC->API.Disable)( CC, cap );
}


void glDisableClientState( GLenum cap )
{
   GET_CONTEXT;
   (*CC->API.DisableClientState)( CC, cap );
}


void glDrawArrays( GLenum mode, GLint first, GLsizei count )
{
   GET_CONTEXT;
   (*CC->API.DrawArrays)(CC, mode, first, count);
}


void glDrawBuffer( GLenum mode )
{
   GET_CONTEXT;
   (*CC->API.DrawBuffer)(CC, mode);
}


void glDrawElements( GLenum mode, GLsizei count,
                     GLenum type, const GLvoid *indices )
{
   GET_CONTEXT;
   (*CC->API.DrawElements)( CC, mode, count, type, indices );
}


void glDrawPixels( GLsizei width, GLsizei height,
                   GLenum format, GLenum type, const GLvoid *pixels )
{
   GET_CONTEXT;
   (*CC->API.DrawPixels)( CC, width, height, format, type, pixels );
}


void glEnable( GLenum cap )
{
   GET_CONTEXT;
   (*CC->API.Enable)( CC, cap );
}


void glEnableClientState( GLenum cap )
{
   GET_CONTEXT;
   (*CC->API.EnableClientState)( CC, cap );
}


void glEnd( void )
{
   GET_CONTEXT;
   (*CC->API.End)( CC );
}


void glEndList( void )
{
   GET_CONTEXT;
   (*CC->API.EndList)(CC);
}




void glEvalCoord1d( GLdouble u )
{
   GET_CONTEXT;
   (*CC->API.EvalCoord1f)( CC, (GLfloat) u );
}


void glEvalCoord1f( GLfloat u )
{
   GET_CONTEXT;
   (*CC->API.EvalCoord1f)( CC, u );
}


void glEvalCoord1dv( const GLdouble *u )
{
   GET_CONTEXT;
   (*CC->API.EvalCoord1f)( CC, (GLfloat) *u );
}


void glEvalCoord1fv( const GLfloat *u )
{
   GET_CONTEXT;
   (*CC->API.EvalCoord1f)( CC, (GLfloat) *u );
}


void glEvalCoord2d( GLdouble u, GLdouble v )
{
   GET_CONTEXT;
   (*CC->API.EvalCoord2f)( CC, (GLfloat) u, (GLfloat) v );
}


void glEvalCoord2f( GLfloat u, GLfloat v )
{
   GET_CONTEXT;
   (*CC->API.EvalCoord2f)( CC, u, v );
}


void glEvalCoord2dv( const GLdouble *u )
{
   GET_CONTEXT;
   (*CC->API.EvalCoord2f)( CC, (GLfloat) u[0], (GLfloat) u[1] );
}


void glEvalCoord2fv( const GLfloat *u )
{
   GET_CONTEXT;
   (*CC->API.EvalCoord2f)( CC, u[0], u[1] );
}


void glEvalPoint1( GLint i )
{
   GET_CONTEXT;
   (*CC->API.EvalPoint1)( CC, i );
}


void glEvalPoint2( GLint i, GLint j )
{
   GET_CONTEXT;
   (*CC->API.EvalPoint2)( CC, i, j );
}


void glEvalMesh1( GLenum mode, GLint i1, GLint i2 )
{
   GET_CONTEXT;
   (*CC->API.EvalMesh1)( CC, mode, i1, i2 );
}


void glEdgeFlag( GLboolean flag )
{
   GET_CONTEXT;
   (*CC->API.EdgeFlag)(CC, flag);
}


void glEdgeFlagv( const GLboolean *flag )
{
   GET_CONTEXT;
   (*CC->API.EdgeFlag)(CC, *flag);
}


void glEdgeFlagPointer( GLsizei stride, const GLboolean *ptr )
{
   GET_CONTEXT;
   (*CC->API.EdgeFlagPointer)(CC, stride, ptr);
}


void glEvalMesh2( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 )
{
   GET_CONTEXT;
   (*CC->API.EvalMesh2)( CC, mode, i1, i2, j1, j2 );
}


void glFeedbackBuffer( GLsizei size, GLenum type, GLfloat *buffer )
{
   GET_CONTEXT;
   (*CC->API.FeedbackBuffer)(CC, size, type, buffer);
}


void glFinish( void )
{
   GET_CONTEXT;
   (*CC->API.Finish)(CC);
}


void glFlush( void )
{
   GET_CONTEXT;
   (*CC->API.Flush)(CC);
}


void glFogf( GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   (*CC->API.Fogfv)(CC, pname, &param);
}


void glFogi( GLenum pname, GLint param )
{
   GLfloat fparam = (GLfloat) param;
   GET_CONTEXT;
   (*CC->API.Fogfv)(CC, pname, &fparam);
}


void glFogfv( GLenum pname, const GLfloat *params )
{
   GET_CONTEXT;
   (*CC->API.Fogfv)(CC, pname, params);
}


void glFogiv( GLenum pname, const GLint *params )
{
   GLfloat p[4];
   GET_CONTEXT;

   switch (pname) {
      case GL_FOG_MODE:
      case GL_FOG_DENSITY:
      case GL_FOG_START:
      case GL_FOG_END:
      case GL_FOG_INDEX:
         p[0] = (GLfloat) *params;
	 break;
      case GL_FOG_COLOR:
	 p[0] = INT_TO_FLOAT( params[0] );
	 p[1] = INT_TO_FLOAT( params[1] );
	 p[2] = INT_TO_FLOAT( params[2] );
	 p[3] = INT_TO_FLOAT( params[3] );
	 break;
      default:
         /* Error will be caught later in gl_Fogfv */
         ;
   }
   (*CC->API.Fogfv)( CC, pname, p );
}



void glFrontFace( GLenum mode )
{
   GET_CONTEXT;
   (*CC->API.FrontFace)(CC, mode);
}


void glFrustum( GLdouble left, GLdouble right,
				GLdouble bottom, GLdouble top,
				GLdouble nearval, GLdouble farval )
{
   GLfloat x, y, a, b, c, d;
   GLfloat m[16];
   GET_CONTEXT;

   if (nearval<=0.0 || farval<=0.0) {
      gl_error( CC, GL_INVALID_VALUE, "glFrustum(near or far)" );
   }

   x = (2.0*nearval) / (right-left);
   y = (2.0*nearval) / (top-bottom);
   a = (right+left) / (right-left);
   b = (top+bottom) / (top-bottom);
   c = -(farval+nearval) / ( farval-nearval);
   d = -(2.0*farval*nearval) / (farval-nearval);  /* error? */

#define M(row,col)  m[col*4+row]
   M(0,0) = x;     M(0,1) = 0.0F;  M(0,2) = a;      M(0,3) = 0.0F;
   M(1,0) = 0.0F;  M(1,1) = y;     M(1,2) = b;      M(1,3) = 0.0F;
   M(2,0) = 0.0F;  M(2,1) = 0.0F;  M(2,2) = c;      M(2,3) = d;
   M(3,0) = 0.0F;  M(3,1) = 0.0F;  M(3,2) = -1.0F;  M(3,3) = 0.0F;
#undef M

   (*CC->API.MultMatrixf)( CC, m );
}


GLuint glGenLists( GLsizei range )
{
   GET_CONTEXT;
   return (*CC->API.GenLists)(CC, range);
}


void glGenTextures( GLsizei n, GLuint *textures )
{
   GET_CONTEXT;
   (*CC->API.GenTextures)(CC, n, textures);
}


void glGetBooleanv( GLenum pname, GLboolean *params )
{
   GET_CONTEXT;
   (*CC->API.GetBooleanv)(CC, pname, params);
}


void glGetClipPlane( GLenum plane, GLdouble *equation )
{
   GET_CONTEXT;
   (*CC->API.GetClipPlane)(CC, plane, equation);
}


void glGetDoublev( GLenum pname, GLdouble *params )
{
   GET_CONTEXT;
   (*CC->API.GetDoublev)(CC, pname, params);
}


GLenum glGetError( void )
{
   GET_CONTEXT;
   if (!CC) {
      /* No current context */
      return GL_NO_ERROR;
   }
   return (*CC->API.GetError)(CC);
}


void glGetFloatv( GLenum pname, GLfloat *params )
{
   GET_CONTEXT;
   (*CC->API.GetFloatv)(CC, pname, params);
}


void glGetIntegerv( GLenum pname, GLint *params )
{
   GET_CONTEXT;
   (*CC->API.GetIntegerv)(CC, pname, params);
}


void glGetLightfv( GLenum light, GLenum pname, GLfloat *params )
{
   GET_CONTEXT;
   (*CC->API.GetLightfv)(CC, light, pname, params);
}


void glGetLightiv( GLenum light, GLenum pname, GLint *params )
{
   GET_CONTEXT;
   (*CC->API.GetLightiv)(CC, light, pname, params);
}


void glGetMapdv( GLenum target, GLenum query, GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.GetMapdv)( CC, target, query, v );
}


void glGetMapfv( GLenum target, GLenum query, GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.GetMapfv)( CC, target, query, v );
}


void glGetMapiv( GLenum target, GLenum query, GLint *v )
{
   GET_CONTEXT;
   (*CC->API.GetMapiv)( CC, target, query, v );
}


void glGetMaterialfv( GLenum face, GLenum pname, GLfloat *params )
{
   GET_CONTEXT;
   (*CC->API.GetMaterialfv)(CC, face, pname, params);
}


void glGetMaterialiv( GLenum face, GLenum pname, GLint *params )
{
   GET_CONTEXT;
   (*CC->API.GetMaterialiv)(CC, face, pname, params);
}


void glGetPixelMapfv( GLenum map, GLfloat *values )
{
   GET_CONTEXT;
   (*CC->API.GetPixelMapfv)(CC, map, values);
}


void glGetPixelMapuiv( GLenum map, GLuint *values )
{
   GET_CONTEXT;
   (*CC->API.GetPixelMapuiv)(CC, map, values);
}


void glGetPixelMapusv( GLenum map, GLushort *values )
{
   GET_CONTEXT;
   (*CC->API.GetPixelMapusv)(CC, map, values);
}


void glGetPointerv( GLenum pname, GLvoid **params )
{
   GET_CONTEXT;
   (*CC->API.GetPointerv)(CC, pname, params);
}


void glGetPolygonStipple( GLubyte *mask )
{
   GET_CONTEXT;
   (*CC->API.GetPolygonStipple)(CC, mask);
}


const GLubyte *glGetString( GLenum name )
{
   GET_CONTEXT;
   return (*CC->API.GetString)(CC, name);
}



void glGetTexEnvfv( GLenum target, GLenum pname, GLfloat *params )
{
   GET_CONTEXT;
   (*CC->API.GetTexEnvfv)(CC, target, pname, params);
}


void glGetTexEnviv( GLenum target, GLenum pname, GLint *params )
{
   GET_CONTEXT;
   (*CC->API.GetTexEnviv)(CC, target, pname, params);
}


void glGetTexGeniv( GLenum coord, GLenum pname, GLint *params )
{
   GET_CONTEXT;
   (*CC->API.GetTexGeniv)(CC, coord, pname, params);
}


void glGetTexGendv( GLenum coord, GLenum pname, GLdouble *params )
{
   GET_CONTEXT;
   (*CC->API.GetTexGendv)(CC, coord, pname, params);
}


void glGetTexGenfv( GLenum coord, GLenum pname, GLfloat *params )
{
   GET_CONTEXT;
   (*CC->API.GetTexGenfv)(CC, coord, pname, params);
}



void glGetTexImage( GLenum target, GLint level, GLenum format,
      			GLenum type, GLvoid *pixels )
{
   GET_CONTEXT;
   (*CC->API.GetTexImage)(CC, target, level, format, type, pixels);
}


void glGetTexLevelParameterfv( GLenum target, GLint level,
                               GLenum pname, GLfloat *params )
{
   GET_CONTEXT;
   (*CC->API.GetTexLevelParameterfv)(CC, target, level, pname, params);
}


void glGetTexLevelParameteriv( GLenum target, GLint level,
                               GLenum pname, GLint *params )
{
   GET_CONTEXT;
   (*CC->API.GetTexLevelParameteriv)(CC, target, level, pname, params);
}




void glGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params)
{
   GET_CONTEXT;
   (*CC->API.GetTexParameterfv)(CC, target, pname, params);
}


void glGetTexParameteriv( GLenum target, GLenum pname, GLint *params )
{
   GET_CONTEXT;
   (*CC->API.GetTexParameteriv)(CC, target, pname, params);
}


void glHint( GLenum target, GLenum mode )
{
   GET_CONTEXT;
   (*CC->API.Hint)(CC, target, mode);
}


void glIndexd( GLdouble c )
{
   GET_CONTEXT;
   (*CC->API.Indexf)( CC, (GLfloat) c );
}


void glIndexf( GLfloat c )
{
   GET_CONTEXT;
   (*CC->API.Indexf)( CC, c );
}


void glIndexi( GLint c )
{
   GET_CONTEXT;
   (*CC->API.Indexi)( CC, c );
}


void glIndexs( GLshort c )
{
   GET_CONTEXT;
   (*CC->API.Indexi)( CC, (GLint) c );
}


#ifdef GL_VERSION_1_1
void glIndexub( GLubyte c )
{
   GET_CONTEXT;
   (*CC->API.Indexi)( CC, (GLint) c );
}
#endif


void glIndexdv( const GLdouble *c )
{
   GET_CONTEXT;
   (*CC->API.Indexf)( CC, (GLfloat) *c );
}


void glIndexfv( const GLfloat *c )
{
   GET_CONTEXT;
   (*CC->API.Indexf)( CC, *c );
}


void glIndexiv( const GLint *c )
{
   GET_CONTEXT;
   (*CC->API.Indexi)( CC, *c );
}


void glIndexsv( const GLshort *c )
{
   GET_CONTEXT;
   (*CC->API.Indexi)( CC, (GLint) *c );
}


#ifdef GL_VERSION_1_1
void glIndexubv( const GLubyte *c )
{
   GET_CONTEXT;
   (*CC->API.Indexi)( CC, (GLint) *c );
}
#endif


void glIndexMask( GLuint mask )
{
   GET_CONTEXT;
   (*CC->API.IndexMask)(CC, mask);
}


void glIndexPointer( GLenum type, GLsizei stride, const GLvoid *ptr )
{
   GET_CONTEXT;
   (*CC->API.IndexPointer)(CC, type, stride, ptr);
}


void glInterleavedArrays( GLenum format, GLsizei stride,
                          const GLvoid *pointer )
{
   GET_CONTEXT;
   (*CC->API.InterleavedArrays)( CC, format, stride, pointer );
}


void glInitNames( void )
{
   GET_CONTEXT;
   (*CC->API.InitNames)(CC);
}


GLboolean glIsList( GLuint list )
{
   GET_CONTEXT;
   return (*CC->API.IsList)(CC, list);
}


GLboolean glIsTexture( GLuint texture )
{
   GET_CONTEXT;
   return (*CC->API.IsTexture)(CC, texture);
}


void glLightf( GLenum light, GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   (*CC->API.Lightfv)( CC, light, pname, &param, 1 );
}



void glLighti( GLenum light, GLenum pname, GLint param )
{
   GLfloat fparam = (GLfloat) param;
   GET_CONTEXT;
   (*CC->API.Lightfv)( CC, light, pname, &fparam, 1 );
}



void glLightfv( GLenum light, GLenum pname, const GLfloat *params )
{
   GET_CONTEXT;
   (*CC->API.Lightfv)( CC, light, pname, params, 4 );
}



void glLightiv( GLenum light, GLenum pname, const GLint *params )
{
   GLfloat fparam[4];
   GET_CONTEXT;

   switch (pname) {
      case GL_AMBIENT:
      case GL_DIFFUSE:
      case GL_SPECULAR:
         fparam[0] = INT_TO_FLOAT( params[0] );
         fparam[1] = INT_TO_FLOAT( params[1] );
         fparam[2] = INT_TO_FLOAT( params[2] );
         fparam[3] = INT_TO_FLOAT( params[3] );
         break;
      case GL_POSITION:
         fparam[0] = (GLfloat) params[0];
         fparam[1] = (GLfloat) params[1];
         fparam[2] = (GLfloat) params[2];
         fparam[3] = (GLfloat) params[3];
         break;
      case GL_SPOT_DIRECTION:
         fparam[0] = (GLfloat) params[0];
         fparam[1] = (GLfloat) params[1];
         fparam[2] = (GLfloat) params[2];
         break;
      case GL_SPOT_EXPONENT:
      case GL_SPOT_CUTOFF:
      case GL_CONSTANT_ATTENUATION:
      case GL_LINEAR_ATTENUATION:
      case GL_QUADRATIC_ATTENUATION:
         fparam[0] = (GLfloat) params[0];
         break;
      default:
         /* error will be caught later in gl_Lightfv */
         ;
   }
   (*CC->API.Lightfv)( CC, light, pname, fparam, 4 );
}



void glLightModelf( GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   (*CC->API.LightModelfv)( CC, pname, &param );
}


void glLightModeli( GLenum pname, GLint param )
{
   GLfloat fparam[4];
   GET_CONTEXT;
   fparam[0] = (GLfloat) param;
   (*CC->API.LightModelfv)( CC, pname, fparam );
}


void glLightModelfv( GLenum pname, const GLfloat *params )
{
   GET_CONTEXT;
   (*CC->API.LightModelfv)( CC, pname, params );
}


void glLightModeliv( GLenum pname, const GLint *params )
{
   GLfloat fparam[4];
   GET_CONTEXT;

   switch (pname) {
      case GL_LIGHT_MODEL_AMBIENT:
         fparam[0] = INT_TO_FLOAT( params[0] );
         fparam[1] = INT_TO_FLOAT( params[1] );
         fparam[2] = INT_TO_FLOAT( params[2] );
         fparam[3] = INT_TO_FLOAT( params[3] );
         break;
      case GL_LIGHT_MODEL_LOCAL_VIEWER:
      case GL_LIGHT_MODEL_TWO_SIDE:
         fparam[0] = (GLfloat) params[0];
         break;
      default:
         /* Error will be caught later in gl_LightModelfv */
         ;
   }
   (*CC->API.LightModelfv)( CC, pname, fparam );
}


void glLineWidth( GLfloat width )
{
   GET_CONTEXT;
   (*CC->API.LineWidth)(CC, width);
}


void glLineStipple( GLint factor, GLushort pattern )
{
   GET_CONTEXT;
   (*CC->API.LineStipple)(CC, factor, pattern);
}


void glListBase( GLuint base )
{
   GET_CONTEXT;
   (*CC->API.ListBase)(CC, base);
}


void glLoadIdentity( void )
{
   static GLfloat identity[16] = {
      1.0, 0.0, 0.0, 0.0,
      0.0, 1.0, 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 0.0, 1.0
   };
   GET_CONTEXT;
   (*CC->API.LoadMatrixf)( CC, identity );
}


void glLoadMatrixd( const GLdouble *m )
{
   GLfloat fm[16];
   GLuint i;
   GET_CONTEXT;

   for (i=0;i<16;i++) {
      fm[i] = (GLfloat) m[i];
   }

   (*CC->API.LoadMatrixf)( CC, fm );
}


void glLoadMatrixf( const GLfloat *m )
{
   GET_CONTEXT;
   (*CC->API.LoadMatrixf)( CC, m );
}


void glLoadName( GLuint name )
{
   GET_CONTEXT;
   (*CC->API.LoadName)(CC, name);
}


void glLogicOp( GLenum opcode )
{
   GET_CONTEXT;
   (*CC->API.LogicOp)(CC, opcode);
}



void glMap1d( GLenum target, GLdouble u1, GLdouble u2, GLint stride,
              GLint order, const GLdouble *points )
{
   GLfloat *pnts;
   GLboolean retain;
   GET_CONTEXT;

   pnts = gl_copy_map_points1d( target, stride, order, points );
   retain = CC->CompileFlag;
   (*CC->API.Map1f)( CC, target, u1, u2, stride, order, pnts, retain );
}


void glMap1f( GLenum target, GLfloat u1, GLfloat u2, GLint stride,
              GLint order, const GLfloat *points )
{
   GLfloat *pnts;
   GLboolean retain;
   GET_CONTEXT;

   pnts = gl_copy_map_points1f( target, stride, order, points );
   retain = CC->CompileFlag;
   (*CC->API.Map1f)( CC, target, u1, u2, stride, order, pnts, retain );
}


void glMap2d( GLenum target,
              GLdouble u1, GLdouble u2, GLint ustride, GLint uorder,
              GLdouble v1, GLdouble v2, GLint vstride, GLint vorder,
              const GLdouble *points )
{
   GLfloat *pnts;
   GLboolean retain;
   GET_CONTEXT;

   pnts = gl_copy_map_points2d( target, ustride, uorder,
                                vstride, vorder, points );
   retain = CC->CompileFlag;
   (*CC->API.Map2f)( CC, target, u1, u2, ustride, uorder,
                     v1, v2, vstride, vorder, pnts, retain );
}


void glMap2f( GLenum target,
              GLfloat u1, GLfloat u2, GLint ustride, GLint uorder,
              GLfloat v1, GLfloat v2, GLint vstride, GLint vorder,
              const GLfloat *points )
{
   GLfloat *pnts;
   GLboolean retain;
   GET_CONTEXT;

   pnts = gl_copy_map_points2f( target, ustride, uorder,
                                vstride, vorder, points );
   retain = CC->CompileFlag;
   (*CC->API.Map2f)( CC, target, u1, u2, ustride, uorder,
                     v1, v2, vstride, vorder, pnts, retain );
}


void glMapGrid1d( GLint un, GLdouble u1, GLdouble u2 )
{
   GET_CONTEXT;
   (*CC->API.MapGrid1f)( CC, un, (GLfloat) u1, (GLfloat) u2 );
}


void glMapGrid1f( GLint un, GLfloat u1, GLfloat u2 )
{
   GET_CONTEXT;
   (*CC->API.MapGrid1f)( CC, un, u1, u2 );
}


void glMapGrid2d( GLint un, GLdouble u1, GLdouble u2,
          GLint vn, GLdouble v1, GLdouble v2 )
{
   GET_CONTEXT;
   (*CC->API.MapGrid2f)( CC, un, (GLfloat) u1, (GLfloat) u2,
                                  vn, (GLfloat) v1, (GLfloat) v2 );
}


void glMapGrid2f( GLint un, GLfloat u1, GLfloat u2,
          GLint vn, GLfloat v1, GLfloat v2 )
{
   GET_CONTEXT;
   (*CC->API.MapGrid2f)( CC, un, u1, u2, vn, v1, v2 );
}


void glMaterialf( GLenum face, GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   (*CC->API.Materialfv)( CC, face, pname, &param );
}



void glMateriali( GLenum face, GLenum pname, GLint param )
{
   GLfloat fparam[4];
   GET_CONTEXT;
   fparam[0] = (GLfloat) param;
   (*CC->API.Materialfv)( CC, face, pname, fparam );
}


void glMaterialfv( GLenum face, GLenum pname, const GLfloat *params )
{
   GET_CONTEXT;
   (*CC->API.Materialfv)( CC, face, pname, params );
}


void glMaterialiv( GLenum face, GLenum pname, const GLint *params )
{
   GLfloat fparam[4];
   GET_CONTEXT;
   switch (pname) {
      case GL_AMBIENT:
      case GL_DIFFUSE:
      case GL_SPECULAR:
      case GL_EMISSION:
      case GL_AMBIENT_AND_DIFFUSE:
         fparam[0] = INT_TO_FLOAT( params[0] );
         fparam[1] = INT_TO_FLOAT( params[1] );
         fparam[2] = INT_TO_FLOAT( params[2] );
         fparam[3] = INT_TO_FLOAT( params[3] );
         break;
      case GL_SHININESS:
         fparam[0] = (GLfloat) params[0];
         break;
      case GL_COLOR_INDEXES:
         fparam[0] = (GLfloat) params[0];
         fparam[1] = (GLfloat) params[1];
         fparam[2] = (GLfloat) params[2];
         break;
      default:
         /* Error will be caught later in gl_Materialfv */
         ;
   }
   (*CC->API.Materialfv)( CC, face, pname, fparam );
}


void glMatrixMode( GLenum mode )
{
   GET_CONTEXT;
   (*CC->API.MatrixMode)( CC, mode );
}


void glMultMatrixd( const GLdouble *m )
{
   GLfloat fm[16];
   GLuint i;
   GET_CONTEXT;

   for (i=0;i<16;i++) {
      fm[i] = (GLfloat) m[i];
   }

   (*CC->API.MultMatrixf)( CC, fm );
}


void glMultMatrixf( const GLfloat *m )
{
   GET_CONTEXT;
   (*CC->API.MultMatrixf)( CC, m );
}

