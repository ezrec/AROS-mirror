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
 * Revision 3.3  1998/06/18 02:38:45  brianp
 * re-implemented INTERP_STUV code for textured lines
 *
 * Revision 3.2  1998/06/17 04:08:12  brianp
 * fixed texture interpolation bug (Marten Stromberg)
 *
 * Revision 3.1  1998/06/07 22:37:44  brianp
 * cast all sizeof() operators to GLint to stop compiler warnings
 *
 * Revision 3.0  1998/01/31 20:55:39  brianp
 * initial rev
 *
 */


/*
 * Line Rasterizer Template
 *
 * This file is #include'd to generate custom line rasterizers.
 *
 * The following macros may be defined to indicate what auxillary information
 * must be interplated along the line:
 *    INTERP_Z      - if defined, interpolate Z values
 *    INTERP_RGB    - if defined, interpolate RGB values
 *    INTERP_ALPHA  - if defined, interpolate Alpha values
 *    INTERP_INDEX  - if defined, interpolate color index values
 *    INTERP_ST     - if defined, interpolate integer ST texcoords
 *                         (fast, simple 2-D texture mapping)
 *    INTERP_STUV    - if defined, interpolate float STU texcoords with
 *                         perspective correction
 *
 * When one can directly address pixels in the color buffer the following
 * macros can be defined and used to directly compute pixel addresses during
 * rasterization (see pixelPtr):
 *    PIXEL_TYPE          - the datatype of a pixel (GLubyte, GLushort, GLuint)
 *    BYTES_PER_ROW       - number of bytes per row in the color buffer
 *    PIXEL_ADDRESS(X,Y)  - returns the address of pixel at (X,Y) where
 *                          Y==0 at bottom of screen and increases upward.
 *
 * Optionally, one may provide one-time setup code
 *    SETUP_CODE    - code which is to be executed once per line
 *
 * To enable line stippling define STIPPLE = 1
 * To enable wide lines define WIDE = 1
 * 
 * To actually "plot" each pixel either the PLOT macro or
 * (XMAJOR_PLOT and YMAJOR_PLOT macros) must be defined...
 *    PLOT(X,Y) - code to plot a pixel.  Example:
 *                if (Z < *zPtr) {
 *                   *zPtr = Z;
 *                   color = pack_rgb( FixedToInt(r0), FixedToInt(g0),
 *                                     FixedToInt(b0) );
 *                   put_pixel( X, Y, color );
 *                }
 *
 * This code was designed for the origin to be in the lower-left corner.
 *
 */


/*void line( GLcontext *ctx, GLuint vert0, GLuint vert1, GLuint pvert )*/
{
   struct vertex_buffer *VB = ctx->VB;
/*
   GLint x0 = (GLint) VB->Win[vert0][0], dx = (GLint) VB->Win[vert1][0] - x0;
   GLint y0 = (GLint) VB->Win[vert0][1], dy = (GLint) VB->Win[vert1][1] - y0;
*/
   GLint x0 = (GLint) VB->Win[vert0][0], x1 = (GLint) VB->Win[vert1][0];
   GLint y0 = (GLint) VB->Win[vert0][1], y1 = (GLint) VB->Win[vert1][1];
   GLint dx, dy;
#if INTERP_XY
   GLint xstep, ystep;
#endif
#if INTERP_Z
   GLint z0, z1, dz, zPtrXstep, zPtrYstep;
   GLdepth *zPtr;
#endif
#if INTERP_RGB
   GLfixed r0 = IntToFixed(VB->Color[vert0][0]);
   GLfixed dr = IntToFixed(VB->Color[vert1][0]) - r0;
   GLfixed g0 = IntToFixed(VB->Color[vert0][1]);
   GLfixed dg = IntToFixed(VB->Color[vert1][1]) - g0;
   GLfixed b0 = IntToFixed(VB->Color[vert0][2]);
   GLfixed db = IntToFixed(VB->Color[vert1][2]) - b0;
#endif
#if INTERP_ALPHA
   GLfixed a0 = IntToFixed(VB->Color[vert0][3]);
   GLfixed da = IntToFixed(VB->Color[vert1][3]) - a0;
#endif
#if INTERP_INDEX
   GLint i0 = VB->Index[vert0] << 8,  di = (GLint) (VB->Index[vert1] << 8)-i0;
#endif
#if INTERP_ST
   GLfixed s0 = FloatToFixed(VB->TexCoord[vert0][0] * S_SCALE);
   GLfixed ds = FloatToFixed(VB->TexCoord[vert1][0] * S_SCALE) - s0;
   GLfixed t0 = FloatToFixed(VB->TexCoord[vert0][1] * T_SCALE);
   GLfixed dt = FloatToFixed(VB->TexCoord[vert1][1] * T_SCALE) - t0;
#endif
#if INTERP_STUV
   /* h denotes hyperbolic */
   GLfloat invw0 = 1.0F / VB->Clip[vert0][3];
   GLfloat invw1 = 1.0F / VB->Clip[vert1][3];
   GLfloat hs0 = invw0 * VB->TexCoord[vert0][0];
   GLfloat dhs = invw1 * VB->TexCoord[vert1][0] - hs0;
   GLfloat ht0 = invw0 * VB->TexCoord[vert0][1];
   GLfloat dht = invw1 * VB->TexCoord[vert1][1] - ht0;
   GLfloat hu0 = invw0 * VB->TexCoord[vert0][2];
   GLfloat dhu = invw1 * VB->TexCoord[vert1][2] - hu0;
   GLfloat hv0 = invw0 * VB->TexCoord[vert0][3];
   GLfloat dhv = invw1 * VB->TexCoord[vert1][3] - hv0;
#endif
#ifdef PIXEL_ADDRESS
   PIXEL_TYPE *pixelPtr;
   GLint pixelXstep, pixelYstep;
#endif

#if WIDE
   GLint width, min, max;
   width = (GLint) CLAMP( ctx->Line.Width, MIN_LINE_WIDTH, MAX_LINE_WIDTH );
   min = -width / 2;
   max = min + width - 1;
#endif

/*
 * Despite being clipped to the view volume, the line's window coordinates
 * may just lie outside the window bounds.  That is, if the legal window
 * coordinates are [0,W-1][0,H-1], it's possible for x==W and/or y==H.
 * This quick and dirty code nudges the endpoints inside the window if
 * necessary.
 */
#if CLIP_HACK
   {
      GLint w = ctx->Buffer->Width;
      GLint h = ctx->Buffer->Height;
      if ((x0==w) | (x1==w)) {
         if ((x0==w) & (x1==w))
           return;
         x0 -= x0==w;
         x1 -= x1==w;
      }
      if ((y0==h) | (y1==h)) {
         if ((y0==h) & (y1==h))
           return;
         y0 -= y0==h;
         y1 -= y1==h;
      }
   }
#endif
   dx = x1 - x0;
   dy = y1 - y0;
   if (dx==0 && dy==0) {
      return;
   }

   /*
    * Setup
    */
#ifdef SETUP_CODE
   SETUP_CODE
#endif

#if INTERP_Z
   zPtr = Z_ADDRESS(ctx,x0,y0);
#  if DEPTH_BITS==16
      z0 = FloatToFixed(VB->Win[vert0][2]);
      z1 = FloatToFixed(VB->Win[vert1][2]);
#  else
      z0 = (int) VB->Win[vert0][2];
      z1 = (int) VB->Win[vert1][2];
#  endif
#endif
#ifdef PIXEL_ADDRESS
   pixelPtr = (PIXEL_TYPE *) PIXEL_ADDRESS(x0,y0);
#endif

   if (dx<0) {
      dx = -dx;   /* make positive */
#if INTERP_XY
      xstep = -1;
#endif
#ifdef INTERP_Z
      zPtrXstep = -((GLint)sizeof(GLdepth));
#endif
#ifdef PIXEL_ADDRESS
      pixelXstep = -((GLint)sizeof(PIXEL_TYPE));
#endif
   }
   else {
#if INTERP_XY
      xstep = 1;
#endif
#if INTERP_Z
      zPtrXstep = ((GLint)sizeof(GLdepth));
#endif
#ifdef PIXEL_ADDRESS
      pixelXstep = ((GLint)sizeof(PIXEL_TYPE));
#endif
   }

   if (dy<0) {
      dy = -dy;   /* make positive */
#if INTERP_XY
      ystep = -1;
#endif
#if INTERP_Z
      zPtrYstep = -ctx->Buffer->Width * ((GLint)sizeof(GLdepth));
#endif
#ifdef PIXEL_ADDRESS
      pixelYstep = BYTES_PER_ROW;
#endif
   }
   else {
#if INTERP_XY
      ystep = 1;
#endif
#if INTERP_Z
      zPtrYstep = ctx->Buffer->Width * ((GLint)sizeof(GLdepth));
#endif
#ifdef PIXEL_ADDRESS
      pixelYstep = -(BYTES_PER_ROW);
#endif
   }

   /*
    * Draw
    */

   if (dx>dy) {
      /*
       * X-major line
       */
      GLint i;
      GLint errorInc = dy+dy;
      GLint error = errorInc-dx;
      GLint errorDec = error-dx;
#if INTERP_Z
      dz = (z1-z0) / dx;
#endif
#if INTERP_RGB
      dr /= dx;   /* convert from whole line delta to per-pixel delta */
      dg /= dx;
      db /= dx;
#endif
#if INTERP_ALPHA
      da /= dx;
#endif
#if INTERP_INDEX
      di /= dx;
#endif
#if INTERP_ST
      ds /= dx;
      dt /= dx;
#endif
#if INTERP_STUV
      {
         GLfloat invDx = 1.0F / (GLfloat) dx;
         dhs *= invDx;
         dht *= invDx;
         dhu *= invDx;
         dhv *= invDx;
      }
#endif
      for (i=0;i<dx;i++) {
#if STIPPLE
         GLushort m;
         m = 1 << ((ctx->StippleCounter/ctx->Line.StippleFactor) & 0xf);
         if (ctx->Line.StipplePattern & m) {
#endif
#if INTERP_Z
#  if DEPTH_BITS==16
            GLdepth Z = FixedToInt(z0);
#  else
            GLdepth Z = z0;
#  endif
#endif
#if INTERP_INDEX
            GLint I = i0 >> 8;
#endif
#if WIDE
            GLint yy;
            GLint ymin = y0 + min;
            GLint ymax = y0 + max;
            for (yy=ymin;yy<=ymax;yy++) {
               PLOT( x0, yy );
            }
#else
#  ifdef XMAJOR_PLOT
            XMAJOR_PLOT( x0, y0 );
#  else
            PLOT( x0, y0 );
#  endif
#endif /*WIDE*/
#if STIPPLE
        }
	ctx->StippleCounter++;
#endif
#if INTERP_XY
         x0 += xstep;
#endif
#if INTERP_Z
         zPtr = (GLdepth *) ((GLubyte*) zPtr + zPtrXstep);
         z0 += dz;
#endif
#if INTERP_RGB
         r0 += dr;
         g0 += dg;
         b0 += db;
#endif
#if INTERP_ALPHA
         a0 += da;
#endif
#if INTERP_INDEX
         i0 += di;
#endif
#if INTERP_ST
         s0 += ds;
         t0 += dt;
#endif
#if INTERP_STUV
         hs0 += dhs;
         ht0 += dht;
         hu0 += dhu;
         hv0 += dhv;
#endif
#ifdef PIXEL_ADDRESS
         pixelPtr = (PIXEL_TYPE*) ((GLubyte*) pixelPtr + pixelXstep);
#endif
         if (error<0) {
            error += errorInc;
         }
         else {
            error += errorDec;
#if INTERP_XY
            y0 += ystep;
#endif
#if INTERP_Z
            zPtr = (GLdepth *) ((GLubyte*) zPtr + zPtrYstep);
#endif
#ifdef PIXEL_ADDRESS
            pixelPtr = (PIXEL_TYPE*) ((GLubyte*) pixelPtr + pixelYstep);
#endif
         }
      }
   }
   else {
      /*
       * Y-major line
       */
      GLint i;
      GLint errorInc = dx+dx;
      GLint error = errorInc-dy;
      GLint errorDec = error-dy;
#if INTERP_Z
      dz = (z1-z0) / dy;
#endif
#if INTERP_RGB
      dr /= dy;   /* convert from whole line delta to per-pixel delta */
      dg /= dy;
      db /= dy;
#endif
#if INTERP_ALPHA
      da /= dy;
#endif
#if INTERP_INDEX
      di /= dy;
#endif
#if INTERP_ST
      ds /= dy;
      dt /= dy;
#endif
#if INTERP_STUV
      {
         GLfloat invDy = 1.0F / (GLfloat) dy;
         dhs *= invDy;
         dht *= invDy;
         dhu *= invDy;
         dhv *= invDy;
      }
#endif
      for (i=0;i<dy;i++) {
#if STIPPLE
         GLushort m;
         m = 1 << ((ctx->StippleCounter/ctx->Line.StippleFactor) & 0xf);
         if (ctx->Line.StipplePattern & m) {
#endif
#if INTERP_Z
#  if DEPTH_BITS==16
            GLdepth Z = FixedToInt(z0);
#  else
            GLdepth Z = z0;
#  endif
#endif
#if INTERP_INDEX
            GLint I = i0 >> 8;
#endif
#if WIDE
            GLint xx;
            GLint xmin = x0 + min;
            GLint xmax = x0 + max;
            for (xx=xmin;xx<=xmax;xx++) {
               PLOT( xx, y0 );
            }
#else
#  ifdef YMAJOR_PLOT
            YMAJOR_PLOT( x0, y0 );
#  else
            PLOT( x0, y0 );
#  endif
#endif /*WIDE*/
#if STIPPLE
        }
	ctx->StippleCounter++;
#endif
#if INTERP_XY
         y0 += ystep;
#endif
#if INTERP_Z
         zPtr = (GLdepth *) ((GLubyte*) zPtr + zPtrYstep);
         z0 += dz;
#endif
#if INTERP_RGB
         r0 += dr;
         g0 += dg;
         b0 += db;
#endif
#if INTERP_ALPHA
         a0 += da;
#endif
#if INTERP_INDEX
         i0 += di;
#endif
#if INTERP_ST
         s0 += ds;
         t0 += dt;
#endif
#if INTERP_STUV
         hs0 += dhs;
         ht0 += dht;
         hu0 += dhu;
         hv0 += dhv;
#endif
#ifdef PIXEL_ADDRESS
         pixelPtr = (PIXEL_TYPE*) ((GLubyte*) pixelPtr + pixelYstep);
#endif
         if (error<0) {
            error += errorInc;
         }
         else {
            error += errorDec;
#if INTERP_XY
            x0 += xstep;
#endif
#if INTERP_Z
            zPtr = (GLdepth *) ((GLubyte*) zPtr + zPtrXstep);
#endif
#ifdef PIXEL_ADDRESS
            pixelPtr = (PIXEL_TYPE*) ((GLubyte*) pixelPtr + pixelXstep);
#endif
         }
      }
   }

}


#undef INTERP_XY
#undef INTERP_Z
#undef INTERP_RGB
#undef INTERP_ALPHA
#undef INTERP_INDEX
#undef PIXEL_ADDRESS
#undef PIXEL_TYPE
#undef BYTES_PER_ROW
#undef SETUP_CODE
#undef PLOT
#undef XMAJOR_PLOT
#undef YMAJOR_PLOT
#undef CLIP_HACK
#undef STIPPLE
#undef WIDE
