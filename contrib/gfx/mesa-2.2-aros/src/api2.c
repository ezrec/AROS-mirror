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

void glNewList( GLuint list, GLenum mode )
{
   GET_CONTEXT;
   (*CC->API.NewList)(CC, list, mode);
}

void glNormal3b( GLbyte nx, GLbyte ny, GLbyte nz )
{
   GET_CONTEXT;
   (*CC->API.Normal3f)( CC, BYTE_TO_FLOAT(nx),
                             BYTE_TO_FLOAT(ny), BYTE_TO_FLOAT(nz) );
}


void glNormal3d( GLdouble nx, GLdouble ny, GLdouble nz )
{
   GLfloat fx, fy, fz;
   GET_CONTEXT;
   if (ABSD(nx)<0.00001)   fx = 0.0F;   else  fx = nx;
   if (ABSD(ny)<0.00001)   fy = 0.0F;   else  fy = ny;
   if (ABSD(nz)<0.00001)   fz = 0.0F;   else  fz = nz;
   (*CC->API.Normal3f)( CC, fx, fy, fz );
}


void glNormal3f( GLfloat nx, GLfloat ny, GLfloat nz )
{
   GET_CONTEXT;
#ifdef SHORTCUT
        if (CC->CompileFlag) {
           (*CC->Save.Normal3f)( CC, nx, ny, nz );
           if (CC->ExecuteFlag) {
              CC->Current.Normal[0] = nx;
              CC->Current.Normal[1] = ny;
              CC->Current.Normal[2] = nz;
           }
        }
        else {
           /* Execute */
           CC->Current.Normal[0] = nx;
           CC->Current.Normal[1] = ny;
           CC->Current.Normal[2] = nz;
        }
#else
   (*CC->API.Normal3f)( CC, nx, ny, nz );
#endif
}


void glNormal3i( GLint nx, GLint ny, GLint nz )
{
   GET_CONTEXT;
   (*CC->API.Normal3f)( CC, INT_TO_FLOAT(nx),
                             INT_TO_FLOAT(ny), INT_TO_FLOAT(nz) );
}


void glNormal3s( GLshort nx, GLshort ny, GLshort nz )
{
   GET_CONTEXT;
   (*CC->API.Normal3f)( CC, SHORT_TO_FLOAT(nx),
                             SHORT_TO_FLOAT(ny), SHORT_TO_FLOAT(nz) );
}


void glNormal3bv( const GLbyte *v )
{
   GET_CONTEXT;
   (*CC->API.Normal3f)( CC, BYTE_TO_FLOAT(v[0]),
                             BYTE_TO_FLOAT(v[1]), BYTE_TO_FLOAT(v[2]) );
}


void glNormal3dv( const GLdouble *v )
{
   GLfloat fx, fy, fz;
   GET_CONTEXT;
   if (ABSD(v[0])<0.00001)   fx = 0.0F;   else  fx = v[0];
   if (ABSD(v[1])<0.00001)   fy = 0.0F;   else  fy = v[1];
   if (ABSD(v[2])<0.00001)   fz = 0.0F;   else  fz = v[2];
   (*CC->API.Normal3f)( CC, fx, fy, fz );
}


void glNormal3fv( const GLfloat *v )
{
   GET_CONTEXT;
#ifdef SHORTCUT
        if (CC->CompileFlag) {
           (*CC->Save.Normal3fv)( CC, v );
           if (CC->ExecuteFlag) {
              CC->Current.Normal[0] = v[0];
              CC->Current.Normal[1] = v[1];
              CC->Current.Normal[2] = v[2];
           }
        }
        else {
           /* Execute */
           GLfloat *n = CC->Current.Normal;
           n[0] = v[0];
           n[1] = v[1];
           n[2] = v[2];
        }
#else
   (*CC->API.Normal3fv)( CC, v );
#endif
}


void glNormal3iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.Normal3f)( CC, INT_TO_FLOAT(v[0]),
                             INT_TO_FLOAT(v[1]), INT_TO_FLOAT(v[2]) );
}


void glNormal3sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.Normal3f)( CC, SHORT_TO_FLOAT(v[0]),
                             SHORT_TO_FLOAT(v[1]), SHORT_TO_FLOAT(v[2]) );
}


void glNormalPointer( GLenum type, GLsizei stride, const GLvoid *ptr )
{
   GET_CONTEXT;
   (*CC->API.NormalPointer)(CC, type, stride, ptr);
}


void glOrtho( GLdouble left, GLdouble right,
              GLdouble bottom, GLdouble top,
              GLdouble nearval, GLdouble farval )
{
   GLfloat x, y, z;
   GLfloat tx, ty, tz;
   GLfloat m[16];
   GET_CONTEXT;

   x = 2.0 / (right-left);
   y = 2.0 / (top-bottom);
   z = -2.0 / (farval-nearval);
   tx = -(right+left) / (right-left);
   ty = -(top+bottom) / (top-bottom);
   tz = -(farval+nearval) / (farval-nearval);

#define M(row,col)  m[col*4+row]
   M(0,0) = x;     M(0,1) = 0.0F;  M(0,2) = 0.0F;  M(0,3) = tx;
   M(1,0) = 0.0F;  M(1,1) = y;     M(1,2) = 0.0F;  M(1,3) = ty;
   M(2,0) = 0.0F;  M(2,1) = 0.0F;  M(2,2) = z;     M(2,3) = tz;
   M(3,0) = 0.0F;  M(3,1) = 0.0F;  M(3,2) = 0.0F;  M(3,3) = 1.0F;
#undef M

   (*CC->API.MultMatrixf)( CC, m );
}


void glPassThrough( GLfloat token )
{
   GET_CONTEXT;
   (*CC->API.PassThrough)(CC, token);
}


void glPixelMapfv( GLenum map, GLint mapsize, const GLfloat *values )
{
   GET_CONTEXT;
   (*CC->API.PixelMapfv)( CC, map, mapsize, values );
}


void glPixelMapuiv( GLenum map, GLint mapsize, const GLuint *values )
{
   GLfloat fvalues[MAX_PIXEL_MAP_TABLE];
   GLuint i;
   GET_CONTEXT;

   if (map==GL_PIXEL_MAP_I_TO_I || map==GL_PIXEL_MAP_S_TO_S) {
      for (i=0;i<mapsize;i++) {
    fvalues[i] = (GLfloat) values[i];
      }
   }
   else {
      for (i=0;i<mapsize;i++) {
    fvalues[i] = UINT_TO_FLOAT( values[i] );
      }
   }
   (*CC->API.PixelMapfv)( CC, map, mapsize, fvalues );
}



void glPixelMapusv( GLenum map, GLint mapsize, const GLushort *values )
{
   GLfloat fvalues[MAX_PIXEL_MAP_TABLE];
   GLuint i;
   GET_CONTEXT;

   if (map==GL_PIXEL_MAP_I_TO_I || map==GL_PIXEL_MAP_S_TO_S) {
      for (i=0;i<mapsize;i++) {
    fvalues[i] = (GLfloat) values[i];
      }
   }
   else {
      for (i=0;i<mapsize;i++) {
    fvalues[i] = USHORT_TO_FLOAT( values[i] );
      }
   }
   (*CC->API.PixelMapfv)( CC, map, mapsize, fvalues );
}


void glPixelStoref( GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   (*CC->API.PixelStorei)( CC, pname, (GLint) param );
}


void glPixelStorei( GLenum pname, GLint param )
{
   GET_CONTEXT;
   (*CC->API.PixelStorei)( CC, pname, param );
}


void glPixelTransferf( GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   (*CC->API.PixelTransferf)(CC, pname, param);
}


void glPixelTransferi( GLenum pname, GLint param )
{
   GET_CONTEXT;
   (*CC->API.PixelTransferf)(CC, pname, (GLfloat) param);
}


void glPixelZoom( GLfloat xfactor, GLfloat yfactor )
{
   GET_CONTEXT;
   (*CC->API.PixelZoom)(CC, xfactor, yfactor);
}


void glPointSize( GLfloat size )
{
   GET_CONTEXT;
   (*CC->API.PointSize)(CC, size);
}


void glPolygonMode( GLenum face, GLenum mode )
{
   GET_CONTEXT;
   (*CC->API.PolygonMode)(CC, face, mode);
}


void glPolygonOffset( GLfloat factor, GLfloat units )
{
   GET_CONTEXT;
   (*CC->API.PolygonOffset)( CC, factor, units );
}


void glPolygonStipple( const GLubyte *mask )
{
   GET_CONTEXT;
   (*CC->API.PolygonStipple)(CC, mask);
}


void glPopAttrib( void )
{
   GET_CONTEXT;
   (*CC->API.PopAttrib)(CC);
}


void glPopClientAttrib( void )
{
   GET_CONTEXT;
   (*CC->API.PopClientAttrib)(CC);
}


void glPopMatrix( void )
{
   GET_CONTEXT;
   (*CC->API.PopMatrix)( CC );
}


void glPopName( void )
{
   GET_CONTEXT;
   (*CC->API.PopName)(CC);
}


void glPrioritizeTextures( GLsizei n, const GLuint *textures,
                           const GLclampf *priorities )
{
   GET_CONTEXT;
   (*CC->API.PrioritizeTextures)(CC, n, textures, priorities);
}


void glPushMatrix( void )
{
   GET_CONTEXT;
   (*CC->API.PushMatrix)( CC );
}


void glRasterPos2d( GLdouble x, GLdouble y )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}


void glRasterPos2f( GLfloat x, GLfloat y )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}


void glRasterPos2i( GLint x, GLint y )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}


void glRasterPos2s( GLshort x, GLshort y )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}


void glRasterPos3d( GLdouble x, GLdouble y, GLdouble z )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}


void glRasterPos3f( GLfloat x, GLfloat y, GLfloat z )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}


void glRasterPos3i( GLint x, GLint y, GLint z )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}


void glRasterPos3s( GLshort x, GLshort y, GLshort z )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}


void glRasterPos4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y,
                               (GLfloat) z, (GLfloat) w );
}


void glRasterPos4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, x, y, z, w );
}


void glRasterPos4i( GLint x, GLint y, GLint z, GLint w )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y,
                               (GLfloat) z, (GLfloat) w );
}


void glRasterPos4s( GLshort x, GLshort y, GLshort z, GLshort w )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y,
                               (GLfloat) z, (GLfloat) w );
}


void glRasterPos2dv( const GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1], 0.0F, 1.0F );
}


void glRasterPos2fv( const GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1], 0.0F, 1.0F );
}


void glRasterPos2iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1], 0.0F, 1.0F );
}


void glRasterPos2sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1], 0.0F, 1.0F );
}


/*** 3 element vector ***/

void glRasterPos3dv( const GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], 1.0F );
}


void glRasterPos3fv( const GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], 1.0F );
}


void glRasterPos3iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], 1.0F );
}


void glRasterPos3sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], 1.0F );
}


void glRasterPos4dv( const GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], (GLfloat) v[3] );
}


void glRasterPos4fv( const GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, v[0], v[1], v[2], v[3] );
}


void glRasterPos4iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], (GLfloat) v[3] );
}


void glRasterPos4sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], (GLfloat) v[3] );
}


void glReadBuffer( GLenum mode )
{
   GET_CONTEXT;
   (*CC->API.ReadBuffer)( CC, mode );
}


void glReadPixels( GLint x, GLint y, GLsizei width, GLsizei height,
           GLenum format, GLenum type, GLvoid *pixels )
{
   GET_CONTEXT;
   (*CC->API.ReadPixels)( CC, x, y, width, height, format, type, pixels );
}


void glRectd( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 )
{
   GET_CONTEXT;
   (*CC->API.Rectf)( CC, (GLfloat) x1, (GLfloat) y1,
                    (GLfloat) x2, (GLfloat) y2 );
}


void glRectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 )
{
   GET_CONTEXT;
   (*CC->API.Rectf)( CC, x1, y1, x2, y2 );
}


void glRecti( GLint x1, GLint y1, GLint x2, GLint y2 )
{
   GET_CONTEXT;
   (*CC->API.Rectf)( CC, (GLfloat) x1, (GLfloat) y1,
                         (GLfloat) x2, (GLfloat) y2 );
}


void glRects( GLshort x1, GLshort y1, GLshort x2, GLshort y2 )
{
   GET_CONTEXT;
   (*CC->API.Rectf)( CC, (GLfloat) x1, (GLfloat) y1,
                     (GLfloat) x2, (GLfloat) y2 );
}


void glRectdv( const GLdouble *v1, const GLdouble *v2 )
{
   GET_CONTEXT;
   (*CC->API.Rectf)(CC, (GLfloat) v1[0], (GLfloat) v1[1],
                    (GLfloat) v2[0], (GLfloat) v2[1]);
}


void glRectfv( const GLfloat *v1, const GLfloat *v2 )
{
   GET_CONTEXT;
   (*CC->API.Rectf)(CC, v1[0], v1[1], v2[0], v2[1]);
}


void glRectiv( const GLint *v1, const GLint *v2 )
{
   GET_CONTEXT;
   (*CC->API.Rectf)( CC, (GLfloat) v1[0], (GLfloat) v1[1],
                     (GLfloat) v2[0], (GLfloat) v2[1] );
}


void glRectsv( const GLshort *v1, const GLshort *v2 )
{
   GET_CONTEXT;
   (*CC->API.Rectf)(CC, (GLfloat) v1[0], (GLfloat) v1[1],
        (GLfloat) v2[0], (GLfloat) v2[1]);
}


void glScissor( GLint x, GLint y, GLsizei width, GLsizei height)
{
   GET_CONTEXT;
   (*CC->API.Scissor)(CC, x, y, width, height);
}


GLboolean glIsEnabled( GLenum cap )
{
   GET_CONTEXT;
   return (*CC->API.IsEnabled)( CC, cap );
}



void glPushAttrib( GLbitfield mask )
{
   GET_CONTEXT;
   (*CC->API.PushAttrib)(CC, mask);
}


void glPushClientAttrib( GLbitfield mask )
{
   GET_CONTEXT;
   (*CC->API.PushClientAttrib)(CC, mask);
}


void glPushName( GLuint name )
{
   GET_CONTEXT;
   (*CC->API.PushName)(CC, name);
}


GLint glRenderMode( GLenum mode )
{
   GET_CONTEXT;
   return (*CC->API.RenderMode)(CC, mode);
}


void glRotated( GLdouble angle, GLdouble x, GLdouble y, GLdouble z )
{
   GET_CONTEXT;
   (*CC->API.Rotatef)( CC, (GLfloat) angle,
                       (GLfloat) x, (GLfloat) y, (GLfloat) z );
}


void glRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z )
{
   GET_CONTEXT;
   (*CC->API.Rotatef)( CC, angle, x, y, z );
}


void glSelectBuffer( GLsizei size, GLuint *buffer )
{
   GET_CONTEXT;
   (*CC->API.SelectBuffer)(CC, size, buffer);
}


void glScaled( GLdouble x, GLdouble y, GLdouble z )
{
   GET_CONTEXT;
   (*CC->API.Scalef)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z );
}


void glScalef( GLfloat x, GLfloat y, GLfloat z )
{
   GET_CONTEXT;
   (*CC->API.Scalef)( CC, x, y, z );
}


void glShadeModel( GLenum mode )
{
   GET_CONTEXT;
   (*CC->API.ShadeModel)(CC, mode);
}


void glStencilFunc( GLenum func, GLint ref, GLuint mask )
{
   GET_CONTEXT;
   (*CC->API.StencilFunc)(CC, func, ref, mask);
}


void glStencilMask( GLuint mask )
{
   GET_CONTEXT;
   (*CC->API.StencilMask)(CC, mask);
}


void glStencilOp( GLenum fail, GLenum zfail, GLenum zpass )
{
   GET_CONTEXT;
   (*CC->API.StencilOp)(CC, fail, zfail, zpass);
}


void glTexCoord1d( GLdouble s )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, 0.0, 0.0, 1.0 );
}


void glTexCoord1f( GLfloat s )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, s, 0.0, 0.0, 1.0 );
}


void glTexCoord1i( GLint s )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, 0.0, 0.0, 1.0 );
}


void glTexCoord1s( GLshort s )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, 0.0, 0.0, 1.0 );
}


void glTexCoord2d( GLdouble s, GLdouble t )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, (GLfloat) t, 0.0, 1.0 );
}


void glTexCoord2f( GLfloat s, GLfloat t )
{
   GET_CONTEXT;
#ifdef SHORTCUT
        if (CC->CompileFlag) {
           (*CC->Save.TexCoord4f)( CC, s, t, 0.0F, 1.0F );
           if (CC->ExecuteFlag) {
              CC->Current.TexCoord[0] = s;
              CC->Current.TexCoord[1] = t;
              CC->Current.TexCoord[2] = 0.0F;
              CC->Current.TexCoord[3] = 1.0F;
           }
        }
        else {
           /* Execute */
           GLfloat *tex = CC->Current.TexCoord;
           tex[0] = s;
           tex[1] = t;
           tex[2] = 0.0F;
           tex[3] = 1.0F;
        }
#else
   (*CC->API.TexCoord4f)( CC, s, tex, 0.0, 1.0 );
#endif
}


void glTexCoord2i( GLint s, GLint t )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, (GLfloat) t, 0.0, 1.0 );
}


void glTexCoord2s( GLshort s, GLshort t )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, (GLfloat) t, 0.0, 1.0 );
}


void glTexCoord3d( GLdouble s, GLdouble t, GLdouble r )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, (GLfloat) t, (GLfloat) r, 1.0 );
}


void glTexCoord3f( GLfloat s, GLfloat t, GLfloat r )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, s, t, r, 1.0 );
}


void glTexCoord3i( GLint s, GLint t, GLint r )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, (GLfloat) t,
                               (GLfloat) r, 1.0 );
}


void glTexCoord3s( GLshort s, GLshort t, GLshort r )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, (GLfloat) t,
                               (GLfloat) r, 1.0 );
}


void glTexCoord4d( GLdouble s, GLdouble t, GLdouble r, GLdouble q )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, (GLfloat) t,
                               (GLfloat) r, (GLfloat) q );
}


void glTexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, s, t, r, q );
}


void glTexCoord4i( GLint s, GLint t, GLint r, GLint q )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, (GLfloat) t,
                               (GLfloat) r, (GLfloat) q );
}


void glTexCoord4s( GLshort s, GLshort t, GLshort r, GLshort q )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, (GLfloat) t,
                               (GLfloat) r, (GLfloat) q );
}


void glTexCoord1dv( const GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) *v, 0.0, 0.0, 1.0 );
}


void glTexCoord1fv( const GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, *v, 0.0, 0.0, 1.0 );
}


void glTexCoord1iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, *v, 0.0, 0.0, 1.0 );
}


void glTexCoord1sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) *v, 0.0, 0.0, 1.0 );
}


void glTexCoord2dv( const GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) v[0], (GLfloat) v[1], 0.0, 1.0 );
}


void glTexCoord2fv( const GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, v[0], v[1], 0.0, 1.0 );
}


void glTexCoord2iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) v[0], (GLfloat) v[1], 0.0, 1.0 );
}


void glTexCoord2sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) v[0], (GLfloat) v[1], 0.0, 1.0 );
}


void glTexCoord3dv( const GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], 1.0 );
}


void glTexCoord3fv( const GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, v[0], v[1], v[2], 1.0 );
}


void glTexCoord3iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                          (GLfloat) v[2], 1.0 );
}


void glTexCoord3sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], 1.0 );
}


void glTexCoord4dv( const GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], (GLfloat) v[3] );
}


void glTexCoord4fv( const GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, v[0], v[1], v[2], v[3] );
}


void glTexCoord4iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], (GLfloat) v[3] );
}


void glTexCoord4sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], (GLfloat) v[3] );
}


void glTexCoordPointer( GLint size, GLenum type, GLsizei stride,
                        const GLvoid *ptr )
{
   GET_CONTEXT;
   (*CC->API.TexCoordPointer)(CC, size, type, stride, ptr);
}


void glTexGend( GLenum coord, GLenum pname, GLdouble param )
{
   GLfloat p = (GLfloat) param;
   GET_CONTEXT;
   (*CC->API.TexGenfv)( CC, coord, pname, &p );
}


void glTexGenf( GLenum coord, GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   (*CC->API.TexGenfv)( CC, coord, pname, &param );
}


void glTexGeni( GLenum coord, GLenum pname, GLint param )
{
   GLfloat p = (GLfloat) param;
   GET_CONTEXT;
   (*CC->API.TexGenfv)( CC, coord, pname, &p );
}


void glTexGendv( GLenum coord, GLenum pname, const GLdouble *params )
{
   GLfloat p[4];
   GET_CONTEXT;
   p[0] = params[0];
   p[1] = params[1];
   p[2] = params[2];
   p[3] = params[3];
   (*CC->API.TexGenfv)( CC, coord, pname, p );
}


void glTexGeniv( GLenum coord, GLenum pname, const GLint *params )
{
   GLfloat p[4];
   GET_CONTEXT;
   p[0] = params[0];
   p[1] = params[1];
   p[2] = params[2];
   p[3] = params[3];
   (*CC->API.TexGenfv)( CC, coord, pname, p );
}


void glTexGenfv( GLenum coord, GLenum pname, const GLfloat *params )
{
   GET_CONTEXT;
   (*CC->API.TexGenfv)( CC, coord, pname, params );
}




void glTexEnvf( GLenum target, GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   (*CC->API.TexEnvfv)( CC, target, pname, &param );
}



void glTexEnvi( GLenum target, GLenum pname, GLint param )
{
   GLfloat p = (GLfloat) param;
   GET_CONTEXT;
   (*CC->API.TexEnvfv)( CC, target, pname, &p );
}



void glTexEnvfv( GLenum target, GLenum pname, const GLfloat *param )
{
   GET_CONTEXT;
   (*CC->API.TexEnvfv)( CC, target, pname, param );
}



void glTexEnviv( GLenum target, GLenum pname, const GLint *param )
{
   GLfloat p[4];
   GET_CONTEXT;
   p[0] = INT_TO_FLOAT( param[0] );
   p[1] = INT_TO_FLOAT( param[1] );
   p[2] = INT_TO_FLOAT( param[2] );
   p[3] = INT_TO_FLOAT( param[3] );
   (*CC->API.TexEnvfv)( CC, target, pname, p );
}


void glTexImage1D( GLenum target, GLint level, GLint internalformat,
                   GLsizei width, GLint border,
                   GLenum format, GLenum type, const GLvoid *pixels )
{
   struct gl_texture_image *teximage;
   GET_CONTEXT;
   teximage = gl_unpack_texture( CC, 1, target, level, internalformat,
                                 width, 1, border, format, type, pixels );
   (*CC->API.TexImage1D)( CC, target, level, internalformat,
                          width, border, format, type, teximage );
}



void glTexImage2D( GLenum target, GLint level, GLint internalformat,
                   GLsizei width, GLsizei height, GLint border,
                   GLenum format, GLenum type, const GLvoid *pixels )
{
   struct gl_texture_image *teximage;
   GET_CONTEXT;
   teximage = gl_unpack_texture( CC, 2, target, level, internalformat,
                                 width, height, border, format, type, pixels );
   (*CC->API.TexImage2D)( CC, target, level, internalformat,
                          width, height, border, format, type, teximage );
}


void glTexParameterf( GLenum target, GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   (*CC->API.TexParameterfv)( CC, target, pname, &param );
}


void glTexParameteri( GLenum target, GLenum pname, GLint param )
{
   GLfloat fparam = param;
   GET_CONTEXT;
   (*CC->API.TexParameterfv)( CC, target, pname, &fparam );
}


void glTexParameterfv( GLenum target, GLenum pname, const GLfloat *params )
{
   GET_CONTEXT;
   (*CC->API.TexParameterfv)( CC, target, pname, params );
}


void glTexParameteriv( GLenum target, GLenum pname, const GLint *params )
{
   GLfloat p[4];
   GET_CONTEXT;
   if (pname==GL_TEXTURE_BORDER_COLOR) {
      p[0] = INT_TO_FLOAT( params[0] );
      p[1] = INT_TO_FLOAT( params[1] );
      p[2] = INT_TO_FLOAT( params[2] );
      p[3] = INT_TO_FLOAT( params[3] );
   }
   else {
      p[0] = (GLfloat) params[0];
      p[1] = (GLfloat) params[1];
      p[2] = (GLfloat) params[2];
      p[3] = (GLfloat) params[3];
   }
   (*CC->API.TexParameterfv)( CC, target, pname, p );
}


void glTexSubImage1D( GLenum target, GLint level, GLint xoffset,
                      GLsizei width, GLenum format,
                      GLenum type, const GLvoid *pixels )
{
   struct gl_image *image;
   GET_CONTEXT;
   image = gl_unpack_texsubimage( CC, width, 1, format, type, pixels );
   (*CC->API.TexSubImage1D)( CC, target, level, xoffset, width,
                             format, type, image );
}


void glTexSubImage2D( GLenum target, GLint level,
                      GLint xoffset, GLint yoffset,
                      GLsizei width, GLsizei height,
                      GLenum format, GLenum type,
                      const GLvoid *pixels )
{
   struct gl_image *image;
   GET_CONTEXT;
   image = gl_unpack_texsubimage( CC, width, height, format, type, pixels );
   (*CC->API.TexSubImage2D)( CC, target, level, xoffset, yoffset,
                             width, height, format, type, image );
}


void glTranslated( GLdouble x, GLdouble y, GLdouble z )
{
   GET_CONTEXT;
   (*CC->API.Translatef)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z );
}


void glTranslatef( GLfloat x, GLfloat y, GLfloat z )
{
   GET_CONTEXT;
   (*CC->API.Translatef)( CC, x, y, z );
}


void glVertex2d( GLdouble x, GLdouble y )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}


void glVertex2f( GLfloat x, GLfloat y )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, x, y, 0.0F, 1.0F );
}


void glVertex2i( GLint x, GLint y )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}


void glVertex2s( GLshort x, GLshort y )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}


void glVertex3d( GLdouble x, GLdouble y, GLdouble z )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}


void glVertex3f( GLfloat x, GLfloat y, GLfloat z )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, x, y, z, 1.0F );
}


void glVertex3i( GLint x, GLint y, GLint z )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}


void glVertex3s( GLshort x, GLshort y, GLshort z )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}


void glVertex4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) x, (GLfloat) y,
                            (GLfloat) z, (GLfloat) w );
}


void glVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, x, y, z, w );
}


void glVertex4i( GLint x, GLint y, GLint z, GLint w )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) x, (GLfloat) y,
                            (GLfloat) z, (GLfloat) w );
}


void glVertex4s( GLshort x, GLshort y, GLshort z, GLshort w )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) x, (GLfloat) y,
                            (GLfloat) z, (GLfloat) w );
}


void glVertex2dv( const GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) v[0], (GLfloat) v[1], 0.0F, 1.0F );
}


void glVertex2fv( const GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, v[0], v[1], 0.0F, 1.0F );
}


void glVertex2iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) v[0], (GLfloat) v[1], 0.0F, 1.0F );
}


void glVertex2sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) v[0], (GLfloat) v[1], 0.0F, 1.0F );
}


void glVertex3dv( const GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                            (GLfloat) v[2], 1.0F );
}


void glVertex3fv( const GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, v[0], v[1], v[2], 1.0F );
}


void glVertex3iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                            (GLfloat) v[2], 1.0F );
}


void glVertex3sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                            (GLfloat) v[2], 1.0F );
}


void glVertex4dv( const GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                            (GLfloat) v[2], (GLfloat) v[3] );
}


void glVertex4fv( const GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, v[0], v[1], v[2], v[3] );
}


void glVertex4iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                  (GLfloat) v[2], (GLfloat) v[3] );
}


void glVertex4sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                            (GLfloat) v[2], (GLfloat) v[3] );
}


void glVertexPointer( GLint size, GLenum type, GLsizei stride,
                      const GLvoid *ptr )
{
   GET_CONTEXT;
   (*CC->API.VertexPointer)(CC, size, type, stride, ptr);
}


void glViewport( GLint x, GLint y, GLsizei width, GLsizei height )
{
//	printf("glViewport(...) inside api \n");
   GET_CONTEXT;
   (*CC->API.Viewport)( CC, x, y, width, height );
//	printf("glViewport(...) inside api done\n");

}



/**
 ** Extensions
 **
 ** Some of these are incorporated into the 1.1 API.  They also remain as
 ** extensions for backward compatibility.  Should be removed in the future.
 **/


/* GL_EXT_blend_minmax */

void glBlendEquationEXT( GLenum mode )
{
   GET_CONTEXT;
   (*CC->API.BlendEquation)(CC, mode);
}


/* GL_EXT_blend_color */

void glBlendColorEXT( GLclampf red, GLclampf green,
                      GLclampf blue, GLclampf alpha )
{
   GET_CONTEXT;
   (*CC->API.BlendColor)(CC, red, green, blue, alpha);
}


/* GL_EXT_vertex_array */
/* TOOD: verify these work */
void glVertexPointerEXT( GLint size, GLenum type, GLsizei stride,
                         GLsizei count, const GLvoid *ptr )
{
   GET_CONTEXT;
   (*CC->API.VertexPointer)(CC, size, type, stride, ptr);
}


void glNormalPointerEXT( GLenum type, GLsizei stride, GLsizei count,
                         const GLvoid *ptr )
{
   GET_CONTEXT;
   (*CC->API.NormalPointer)(CC, type, stride, ptr);
}


void glColorPointerEXT( GLint size, GLenum type, GLsizei stride,
                        GLsizei count, const GLvoid *ptr )
{
   GET_CONTEXT;
   (*CC->API.ColorPointer)(CC, size, type, stride, ptr);
}


void glIndexPointerEXT( GLenum type, GLsizei stride,
                        GLsizei count, const GLvoid *ptr )
{
   GET_CONTEXT;
   (*CC->API.IndexPointer)(CC, type, stride, ptr);
}


void glTexCoordPointerEXT( GLint size, GLenum type, GLsizei stride,
                           GLsizei count, const GLvoid *ptr )
{
   GET_CONTEXT;
   (*CC->API.TexCoordPointer)(CC, size, type, stride, ptr);
}


void glEdgeFlagPointerEXT( GLsizei stride, GLsizei count,
                           const GLboolean *ptr )
{
   GET_CONTEXT;
   (*CC->API.EdgeFlagPointer)(CC, stride, ptr);
}


void glGetPointervEXT( GLenum pname, GLvoid **params )
{
   GET_CONTEXT;
   (*CC->API.GetPointerv)(CC, pname, params);
}


void glArrayElementEXT( GLint i )
{
   GET_CONTEXT;
   (*CC->API.ArrayElement)(CC, i);
}


void glDrawArraysEXT( GLenum mode, GLint first, GLsizei count )
{
   GET_CONTEXT;
   (*CC->API.DrawArrays)(CC, mode, first, count);
}





#ifdef GL_MESA_window_pos
/*
 * Mesa implementation of glWindowPos*MESA()
 */
void glWindowPos4fMESA( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   GET_CONTEXT;
   (*CC->API.WindowPos4fMESA)( CC, x, y, z, w );
}
#else
/* Implementation in winpos.c is used */
#endif


void glWindowPos2iMESA( GLint x, GLint y )
{
   glWindowPos4fMESA( (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}

void glWindowPos2sMESA( GLshort x, GLshort y )
{
   glWindowPos4fMESA( (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}

void glWindowPos2fMESA( GLfloat x, GLfloat y )
{
   glWindowPos4fMESA( x, y, 0.0F, 1.0F );
}

void glWindowPos2dMESA( GLdouble x, GLdouble y )
{
   glWindowPos4fMESA( (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}

void glWindowPos2ivMESA( const GLint *p )
{
   glWindowPos4fMESA( (GLfloat) p[0], (GLfloat) p[1], 0.0F, 1.0F );
}

void glWindowPos2svMESA( const GLshort *p )
{
   glWindowPos4fMESA( (GLfloat) p[0], (GLfloat) p[1], 0.0F, 1.0F );
}

void glWindowPos2fvMESA( const GLfloat *p )
{
   glWindowPos4fMESA( p[0], p[1], 0.0F, 1.0F );
}

void glWindowPos2dvMESA( const GLdouble *p )
{
   glWindowPos4fMESA( (GLfloat) p[0], (GLfloat) p[1], 0.0F, 1.0F );
}

void glWindowPos3iMESA( GLint x, GLint y, GLint z )
{
   glWindowPos4fMESA( (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}

void glWindowPos3sMESA( GLshort x, GLshort y, GLshort z )
{
   glWindowPos4fMESA( (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}

void glWindowPos3fMESA( GLfloat x, GLfloat y, GLfloat z )
{
   glWindowPos4fMESA( x, y, z, 1.0F );
}

void glWindowPos3dMESA( GLdouble x, GLdouble y, GLdouble z )
{
   glWindowPos4fMESA( (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}

void glWindowPos3ivMESA( const GLint *p )
{
   glWindowPos4fMESA( (GLfloat) p[0], (GLfloat) p[1], (GLfloat) p[2], 1.0F );
}

void glWindowPos3svMESA( const GLshort *p )
{
   glWindowPos4fMESA( (GLfloat) p[0], (GLfloat) p[1], (GLfloat) p[2], 1.0F );
}

void glWindowPos3fvMESA( const GLfloat *p )
{
   glWindowPos4fMESA( p[0], p[1], p[2], 1.0F );
}

void glWindowPos3dvMESA( const GLdouble *p )
{
   glWindowPos4fMESA( (GLfloat) p[0], (GLfloat) p[1], (GLfloat) p[2], 1.0F );
}

void glWindowPos4iMESA( GLint x, GLint y, GLint z, GLint w )
{
   glWindowPos4fMESA( (GLfloat) x, (GLfloat) y, (GLfloat) z, (GLfloat) w );
}

void glWindowPos4sMESA( GLshort x, GLshort y, GLshort z, GLshort w )
{
   glWindowPos4fMESA( (GLfloat) x, (GLfloat) y, (GLfloat) z, (GLfloat) w );
}

void glWindowPos4dMESA( GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
   glWindowPos4fMESA( (GLfloat) x, (GLfloat) y, (GLfloat) z, (GLfloat) w );
}


void glWindowPos4ivMESA( const GLint *p )
{
   glWindowPos4fMESA( (GLfloat) p[0], (GLfloat) p[1],
                      (GLfloat) p[2], (GLfloat) p[3] );
}

void glWindowPos4svMESA( const GLshort *p )
{
   glWindowPos4fMESA( (GLfloat) p[0], (GLfloat) p[1],
                      (GLfloat) p[2], (GLfloat) p[3] );
}

void glWindowPos4fvMESA( const GLfloat *p )
{
   glWindowPos4fMESA( p[0], p[1], p[2], p[3] );
}

void glWindowPos4dvMESA( const GLdouble *p )
{
   glWindowPos4fMESA( (GLfloat) p[0], (GLfloat) p[1],
                      (GLfloat) p[2], (GLfloat) p[3] );
}

