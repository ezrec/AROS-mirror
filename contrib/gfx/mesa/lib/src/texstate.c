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
 * Revision 3.11  1998/08/21 01:50:14  brianp
 * made max_texture_coord_sets() non-static
 *
 * Revision 3.10  1998/07/18 03:35:28  brianp
 * get of GL_TEXTURE_INTERNAL_FORMAT wasn't handled
 *
 * Revision 3.9  1998/06/17 01:21:06  brianp
 * include assert.h
 *
 * Revision 3.8  1998/06/07 22:18:52  brianp
 * implemented GL_EXT_multitexture extension
 *
 * Revision 3.7  1998/04/20 23:55:13  brianp
 * applied DavidB's changes for v0.25 of 3Dfx driver
 *
 * Revision 3.6  1998/03/27 04:17:31  brianp
 * fixed G++ warnings
 *
 * Revision 3.5  1998/03/15 17:55:03  brianp
 * fixed a typo in gl_update_texture_state()
 *
 * Revision 3.4  1998/02/20 04:53:37  brianp
 * implemented GL_SGIS_multitexture
 *
 * Revision 3.3  1998/02/04 05:00:07  brianp
 * added a few casts for Amiga StormC compiler
 *
 * Revision 3.2  1998/02/03 04:27:54  brianp
 * added texture lod clamping
 *
 * Revision 3.1  1998/02/01 19:39:09  brianp
 * added GL_CLAMP_TO_EDGE texture wrap mode
 *
 * Revision 3.0  1998/01/31 21:04:38  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <assert.h>
#include "context.h"
#include "macros.h"
#include "matrix.h"
#include "texobj.h"
#include "texstate.h"
#include "texture.h"
#include "types.h"
#include "xform.h"
#endif



#ifdef SPECIALCAST
/* Needed for an Amiga compiler */
#define ENUM_TO_FLOAT(X) ((GLfloat)(GLint)(X))
#define ENUM_TO_DOUBLE(X) ((GLdouble)(GLint)(X))
#else
/* all other compilers */
#define ENUM_TO_FLOAT(X) ((GLfloat)(X))
#define ENUM_TO_DOUBLE(X) ((GLdouble)(X))
#endif



/*
 * Return max number of simultaneous textures.
 */
GLint gl_max_texture_coord_sets( const GLcontext *ctx )
{
   if (ctx->Driver.GetParameteri) {
      GLint maxCoordSets = (*ctx->Driver.GetParameteri)(ctx, DD_MAX_TEXTURE_COORD_SETS);
      assert(maxCoordSets >= 1 && maxCoordSets <= MAX_TEX_COORD_SETS);
      return maxCoordSets;
   }
   else {
      return MAX_TEX_COORD_SETS;
   }
}



/**********************************************************************/
/*                       Texture Environment                          */
/**********************************************************************/


void gl_TexEnvfv( GLcontext *ctx,
                  GLenum target, GLenum pname, const GLfloat *param )
{
   struct gl_texture_set *texSet = &ctx->Texture.Set[ctx->Texture.CurrentSet];

   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glTexEnv" );
      return;
   }

   if (target!=GL_TEXTURE_ENV) {
      gl_error( ctx, GL_INVALID_ENUM, "glTexEnv(target)" );
      return;
   }

   if (pname==GL_TEXTURE_ENV_MODE) {
      GLenum mode = (GLenum) (GLint) *param;
      switch (mode) {
	 case GL_MODULATE:
	 case GL_BLEND:
	 case GL_DECAL:
	 case GL_REPLACE:
	    /* A small optimization for drivers */ 
	    if (texSet->EnvMode == mode)
               return;
	    texSet->EnvMode = mode;
	    break;
	 default:
	    gl_error( ctx, GL_INVALID_VALUE, "glTexEnv(param)" );
	    return;
      }
   }
   else if (pname==GL_TEXTURE_ENV_COLOR) {
      texSet->EnvColor[0] = CLAMP( param[0], 0.0, 1.0 );
      texSet->EnvColor[1] = CLAMP( param[1], 0.0, 1.0 );
      texSet->EnvColor[2] = CLAMP( param[2], 0.0, 1.0 );
      texSet->EnvColor[3] = CLAMP( param[3], 0.0, 1.0 );
   }
   else if (pname==GL_TEXTURE_ENV_COORD_SET_EXT) {
      GLint p = (GLint) param[0] - GL_TEXTURE0_EXT;
      if (p >= 0 && p < gl_max_texture_coord_sets(ctx)) {
         texSet->TexCoordSet = p;
      }
      else {
         gl_error( ctx, GL_INVALID_ENUM, "glTexEnv(pname)" );
         return;
      }
   }
   else {
      gl_error( ctx, GL_INVALID_ENUM, "glTexEnv(pname)" );
      return;
   }

   /* Tell device driver about the new texture environment */
   if (ctx->Driver.TexEnv) {
      (*ctx->Driver.TexEnv)( ctx, pname, param );
   }
}





void gl_GetTexEnvfv( GLcontext *ctx,
                     GLenum target, GLenum pname, GLfloat *params )
{
   struct gl_texture_set *texSet = &ctx->Texture.Set[ctx->Texture.CurrentSet];
   if (target!=GL_TEXTURE_ENV) {
      gl_error( ctx, GL_INVALID_ENUM, "glGetTexEnvfv(target)" );
      return;
   }
   switch (pname) {
      case GL_TEXTURE_ENV_MODE:
         *params = ENUM_TO_FLOAT(texSet->EnvMode);
	 break;
      case GL_TEXTURE_ENV_COLOR:
	 COPY_4V( params, texSet->EnvColor );
	 break;
      case GL_TEXTURE_ENV_COORD_SET_EXT:
         params[0] = (GLfloat) (GL_TEXTURE0_EXT + texSet->TexCoordSet);
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glGetTexEnvfv(pname)" );
   }
}


void gl_GetTexEnviv( GLcontext *ctx,
                     GLenum target, GLenum pname, GLint *params )
{
   struct gl_texture_set *texSet = &ctx->Texture.Set[ctx->Texture.CurrentSet];
   if (target!=GL_TEXTURE_ENV) {
      gl_error( ctx, GL_INVALID_ENUM, "glGetTexEnviv(target)" );
      return;
   }
   switch (pname) {
      case GL_TEXTURE_ENV_MODE:
         *params = (GLint) texSet->EnvMode;
	 break;
      case GL_TEXTURE_ENV_COLOR:
	 params[0] = FLOAT_TO_INT( texSet->EnvColor[0] );
	 params[1] = FLOAT_TO_INT( texSet->EnvColor[1] );
	 params[2] = FLOAT_TO_INT( texSet->EnvColor[2] );
	 params[3] = FLOAT_TO_INT( texSet->EnvColor[3] );
	 break;
      case GL_TEXTURE_ENV_COORD_SET_EXT:
         params[0] = GL_TEXTURE0_EXT + texSet->TexCoordSet;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glGetTexEnviv(pname)" );
   }
}




/**********************************************************************/
/*                       Texture Parameters                           */
/**********************************************************************/


void gl_TexParameterfv( GLcontext *ctx,
                        GLenum target, GLenum pname, const GLfloat *params )
{
   struct gl_texture_set *texSet = &ctx->Texture.Set[ctx->Texture.CurrentSet];
   GLenum eparam = (GLenum) (GLint) params[0];
   struct gl_texture_object *texObj;

   switch (target) {
      case GL_TEXTURE_1D:
         texObj = texSet->Current1D;
         break;
      case GL_TEXTURE_2D:
         texObj = texSet->Current2D;
         break;
      case GL_TEXTURE_3D_EXT:
         texObj = texSet->Current3D;
         break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glTexParameter(target)" );
         return;
   }

   switch (pname) {
      case GL_TEXTURE_MIN_FILTER:
         /* A small optimization */
         if (texObj->MinFilter == eparam)
            return;

         if (eparam==GL_NEAREST || eparam==GL_LINEAR
             || eparam==GL_NEAREST_MIPMAP_NEAREST
             || eparam==GL_LINEAR_MIPMAP_NEAREST
             || eparam==GL_NEAREST_MIPMAP_LINEAR
             || eparam==GL_LINEAR_MIPMAP_LINEAR) {
            texObj->MinFilter = eparam;
            ctx->NewState |= NEW_TEXTURING;
         }
         else {
            gl_error( ctx, GL_INVALID_VALUE, "glTexParameter(param)" );
            return;
         }
         break;
      case GL_TEXTURE_MAG_FILTER:
         /* A small optimization */
         if (texObj->MagFilter == eparam)
            return;

         if (eparam==GL_NEAREST || eparam==GL_LINEAR) {
            texObj->MagFilter = eparam;
            ctx->NewState |= NEW_TEXTURING;
         }
         else {
            gl_error( ctx, GL_INVALID_VALUE, "glTexParameter(param)" );
            return;
         }
         break;
      case GL_TEXTURE_WRAP_S:
         if (eparam==GL_CLAMP || eparam==GL_REPEAT || eparam==GL_CLAMP_TO_EDGE) {
            texObj->WrapS = eparam;
         }
         else {
            gl_error( ctx, GL_INVALID_VALUE, "glTexParameter(param)" );
            return;
         }
         break;
      case GL_TEXTURE_WRAP_T:
         if (eparam==GL_CLAMP || eparam==GL_REPEAT || eparam==GL_CLAMP_TO_EDGE) {
            texObj->WrapT = eparam;
         }
         else {
            gl_error( ctx, GL_INVALID_VALUE, "glTexParameter(param)" );
            return;
         }
         break;
      case GL_TEXTURE_WRAP_R_EXT:
         if (eparam==GL_CLAMP || eparam==GL_REPEAT || eparam==GL_CLAMP_TO_EDGE) {
            texObj->WrapR = eparam;
         }
         else {
            gl_error( ctx, GL_INVALID_VALUE, "glTexParameter(param)" );
         }
         break;
      case GL_TEXTURE_BORDER_COLOR:
         texObj->BorderColor[0] = CLAMP((GLint)(params[0]*255.0), 0, 255);
         texObj->BorderColor[1] = CLAMP((GLint)(params[1]*255.0), 0, 255);
         texObj->BorderColor[2] = CLAMP((GLint)(params[2]*255.0), 0, 255);
         texObj->BorderColor[3] = CLAMP((GLint)(params[3]*255.0), 0, 255);
         break;
      case GL_TEXTURE_MIN_LOD:
         texObj->MinLod = params[0];
         break;
      case GL_TEXTURE_MAX_LOD:
         texObj->MaxLod = params[0];
         break;
      case GL_TEXTURE_BASE_LEVEL:
         if (params[0] < 0.0) {
            gl_error(ctx, GL_INVALID_VALUE, "glTexParameter(param)" );
            return;
         }
         texObj->BaseLevel = (GLint) params[0];
         break;
      case GL_TEXTURE_MAX_LEVEL:
         if (params[0] < 0.0) {
            gl_error(ctx, GL_INVALID_VALUE, "glTexParameter(param)" );
            return;
         }
         texObj->MaxLevel = (GLint) params[0];
         break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glTexParameter(pname)" );
         return;
   }

   texObj->Dirty = GL_TRUE;
   ctx->Texture.AnyDirty = GL_TRUE;
   
   if (ctx->Driver.TexParameter) {
      (*ctx->Driver.TexParameter)( ctx, target, texObj, pname, params );
   }
}



void gl_GetTexLevelParameterfv( GLcontext *ctx, GLenum target, GLint level,
                                GLenum pname, GLfloat *params )
{
   GLint iparam;
   gl_GetTexLevelParameteriv( ctx, target, level, pname, &iparam );
   *params = (GLfloat) iparam;
}



void gl_GetTexLevelParameteriv( GLcontext *ctx, GLenum target, GLint level,
                                GLenum pname, GLint *params )
{
   struct gl_texture_set *texSet = &ctx->Texture.Set[ctx->Texture.CurrentSet];
   struct gl_texture_image *tex;

   if (level<0 || level>=MAX_TEXTURE_LEVELS) {
      gl_error( ctx, GL_INVALID_VALUE, "glGetTexLevelParameter[if]v" );
      return;
   }

   switch (target) {
      case GL_TEXTURE_1D:
         tex = texSet->Current1D->Image[level];
         if (!tex)
            return;
         switch (pname) {
	    case GL_TEXTURE_WIDTH:
	       *params = tex->Width;
	       break;
	    case GL_TEXTURE_COMPONENTS:
               /* alias for case GL_TEXTURE_INTERNAL_FORMAT */
	       *params = tex->IntFormat;
	       break;
	    case GL_TEXTURE_BORDER:
	       *params = tex->Border;
	       break;
            case GL_TEXTURE_RED_SIZE:
            case GL_TEXTURE_GREEN_SIZE:
            case GL_TEXTURE_BLUE_SIZE:
            case GL_TEXTURE_ALPHA_SIZE:
            case GL_TEXTURE_INTENSITY_SIZE:
            case GL_TEXTURE_LUMINANCE_SIZE:
               *params = 8;  /* 8-bits */
               break;
            case GL_TEXTURE_INDEX_SIZE_EXT:
               *params = 8;
               break;
	    default:
	       gl_error( ctx, GL_INVALID_ENUM,
                         "glGetTexLevelParameter[if]v(pname)" );
	 }
	 break;
      case GL_TEXTURE_2D:
         tex = texSet->Current2D->Image[level];
         if (!tex)
            return;
	 switch (pname) {
	    case GL_TEXTURE_WIDTH:
	       *params = tex->Width;
	       break;
	    case GL_TEXTURE_HEIGHT:
	       *params = tex->Height;
	       break;
	    case GL_TEXTURE_COMPONENTS:
               /* alias for case GL_TEXTURE_INTERNAL_FORMAT */
	       *params = tex->IntFormat;
	       break;
	    case GL_TEXTURE_BORDER:
	       *params = tex->Border;
	       break;
            case GL_TEXTURE_RED_SIZE:
            case GL_TEXTURE_GREEN_SIZE:
            case GL_TEXTURE_BLUE_SIZE:
            case GL_TEXTURE_ALPHA_SIZE:
            case GL_TEXTURE_INTENSITY_SIZE:
            case GL_TEXTURE_LUMINANCE_SIZE:
               *params = 8;  /* 8-bits */
               break;
            case GL_TEXTURE_INDEX_SIZE_EXT:
               *params = 8;
               break;
	    default:
	       gl_error( ctx, GL_INVALID_ENUM,
                         "glGetTexLevelParameter[if]v(pname)" );
	 }
	 break;
      case GL_TEXTURE_3D_EXT:
         tex = texSet->Current3D->Image[level];
         if (!tex)
            return;
         switch (pname) {
            case GL_TEXTURE_WIDTH:
               *params = tex->Width;
               break;
            case GL_TEXTURE_HEIGHT:
               *params = tex->Height;
               break;
            case GL_TEXTURE_DEPTH_EXT:
               *params = tex->Depth;
               break;
            case GL_TEXTURE_COMPONENTS:
               /* alias for case GL_TEXTURE_INTERNAL_FORMAT */
	       *params = tex->IntFormat;
               break;
            case GL_TEXTURE_BORDER:
               *params = tex->Border;
               break;
            case GL_TEXTURE_INDEX_SIZE_EXT:
               *params = 8;
               break;
            default:
               gl_error( ctx, GL_INVALID_ENUM,
                         "glGetTexLevelParameter[if]v(pname)" );
         }
         break;
      case GL_PROXY_TEXTURE_1D:
         tex = ctx->Texture.Proxy1D->Image[level];
         if (!tex)
            return;
         switch (pname) {
	    case GL_TEXTURE_WIDTH:
	       *params = tex->Width;
	       break;
	    case GL_TEXTURE_COMPONENTS:
               /* alias for case GL_TEXTURE_INTERNAL_FORMAT */
	       *params = tex->IntFormat;
	       break;
	    case GL_TEXTURE_BORDER:
	       *params = tex->Border;
	       break;
            case GL_TEXTURE_RED_SIZE:
            case GL_TEXTURE_GREEN_SIZE:
            case GL_TEXTURE_BLUE_SIZE:
            case GL_TEXTURE_ALPHA_SIZE:
            case GL_TEXTURE_INTENSITY_SIZE:
            case GL_TEXTURE_LUMINANCE_SIZE:
               *params = 8;  /* 8-bits */
               break;
            case GL_TEXTURE_INDEX_SIZE_EXT:
               *params = 8;
               break;
	    default:
	       gl_error( ctx, GL_INVALID_ENUM,
                         "glGetTexLevelParameter[if]v(pname)" );
	 }
	 break;
      case GL_PROXY_TEXTURE_2D:
         tex = ctx->Texture.Proxy2D->Image[level];
         if (!tex)
            return;
	 switch (pname) {
	    case GL_TEXTURE_WIDTH:
	       *params = tex->Width;
	       break;
	    case GL_TEXTURE_HEIGHT:
	       *params = tex->Height;
	       break;
	    case GL_TEXTURE_COMPONENTS:
               /* alias for case GL_TEXTURE_INTERNAL_FORMAT */
	       *params = tex->IntFormat;
	       break;
	    case GL_TEXTURE_BORDER:
	       *params = tex->Border;
	       break;
            case GL_TEXTURE_RED_SIZE:
            case GL_TEXTURE_GREEN_SIZE:
            case GL_TEXTURE_BLUE_SIZE:
            case GL_TEXTURE_ALPHA_SIZE:
            case GL_TEXTURE_INTENSITY_SIZE:
            case GL_TEXTURE_LUMINANCE_SIZE:
               *params = 8;  /* 8-bits */
               break;
            case GL_TEXTURE_INDEX_SIZE_EXT:
               *params = 8;
               break;
	    default:
	       gl_error( ctx, GL_INVALID_ENUM,
                         "glGetTexLevelParameter[if]v(pname)" );
	 }
	 break;
      case GL_PROXY_TEXTURE_3D_EXT:
         tex = ctx->Texture.Proxy3D->Image[level];
         if (!tex)
            return;
	 switch (pname) {
	    case GL_TEXTURE_WIDTH:
	       *params = tex->Width;
	       break;
	    case GL_TEXTURE_HEIGHT:
	       *params = tex->Height;
	       break;
	    case GL_TEXTURE_DEPTH_EXT:
	       *params = tex->Depth;
	       break;
	    case GL_TEXTURE_COMPONENTS:
               /* alias for case GL_TEXTURE_INTERNAL_FORMAT */
	       *params = tex->IntFormat;
	       break;
	    case GL_TEXTURE_BORDER:
	       *params = tex->Border;
	       break;
            case GL_TEXTURE_RED_SIZE:
            case GL_TEXTURE_GREEN_SIZE:
            case GL_TEXTURE_BLUE_SIZE:
            case GL_TEXTURE_ALPHA_SIZE:
            case GL_TEXTURE_INTENSITY_SIZE:
            case GL_TEXTURE_LUMINANCE_SIZE:
               *params = 8;  /* 8-bits */
               break;
            case GL_TEXTURE_INDEX_SIZE_EXT:
               *params = 8;
               break;
	    default:
	       gl_error( ctx, GL_INVALID_ENUM,
                         "glGetTexLevelParameter[if]v(pname)" );
	 }
	 break;
     default:
	 gl_error(ctx, GL_INVALID_ENUM, "glGetTexLevelParameter[if]v(target)");
   }	 
}




void gl_GetTexParameterfv( GLcontext *ctx,
                           GLenum target, GLenum pname, GLfloat *params )
{
   struct gl_texture_set *texSet = &ctx->Texture.Set[ctx->Texture.CurrentSet];
   struct gl_texture_object *obj;

   switch (target) {
      case GL_TEXTURE_1D:
         obj = texSet->Current1D;
	       break;
      case GL_TEXTURE_2D:
         obj = texSet->Current2D;
	       break;
      case GL_TEXTURE_3D_EXT:
         obj = texSet->Current3D;
	       break;
	    default:
         gl_error(ctx, GL_INVALID_ENUM, "glGetTexParameterfv(target)");
         return;
	 }

         switch (pname) {
	    case GL_TEXTURE_MAG_FILTER:
	 *params = ENUM_TO_FLOAT(obj->MagFilter);
	       break;
	    case GL_TEXTURE_MIN_FILTER:
         *params = ENUM_TO_FLOAT(obj->MinFilter);
	       break;
	    case GL_TEXTURE_WRAP_S:
         *params = ENUM_TO_FLOAT(obj->WrapS);
	       break;
	    case GL_TEXTURE_WRAP_T:
         *params = ENUM_TO_FLOAT(obj->WrapT);
         break;
      case GL_TEXTURE_WRAP_R_EXT:
         *params = ENUM_TO_FLOAT(obj->WrapR);
	       break;
	    case GL_TEXTURE_BORDER_COLOR:
         params[0] = obj->BorderColor[0] / 255.0F;
         params[1] = obj->BorderColor[1] / 255.0F;
         params[2] = obj->BorderColor[2] / 255.0F;
         params[3] = obj->BorderColor[3] / 255.0F;
               break;
	    case GL_TEXTURE_RESIDENT:
               *params = ENUM_TO_FLOAT(GL_TRUE);
	       break;
	    case GL_TEXTURE_PRIORITY:
         *params = texSet->Current1D->Priority;
               break;
      case GL_TEXTURE_MIN_LOD:
         *params = obj->MinLod;
               break;
      case GL_TEXTURE_MAX_LOD:
         *params = obj->MaxLod;
               break;
      case GL_TEXTURE_BASE_LEVEL:
         *params = obj->BaseLevel;
               break;
      case GL_TEXTURE_MAX_LEVEL:
         *params = obj->MaxLevel;
               break;
            default:
               gl_error( ctx, GL_INVALID_ENUM, "glGetTexParameterfv(pname)" );
         }
}


void gl_GetTexParameteriv( GLcontext *ctx,
                           GLenum target, GLenum pname, GLint *params )
{
   struct gl_texture_set *texSet = &ctx->Texture.Set[ctx->Texture.CurrentSet];
   struct gl_texture_object *obj;

   switch (target) {
      case GL_TEXTURE_1D:
         obj = texSet->Current1D;
	       break;
      case GL_TEXTURE_2D:
         obj = texSet->Current2D;
	       break;
      case GL_TEXTURE_3D_EXT:
         obj = texSet->Current3D;
	       break;
	    default:
         gl_error(ctx, GL_INVALID_ENUM, "glGetTexParameterfv(target)");
         return;
	 }

         switch (pname) {
	    case GL_TEXTURE_MAG_FILTER:
         *params = (GLint) obj->MagFilter;
	       break;
	    case GL_TEXTURE_MIN_FILTER:
         *params = (GLint) obj->MinFilter;
	       break;
	    case GL_TEXTURE_WRAP_S:
         *params = (GLint) obj->WrapS;
	       break;
	    case GL_TEXTURE_WRAP_T:
         *params = (GLint) obj->WrapT;
         break;
      case GL_TEXTURE_WRAP_R_EXT:
         *params = (GLint) obj->WrapR;
	       break;
	    case GL_TEXTURE_BORDER_COLOR:
               {
                  GLfloat color[4];
            color[0] = obj->BorderColor[0]/255.0;
            color[1] = obj->BorderColor[1]/255.0;
            color[2] = obj->BorderColor[2]/255.0;
            color[3] = obj->BorderColor[3]/255.0;
                  params[0] = FLOAT_TO_INT( color[0] );
                  params[1] = FLOAT_TO_INT( color[1] );
                  params[2] = FLOAT_TO_INT( color[2] );
                  params[3] = FLOAT_TO_INT( color[3] );
               }
	       break;
	    case GL_TEXTURE_RESIDENT:
               *params = (GLint) GL_TRUE;
	       break;
	    case GL_TEXTURE_PRIORITY:
         *params = (GLint) obj->Priority;
	 break;
      case GL_TEXTURE_MIN_LOD:
         *params = (GLint) obj->MinLod;
               break;
      case GL_TEXTURE_MAX_LOD:
         *params = (GLint) obj->MaxLod;
               break;
      case GL_TEXTURE_BASE_LEVEL:
         *params = obj->BaseLevel;
               break;
      case GL_TEXTURE_MAX_LEVEL:
         *params = obj->MaxLevel;
               break;
            default:
               gl_error( ctx, GL_INVALID_ENUM, "glGetTexParameteriv(pname)" );
         }
}




/**********************************************************************/
/*                    Texture Coord Generation                        */
/**********************************************************************/


void gl_TexGenfv( GLcontext *ctx,
                  GLenum coord, GLenum pname, const GLfloat *params )
{
   GLuint tSet = ctx->Texture.CurrentTransformSet;
   struct gl_texture_set *texSet = &ctx->Texture.Set[tSet];
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glTexGenfv" );
      return;
   }

   switch( coord ) {
      case GL_S:
         if (pname==GL_TEXTURE_GEN_MODE) {
	    GLenum mode = (GLenum) (GLint) *params;
	    if (mode==GL_OBJECT_LINEAR ||
		mode==GL_EYE_LINEAR ||
		mode==GL_SPHERE_MAP) {
	       texSet->GenModeS = mode;
	    }
	    else {
	       gl_error( ctx, GL_INVALID_ENUM, "glTexGenfv(param)" );
	       return;
	    }
	 }
	 else if (pname==GL_OBJECT_PLANE) {
	    texSet->ObjectPlaneS[0] = params[0];
	    texSet->ObjectPlaneS[1] = params[1];
	    texSet->ObjectPlaneS[2] = params[2];
	    texSet->ObjectPlaneS[3] = params[3];
	 }
	 else if (pname==GL_EYE_PLANE) {
            /* Transform plane equation by the inverse modelview matrix */
            if (ctx->NewModelViewMatrix) {
               gl_analyze_modelview_matrix(ctx);
            }
            gl_transform_vector( texSet->EyePlaneS, params,
                                 ctx->ModelViewInv );
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glTexGenfv(pname)" );
	    return;
	 }
	 break;
      case GL_T:
         if (pname==GL_TEXTURE_GEN_MODE) {
	    GLenum mode = (GLenum) (GLint) *params;
	    if (mode==GL_OBJECT_LINEAR ||
		mode==GL_EYE_LINEAR ||
		mode==GL_SPHERE_MAP) {
	       texSet->GenModeT = mode;
	    }
	    else {
	       gl_error( ctx, GL_INVALID_ENUM, "glTexGenfv(param)" );
	       return;
	    }
	 }
	 else if (pname==GL_OBJECT_PLANE) {
	    texSet->ObjectPlaneT[0] = params[0];
	    texSet->ObjectPlaneT[1] = params[1];
	    texSet->ObjectPlaneT[2] = params[2];
	    texSet->ObjectPlaneT[3] = params[3];
	 }
	 else if (pname==GL_EYE_PLANE) {
            /* Transform plane equation by the inverse modelview matrix */
            if (ctx->NewModelViewMatrix) {
               gl_analyze_modelview_matrix(ctx);
            }
            gl_transform_vector( texSet->EyePlaneT, params,
                                 ctx->ModelViewInv );
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glTexGenfv(pname)" );
	    return;
	 }
	 break;
      case GL_R:
         if (pname==GL_TEXTURE_GEN_MODE) {
	    GLenum mode = (GLenum) (GLint) *params;
	    if (mode==GL_OBJECT_LINEAR ||
		mode==GL_EYE_LINEAR) {
	       texSet->GenModeR = mode;
	    }
	    else {
	       gl_error( ctx, GL_INVALID_ENUM, "glTexGenfv(param)" );
	       return;
	    }
	 }
	 else if (pname==GL_OBJECT_PLANE) {
	    texSet->ObjectPlaneR[0] = params[0];
	    texSet->ObjectPlaneR[1] = params[1];
	    texSet->ObjectPlaneR[2] = params[2];
	    texSet->ObjectPlaneR[3] = params[3];
	 }
	 else if (pname==GL_EYE_PLANE) {
            /* Transform plane equation by the inverse modelview matrix */
            if (ctx->NewModelViewMatrix) {
               gl_analyze_modelview_matrix(ctx);
            }
            gl_transform_vector( texSet->EyePlaneR, params,
                                 ctx->ModelViewInv );
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glTexGenfv(pname)" );
	    return;
	 }
	 break;
      case GL_Q:
         if (pname==GL_TEXTURE_GEN_MODE) {
	    GLenum mode = (GLenum) (GLint) *params;
	    if (mode==GL_OBJECT_LINEAR ||
		mode==GL_EYE_LINEAR) {
	       texSet->GenModeQ = mode;
	    }
	    else {
	       gl_error( ctx, GL_INVALID_ENUM, "glTexGenfv(param)" );
	       return;
	    }
	 }
	 else if (pname==GL_OBJECT_PLANE) {
	    texSet->ObjectPlaneQ[0] = params[0];
	    texSet->ObjectPlaneQ[1] = params[1];
	    texSet->ObjectPlaneQ[2] = params[2];
	    texSet->ObjectPlaneQ[3] = params[3];
	 }
	 else if (pname==GL_EYE_PLANE) {
            /* Transform plane equation by the inverse modelview matrix */
            if (ctx->NewModelViewMatrix) {
               gl_analyze_modelview_matrix(ctx);
            }
            gl_transform_vector( texSet->EyePlaneQ, params,
                                 ctx->ModelViewInv );
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glTexGenfv(pname)" );
	    return;
	 }
	 break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glTexGenfv(coord)" );
	 return;
   }

   ctx->NewState |= NEW_TEXTURING;
}



void gl_GetTexGendv( GLcontext *ctx,
                     GLenum coord, GLenum pname, GLdouble *params )
{
   GLuint tSet = ctx->Texture.CurrentTransformSet;
   struct gl_texture_set *texSet = &ctx->Texture.Set[tSet];
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glGetTexGendv" );
      return;
   }

   switch( coord ) {
      case GL_S:
         if (pname==GL_TEXTURE_GEN_MODE) {
            params[0] = ENUM_TO_DOUBLE(texSet->GenModeS);
	 }
	 else if (pname==GL_OBJECT_PLANE) {
            COPY_4V( params, texSet->ObjectPlaneS );
	 }
	 else if (pname==GL_EYE_PLANE) {
            COPY_4V( params, texSet->EyePlaneS );
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glGetTexGendv(pname)" );
	    return;
	 }
	 break;
      case GL_T:
         if (pname==GL_TEXTURE_GEN_MODE) {
            params[0] = ENUM_TO_DOUBLE(texSet->GenModeT);
	 }
	 else if (pname==GL_OBJECT_PLANE) {
            COPY_4V( params, texSet->ObjectPlaneT );
	 }
	 else if (pname==GL_EYE_PLANE) {
            COPY_4V( params, texSet->EyePlaneT );
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glGetTexGendv(pname)" );
	    return;
	 }
	 break;
      case GL_R:
         if (pname==GL_TEXTURE_GEN_MODE) {
            params[0] = ENUM_TO_DOUBLE(texSet->GenModeR);
	 }
	 else if (pname==GL_OBJECT_PLANE) {
            COPY_4V( params, texSet->ObjectPlaneR );
	 }
	 else if (pname==GL_EYE_PLANE) {
            COPY_4V( params, texSet->EyePlaneR );
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glGetTexGendv(pname)" );
	    return;
	 }
	 break;
      case GL_Q:
         if (pname==GL_TEXTURE_GEN_MODE) {
            params[0] = ENUM_TO_DOUBLE(texSet->GenModeQ);
	 }
	 else if (pname==GL_OBJECT_PLANE) {
            COPY_4V( params, texSet->ObjectPlaneQ );
	 }
	 else if (pname==GL_EYE_PLANE) {
            COPY_4V( params, texSet->EyePlaneQ );
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glGetTexGendv(pname)" );
	    return;
	 }
	 break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glGetTexGendv(coord)" );
	 return;
   }
}



void gl_GetTexGenfv( GLcontext *ctx,
                     GLenum coord, GLenum pname, GLfloat *params )
{
   GLuint tSet = ctx->Texture.CurrentTransformSet;
   struct gl_texture_set *texSet = &ctx->Texture.Set[tSet];
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glGetTexGenfv" );
      return;
   }

   switch( coord ) {
      case GL_S:
         if (pname==GL_TEXTURE_GEN_MODE) {
            params[0] = ENUM_TO_FLOAT(texSet->GenModeS);
	 }
	 else if (pname==GL_OBJECT_PLANE) {
            COPY_4V( params, texSet->ObjectPlaneS );
	 }
	 else if (pname==GL_EYE_PLANE) {
            COPY_4V( params, texSet->EyePlaneS );
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glGetTexGenfv(pname)" );
	    return;
	 }
	 break;
      case GL_T:
         if (pname==GL_TEXTURE_GEN_MODE) {
            params[0] = ENUM_TO_FLOAT(texSet->GenModeT);
	 }
	 else if (pname==GL_OBJECT_PLANE) {
            COPY_4V( params, texSet->ObjectPlaneT );
	 }
	 else if (pname==GL_EYE_PLANE) {
            COPY_4V( params, texSet->EyePlaneT );
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glGetTexGenfv(pname)" );
	    return;
	 }
	 break;
      case GL_R:
         if (pname==GL_TEXTURE_GEN_MODE) {
            params[0] = ENUM_TO_FLOAT(texSet->GenModeR);
	 }
	 else if (pname==GL_OBJECT_PLANE) {
            COPY_4V( params, texSet->ObjectPlaneR );
	 }
	 else if (pname==GL_EYE_PLANE) {
            COPY_4V( params, texSet->EyePlaneR );
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glGetTexGenfv(pname)" );
	    return;
	 }
	 break;
      case GL_Q:
         if (pname==GL_TEXTURE_GEN_MODE) {
            params[0] = ENUM_TO_FLOAT(texSet->GenModeQ);
	 }
	 else if (pname==GL_OBJECT_PLANE) {
            COPY_4V( params, texSet->ObjectPlaneQ );
	 }
	 else if (pname==GL_EYE_PLANE) {
            COPY_4V( params, texSet->EyePlaneQ );
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glGetTexGenfv(pname)" );
	    return;
	 }
	 break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glGetTexGenfv(coord)" );
	 return;
   }
}



void gl_GetTexGeniv( GLcontext *ctx,
                     GLenum coord, GLenum pname, GLint *params )
{
   GLuint tSet = ctx->Texture.CurrentTransformSet;
   struct gl_texture_set *texSet = &ctx->Texture.Set[tSet];
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glGetTexGeniv" );
      return;
   }

   switch( coord ) {
      case GL_S:
         if (pname==GL_TEXTURE_GEN_MODE) {
            params[0] = texSet->GenModeS;
	 }
	 else if (pname==GL_OBJECT_PLANE) {
            COPY_4V( params, (GLint) texSet->ObjectPlaneS );
	 }
	 else if (pname==GL_EYE_PLANE) {
            COPY_4V( params, (GLint) texSet->EyePlaneS );
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glGetTexGeniv(pname)" );
	    return;
	 }
	 break;
      case GL_T:
         if (pname==GL_TEXTURE_GEN_MODE) {
            params[0] = texSet->GenModeT;
	 }
	 else if (pname==GL_OBJECT_PLANE) {
            COPY_4V( params, (GLint) texSet->ObjectPlaneT );
	 }
	 else if (pname==GL_EYE_PLANE) {
            COPY_4V( params, (GLint) texSet->EyePlaneT );
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glGetTexGeniv(pname)" );
	    return;
	 }
	 break;
      case GL_R:
         if (pname==GL_TEXTURE_GEN_MODE) {
            params[0] = texSet->GenModeR;
	 }
	 else if (pname==GL_OBJECT_PLANE) {
            COPY_4V( params, (GLint) texSet->ObjectPlaneR );
	 }
	 else if (pname==GL_EYE_PLANE) {
            COPY_4V( params, (GLint) texSet->EyePlaneR );
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glGetTexGeniv(pname)" );
	    return;
	 }
	 break;
      case GL_Q:
         if (pname==GL_TEXTURE_GEN_MODE) {
            params[0] = texSet->GenModeQ;
	 }
	 else if (pname==GL_OBJECT_PLANE) {
            COPY_4V( params, (GLint) texSet->ObjectPlaneQ );
	 }
	 else if (pname==GL_EYE_PLANE) {
            COPY_4V( params, (GLint) texSet->EyePlaneQ );
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glGetTexGeniv(pname)" );
	    return;
	 }
	 break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glGetTexGeniv(coord)" );
	 return;
   }
}



/* GL_SGIS_multitexture */
void gl_SelectTextureSGIS( GLcontext *ctx, GLenum target )
{
   GLint texSet;
   GLint maxSets = gl_max_texture_coord_sets(ctx);
   if (target >= GL_TEXTURE0_SGIS && target < GL_TEXTURE0_SGIS + maxSets) {
      texSet = target - GL_TEXTURE0_SGIS;
      /* Select current texture env AND transformation set! */
      ctx->Texture.CurrentSet = texSet;
      ctx->Texture.CurrentTransformSet = texSet;
   }
   else {
      gl_error(ctx, GL_INVALID_OPERATION, "glSelectTextureSGIS(target)");
      return;
   }
}


/* GL_EXT_multitexture */
void gl_SelectTexture( GLcontext *ctx, GLenum target )
{
   GLint texSet;
   GLint maxSets = gl_max_texture_coord_sets(ctx);
   if (target >= GL_TEXTURE0_EXT && target < GL_TEXTURE0_EXT + maxSets) {
      texSet = target - GL_TEXTURE0_EXT;
   }
   else {
      gl_error(ctx, GL_INVALID_OPERATION, "glSelectTexture(target)");
      return;
   }
   ctx->Texture.CurrentSet = texSet;
}



/* GL_SGIS_multitexture / GL_EXT_multitexture */
void gl_SelectTextureCoordSet( GLcontext *ctx, GLenum target )
{
   GLint texSet;
   GLint maxSets = gl_max_texture_coord_sets(ctx);
   if (target >= GL_TEXTURE0_SGIS && target < GL_TEXTURE0_SGIS + maxSets) {
      texSet = target - GL_TEXTURE0_SGIS;
   }
   else if (target >= GL_TEXTURE0_EXT && target < GL_TEXTURE0_EXT + maxSets) {
      texSet = target - GL_TEXTURE0_EXT;
   }
   else {
      gl_error(ctx, GL_INVALID_OPERATION, "glSelectTextureCoordSet(target)");
      return;
   }
   ctx->TexCoordSet = texSet;
   ctx->Current.TexCoord = ctx->Current.MultiTexCoord[texSet];
}



/* GL_EXT_multitexture */
void gl_SelectTextureTransform( GLcontext *ctx, GLenum target )
{
   GLint texSet;
   GLint maxSets = gl_max_texture_coord_sets(ctx);
   if (target >= GL_TEXTURE0_EXT && target < GL_TEXTURE0_EXT + maxSets) {
      texSet = target - GL_TEXTURE0_EXT;
      ctx->Texture.CurrentTransformSet = texSet;
   }
   else {
      gl_error(ctx, GL_INVALID_ENUM, "glSelectTextureTransform(target)");
      return;
   }
}




/*
 * This is called by gl_update_state() if the NEW_TEXTURING bit in
 * ctx->NewState is set.
 */
void gl_update_texture_state( GLcontext *ctx )
{
   struct gl_texture_object *t;
   GLuint texSet;

   for (texSet=0; texSet<MAX_TEX_SETS; texSet++) {
      GLuint setShift = texSet * 4;
      if (ctx->Texture.Enabled & (TEXTURE0_3D << setShift))
         ctx->Texture.Set[texSet].Current = ctx->Texture.Set[texSet].Current3D;
      else if (ctx->Texture.Enabled & (TEXTURE0_2D << setShift))
         ctx->Texture.Set[texSet].Current = ctx->Texture.Set[texSet].Current2D;
      else if (ctx->Texture.Enabled & (TEXTURE0_1D << setShift))
         ctx->Texture.Set[texSet].Current = ctx->Texture.Set[texSet].Current1D;
   else
         ctx->Texture.Set[texSet].Current = NULL;
   }

   if (ctx->Texture.AnyDirty) {
      for (t = ctx->Shared->TexObjectList; t; t = t->Next) {
         if (t->Dirty) {
            gl_test_texture_object_completeness(t);
            gl_set_texture_sampler(t);
            t->Dirty = GL_FALSE;
         }
      }
      ctx->Texture.AnyDirty = GL_FALSE;
   }
}
