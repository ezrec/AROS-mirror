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
 * Revision 3.5  1998/09/01 03:07:37  brianp
 * references to texture coords now indexed by ctx->TexCoordSet
 *
 * Revision 3.4  1998/03/28 03:58:30  brianp
 * removed unneeded break
 *
 * Revision 3.3  1998/02/20 04:50:44  brianp
 * implemented GL_SGIS_multitexture
 *
 * Revision 3.2  1998/02/08 20:18:20  brianp
 * removed unneeded headers
 *
 * Revision 3.1  1998/02/01 16:37:19  brianp
 * added GL_EXT_rescale_normal extension
 *
 * Revision 3.0  1998/01/31 20:51:23  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <string.h>
#include "context.h"
#include "enable.h"
#include "light.h"
#include "macros.h"
#include "types.h"
#include "vbfill.h"
#endif



/*
 * Perform glEnable and glDisable calls.
 */
static void gl_enable( GLcontext* ctx, GLenum cap, GLboolean state )
{
   GLuint texSet = ctx->Texture.CurrentSet;
   GLuint p;

   if (INSIDE_BEGIN_END(ctx)) {
      if (state) {
	 gl_error( ctx, GL_INVALID_OPERATION, "glEnable" );
      }
      else {
	 gl_error( ctx, GL_INVALID_OPERATION, "glDisable" );
      }
      return;
   }

   switch (cap) {
      case GL_ALPHA_TEST:
         if (ctx->Color.AlphaEnabled!=state) {
            ctx->Color.AlphaEnabled = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_AUTO_NORMAL:
	 ctx->Eval.AutoNormal = state;
	 break;
      case GL_BLEND:
         if (ctx->Color.BlendEnabled!=state) {
            ctx->Color.BlendEnabled = state;
            /* The following needed to accomodate 1.0 RGB logic op blending */
            if (ctx->Color.BlendEquation==GL_LOGIC_OP && state) {
               ctx->Color.ColorLogicOpEnabled = GL_TRUE;
            }
            else {
               ctx->Color.ColorLogicOpEnabled = GL_FALSE;
            }
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_CLIP_PLANE0:
      case GL_CLIP_PLANE1:
      case GL_CLIP_PLANE2:
      case GL_CLIP_PLANE3:
      case GL_CLIP_PLANE4:
      case GL_CLIP_PLANE5:
	 ctx->Transform.ClipEnabled[cap-GL_CLIP_PLANE0] = state;
	 /* Check if any clip planes enabled */
         ctx->Transform.AnyClip = GL_FALSE;
         for (p=0;p<MAX_CLIP_PLANES;p++) {
            if (ctx->Transform.ClipEnabled[p]) {
               ctx->Transform.AnyClip = GL_TRUE;
               break;
            }
         }
	 break;
      case GL_COLOR_MATERIAL:
         if (ctx->Light.ColorMaterialEnabled!=state) {
            ctx->Light.ColorMaterialEnabled = state;
            if (state) {
               GLfloat color[4];
               color[0] = ctx->Current.ByteColor[0] * (1.0F / 255.0F);
               color[1] = ctx->Current.ByteColor[1] * (1.0F / 255.0F);
               color[2] = ctx->Current.ByteColor[2] * (1.0F / 255.0F);
               color[3] = ctx->Current.ByteColor[3] * (1.0F / 255.0F);
               /* update material with current color */
               gl_set_material( ctx, ctx->Light.ColorMaterialBitmask, color );
            }
            gl_set_color_function(ctx);
            ctx->NewState |= NEW_LIGHTING;
         }
	 break;
      case GL_CULL_FACE:
         if (ctx->Polygon.CullFlag!=state) {
            ctx->Polygon.CullFlag = state;
            ctx->NewState |= NEW_POLYGON;
         }
	 break;
      case GL_DEPTH_TEST:
         if (state && ctx->Visual->DepthBits==0) {
            gl_warning(ctx,"glEnable(GL_DEPTH_TEST) but no depth buffer");
            return;
         }
	 if (ctx->Depth.Test!=state) {
            ctx->Depth.Test = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
         break;
      case GL_DITHER:
         if (ctx->NoDither) {
            /* MESA_NO_DITHER env var */
            state = GL_FALSE;
         }
         if (ctx->Color.DitherFlag!=state) {
	 ctx->Color.DitherFlag = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_FOG:
	 if (ctx->Fog.Enabled!=state) {
            ctx->Fog.Enabled = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_LIGHT0:
      case GL_LIGHT1:
      case GL_LIGHT2:
      case GL_LIGHT3:
      case GL_LIGHT4:
      case GL_LIGHT5:
      case GL_LIGHT6:
      case GL_LIGHT7:
         ctx->Light.Light[cap-GL_LIGHT0].Enabled = state;
         ctx->NewState |= NEW_LIGHTING;
         break;
      case GL_LIGHTING:
         if (ctx->Light.Enabled!=state) {
            ctx->Light.Enabled = state;
            ctx->NewState |= NEW_LIGHTING;
         }
         break;
      case GL_LINE_SMOOTH:
	 if (ctx->Line.SmoothFlag!=state) {
            ctx->Line.SmoothFlag = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_LINE_STIPPLE:
	 if (ctx->Line.StippleFlag!=state) {
            ctx->Line.StippleFlag = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_INDEX_LOGIC_OP:
         if (ctx->Color.IndexLogicOpEnabled!=state) {
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 ctx->Color.IndexLogicOpEnabled = state;
	 break;
      case GL_COLOR_LOGIC_OP:
         if (ctx->Color.ColorLogicOpEnabled!=state) {
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 ctx->Color.ColorLogicOpEnabled = state;
	 break;
      case GL_MAP1_COLOR_4:
	 ctx->Eval.Map1Color4 = state;
	 break;
      case GL_MAP1_INDEX:
	 ctx->Eval.Map1Index = state;
	 break;
      case GL_MAP1_NORMAL:
	 ctx->Eval.Map1Normal = state;
	 break;
      case GL_MAP1_TEXTURE_COORD_1:
	 ctx->Eval.Map1TextureCoord1 = state;
	 break;
      case GL_MAP1_TEXTURE_COORD_2:
	 ctx->Eval.Map1TextureCoord2 = state;
	 break;
      case GL_MAP1_TEXTURE_COORD_3:
	 ctx->Eval.Map1TextureCoord3 = state;
	 break;
      case GL_MAP1_TEXTURE_COORD_4:
	 ctx->Eval.Map1TextureCoord4 = state;
	 break;
      case GL_MAP1_VERTEX_3:
	 ctx->Eval.Map1Vertex3 = state;
	 break;
      case GL_MAP1_VERTEX_4:
	 ctx->Eval.Map1Vertex4 = state;
	 break;
      case GL_MAP2_COLOR_4:
	 ctx->Eval.Map2Color4 = state;
	 break;
      case GL_MAP2_INDEX:
	 ctx->Eval.Map2Index = state;
	 break;
      case GL_MAP2_NORMAL:
	 ctx->Eval.Map2Normal = state;
	 break;
      case GL_MAP2_TEXTURE_COORD_1: 
	 ctx->Eval.Map2TextureCoord1 = state;
	 break;
      case GL_MAP2_TEXTURE_COORD_2:
	 ctx->Eval.Map2TextureCoord2 = state;
	 break;
      case GL_MAP2_TEXTURE_COORD_3:
	 ctx->Eval.Map2TextureCoord3 = state;
	 break;
      case GL_MAP2_TEXTURE_COORD_4:
	 ctx->Eval.Map2TextureCoord4 = state;
	 break;
      case GL_MAP2_VERTEX_3:
	 ctx->Eval.Map2Vertex3 = state;
	 break;
      case GL_MAP2_VERTEX_4:
	 ctx->Eval.Map2Vertex4 = state;
	 break;
      case GL_NORMALIZE:
	 ctx->Transform.Normalize = state;
	 break;
      case GL_POINT_SMOOTH:
	 if (ctx->Point.SmoothFlag!=state) {
            ctx->Point.SmoothFlag = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_POLYGON_SMOOTH:
	 if (ctx->Polygon.SmoothFlag!=state) {
            ctx->Polygon.SmoothFlag = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_POLYGON_STIPPLE:
	 if (ctx->Polygon.StippleFlag!=state) {
            ctx->Polygon.StippleFlag = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_POLYGON_OFFSET_POINT:
         if (ctx->Polygon.OffsetPoint!=state) {
            ctx->Polygon.OffsetPoint = state;
            ctx->NewState |= NEW_POLYGON;
         }
         break;
      case GL_POLYGON_OFFSET_LINE:
         if (ctx->Polygon.OffsetLine!=state) {
            ctx->Polygon.OffsetLine = state;
            ctx->NewState |= NEW_POLYGON;
         }
         break;
      case GL_POLYGON_OFFSET_FILL:
      /*case GL_POLYGON_OFFSET_EXT:*/
         if (ctx->Polygon.OffsetFill!=state) {
            ctx->Polygon.OffsetFill = state;
            ctx->NewState |= NEW_POLYGON;
         }
         break;
      case GL_RESCALE_NORMAL_EXT:
         ctx->Transform.RescaleNormals = state;
         break;
      case GL_SCISSOR_TEST:
         if (ctx->Scissor.Enabled!=state) {
            ctx->Scissor.Enabled = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_SHARED_TEXTURE_PALETTE_EXT:
         ctx->Texture.SharedPalette = state;
         if (ctx->Driver.UseGlobalTexturePalette)
            (*ctx->Driver.UseGlobalTexturePalette)( ctx, state );
         break;
      case GL_STENCIL_TEST:
	 if (state && ctx->Visual->StencilBits==0) {
            gl_warning(ctx, "glEnable(GL_STENCIL_TEST) but no stencil buffer");
            return;
	 }
	 if (ctx->Stencil.Enabled!=state) {
            ctx->Stencil.Enabled = state;
            ctx->NewState |= NEW_RASTER_OPS;
         }
	 break;
      case GL_TEXTURE_1D:
         if (ctx->Visual->RGBAflag) {
            /* texturing only works in RGB mode */
            GLuint bit = TEXTURE0_1D << (ctx->Texture.CurrentSet * 4);
            if (state) {
               ctx->Texture.Enabled |= bit;
            }
            else {
               ctx->Texture.Enabled &= (~bit);
            }
            ctx->NewState |= (NEW_RASTER_OPS | NEW_TEXTURING);
         }
	 break;
      case GL_TEXTURE_2D:
         if (ctx->Visual->RGBAflag) {
            /* texturing only works in RGB mode */
            GLuint bit = TEXTURE0_2D << (ctx->Texture.CurrentSet * 4);
            if (state) {
               ctx->Texture.Enabled |= bit;
            }
            else {
               ctx->Texture.Enabled &= (~bit);
            }
            ctx->NewState |= (NEW_RASTER_OPS | NEW_TEXTURING);
         }
	 break;
      case GL_TEXTURE_3D_EXT:
         if (ctx->Visual->RGBAflag) {
            /* texturing only works in RGB mode */
            GLuint bit = TEXTURE0_3D << (ctx->Texture.CurrentSet * 4);
            if (state) {
               ctx->Texture.Enabled |= bit;
            }
            else {
               ctx->Texture.Enabled &= (~bit);
            }
            ctx->NewState |= (NEW_RASTER_OPS | NEW_TEXTURING);
         }
         break;
      case GL_TEXTURE_GEN_Q:
         if (state) {
            ctx->Texture.Set[texSet].TexGenEnabled |= Q_BIT;
         }
         else {
            ctx->Texture.Set[texSet].TexGenEnabled &= ~Q_BIT;
         }
         ctx->NewState |= NEW_TEXTURING;
	 break;
      case GL_TEXTURE_GEN_R:
         if (state) {
            ctx->Texture.Set[texSet].TexGenEnabled |= R_BIT;
         }
         else {
            ctx->Texture.Set[texSet].TexGenEnabled &= ~R_BIT;
         }
         ctx->NewState |= NEW_TEXTURING;
	 break;
      case GL_TEXTURE_GEN_S:
	 if (state) {
            ctx->Texture.Set[texSet].TexGenEnabled |= S_BIT;
         }
         else {
            ctx->Texture.Set[texSet].TexGenEnabled &= ~S_BIT;
         }
         ctx->NewState |= NEW_TEXTURING;
	 break;
      case GL_TEXTURE_GEN_T:
         if (state) {
            ctx->Texture.Set[texSet].TexGenEnabled |= T_BIT;
         }
         else {
            ctx->Texture.Set[texSet].TexGenEnabled &= ~T_BIT;
         }
         ctx->NewState |= NEW_TEXTURING;
	 break;

      /*
       * CLIENT STATE!!!
       */
      case GL_VERTEX_ARRAY:
         ctx->Array.VertexEnabled = state;
         break;
      case GL_NORMAL_ARRAY:
         ctx->Array.NormalEnabled = state;
         break;
      case GL_COLOR_ARRAY:
         ctx->Array.ColorEnabled = state;
         break;
      case GL_INDEX_ARRAY:
         ctx->Array.IndexEnabled = state;
         break;
      case GL_TEXTURE_COORD_ARRAY:
         ctx->Array.TexCoordEnabled[ctx->TexCoordSet] = state;
         break;
      case GL_EDGE_FLAG_ARRAY:
         ctx->Array.EdgeFlagEnabled = state;
         break;

      default:
	 if (state) {
	    gl_error( ctx, GL_INVALID_ENUM, "glEnable" );
	 }
	 else {
	    gl_error( ctx, GL_INVALID_ENUM, "glDisable" );
	 }
         break;
   }
}




void gl_Enable( GLcontext* ctx, GLenum cap )
{
   gl_enable( ctx, cap, GL_TRUE );
}



void gl_Disable( GLcontext* ctx, GLenum cap )
{
   gl_enable( ctx, cap, GL_FALSE );
}



GLboolean gl_IsEnabled( GLcontext* ctx, GLenum cap )
{
   switch (cap) {
      case GL_ALPHA_TEST:
         return ctx->Color.AlphaEnabled;
      case GL_AUTO_NORMAL:
	 return ctx->Eval.AutoNormal;
      case GL_BLEND:
         return ctx->Color.BlendEnabled;
      case GL_CLIP_PLANE0:
      case GL_CLIP_PLANE1:
      case GL_CLIP_PLANE2:
      case GL_CLIP_PLANE3:
      case GL_CLIP_PLANE4:
      case GL_CLIP_PLANE5:
	 return ctx->Transform.ClipEnabled[cap-GL_CLIP_PLANE0];
      case GL_COLOR_MATERIAL:
	 return ctx->Light.ColorMaterialEnabled;
      case GL_CULL_FACE:
         return ctx->Polygon.CullFlag;
      case GL_DEPTH_TEST:
         return ctx->Depth.Test;
      case GL_DITHER:
	 return ctx->Color.DitherFlag;
      case GL_FOG:
	 return ctx->Fog.Enabled;
      case GL_LIGHTING:
         return ctx->Light.Enabled;
      case GL_LIGHT0:
      case GL_LIGHT1:
      case GL_LIGHT2:
      case GL_LIGHT3:
      case GL_LIGHT4:
      case GL_LIGHT5:
      case GL_LIGHT6:
      case GL_LIGHT7:
         return ctx->Light.Light[cap-GL_LIGHT0].Enabled;
      case GL_LINE_SMOOTH:
	 return ctx->Line.SmoothFlag;
      case GL_LINE_STIPPLE:
	 return ctx->Line.StippleFlag;
      case GL_INDEX_LOGIC_OP:
	 return ctx->Color.IndexLogicOpEnabled;
      case GL_COLOR_LOGIC_OP:
	 return ctx->Color.ColorLogicOpEnabled;
      case GL_MAP1_COLOR_4:
	 return ctx->Eval.Map1Color4;
      case GL_MAP1_INDEX:
	 return ctx->Eval.Map1Index;
      case GL_MAP1_NORMAL:
	 return ctx->Eval.Map1Normal;
      case GL_MAP1_TEXTURE_COORD_1:
	 return ctx->Eval.Map1TextureCoord1;
      case GL_MAP1_TEXTURE_COORD_2:
	 return ctx->Eval.Map1TextureCoord2;
      case GL_MAP1_TEXTURE_COORD_3:
	 return ctx->Eval.Map1TextureCoord3;
      case GL_MAP1_TEXTURE_COORD_4:
	 return ctx->Eval.Map1TextureCoord4;
      case GL_MAP1_VERTEX_3:
	 return ctx->Eval.Map1Vertex3;
      case GL_MAP1_VERTEX_4:
	 return ctx->Eval.Map1Vertex4;
      case GL_MAP2_COLOR_4:
	 return ctx->Eval.Map2Color4;
      case GL_MAP2_INDEX:
	 return ctx->Eval.Map2Index;
      case GL_MAP2_NORMAL:
	 return ctx->Eval.Map2Normal;
      case GL_MAP2_TEXTURE_COORD_1: 
	 return ctx->Eval.Map2TextureCoord1;
      case GL_MAP2_TEXTURE_COORD_2:
	 return ctx->Eval.Map2TextureCoord2;
      case GL_MAP2_TEXTURE_COORD_3:
	 return ctx->Eval.Map2TextureCoord3;
      case GL_MAP2_TEXTURE_COORD_4:
	 return ctx->Eval.Map2TextureCoord4;
      case GL_MAP2_VERTEX_3:
	 return ctx->Eval.Map2Vertex3;
      case GL_MAP2_VERTEX_4:
	 return ctx->Eval.Map2Vertex4;
      case GL_NORMALIZE:
	 return ctx->Transform.Normalize;
      case GL_POINT_SMOOTH:
	 return ctx->Point.SmoothFlag;
      case GL_POLYGON_SMOOTH:
	 return ctx->Polygon.SmoothFlag;
      case GL_POLYGON_STIPPLE:
	 return ctx->Polygon.StippleFlag;
      case GL_POLYGON_OFFSET_POINT:
	 return ctx->Polygon.OffsetPoint;
      case GL_POLYGON_OFFSET_LINE:
	 return ctx->Polygon.OffsetLine;
      case GL_POLYGON_OFFSET_FILL:
      /*case GL_POLYGON_OFFSET_EXT:*/
	 return ctx->Polygon.OffsetFill;
      case GL_RESCALE_NORMAL_EXT:
         return ctx->Transform.RescaleNormals;
      case GL_SCISSOR_TEST:
	 return ctx->Scissor.Enabled;
      case GL_SHARED_TEXTURE_PALETTE_EXT:
         return ctx->Texture.SharedPalette;
      case GL_STENCIL_TEST:
	 return ctx->Stencil.Enabled;
      case GL_TEXTURE_1D:
         {
            GLuint bit = TEXTURE0_1D << (ctx->Texture.CurrentSet * 4);
            return (ctx->Texture.Enabled & bit) ? GL_TRUE : GL_FALSE;
         }
      case GL_TEXTURE_2D:
         {
            GLuint bit = TEXTURE0_1D << (ctx->Texture.CurrentSet * 4);
            return (ctx->Texture.Enabled & bit) ? GL_TRUE : GL_FALSE;
         }
      case GL_TEXTURE_3D_EXT:
         {
            GLuint bit = TEXTURE0_1D << (ctx->Texture.CurrentSet * 4);
            return (ctx->Texture.Enabled & bit) ? GL_TRUE : GL_FALSE;
         }
      case GL_TEXTURE_GEN_Q:
         {
            const struct gl_texture_set *texSet = &ctx->Texture.Set[ctx->Texture.CurrentSet];
            return (texSet->TexGenEnabled & Q_BIT) ? GL_TRUE : GL_FALSE;
         }
      case GL_TEXTURE_GEN_R:
         {
            const struct gl_texture_set *texSet = &ctx->Texture.Set[ctx->Texture.CurrentSet];
            return (texSet->TexGenEnabled & R_BIT) ? GL_TRUE : GL_FALSE;
         }
      case GL_TEXTURE_GEN_S:
         {
            const struct gl_texture_set *texSet = &ctx->Texture.Set[ctx->Texture.CurrentSet];
            return (texSet->TexGenEnabled & S_BIT) ? GL_TRUE : GL_FALSE;
         }
      case GL_TEXTURE_GEN_T:
         {
            const struct gl_texture_set *texSet = &ctx->Texture.Set[ctx->Texture.CurrentSet];
            return (texSet->TexGenEnabled & T_BIT) ? GL_TRUE : GL_FALSE;
         }

      /*
       * CLIENT STATE!!!
       */
      case GL_VERTEX_ARRAY:
         return ctx->Array.VertexEnabled;
      case GL_NORMAL_ARRAY:
         return ctx->Array.NormalEnabled;
      case GL_COLOR_ARRAY:
         return ctx->Array.ColorEnabled;
      case GL_INDEX_ARRAY:
         return ctx->Array.IndexEnabled;
      case GL_TEXTURE_COORD_ARRAY:
         return ctx->Array.TexCoordEnabled[ctx->TexCoordSet];
      case GL_EDGE_FLAG_ARRAY:
         return ctx->Array.EdgeFlagEnabled;
      default:
	 gl_error( ctx, GL_INVALID_ENUM, "glIsEnabled" );
	 return GL_FALSE;
   }
}




void gl_client_state( GLcontext *ctx, GLenum cap, GLboolean state )
{
   switch (cap) {
      case GL_VERTEX_ARRAY:
         ctx->Array.VertexEnabled = state;
         break;
      case GL_NORMAL_ARRAY:
         ctx->Array.NormalEnabled = state;
         break;
      case GL_COLOR_ARRAY:
         ctx->Array.ColorEnabled = state;
         break;
      case GL_INDEX_ARRAY:
         ctx->Array.IndexEnabled = state;
         break;
      case GL_TEXTURE_COORD_ARRAY:
         ctx->Array.TexCoordEnabled[ctx->TexCoordSet] = state;
         break;
      case GL_EDGE_FLAG_ARRAY:
         ctx->Array.EdgeFlagEnabled = state;
         break;
      default:
         gl_error( ctx, GL_INVALID_ENUM, "glEnable/DisableClientState" );
   }
}



void gl_EnableClientState( GLcontext *ctx, GLenum cap )
{
   gl_client_state( ctx, cap, GL_TRUE );
}



void gl_DisableClientState( GLcontext *ctx, GLenum cap )
{
   gl_client_state( ctx, cap, GL_FALSE );
}

