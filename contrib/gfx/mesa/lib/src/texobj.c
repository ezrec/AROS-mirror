/* $Id$ */

/*
 * Mesa 3-D graphics library
 * Version:  2.8
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
 * Revision 3.5  1998/04/20 23:55:13  brianp
 * applied DavidB's changes for v0.25 of 3Dfx driver
 *
 * Revision 3.4  1998/03/27 04:17:31  brianp
 * fixed G++ warnings
 *
 * Revision 3.3  1998/02/27 00:53:52  brianp
 * fixed a few incorrect error messages
 *
 * Revision 3.2  1998/02/20 04:53:37  brianp
 * implemented GL_SGIS_multitexture
 *
 * Revision 3.1  1998/02/03 04:27:54  brianp
 * added texture lod clamping
 *
 * Revision 3.0  1998/01/31 21:04:38  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "context.h"
#include "hash.h"
#include "macros.h"
#include "teximage.h"
#include "texobj.h"
#include "types.h"
#endif



/*
 * Allocate a new texture object and add it to the linked list of texture
 * objects.  If name>0 then also insert the new texture object into the hash
 * table.
 * Input:  shared - the shared GL state structure to contain the texture object
 *         name - integer name for the texture object
 *         dimensions - either 1, 2 or 3
 * Return:  pointer to new texture object
 */
struct gl_texture_object *
gl_alloc_texture_object( struct gl_shared_state *shared, GLuint name,
                         GLuint dimensions)
{
   struct gl_texture_object *obj;

   assert(dimensions <= 3);

   obj = (struct gl_texture_object *)
                     calloc(1,sizeof(struct gl_texture_object));
   if (obj) {
      /* init the non-zero fields */
      obj->Name = name;
      obj->Dimensions = dimensions;
      obj->WrapS = GL_REPEAT;
      obj->WrapT = GL_REPEAT;
      obj->MinFilter = GL_NEAREST_MIPMAP_LINEAR;
      obj->MagFilter = GL_LINEAR;
      obj->MinLod = -1000.0;
      obj->MaxLod = 1000.0;
      obj->BaseLevel = 0;
      obj->MaxLevel = 1000;
      obj->MinMagThresh = 0.0F;
      obj->Palette[0] = 255;
      obj->Palette[1] = 255;
      obj->Palette[2] = 255;
      obj->Palette[3] = 255;
      obj->PaletteSize = 1;
      obj->PaletteIntFormat = GL_RGBA;
      obj->PaletteFormat = GL_RGBA;

      /* insert into linked list */
      if (shared) {
         obj->Next = shared->TexObjectList;
         shared->TexObjectList = obj;
      }

      if (name > 0) {
         /* insert into hash table */
         HashInsert(shared->TexObjects, name, obj);
      }
   }
   return obj;
}


/*
 * Deallocate a texture object struct and remove it from the given
 * shared GL state.
 * Input:  shared - the shared GL state to which the object belongs
 *         t - the texture object to delete
 */
void gl_free_texture_object( struct gl_shared_state *shared,
                             struct gl_texture_object *t )
{
   struct gl_texture_object *tprev, *tcurr;

   assert(t);

   /* unlink t from the linked list */
   if (shared) {
      tprev = NULL;
      tcurr = shared->TexObjectList;
      while (tcurr) {
         if (tcurr==t) {
            if (tprev) {
               tprev->Next = t->Next;
            }
            else {
               shared->TexObjectList = t->Next;
            }
            break;
         }
         tprev = tcurr;
         tcurr = tcurr->Next;
      }
   }

   if (t->Name) {
      /* remove from hash table */
      HashRemove(shared->TexObjects, t->Name);
   }

   /* free texture image */
   {
   GLuint i;
   for (i=0;i<MAX_TEXTURE_LEVELS;i++) {
      if (t->Image[i]) {
         gl_free_texture_image( t->Image[i] );
      }
   }
   }
   /* free this object */
   free( t );
}



/*
 * Examine a texture object to determine if it is complete or not.
 * The t->Complete flag will be set to GL_TRUE or GL_FALSE accordingly.
 */
void gl_test_texture_object_completeness( struct gl_texture_object *t )
{
   t->Complete = GL_TRUE;  /* be optimistic */

   /* Always need level zero image */
   if (!t->Image[0] || !t->Image[0]->Data) {
      t->Complete = GL_FALSE;
      return;
   }

   /* Compute number of mipmap levels */
   if (t->Dimensions==1) {
      t->P = t->Image[0]->WidthLog2;
   }
   else if (t->Dimensions==2) {
      t->P = MAX2(t->Image[0]->WidthLog2, t->Image[0]->HeightLog2);
   }
   else if (t->Dimensions==3) {
      GLint max = MAX2(t->Image[0]->WidthLog2, t->Image[0]->HeightLog2);
      max = MAX2(max, (GLint)(t->Image[0]->DepthLog2));
      t->P = max;
   }

   /* Compute M (see the 1.2 spec) used during mipmapping */
   t->M = (GLfloat) (MIN2(t->MaxLevel, t->P) - t->BaseLevel);


   if (t->MinFilter!=GL_NEAREST && t->MinFilter!=GL_LINEAR) {
      /*
       * Mipmapping: determine if we have a complete set of mipmaps
       */
      GLint i;
      GLint minLevel = t->BaseLevel;
      GLint maxLevel = MIN2(t->MaxLevel, MAX_TEXTURE_LEVELS-1);

      if (minLevel > maxLevel) {
         t->Complete = GL_FALSE;
         return;
      }

      /* Test dimension-independent attributes */
      for (i = minLevel; i <= maxLevel; i++) {
         if (t->Image[i]) {
            if (!t->Image[i]->Data) {
               t->Complete = GL_FALSE;
               return;
            }
            if (t->Image[i]->Format != t->Image[0]->Format) {
               t->Complete = GL_FALSE;
               return;
            }
            if (t->Image[i]->Border != t->Image[0]->Border) {
               t->Complete = GL_FALSE;
               return;
            }
         }
      }

      /* Test things which depend on number of texture image dimensions */
      if (t->Dimensions==1) {
         /* Test 1-D mipmaps */
         GLuint width = t->Image[0]->Width2;
         for (i=1; i<MAX_TEXTURE_LEVELS; i++) {
            if (width>1) {
               width /= 2;
            }
            if (i >= minLevel && i <= maxLevel) {
            if (!t->Image[i]) {
               t->Complete = GL_FALSE;
               return;
            }
            if (!t->Image[i]->Data) {
               t->Complete = GL_FALSE;
               return;
            }
               if (t->Image[i]->Width2 != width ) {
               t->Complete = GL_FALSE;
               return;
            }
            }
            if (width==1) {
               return;  /* found smallest needed mipmap, all done! */
            }
         }
      }
      else if (t->Dimensions==2) {
         /* Test 2-D mipmaps */
         GLuint width = t->Image[0]->Width2;
         GLuint height = t->Image[0]->Height2;
         for (i=1; i<MAX_TEXTURE_LEVELS; i++) {
            if (width>1) {
               width /= 2;
            }
            if (height>1) {
               height /= 2;
            }
            if (i >= minLevel && i <= maxLevel) {
            if (!t->Image[i]) {
               t->Complete = GL_FALSE;
               return;
            }
            if (t->Image[i]->Width2 != width) {
               t->Complete = GL_FALSE;
               return;
            }
            if (t->Image[i]->Height2 != height) {
               t->Complete = GL_FALSE;
               return;
            }
            if (width==1 && height==1) {
               return;  /* found smallest needed mipmap, all done! */
            }
         }
      }
      }
      else if (t->Dimensions==3) {
         /* Test 3-D mipmaps */
         GLuint width = t->Image[0]->Width2;
         GLuint height = t->Image[0]->Height2;
         GLuint depth = t->Image[0]->Depth2;
         for (i=1; i<MAX_TEXTURE_LEVELS; i++) {
            if (width>1) {
               width /= 2;
            }
            if (height>1) {
               height /= 2;
            }
            if (depth>1) {
               depth /= 2;
            }
            if (i >= minLevel && i <= maxLevel) {
            if (!t->Image[i]) {
               t->Complete = GL_FALSE;
               return;
            }
            if (t->Image[i]->Width2 != width) {
               t->Complete = GL_FALSE;
               return;
            }
            if (t->Image[i]->Height2 != height) {
               t->Complete = GL_FALSE;
               return;
            }
            if (t->Image[i]->Depth2 != depth) {
               t->Complete = GL_FALSE;
               return;
            }
            }
            if (width==1 && height==1 && depth==1) {
               return;  /* found smallest needed mipmap, all done! */
            }
         }
      }
      else {
         /* Dimensions = ??? */
         gl_problem(NULL, "Bug in gl_test_texture_object_completeness\n");
      }
   }
}



/*
 * Execute glGenTextures
 */
void gl_GenTextures( GLcontext *ctx, GLsizei n, GLuint *texName )
{
   GLuint first;
   GLint i;

   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glGenTextures" );
      return;
   }
   if (n<0) {
      gl_error( ctx, GL_INVALID_VALUE, "glGenTextures" );
      return;
   }

   first = HashFindFreeKeyBlock(ctx->Shared->TexObjects, n);

   /* Return the texture names */
   for (i=0;i<n;i++) {
      texName[i] = first + i;
   }

   /* Allocate new, empty texture objects */
   for (i=0;i<n;i++) {
      GLuint name = first + i;
      GLuint dims = 0;
      (void) gl_alloc_texture_object(ctx->Shared, name, dims);
   }
}



/*
 * Execute glDeleteTextures
 */
void gl_DeleteTextures( GLcontext *ctx, GLsizei n, const GLuint *texName)
{
   GLint i;

   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glDeleteTextures" );
      return;
   }

   for (i=0;i<n;i++) {
      struct gl_texture_object *t;
      if (texName[i]>0) {
         t = (struct gl_texture_object *)
            HashLookup(ctx->Shared->TexObjects, texName[i]);
         if (t) {
            GLuint texSet;
            for (texSet=0; texSet<MAX_TEX_SETS; texSet++) {
               struct gl_texture_set *set = &ctx->Texture.Set[texSet];
               if (set->Current1D==t) {
               /* revert to default 1-D texture */
                  set->Current1D = ctx->Shared->Default1D[texSet];
               t->RefCount--;
               assert( t->RefCount >= 0 );
            }
               else if (set->Current2D==t) {
               /* revert to default 2-D texture */
                  set->Current2D = ctx->Shared->Default2D[texSet];
               t->RefCount--;
               assert( t->RefCount >= 0 );
            }
               else if (set->Current3D==t) {
               /* revert to default 3-D texture */
                  set->Current3D = ctx->Shared->Default3D[texSet];
               t->RefCount--;
               assert( t->RefCount >= 0 );
            }
            }

            /* tell device driver to delete texture */
            if (ctx->Driver.DeleteTexture) {
               (*ctx->Driver.DeleteTexture)( ctx, t );
            }

            if (t->RefCount==0) {
               gl_free_texture_object(ctx->Shared, t);
            }
         }
      }
   }
}



/*
 * Execute glBindTexture
 */
void gl_BindTexture( GLcontext *ctx, GLenum target, GLuint texName )
{
   struct gl_texture_set *texSet = &ctx->Texture.Set[ctx->Texture.CurrentSet];
   struct gl_texture_object *oldTexObj;
   struct gl_texture_object *newTexObj;
   struct gl_texture_object **targetPointer;
   GLuint targetDimensions;

   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glBindTexture" );
      return;
   }
   switch (target) {
      case GL_TEXTURE_1D:
         oldTexObj = texSet->Current1D;
         targetPointer = &texSet->Current1D;
         targetDimensions = 1;
         break;
      case GL_TEXTURE_2D:
         oldTexObj = texSet->Current2D;
         targetPointer = &texSet->Current2D;
         targetDimensions = 2;
         break;
      case GL_TEXTURE_3D_EXT:
         oldTexObj = texSet->Current3D;
         targetPointer = &texSet->Current3D;
         targetDimensions = 3;
         break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glBindTexture" );
                  return;
               }

   if (texName==0) {
      /* use default n-D texture */
      switch (target) {
         case GL_TEXTURE_1D:
            newTexObj = ctx->Shared->Default1D[ctx->Texture.CurrentSet];
         break;
      case GL_TEXTURE_2D:
            newTexObj = ctx->Shared->Default2D[ctx->Texture.CurrentSet];
            break;
         case GL_TEXTURE_3D_EXT:
            newTexObj = ctx->Shared->Default3D[ctx->Texture.CurrentSet];
            break;
         default:
            gl_problem(ctx, "Bad target in gl_BindTexture");
            return;
      }
         }
         else {
      newTexObj = (struct gl_texture_object *)
                             HashLookup(ctx->Shared->TexObjects, texName);
      if (newTexObj) {
         if (newTexObj->Dimensions == 0) {
            /* first time bound */
            newTexObj->Dimensions = targetDimensions;
               }
         else {
	    /* A small optimization */
	    if (newTexObj == oldTexObj)
	       return;

            if (newTexObj->Dimensions != targetDimensions) {
                  /* wrong dimensionality */
               gl_error( ctx, GL_INVALID_OPERATION, "glBindTexture" );
                  return;
               }
            }
      }
            else {
               /* create new texture object */
         newTexObj = gl_alloc_texture_object(ctx->Shared, texName,
                                             targetDimensions);
            }
         }

   /* Update the Texture.Current[123]D pointer */
   *targetPointer = newTexObj;

         /* Tidy up reference counting */
   if (*targetPointer != oldTexObj && oldTexObj->Name>0) {
      /* decrement reference count of the prev texture object */
      oldTexObj->RefCount--;
      assert( oldTexObj->RefCount >= 0 );
         }

   if (newTexObj->Name>0) {
      newTexObj->RefCount++;
         }

         /* Check if we may have to use a new triangle rasterizer */
   if (   oldTexObj->WrapS != newTexObj->WrapS
       || oldTexObj->WrapT != newTexObj->WrapT
       || oldTexObj->WrapR != newTexObj->WrapR
       || oldTexObj->MinFilter != newTexObj->MinFilter
       || oldTexObj->MagFilter != newTexObj->MagFilter
       || (oldTexObj->Image[0] && newTexObj->Image[0] && 
	  (oldTexObj->Image[0]->Format!=newTexObj->Image[0]->Format))
       || !newTexObj->Complete) {
            ctx->NewState |= NEW_RASTER_OPS;
         }

   /* If we've changed the Current[123]D texture object then update the
    * ctx->Texture.Current pointer to point to the new texture object.
    */
   if (oldTexObj == texSet->Current) {
      texSet->Current = newTexObj;
   }

   /* The current n-D texture object can never be NULL! */
   assert(*targetPointer);
 
   /* Pass BindTexture call to device driver */
   if (ctx->Driver.BindTexture) {
      (*ctx->Driver.BindTexture)( ctx, target, newTexObj );
   }
}



/*
 * Execute glPrioritizeTextures
 */
void gl_PrioritizeTextures( GLcontext *ctx,
                            GLsizei n, const GLuint *texName,
                            const GLclampf *priorities )
{
   GLint i;

   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glPrioritizeTextures" );
      return;
   }
   if (n<0) {
      gl_error( ctx, GL_INVALID_VALUE, "glPrioritizeTextures" );
      return;
   }

   for (i=0;i<n;i++) {
      struct gl_texture_object *t;
      if (texName[i]>0) {
         t = (struct gl_texture_object *)
            HashLookup(ctx->Shared->TexObjects, texName[i]);
         if (t) {
            t->Priority = CLAMP( priorities[i], 0.0F, 1.0F );
         }
      }
   }
}



/*
 * Execute glAreTexturesResident
 */
GLboolean gl_AreTexturesResident( GLcontext *ctx, GLsizei n,
                                  const GLuint *texName,
                                  GLboolean *residences )
{
   GLboolean resident = GL_TRUE;
   GLint i;

   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glAreTexturesResident" );
      return GL_FALSE;
   }
   if (n<0) {
      gl_error( ctx, GL_INVALID_VALUE, "glAreTexturesResident(n)" );
      return GL_FALSE;
   }

   for (i=0;i<n;i++) {
      struct gl_texture_object *t;
      if (texName[i]==0) {
         gl_error( ctx, GL_INVALID_VALUE, "glAreTexturesResident(textures)" );
         return GL_FALSE;
      }
      t = (struct gl_texture_object *)
         HashLookup(ctx->Shared->TexObjects, texName[i]);
      if (t) {
         /* we consider all valid texture objects to be resident */
         residences[i] = GL_TRUE;
      }
      else {
         gl_error( ctx, GL_INVALID_VALUE, "glAreTexturesResident(textures)" );
         return GL_FALSE;
      }
   }
   return resident;
}



/*
 * Execute glIsTexture
 */
GLboolean gl_IsTexture( GLcontext *ctx, GLuint texture )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glIsTextures" );
      return GL_FALSE;
   }
   if (texture>0 && HashLookup(ctx->Shared->TexObjects, texture)) {
      return GL_TRUE;
   }
   else {
      return GL_FALSE;
   }
}

