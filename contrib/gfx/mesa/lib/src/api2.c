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
 * Revision 3.6  1998/08/21 02:43:52  brianp
 * implemented true packing/unpacking of polygon stipples
 *
 * Revision 3.5  1998/07/26 02:32:43  brianp
 * removed unused variable
 *
 * Revision 3.4  1998/03/27 03:30:36  brianp
 * fixed G++ warnings
 *
 * Revision 3.3  1998/03/10 01:26:57  brianp
 * updated for David's v0.23 fxmesa driver
 *
 * Revision 3.2  1998/02/20 04:49:19  brianp
 * move extension functions into apiext.c
 *
 * Revision 3.1  1998/02/06 01:57:42  brianp
 * added GL 1.2 3-D texture enums and functions
 *
 * Revision 3.0  1998/01/31 20:43:12  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <stdio.h>
#include <stdlib.h>
#include "api.h"
#include "context.h"
#include "image.h"
#include "macros.h"
#include "matrix.h"
#include "teximage.h"
#include "types.h"
#include "vb.h"
#endif


/*
 * Part 2 of API functions
 */


void APIENTRY glOrtho( GLdouble left, GLdouble right,
              GLdouble bottom, GLdouble top,
              GLdouble nearval, GLdouble farval )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Ortho)(CC, left, right, bottom, top, nearval, farval);
}


void APIENTRY glPassThrough( GLfloat token )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PassThrough)(CC, token);
}


void APIENTRY glPixelMapfv( GLenum map, GLint mapsize, const GLfloat *values )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PixelMapfv)( CC, map, mapsize, values );
}


void APIENTRY glPixelMapuiv( GLenum map, GLint mapsize, const GLuint *values )
{
   GLfloat fvalues[MAX_PIXEL_MAP_TABLE];
   GLint i;
   GET_CONTEXT;
   CHECK_CONTEXT;

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



void APIENTRY glPixelMapusv( GLenum map, GLint mapsize, const GLushort *values )
{
   GLfloat fvalues[MAX_PIXEL_MAP_TABLE];
   GLint i;
   GET_CONTEXT;
   CHECK_CONTEXT;

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


void APIENTRY glPixelStoref( GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PixelStorei)( CC, pname, (GLint) param );
}


void APIENTRY glPixelStorei( GLenum pname, GLint param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PixelStorei)( CC, pname, param );
}


void APIENTRY glPixelTransferf( GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PixelTransferf)(CC, pname, param);
}


void APIENTRY glPixelTransferi( GLenum pname, GLint param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PixelTransferf)(CC, pname, (GLfloat) param);
}


void APIENTRY glPixelZoom( GLfloat xfactor, GLfloat yfactor )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PixelZoom)(CC, xfactor, yfactor);
}


void APIENTRY glPointSize( GLfloat size )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PointSize)(CC, size);
}


void APIENTRY glPolygonMode( GLenum face, GLenum mode )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PolygonMode)(CC, face, mode);
}


void APIENTRY glPolygonOffset( GLfloat factor, GLfloat units )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PolygonOffset)( CC, factor, units );
}


/* GL_EXT_polygon_offset */
void APIENTRY glPolygonOffsetEXT( GLfloat factor, GLfloat bias )
{
   glPolygonOffset( factor, bias * DEPTH_SCALE );
}


void APIENTRY glPolygonStipple( const GLubyte *pattern )
{
   GLuint unpackedPattern[32];
   GET_CONTEXT;
   CHECK_CONTEXT;
   gl_unpack_polygon_stipple( CC, pattern, unpackedPattern );
   (*CC->API.PolygonStipple)(CC, unpackedPattern);
}


void APIENTRY glPopAttrib( void )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PopAttrib)(CC);
}


void APIENTRY glPopClientAttrib( void )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PopClientAttrib)(CC);
}


void APIENTRY glPopMatrix( void )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PopMatrix)( CC );
}


void APIENTRY glPopName( void )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PopName)(CC);
}


void APIENTRY glPrioritizeTextures( GLsizei n, const GLuint *textures,
                           const GLclampf *priorities )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PrioritizeTextures)(CC, n, textures, priorities);
}


void APIENTRY glPushMatrix( void )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PushMatrix)( CC );
}


void APIENTRY glRasterPos2d( GLdouble x, GLdouble y )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}


void APIENTRY glRasterPos2f( GLfloat x, GLfloat y )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}


void APIENTRY glRasterPos2i( GLint x, GLint y )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}


void APIENTRY glRasterPos2s( GLshort x, GLshort y )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, 0.0F, 1.0F );
}


void APIENTRY glRasterPos3d( GLdouble x, GLdouble y, GLdouble z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}


void APIENTRY glRasterPos3f( GLfloat x, GLfloat y, GLfloat z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}


void APIENTRY glRasterPos3i( GLint x, GLint y, GLint z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}


void APIENTRY glRasterPos3s( GLshort x, GLshort y, GLshort z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z, 1.0F );
}


void APIENTRY glRasterPos4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y,
                               (GLfloat) z, (GLfloat) w );
}


void APIENTRY glRasterPos4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, x, y, z, w );
}


void APIENTRY glRasterPos4i( GLint x, GLint y, GLint z, GLint w )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y,
                               (GLfloat) z, (GLfloat) w );
}


void APIENTRY glRasterPos4s( GLshort x, GLshort y, GLshort z, GLshort w )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) x, (GLfloat) y,
                               (GLfloat) z, (GLfloat) w );
}


void APIENTRY glRasterPos2dv( const GLdouble *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1], 0.0F, 1.0F );
}


void APIENTRY glRasterPos2fv( const GLfloat *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1], 0.0F, 1.0F );
}


void APIENTRY glRasterPos2iv( const GLint *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1], 0.0F, 1.0F );
}


void APIENTRY glRasterPos2sv( const GLshort *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1], 0.0F, 1.0F );
}


/*** 3 element vector ***/

void APIENTRY glRasterPos3dv( const GLdouble *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], 1.0F );
}


void APIENTRY glRasterPos3fv( const GLfloat *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], 1.0F );
}


void APIENTRY glRasterPos3iv( const GLint *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], 1.0F );
}


void APIENTRY glRasterPos3sv( const GLshort *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], 1.0F );
}


void APIENTRY glRasterPos4dv( const GLdouble *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], (GLfloat) v[3] );
}


void APIENTRY glRasterPos4fv( const GLfloat *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, v[0], v[1], v[2], v[3] );
}


void APIENTRY glRasterPos4iv( const GLint *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], (GLfloat) v[3] );
}


void APIENTRY glRasterPos4sv( const GLshort *v )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.RasterPos4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], (GLfloat) v[3] );
}


void APIENTRY glReadBuffer( GLenum mode )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ReadBuffer)( CC, mode );
}


void APIENTRY glReadPixels( GLint x, GLint y, GLsizei width, GLsizei height,
           GLenum format, GLenum type, GLvoid *pixels )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ReadPixels)( CC, x, y, width, height, format, type, pixels );
}


void APIENTRY glRectd( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rectf)( CC, (GLfloat) x1, (GLfloat) y1,
                    (GLfloat) x2, (GLfloat) y2 );
}


void APIENTRY glRectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rectf)( CC, x1, y1, x2, y2 );
}


void APIENTRY glRecti( GLint x1, GLint y1, GLint x2, GLint y2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rectf)( CC, (GLfloat) x1, (GLfloat) y1,
                         (GLfloat) x2, (GLfloat) y2 );
}


void APIENTRY glRects( GLshort x1, GLshort y1, GLshort x2, GLshort y2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rectf)( CC, (GLfloat) x1, (GLfloat) y1,
                     (GLfloat) x2, (GLfloat) y2 );
}


void APIENTRY glRectdv( const GLdouble *v1, const GLdouble *v2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rectf)(CC, (GLfloat) v1[0], (GLfloat) v1[1],
                    (GLfloat) v2[0], (GLfloat) v2[1]);
}


void APIENTRY glRectfv( const GLfloat *v1, const GLfloat *v2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rectf)(CC, v1[0], v1[1], v2[0], v2[1]);
}


void APIENTRY glRectiv( const GLint *v1, const GLint *v2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rectf)( CC, (GLfloat) v1[0], (GLfloat) v1[1],
                     (GLfloat) v2[0], (GLfloat) v2[1] );
}


void APIENTRY glRectsv( const GLshort *v1, const GLshort *v2 )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rectf)(CC, (GLfloat) v1[0], (GLfloat) v1[1],
        (GLfloat) v2[0], (GLfloat) v2[1]);
}


void APIENTRY glScissor( GLint x, GLint y, GLsizei width, GLsizei height)
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Scissor)(CC, x, y, width, height);
}


GLboolean APIENTRY glIsEnabled( GLenum cap )
{
   GET_CONTEXT;
   CHECK_CONTEXT_RETURN(GL_FALSE);
   return (*CC->API.IsEnabled)( CC, cap );
}



void APIENTRY glPushAttrib( GLbitfield mask )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PushAttrib)(CC, mask);
}


void APIENTRY glPushClientAttrib( GLbitfield mask )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PushClientAttrib)(CC, mask);
}


void APIENTRY glPushName( GLuint name )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.PushName)(CC, name);
}


GLint APIENTRY glRenderMode( GLenum mode )
{
   GET_CONTEXT;
   CHECK_CONTEXT_RETURN(0);
   return (*CC->API.RenderMode)(CC, mode);
}


void APIENTRY glRotated( GLdouble angle, GLdouble x, GLdouble y, GLdouble z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rotatef)( CC, (GLfloat) angle,
                       (GLfloat) x, (GLfloat) y, (GLfloat) z );
}


void APIENTRY glRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Rotatef)( CC, angle, x, y, z );
}


void APIENTRY glSelectBuffer( GLsizei size, GLuint *buffer )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.SelectBuffer)(CC, size, buffer);
}


void APIENTRY glScaled( GLdouble x, GLdouble y, GLdouble z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Scalef)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z );
}


void APIENTRY glScalef( GLfloat x, GLfloat y, GLfloat z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Scalef)( CC, x, y, z );
}


void APIENTRY glShadeModel( GLenum mode )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.ShadeModel)(CC, mode);
}


void APIENTRY glStencilFunc( GLenum func, GLint ref, GLuint mask )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.StencilFunc)(CC, func, ref, mask);
}


void APIENTRY glStencilMask( GLuint mask )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.StencilMask)(CC, mask);
}


void APIENTRY glStencilOp( GLenum fail, GLenum zfail, GLenum zpass )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.StencilOp)(CC, fail, zfail, zpass);
}


void APIENTRY glTexCoord1d( GLdouble s )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, 0.0, 0.0, 1.0 );
}


void APIENTRY glTexCoord1f( GLfloat s )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, s, 0.0, 0.0, 1.0 );
}


void APIENTRY glTexCoord1i( GLint s )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, 0.0, 0.0, 1.0 );
}


void APIENTRY glTexCoord1s( GLshort s )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, 0.0, 0.0, 1.0 );
}


void APIENTRY glTexCoord2d( GLdouble s, GLdouble t )
{
   GET_CONTEXT;
   (*CC->API.TexCoord2f)( CC, (GLfloat) s, (GLfloat) t );
}


void APIENTRY glTexCoord2f( GLfloat s, GLfloat t )
{
   GET_CONTEXT;
   (*CC->API.TexCoord2f)( CC, s, t );
}


void APIENTRY glTexCoord2i( GLint s, GLint t )
{
   GET_CONTEXT;
   (*CC->API.TexCoord2f)( CC, (GLfloat) s, (GLfloat) t );
}


void APIENTRY glTexCoord2s( GLshort s, GLshort t )
{
   GET_CONTEXT;
   (*CC->API.TexCoord2f)( CC, (GLfloat) s, (GLfloat) t );
}


void APIENTRY glTexCoord3d( GLdouble s, GLdouble t, GLdouble r )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, (GLfloat) t, (GLfloat) r, 1.0 );
}


void APIENTRY glTexCoord3f( GLfloat s, GLfloat t, GLfloat r )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, s, t, r, 1.0 );
}


void APIENTRY glTexCoord3i( GLint s, GLint t, GLint r )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, (GLfloat) t,
                               (GLfloat) r, 1.0 );
}


void APIENTRY glTexCoord3s( GLshort s, GLshort t, GLshort r )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, (GLfloat) t,
                               (GLfloat) r, 1.0 );
}


void APIENTRY glTexCoord4d( GLdouble s, GLdouble t, GLdouble r, GLdouble q )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, (GLfloat) t,
                               (GLfloat) r, (GLfloat) q );
}


void APIENTRY glTexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, s, t, r, q );
}


void APIENTRY glTexCoord4i( GLint s, GLint t, GLint r, GLint q )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, (GLfloat) t,
                               (GLfloat) r, (GLfloat) q );
}


void APIENTRY glTexCoord4s( GLshort s, GLshort t, GLshort r, GLshort q )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) s, (GLfloat) t,
                               (GLfloat) r, (GLfloat) q );
}


void APIENTRY glTexCoord1dv( const GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) *v, 0.0, 0.0, 1.0 );
}


void APIENTRY glTexCoord1fv( const GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, *v, 0.0, 0.0, 1.0 );
}


void APIENTRY glTexCoord1iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, *v, 0.0, 0.0, 1.0 );
}


void APIENTRY glTexCoord1sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) *v, 0.0, 0.0, 1.0 );
}


void APIENTRY glTexCoord2dv( const GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord2f)( CC, (GLfloat) v[0], (GLfloat) v[1] );
}


void APIENTRY glTexCoord2fv( const GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord2f)( CC, v[0], v[1] );
}


void APIENTRY glTexCoord2iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord2f)( CC, (GLfloat) v[0], (GLfloat) v[1] );
}


void APIENTRY glTexCoord2sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord2f)( CC, (GLfloat) v[0], (GLfloat) v[1] );
}


void APIENTRY glTexCoord3dv( const GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], 1.0 );
}


void APIENTRY glTexCoord3fv( const GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, v[0], v[1], v[2], 1.0 );
}


void APIENTRY glTexCoord3iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                          (GLfloat) v[2], 1.0 );
}


void APIENTRY glTexCoord3sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], 1.0 );
}


void APIENTRY glTexCoord4dv( const GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], (GLfloat) v[3] );
}


void APIENTRY glTexCoord4fv( const GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, v[0], v[1], v[2], v[3] );
}


void APIENTRY glTexCoord4iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], (GLfloat) v[3] );
}


void APIENTRY glTexCoord4sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.TexCoord4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                               (GLfloat) v[2], (GLfloat) v[3] );
}


void APIENTRY glTexCoordPointer( GLint size, GLenum type, GLsizei stride,
                        const GLvoid *ptr )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexCoordPointer)(CC, size, type, stride, ptr);
}


void APIENTRY glTexGend( GLenum coord, GLenum pname, GLdouble param )
{
   GLfloat p = (GLfloat) param;
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexGenfv)( CC, coord, pname, &p );
}


void APIENTRY glTexGenf( GLenum coord, GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexGenfv)( CC, coord, pname, &param );
}


void APIENTRY glTexGeni( GLenum coord, GLenum pname, GLint param )
{
   GLfloat p = (GLfloat) param;
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexGenfv)( CC, coord, pname, &p );
}


void APIENTRY glTexGendv( GLenum coord, GLenum pname, const GLdouble *params )
{
   GLfloat p[4];
   GET_CONTEXT;
   CHECK_CONTEXT;
   p[0] = params[0];
   p[1] = params[1];
   p[2] = params[2];
   p[3] = params[3];
   (*CC->API.TexGenfv)( CC, coord, pname, p );
}


void APIENTRY glTexGeniv( GLenum coord, GLenum pname, const GLint *params )
{
   GLfloat p[4];
   GET_CONTEXT;
   CHECK_CONTEXT;
   p[0] = params[0];
   p[1] = params[1];
   p[2] = params[2];
   p[3] = params[3];
   (*CC->API.TexGenfv)( CC, coord, pname, p );
}


void APIENTRY glTexGenfv( GLenum coord, GLenum pname, const GLfloat *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexGenfv)( CC, coord, pname, params );
}




void APIENTRY glTexEnvf( GLenum target, GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexEnvfv)( CC, target, pname, &param );
}



void APIENTRY glTexEnvi( GLenum target, GLenum pname, GLint param )
{
   GLfloat p[4];
   GET_CONTEXT;
   p[0] = (GLfloat) param;
   p[1] = p[2] = p[3] = 0.0;
   CHECK_CONTEXT;
   (*CC->API.TexEnvfv)( CC, target, pname, p );
}



void APIENTRY glTexEnvfv( GLenum target, GLenum pname, const GLfloat *param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexEnvfv)( CC, target, pname, param );
}



void APIENTRY glTexEnviv( GLenum target, GLenum pname, const GLint *param )
{
   GLfloat p[4];
   GET_CONTEXT;
   p[0] = INT_TO_FLOAT( param[0] );
   p[1] = INT_TO_FLOAT( param[1] );
   p[2] = INT_TO_FLOAT( param[2] );
   p[3] = INT_TO_FLOAT( param[3] );
   CHECK_CONTEXT;
   (*CC->API.TexEnvfv)( CC, target, pname, p );
}


void APIENTRY glTexImage1D( GLenum target, GLint level, GLint internalformat,
                   GLsizei width, GLint border,
                   GLenum format, GLenum type, const GLvoid *pixels )
{
   struct gl_image *teximage;
   GET_CONTEXT;
   CHECK_CONTEXT;
   teximage = gl_unpack_image( CC, width, 1, format, type, pixels );
   (*CC->API.TexImage1D)( CC, target, level, internalformat,
                          width, border, format, type, teximage );
}



void APIENTRY glTexImage2D( GLenum target, GLint level, GLint internalformat,
                   GLsizei width, GLsizei height, GLint border,
                   GLenum format, GLenum type, const GLvoid *pixels )
{
  struct gl_image *teximage;
#if defined(FX) && defined(__WIN32__)
  GLvoid *newpixels=NULL;
  GLsizei newwidth,newheight;
  GLint x,y;
  static GLint leveldif=0;
  static GLuint lasttexobj=0;
#endif
   GET_CONTEXT;
  CHECK_CONTEXT;

#if defined(FX) && defined(__WIN32__)
  newpixels=NULL;
  
  /* AN HACK for WinGLQuake*/
  
  if (CC->Texture.Set[0].Current2D->Name!=lasttexobj) {
    lasttexobj=CC->Texture.Set[0].Current2D->Name;
    leveldif=0;
  }
  
  if ((format==GL_COLOR_INDEX) && (internalformat==1))
    internalformat=GL_COLOR_INDEX8_EXT;
  
  if (width>256 || height>256) {
    newpixels=malloc((width+4)*height*4);

    while (width>256 || height>256) {
      newwidth=width/2;
      newheight=height/2;
      leveldif++;
      
      fprintf(stderr,"Scaling: (%d) %dx%d -> %dx%d\n",internalformat,width,height,newwidth,newheight);
      fflush(stderr);
      
      for(y=0;y<newheight;y++)
	for(x=0;x<newwidth;x++) {
	  ((GLubyte *)newpixels)[(x+y*newwidth)*4+0]=((GLubyte *)pixels)[(x*2+y*width*2)*4+0];
	  ((GLubyte *)newpixels)[(x+y*newwidth)*4+1]=((GLubyte *)pixels)[(x*2+y*width*2)*4+1];
	  ((GLubyte *)newpixels)[(x+y*newwidth)*4+2]=((GLubyte *)pixels)[(x*2+y*width*2)*4+2];
	  ((GLubyte *)newpixels)[(x+y*newwidth)*4+3]=((GLubyte *)pixels)[(x*2+y*width*2)*4+3];
	}
      
      pixels=newpixels;
      width=newwidth;
      height=newheight;
    }
    
    level=0;
  } else
    level-=leveldif;
#endif
  teximage = gl_unpack_image( CC, width, height, format, type, pixels );
   (*CC->API.TexImage2D)( CC, target, level, internalformat,
                          width, height, border, format, type, teximage );
#if defined(FX) && defined(__WIN32__)
  if(newpixels)
    free(newpixels);
#endif
}


void APIENTRY glTexImage3D( GLenum target, GLint level, GLenum internalformat,
                            GLsizei width, GLsizei height, GLsizei depth,
                            GLint border, GLenum format, GLenum type,
                            const GLvoid *pixels )
{
   struct gl_image *teximage;
   GET_CONTEXT;
   CHECK_CONTEXT;
   teximage = gl_unpack_image3D( CC, width, height, depth, format, type, pixels);
   (*CC->API.TexImage3DEXT)( CC, target, level, internalformat,
                             width, height, depth, border, format, type, 
                             teximage );
}


void APIENTRY glTexParameterf( GLenum target, GLenum pname, GLfloat param )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexParameterfv)( CC, target, pname, &param );
}


void APIENTRY glTexParameteri( GLenum target, GLenum pname, GLint param )
{
   GLfloat fparam[4];
   GET_CONTEXT;
   fparam[0] = (GLfloat) param;
   fparam[1] = fparam[2] = fparam[3] = 0.0;
   CHECK_CONTEXT;
   (*CC->API.TexParameterfv)( CC, target, pname, fparam );
}


void APIENTRY glTexParameterfv( GLenum target, GLenum pname, const GLfloat *params )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.TexParameterfv)( CC, target, pname, params );
}


void APIENTRY glTexParameteriv( GLenum target, GLenum pname, const GLint *params )
{
   GLfloat p[4];
   GET_CONTEXT;
   CHECK_CONTEXT;
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


void APIENTRY glTexSubImage1D( GLenum target, GLint level, GLint xoffset,
                      GLsizei width, GLenum format,
                      GLenum type, const GLvoid *pixels )
{
   struct gl_image *image;
   GET_CONTEXT;
   CHECK_CONTEXT;
   image = gl_unpack_texsubimage( CC, width, 1, format, type, pixels );
   (*CC->API.TexSubImage1D)( CC, target, level, xoffset, width,
                             format, type, image );
}


void APIENTRY glTexSubImage2D( GLenum target, GLint level,
                      GLint xoffset, GLint yoffset,
                      GLsizei width, GLsizei height,
                      GLenum format, GLenum type,
                      const GLvoid *pixels )
{
   struct gl_image *image;
   GET_CONTEXT;
   CHECK_CONTEXT;
   image = gl_unpack_texsubimage( CC, width, height, format, type, pixels );
   (*CC->API.TexSubImage2D)( CC, target, level, xoffset, yoffset,
                             width, height, format, type, image );
}


void APIENTRY glTexSubImage3D( GLenum target, GLint level, GLint xoffset,
                               GLint yoffset, GLint zoffset, GLsizei width,
                               GLsizei height, GLsizei depth, GLenum format,
                               GLenum type, const GLvoid *pixels )
{
   struct gl_image *image;
   GET_CONTEXT;
   CHECK_CONTEXT;
   image = gl_unpack_texsubimage3D( CC, width, height, depth, format, type,
                                    pixels );
   (*CC->API.TexSubImage3DEXT)( CC, target, level, xoffset, yoffset, zoffset,
                                width, height, depth, format, type, image );
}


void APIENTRY glTranslated( GLdouble x, GLdouble y, GLdouble z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Translatef)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z );
}


void APIENTRY glTranslatef( GLfloat x, GLfloat y, GLfloat z )
{
   GET_CONTEXT;
   CHECK_CONTEXT;
   (*CC->API.Translatef)( CC, x, y, z );
}


void APIENTRY glVertex2d( GLdouble x, GLdouble y )
{
   GET_CONTEXT;
   (*CC->API.Vertex2f)( CC, (GLfloat) x, (GLfloat) y );
}


void APIENTRY glVertex2f( GLfloat x, GLfloat y )
{
   GET_CONTEXT;
   (*CC->API.Vertex2f)( CC, x, y );
}


void APIENTRY glVertex2i( GLint x, GLint y )
{
   GET_CONTEXT;
   (*CC->API.Vertex2f)( CC, (GLfloat) x, (GLfloat) y );
}


void APIENTRY glVertex2s( GLshort x, GLshort y )
{
   GET_CONTEXT;
   (*CC->API.Vertex2f)( CC, (GLfloat) x, (GLfloat) y );
}


void APIENTRY glVertex3d( GLdouble x, GLdouble y, GLdouble z )
{
   GET_CONTEXT;
   (*CC->API.Vertex3f)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z );
}


void APIENTRY glVertex3f( GLfloat x, GLfloat y, GLfloat z )
{
   GET_CONTEXT;
   (*CC->API.Vertex3f)( CC, x, y, z );
}


void APIENTRY glVertex3i( GLint x, GLint y, GLint z )
{
   GET_CONTEXT;
   (*CC->API.Vertex3f)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z );
}


void APIENTRY glVertex3s( GLshort x, GLshort y, GLshort z )
{
   GET_CONTEXT;
   (*CC->API.Vertex3f)( CC, (GLfloat) x, (GLfloat) y, (GLfloat) z );
}


void APIENTRY glVertex4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) x, (GLfloat) y,
                            (GLfloat) z, (GLfloat) w );
}


void APIENTRY glVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, x, y, z, w );
}


void APIENTRY glVertex4i( GLint x, GLint y, GLint z, GLint w )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) x, (GLfloat) y,
                            (GLfloat) z, (GLfloat) w );
}


void APIENTRY glVertex4s( GLshort x, GLshort y, GLshort z, GLshort w )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) x, (GLfloat) y,
                            (GLfloat) z, (GLfloat) w );
}


void APIENTRY glVertex2dv( const GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex2f)( CC, (GLfloat) v[0], (GLfloat) v[1] );
}


void APIENTRY glVertex2fv( const GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex2f)( CC, v[0], v[1] );
}


void APIENTRY glVertex2iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex2f)( CC, (GLfloat) v[0], (GLfloat) v[1] );
}


void APIENTRY glVertex2sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex2f)( CC, (GLfloat) v[0], (GLfloat) v[1] );
}


void APIENTRY glVertex3dv( const GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex3f)( CC, (GLfloat) v[0], (GLfloat) v[1], (GLfloat) v[2] );
}


void APIENTRY glVertex3fv( const GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex3fv)( CC, v );
}


void APIENTRY glVertex3iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex3f)( CC, (GLfloat) v[0], (GLfloat) v[1], (GLfloat) v[2] );
}


void APIENTRY glVertex3sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex3f)( CC, (GLfloat) v[0], (GLfloat) v[1], (GLfloat) v[2] );
}


void APIENTRY glVertex4dv( const GLdouble *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                            (GLfloat) v[2], (GLfloat) v[3] );
}


void APIENTRY glVertex4fv( const GLfloat *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, v[0], v[1], v[2], v[3] );
}


void APIENTRY glVertex4iv( const GLint *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                  (GLfloat) v[2], (GLfloat) v[3] );
}


void APIENTRY glVertex4sv( const GLshort *v )
{
   GET_CONTEXT;
   (*CC->API.Vertex4f)( CC, (GLfloat) v[0], (GLfloat) v[1],
                            (GLfloat) v[2], (GLfloat) v[3] );
}


void APIENTRY glVertexPointer( GLint size, GLenum type, GLsizei stride,
                      const GLvoid *ptr )
{
   GET_CONTEXT;
   (*CC->API.VertexPointer)(CC, size, type, stride, ptr);
}


void APIENTRY glViewport( GLint x, GLint y, GLsizei width, GLsizei height )
{
   GET_CONTEXT;
   (*CC->API.Viewport)( CC, x, y, width, height );
}

