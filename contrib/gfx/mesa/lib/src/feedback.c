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
 * Revision 3.3  1998/02/21 01:32:49  brianp
 * fixed bug in gl_InitNames()
 *
 * Revision 3.2  1998/02/08 20:18:20  brianp
 * removed unneeded headers
 *
 * Revision 3.1  1998/02/04 00:34:29  brianp
 * fixed a few cast problems for Amiga StormC compiler
 *
 * Revision 3.0  1998/01/31 20:52:30  brianp
 * initial rev
 *
 */


#ifdef PC_HEADER
#include "all.h"
#else
#include <assert.h>
#include <stdio.h>
#include "context.h"
#include "feedback.h"
#include "macros.h"
#include "types.h"
#endif



#define FB_3D		0x01
#define FB_4D		0x02
#define FB_INDEX	0x04
#define FB_COLOR	0x08
#define FB_TEXTURE	0X10



void
gl_FeedbackBuffer( GLcontext *ctx, GLsizei size, GLenum type, GLfloat *buffer )
{
   if (ctx->RenderMode==GL_FEEDBACK || INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glFeedbackBuffer" );
      return;
   }

   if (size<0) {
      gl_error( ctx, GL_INVALID_VALUE, "glFeedbackBuffer(size<0)" );
      return;
   }
   if (!buffer) {
      gl_error( ctx, GL_INVALID_VALUE, "glFeedbackBuffer(buffer==NULL)" );
      ctx->Feedback.BufferSize = 0;
      return;
   }

   switch (type) {
      case GL_2D:
	 ctx->Feedback.Mask = 0;
         ctx->Feedback.Type = type;
	 break;
      case GL_3D:
	 ctx->Feedback.Mask = FB_3D;
         ctx->Feedback.Type = type;
	 break;
      case GL_3D_COLOR:
	 ctx->Feedback.Mask = FB_3D
                           | (ctx->Visual->RGBAflag ? FB_COLOR : FB_INDEX);
         ctx->Feedback.Type = type;
	 break;
      case GL_3D_COLOR_TEXTURE:
	 ctx->Feedback.Mask = FB_3D
                           | (ctx->Visual->RGBAflag ? FB_COLOR : FB_INDEX)
	                   | FB_TEXTURE;
         ctx->Feedback.Type = type;
	 break;
      case GL_4D_COLOR_TEXTURE:
	 ctx->Feedback.Mask = FB_3D | FB_4D
                           | (ctx->Visual->RGBAflag ? FB_COLOR : FB_INDEX)
	                   | FB_TEXTURE;
         ctx->Feedback.Type = type;
	 break;
      default:
	 ctx->Feedback.Mask = 0;
         gl_error( ctx, GL_INVALID_ENUM, "glFeedbackBuffer" );
   }

   ctx->Feedback.BufferSize = size;
   ctx->Feedback.Buffer = buffer;
   ctx->Feedback.Count = 0;
}



void gl_PassThrough( GLcontext *ctx, GLfloat token )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glPassThrough" );
      return;
   }

   if (ctx->RenderMode==GL_FEEDBACK) {
      FEEDBACK_TOKEN( ctx, (GLfloat) (GLint) GL_PASS_THROUGH_TOKEN );
      FEEDBACK_TOKEN( ctx, token );
   }
}



/*
 * Put a vertex into the feedback buffer.
 */
void gl_feedback_vertex( GLcontext *ctx,
                         GLfloat x, GLfloat y, GLfloat z, GLfloat w,
			 const GLfloat color[4], GLfloat index,
			 const GLfloat texcoord[4] )
{
   FEEDBACK_TOKEN( ctx, x );
   FEEDBACK_TOKEN( ctx, y );
   if (ctx->Feedback.Mask & FB_3D) {
      FEEDBACK_TOKEN( ctx, z );
   }
   if (ctx->Feedback.Mask & FB_4D) {
      FEEDBACK_TOKEN( ctx, w );
   }
   if (ctx->Feedback.Mask & FB_INDEX) {
      FEEDBACK_TOKEN( ctx, index );
   }
   if (ctx->Feedback.Mask & FB_COLOR) {
      FEEDBACK_TOKEN( ctx, color[0] );
      FEEDBACK_TOKEN( ctx, color[1] );
      FEEDBACK_TOKEN( ctx, color[2] );
      FEEDBACK_TOKEN( ctx, color[3] );
   }
   if (ctx->Feedback.Mask & FB_TEXTURE) {
      FEEDBACK_TOKEN( ctx, texcoord[0] );
      FEEDBACK_TOKEN( ctx, texcoord[1] );
      FEEDBACK_TOKEN( ctx, texcoord[2] );
      FEEDBACK_TOKEN( ctx, texcoord[3] );
   }
}



/**********************************************************************/
/*                              Selection                             */
/**********************************************************************/


/*
 * NOTE: this function can't be put in a display list.
 */
void gl_SelectBuffer( GLcontext *ctx, GLsizei size, GLuint *buffer )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glSelectBuffer" );
   }
   if (ctx->RenderMode==GL_SELECT) {
      gl_error( ctx, GL_INVALID_OPERATION, "glSelectBuffer" );
   }
   ctx->Select.Buffer = buffer;
   ctx->Select.BufferSize = size;
   ctx->Select.BufferCount = 0;

   ctx->Select.HitFlag = GL_FALSE;
   ctx->Select.HitMinZ = 1.0;
   ctx->Select.HitMaxZ = 0.0;
}


#define WRITE_RECORD( CTX, V )					\
	if (CTX->Select.BufferCount < CTX->Select.BufferSize) {	\
	   CTX->Select.Buffer[CTX->Select.BufferCount] = (V);		\
	}							\
	CTX->Select.BufferCount++;



void gl_update_hitflag( GLcontext *ctx, GLfloat z )
{
   ctx->Select.HitFlag = GL_TRUE;
   if (z < ctx->Select.HitMinZ) {
      ctx->Select.HitMinZ = z;
   }
   if (z > ctx->Select.HitMaxZ) {
      ctx->Select.HitMaxZ = z;
   }
}



static void write_hit_record( GLcontext *ctx )
{
   GLuint i;
   GLuint zmin, zmax, zscale = (~0u);

   /* HitMinZ and HitMaxZ are in [0,1].  Multiply these values by */
   /* 2^32-1 and round to nearest unsigned integer. */

   assert( ctx != NULL ); /* this line magically fixes a SunOS 5.x/gcc bug */
   zmin = (GLuint) ((GLfloat) zscale * ctx->Select.HitMinZ);
   zmax = (GLuint) ((GLfloat) zscale * ctx->Select.HitMaxZ);

   WRITE_RECORD( ctx, ctx->Select.NameStackDepth );
   WRITE_RECORD( ctx, zmin );
   WRITE_RECORD( ctx, zmax );
   for (i=0;i<ctx->Select.NameStackDepth;i++) {
      WRITE_RECORD( ctx, ctx->Select.NameStack[i] );
   }

   ctx->Select.Hits++;
   ctx->Select.HitFlag = GL_FALSE;
   ctx->Select.HitMinZ = 1.0;
   ctx->Select.HitMaxZ = -1.0;
}



void gl_InitNames( GLcontext *ctx )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glInitNames" );
   }
   /* Record the hit before the HitFlag is wiped out again. */
   if (ctx->RenderMode==GL_SELECT) {
      if (ctx->Select.HitFlag) {
         write_hit_record( ctx );
      }
   }
   ctx->Select.NameStackDepth = 0;
   ctx->Select.HitFlag = GL_FALSE;
   ctx->Select.HitMinZ = 1.0;
   ctx->Select.HitMaxZ = 0.0;
}



void gl_LoadName( GLcontext *ctx, GLuint name )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glLoadName" );
      return;
   }
   if (ctx->RenderMode!=GL_SELECT) {
      return;
   }
   if (ctx->Select.NameStackDepth==0) {
      gl_error( ctx, GL_INVALID_OPERATION, "glLoadName" );
      return;
   }
   if (ctx->Select.HitFlag) {
      write_hit_record( ctx );
   }
   if (ctx->Select.NameStackDepth<MAX_NAME_STACK_DEPTH) {
      ctx->Select.NameStack[ctx->Select.NameStackDepth-1] = name;
   }
   else {
      ctx->Select.NameStack[MAX_NAME_STACK_DEPTH-1] = name;
   }
}


void gl_PushName( GLcontext *ctx, GLuint name )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glPushName" );
      return;
   }
   if (ctx->RenderMode!=GL_SELECT) {
      return;
   }
   if (ctx->Select.HitFlag) {
      write_hit_record( ctx );
   }
   if (ctx->Select.NameStackDepth<MAX_NAME_STACK_DEPTH) {
      ctx->Select.NameStack[ctx->Select.NameStackDepth++] = name;
   }
   else {
      gl_error( ctx, GL_STACK_OVERFLOW, "glPushName" );
   }
}



void gl_PopName( GLcontext *ctx )
{
   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glPopName" );
      return;
   }
   if (ctx->RenderMode!=GL_SELECT) {
      return;
   }
   if (ctx->Select.HitFlag) {
      write_hit_record( ctx );
   }
   if (ctx->Select.NameStackDepth>0) {
      ctx->Select.NameStackDepth--;
   }
   else {
      gl_error( ctx, GL_STACK_UNDERFLOW, "glPopName" );
   }
}



/**********************************************************************/
/*                           Render Mode                              */
/**********************************************************************/



/*
 * NOTE: this function can't be put in a display list.
 */
GLint gl_RenderMode( GLcontext *ctx, GLenum mode )
{
   GLint result;

   if (INSIDE_BEGIN_END(ctx)) {
      gl_error( ctx, GL_INVALID_OPERATION, "glRenderMode" );
   }

   switch (ctx->RenderMode) {
      case GL_RENDER:
	 result = 0;
	 break;
      case GL_SELECT:
	 if (ctx->Select.HitFlag) {
	    write_hit_record( ctx );
	 }
	 if (ctx->Select.BufferCount > ctx->Select.BufferSize) {
	    /* overflow */
#ifdef DEBUG
            gl_warning(ctx, "Feedback buffer overflow");
#endif
	    result = -1;
	 }
	 else {
	    result = ctx->Select.Hits;
	 }
	 ctx->Select.BufferCount = 0;
	 ctx->Select.Hits = 0;
	 ctx->Select.NameStackDepth = 0;
	 break;
      case GL_FEEDBACK:
	 if (ctx->Feedback.Count > ctx->Feedback.BufferSize) {
	    /* overflow */
	    result = -1;
	 }
	 else {
	    result = ctx->Feedback.Count;
	 }
	 ctx->Feedback.Count = 0;
	 break;
      default:
	 gl_error( ctx, GL_INVALID_ENUM, "glRenderMode" );
	 return 0;
   }

   switch (mode) {
      case GL_RENDER:
         break;
      case GL_SELECT:
	 if (ctx->Select.BufferSize==0) {
	    /* haven't called glSelectBuffer yet */
	    gl_error( ctx, GL_INVALID_OPERATION, "glRenderMode" );
	 }
	 break;
      case GL_FEEDBACK:
	 if (ctx->Feedback.BufferSize==0) {
	    /* haven't called glFeedbackBuffer yet */
	    gl_error( ctx, GL_INVALID_OPERATION, "glRenderMode" );
	 }
	 break;
      default:
	 gl_error( ctx, GL_INVALID_ENUM, "glRenderMode" );
	 return 0;
   }

   ctx->RenderMode = mode;
   ctx->NewState |= NEW_ALL;

   return result;
}

