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
 * Revision 3.2  1998/03/28 03:57:13  brianp
 * added CONST macro to fix IRIX compilation problems
 *
 * Revision 3.1  1998/03/27 04:40:28  brianp
 * added a few const keywords
 *
 * Revision 3.0  1998/01/31 20:45:34  brianp
 * initial rev
 *
 */



/*
 * Software alpha planes.  Many frame buffers don't have alpha bits so
 * we simulate them in software.
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <stdlib.h>
#include <string.h>
#include "alphabuf.h"
#include "context.h"
#include "macros.h"
#include "types.h"
#endif



#define ALPHA_ADDR(X,Y)  (ctx->Buffer->Alpha + (Y) * ctx->Buffer->Width + (X))



/*
 * Allocate a new front and back alpha buffer.
 */
void gl_alloc_alpha_buffers( GLcontext* ctx )
{
   GLint bytes = ctx->Buffer->Width * ctx->Buffer->Height * sizeof(GLubyte);

   if (ctx->Visual->FrontAlphaEnabled) {
      if (ctx->Buffer->FrontAlpha) {
         free( ctx->Buffer->FrontAlpha );
      }
      ctx->Buffer->FrontAlpha = (GLubyte *) malloc( bytes );
      if (!ctx->Buffer->FrontAlpha) {
         /* out of memory */
         gl_error( ctx, GL_OUT_OF_MEMORY, "Couldn't allocate front alpha buffer" );
      }
   }
   if (ctx->Visual->BackAlphaEnabled) {
      if (ctx->Buffer->BackAlpha) {
         free( ctx->Buffer->BackAlpha );
      }
      ctx->Buffer->BackAlpha = (GLubyte *) malloc( bytes );
      if (!ctx->Buffer->BackAlpha) {
         /* out of memory */
         gl_error( ctx, GL_OUT_OF_MEMORY, "Couldn't allocate back alpha buffer" );
      }
   }
   if (ctx->Color.DrawBuffer==GL_FRONT) {
      ctx->Buffer->Alpha = ctx->Buffer->FrontAlpha;
   }
   if (ctx->Color.DrawBuffer==GL_BACK) {
      ctx->Buffer->Alpha = ctx->Buffer->BackAlpha;
   }
}



/*
 * Clear the front and/or back alpha planes.
 */
void gl_clear_alpha_buffers( GLcontext* ctx )
{
   GLint buffer;

   /* Loop over front and back buffers */
   for (buffer=0;buffer<2;buffer++) {

      /* Get pointer to front or back buffer */
      GLubyte *abuffer = NULL;
      if (buffer==0
          && (   ctx->Color.DrawBuffer==GL_FRONT
              || ctx->Color.DrawBuffer==GL_FRONT_AND_BACK)
          && ctx->Visual->FrontAlphaEnabled && ctx->Buffer->FrontAlpha) {
         abuffer = ctx->Buffer->FrontAlpha;
      }
      else if (buffer==1
               && (   ctx->Color.DrawBuffer==GL_BACK
                   || ctx->Color.DrawBuffer==GL_FRONT_AND_BACK)
               && ctx->Visual->BackAlphaEnabled && ctx->Buffer->BackAlpha) {
         abuffer = ctx->Buffer->BackAlpha;
      }

      /* Clear the alpha buffer */
      if (abuffer) {
         GLubyte aclear = (GLint) (ctx->Color.ClearColor[3] * 255.0F);
         if (ctx->Scissor.Enabled) {
            /* clear scissor region */
            GLint i, j;
            for (j=0;j<ctx->Scissor.Height;j++) {
               GLubyte *aptr = ALPHA_ADDR(ctx->Buffer->Xmin,
                                          ctx->Buffer->Ymin+j);
               for (i=0;i<ctx->Scissor.Width;i++) {
                  *aptr++ = aclear;
               }
            }
         }
         else {
            /* clear whole buffer */
            MEMSET( abuffer, aclear, ctx->Buffer->Width*ctx->Buffer->Height );
         }
      }
   }
}



void gl_write_alpha_span( GLcontext* ctx, GLuint n, GLint x, GLint y,
                          CONST GLubyte rgba[][4], const GLubyte mask[] )
{
   GLubyte *aptr = ALPHA_ADDR( x, y );
   GLuint i;

   if (mask) {
      for (i=0;i<n;i++) {
         if (mask[i]) {
            *aptr = rgba[i][ACOMP];
         }
         aptr++;
      }
   }
   else {
      for (i=0;i<n;i++) {
         *aptr++ = rgba[i][ACOMP];
      }
   }
}


void gl_write_mono_alpha_span( GLcontext* ctx, GLuint n, GLint x, GLint y,
                               GLubyte alpha, const GLubyte mask[] )
{
   GLubyte *aptr = ALPHA_ADDR( x, y );
   GLuint i;

   if (mask) {
      for (i=0;i<n;i++) {
         if (mask[i]) {
            *aptr = alpha;
         }
         aptr++;
      }
   }
   else {
      for (i=0;i<n;i++) {
         *aptr++ = alpha;
      }
   }
}


void gl_write_alpha_pixels( GLcontext* ctx,
                            GLuint n, const GLint x[], const GLint y[],
                            CONST GLubyte rgba[][4], const GLubyte mask[] )
{
   GLuint i;

   if (mask) {
      for (i=0;i<n;i++) {
         if (mask[i]) {
            GLubyte *aptr = ALPHA_ADDR( x[i], y[i] );
            *aptr = rgba[i][ACOMP];
         }
      }
   }
   else {
      for (i=0;i<n;i++) {
         GLubyte *aptr = ALPHA_ADDR( x[i], y[i] );
         *aptr = rgba[i][ACOMP];
      }
   }
}


void gl_write_mono_alpha_pixels( GLcontext* ctx,
                                 GLuint n, const GLint x[], const GLint y[],
                                 GLubyte alpha, const GLubyte mask[] )
{
   GLuint i;

   if (mask) {
      for (i=0;i<n;i++) {
         if (mask[i]) {
            GLubyte *aptr = ALPHA_ADDR( x[i], y[i] );
            *aptr = alpha;
         }
      }
   }
   else {
      for (i=0;i<n;i++) {
         GLubyte *aptr = ALPHA_ADDR( x[i], y[i] );
         *aptr = alpha;
      }
   }
}



void gl_read_alpha_span( GLcontext *ctx,
                         GLuint n, GLint x, GLint y, GLubyte rgba[][4] )
{
   GLubyte *aptr = ALPHA_ADDR( x, y );
   GLuint i;
   for (i=0;i<n;i++) {
      rgba[i][ACOMP] = *aptr++;
   }
}


void gl_read_alpha_pixels( GLcontext *ctx,
                           GLuint n, const GLint x[], const GLint y[],
                           GLubyte rgba[][4], const GLubyte mask[] )
{
   GLuint i;
   for (i=0;i<n;i++) {
      if (mask[i]) {
         GLubyte *aptr = ALPHA_ADDR( x[i], y[i] );
         rgba[i][ACOMP] = *aptr;
      }
   }
}



