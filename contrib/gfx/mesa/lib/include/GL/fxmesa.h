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
 * Revision 1.1  2005/01/11 14:58:29  NicJA
 * AROSMesa 3.0
 *
 * - Based on the official mesa 3 code with major patches to the amigamesa driver code to get it working.
 * - GLUT not yet started (ive left the _old_ mesaaux, mesatk and demos in for this reason)
 * - Doesnt yet work - the _db functions seem to be writing the data incorrectly, and color picking also seems broken somewhat - giving most things a blue tinge (those that are currently working)
 *
 * Revision 3.1  1998/04/01 03:00:28  brianp
 * updated for v0.24 of 3Dfx/Glide driver
 *
 * Revision 3.0  1998/02/20 05:04:45  brianp
 * initial rev
 *
 */


/*
 * FXMesa - 3Dfx Glide driver for Mesa.  Contributed by David Bucciarelli
 *
 * NOTE: This version requires Glide 2.3 or later.
 */


#ifndef FXMESA_H
#define FXMESA_H


#include <glide.h>


#ifdef __cplusplus
extern "C" {
#endif


#define FXMESA_MAJOR_VERSION 3
#define FXMESA_MINOR_VERSION 0


/*
 * Values for attribList parameter to fxMesaCreateContext():
 */
#define FXMESA_NONE		0	/* to terminate attribList */
#define FXMESA_DOUBLEBUFFER	10
#define FXMESA_ALPHA_SIZE	11      /* followed by an integer */
#define FXMESA_DEPTH_SIZE	12      /* followed by an integer */
#define FXMESA_STENCIL_SIZE	13      /* followed by an integer */
#define FXMESA_ACCUM_SIZE	14      /* followed by an integer */



typedef struct tfxMesaContext *fxMesaContext;


#if defined (__BEOS__)
#pragma export on
#endif


WINGDIAPI fxMesaContext APIENTRY fxMesaCreateContext(GLuint win, GrScreenResolution_t,
                                         GrScreenRefresh_t,
                                         const GLint attribList[]);

WINGDIAPI fxMesaContext APIENTRY fxMesaCreateBestContext(GLuint win,
                                             GLint width, GLint height,
                                             const GLint attribList[]);
WINGDIAPI void APIENTRY fxMesaDestroyContext(fxMesaContext ctx);

WINGDIAPI GLboolean APIENTRY fxMesaSelectCurrentBoard(int n);

WINGDIAPI void APIENTRY fxMesaMakeCurrent(fxMesaContext ctx);

WINGDIAPI fxMesaContext APIENTRY fxMesaGetCurrentContext(void);

WINGDIAPI void APIENTRY fxMesaSwapBuffers(void);

WINGDIAPI void APIENTRY fxMesaSetNearFar(GLfloat nearVal, GLfloat farVal);

WINGDIAPI void APIENTRY fxMesaUpdateScreenSize(fxMesaContext ctx);

WINGDIAPI int APIENTRY fxQueryHardware(void);

WINGDIAPI void APIENTRY fxCloseHardware(void);


#if defined (__BEOS__)
#pragma export off
#endif


#ifdef __cplusplus
}
#endif


#endif
