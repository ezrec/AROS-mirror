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
 * Revision 3.27  1998/08/23 22:19:30  brianp
 * added DoViewportMapping to context struct
 *
 * Revision 3.26  1998/08/21 02:41:55  brianp
 * changed PolygonStipple function pointer param type to GLuint *
 *
 * Revision 3.25  1998/08/21 01:49:29  brianp
 * added vertex array TexCoordInterleaveFactor
 *
 * Revision 3.24  1998/07/29 04:10:01  brianp
 * added extra texture state fields for glPush/PopAttrib()
 *
 * Revision 3.23  1998/07/17 03:23:32  brianp
 * added Pixel.ScaleOrBiasRGBA field
 *
 * Revision 3.22  1998/06/21 02:00:28  brianp
 * renamed clip interpolator function.  cleaned-up comments
 *
 * Revision 3.21  1998/06/07 22:18:52  brianp
 * implemented GL_EXT_multitexture extension
 *
 * Revision 3.20  1998/06/02 23:51:04  brianp
 * added CHAN_BITS and GLchan type (for the future)
 *
 * Revision 3.19  1998/04/20 23:55:13  brianp
 * applied DavidB's changes for v0.25 of 3Dfx driver
 *
 * Revision 3.18  1998/03/28 03:58:40  brianp
 * added CONST macro to fix IRIX compilation problems
 *
 * Revision 3.17  1998/03/27 04:27:05  brianp
 * changed a few evaluator members from GLuint to GLint to fix G++ warnings
 *
 * Revision 3.16  1998/03/22 16:42:05  brianp
 * added 8-bit CI->RGBA pixel mapping tables
 *
 * Revision 3.15  1998/03/15 17:55:54  brianp
 * added FogMode to context struct
 *
 * Revision 3.14  1998/03/01 20:18:14  brianp
 * removed unused context fields
 *
 * Revision 3.13  1998/02/21 01:01:29  brianp
 * added GL_MAX_TEXTURES constant
 *
 * Revision 3.12  1998/02/20 04:53:07  brianp
 * implemented GL_SGIS_multitexture
 *
 * Revision 3.11  1998/02/13 04:38:05  brianp
 * optimized blending functions called via BlendFunc function pointer
 *
 * Revision 3.10  1998/02/13 03:23:04  brianp
 * AlphaRef is now a GLubyte
 *
 * Revision 3.9  1998/02/13 03:17:02  brianp
 * added basic stereo support
 *
 * Revision 3.8  1998/02/08 20:19:41  brianp
 * ColorMask is now GLubyte[4] instead of GLuint
 *
 * Revision 3.7  1998/02/03 23:46:40  brianp
 * added 'space' parameter to clip interpolation functions
 *
 * Revision 3.6  1998/02/03 04:27:54  brianp
 * added texture lod clamping
 *
 * Revision 3.5  1998/02/02 03:09:34  brianp
 * added GL_LIGHT_MODEL_COLOR_CONTROL (separate specular color interpolation)
 *
 * Revision 3.4  1998/02/01 22:20:25  brianp
 * minor clean-up
 *
 * Revision 3.3  1998/02/01 20:05:10  brianp
 * added glDrawRangeElements()
 *
 * Revision 3.2  1998/02/01 19:39:09  brianp
 * added GL_CLAMP_TO_EDGE texture wrap mode
 *
 * Revision 3.1  1998/02/01 16:37:19  brianp
 * added GL_EXT_rescale_normal extension
 *
 * Revision 3.0  1998/01/31 21:05:58  brianp
 * initial rev
 *
 */


#ifndef TYPES_H
#define TYPES_H


#include "GL/gl.h"
#include "config.h"
#include "macros.h"
#include "fixed.h"


#define GL_MAX_TEXTURES GL_MAX_TEXTURES_SGIS


/*
 * Color channel data type:
 */
#if CHAN_BITS==8
   typedef GLubyte GLchan;
#elif CHAN_BITS==16
   typedef GLushort GLchan;
#else
   illegal number of color channel bits
#endif


/*
 * Accumulation buffer data type:
 */
#if ACCUM_BITS==8
   typedef GLbyte GLaccum;
#elif ACCUM_BITS==16
   typedef GLshort GLaccum;
#else
   illegal number of accumulation bits
#endif


/*
 * Stencil buffer data type:
 */
#if STENCIL_BITS==8
   typedef GLubyte GLstencil;
#else
   illegal number of stencil bits
#endif


/*
 * Depth buffer data type:
 */
#if DEPTH_BITS==16
   typedef GLushort GLdepth;
#elif DEPTH_BITS==32
   typedef GLint GLdepth;
#else
   illegal number of depth bits
#endif





struct HashTable;

typedef struct gl_visual GLvisual;

typedef struct gl_context GLcontext;

typedef struct gl_frame_buffer GLframebuffer;



/*
 * Point, line, triangle, quadrilateral and rectangle rasterizer functions:
 */
typedef void (*points_func)( GLcontext *ctx, GLuint first, GLuint last );

typedef void (*line_func)( GLcontext *ctx, GLuint v1, GLuint v2, GLuint pv );

typedef void (*triangle_func)( GLcontext *ctx,
                               GLuint v1, GLuint v2, GLuint v3, GLuint pv );

typedef void (*quad_func)( GLcontext *ctx, GLuint v1, GLuint v2,
                           GLuint v3, GLuint v4, GLuint pv );

typedef void (*rect_func)( GLcontext *ctx, GLint x, GLint y,
                           GLint width, GLint height );

/*
 * The auxiliary interpolation function for clipping
 */
typedef void (*clip_interp_func)( GLcontext *ctx, GLuint space, GLuint dst,
                                  GLfloat t, GLuint in, GLuint out );



/*
 * Blending function
 */
typedef void (*blend_func)( GLcontext *ctx, GLuint n, const GLubyte mask[],
                            GLubyte src[][4], CONST GLubyte dst[][4] );



/*
 * For texture sampling:
 */
struct gl_texture_object;

typedef void (*TextureSampleFunc)( const struct gl_texture_object *tObj,
                                   GLuint n,
                                   const GLfloat s[], const GLfloat t[],
                                   const GLfloat u[], const GLfloat lambda[],
                                   GLubyte rgba[][4] );



/* Generic internal image format */
struct gl_image {
	GLint Width;
	GLint Height;
	GLint Depth;		/* for 3-D texturing */
	GLint Components;	/* 1, 2, 3 or 4 */
        GLenum Format;		/* GL_COLOR_INDEX, GL_RED, GL_RGB, etc */
	GLenum Type;		/* GL_UNSIGNED_BYTE or GL_FLOAT or GL_BITMAP */
	GLvoid *Data;
	GLboolean ErrorFlag;
	GLint RefCount;
};



/* Texture image record */
struct gl_texture_image {
	GLenum Format;		/* GL_ALPHA, GL_LUMINANCE, GL_LUMINANCE_ALPHA,
				 * GL_INTENSITY, GL_RGB, GL_RGBA, or
				 * GL_COLOR_INDEX
				 */
	GLenum IntFormat;	/* Internal format as given by the user */
	GLuint Border;		/* 0 or 1 */
	GLuint Width;		/* = 2^WidthLog2 + 2*Border */
	GLuint Height;		/* = 2^HeightLog2 + 2*Border */
	GLuint Depth;		/* = 2^DepthLog2 + 2*Border */
	GLuint Width2;		/* = Width - 2*Border */
	GLuint Height2;		/* = Height - 2*Border */
	GLuint Depth2;		/* = Depth - 2*Border */
	GLuint WidthLog2;	/* = log2(Width2) */
	GLuint HeightLog2;	/* = log2(Height2) */
	GLuint DepthLog2;	/* = log2(Depth2) */
	GLuint MaxLog2;		/* = MAX(WidthLog2, HeightLog2) */
	GLubyte *Data;		/* Image data as unsigned bytes */

	/* For device driver: */
	void *DriverData;	/* Arbitrary device driver data */
};



/*
 * All gl* API functions in api*.c jump through pointers in this struct.
 */
struct gl_api_table {
   void (*Accum)( GLcontext *, GLenum, GLfloat );
   void (*AlphaFunc)( GLcontext *, GLenum, GLclampf );
   GLboolean (*AreTexturesResident)( GLcontext *, GLsizei,
                                     const GLuint *, GLboolean * );
   void (*ArrayElement)( GLcontext *, GLint );
   void (*Begin)( GLcontext *, GLenum );
   void (*BindTexture)( GLcontext *, GLenum, GLuint );
   void (*Bitmap)( GLcontext *, GLsizei, GLsizei, GLfloat, GLfloat,
		     GLfloat, GLfloat, struct gl_image *bitmap );
   void (*BlendColor)( GLcontext *, GLclampf, GLclampf, GLclampf, GLclampf);
   void (*BlendEquation)( GLcontext *, GLenum );
   void (*BlendFunc)( GLcontext *, GLenum, GLenum );
   void (*CallList)( GLcontext *, GLuint list );
   void (*CallLists)( GLcontext *, GLsizei, GLenum, const GLvoid * );
   void (*Clear)( GLcontext *, GLbitfield );
   void (*ClearAccum)( GLcontext *, GLfloat, GLfloat, GLfloat, GLfloat );
   void (*ClearColor)( GLcontext *, GLclampf, GLclampf, GLclampf, GLclampf );
   void (*ClearDepth)( GLcontext *, GLclampd );
   void (*ClearIndex)( GLcontext *, GLfloat );
   void (*ClearStencil)( GLcontext *, GLint );
   void (*ClipPlane)( GLcontext *, GLenum, const GLfloat * );
   void (*Color3f)( GLcontext *, GLfloat, GLfloat, GLfloat );
   void (*Color3fv)( GLcontext *, const GLfloat * );
   void (*Color4f)( GLcontext *, GLfloat, GLfloat, GLfloat, GLfloat );
   void (*Color4fv)( GLcontext *, const GLfloat * );
   void (*Color4ub)( GLcontext *, GLubyte, GLubyte, GLubyte, GLubyte );
   void (*Color4ubv)( GLcontext *, const GLubyte * );
   void (*ColorMask)( GLcontext *,
			GLboolean, GLboolean, GLboolean, GLboolean );
   void (*ColorMaterial)( GLcontext *, GLenum, GLenum );
   void (*ColorPointer)( GLcontext *, GLint, GLenum, GLsizei, const GLvoid * );
   void (*ColorTable)( GLcontext *, GLenum, GLenum, struct gl_image * );
   void (*ColorSubTable)( GLcontext *, GLenum, GLsizei, struct gl_image * );
   void (*CopyPixels)( GLcontext *, GLint, GLint, GLsizei, GLsizei, GLenum );
   void (*CopyTexImage1D)( GLcontext *, GLenum, GLint, GLenum,
                           GLint, GLint, GLsizei, GLint );
   void (*CopyTexImage2D)( GLcontext *, GLenum, GLint, GLenum,
                           GLint, GLint, GLsizei, GLsizei, GLint );
   void (*CopyTexSubImage1D)( GLcontext *, GLenum, GLint, GLint,
                              GLint, GLint, GLsizei );
   void (*CopyTexSubImage2D)( GLcontext *, GLenum, GLint, GLint, GLint,
                              GLint, GLint, GLsizei, GLsizei );
   void (*CopyTexSubImage3DEXT)(GLcontext *,
                                GLenum , GLint ,
                                GLint , GLint , GLint ,
                                GLint , GLint ,
                                GLsizei , GLsizei  );
   void (*CullFace)( GLcontext *, GLenum );
   void (*DeleteLists)( GLcontext *, GLuint, GLsizei );
   void (*DeleteTextures)( GLcontext *, GLsizei, const GLuint *);
   void (*DepthFunc)( GLcontext *, GLenum );
   void (*DepthMask)( GLcontext *, GLboolean );
   void (*DepthRange)( GLcontext *, GLclampd, GLclampd );
   void (*Disable)( GLcontext *, GLenum );
   void (*DisableClientState)( GLcontext *, GLenum );
   void (*DrawArrays)( GLcontext *, GLenum, GLint, GLsizei );
   void (*DrawBuffer)( GLcontext *, GLenum );
   void (*DrawElements)( GLcontext *, GLenum, GLsizei, GLenum, const GLvoid *);
   void (*DrawPixels)( GLcontext *, struct gl_image *image );
   void (*DrawRangeElements)( GLcontext *, GLenum, GLuint, GLuint, GLsizei,
                              GLenum, const GLvoid * );
   void (*EdgeFlag)( GLcontext *, GLboolean );
   void (*EdgeFlagPointer)( GLcontext *, GLsizei, const GLboolean * );
   void (*Enable)( GLcontext *, GLenum );
   void (*EnableClientState)( GLcontext *, GLenum );
   void (*End)( GLcontext * );
   void (*EndList)( GLcontext * );
   void (*EvalCoord1f)( GLcontext *, GLfloat );
   void (*EvalCoord2f)( GLcontext *, GLfloat , GLfloat );
   void (*EvalMesh1)( GLcontext *, GLenum, GLint, GLint );
   void (*EvalMesh2)( GLcontext *, GLenum, GLint, GLint, GLint, GLint );
   void (*EvalPoint1)( GLcontext *, GLint );
   void (*EvalPoint2)( GLcontext *, GLint, GLint );
   void (*FeedbackBuffer)( GLcontext *, GLsizei, GLenum, GLfloat * );
   void (*Finish)( GLcontext * );
   void (*Flush)( GLcontext * );
   void (*Fogfv)( GLcontext *, GLenum, const GLfloat * );
   void (*FrontFace)( GLcontext *, GLenum );
   void (*Frustum)( GLcontext *, GLdouble, GLdouble, GLdouble, GLdouble,
		      GLdouble, GLdouble );
   GLuint (*GenLists)( GLcontext *, GLsizei );
   void (*GenTextures)( GLcontext *, GLsizei, GLuint * );
   void (*GetBooleanv)( GLcontext *, GLenum, GLboolean * );
   void (*GetClipPlane)( GLcontext *, GLenum, GLdouble * );
   void (*GetColorTable)( GLcontext *, GLenum, GLenum, GLenum, GLvoid *);
   void (*GetColorTableParameteriv)( GLcontext *, GLenum, GLenum, GLint *);
   void (*GetDoublev)( GLcontext *, GLenum, GLdouble * );
   GLenum (*GetError)( GLcontext * );
   void (*GetFloatv)( GLcontext *, GLenum, GLfloat * );
   void (*GetIntegerv)( GLcontext *, GLenum, GLint * );
   const GLubyte* (*GetString)( GLcontext *, GLenum name );
   void (*GetLightfv)( GLcontext *, GLenum light, GLenum, GLfloat * );
   void (*GetLightiv)( GLcontext *, GLenum light, GLenum, GLint * );
   void (*GetMapdv)( GLcontext *, GLenum, GLenum, GLdouble * );
   void (*GetMapfv)( GLcontext *, GLenum, GLenum, GLfloat * );
   void (*GetMapiv)( GLcontext *, GLenum, GLenum, GLint * );
   void (*GetMaterialfv)( GLcontext *, GLenum, GLenum, GLfloat * );
   void (*GetMaterialiv)( GLcontext *, GLenum, GLenum, GLint * );
   void (*GetPixelMapfv)( GLcontext *, GLenum, GLfloat * );
   void (*GetPixelMapuiv)( GLcontext *, GLenum, GLuint * );
   void (*GetPixelMapusv)( GLcontext *, GLenum, GLushort * );
   void (*GetPointerv)( GLcontext *, GLenum, GLvoid ** );
   void (*GetPolygonStipple)( GLcontext *, GLubyte * );
   void (*PrioritizeTextures)( GLcontext *, GLsizei, const GLuint *,
                               const GLclampf * );
   void (*GetTexEnvfv)( GLcontext *, GLenum, GLenum, GLfloat * );
   void (*GetTexEnviv)( GLcontext *, GLenum, GLenum, GLint * );
   void (*GetTexGendv)( GLcontext *, GLenum coord, GLenum, GLdouble * );
   void (*GetTexGenfv)( GLcontext *, GLenum coord, GLenum, GLfloat * );
   void (*GetTexGeniv)( GLcontext *, GLenum coord, GLenum, GLint * );
   void (*GetTexImage)( GLcontext *, GLenum, GLint level, GLenum, GLenum,
                        GLvoid * );
   void (*GetTexLevelParameterfv)( GLcontext *,
				     GLenum, GLint, GLenum, GLfloat * );
   void (*GetTexLevelParameteriv)( GLcontext *,
				     GLenum, GLint, GLenum, GLint * );
   void (*GetTexParameterfv)( GLcontext *, GLenum, GLenum, GLfloat *);
   void (*GetTexParameteriv)( GLcontext *, GLenum, GLenum, GLint * );
   void (*Hint)( GLcontext *, GLenum, GLenum );
   void (*IndexMask)( GLcontext *, GLuint );
   void (*Indexf)( GLcontext *, GLfloat c );
   void (*Indexi)( GLcontext *, GLint c );
   void (*IndexPointer)( GLcontext *, GLenum, GLsizei, const GLvoid * );
   void (*InitNames)( GLcontext * );
   void (*InterleavedArrays)( GLcontext *, GLenum, GLsizei, const GLvoid * );
   GLboolean (*IsEnabled)( GLcontext *, GLenum );
   GLboolean (*IsList)( GLcontext *, GLuint );
   GLboolean (*IsTexture)( GLcontext *, GLuint );
   void (*LightModelfv)( GLcontext *, GLenum, const GLfloat * );
   void (*Lightfv)( GLcontext *, GLenum light, GLenum, const GLfloat *, GLint);
   void (*LineStipple)( GLcontext *, GLint factor, GLushort );
   void (*LineWidth)( GLcontext *, GLfloat );
   void (*ListBase)( GLcontext *, GLuint );
   void (*LoadIdentity)( GLcontext * );
   /* LoadMatrixd implemented with glLoadMatrixf */
   void (*LoadMatrixf)( GLcontext *, const GLfloat * );
   void (*LoadName)( GLcontext *, GLuint );
   void (*LogicOp)( GLcontext *, GLenum );
   void (*Map1f)( GLcontext *, GLenum, GLfloat, GLfloat, GLint, GLint,
		  const GLfloat *, GLboolean );
   void (*Map2f)( GLcontext *, GLenum, GLfloat, GLfloat, GLint, GLint,
		  GLfloat, GLfloat, GLint, GLint, const GLfloat *,
		  GLboolean );
   void (*MapGrid1f)( GLcontext *, GLint, GLfloat, GLfloat );
   void (*MapGrid2f)( GLcontext *, GLint, GLfloat, GLfloat,
			GLint, GLfloat, GLfloat );
   void (*Materialfv)( GLcontext *, GLenum, GLenum, const GLfloat * );
   void (*MatrixMode)( GLcontext *, GLenum );
   /* MultMatrixd implemented with glMultMatrixf */
   void (*MultMatrixf)( GLcontext *, const GLfloat * );
   void (*NewList)( GLcontext *, GLuint list, GLenum );
   void (*Normal3f)( GLcontext *, GLfloat, GLfloat, GLfloat );
   void (*Normal3fv)( GLcontext *, const GLfloat * );
   void (*NormalPointer)( GLcontext *, GLenum, GLsizei, const GLvoid * );
   void (*Ortho)( GLcontext *, GLdouble, GLdouble, GLdouble, GLdouble,
                  GLdouble, GLdouble );
   void (*PassThrough)( GLcontext *, GLfloat );
   void (*PixelMapfv)( GLcontext *, GLenum, GLint, const GLfloat * );
   void (*PixelStorei)( GLcontext *, GLenum, GLint );
   void (*PixelTransferf)( GLcontext *, GLenum, GLfloat );
   void (*PixelZoom)( GLcontext *, GLfloat, GLfloat );
   void (*PointParameterfvEXT)( GLcontext *, GLenum, const GLfloat * );
   void (*PointSize)( GLcontext *, GLfloat );
   void (*PolygonMode)( GLcontext *, GLenum, GLenum );
   void (*PolygonOffset)( GLcontext *, GLfloat, GLfloat );
   void (*PolygonStipple)( GLcontext *, const GLuint * );
   void (*PopAttrib)( GLcontext * );
   void (*PopClientAttrib)( GLcontext * );
   void (*PopMatrix)( GLcontext * );
   void (*PopName)( GLcontext * );
   void (*PushAttrib)( GLcontext *, GLbitfield );
   void (*PushClientAttrib)( GLcontext *, GLbitfield );
   void (*PushMatrix)( GLcontext * );
   void (*PushName)( GLcontext *, GLuint );
   void (*RasterPos4f)( GLcontext *,
                        GLfloat x, GLfloat y, GLfloat z, GLfloat w );
   void (*ReadBuffer)( GLcontext *, GLenum );
   void (*ReadPixels)( GLcontext *, GLint, GLint, GLsizei, GLsizei, GLenum,
			 GLenum, GLvoid * );
   void (*Rectf)( GLcontext *, GLfloat, GLfloat, GLfloat, GLfloat );
   GLint (*RenderMode)( GLcontext *, GLenum );
   void (*Rotatef)( GLcontext *, GLfloat, GLfloat, GLfloat, GLfloat );
   void (*Scalef)( GLcontext *, GLfloat, GLfloat, GLfloat );
   void (*Scissor)( GLcontext *, GLint, GLint, GLsizei, GLsizei);
   void (*SelectBuffer)( GLcontext *, GLsizei, GLuint * );
   void (*ShadeModel)( GLcontext *, GLenum );
   void (*StencilFunc)( GLcontext *, GLenum, GLint, GLuint );
   void (*StencilMask)( GLcontext *, GLuint );
   void (*StencilOp)( GLcontext *, GLenum, GLenum, GLenum );
   void (*TexCoord2f)( GLcontext *, GLfloat, GLfloat );
   void (*TexCoord4f)( GLcontext *, GLfloat, GLfloat, GLfloat, GLfloat );
   void (*TexCoordPointer)( GLcontext *, GLint, GLenum, GLsizei,
                            const GLvoid *);
   void (*TexEnvfv)( GLcontext *, GLenum, GLenum, const GLfloat * );
   void (*TexGenfv)( GLcontext *, GLenum coord, GLenum, const GLfloat * );
   void (*TexImage1D)( GLcontext *, GLenum, GLint, GLint, GLsizei,
                       GLint, GLenum, GLenum, struct gl_image * );
   void (*TexImage2D)( GLcontext *, GLenum, GLint, GLint, GLsizei, GLsizei,
                       GLint, GLenum, GLenum, struct gl_image * );
   void (*TexSubImage1D)( GLcontext *, GLenum, GLint, GLint, GLsizei,
                          GLenum, GLenum, struct gl_image * );
   void (*TexSubImage2D)( GLcontext *, GLenum, GLint, GLint, GLint,
                          GLsizei, GLsizei, GLenum, GLenum,
                          struct gl_image * );
   void (*TexImage3DEXT)(GLcontext *,
                         GLenum , GLint , GLint ,
                         GLsizei , GLsizei , GLsizei ,
                         GLint ,
                         GLenum , GLenum ,
                         struct gl_image * );
   void (*TexSubImage3DEXT)(GLcontext *,
                            GLenum , GLint ,
                            GLint , GLint , GLint,
                            GLsizei , GLsizei , GLsizei ,
                            GLenum , GLenum ,
                            struct gl_image * );
   void (*TexParameterfv)( GLcontext *, GLenum, GLenum, const GLfloat * );
   /* Translated implemented by Translatef */
   void (*Translatef)( GLcontext *, GLfloat, GLfloat, GLfloat );
   void (*Vertex2f)( GLcontext *, GLfloat, GLfloat );
   void (*Vertex3f)( GLcontext *, GLfloat, GLfloat, GLfloat );
   void (*Vertex4f)( GLcontext *, GLfloat, GLfloat, GLfloat, GLfloat );
   void (*Vertex3fv)( GLcontext *, const GLfloat * );
   void (*VertexPointer)( GLcontext *, GLint, GLenum, GLsizei, const GLvoid *);
   void (*Viewport)( GLcontext *, GLint, GLint, GLsizei, GLsizei );

   /* GL_MESA_window_pos extension */
   void (*WindowPos4fMESA)( GLcontext *, GLfloat, GLfloat, GLfloat, GLfloat );

   /* GL_MESA_resize_buffers extension */
   void (*ResizeBuffersMESA)( GLcontext * );

   /* GL_SGIS_multitexture / GL_EXT_multitexture */
   void (*MultiTexCoord4f)( GLcontext *, GLenum, GLfloat, GLfloat, GLfloat, GLfloat );
   void (*MultiTexCoordPointer)( GLcontext *, GLenum, GLint, GLenum, GLsizei, const GLvoid *);
   void (*InterleavedTextureCoordSets)( GLcontext *, GLint );
   void (*SelectTextureSGIS)( GLcontext *, GLenum );
   void (*SelectTexture)( GLcontext *, GLenum );
   void (*SelectTextureCoordSet)( GLcontext *, GLenum );
   void (*SelectTextureTransform)( GLcontext *, GLenum );
};



/* This has to included here. */
#include "dd.h"


/*
 * Bit flags used for updating material values.
 */
#define FRONT_AMBIENT_BIT     0x1
#define BACK_AMBIENT_BIT      0x2
#define FRONT_DIFFUSE_BIT     0x4
#define BACK_DIFFUSE_BIT      0x8
#define FRONT_SPECULAR_BIT   0x10
#define BACK_SPECULAR_BIT    0x20
#define FRONT_EMISSION_BIT   0x40
#define BACK_EMISSION_BIT    0x80
#define FRONT_SHININESS_BIT 0x100
#define BACK_SHININESS_BIT  0x200
#define FRONT_INDEXES_BIT   0x400
#define BACK_INDEXES_BIT    0x800

#define FRONT_MATERIAL_BITS	(FRONT_EMISSION_BIT | FRONT_AMBIENT_BIT | \
				 FRONT_DIFFUSE_BIT | FRONT_SPECULAR_BIT | \
				 FRONT_SHININESS_BIT | FRONT_INDEXES_BIT)

#define BACK_MATERIAL_BITS	(BACK_EMISSION_BIT | BACK_AMBIENT_BIT | \
				 BACK_DIFFUSE_BIT | BACK_SPECULAR_BIT | \
				 BACK_SHININESS_BIT | BACK_INDEXES_BIT)

#define ALL_MATERIAL_BITS	(FRONT_MATERIAL_BITS | BACK_MATERIAL_BITS)



/*
 * Specular exponent and material shininess lookup table sizes:
 */
#define EXP_TABLE_SIZE 512
#define SHINE_TABLE_SIZE 200

struct gl_light {
	GLfloat Ambient[4];		/* ambient color */
	GLfloat Diffuse[4];		/* diffuse color */
	GLfloat Specular[4];		/* specular color */
	GLfloat Position[4];		/* position in eye coordinates */
	GLfloat Direction[4];		/* spotlight dir in eye coordinates */
	GLfloat SpotExponent;
	GLfloat SpotCutoff;		/* in degress */
        GLfloat CosCutoff;		/* = cos(SpotCutoff) */
	GLfloat ConstantAttenuation;
	GLfloat LinearAttenuation;
	GLfloat QuadraticAttenuation;
	GLboolean Enabled;		/* On/off flag */

	struct gl_light *NextEnabled;	/* Ptr to next enabled light or NULL */

	/* Derived fields */
	GLfloat VP_inf_norm[3];		/* Norm direction to infinite light */
	GLfloat h_inf_norm[3];		/* Norm( VP_inf_norm + <0,0,1> ) */
        GLfloat NormDirection[3];	/* normalized spotlight direction */
        GLfloat SpotExpTable[EXP_TABLE_SIZE][2];  /* to replace a pow() call */
	GLfloat MatAmbient[2][3];	/* material ambient * light ambient */
	GLfloat MatDiffuse[2][3];	/* material diffuse * light diffuse */
	GLfloat MatSpecular[2][3];	/* material spec * light specular */
	GLfloat dli;			/* CI diffuse light intensity */
	GLfloat sli;			/* CI specular light intensity */
};


struct gl_lightmodel {
	GLfloat Ambient[4];		/* ambient color */
	GLboolean LocalViewer;		/* Local (or infinite) view point? */
	GLboolean TwoSide;		/* Two (or one) sided lighting? */
	GLenum ColorControl;		/* either GL_SINGLE_COLOR */
					/* or GL_SEPARATE_SPECULAR_COLOR */
};


struct gl_material {
	GLfloat Ambient[4];
	GLfloat Diffuse[4];
	GLfloat Specular[4];
	GLfloat Emission[4];
	GLfloat Shininess;
	GLfloat AmbientIndex;	/* for color index lighting */
	GLfloat DiffuseIndex;	/* for color index lighting */
	GLfloat SpecularIndex;	/* for color index lighting */
        GLfloat ShineTable[SHINE_TABLE_SIZE];  /* to replace a pow() call */
};



/*
 * Attribute structures:
 *    We define a struct for each attribute group to make pushing and
 *    popping attributes easy.  Also it's a good organization.
 */


struct gl_accum_attrib {
	GLfloat ClearColor[4];	/* Accumulation buffer clear color */
};


struct gl_colorbuffer_attrib {
	GLuint ClearIndex;		/* Index to use for glClear */
	GLfloat ClearColor[4];		/* Color to use for glClear */

	GLuint IndexMask;		/* Color index write mask */
	GLubyte ColorMask[4];		/* Each flag is 0xff or 0x0 */
        GLboolean SWmasking;		/* Do color/CI masking in software? */

	GLenum DrawBuffer;		/* Which buffer to draw into */

	/* alpha testing */
	GLboolean AlphaEnabled;		/* Alpha test enabled flag */
	GLenum AlphaFunc;		/* Alpha test function */
	GLubyte AlphaRef;		/* Alpha ref value scaled to [0,255] */

	/* blending */
	GLboolean BlendEnabled;		/* Blending enabled flag */
	GLenum BlendSrc;		/* Blending source operator */
	GLenum BlendDst;		/* Blending destination operator */
	GLenum BlendEquation;
	GLfloat BlendColor[4];
	blend_func BlendFunc;		/* Points to C blending function */

	/* logic op */
	GLenum LogicOp;			/* Logic operator */
	GLboolean IndexLogicOpEnabled;	/* Color index logic op enabled flag */
	GLboolean ColorLogicOpEnabled;	/* RGBA logic op enabled flag */
	GLboolean SWLogicOpEnabled;	/* Do logic ops in software? */

	GLboolean DitherFlag;		/* Dither enable flag */
};


struct gl_current_attrib {
	GLubyte ByteColor[4];		/* Current RGBA color */
	GLuint Index;			/* Current color index */
	GLfloat Normal[3];		/* Current normal vector */
   GLfloat *TexCoord;				/* points into MultiTexCoord */
   GLfloat MultiTexCoord[MAX_TEX_SETS][4];	/* Current texture coords */
	GLfloat RasterPos[4];		/* Current raster position */
	GLfloat RasterDistance;		/* Current raster distance */
	GLfloat RasterColor[4];		/* Current raster color */
	GLuint RasterIndex;		/* Current raster index */
   GLfloat *RasterTexCoord;			/* Current raster texcoord*/
   GLfloat RasterMultiTexCoord[MAX_TEX_SETS][4];/* Current raster texcoords */
   GLboolean RasterPosValid;			/* Raster po valid flag */
	GLboolean EdgeFlag;		/* Current edge flag */
};


struct gl_depthbuffer_attrib {
	GLenum Func;		/* Function for depth buffer compare */
	GLfloat Clear;		/* Value to clear depth buffer to */
	GLboolean Test;		/* Depth buffering enabled flag */
	GLboolean Mask;		/* Depth buffer writable? */
};


struct gl_enable_attrib {
	GLboolean AlphaTest;
	GLboolean AutoNormal;
	GLboolean Blend;
	GLboolean ClipPlane[MAX_CLIP_PLANES];
	GLboolean ColorMaterial;
	GLboolean CullFace;
	GLboolean DepthTest;
	GLboolean Dither;
	GLboolean Fog;
	GLboolean Light[MAX_LIGHTS];
	GLboolean Lighting;
	GLboolean LineSmooth;
	GLboolean LineStipple;
	GLboolean IndexLogicOp;
	GLboolean ColorLogicOp;
	GLboolean Map1Color4;
	GLboolean Map1Index;
	GLboolean Map1Normal;
	GLboolean Map1TextureCoord1;
	GLboolean Map1TextureCoord2;
	GLboolean Map1TextureCoord3;
	GLboolean Map1TextureCoord4;
	GLboolean Map1Vertex3;
	GLboolean Map1Vertex4;
	GLboolean Map2Color4;
	GLboolean Map2Index;
	GLboolean Map2Normal;
	GLboolean Map2TextureCoord1;
	GLboolean Map2TextureCoord2;
	GLboolean Map2TextureCoord3;
	GLboolean Map2TextureCoord4;
	GLboolean Map2Vertex3;
	GLboolean Map2Vertex4;
	GLboolean Normalize;
	GLboolean PointSmooth;
	GLboolean PolygonOffsetPoint;
	GLboolean PolygonOffsetLine;
	GLboolean PolygonOffsetFill;
	GLboolean PolygonSmooth;
	GLboolean PolygonStipple;
	GLboolean RescaleNormals;
	GLboolean Scissor;
	GLboolean Stencil;
	GLuint Texture;
	GLuint TexGen[MAX_TEX_SETS];
};


struct gl_eval_attrib {
	/* Enable bits */
	GLboolean Map1Color4;
	GLboolean Map1Index;
	GLboolean Map1Normal;
	GLboolean Map1TextureCoord1;
	GLboolean Map1TextureCoord2;
	GLboolean Map1TextureCoord3;
	GLboolean Map1TextureCoord4;
	GLboolean Map1Vertex3;
	GLboolean Map1Vertex4;
	GLboolean Map2Color4;
	GLboolean Map2Index;
	GLboolean Map2Normal;
	GLboolean Map2TextureCoord1;
	GLboolean Map2TextureCoord2;
	GLboolean Map2TextureCoord3;
	GLboolean Map2TextureCoord4;
	GLboolean Map2Vertex3;
	GLboolean Map2Vertex4;
	GLboolean AutoNormal;
	/* Map Grid endpoints and divisions */
	GLint MapGrid1un;
	GLfloat MapGrid1u1, MapGrid1u2;
	GLint MapGrid2un, MapGrid2vn;
	GLfloat MapGrid2u1, MapGrid2u2;
	GLfloat MapGrid2v1, MapGrid2v2;
};


struct gl_fog_attrib {
	GLboolean Enabled;		/* Fog enabled flag */
	GLfloat Color[4];		/* Fog color */
	GLfloat Density;		/* Density >= 0.0 */
	GLfloat Start;			/* Start distance in eye coords */
	GLfloat End;			/* End distance in eye coords */
	GLfloat Index;			/* Fog index */
	GLenum Mode;			/* Fog mode */
};


struct gl_hint_attrib {
	/* always one of GL_FASTEST, GL_NICEST, or GL_DONT_CARE */
	GLenum PerspectiveCorrection;
	GLenum PointSmooth;
	GLenum LineSmooth;
	GLenum PolygonSmooth;
	GLenum Fog;
};


struct gl_light_attrib {
   struct gl_light Light[MAX_LIGHTS];	/* Array of lights */
   struct gl_lightmodel Model;		/* Lighting model */
   struct gl_material Material[2];	/* Material 0=front, 1=back */
   GLboolean Enabled;			/* Lighting enabled flag */
   GLenum ShadeModel;			/* GL_FLAT or GL_SMOOTH */
   GLenum ColorMaterialFace;		/* GL_FRONT, BACK or FRONT_AND_BACK */
   GLenum ColorMaterialMode;		/* GL_AMBIENT, GL_DIFFUSE, etc */
   GLuint ColorMaterialBitmask;		/* bitmask formed from Face and Mode */
   GLboolean ColorMaterialEnabled;

   /* Derived for optimizations: */
   struct gl_light *FirstEnabled;	/* Ptr to 1st enabled light */
   GLboolean Fast;			/* Use fast shader? */
   GLfloat BaseColor[2][4];
};


struct gl_line_attrib {
	GLboolean SmoothFlag;		/* GL_LINE_SMOOTH enabled? */
	GLboolean StippleFlag;		/* GL_LINE_STIPPLE enabled? */
	GLushort StipplePattern;	/* Stipple pattern */
	GLint StippleFactor;		/* Stipple repeat factor */
	GLfloat Width;			/* Line width */
};


struct gl_list_attrib {
	GLuint ListBase;
};


struct gl_pixel_attrib {
	GLenum ReadBuffer;		/* src buffer for glRead/CopyPixels */
	GLfloat RedBias, RedScale;
	GLfloat GreenBias, GreenScale;
	GLfloat BlueBias, BlueScale;
	GLfloat AlphaBias, AlphaScale;
	GLboolean ScaleOrBiasRGBA;
	GLfloat DepthBias, DepthScale;
	GLint IndexShift, IndexOffset;
	GLboolean MapColorFlag;
	GLboolean MapStencilFlag;
	GLfloat ZoomX;			/* Pixel zoom X factor */
	GLfloat ZoomY;			/* Pixel zoom Y factor */
	GLint MapStoSsize;			/* Size of each pixel map */
	GLint MapItoIsize;
	GLint MapItoRsize;
	GLint MapItoGsize;
	GLint MapItoBsize;
	GLint MapItoAsize;
	GLint MapRtoRsize;
	GLint MapGtoGsize;
	GLint MapBtoBsize;
	GLint MapAtoAsize;
	GLint MapStoS[MAX_PIXEL_MAP_TABLE];	/* Pixel map tables */
	GLint MapItoI[MAX_PIXEL_MAP_TABLE];
	GLfloat MapItoR[MAX_PIXEL_MAP_TABLE];
	GLfloat MapItoG[MAX_PIXEL_MAP_TABLE];
	GLfloat MapItoB[MAX_PIXEL_MAP_TABLE];
	GLfloat MapItoA[MAX_PIXEL_MAP_TABLE];
	GLubyte MapItoR8[MAX_PIXEL_MAP_TABLE];  /* converted to 8-bit color */
	GLubyte MapItoG8[MAX_PIXEL_MAP_TABLE];
	GLubyte MapItoB8[MAX_PIXEL_MAP_TABLE];
	GLubyte MapItoA8[MAX_PIXEL_MAP_TABLE];
	GLfloat MapRtoR[MAX_PIXEL_MAP_TABLE];
	GLfloat MapGtoG[MAX_PIXEL_MAP_TABLE];
	GLfloat MapBtoB[MAX_PIXEL_MAP_TABLE];
	GLfloat MapAtoA[MAX_PIXEL_MAP_TABLE];
};


struct gl_point_attrib {
	GLboolean SmoothFlag;	/* True if GL_POINT_SMOOTH is enabled */
	GLfloat Size;		/* Point size */
	GLfloat Params[3];	/* Point Parameters EXT distance atenuation 
				   factors by default = {1,0,0} */ 
	GLfloat MinSize;	/* Default 0.0 always >=0 */
	GLfloat MaxSize;	/* Default MAX_POINT_SIZE */
	GLfloat Threshold;	/* Default 1.0 */ 
};


struct gl_polygon_attrib {
	GLenum FrontFace;	/* Either GL_CW or GL_CCW */
	GLenum FrontMode;	/* Either GL_POINT, GL_LINE or GL_FILL */
	GLenum BackMode;	/* Either GL_POINT, GL_LINE or GL_FILL */
	GLboolean Unfilled;	/* True if back or front mode is not GL_FILL */
	GLboolean CullFlag;	/* Culling on/off flag */
	GLenum CullFaceMode;	/* Culling mode GL_FRONT or GL_BACK */
        GLuint CullBits;	/* Used for cull testing */
	GLboolean SmoothFlag;	/* True if GL_POLYGON_SMOOTH is enabled */
	GLboolean StippleFlag;	/* True if GL_POLYGON_STIPPLE is enabled */
        GLfloat OffsetFactor;	/* Polygon offset factor */
        GLfloat OffsetUnits;	/* Polygon offset units */
        GLboolean OffsetPoint;	/* Offset in GL_POINT mode? */
        GLboolean OffsetLine;	/* Offset in GL_LINE mode? */
        GLboolean OffsetFill;	/* Offset in GL_FILL mode? */
        GLboolean OffsetAny;	/* OR of OffsetPoint, OffsetLine, OffsetFill */
};


struct gl_scissor_attrib {
	GLboolean	Enabled;		/* Scissor test enabled? */
	GLint		X, Y;			/* Lower left corner of box */
	GLsizei		Width, Height;		/* Size of box */
};


struct gl_stencil_attrib {
	GLboolean	Enabled;	/* Enabled flag */
	GLenum		Function;	/* Stencil function */
	GLenum		FailFunc;	/* Fail function */
	GLenum		ZPassFunc;	/* Depth buffer pass function */
	GLenum		ZFailFunc;	/* Depth buffer fail function */
	GLstencil	Ref;		/* Reference value */
	GLstencil	ValueMask;	/* Value mask */
	GLstencil	Clear;		/* Clear value */
	GLstencil	WriteMask;	/* Write mask */
};


/* TexGenEnabled flags */
#define S_BIT 1
#define T_BIT 2
#define R_BIT 4
#define Q_BIT 8

/* Texture Enabled flags */
#define TEXTURE0_1D 0x1     /* Texture set/stage 0 (default) */
#define TEXTURE0_2D 0x2
#define TEXTURE0_3D 0x4
#define TEXTURE1_1D 0x10    /* Texture set/stage 1 */
#define TEXTURE1_2D 0x20
#define TEXTURE1_3D 0x40


/*
 * The GL_SGIS_multitexture extension introduces the idea of multiple stages
 * of texture application.  In Mesa the state for each stage is stored in
 * a "texture set".  This struct stores the state which is replicated for
 * each texture stage/set.
 *
 * The current set is indicated by the context's Texture.CurrentSet member.
 */
struct gl_texture_set {
	GLenum EnvMode;			/* GL_MODULATE, GL_DECAL, GL_BLEND */
	GLfloat EnvColor[4];
   GLuint TexGenEnabled;		/* Bitwise-OR of [STRQ]_BIT values */
	GLenum GenModeS;		/* Tex coord generation mode, either */
	GLenum GenModeT;		/*	GL_OBJECT_LINEAR, or */
	GLenum GenModeR;		/*	GL_EYE_LINEAR, or    */
	GLenum GenModeQ;		/*      GL_SPHERE_MAP        */
	GLfloat ObjectPlaneS[4];
	GLfloat ObjectPlaneT[4];
	GLfloat ObjectPlaneR[4];
	GLfloat ObjectPlaneQ[4];
	GLfloat EyePlaneS[4];
	GLfloat EyePlaneT[4];
	GLfloat EyePlaneR[4];
	GLfloat EyePlaneQ[4];
	struct gl_texture_object *Current1D;
	struct gl_texture_object *Current2D;
	struct gl_texture_object *Current3D;
	struct gl_texture_object *Current;  /* = Current1D, 2D, 3D or NULL */

   /* GL_EXT_multitexture */
   GLint TexCoordSet;			/* in [0, MAX_TEX_COORD_SETS-1] */
};


struct gl_texture_attrib {
   /* GL_EXT_multitexture */
   GLuint CurrentSet;			/* Current texture set (stage) */
   GLuint CurrentTransformSet;		/* Current texture xform set */

   GLuint Enabled;			/* Bitwise-OR of TEXTURE_XD values */

   struct gl_texture_set Set[MAX_TEX_SETS];  /* The multitexture stages/sets */

   GLboolean AnyDirty;
	struct gl_texture_object *Proxy1D;
	struct gl_texture_object *Proxy2D;
	struct gl_texture_object *Proxy3D;

	/* GL_EXT_shared_texture_palette */
	GLboolean SharedPalette;
	GLubyte Palette[MAX_TEXTURE_PALETTE_SIZE*4];
	GLuint PaletteSize;
	GLenum PaletteIntFormat;
	GLenum PaletteFormat;

   /* Temporary state for glPush/PopAttrib() only! */
   GLfloat Priority[3];
   GLint BorderColor[3][4];
   GLenum WrapS[3];
   GLenum WrapT[3];
   GLenum WrapR[3];
   GLenum MinFilter[3];
   GLenum MagFilter[3];
};


struct gl_transform_attrib {
	GLenum MatrixMode;			/* Matrix mode */
	GLfloat ClipEquation[MAX_CLIP_PLANES][4];
	GLboolean ClipEnabled[MAX_CLIP_PLANES];
	GLboolean AnyClip;			/* Any ClipEnabled[] true? */
	GLboolean Normalize;			/* Normalize all normals? */
	GLboolean RescaleNormals;		/* GL_EXT_rescale_normal */
};


struct gl_viewport_attrib {
	GLint X, Y;		/* position */
	GLsizei Width, Height;	/* size */
	GLfloat Near, Far;	/* Depth buffer range */
	GLfloat Sx, Sy, Sz;	/* NDC to WinCoord scaling */
	GLfloat Tx, Ty, Tz;	/* NDC to WinCoord translation */
};


/* For the attribute stack: */
struct gl_attrib_node {
	GLbitfield kind;
	void *data;
	struct gl_attrib_node *next;
};



/*
 * Client pixel packing/unpacking attributes
 */
struct gl_pixelstore_attrib {
	GLint Alignment;
	GLint RowLength;
	GLint SkipPixels;
	GLint SkipRows;
	GLint ImageHeight;     /* for GL_EXT_texture3D */
	GLint SkipImages;      /* for GL_EXT_texture3D */
	GLboolean SwapBytes;
	GLboolean LsbFirst;
};


/*
 * Client vertex array attributes
 */
struct gl_array_attrib {
	GLint VertexSize;
	GLenum VertexType;
	GLsizei VertexStride;		/* user-specified stride */
	GLsizei VertexStrideB;		/* actual stride in bytes */
	void *VertexPtr;
	GLboolean VertexEnabled;

	GLenum NormalType;
	GLsizei NormalStride;		/* user-specified stride */
	GLsizei NormalStrideB;		/* actual stride in bytes */
	void *NormalPtr;
	GLboolean NormalEnabled;

	GLint ColorSize;
	GLenum ColorType;
	GLsizei ColorStride;		/* user-specified stride */
	GLsizei ColorStrideB;		/* actual stride in bytes */
	void *ColorPtr;
	GLboolean ColorEnabled;

	GLenum IndexType;
	GLsizei IndexStride;		/* user-specified stride */
	GLsizei IndexStrideB;		/* actual stride in bytes */
	void *IndexPtr;
	GLboolean IndexEnabled;

	GLint TexCoordSize[MAX_TEX_SETS];
	GLenum TexCoordType[MAX_TEX_SETS];
	GLsizei TexCoordStride[MAX_TEX_SETS];	/* user-specified stride */
	GLsizei TexCoordStrideB[MAX_TEX_SETS];	/* actual stride in bytes */
	void *TexCoordPtr[MAX_TEX_SETS];
	GLboolean TexCoordEnabled[MAX_TEX_SETS];
	GLint TexCoordInterleaveFactor;

	GLsizei EdgeFlagStride;		/* user-specified stride */
	GLsizei EdgeFlagStrideB;	/* actual stride in bytes */
	GLboolean *EdgeFlagPtr;
	GLboolean EdgeFlagEnabled;
};



struct gl_feedback {
	GLenum Type;
	GLuint Mask;
	GLfloat *Buffer;
	GLuint BufferSize;
	GLuint Count;
};



struct gl_selection {
	GLuint *Buffer;
	GLuint BufferSize;	/* size of SelectBuffer */
	GLuint BufferCount;	/* number of values in SelectBuffer */
	GLuint Hits;		/* number of records in SelectBuffer */
	GLuint NameStackDepth;
	GLuint NameStack[MAX_NAME_STACK_DEPTH];
	GLboolean HitFlag;
	GLfloat HitMinZ, HitMaxZ;
};



/*
 * 1-D Evaluator control points
 */
struct gl_1d_map {
	GLuint Order;		/* Number of control points */
	GLfloat u1, u2;
	GLfloat *Points;	/* Points to contiguous control points */
	GLboolean Retain;	/* Reference counter */
};
	

/*
 * 2-D Evaluator control points
 */
struct gl_2d_map {
	GLuint Uorder;		/* Number of control points in U dimension */
	GLuint Vorder;		/* Number of control points in V dimension */
	GLfloat u1, u2;
	GLfloat v1, v2;
	GLfloat *Points;	/* Points to contiguous control points */
	GLboolean Retain;	/* Reference counter */
};


/*
 * All evalutator control points
 */
struct gl_evaluators {
	/* 1-D maps */
	struct gl_1d_map Map1Vertex3;
	struct gl_1d_map Map1Vertex4;
	struct gl_1d_map Map1Index;
	struct gl_1d_map Map1Color4;
	struct gl_1d_map Map1Normal;
	struct gl_1d_map Map1Texture1;
	struct gl_1d_map Map1Texture2;
	struct gl_1d_map Map1Texture3;
	struct gl_1d_map Map1Texture4;

	/* 2-D maps */
	struct gl_2d_map Map2Vertex3;
	struct gl_2d_map Map2Vertex4;
	struct gl_2d_map Map2Index;
	struct gl_2d_map Map2Color4;
	struct gl_2d_map Map2Normal;
	struct gl_2d_map Map2Texture1;
	struct gl_2d_map Map2Texture2;
	struct gl_2d_map Map2Texture3;
	struct gl_2d_map Map2Texture4;
};



/* Texture object record */
struct gl_texture_object {
	GLint RefCount;			/* reference count */
	GLuint Name;			/* an unsigned integer */
	GLuint Dimensions;		/* 1 or 2 or 3 */
	GLfloat Priority;		/* in [0,1] */
	GLint BorderColor[4];		/* as integers in [0,255] */
	GLenum WrapS;			/* GL_CLAMP, REPEAT or CLAMP_TO_EDGE */
	GLenum WrapT;			/* GL_CLAMP, REPEAT or CLAMP_TO_EDGE */
	GLenum WrapR;			/* GL_CLAMP, REPEAT or CLAMP_TO_EDGE */
	GLenum MinFilter;		/* minification filter */
	GLenum MagFilter;		/* magnification filter */
	GLfloat MinLod;			/* OpenGL 1.2 */
	GLfloat MaxLod;			/* OpenGL 1.2 */
	GLint BaseLevel;		/* OpenGL 1.2 */
	GLint MaxLevel;			/* OpenGL 1.2 */
	GLint P;			/* Highest mipmap level */
	GLfloat M;			/* = MIN(MaxLevel, P) - BaseLevel */
	GLfloat MinMagThresh;		/* min/mag threshold */
	struct gl_texture_image *Image[MAX_TEXTURE_LEVELS];

	/* GL_EXT_paletted_texture */
	GLubyte Palette[MAX_TEXTURE_PALETTE_SIZE*4];
	GLuint PaletteSize;
	GLenum PaletteIntFormat;
	GLenum PaletteFormat;

	/* For device driver: */
	GLboolean Dirty;	/* Set when any texobj state changes */
	void *DriverData;	/* Arbitrary device driver data */

	GLboolean Complete;		/* Complete set of images? */
	TextureSampleFunc SampleFunc;
	struct gl_texture_object *Next;	/* Next in linked list */
};



/*
 * State which can be shared by multiple contexts:
 */
struct gl_shared_state {
	GLint RefCount;				/* Reference count */
   struct HashTable *DisplayList;	/* Display lists hash table */
   struct HashTable *TexObjects;	/* Texture objects hash table */
   struct gl_texture_object *TexObjectList;/* Linked list of texture objects */

   /* Default texture objects */
   struct gl_texture_object *Default1D[MAX_TEX_SETS];
   struct gl_texture_object *Default2D[MAX_TEX_SETS];
   struct gl_texture_object *Default3D[MAX_TEX_SETS];
};



/*
 * Describes the color, depth, stencil and accum buffer parameters.
 * In C++ terms, think of this as a base class from which device drivers
 * will make derived classes.
 */
struct gl_visual {
	GLboolean RGBAflag;	/* Is frame buffer in RGBA mode, not CI? */
	GLboolean DBflag;	/* Is color buffer double buffered? */
	GLboolean StereoFlag;	/* stereo buffer? */

	GLint RedBits;		/* Bits per color component */
	GLint GreenBits;
	GLint BlueBits;
	GLint AlphaBits;

	GLint IndexBits;	/* Bits/pixel if in color index mode */

	GLint AccumBits;	/* Number of bits per color channel, or 0 */
	GLint DepthBits;	/* Number of bits in depth buffer, or 0 */
	GLint StencilBits;	/* Number of bits in stencil buffer, or 0 */

	/* Software alpha planes: */
	GLboolean FrontAlphaEnabled;
	GLboolean BackAlphaEnabled;
};



/*
 * A "frame buffer" is a color buffer and its optional ancillary buffers:
 * depth, accum, stencil, and software-simulated alpha buffers.
 * In C++ terms, think of this as a base class from which device drivers
 * will make derived classes.
 */
struct gl_frame_buffer {
	GLvisual *Visual;	/* The corresponding visual */

	GLint Width;		/* Width of frame buffer in pixels */
	GLint Height;		/* Height of frame buffer in pixels */

	GLdepth *Depth;		/* array [Width*Height] of GLdepth values */

	/* Stencil buffer */
	GLstencil *Stencil;	/* array [Width*Height] of GLstencil values */

	/* Accumulation buffer */
	GLaccum *Accum;		/* array [4*Width*Height] of GLaccum values */

	/* Software alpha planes: */
	GLubyte *FrontAlpha;	/* array [Width*Height] of GLubyte */
	GLubyte *BackAlpha;	/* array [Width*Height] of GLubyte */
	GLubyte *Alpha;		/* Points to front or back alpha buffer */

	/* Drawing bounds: intersection of window size and scissor box */
	GLint Xmin, Xmax, Ymin, Ymax;
};



/*
 * Bitmasks to indicate what auxillary information must be interpolated
 * when clipping (ClipMask).
 */
#define CLIP_FCOLOR_BIT		0x01
#define CLIP_BCOLOR_BIT		0x02
#define CLIP_FINDEX_BIT		0x04
#define CLIP_BINDEX_BIT		0x08
#define CLIP_TEXTURE_BIT	0x10



/*
 * Bitmasks to indicate which rasterization options are enabled (RasterMask)
 */
#define ALPHATEST_BIT		0x001	/* Alpha-test pixels */
#define BLEND_BIT		0x002	/* Blend pixels */
#define DEPTH_BIT		0x004	/* Depth-test pixels */
#define FOG_BIT			0x008	/* Per-pixel fog */
#define LOGIC_OP_BIT		0x010	/* Apply logic op in software */
#define SCISSOR_BIT		0x020	/* Scissor pixels */
#define STENCIL_BIT		0x040	/* Stencil pixels */
#define MASKING_BIT		0x080	/* Do glColorMask() or glIndexMask() */
#define ALPHABUF_BIT		0x100	/* Using software alpha buffer */
#define WINCLIP_BIT		0x200	/* Clip pixels/primitives to window */
#define FRONT_AND_BACK_BIT	0x400	/* Write to front and back buffers */
#define NO_DRAW_BIT		0x800	/* Don't write any pixels */


/*
 * Bits to indicate what state has to be updated (NewState)
 */
#define NEW_LIGHTING	0x01
#define NEW_RASTER_OPS	0x02
#define NEW_TEXTURING	0x04
#define NEW_POLYGON	0x08
#define NEW_DRVSTATE0	0x10 /* Reserved for drivers */
#define NEW_DRVSTATE1	0x20 /* Reserved for drivers */
#define NEW_DRVSTATE2	0x40 /* Reserved for drivers */
#define NEW_DRVSTATE3	0x80 /* Reserved for drivers */
#define NEW_ALL		0xff


/*
 * Different kinds of 4x4 transformation matrices:
 */
#define MATRIX_GENERAL		0	/* general 4x4 matrix */
#define MATRIX_IDENTITY		1	/* identity matrix */
#define MATRIX_ORTHO		2	/* orthographic projection matrix */
#define MATRIX_PERSPECTIVE	3	/* perspective projection matrix */
#define MATRIX_2D		4	/* 2-D transformation */
#define MATRIX_2D_NO_ROT	5	/* 2-D scale & translate only */
#define MATRIX_3D		6	/* 3-D transformation */


/*
 * FogMode values:
 */
#define FOG_NONE	0	/* no fog */
#define FOG_VERTEX	1	/* apply per vertex */
#define FOG_FRAGMENT	2	/* apply per fragment */


/*
 * Forward declaration of display list datatypes:
 */
union node;
typedef union node Node;



/*
 * The library context: 
 */

struct gl_context {
	/* State possibly shared with other contexts in the address space */
	struct gl_shared_state *Shared;

	/* API function pointer tables */
	struct gl_api_table API;		/* For api.c */
	struct gl_api_table Save;		/* Display list save funcs */
	struct gl_api_table Exec;		/* Execute funcs */

        GLvisual *Visual;
        GLframebuffer *Buffer;

	/* Driver function pointer table */
	struct dd_function_table Driver;

	void *DriverCtx;	/* Points to device driver context/state */
	void *DriverMgrCtx;	/* Points to device driver manager (optional)*/

	/* Modelview matrix and stack */
	GLboolean NewModelViewMatrix;
	GLuint ModelViewMatrixType;	/* = one of MATRIX_* values */
	GLfloat ModelViewMatrix[16];
	GLfloat ModelViewInv[16];	/* Inverse of ModelViewMatrix */
	GLuint ModelViewStackDepth;
	GLfloat ModelViewStack[MAX_MODELVIEW_STACK_DEPTH][16];

	/* Projection matrix and stack */
	GLboolean NewProjectionMatrix;
	GLuint ProjectionMatrixType;	/* = one of MATRIX_* values */
	GLfloat ProjectionMatrix[16];
	GLuint ProjectionStackDepth;
	GLfloat ProjectionStack[MAX_PROJECTION_STACK_DEPTH][16];
	GLfloat NearFarStack[MAX_PROJECTION_STACK_DEPTH][2];

	/* Texture matrix and stack */
	GLboolean NewTextureMatrix;
	GLuint TextureMatrixType[MAX_TEX_COORD_SETS];	/* = MATRIX_* */
	GLfloat TextureMatrix[MAX_TEX_COORD_SETS][16];
	GLuint TextureStackDepth[MAX_TEX_COORD_SETS];
	GLfloat TextureStack[MAX_TEX_COORD_SETS][MAX_TEXTURE_STACK_DEPTH][16];

	/* Display lists */
	GLuint CallDepth;	/* Current recursion calling depth */
	GLboolean ExecuteFlag;	/* Execute GL commands? */
	GLboolean CompileFlag;	/* Compile GL commands into display list? */
	Node *CurrentListPtr;	/* Head of list being compiled */
	GLuint CurrentListNum;	/* Number of the list being compiled */
	Node *CurrentBlock;	/* Pointer to current block of nodes */
	GLuint CurrentPos;	/* Index into current block of nodes */

	/* Renderer attribute stack */
	GLuint AttribStackDepth;
	struct gl_attrib_node *AttribStack[MAX_ATTRIB_STACK_DEPTH];

	/* Renderer attribute groups */
	struct gl_accum_attrib		Accum;
	struct gl_colorbuffer_attrib	Color;
	struct gl_current_attrib	Current;
	struct gl_depthbuffer_attrib	Depth;
	struct gl_eval_attrib		Eval;
	struct gl_fog_attrib		Fog;
	struct gl_hint_attrib		Hint;
	struct gl_light_attrib		Light;
	struct gl_line_attrib		Line;
	struct gl_list_attrib		List;
	struct gl_pixel_attrib		Pixel;
	struct gl_point_attrib		Point;
	struct gl_polygon_attrib	Polygon;
	GLuint PolygonStipple[32];
	struct gl_scissor_attrib	Scissor;
	struct gl_stencil_attrib	Stencil;
	struct gl_texture_attrib	Texture;
	struct gl_transform_attrib	Transform;
	struct gl_viewport_attrib	Viewport;

	/* Client state - GL_EXT_multitexture */
	GLuint TexCoordSet;		/* Target of glTexCoord() functions */

	/* Client attribute stack */
	GLuint ClientAttribStackDepth;
	struct gl_attrib_node *ClientAttribStack[MAX_CLIENT_ATTRIB_STACK_DEPTH];

	/* Client attribute groups */
	struct gl_array_attrib		Array;	/* Vertex arrays */
	struct gl_pixelstore_attrib	Pack;	/* Pixel packing */
	struct gl_pixelstore_attrib	Unpack;	/* Pixel unpacking */

	struct gl_evaluators EvalMap;	/* All evaluators */
	struct gl_feedback Feedback;	/* Feedback */
	struct gl_selection Select;	/* Selection */

	GLenum ErrorValue;		/* Last error code */

	GLboolean DirectContext;	/* Important for real GLX */

	/* Miscellaneous */
        GLuint NewState;        /* bitwise OR of NEW_* flags */
	GLenum RenderMode;	/* either GL_RENDER, GL_SELECT, GL_FEEDBACK */
	GLenum Primitive;	/* glBegin primitive or GL_BITMAP */
	GLuint StippleCounter;	/* Line stipple counter */
	GLuint ClipMask;	/* OR of CLIP_* values from above */
	GLuint RasterMask;	/* OR of rasterization flags */
	GLboolean LightTwoSide;	/* Compute two-sided lighting? */
	GLboolean DirectTriangles;/* Directly call (*ctx->TriangleFunc) ? */
	GLfloat PolygonZoffset;	/* Z offset for GL_FILL polygons */
	GLfloat LineZoffset;	/* Z offset for GL_LINE polygons */
	GLfloat PointZoffset;	/* Z offset for GL_POINT polygons */
	GLboolean NeedNormals;	 /* Are vertex normal vectors needed? */
        GLboolean MutablePixels;/* Can rasterization change pixel's color? */
        GLboolean MonoPixels;    /* Are all pixels likely to be same color? */
	GLuint FogMode;         /* FOG_OFF, FOG_VERTEX or FOG_FRAGMENT */

	GLboolean DoViewportMapping;

	/* Points to function which interpolates colors, etc when clipping */
	clip_interp_func ClipInterpFunc;

	/* The vertex buffer being used by this context */
	struct vertex_buffer* VB;

	/* The pixel buffer being used by this context */
	struct pixel_buffer* PB;

#ifdef PROFILE
        /* Performance measurements */
        GLuint BeginEndCount;	/* number of glBegin/glEnd pairs */
        GLdouble BeginEndTime;	/* seconds spent between glBegin/glEnd */
        GLuint VertexCount;	/* number of vertices processed */
        GLdouble VertexTime;	/* total time in seconds */
        GLuint PointCount;	/* number of points rendered */
        GLdouble PointTime;	/* total time in seconds */
        GLuint LineCount;	/* number of lines rendered */
        GLdouble LineTime;	/* total time in seconds */
        GLuint PolygonCount;	/* number of polygons rendered */
        GLdouble PolygonTime;	/* total time in seconds */
        GLuint ClearCount;	/* number of glClear calls */
        GLdouble ClearTime;	/* seconds spent in glClear */
        GLuint SwapCount;	/* number of swap-buffer calls */
        GLdouble SwapTime;	/* seconds spent in swap-buffers */
#endif

        /* For debugging/development only */
        GLboolean NoRaster;

        /* Dither disable via MESA_NO_DITHER env var */
        GLboolean NoDither;
};


#endif
