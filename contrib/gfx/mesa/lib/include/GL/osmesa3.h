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
 * Revision 1.1  1998/02/13 03:17:50  brianp
 * Initial revision
 *
 */


/*
 * Mesa Off-Screen rendering interface.  VERSION 3.0
 *
 * This is an operating system and window system independent interface to
 * Mesa which allows one to render images into a client-supplied buffer in
 * main memory.  Such images may manipulated or saved in whatever way the
 * client wants.
 *
 * The new VERSION 3.0 interface is modelled after GLX.
 *
 * These are the main API functions:
 *   OSMesaCreateVisual - create a new OSMesaVisual
 *   OSMesaCreateBuffer - create a new color buffer with optional ancillary
 *                        buffers
 *   OSMesaCreateContext - create a new Off-Screen Mesa rendering context
 *   OSMesaMakeCurrent - bind an OSMesaContext to a OSMesaBuffer
 *                       and make them active for rendering.
 *
 * The limits on the width and height of an image buffer are MAX_WIDTH and
 * MAX_HEIGHT as defined in Mesa/src/config.h.  Defaults are at least
 * 1280 and 1024.  You can increase them as needed but beware that many
 * temporary arrays in Mesa are dimensioned by MAX_WIDTH or MAX_HEIGHT.
 */



#ifndef OSMESA3_H
#define OSMESA3_H



#ifdef __cplusplus
extern "C" {
#endif


#include "GL/gl.h"



#define OSMESA3_MAJOR_VERSION 3
#define OSMESA3_MINOR_VERSION 0



/*
 * Values for the format parameter of OSMesaCreateContext()
 * New in version 2.0.
 */
#define OSMESA_COLOR_INDEX	GL_COLOR_INDEX
#define OSMESA_RGBA		GL_RGBA
#define OSMESA_BGRA		0x1
#define OSMESA_ARGB		0x2
#define OSMESA_RGB		GL_RGB
#define OSMESA_BGR		0x4


/*
 * OSMesaPixelStore() parameters:
 * New in version 2.0.
 */
#define OSMESA_ROW_LENGTH	0x10
#define OSMESA_Y_UP		0x11


/*
 * Accepted by OSMesaGetIntegerv:
 */
#define OSMESA_WIDTH		0x20
#define OSMESA_HEIGHT		0x21
#define OSMESA_FORMAT		0x22
#define OSMESA_TYPE		0x23


/*
 * Bitfield flags for OSMesaCreateVisual:
 */
#define OSMESA_DEPTH		0x1
#define OSMESA_STENCIL		0X2
#define OSMESA_ACCUM		0X4



typedef struct osmesa_context *OSMesaContext;

typedef struct osmesa_buffer *OSMesaBuffer;

typedef struct osmesa_visual *OSMesaVisual;




#if defined(__BEOS__) || defined(__QUICKDRAW__)
#pragma export on
#endif



/*
 * Create an OSMesaVisual.
 * Input:  format = one of OSMESA_COLOR_INDEX, OSMESA_RGBA, OSMESA_RGB, etc.
 *         flags = bitmask of OSMESA_DEPTH, OSMESA_STENCIL, OSMESA_ACCUM, etc.
 * Return:  new OSMesaVisual or 0 if error
 */
extern OSMesaVisual OSMesaCreateVisual( GLenum format, GLbitfield flags );



/*
 * Destroy an OSMesaVisual.
 */
extern void OSMesaDestroyVisual( OSMesaVisual v );



/*
 * Create an off-screen rendering buffer.
 *
 * The image (color) buffer is just a
 * block of memory which the client provides.  Its size must be at least
 * as large as width*height*sizeof(type).  Its address should be a multiple
 * of 4 if using RGBA mode.
 *
 * Image data is stored in the order of glDrawPixels:  row-major order
 * with the lower-left image pixel stored in the first array position
 * (ie. bottom-to-top).
 *
 * Input:  v - the OSMesaVisual (specifies depth, stencil buffers, etc)
 *         widht, height - size of buffer in pixels
 *         type - pixel datatype, must bye GL_UNSIGNED_BYTE at this time
 *         buffer - pointer to user-allocated color buffer memory
 */
extern OSMesaBuffer OSMesaCreateBuffer( OSMesaVisual v
                                        GLint width, GLint height,
                                        GLenum type, void *buffer);



/*
 * Free an OSMesaBuffer, but not the user-provided color buffer!
 */
extern void OSMesaDestroyBuffer( OSMesaBuffer b );



/*
 * Create an Off-Screen Mesa rendering context.
 *
 * Input:  visual - the OSMesaVisual to use.
 *         sharelist - specifies another OSMesaContext with which to share
 *                     display lists.  NULL indicates no sharing.
 * Return:  an OSMesaContext or 0 if error
 */
extern OSMesaContext OSMesaCreateContext( OSMesaVisual visual,
                                          OSMesaContext sharelist );




/*
 * Destroy an Off-Screen Mesa rendering context.
 *
 * Input:  ctx - the context to destroy
 */
extern void OSMesaDestroyContext( OSMesaContext ctx );



/*
 * Bind an OSMesaContext to an OSMesaBuffer buffer and make the current
 * context and buffer.
 */
extern void OSMesaMakeCurrent( OSMesaContext ctx, OSMesaBuffer buffer );



/*
 * Return the current Off-Screen Mesa rendering context handle.
 */
extern OSMesaContext OSMesaGetCurrentContext( void );



/*
 * Return the current Off-Screen Mesa buffer.
 */
extern OSMesaBuffer OSMesaGetCurrentBuffer( void );



/*
 * Set pixel store/packing parameters for the current context.
 * This is similar to glPixelStore.
 * Input:  pname - OSMESA_ROW_LENGTH
 *                    specify actual pixels per row in image buffer
 *                    0 = same as image width (default)
 *                 OSMESA_Y_UP
 *                    zero = Y coordinates increase downward
 *                    non-zero = Y coordinates increase upward (default)
 *         value - the value for the parameter pname
 */
extern void OSMesaPixelStore( GLint pname, GLint value );



/*
 * Return context info.  This is like glGetIntegerv.
 * Input:  pname -
 *                 OSMESA_WIDTH  return current image width
 *                 OSMESA_HEIGHT  return current image height
 *                 OSMESA_FORMAT  return image format
 *                 OSMESA_TYPE  return color component data type
 *                 OSMESA_ROW_LENGTH return row length in pixels
 *                 OSMESA_Y_UP returns 1 or 0 to indicate Y axis direction
 *         value - pointer to integer in which to return result.
 */
extern void OSMesaGetIntegerv( GLint pname, GLint *value );



/*
 * Return the depth buffer associated with an OSMesaBuffer.
 * Input:  b - the OSMesaBuffer
 * Output:  width, height - size of buffer in pixels
 *          bytesPerValue - bytes per depth value (2 or 4)
 *          buffer - pointer to depth buffer values
 * Return:  GL_TRUE or GL_FALSE to indicate success or failure.
 */
extern GLboolean OSMesaGetDepthBuffer( OSMesaBuffer b,
                                       GLint *width, GLint *height,
                                       GLint *bytesPerValue, void **buffer );





#if defined(__BEOS__) || defined(__QUICKDRAW__)
#pragma export off
#endif


#ifdef __cplusplus
}
#endif


#endif
