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
 * Revision 1.1  2003/08/09 00:23:26  chodorowski
 * Initial revision
 *
 * Revision 1.12  1997/01/31 23:45:19  brianp
 * faster flat-shaded dithered triangles from code by Martin Schenk (schenkm@ping.at)
 *
 * Revision 1.11  1996/11/30 15:13:26  brianp
 * added some parenthesis to WINCLIP macros
 *
 * Revision 1.10  1996/11/02 06:17:02  brianp
 * removed some unused local vars
 *
 * Revision 1.9  1996/10/22 02:59:12  brianp
 * incorporated Micheal Pichler's X line stipple patches
 *
 * Revision 1.8  1996/10/22 02:48:18  brianp
 * now use DITHER_SETUP and XDITHER macros
 * use array indexing instead of pointer dereferencing in inner loops
 *
 * Revision 1.7  1996/10/11 03:43:03  brianp
 * add LineZoffset factor to window Z coords
 *
 * Revision 1.6  1996/10/01 03:31:30  brianp
 * use new FixedToDepth() macro
 *
 * Revision 1.5  1996/09/27 01:31:54  brianp
 * removed unused variables
 *
 * Revision 1.4  1996/09/25 02:02:59  brianp
 * coordinates were incorrectly biased in flat_pixmap_triangle()
 *
 * Revision 1.3  1996/09/19 03:16:04  brianp
 * new X/Mesa interface with XMesaContext, XMesaVisual, and XMesaBuffer types
 *
 * Revision 1.2  1996/09/15 14:21:43  brianp
 * now use GLframebuffer and GLvisual
 *
 * Revision 1.1  1996/09/13 01:38:16  brianp
 * Initial revision
 *
 */


/*
 * Mesa/X11 interface, part 3.
 *
 * This file contains "accelerated" point, line, and triangle functions.
 * It should be fairly easy to write new special-purpose point, line or
 * triangle functions and hook them into this module.
 */



#include <sys/time.h>
#include <assert.h>


#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include "bresenhm.h"
#include "depth.h"
#include "interp.h"
#include "macros.h"
#include "vb.h"
#include "types.h"
#include "xmesaP.h"


/*
 * Like PACK_8A8B8G8R() but don't use alpha.  This is usually an acceptable
 * shortcut.
 */
#define PACK_8B8G8R( R, G, B )   ( ((B) << 16) | ((G) << 8) | (R) )




/**********************************************************************/
/***                    Point rendering                             ***/
/**********************************************************************/


/*
 * Render an array of points into a pixmap, any pixel format.
 */
static void draw_points_ANY_pixmap( GLcontext *ctx, GLuint first, GLuint last )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   Display *dpy = xmesa->xm_visual->display;
   Drawable buffer = xmesa->xm_buffer->buffer;
   GC gc = xmesa->xm_buffer->gc2;
   struct vertex_buffer *VB = ctx->VB;
   register GLuint i;

   if (VB->MonoColor) {
      /* all same color */
      XPoint p[VB_SIZE];
      int n = 0;
      for (i=first;i<=last;i++) {
         if (VB->Unclipped[i]) {
            p[n].x =       (GLint) VB->Win[i][0];
            p[n].y = FLIP( (GLint) VB->Win[i][1] );
            n++;
         }
      }
      XDrawPoints( dpy, buffer, xmesa->xm_buffer->gc1, p, n, CoordModeOrigin );
   }
   else {
      /* all different colors */
      int shift = ctx->ColorShift;
      if (xmesa->xm_visual->gl_visual->RGBAflag) {
         /* RGB mode */
         for (i=first;i<=last;i++) {
            if (VB->Unclipped[i]) {
               register int x, y;
               unsigned long pixel = xmesa_color_to_pixel( xmesa,
                                                VB->Color[i][0] >> shift,
                                                VB->Color[i][1] >> shift,
                                                VB->Color[i][2] >> shift,
                                                VB->Color[i][3] >> shift );
               XSetForeground( dpy, gc, pixel );
               x =       (GLint) VB->Win[i][0];
               y = FLIP( (GLint) VB->Win[i][1] );
               XDrawPoint( dpy, buffer, gc, x, y);
            }
         }
      }
      else {
         /* Color index mode */
         for (i=first;i<=last;i++) {
            if (VB->Unclipped[i]) {
               register int x, y;
               XSetForeground( dpy, gc, VB->Index[i] >> shift );
               x =       (GLint) VB->Win[i][0];
               y = FLIP( (GLint) VB->Win[i][1] );
               XDrawPoint( dpy, buffer, gc, x, y);
            }
         }
      }
   }
}



/*
 * Analyze context state to see if we can provide a fast points drawing
 * function, like those in points.c.  Otherwise, return NULL.
 */
points_func xmesa_get_points_func( GLcontext *ctx )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;

   if (ctx->Point.Size==1.0F && !ctx->Point.SmoothFlag && ctx->RasterMask==0
       && !ctx->Texture.Enabled) {
      if (xmesa->xm_buffer->buffer==XIMAGE) {
         return NULL; /*draw_points_ximage;*/
      }
      else {
         return draw_points_ANY_pixmap;
      }
   }
   else {
      return NULL;
   }
}



/**********************************************************************/
/***                      Line rendering                            ***/
/**********************************************************************/

/*
 * Render a line into a pixmap, any pixel format.
 */
static void flat_pixmap_line( GLcontext *ctx, GLuint v0, GLuint v1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   struct vertex_buffer *VB = ctx->VB;
   register int x0, y0, x1, y1;
   GC gc;
   if (VB->MonoColor) {
      gc = xmesa->xm_buffer->gc1;  /* use current color */
   }
   else {
      unsigned long pixel;
      if (xmesa->xm_visual->gl_visual->RGBAflag) {
         pixel = xmesa_color_to_pixel( xmesa,
                                       VB->Color[pv][0], VB->Color[pv][1],
                                       VB->Color[pv][2], VB->Color[pv][3] );
      }
      else {
         pixel = VB->Index[pv];
      }
      gc = xmesa->xm_buffer->gc2;
      XSetForeground( xmesa->display, gc, pixel );
   }
   x0 =       (GLint) VB->Win[v0][0];
   y0 = FLIP( (GLint) VB->Win[v0][1] );
   x1 =       (GLint) VB->Win[v1][0];
   y1 = FLIP( (GLint) VB->Win[v1][1] );
   XDrawLine( xmesa->display, xmesa->xm_buffer->buffer, gc, x0, y0, x1, y1 );
}


/*
 * Despite being clipped to the view volume, the line's window coordinates
 * may just lie outside the window bounds.  That is, if the legal window
 * coordinates are [0,W-1][0,H-1], it's possible for x==W and/or y==H.
 * These quick and dirty macros take care of that possibility.
 */
#define WINCLIP_X(X1,X2)		\
   {					\
      GLint w = ctx->Buffer->Width;	\
      if ((X1==w) | (X2==w)) {		\
         if ((X1==w) & (X2==w))  return;\
         X1 -= (X1==w);   X2 -= (X2==w);\
      }					\
   }

#define WINCLIP_Y(Y1,Y2)		\
   {					\
      GLint h = ctx->Buffer->Height;	\
      if ((Y1==h) | (Y2==h)) {		\
         if ((Y1==h) & (Y2==h))  return;\
         Y1 -= (Y1==h);   Y2 -= (Y2==h);\
      }					\
   }


/*
 * Draw a flat-shaded, PF_8A8B8G8R line into an XImage.
 */
static void flat_8A8B8G8R_line( GLcontext *ctx,
                                GLuint v0, GLuint v1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   struct vertex_buffer *VB = ctx->VB;
   GLint x1 = (GLint) VB->Win[v0][0],  y1 = (GLint) VB->Win[v0][1];
   GLint x2 = (GLint) VB->Win[v1][0],  y2 = (GLint) VB->Win[v1][1];
   GLuint pixel;
   WINCLIP_X(x1,x2);
   WINCLIP_Y(y1,y2);
   pixel = PACK_8B8G8R( VB->Color[pv][0], VB->Color[pv][1], VB->Color[pv][2] );

#define BRESENHAM_PLOT(X,Y)			\
	{					\
	   GLuint *ptr = PIXELADDR4(X,Y);	\
	   *ptr = pixel;			\
	}

   BRESENHAM( x1, y1, x2, y2 );

#undef BRESENHAM_PLOT
}


/*
 * Draw a flat-shaded, PF_8R8G8B line into an XImage.
 */
static void flat_8R8G8B_line( GLcontext *ctx,
                              GLuint v0, GLuint v1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   struct vertex_buffer *VB = ctx->VB;
   GLint x1 = (GLint) VB->Win[v0][0],  y1 = (GLint) VB->Win[v0][1];
   GLint x2 = (GLint) VB->Win[v1][0],  y2 = (GLint) VB->Win[v1][1];
   GLuint pixel;
   WINCLIP_X(x1,x2);
   WINCLIP_Y(y1,y2);
   pixel = PACK_8R8G8B( VB->Color[pv][0], VB->Color[pv][1], VB->Color[pv][2] );

#define BRESENHAM_PLOT(X,Y)			\
	{					\
	   GLuint *ptr = PIXELADDR4(X,Y);	\
	   *ptr = pixel;			\
	}

   BRESENHAM( x1, y1, x2, y2 );

#undef BRESENHAM_PLOT
}


/*
 * Draw a flat-shaded, PF_5R6G5B line into an XImage.
 */
static void flat_5R6G5B_line( GLcontext *ctx,
                              GLuint v0, GLuint v1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   struct vertex_buffer *VB = ctx->VB;
   GLint x1 = (GLint) VB->Win[v0][0],  y1 = (GLint) VB->Win[v0][1];
   GLint x2 = (GLint) VB->Win[v1][0],  y2 = (GLint) VB->Win[v1][1];
   GLushort pixel;
   WINCLIP_X(x1,x2);
   WINCLIP_Y(y1,y2);
   pixel = PACK_5R6G5B( VB->Color[pv][0], VB->Color[pv][1], VB->Color[pv][2] );

#define BRESENHAM_PLOT(X,Y)			\
	{					\
	   GLushort *ptr = PIXELADDR2(X,Y);	\
	   *ptr = pixel;			\
	}

   BRESENHAM( x1, y1, x2, y2 );

#undef BRESENHAM_PLOT
}



/*
 * Draw a flat-shaded, PF_TRUECOLOR line into an XImage.
 */
static void flat_TRUECOLOR_line( GLcontext *ctx,
                                 GLuint v0, GLuint v1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   struct vertex_buffer *VB = ctx->VB;
   XImage *img = xmesa->xm_buffer->backimage;
   GLint x1 = (GLint) VB->Win[v0][0], y1 = (GLint) VB->Win[v0][1];
   GLint x2 = (GLint) VB->Win[v1][0], y2 = (GLint) VB->Win[v1][1];
   unsigned long pixel = PACK_RGB( VB->Color[pv][0], VB->Color[pv][1],
                                   VB->Color[pv][2] );
   WINCLIP_X(x1,x2);
   WINCLIP_Y(y1,y2);

#define BRESENHAM_PLOT(X,Y)					\
        XPutPixel( img, X, FLIP(Y), pixel );

   BRESENHAM( x1, y1, x2, y2 );

#undef BRESENHAM_PLOT
}


/*
 * Draw a flat-shaded, PF_DITHER 8-bit line into an XImage.
 */
static void flat_DITHER8_line( GLcontext *ctx,
                               GLuint v0, GLuint v1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   struct vertex_buffer *VB = ctx->VB;
   GLint x1 = (GLint) VB->Win[v0][0], y1 = (GLint) VB->Win[v0][1];
   GLint x2 = (GLint) VB->Win[v1][0], y2 = (GLint) VB->Win[v1][1];
   GLint r = VB->Color[pv][0], g = VB->Color[pv][1], b = VB->Color[pv][2];
   DITHER_SETUP;

   WINCLIP_X(x1,x2);
   WINCLIP_Y(y1,y2);

#define BRESENHAM_PLOT(X,Y)	       	\
	GLubyte *ptr = PIXELADDR1(X,Y);	\
	*ptr = DITHER( X, Y, r, g, b);

   BRESENHAM( x1, y1, x2, y2 );

#undef BRESENHAM_PLOT
}


/*
 * Draw a flat-shaded, PF_LOOKUP 8-bit line into an XImage.
 */
static void flat_LOOKUP8_line( GLcontext *ctx,
                               GLuint v0, GLuint v1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   struct vertex_buffer *VB = ctx->VB;
   GLint x1 = (GLint) VB->Win[v0][0], y1 = (GLint) VB->Win[v0][1];
   GLint x2 = (GLint) VB->Win[v1][0], y2 = (GLint) VB->Win[v1][1];
   LOOKUP_SETUP;
   GLubyte pixel = LOOKUP( VB->Color[pv][0], VB->Color[pv][1], VB->Color[pv][2] );

   WINCLIP_X(x1,x2);
   WINCLIP_Y(y1,y2);

#define BRESENHAM_PLOT(X,Y)	       	\
	GLubyte *ptr = PIXELADDR1(X,Y);	\
	*ptr = pixel;

   BRESENHAM( x1, y1, x2, y2 );

#undef BRESENHAM_PLOT
}


/*
 * Draw a flat-shaded, PF_HPCR line into an XImage.
 */
static void flat_HPCR_line( GLcontext *ctx,
                            GLuint v0, GLuint v1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   struct vertex_buffer *VB = ctx->VB;
   GLint x1 = (GLint) VB->Win[v0][0], y1 = (GLint) VB->Win[v0][1];
   GLint x2 = (GLint) VB->Win[v1][0], y2 = (GLint) VB->Win[v1][1];
   GLint r = VB->Color[pv][0], g = VB->Color[pv][1], b = VB->Color[pv][2];

   WINCLIP_X(x1,x2);
   WINCLIP_Y(y1,y2);

#define BRESENHAM_PLOT(X,Y)			\
	GLubyte *ptr = PIXELADDR1(X,Y);		\
	*ptr = DITHER_HPCR( X, Y, r, g, b);

   BRESENHAM( x1, y1, x2, y2 );

#undef BRESENHAM_PLOT
}



/*
 * Draw a flat-shaded, Z-less, PF_TRUECOLOR line into an XImage.
 */
static void flat_TRUECOLOR_z_line( GLcontext *ctx,
                                   GLuint v0, GLuint v1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XImage *img = xmesa->xm_buffer->backimage;
   struct vertex_buffer *VB = ctx->VB;
   GLint x1 = (GLint) VB->Win[v0][0], x2 = (GLint) VB->Win[v1][0];
   GLint y1 = (GLint) VB->Win[v0][1], y2 = (GLint) VB->Win[v1][1];
   GLint z1 = (GLint) (VB->Win[v0][2] + ctx->LineZoffset);
   GLint z2 = (GLint) (VB->Win[v1][2] + ctx->LineZoffset);
   unsigned long pixel = PACK_RGB( VB->Color[pv][0], VB->Color[pv][1],
                                   VB->Color[pv][2] );

   WINCLIP_X(x1,x2);
   WINCLIP_Y(y1,y2);

#define BRESENHAM_PLOT(X,Y,Z,ZBUF)		\
	if (Z < *ZBUF) {			\
           XPutPixel( img, X, FLIP(Y), pixel );	\
	   *ZBUF = Z;				\
	}

   BRESENHAM_Z( ctx, x1, y1, z1, x2, y2, z2 );

#undef BRESENHAM_PLOT
}


/*
 * Draw a flat-shaded, Z-less, PF_8A8B8G8R line into an XImage.
 */
static void flat_8A8B8G8R_z_line( GLcontext *ctx,
                                  GLuint v0, GLuint v1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   struct vertex_buffer *VB = ctx->VB;
   GLint x1 = (GLint) VB->Win[v0][0], x2 = (GLint) VB->Win[v1][0];
   GLint y1 = (GLint) VB->Win[v0][1], y2 = (GLint) VB->Win[v1][1];
   GLint z1 = (GLint) (VB->Win[v0][2] + ctx->LineZoffset);
   GLint z2 = (GLint) (VB->Win[v1][2] + ctx->LineZoffset);
   GLuint pixel = PACK_8B8G8R( VB->Color[pv][0], VB->Color[pv][1],
                               VB->Color[pv][2] );

   WINCLIP_X(x1,x2);
   WINCLIP_Y(y1,y2);

#define BRESENHAM_PLOT(X,Y,Z,ZBUF)		\
	if (Z < *ZBUF) {			\
	   GLuint *ptr = PIXELADDR4(X,Y);	\
	   *ptr = pixel;			\
	   *ZBUF = Z;				\
	}

   BRESENHAM_Z( ctx, x1, y1, z1, x2, y2, z2 );

#undef BRESENHAM_PLOT
}


/*
 * Draw a flat-shaded, Z-less, PF_8R8G8B line into an XImage.
 */
static void flat_8R8G8B_z_line( GLcontext *ctx,
                                GLuint v0, GLuint v1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   struct vertex_buffer *VB = ctx->VB;
   GLint x1 = (GLint) VB->Win[v0][0], x2 = (GLint) VB->Win[v1][0];
   GLint y1 = (GLint) VB->Win[v0][1], y2 = (GLint) VB->Win[v1][1];
   GLint z1 = (GLint) (VB->Win[v0][2] + ctx->LineZoffset);
   GLint z2 = (GLint) (VB->Win[v1][2] + ctx->LineZoffset);
   GLuint pixel = PACK_8R8G8B( VB->Color[pv][0], VB->Color[pv][1],
                               VB->Color[pv][2] );

   WINCLIP_X(x1,x2);
   WINCLIP_Y(y1,y2);

#define BRESENHAM_PLOT(X,Y,Z,ZBUF)		\
	if (Z < *ZBUF) {			\
	   GLuint *ptr = PIXELADDR4(X,Y);	\
	   *ptr = pixel;			\
	   *ZBUF = Z;				\
	}

   BRESENHAM_Z( ctx, x1, y1, z1, x2, y2, z2 );

#undef BRESENHAM_PLOT
}


/*
 * Draw a flat-shaded, Z-less, PF_5R6G5B line into an XImage.
 */
static void flat_5R6G5B_z_line( GLcontext *ctx,
                                GLuint v0, GLuint v1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   struct vertex_buffer *VB = ctx->VB;
   GLint x1 = (GLint) VB->Win[v0][0], x2 = (GLint) VB->Win[v1][0];
   GLint y1 = (GLint) VB->Win[v0][1], y2 = (GLint) VB->Win[v1][1];
   GLint z1 = (GLint) (VB->Win[v0][2] + ctx->LineZoffset);
   GLint z2 = (GLint) (VB->Win[v1][2] + ctx->LineZoffset);
   GLuint pixel = PACK_5R6G5B( VB->Color[pv][0], VB->Color[pv][1],
                               VB->Color[pv][2] );

   WINCLIP_X(x1,x2);
   WINCLIP_Y(y1,y2);

#define BRESENHAM_PLOT(X,Y,Z,ZBUF)		\
	if (Z < *ZBUF) {			\
	   GLushort *ptr = PIXELADDR2(X,Y);	\
	   *ptr = pixel;			\
	   *ZBUF = Z;				\
	}

   BRESENHAM_Z( ctx, x1, y1, z1, x2, y2, z2 );

#undef BRESENHAM_PLOT
}


/*
 * Draw a flat-shaded, Z-less, PF_DITHER 8-bit line into an XImage.
 */
static void flat_DITHER8_z_line( GLcontext *ctx,
                                 GLuint v0, GLuint v1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   struct vertex_buffer *VB = ctx->VB;
   GLint x1 = (GLint) VB->Win[v0][0], x2 = (GLint) VB->Win[v1][0];
   GLint y1 = (GLint) VB->Win[v0][1], y2 = (GLint) VB->Win[v1][1];
   GLint z1 = (GLint) (VB->Win[v0][2] + ctx->LineZoffset);
   GLint z2 = (GLint) (VB->Win[v1][2] + ctx->LineZoffset);
   GLint r = VB->Color[pv][0], g = VB->Color[pv][1], b = VB->Color[pv][2];
   DITHER_SETUP;

   WINCLIP_X(x1,x2);
   WINCLIP_Y(y1,y2);

#define BRESENHAM_PLOT(X,Y,Z,ZBUF)		\
	if (Z < *ZBUF) {			\
	   GLubyte *ptr = PIXELADDR1(X,Y);	\
	   *ptr = DITHER( X, Y, r, g, b);	\
	   *ZBUF = Z;				\
	}

   BRESENHAM_Z( ctx, x1, y1, z1, x2, y2, z2 );

#undef BRESENHAM_PLOT
}


/*
 * Draw a flat-shaded, Z-less, PF_LOOKUP 8-bit line into an XImage.
 */
static void flat_LOOKUP8_z_line( GLcontext *ctx,
                                 GLuint v0, GLuint v1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   struct vertex_buffer *VB = ctx->VB;
   GLint x1 = (GLint) VB->Win[v0][0], x2 = (GLint) VB->Win[v1][0];
   GLint y1 = (GLint) VB->Win[v0][1], y2 = (GLint) VB->Win[v1][1];
   GLint z1 = (GLint) (VB->Win[v0][2] + ctx->LineZoffset);
   GLint z2 = (GLint) (VB->Win[v1][2] + ctx->LineZoffset);
   LOOKUP_SETUP;
   GLubyte pixel = LOOKUP( VB->Color[pv][0], VB->Color[pv][1], VB->Color[pv][2] );

   WINCLIP_X(x1,x2);
   WINCLIP_Y(y1,y2);

#define BRESENHAM_PLOT(X,Y,Z,ZBUF)		\
	if (Z < *ZBUF) {			\
	   GLubyte *ptr = PIXELADDR1(X,Y);	\
	   *ptr = pixel;			\
	   *ZBUF = Z;				\
	}

   BRESENHAM_Z( ctx, x1, y1, z1, x2, y2, z2 );

#undef BRESENHAM_PLOT
}


/*
 * Draw a flat-shaded, Z-less, PF_HPCR line into an XImage.
 */
static void flat_HPCR_z_line( GLcontext *ctx,
                              GLuint v0, GLuint v1, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   struct vertex_buffer *VB = ctx->VB;
   GLint x1 = (GLint) VB->Win[v0][0], x2 = (GLint) VB->Win[v1][0];
   GLint y1 = (GLint) VB->Win[v0][1], y2 = (GLint) VB->Win[v1][1];
   GLint z1 = (GLint) (VB->Win[v0][2] + ctx->LineZoffset);
   GLint z2 = (GLint) (VB->Win[v1][2] + ctx->LineZoffset);
   GLint r = VB->Color[pv][0], g = VB->Color[pv][1], b = VB->Color[pv][2];

   WINCLIP_X(x1,x2);
   WINCLIP_Y(y1,y2);

#define BRESENHAM_PLOT(X,Y,Z,ZBUF)		\
	if (Z < *ZBUF) {			\
	   GLubyte *ptr = PIXELADDR1(X,Y);	\
	   *ptr = DITHER_HPCR( X, Y, r, g, b);	\
	   *ZBUF = Z;				\
	}

   BRESENHAM_Z( ctx, x1, y1, z1, x2, y2, z2 );

#undef BRESENHAM_PLOT
}



/*
 * Examine ctx->Line attributes and set xmesa->xm_buffer->gc1
 * and xmesa->xm_buffer->gc2 appropriately.
 */
static void setup_x_line_options( GLcontext *ctx )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   int i, state, state0, new_state, len, offs;
   int tbit;
   char *dptr;
   int n_segments = 0;
   char dashes[20];
   int line_width, line_style;

   /*** Line Stipple ***/
   if (ctx->Line.StippleFlag) {
      const int pattern = ctx->Line.StipplePattern;

      dptr = dashes;
      state0 = state = ((pattern & 1) != 0);

      /* Decompose the pattern */
      for (i=1,tbit=2,len=1;i<16;++i,tbit=(tbit<<1))
	{
	  new_state = ((tbit & pattern) != 0);
	  if (state != new_state)
	    {
	      *dptr++ = ctx->Line.StippleFactor * len;
	      len = 1;
	      state = new_state;
	    }
	  else
	    ++len;
	}
      *dptr = ctx->Line.StippleFactor * len;
      n_segments = 1 + (dptr - dashes);

      /* ensure an even no. of segments, or X may toggle on/off for consecutive patterns */
      /* if (n_segments & 1)  dashes [n_segments++] = 0;  value of 0 not allowed in dash list */

      /* Handle case where line style starts OFF */
      if (state0 == 0)
        offs = dashes[0];
      else
        offs = 0;

#if 0
fprintf (stderr, "input pattern: 0x%04x, offset %d, %d segments:", pattern, offs, n_segments);
for (i = 0;  i < n_segments;  i++)
fprintf (stderr, " %d", dashes[i]);
fprintf (stderr, "\n");
#endif

      XSetDashes( xmesa->display, xmesa->xm_buffer->gc1, offs, dashes, n_segments );
      XSetDashes( xmesa->display, xmesa->xm_buffer->gc2, offs, dashes, n_segments );

      line_style = LineOnOffDash;
   }
   else {
      line_style = LineSolid;
   }

   /*** Line Width ***/
   line_width = (int) (ctx->Line.Width+0.5F);
   if (line_width < 2) {
      /* Use fast lines when possible */
      line_width = 0;
   }

   /*** Set GC attributes ***/
   XSetLineAttributes( xmesa->display, xmesa->xm_buffer->gc1,
                       line_width, line_style, CapButt, JoinBevel);
   XSetLineAttributes( xmesa->display, xmesa->xm_buffer->gc2,
                       line_width, line_style, CapButt, JoinBevel);
   XSetFillStyle( xmesa->display, xmesa->xm_buffer->gc1, FillSolid );
   XSetFillStyle( xmesa->display, xmesa->xm_buffer->gc2, FillSolid );
}



/*
 * Analyze context state to see if we can provide a fast line drawing
 * function, like those in lines.c.  Otherwise, return NULL.
 */
line_func xmesa_get_line_func( GLcontext *ctx )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   int depth = xmesa->xm_visual->visinfo->depth;

   if (ctx->Line.SmoothFlag)              return NULL;
   if (ctx->Texture.Enabled)              return NULL;
   if (ctx->Light.ShadeModel!=GL_FLAT)    return NULL;

   if (xmesa->xm_buffer->buffer==XIMAGE
       && ctx->RasterMask==DEPTH_BIT
       && ctx->Depth.Func==GL_LESS
       && ctx->Depth.Mask==GL_TRUE
       && ctx->Line.Width==1.0F
       && ctx->Line.StippleFlag==GL_FALSE) {
      switch (xmesa->pixelformat) {
         case PF_TRUECOLOR:
            return flat_TRUECOLOR_z_line;
         case PF_8A8B8G8R:
            return flat_8A8B8G8R_z_line;
         case PF_8R8G8B:
            return flat_8R8G8B_z_line;
         case PF_5R6G5B:
            return flat_5R6G5B_z_line;
         case PF_DITHER:
            return (depth==8) ? flat_DITHER8_z_line : NULL;
         case PF_LOOKUP:
            return (depth==8) ? flat_LOOKUP8_z_line : NULL;
         case PF_HPCR:
            return flat_HPCR_z_line;
         default:
            return NULL;
      }
   }
   if (xmesa->xm_buffer->buffer==XIMAGE
       && ctx->RasterMask==0
       && ctx->Line.Width==1.0F
       && ctx->Line.StippleFlag==GL_FALSE) {
      switch (xmesa->pixelformat) {
         case PF_TRUECOLOR:
            return flat_TRUECOLOR_line;
         case PF_8A8B8G8R:
            return flat_8A8B8G8R_line;
         case PF_8R8G8B:
            return flat_8R8G8B_line;
         case PF_5R6G5B:
            return flat_5R6G5B_line;
         case PF_DITHER:
            return (depth==8) ? flat_DITHER8_line : NULL;
         case PF_LOOKUP:
            return (depth==8) ? flat_LOOKUP8_line : NULL;
         case PF_HPCR:
            return flat_HPCR_line;
	 default:
	    return NULL;
      }
   }
   if (xmesa->xm_buffer->buffer!=XIMAGE && ctx->RasterMask==0) {
      setup_x_line_options( ctx );
      return flat_pixmap_line;
   }
   return NULL;
}




/**********************************************************************/
/***                   Triangle rendering                            ***/
/**********************************************************************/

/*
 * Render a triangle into a pixmap, any pixel format, flat shaded and
 * no raster ops.
 */
void flat_pixmap_triangle( GLcontext *ctx,
 	                   GLuint v0, GLuint v1, GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   struct vertex_buffer *VB = ctx->VB;
   XPoint p[3];
   GC gc;
   if (VB->MonoColor) {
      gc = xmesa->xm_buffer->gc1;  /* use current color */
   }
   else {
      unsigned long pixel;
      if (xmesa->xm_visual->gl_visual->RGBAflag) {
         pixel = xmesa_color_to_pixel( xmesa,
                                       VB->Color[pv][0], VB->Color[pv][1],
                                       VB->Color[pv][2], VB->Color[pv][3] );
      }
      else {
         pixel = VB->Index[pv];
      }
      gc = xmesa->xm_buffer->gc2;
      XSetForeground( xmesa->display, gc, pixel );
   }
   p[0].x =       (GLint) (VB->Win[v0][0] + 0.5f);
   p[0].y = FLIP( (GLint) (VB->Win[v0][1] - 0.5f) );
   p[1].x =       (GLint) (VB->Win[v1][0] + 0.5f);
   p[1].y = FLIP( (GLint) (VB->Win[v1][1] - 0.5f) );
   p[2].x =       (GLint) (VB->Win[v2][0] + 0.5f);
   p[2].y = FLIP( (GLint) (VB->Win[v2][1] - 0.5f) );
   XFillPolygon( xmesa->display, xmesa->xm_buffer->buffer, gc,
		 p, 3, Convex, CoordModeOrigin );
}



/*
 * XImage, smooth, depth-buffered, PF_TRUECOLOR triangle.
 */
static void smooth_TRUECOLOR_z_triangle( GLcontext *ctx,
                                         GLuint v0, GLuint v1, GLuint v2,
                                         GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XImage *img = xmesa->xm_buffer->backimage;
#define INTERP_Z 1
#define INTERP_RGB 1
#define INNER_LOOP( LEFT, RIGHT, Y )					\
{									\
   GLint i, xx = LEFT, yy = FLIP(Y), len = RIGHT-LEFT;			\
   for (i=0;i<len;i++,xx++) {						\
      GLdepth z = FixedToDepth(ffz);					\
      if (z < zRow[i]) {						\
         unsigned long p = PACK_RGB( FixedToInt(ffr), FixedToInt(ffg),	\
					 FixedToInt(ffb) );		\
         XPutPixel( img, xx, yy, p );					\
         zRow[i] = z;							\
      }									\
      ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;			\
      ffz += fdzdx;							\
   }									\
}
#include "tritemp.h"
}



/*
 * XImage, smooth, depth-buffered, PF_8A8B8G8R triangle.
 */
static void smooth_8A8B8G8R_z_triangle( GLcontext *ctx,
                                         GLuint v0, GLuint v1, GLuint v2,
                                         GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define INNER_LOOP( LEFT, RIGHT, Y )					\
{									\
   GLint i, len = RIGHT-LEFT;						\
   for (i=0;i<len;i++) {						\
      GLdepth z = FixedToDepth(ffz);					\
      if (z < zRow[i]) {						\
         pRow[i] = PACK_8B8G8R( FixedToInt(ffr), FixedToInt(ffg),	\
				 FixedToInt(ffb) );			\
         zRow[i] = z;							\
      }									\
      ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;			\
      ffz += fdzdx;							\
   }									\
}
#include "tritemp.h"
}


/*
 * XImage, smooth, depth-buffered, PF_8R8G8B triangle.
 */
static void smooth_8R8G8B_z_triangle( GLcontext *ctx,
                                         GLuint v0, GLuint v1, GLuint v2,
                                         GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define INNER_LOOP( LEFT, RIGHT, Y )					\
{									\
   GLint i, len = RIGHT-LEFT;						\
   for (i=0;i<len;i++) {						\
      GLdepth z = FixedToDepth(ffz);					\
      if (z < zRow[i]) {						\
         pRow[i] = PACK_8R8G8B( FixedToInt(ffr), FixedToInt(ffg),	\
				 FixedToInt(ffb) );			\
         zRow[i] = z;							\
      }									\
      ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;			\
      ffz += fdzdx;							\
   }									\
}
#include "tritemp.h"
}



/*
 * XImage, smooth, depth-buffered, PF_5R6G5B triangle.
 */
static void smooth_5R6G5B_z_triangle( GLcontext *ctx,
                                         GLuint v0, GLuint v1, GLuint v2,
                                         GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(X,Y)
#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define INNER_LOOP( LEFT, RIGHT, Y )					\
{									\
   GLint i, len = RIGHT-LEFT;						\
   for (i=0;i<len;i++) {						\
      GLdepth z = FixedToDepth(ffz);					\
      if (z < zRow[i]) {						\
         pRow[i] = PACK_5R6G5B( FixedToInt(ffr), FixedToInt(ffg),	\
				 FixedToInt(ffb) );			\
         zRow[i] = z;							\
      }									\
      ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;			\
      ffz += fdzdx;							\
   }									\
}
#include "tritemp.h"
}


/*
 * XImage, smooth, depth-buffered, 8-bit, PF_DITHER8 triangle.
 */
static void smooth_DITHER8_z_triangle( GLcontext *ctx,
                                       GLuint v0, GLuint v1, GLuint v2,
                                       GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define INNER_LOOP( LEFT, RIGHT, Y )					\
{									\
   GLint i, xx = LEFT, yy = FLIP(Y), len = RIGHT-LEFT;			\
   XDITHER_SETUP(yy);							\
   for (i=0;i<len;i++,xx++) {						\
      GLdepth z = FixedToDepth(ffz);					\
      if (z < zRow[i]) {						\
         pRow[i] = XDITHER( xx, FixedToInt(ffr), FixedToInt(ffg),	\
				FixedToInt(ffb) );			\
         zRow[i] = z;							\
      }									\
      ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;			\
      ffz += fdzdx;							\
   }									\
}
#include "tritemp.h"
}



/*
 * XImage, smooth, depth-buffered, PF_DITHER triangle.
 */
static void smooth_DITHER_z_triangle( GLcontext *ctx,
                                       GLuint v0, GLuint v1, GLuint v2,
                                       GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XImage *img = xmesa->xm_buffer->backimage;
#define INTERP_Z 1
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define INNER_LOOP( LEFT, RIGHT, Y )					\
{									\
   GLint i, xx = LEFT, yy = FLIP(Y), len = RIGHT-LEFT;			\
   XDITHER_SETUP(yy);							\
   for (i=0;i<len;i++,xx++) {						\
      GLdepth z = FixedToDepth(ffz);					\
      if (z < zRow[i]) {						\
	 unsigned long p = XDITHER( xx, FixedToInt(ffr),		\
				 FixedToInt(ffg), FixedToInt(ffb) );	\
	 XPutPixel( img, xx, yy, p );					\
         zRow[i] = z;							\
      }									\
      ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;			\
      ffz += fdzdx;							\
   }									\
}
#include "tritemp.h"
}


/*
 * XImage, smooth, depth-buffered, 8-bit PF_LOOKUP triangle.
 */
static void smooth_LOOKUP8_z_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                       GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define INNER_LOOP( LEFT, RIGHT, Y )					\
{									\
   GLint i, len = RIGHT-LEFT;						\
   LOOKUP_SETUP;							\
   for (i=0;i<len;i++) {						\
      GLdepth z = FixedToDepth(ffz);					\
      if (z < zRow[i]) {						\
         pRow[i] = LOOKUP( FixedToInt(ffr), FixedToInt(ffg),		\
				 FixedToInt(ffb) );			\
         zRow[i] = z;							\
      }									\
      ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;			\
      ffz += fdzdx;							\
   }									\
}
#include "tritemp.h"
}



/*
 * XImage, smooth, depth-buffered, 8-bit PF_HPCR triangle.
 */
static void smooth_HPCR_z_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                    GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define INNER_LOOP( LEFT, RIGHT, Y )					\
{									\
   GLint i, xx = LEFT, yy = FLIP(Y), len = RIGHT-LEFT;			\
   for (i=0;i<len;i++,xx++) {						\
      GLdepth z = FixedToDepth(ffz);					\
      if (z < zRow[i]) {						\
         pRow[i] = DITHER_HPCR( xx, yy, FixedToInt(ffr),		\
				 FixedToInt(ffg), FixedToInt(ffb) );	\
         zRow[i] = z;							\
      }									\
      ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;			\
      ffz += fdzdx;							\
   }									\
}
#include "tritemp.h"
}



/*
 * XImage, flat, depth-buffered, PF_TRUECOLOR triangle.
 */
static void flat_TRUECOLOR_z_triangle( GLcontext *ctx,
                        	       GLuint v0, GLuint v1, GLuint v2,
                                       GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XImage *img = xmesa->xm_buffer->backimage;
#define INTERP_Z 1
#define SETUP_CODE					\
   unsigned long pixel = PACK_RGB( VB->Color[pv][0],	\
		 VB->Color[pv][1], VB->Color[pv][2] );

#define INNER_LOOP( LEFT, RIGHT, Y )					\
{									\
   GLint i, xx = LEFT, yy = FLIP(Y), len = RIGHT-LEFT;			\
   for (i=0;i<len;i++,xx++) {						\
      GLdepth z = FixedToDepth(ffz);					\
      if (z < zRow[i]) {						\
         XPutPixel( img, xx, yy, pixel );				\
         zRow[i] = z;							\
      }									\
      ffz += fdzdx;							\
   }									\
}
#include "tritemp.h"
}


/*
 * XImage, flat, depth-buffered, PF_8A8B8G8R triangle.
 */
static void flat_8A8B8G8R_z_triangle( GLcontext *ctx, GLuint v0,
                        	      GLuint v1, GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE					\
   unsigned long p = PACK_8B8G8R( VB->Color[pv][0],	\
		 VB->Color[pv][1], VB->Color[pv][2] );
#define INNER_LOOP( LEFT, RIGHT, Y )					\
{									\
   GLint i, len = RIGHT-LEFT;						\
   for (i=0;i<len;i++) {						\
      GLdepth z = FixedToDepth(ffz);					\
      if (z < zRow[i]) {						\
	 pRow[i] = p;							\
         zRow[i] = z;							\
      }									\
      ffz += fdzdx;							\
   }									\
}
#include "tritemp.h"
}


/*
 * XImage, flat, depth-buffered, PF_8R8G8B triangle.
 */
static void flat_8R8G8B_z_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                    GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE					\
   unsigned long p = PACK_8R8G8B( VB->Color[pv][0],	\
		 VB->Color[pv][1], VB->Color[pv][2] );
#define INNER_LOOP( LEFT, RIGHT, Y )			\
{							\
   GLint i, len = RIGHT-LEFT;				\
   for (i=0;i<len;i++) {				\
      GLdepth z = FixedToDepth(ffz);			\
      if (z < zRow[i]) {				\
	 pRow[i] = p;					\
         zRow[i] = z;					\
      }							\
      ffz += fdzdx;					\
   }							\
}
#include "tritemp.h"
}


/*
 * XImage, flat, depth-buffered, PF_5R6G5B triangle.
 */
static void flat_5R6G5B_z_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                    GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(X,Y)
#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE					\
   unsigned long p = PACK_5R6G5B( VB->Color[pv][0],	\
		 VB->Color[pv][1], VB->Color[pv][2] );
#define INNER_LOOP( LEFT, RIGHT, Y )			\
{							\
   GLint i, len = RIGHT-LEFT;				\
   for (i=0;i<len;i++) {				\
      GLdepth z = FixedToDepth(ffz);			\
      if (z < zRow[i]) {				\
	 pRow[i] = p;					\
         zRow[i] = z;					\
      }							\
      ffz += fdzdx;					\
   }							\
}
#include "tritemp.h"
}


/*
 * XImage, flat, depth-buffered, 8-bit PF_DITHER triangle.
 */
static void flat_DITHER8_z_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                     GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE	\
   FLAT_DITHER_SETUP( VB->Color[pv][0], VB->Color[pv][1], VB->Color[pv][2] );

#define INNER_LOOP( LEFT, RIGHT, Y )				\
{								\
   GLint i, xx = LEFT, len = RIGHT-LEFT;			\
   FLAT_DITHER_ROW_SETUP(FLIP(Y));				\
   for (i=0;i<len;i++,xx++) {					\
      GLdepth z = FixedToDepth(ffz);				\
      if (z < zRow[i]) {					\
	 pRow[i] = FLAT_DITHER(xx);				\
         zRow[i] = z;						\
      }								\
      ffz += fdzdx;						\
   }								\
}
#include "tritemp.h"
}


/*
 * XImage, flat, depth-buffered, PF_DITHER triangle.
 */
static void flat_DITHER_z_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                    GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XImage *img = xmesa->xm_buffer->backimage;
#define INTERP_Z 1
#define SETUP_CODE	\
   FLAT_DITHER_SETUP( VB->Color[pv][0], VB->Color[pv][1], VB->Color[pv][2] );

#define INNER_LOOP( LEFT, RIGHT, Y )					\
{									\
   GLint i, xx = LEFT, yy = FLIP(Y), len = RIGHT-LEFT;			\
   FLAT_DITHER_ROW_SETUP(yy);						\
   for (i=0;i<len;i++,xx++) {						\
      GLdepth z = FixedToDepth(ffz);					\
      if (z < zRow[i]) {						\
         unsigned long p = FLAT_DITHER(xx);				\
	 XPutPixel( img, xx, yy, p );					\
         zRow[i] = z;							\
      }									\
      ffz += fdzdx;							\
   }									\
}
#include "tritemp.h"
}


/*
 * XImage, flat, depth-buffered, 8-bit PF_HPCR triangle.
 */
static void flat_HPCR_z_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                  GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE				\
   GLubyte r = VB->Color[pv][0];		\
   GLubyte g = VB->Color[pv][1];		\
   GLubyte b = VB->Color[pv][2];
#define INNER_LOOP( LEFT, RIGHT, Y )				\
{								\
   GLint i, xx = LEFT, yy = FLIP(Y), len = RIGHT-LEFT;		\
   for (i=0;i<len;i++,xx++) {					\
      GLdepth z = FixedToDepth(ffz);				\
      if (z < zRow[i]) {					\
	 pRow[i] = DITHER_HPCR( xx, yy, r, g, b );		\
         zRow[i] = z;						\
      }								\
      ffz += fdzdx;						\
   }								\
}
#include "tritemp.h"
}



/*
 * XImage, flat, depth-buffered, 8-bit PF_LOOKUP triangle.
 */
static void flat_LOOKUP8_z_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                        	     GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_Z 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE				\
   LOOKUP_SETUP;				\
   GLubyte r = VB->Color[pv][0];		\
   GLubyte g = VB->Color[pv][1];		\
   GLubyte b = VB->Color[pv][2];		\
   GLubyte p = LOOKUP(r,g,b);
#define INNER_LOOP( LEFT, RIGHT, Y )			\
{							\
   GLint i, len = RIGHT-LEFT;				\
   for (i=0;i<len;i++) {				\
      GLdepth z = FixedToDepth(ffz);			\
      if (z < zRow[i]) {				\
	 pRow[i] = p;					\
         zRow[i] = z;					\
      }							\
      ffz += fdzdx;					\
   }							\
}
#include "tritemp.h"
}



/*
 * XImage, smooth, NON-depth-buffered, PF_TRUECOLOR triangle.
 */
static void smooth_TRUECOLOR_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                       GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XImage *img = xmesa->xm_buffer->backimage;
#define INTERP_RGB 1
#define INNER_LOOP( LEFT, RIGHT, Y )					\
{									\
   GLint xx, yy = FLIP(Y);						\
   for (xx=LEFT;xx<RIGHT;xx++) {					\
      unsigned long p = PACK_RGB( FixedToInt(ffr), FixedToInt(ffg),	\
					FixedToInt(ffb) );		\
      XPutPixel( img, xx, yy, p );					\
      ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;			\
   }									\
}
#include "tritemp.h"
}


/*
 * XImage, smooth, NON-depth-buffered, PF_8A8B8G8R triangle.
 */
static void smooth_8A8B8G8R_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
				      GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define INNER_LOOP( LEFT, RIGHT, Y )					\
{									\
   GLint xx;								\
   PIXEL_TYPE *pixel = pRow;						\
   for (xx=LEFT;xx<RIGHT;xx++,pixel++) {				\
      *pixel = PACK_8B8G8R( FixedToInt(ffr), FixedToInt(ffg),		\
				FixedToInt(ffb) );			\
      ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;			\
   }									\
}
#include "tritemp.h"
}


/*
 * XImage, smooth, NON-depth-buffered, PF_8R8G8B triangle.
 */
static void smooth_8R8G8B_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                    GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define INNER_LOOP( LEFT, RIGHT, Y )					\
{									\
   GLint xx;								\
   PIXEL_TYPE *pixel = pRow;						\
   for (xx=LEFT;xx<RIGHT;xx++,pixel++) {				\
      *pixel = PACK_8R8G8B( FixedToInt(ffr), FixedToInt(ffg),		\
				FixedToInt(ffb) );			\
      ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;			\
   }									\
}
#include "tritemp.h"
}


/*
 * XImage, smooth, NON-depth-buffered, PF_5R6G5B triangle.
 */
static void smooth_5R6G5B_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
				    GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(X,Y)
#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define INNER_LOOP( LEFT, RIGHT, Y )					\
{									\
   GLint xx;								\
   PIXEL_TYPE *pixel = pRow;						\
   for (xx=LEFT;xx<RIGHT;xx++,pixel++) {				\
      *pixel = PACK_5R6G5B( FixedToInt(ffr), FixedToInt(ffg),		\
				FixedToInt(ffb) );			\
      ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;			\
   }									\
}
#include "tritemp.h"
}


/*
 * XImage, smooth, NON-depth-buffered, 8-bit PF_DITHER triangle.
 */
static void smooth_DITHER8_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
				     GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define INNER_LOOP( LEFT, RIGHT, Y )					\
{									\
   GLint xx, yy = FLIP(Y);						\
   PIXEL_TYPE *pixel = pRow;						\
   XDITHER_SETUP(yy);							\
   for (xx=LEFT;xx<RIGHT;xx++,pixel++) {				\
      *pixel = XDITHER( xx, FixedToInt(ffr), FixedToInt(ffg),		\
				FixedToInt(ffb) );			\
      ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;			\
   }									\
}
#include "tritemp.h"
}


/*
 * XImage, smooth, NON-depth-buffered, PF_DITHER triangle.
 */
static void smooth_DITHER_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                    GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XImage *img = xmesa->xm_buffer->backimage;
#define INTERP_RGB 1
#define INNER_LOOP( LEFT, RIGHT, Y )					\
{									\
   GLint xx, yy = FLIP(Y);						\
   XDITHER_SETUP(yy);							\
   for (xx=LEFT;xx<RIGHT;xx++) {					\
      unsigned long p = XDITHER( xx, FixedToInt(ffr),			\
				FixedToInt(ffg), FixedToInt(ffb) );	\
      XPutPixel( img, xx, yy, p );					\
      ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;			\
   }									\
}
#include "tritemp.h"
}


/*
 * XImage, smooth, NON-depth-buffered, 8-bit PF_LOOKUP triangle.
 */
static void smooth_LOOKUP8_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                     GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define INNER_LOOP( LEFT, RIGHT, Y )					\
{									\
   GLint xx;								\
   PIXEL_TYPE *pixel = pRow;						\
   LOOKUP_SETUP;							\
   for (xx=LEFT;xx<RIGHT;xx++,pixel++) {				\
      *pixel = LOOKUP( FixedToInt(ffr), FixedToInt(ffg),		\
			FixedToInt(ffb) );				\
      ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;			\
   }									\
}
#include "tritemp.h"
}



/*
 * XImage, smooth, NON-depth-buffered, 8-bit PF_HPCR triangle.
 */
static void smooth_HPCR_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                  GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define INTERP_RGB 1
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define INNER_LOOP( LEFT, RIGHT, Y )					\
{									\
   GLint xx, yy = FLIP(Y);						\
   PIXEL_TYPE *pixel = pRow;						\
   for (xx=LEFT;xx<RIGHT;xx++,pixel++) {				\
      *pixel = DITHER_HPCR( xx, yy, FixedToInt(ffr),			\
				FixedToInt(ffg), FixedToInt(ffb) );	\
      ffr += fdrdx;  ffg += fdgdx;  ffb += fdbdx;			\
   }									\
}
#include "tritemp.h"
}


/*
 * XImage, flat, NON-depth-buffered, PF_TRUECOLOR triangle.
 */
static void flat_TRUECOLOR_triangle( GLcontext *ctx, GLuint v0,
                                     GLuint v1, GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XImage *img = xmesa->xm_buffer->backimage;
#define SETUP_CODE					\
   unsigned long pixel = PACK_RGB( VB->Color[pv][0],	\
		 VB->Color[pv][1], VB->Color[pv][2] );
#define INNER_LOOP( LEFT, RIGHT, Y )				\
{								\
   GLint xx, yy = FLIP(Y);					\
   for (xx=LEFT;xx<RIGHT;xx++) {				\
      XPutPixel( img, xx, yy, pixel );				\
   }								\
}
#include "tritemp.h"
}


/*
 * XImage, flat, NON-depth-buffered, PF_8A8B8G8R triangle.
 */
static void flat_8A8B8G8R_triangle( GLcontext *ctx, GLuint v0,
                        	    GLuint v1, GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE					\
   unsigned long p = PACK_8B8G8R( VB->Color[pv][0],	\
		 VB->Color[pv][1], VB->Color[pv][2] );
#define INNER_LOOP( LEFT, RIGHT, Y )			\
{							\
   GLint xx;						\
   PIXEL_TYPE *pixel = pRow;				\
   for (xx=LEFT;xx<RIGHT;xx++,pixel++) {		\
      *pixel = p;					\
   }							\
}
#include "tritemp.h"
}


/*
 * XImage, flat, NON-depth-buffered, PF_8R8G8B triangle.
 */
static void flat_8R8G8B_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                  GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define PIXEL_ADDRESS(X,Y) PIXELADDR4(X,Y)
#define PIXEL_TYPE GLuint
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE					\
   unsigned long p = PACK_8R8G8B( VB->Color[pv][0],	\
		 VB->Color[pv][1], VB->Color[pv][2] );
#define INNER_LOOP( LEFT, RIGHT, Y )			\
{							\
   GLint xx;						\
   PIXEL_TYPE *pixel = pRow;				\
   for (xx=LEFT;xx<RIGHT;xx++,pixel++) {		\
      *pixel = p;					\
   }							\
}
#include "tritemp.h"
}


/*
 * XImage, flat, NON-depth-buffered, PF_5R6G5B triangle.
 */
static void flat_5R6G5B_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                  GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define PIXEL_ADDRESS(X,Y) PIXELADDR2(X,Y)
#define PIXEL_TYPE GLushort
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE					\
   unsigned long p = PACK_5R6G5B( VB->Color[pv][0],	\
		 VB->Color[pv][1], VB->Color[pv][2] );
#define INNER_LOOP( LEFT, RIGHT, Y )			\
{							\
   GLint xx;						\
   PIXEL_TYPE *pixel = pRow;				\
   for (xx=LEFT;xx<RIGHT;xx++,pixel++) {		\
      *pixel = p;					\
   }							\
}
#include "tritemp.h"
}


/*
 * XImage, flat, NON-depth-buffered, 8-bit PF_DITHER triangle.
 */
static void flat_DITHER8_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                   GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE	\
   FLAT_DITHER_SETUP( VB->Color[pv][0], VB->Color[pv][1], VB->Color[pv][2] );

#define INNER_LOOP( LEFT, RIGHT, Y )			\
{							\
   GLint xx;						\
   PIXEL_TYPE *pixel = pRow;				\
   FLAT_DITHER_ROW_SETUP(FLIP(Y));			\
   for (xx=LEFT;xx<RIGHT;xx++,pixel++) {		\
      *pixel = FLAT_DITHER(xx);				\
   }							\
}
#include "tritemp.h"
}


/*
 * XImage, flat, NON-depth-buffered, PF_DITHER triangle.
 */
static void flat_DITHER_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                  GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   XImage *img = xmesa->xm_buffer->backimage;
#define SETUP_CODE	\
   FLAT_DITHER_SETUP( VB->Color[pv][0], VB->Color[pv][1], VB->Color[pv][2] );

#define INNER_LOOP( LEFT, RIGHT, Y )			\
{							\
   GLint xx, yy = FLIP(Y);				\
   FLAT_DITHER_ROW_SETUP(yy);				\
   for (xx=LEFT;xx<RIGHT;xx++) {			\
      unsigned long p = FLAT_DITHER(xx);		\
      XPutPixel( img, xx, yy, p );			\
   }							\
}
#include "tritemp.h"
}


/*
 * XImage, flat, NON-depth-buffered, 8-bit PF_HPCR triangle.
 */
static void flat_HPCR_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                                GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE				\
   GLubyte r = VB->Color[pv][0];		\
   GLubyte g = VB->Color[pv][1];		\
   GLubyte b = VB->Color[pv][2];
#define INNER_LOOP( LEFT, RIGHT, Y )		\
{						\
   GLint xx, yy = FLIP(Y);			\
   PIXEL_TYPE *pixel = pRow;			\
   for (xx=LEFT;xx<RIGHT;xx++,pixel++) {	\
      *pixel = DITHER_HPCR( xx, yy, r, g, b );	\
   }						\
}
#include "tritemp.h"
}


/*
 * XImage, flat, NON-depth-buffered, 8-bit PF_LOOKUP triangle.
 */
static void flat_LOOKUP8_triangle( GLcontext *ctx, GLuint v0, GLuint v1,
                        	   GLuint v2, GLuint pv )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
#define PIXEL_ADDRESS(X,Y) PIXELADDR1(X,Y)
#define PIXEL_TYPE GLubyte
#define BYTES_PER_ROW (xmesa->xm_buffer->backimage->bytes_per_line)
#define SETUP_CODE				\
   LOOKUP_SETUP;				\
   GLubyte r = VB->Color[pv][0];		\
   GLubyte g = VB->Color[pv][1];		\
   GLubyte b = VB->Color[pv][2];		\
   GLubyte p = LOOKUP(r,g,b);
#define INNER_LOOP( LEFT, RIGHT, Y )		\
{						\
   GLint xx;					\
   PIXEL_TYPE *pixel = pRow;			\
   for (xx=LEFT;xx<RIGHT;xx++,pixel++) {	\
      *pixel = p;				\
   }						\
}
#include "tritemp.h"
}




/*
 * This function is called if we're about to render triangles into an
 * X window/pixmap.  It sets the polygon stipple pattern if enabled.
 */
static void setup_x_polygon_options( GLcontext *ctx )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   int fill_type;

   if (ctx->Polygon.StippleFlag) {
      /*
       * Allocate polygon stippling stuff once for this context.
       */
      if (xmesa->xm_buffer->stipple_pixmap == 0) {
         XGCValues values;
         XMesaBuffer b = xmesa->xm_buffer;
         b->stipple_pixmap = XCreatePixmap( xmesa->display,
                                            b->buffer, 32, 32, 1 );
         values.function = GXcopy;
         values.foreground = 1;
         values.background = 0;
         b->stipple_gc = XCreateGC( xmesa->display, b->stipple_pixmap,
                                    (GCFunction|GCForeground|GCBackground),
                                    &values );
         b->stipple_ximage = XCreateImage( xmesa->display,
                                           xmesa->xm_visual->visinfo->visual,
                                           1, ZPixmap, 0,
                                           (char *)ctx->PolygonStipple,
                                           32, 32, 8, 0 );
         b->stipple_ximage->byte_order = LSBFirst;
         b->stipple_ximage->bitmap_bit_order = LSBFirst;
         b->stipple_ximage->bitmap_unit = 32;
      }

      /*
       * NOTE: We don't handle the following here!
       *    GL_UNPACK_SWAP_BYTES
       *    GL_UNPACK_LSB_FIRST
       */
      XPutImage( xmesa->display,
		 xmesa->xm_buffer->stipple_pixmap,
		 xmesa->xm_buffer->stipple_gc,
                 xmesa->xm_buffer->stipple_ximage, 0, 0, 0, 0, 32, 32 );
      XSetStipple( xmesa->display, xmesa->xm_buffer->gc1,
                   xmesa->xm_buffer->stipple_pixmap );
      XSetStipple( xmesa->display, xmesa->xm_buffer->gc2,
                   xmesa->xm_buffer->stipple_pixmap );
      fill_type = FillStippled;
   }
   else {
      fill_type = FillSolid;
   }

   XSetFillStyle( xmesa->display, xmesa->xm_buffer->gc1, fill_type );
   XSetFillStyle( xmesa->display, xmesa->xm_buffer->gc2, fill_type );
}



triangle_func xmesa_get_triangle_func( GLcontext *ctx )
{
   XMesaContext xmesa = (XMesaContext) ctx->DriverCtx;
   int depth = xmesa->xm_visual->visinfo->depth;

   if (ctx->Polygon.SmoothFlag)     return NULL;
   if (ctx->Texture.Enabled)        return NULL;

   if (xmesa->xm_buffer->buffer==XIMAGE) {
      if (   ctx->Light.ShadeModel==GL_SMOOTH
          && ctx->RasterMask==DEPTH_BIT
          && ctx->Depth.Func==GL_LESS
          && ctx->Depth.Mask==GL_TRUE
          && ctx->Polygon.StippleFlag==GL_FALSE) {
         switch (xmesa->pixelformat) {
            case PF_TRUECOLOR:
	       return smooth_TRUECOLOR_z_triangle;
            case PF_8A8B8G8R:
               return smooth_8A8B8G8R_z_triangle;
            case PF_8R8G8B:
               return smooth_8R8G8B_z_triangle;
            case PF_5R6G5B:
               return smooth_5R6G5B_z_triangle;
            case PF_HPCR:
	       return smooth_HPCR_z_triangle;
            case PF_DITHER:
               return (depth==8) ? smooth_DITHER8_z_triangle
                                        : smooth_DITHER_z_triangle;
            case PF_LOOKUP:
               return (depth==8) ? smooth_LOOKUP8_z_triangle : NULL;
            default:
               return NULL;
         }
      }
      if (   ctx->Light.ShadeModel==GL_FLAT
          && ctx->RasterMask==DEPTH_BIT
          && ctx->Depth.Func==GL_LESS
          && ctx->Depth.Mask==GL_TRUE
          && ctx->Polygon.StippleFlag==GL_FALSE) {
         switch (xmesa->pixelformat) {
            case PF_TRUECOLOR:
	       return flat_TRUECOLOR_z_triangle;
            case PF_8A8B8G8R:
               return flat_8A8B8G8R_z_triangle;
            case PF_8R8G8B:
               return flat_8R8G8B_z_triangle;
            case PF_5R6G5B:
               return flat_5R6G5B_z_triangle;
            case PF_HPCR:
	       return flat_HPCR_z_triangle;
            case PF_DITHER:
               return (depth==8) ? flat_DITHER8_z_triangle
                                        : flat_DITHER_z_triangle;
            case PF_LOOKUP:
               return (depth==8) ? flat_LOOKUP8_z_triangle : NULL;
            default:
               return NULL;
         }
      }
      if (   ctx->RasterMask==0   /* no depth test */
          && ctx->Light.ShadeModel==GL_SMOOTH
          && ctx->Polygon.StippleFlag==GL_FALSE) {
         switch (xmesa->pixelformat) {
            case PF_TRUECOLOR:
	       return smooth_TRUECOLOR_triangle;
            case PF_8A8B8G8R:
               return smooth_8A8B8G8R_triangle;
            case PF_8R8G8B:
               return smooth_8R8G8B_triangle;
            case PF_5R6G5B:
               return smooth_5R6G5B_triangle;
            case PF_HPCR:
	       return smooth_HPCR_triangle;
            case PF_DITHER:
               return (depth==8) ? smooth_DITHER8_triangle
                                        : smooth_DITHER_triangle;
            case PF_LOOKUP:
               return (depth==8) ? smooth_LOOKUP8_triangle : NULL;
            default:
               return NULL;
         }
      }

      if (   ctx->RasterMask==0   /* no depth test */
          && ctx->Light.ShadeModel==GL_FLAT
          && ctx->Polygon.StippleFlag==GL_FALSE) {
         switch (xmesa->pixelformat) {
            case PF_TRUECOLOR:
	       return flat_TRUECOLOR_triangle;
            case PF_8A8B8G8R:
               return flat_8A8B8G8R_triangle;
            case PF_8R8G8B:
               return flat_8R8G8B_triangle;
            case PF_5R6G5B:
               return flat_5R6G5B_triangle;
            case PF_HPCR:
	       return flat_HPCR_triangle;
            case PF_DITHER:
               return (depth==8) ? flat_DITHER8_triangle
                                        : flat_DITHER_triangle;
            case PF_LOOKUP:
               return (depth==8) ? flat_LOOKUP8_triangle : NULL;
            default:
               return NULL;
         }
      }

      return NULL;
   }
   else {
      /* pixmap */
      if (ctx->Light.ShadeModel==GL_FLAT && ctx->RasterMask==0) {
         setup_x_polygon_options( ctx );
         return flat_pixmap_triangle;
      }
      return NULL;
   }
}

