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
 * Revision 1.1  2005/01/11 14:58:33  NicJA
 * AROSMesa 3.0
 *
 * - Based on the official mesa 3 code with major patches to the amigamesa driver code to get it working.
 * - GLUT not yet started (ive left the _old_ mesaaux, mesatk and demos in for this reason)
 * - Doesnt yet work - the _db functions seem to be writing the data incorrectly, and color picking also seems broken somewhat - giving most things a blue tinge (those that are currently working)
 *
 * Revision 3.0  1998/01/31 21:07:51  brianp
 * initial rev
 *
 */


/*
 * GL_MESA_window_pos extension
 *
 * This extension offers a set of functions named glWindowPos*MESA() which
 * directly set the current raster position to the given window coordinate.
 * glWindowPos*MESA() are similar to glRasterPos*() but bypass the
 * modelview, projection and viewport transformations.
 *
 * These functions should be very handy in conjunction with glDrawPixels()
 * and glCopyPixels().
 *
 * If your application uses glWindowPos*MESA() and may be compiled with
 * a real OpenGL instead of Mesa you can simply copy the winpos.[ch] files
 * into your source tree and compile them with the rest of your code
 * since glWindowPos*MESA() can, and is, implemented in terms of standard
 * OpenGL commands when not using Mesa.  In your source files which use
 * glWindowPos*MESA() just #include "winpos.h".
 */



#ifdef PC_HEADER
#include "all.h"
#else
#include "GL/gl.h"
#endif

#ifdef GL_MESA_window_pos


#ifndef PC_HEADER
#include "rastpos.h"
#include "winpos.h"
#endif



/*
 * Mesa implementation of glWindowPos*MESA()
 */
void gl_WindowPos4fMESA( GLcontext *ctx,
                         GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   gl_windowpos( ctx, x, y, z, w );
}


#else


/*
 * OpenGL implementation of glWindowPos*MESA()
 */
void glWindowPos4fMESA( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
   GLfloat fx, fy;

   /* Push current matrix mode and viewport attributes */
   glPushAttrib( GL_TRANSFORM_BIT | GL_VIEWPORT_BIT );

   /* Setup projection parameters */
   glMatrixMode( GL_PROJECTION );
   glPushMatrix();
   glLoadIdentity();
   glMatrixMode( GL_MODELVIEW );
   glPushMatrix();
   glLoadIdentity();

   glDepthRange( z, z );
   glViewport( (int) x - 1, (int) y - 1, 2, 2 );

   /* set the raster (window) position */
   fx = x - (int) x;
   fy = y - (int) y;
   glRasterPos4f( fx, fy, 0.0, w );

   /* restore matrices, viewport and matrix mode */
   glPopMatrix();
   glMatrixMode( GL_PROJECTION );
   glPopMatrix();

   glPopAttrib();
}


#endif



