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
 * Revision 1.1  2005/01/11 14:58:32  NicJA
 * AROSMesa 3.0
 *
 * - Based on the official mesa 3 code with major patches to the amigamesa driver code to get it working.
 * - GLUT not yet started (ive left the _old_ mesaaux, mesatk and demos in for this reason)
 * - Doesnt yet work - the _db functions seem to be writing the data incorrectly, and color picking also seems broken somewhat - giving most things a blue tinge (those that are currently working)
 *
 * Revision 3.1  1998/02/20 04:53:07  brianp
 * implemented GL_SGIS_multitexture
 *
 * Revision 3.0  1998/01/31 21:06:45  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <stdlib.h>
#include "types.h"
#include "vb.h"
#endif


/*
 * Allocate and initialize a vertex buffer.
 */
struct vertex_buffer *gl_alloc_vb(void)
{
   struct vertex_buffer *vb;
   vb = (struct vertex_buffer *) calloc(sizeof(struct vertex_buffer), 1);
   if (vb) {
      /* set non-zero fields */
      GLuint i, j;
   for (i=0;i<VB_SIZE;i++) {
         vb->MaterialMask[i] = 0;
         vb->ClipMask[i] = 0;
         vb->Obj[i][3] = 1.0F;
         for (j=0;j<MAX_TEX_SETS;j++) {
            vb->MultiTexCoord[j][i][2] = 0.0F;
            vb->MultiTexCoord[j][i][3] = 1.0F;
         }
      }
      vb->TexCoord = vb->MultiTexCoord[0];
      vb->VertexSizeMask = VERTEX3_BIT;
      vb->TexCoordSize = 2;
      vb->MonoColor = GL_TRUE;
      vb->MonoMaterial = GL_TRUE;
      vb->MonoNormal = GL_TRUE;
      vb->ClipOrMask = 0;
      vb->ClipAndMask = CLIP_ALL_BITS;
   }
   return vb;
}
