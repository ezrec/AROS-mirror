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
 * Revision 3.4  1998/04/01 02:58:52  brianp
 * applied Miklos Fazekas's 3-31-98 Macintosh changes
 *
 * Revision 3.3  1998/03/05 02:59:05  brianp
 * [RGBA]COMP wasn't being used in a few places
 *
 * Revision 3.2  1998/02/20 04:50:44  brianp
 * implemented GL_SGIS_multitexture
 *
 * Revision 3.1  1998/02/15 01:32:37  brianp
 * fixed a comment
 *
 * Revision 3.0  1998/01/31 21:00:28  brianp
 * initial rev
 *
 */


/*
 * Pixel buffer:
 *
 * As fragments are produced (by point, line, and bitmap drawing) they
 * are accumlated in a buffer.  When the buffer is full or has to be
 * flushed (glEnd), we apply all enabled rasterization functions to the
 * pixels and write the results to the display buffer.  The goal is to
 * maximize the number of pixels processed inside loops and to minimize
 * the number of function calls.
 */



#ifdef PC_HEADER
#include "all.h"
#else
#include <stdlib.h>
#include <string.h>
#include "alpha.h"
#include "alphabuf.h"
#include "blend.h"
#include "depth.h"
#include "fog.h"
#include "logic.h"
#include "macros.h"
#include "masking.h"
#include "pb.h"
#include "scissor.h"
#include "stencil.h"
#include "texture.h"
#include "types.h"
#endif



/*
 * Allocate and initialize a new pixel buffer structure.
 */
struct pixel_buffer *gl_alloc_pb(void)
{
   struct pixel_buffer *pb;
   pb = (struct pixel_buffer *) calloc(sizeof(struct pixel_buffer), 1);
   if (pb) {
      int i;
      /* set non-zero fields */
      pb->primitive = GL_BITMAP;
      /* Set all lambda values to 0.0 since we don't do mipmapping for
       * points or lines and want to use the level 0 texture image.
       */
      for (i=0; i<PB_SIZE; i++) {
         pb->lambda[i] = 0.0;
      }
   }
   return pb;
}




/*
 * When the pixel buffer is full, or needs to be flushed, call this
 * function.  All the pixels in the pixel buffer will be subjected
 * to texturing, scissoring, stippling, alpha testing, stenciling,
 * depth testing, blending, and finally written to the frame buffer.
 */
void gl_flush_pb( GLcontext *ctx )
{
   struct pixel_buffer* PB = ctx->PB;

   GLubyte mask[PB_SIZE];
   GLubyte saved[PB_SIZE][4];

   if (PB->count==0)  goto CleanUp;

   /* initialize mask array and clip pixels simultaneously */
   {
      GLint xmin = ctx->Buffer->Xmin;
      GLint xmax = ctx->Buffer->Xmax;
      GLint ymin = ctx->Buffer->Ymin;
      GLint ymax = ctx->Buffer->Ymax;
      GLint *x = PB->x;
      GLint *y = PB->y;
      GLuint i, n = PB->count;
      for (i=0;i<n;i++) {
         mask[i] = (x[i]>=xmin) & (x[i]<=xmax) & (y[i]>=ymin) & (y[i]<=ymax);
      }
   }

   if (ctx->Visual->RGBAflag) {
      /* RGBA COLOR PIXELS */
      if (PB->mono && ctx->MutablePixels) {
	 /* Copy flat color to all pixels */
         GLuint i;
         for (i=0; i<PB->count; i++) {
            PB->rgba[i][RCOMP] = PB->color[RCOMP];
            PB->rgba[i][GCOMP] = PB->color[GCOMP];
            PB->rgba[i][BCOMP] = PB->color[BCOMP];
            PB->rgba[i][ACOMP] = PB->color[ACOMP];
         }
      }

      /* If each pixel can be of a different color... */
      if (ctx->MutablePixels || !PB->mono) {

	 if (ctx->Texture.Enabled) {
	    gl_texture_pixels( ctx, 0,
                               PB->count, PB->s, PB->t, PB->u, PB->lambda,
                               PB->rgba);
	 }

	 if (ctx->Fog.Enabled
             && (ctx->Hint.Fog==GL_NICEST || PB->primitive==GL_BITMAP
                 || ctx->Texture.Enabled)) {
	    gl_fog_rgba_pixels( ctx, PB->count, PB->z, PB->rgba );
	 }

         /* Scissoring already done above */

	 if (ctx->Color.AlphaEnabled) {
	    if (gl_alpha_test( ctx, PB->count, PB->rgba, mask )==0) {
	       goto CleanUp;
	    }
	 }

	 if (ctx->Stencil.Enabled) {
	    /* first stencil test */
	    if (gl_stencil_pixels( ctx, PB->count, PB->x, PB->y, mask )==0) {
	       goto CleanUp;
	    }
	    /* depth buffering w/ stencil */
	    gl_depth_stencil_pixels( ctx, PB->count, PB->x, PB->y, PB->z, mask );
	 }
	 else if (ctx->Depth.Test) {
	    /* regular depth testing */
	    (*ctx->Driver.DepthTestPixels)( ctx, PB->count, PB->x, PB->y, PB->z, mask );
	 }

         if (ctx->RasterMask & NO_DRAW_BIT) {
            goto CleanUp;
         }

         if (ctx->RasterMask & FRONT_AND_BACK_BIT) {
            /* make a copy of the colors */
            MEMCPY( saved, PB->rgba, PB->count * 4 * sizeof(GLubyte) );
         }

         if (ctx->Color.SWLogicOpEnabled) {
            gl_logicop_rgba_pixels( ctx, PB->count, PB->x, PB->y,
                                    PB->rgba, mask);
         }
         else if (ctx->Color.BlendEnabled) {
            gl_blend_pixels( ctx, PB->count, PB->x, PB->y, PB->rgba, mask);
         }

         if (ctx->Color.SWmasking) {
            gl_mask_rgba_pixels(ctx, PB->count, PB->x, PB->y, PB->rgba, mask);
         }

         /* write pixels */
         (*ctx->Driver.WriteRGBAPixels)( ctx, PB->count, PB->x, PB->y,
                                         PB->rgba, mask );
         if (ctx->RasterMask & ALPHABUF_BIT) {
            gl_write_alpha_pixels( ctx, PB->count, PB->x, PB->y, PB->rgba, mask );
         }

         if (ctx->RasterMask & FRONT_AND_BACK_BIT) {
            /*** Also draw to back buffer ***/
            (*ctx->Driver.SetBuffer)( ctx, GL_BACK );
            if (ctx->Color.SWLogicOpEnabled) {
               gl_logicop_rgba_pixels( ctx, PB->count, PB->x, PB->y,
                                       PB->rgba, mask);
            }
            else if (ctx->Color.BlendEnabled) {
               gl_blend_pixels( ctx, PB->count, PB->x, PB->y, saved, mask );
            }
            if (ctx->Color.SWmasking) {
               gl_mask_rgba_pixels(ctx, PB->count, PB->x, PB->y, saved, mask);
            }
            (*ctx->Driver.WriteRGBAPixels)( ctx, PB->count, PB->x, PB->y,
                                            saved, mask);
            if (ctx->RasterMask & ALPHABUF_BIT) {
               ctx->Buffer->Alpha = ctx->Buffer->BackAlpha;
               gl_write_alpha_pixels( ctx, PB->count, PB->x, PB->y,
                                      saved, mask );
               ctx->Buffer->Alpha = ctx->Buffer->FrontAlpha;
            }
            (*ctx->Driver.SetBuffer)( ctx, GL_FRONT );
            /*** ALL DONE ***/
         }
      }
      else {
	 /* Same color for all pixels */

         /* Scissoring already done above */

	 if (ctx->Color.AlphaEnabled) {
	    if (gl_alpha_test( ctx, PB->count, PB->rgba, mask )==0) {
	       goto CleanUp;
	    }
	 }

	 if (ctx->Stencil.Enabled) {
	    /* first stencil test */
	    if (gl_stencil_pixels( ctx, PB->count, PB->x, PB->y, mask )==0) {
	       goto CleanUp;
	    }
	    /* depth buffering w/ stencil */
	    gl_depth_stencil_pixels( ctx, PB->count, PB->x, PB->y, PB->z, mask );
	 }
	 else if (ctx->Depth.Test) {
	    /* regular depth testing */
	    (*ctx->Driver.DepthTestPixels)( ctx, PB->count, PB->x, PB->y, PB->z, mask );
	 }

         if (ctx->RasterMask & NO_DRAW_BIT) {
            goto CleanUp;
         }

         /* write pixels */
         {
            GLubyte red, green, blue, alpha;
            red   = PB->color[RCOMP];
            green = PB->color[GCOMP];
            blue  = PB->color[BCOMP];
            alpha = PB->color[ACOMP];
	    (*ctx->Driver.Color)( ctx, red, green, blue, alpha );
         }
         (*ctx->Driver.WriteMonoRGBAPixels)( ctx, PB->count, PB->x, PB->y, mask );
         if (ctx->RasterMask & ALPHABUF_BIT) {
            gl_write_mono_alpha_pixels( ctx, PB->count, PB->x, PB->y,
                                        PB->color[ACOMP], mask );
         }

         if (ctx->RasterMask & FRONT_AND_BACK_BIT) {
            /*** Also render to back buffer ***/
            (*ctx->Driver.SetBuffer)( ctx, GL_BACK );
            (*ctx->Driver.WriteMonoRGBAPixels)( ctx, PB->count, PB->x, PB->y, mask );
            if (ctx->RasterMask & ALPHABUF_BIT) {
               ctx->Buffer->Alpha = ctx->Buffer->BackAlpha;
               gl_write_mono_alpha_pixels( ctx, PB->count, PB->x, PB->y,
                                           PB->color[ACOMP], mask );
               ctx->Buffer->Alpha = ctx->Buffer->FrontAlpha;
            }
            (*ctx->Driver.SetBuffer)( ctx, GL_FRONT );
	 }
         /*** ALL DONE ***/
      }
   }
   else {
      /* COLOR INDEX PIXELS */

      /* If we may be writting pixels with different indexes... */
      if (PB->mono && ctx->MutablePixels) {
	 /* copy index to all pixels */
         GLuint n = PB->count, indx = PB->index;
         GLuint *pbindex = PB->i;
         do {
	    *pbindex++ = indx;
            n--;
	 } while (n);
      }

      if (ctx->MutablePixels || !PB->mono) {
	 /* Pixel color index may be modified */
         GLuint *isave = (GLuint*)saved;

	 if (ctx->Fog.Enabled
             && (ctx->Hint.Fog==GL_NICEST || PB->primitive==GL_BITMAP)) {
	    gl_fog_ci_pixels( ctx, PB->count, PB->z, PB->i );
	 }

         /* Scissoring already done above */

	 if (ctx->Stencil.Enabled) {
	    /* first stencil test */
	    if (gl_stencil_pixels( ctx, PB->count, PB->x, PB->y, mask )==0) {
	       goto CleanUp;
	    }
	    /* depth buffering w/ stencil */
	    gl_depth_stencil_pixels( ctx, PB->count, PB->x, PB->y, PB->z, mask );
	 }
	 else if (ctx->Depth.Test) {
	    /* regular depth testing */
	    (*ctx->Driver.DepthTestPixels)( ctx, PB->count, PB->x, PB->y, PB->z, mask );
	 }

         if (ctx->RasterMask & NO_DRAW_BIT) {
            goto CleanUp;
         }

         if (ctx->RasterMask & FRONT_AND_BACK_BIT) {
            /* make a copy of the indexes */
            MEMCPY( isave, PB->i, PB->count * sizeof(GLuint) );
         }

         if (ctx->Color.SWLogicOpEnabled) {
            gl_logicop_ci_pixels( ctx, PB->count, PB->x, PB->y, PB->i, mask );
         }

         if (ctx->Color.SWmasking) {
            gl_mask_index_pixels( ctx, PB->count, PB->x, PB->y, PB->i, mask );
         }

         /* write pixels */
         (*ctx->Driver.WriteCI32Pixels)( ctx, PB->count, PB->x, PB->y,
                                          PB->i, mask );

         if (ctx->RasterMask & FRONT_AND_BACK_BIT) {
            /*** Also write to back buffer ***/
            (*ctx->Driver.SetBuffer)( ctx, GL_BACK );
            MEMCPY( PB->i, isave, PB->count * sizeof(GLuint) );
            if (ctx->Color.SWLogicOpEnabled) {
               gl_logicop_ci_pixels( ctx, PB->count, PB->x, PB->y, PB->i, mask );
            }
            if (ctx->Color.SWmasking) {
               gl_mask_index_pixels( ctx, PB->count, PB->x, PB->y,
                                     PB->i, mask );
            }
            (*ctx->Driver.WriteCI32Pixels)( ctx, PB->count, PB->x, PB->y,
                                             PB->i, mask );
            (*ctx->Driver.SetBuffer)( ctx, GL_FRONT );
         }

         /*** ALL DONE ***/
      }
      else {
	 /* Same color index for all pixels */

         /* Scissoring already done above */

	 if (ctx->Stencil.Enabled) {
	    /* first stencil test */
	    if (gl_stencil_pixels( ctx, PB->count, PB->x, PB->y, mask )==0) {
	       goto CleanUp;
	    }
	    /* depth buffering w/ stencil */
	    gl_depth_stencil_pixels( ctx, PB->count, PB->x, PB->y, PB->z, mask );
	 }
	 else if (ctx->Depth.Test) {
	    /* regular depth testing */
	    (*ctx->Driver.DepthTestPixels)( ctx, PB->count, PB->x, PB->y, PB->z, mask );
	 }
         
         if (ctx->RasterMask & NO_DRAW_BIT) {
            goto CleanUp;
         }

         /* write pixels */
         (*ctx->Driver.Index)( ctx, PB->index );
         (*ctx->Driver.WriteMonoCIPixels)( ctx, PB->count, PB->x, PB->y, mask );

         if (ctx->RasterMask & FRONT_AND_BACK_BIT) {
            /*** Also write to back buffer ***/
            (*ctx->Driver.SetBuffer)( ctx, GL_BACK );
            (*ctx->Driver.WriteMonoCIPixels)( ctx, PB->count, PB->x, PB->y, mask );
            (*ctx->Driver.SetBuffer)( ctx, GL_FRONT );
         }
         /*** ALL DONE ***/
      }
   }

CleanUp:
   PB->count = 0;
}


