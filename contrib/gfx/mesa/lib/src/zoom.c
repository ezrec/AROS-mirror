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
 * Revision 3.3  1998/03/28 03:57:13  brianp
 * added CONST macro to fix IRIX compilation problems
 *
 * Revision 3.2  1998/03/15 17:56:56  brianp
 * added a const in gl_write_zoomed_rgba_span()
 *
 * Revision 3.1  1998/02/08 20:22:40  brianp
 * added gl_write_zoomed_rgb_span()
 *
 * Revision 3.0  1998/02/01 22:13:55  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <assert.h>
#include "macros.h"
#include "span.h"
#include "stencil.h"
#include "zoom.h"
#endif



/*
 * Write a span of pixels to the frame buffer while applying a pixel zoom.
 * This is only used by glDrawPixels and glCopyPixels.
 * Input:  n - number of pixels in input row
 *         x, y - destination of the span
 *         z - depth values for the span
 *         red, green, blue, alpha - array of colors
 *         y0 - location of first row in the image we're drawing.
 */
void
gl_write_zoomed_rgba_span( GLcontext *ctx,
                           GLuint n, GLint x, GLint y, const GLdepth z[],
                           CONST GLubyte rgba[][4], GLint y0 )
{
   GLint m;
   GLint r0, r1, row, r;
   GLint i, j, skipcol;
   GLubyte zrgba[MAX_WIDTH][4];  /* zoomed pixel colors */
   GLdepth zdepth[MAX_WIDTH];  /* zoomed depth values */
   GLint maxwidth = MIN2( ctx->Buffer->Width, MAX_WIDTH );
   const GLuint *srcRGBA32 = (const GLuint *) rgba;
   GLuint *dstRGBA32 = (GLuint *) zrgba;

   /* compute width of output row */
   m = (GLint) ABSF( n * ctx->Pixel.ZoomX );
   if (m==0) {
      return;
   }
   if (ctx->Pixel.ZoomX<0.0) {
      /* adjust x coordinate for left/right mirroring */
      x = x - m;
   }

   /* compute which rows to draw */
   row = y-y0;
   r0 = y0 + (GLint) (row * ctx->Pixel.ZoomY);
   r1 = y0 + (GLint) ((row+1) * ctx->Pixel.ZoomY);
   if (r0==r1) {
      return;
   }
   else if (r1<r0) {
      GLint rtmp = r1;
      r1 = r0;
      r0 = rtmp;
   }

   /* return early if r0...r1 is above or below window */
   if (r0<0 && r1<0) {
      /* below window */
      return;
   }
   if (r0>=ctx->Buffer->Height && r1>=ctx->Buffer->Height) {
      /* above window */
      return;
   }

   /* check if left edge is outside window */
   skipcol = 0;
   if (x<0) {
      skipcol = -x;
      m += x;
   }
   /* make sure span isn't too long or short */
   if (m>maxwidth) {
      m = maxwidth;
   }
   else if (m<=0) {
      return;
   }

   assert( m <= MAX_WIDTH );

   /* zoom the span horizontally */
   if (ctx->Pixel.ZoomX==-1.0F) {
      /* n==m */
      for (j=0;j<m;j++) {
         i = n - (j+skipcol) - 1;
         dstRGBA32[j] = srcRGBA32[i];
         zdepth[j] = z[i];
      }
   }
   else {
      GLfloat xscale = 1.0F / ctx->Pixel.ZoomX;
      for (j=0;j<m;j++) {
         i = (j+skipcol) * xscale;
         if (i<0)  i = n + i - 1;
         dstRGBA32[j] = srcRGBA32[i];
         zdepth[j] = z[i];
      }
   }

   /* write the span */
   for (r=r0; r<r1; r++) {
      gl_write_rgba_span( ctx, m, x+skipcol, r, zdepth, zrgba, GL_BITMAP );
   }
}



void
gl_write_zoomed_rgb_span( GLcontext *ctx,
                          GLuint n, GLint x, GLint y, const GLdepth z[],
                          CONST GLubyte rgb[][3], GLint y0 )
{
   GLint m;
   GLint r0, r1, row, r;
   GLint i, j, skipcol;
   GLubyte zrgba[MAX_WIDTH][4];  /* zoomed pixel colors */
   GLdepth zdepth[MAX_WIDTH];  /* zoomed depth values */
   GLint maxwidth = MIN2( ctx->Buffer->Width, MAX_WIDTH );

   /* compute width of output row */
   m = (GLint) ABSF( n * ctx->Pixel.ZoomX );
   if (m==0) {
      return;
   }
   if (ctx->Pixel.ZoomX<0.0) {
      /* adjust x coordinate for left/right mirroring */
      x = x - m;
   }

   /* compute which rows to draw */
   row = y-y0;
   r0 = y0 + (GLint) (row * ctx->Pixel.ZoomY);
   r1 = y0 + (GLint) ((row+1) * ctx->Pixel.ZoomY);
   if (r0==r1) {
      return;
   }
   else if (r1<r0) {
      GLint rtmp = r1;
      r1 = r0;
      r0 = rtmp;
   }

   /* return early if r0...r1 is above or below window */
   if (r0<0 && r1<0) {
      /* below window */
      return;
   }
   if (r0>=ctx->Buffer->Height && r1>=ctx->Buffer->Height) {
      /* above window */
      return;
   }

   /* check if left edge is outside window */
   skipcol = 0;
   if (x<0) {
      skipcol = -x;
      m += x;
   }
   /* make sure span isn't too long or short */
   if (m>maxwidth) {
      m = maxwidth;
   }
   else if (m<=0) {
      return;
   }

   assert( m <= MAX_WIDTH );

   /* zoom the span horizontally */
   if (ctx->Pixel.ZoomX==-1.0F) {
      /* n==m */
      for (j=0;j<m;j++) {
         i = n - (j+skipcol) - 1;
         zrgba[j][0] = rgb[i][0];
         zrgba[j][1] = rgb[i][1];
         zrgba[j][2] = rgb[i][2];
         zrgba[j][3] = 255;
         zdepth[j] = z[i];
      }
   }
   else {
      GLfloat xscale = 1.0F / ctx->Pixel.ZoomX;
      for (j=0;j<m;j++) {
         i = (j+skipcol) * xscale;
         if (i<0)  i = n + i - 1;
         zrgba[j][0] = rgb[i][0];
         zrgba[j][1] = rgb[i][1];
         zrgba[j][2] = rgb[i][2];
         zrgba[j][3] = 255;
         zdepth[j] = z[i];
      }
   }

   /* write the span */
   for (r=r0; r<r1; r++) {
      gl_write_rgba_span( ctx, m, x+skipcol, r, zdepth, zrgba, GL_BITMAP );
   }
}



/*
 * As above, but write CI pixels.
 */
void
gl_write_zoomed_index_span( GLcontext *ctx,
                            GLuint n, GLint x, GLint y, const GLdepth z[],
                            const GLuint indexes[], GLint y0 )
{
   GLint m;
   GLint r0, r1, row, r;
   GLint i, j, skipcol;
   GLuint zindexes[MAX_WIDTH];  /* zoomed color indexes */
   GLdepth zdepth[MAX_WIDTH];  /* zoomed depth values */
   GLint maxwidth = MIN2( ctx->Buffer->Width, MAX_WIDTH );

   /* compute width of output row */
   m = (GLint) ABSF( n * ctx->Pixel.ZoomX );
   if (m==0) {
      return;
   }
   if (ctx->Pixel.ZoomX<0.0) {
      /* adjust x coordinate for left/right mirroring */
      x = x - m;
   }

   /* compute which rows to draw */
   row = y-y0;
   r0 = y0 + (GLint) (row * ctx->Pixel.ZoomY);
   r1 = y0 + (GLint) ((row+1) * ctx->Pixel.ZoomY);
   if (r0==r1) {
      return;
   }
   else if (r1<r0) {
      GLint rtmp = r1;
      r1 = r0;
      r0 = rtmp;
   }

   /* return early if r0...r1 is above or below window */
   if (r0<0 && r1<0) {
      /* below window */
      return;
   }
   if (r0>=ctx->Buffer->Height && r1>=ctx->Buffer->Height) {
      /* above window */
      return;
   }

   /* check if left edge is outside window */
   skipcol = 0;
   if (x<0) {
      skipcol = -x;
      m += x;
   }
   /* make sure span isn't too long or short */
   if (m>maxwidth) {
      m = maxwidth;
   }
   else if (m<=0) {
      return;
   }

   assert( m <= MAX_WIDTH );

   /* zoom the span horizontally */
   if (ctx->Pixel.ZoomX==-1.0F) {
      /* n==m */
      for (j=0;j<m;j++) {
         i = n - (j+skipcol) - 1;
         zindexes[j] = indexes[i];
         zdepth[j]   = z[i];
      }
   }
   else {
      GLfloat xscale = 1.0F / ctx->Pixel.ZoomX;
      for (j=0;j<m;j++) {
         i = (j+skipcol) * xscale;
         if (i<0)  i = n + i - 1;
         zindexes[j] = indexes[i];
         zdepth[j] = z[i];
      }
   }

   /* write the span */
   for (r=r0; r<r1; r++) {
      gl_write_index_span( ctx, m, x+skipcol, r, zdepth, zindexes, GL_BITMAP );
   }
}



/*
 * As above, but write stencil values.
 */
void
gl_write_zoomed_stencil_span( GLcontext *ctx,
                              GLuint n, GLint x, GLint y,
                              const GLubyte stencil[], GLint y0 )
{
   GLint m;
   GLint r0, r1, row, r;
   GLint i, j, skipcol;
   GLubyte zstencil[MAX_WIDTH];  /* zoomed stencil values */
   GLint maxwidth = MIN2( ctx->Buffer->Width, MAX_WIDTH );

   /* compute width of output row */
   m = (GLint) ABSF( n * ctx->Pixel.ZoomX );
   if (m==0) {
      return;
   }
   if (ctx->Pixel.ZoomX<0.0) {
      /* adjust x coordinate for left/right mirroring */
      x = x - m;
   }

   /* compute which rows to draw */
   row = y-y0;
   r0 = y0 + (GLint) (row * ctx->Pixel.ZoomY);
   r1 = y0 + (GLint) ((row+1) * ctx->Pixel.ZoomY);
   if (r0==r1) {
      return;
   }
   else if (r1<r0) {
      GLint rtmp = r1;
      r1 = r0;
      r0 = rtmp;
   }

   /* return early if r0...r1 is above or below window */
   if (r0<0 && r1<0) {
      /* below window */
      return;
   }
   if (r0>=ctx->Buffer->Height && r1>=ctx->Buffer->Height) {
      /* above window */
      return;
   }

   /* check if left edge is outside window */
   skipcol = 0;
   if (x<0) {
      skipcol = -x;
      m += x;
   }
   /* make sure span isn't too long or short */
   if (m>maxwidth) {
      m = maxwidth;
   }
   else if (m<=0) {
      return;
   }

   assert( m <= MAX_WIDTH );

   /* zoom the span horizontally */
   if (ctx->Pixel.ZoomX==-1.0F) {
      /* n==m */
      for (j=0;j<m;j++) {
         i = n - (j+skipcol) - 1;
         zstencil[j] = stencil[i];
      }
   }
   else {
      GLfloat xscale = 1.0F / ctx->Pixel.ZoomX;
      for (j=0;j<m;j++) {
         i = (j+skipcol) * xscale;
         if (i<0)  i = n + i - 1;
         zstencil[j] = stencil[i];
      }
   }

   /* write the span */
   for (r=r0; r<r1; r++) {
      gl_write_stencil_span( ctx, m, x+skipcol, r, zstencil );
   }
}
