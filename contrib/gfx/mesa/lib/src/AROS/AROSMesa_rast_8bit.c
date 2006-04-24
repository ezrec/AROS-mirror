/* $Id: AROSMesa_8bit.c 24318 2006-04-15 17:52:01Z NicJA $*/

/*
 * Mesa 3-D graphics library
 * Copyright (C) 1995  Brian Paul  (brianp@ssec.wisc.edu)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#warning "TODO: 8bit code needs to be synchronised with the truecolor rasterizer"

#include "AROSMesa_intern.h"
#include <GL/AROSMesa.h>

#include "AROSMesa_rast_common.h"
#ifdef ADISP_CYBERGFX
#include "AROSMesa_rast_TC.h"
#endif

#ifdef __GNUC__
#include "../aros/misc/ht_colors_protos.h"
#else
#include "/aros/misc/ht_colors_protos.h"
#endif

#include <exec/memory.h>
#include <exec/types.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>
#include <graphics/rpattr.h>

extern struct Library*  CyberGfxBase;

#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include "../glheader.h"
#include "../context.h"
#include "../colormac.h"
#include "../depth.h"
#include "../extensions.h"
#include "../macros.h"
#include "../matrix.h"
#include "../mem.h"
#include "../mmath.h"
#include "../mtypes.h"
#include "../texformat.h"
#include "../texstore.h"
#include "../array_cache/acache.h"
#include "../swrast/swrast.h"
#include "../swrast_setup/swrast_setup.h"
#include "../swrast/s_context.h"
#include "../swrast/s_depth.h"
#include "../swrast/s_lines.h"
#include "../swrast/s_triangle.h"
#include "../swrast/s_trispan.h"
#include "../tnl/tnl.h"
#include "../tnl/t_context.h"
#include "../tnl/t_pipeline.h"

static void aros8bit_buffer_size( GLcontext *ctx,GLuint *width, GLuint *height);

/* Functions from Rasterizer_common */
extern void arosRasterizer_Standard_SwapBuffer(AROSMesaContext amesa);
extern void arosRasterizer_Standard_Dispose(AROSMesaContext amesa);

extern BOOL arosRasterizer_allocarea(struct RastPort *rp );
extern void arosRasterizer_freearea(struct RastPort *rp);
extern BOOL arosRasterizer_make_temp_raster( struct RastPort *rp );
extern void arosRasterizer_destroy_temp_raster( struct RastPort *rp );
extern BOOL arosRasterizer_alloc_temp_rastport(AROSMesaContext amesa);
extern void arosRasterizer_free_temp_rastport(AROSMesaContext amesa);
extern UBYTE* arosRasterizer_alloc_penbackarray( int width, int height, int bytes);
extern void arosRasterizer_destroy_penbackarray(UBYTE *buf);
extern struct RastPort *arosRasterizer_make_rastport( int width, int height, int depth, struct BitMap *friendbm );
extern void arosRasterizer_destroy_rastport( struct RastPort *rp );
extern void arosRasterizer_AllocOneLine(AROSMesaContext amesa);
extern void arosRasterizer_FreeOneLine(AROSMesaContext amesa);

/**********************************************************************/
/*****        Miscellaneous device driver funcs       *****/
/**********************************************************************/

static void
aros8bit_finish( GLcontext *ctx )
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_finish(ctx @ %x) : Unimplemented - glFinish()\n", ctx));
#endif
  /* implements glFinish if possible */
}

static void
aros8bit_flush( GLcontext *ctx )
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_flush(ctx @ %x) : Unimplemented - glFlush()\n", ctx));
#endif
  /* implements glFlush if possible */
}

static void
aros8bit_clear_index( GLcontext *ctx, GLuint index )
{
  /* implement glClearIndex */
  /* usually just save the value in the context struct */
  /*printf("aros8bit_clear_index=glClearIndex=%d\n",index);*/
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_clear_index()\n"));
#endif
  amesa->clearpixel = amesa->penconv[index];
}

static void
aros8bit_clear(GLcontext* ctx, GLbitfield mask,
                GLboolean all, GLint x, GLint y, GLint width, GLint height)
{
  /*
  * Clear the specified region of the color buffer using the clear color
  * or index as specified by one of the two functions above.
  * If all==GL_TRUE, clear whole buffer
  */
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;

#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_clear(%d,%d,%d,%d,%d)\n", all, x, y, width, height));
#endif

  if ((mask & (DD_FRONT_LEFT_BIT)))
  {    
    if(amesa->front_rp != 0)
    {
      SetAPen(amesa->front_rp, amesa->clearpixel);
      if(all)
      {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_clear: front_rp->Clearing viewport (ALL)\n"));
#endif
        RectFill(amesa->front_rp, FIXx(ctx->Viewport.X), (FIXy(ctx->Viewport.Y) - ctx->Viewport.Height) + 1, (FIXx(ctx->Viewport.X) + ctx->Viewport.Width) - 1, FIXy(ctx->Viewport.Y));
      }
      else
      {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_clear: front_rp->Clearing Area\n"));
#endif
        RectFill(amesa->front_rp, FIXx(x), (FIXy(y) - height), width, FIXy(y));
      }
      mask &= ~DD_FRONT_LEFT_BIT;
    } else {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_clear: Serious ERROR amesa->front_rp = NULL detected\n"));
#endif
    }
  }

  if ((mask & (DD_BACK_LEFT_BIT)))
  {    
    if(amesa->back_rp != 0)
    {
      SetAPen(amesa->back_rp, amesa->clearpixel);
      if(all)
      {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_clear: back_rp->Clearing viewport (ALL)\n"));
#endif
        RectFill(amesa->back_rp, FIXx(ctx->Viewport.X), (FIXy(ctx->Viewport.Y) - ctx->Viewport.Height) + 1, (FIXx(ctx->Viewport.X) + ctx->Viewport.Width) - 1, FIXy(ctx->Viewport.Y));
      }
      else
      {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_clear: back_rp->Clearing Area\n"));
#endif
        RectFill(amesa->back_rp, FIXx(x), (FIXy(y) - height), width, FIXy(y));
      }
      mask &= ~DD_BACK_LEFT_BIT;
    } else {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_clear: Serious ERROR amesa->back_rp = NULL detected\n"));
#endif
    }
  }

  if (mask)
    _swrast_Clear( ctx, mask, all, x, y, width, height ); /* Remaining buffers to be cleared .. */
}

/**********************************************************************/
/*****      Accelerated point, line, polygon rendering    *****/
/**********************************************************************/


/**********************************************************************/
/*****         Span-based pixel drawing           *****/
/**********************************************************************/

/* Write a horizontal span of 32-bit color-index pixels with a boolean mask. */
static void
aros8bit_write_ci32_span(const GLcontext *ctx,
                GLuint n, GLint x, GLint y,
                const GLuint index[],
                const GLubyte mask[] )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i, ant;
  UBYTE *dp = NULL;
  unsigned long *penconv = amesa->penconv;
  struct RastPort *rp = amesa->rp;

#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_write_ci32_span(%d,%d,%d)\n",n,x,y));
#endif

  y = FIXy(y);
  x = FIXx(x);
  /* JAM: Added correct checking */
  /* if((dp = amesa->imageline) && amesa->tmpras) */
  if((dp = amesa->imageline) && amesa->temprp)
  {           /* if imageline and temporary rastport have been
               allocated then use fastversion */
    ant = 0;
    for (i = 0; i < n; i++)
    { /* draw pixel (x[i],y[i]) using index[i] */
      if (mask[i])
      {
        ant++;
/*        x++; */
        *dp = penconv[index[i]];
        dp++;
      } else {
        if(ant)
        {
          /* JAM: Replaced by correct call */
          /* WritePixelLine8(amesa->rp,x,y,ant,amesa->imageline,amesa->tmpras); */
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_write_index_span: WritePixelLine8()\n"));
#endif
          WritePixelLine8(rp, x, y, ant, amesa->imageline, amesa->temprp);
        }
        dp = amesa->imageline;
        ant = 0;
        x++;
      }
    }
    if(ant)
    {
      /* JAM: Replaced by correct call */
      /* WritePixelLine8(amesa->rp,x,y,ant,amesa->imageline,amesa->tmpras); */
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_write_index_span: WritePixelLine8()\n"));
#endif
      WritePixelLine8(rp, x, y, ant, amesa->imageline, amesa->temprp);
    }
  } else {      /* Slower */
    for (i = 0; i < n; i++, x++)
    {
      if (mask[i])
      {
         /* draw pixel (x[i],y[i]) using index[i] */
        SetAPen(rp, penconv[index[i]]);
        WritePixel(rp, x, y);
      }
    }
  }
}

/* Write a horizontal span of 8-bit color-index pixels with a boolean mask. */
static void
aros8bit_write_ci8_span(const GLcontext *ctx,
                GLuint n, GLint x, GLint y,
                const GLubyte index[],
                const GLubyte mask[] )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i, ant;
  UBYTE *dp = NULL;
  unsigned long *penconv = amesa->penconv;
  struct RastPort *rp = amesa->rp;

#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_write_ci8_span(%d,%d,%d)\n", n, x, y));
#endif

  y = FIXy(y);
  x = FIXx(x);
  /* JAM: Added correct checking */
  /* if((dp = amesa->imageline) && amesa->tmpras) */
  if((dp = amesa->imageline) && amesa->temprp)
  {           /* if imageline and temporary rastport have been
               allocated then use fastversion */
    ant = 0;
    for (i = 0; i < n; i++)
    { /* draw pixel (x[i],y[i]) using index[i] */
      if (mask[i])
      {
        ant++;
/*        x++; */
        *dp = penconv[index[i]];
        dp++;
      } else {
        if(ant)
        {
          /* JAM: Replaced by correct call */
          /* WritePixelLine8(amesa->rp,x,y,ant,amesa->imageline,amesa->tmpras); */
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_write_index_span: WritePixelLine8()\n"));
#endif
          WritePixelLine8(rp, x, y, ant, amesa->imageline, amesa->temprp);
        }
        dp = amesa->imageline;
        ant = 0;
        x++;
      }
    }
    if(ant)
    {
      /* JAM: Replaced by correct call */
      /* WritePixelLine8(amesa->rp,x,y,ant,amesa->imageline,amesa->tmpras); */
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_write_index_span: WritePixelLine8()\n"));
#endif
      WritePixelLine8(rp, x, y, ant, amesa->imageline, amesa->temprp);
    }
  } else {      /* Slower */
    for (i = 0; i < n; i++, x++)
    {
      if (mask[i])
      {
         /* draw pixel (x[i],y[i]) using index[i] */
        SetAPen(rp, penconv[index[i]]);
        WritePixel(rp, x, y);
      }
    }
  }
}

/*
* Write a horizontal span of pixels with a boolean mask.  The current
* color index is used for all pixels.
*/
static void
aros8bit_write_mono_ci_span(const GLcontext* ctx,
                 GLuint n,GLint x,GLint y,
                 const GLubyte mask[])
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i;
  y = FIXy(y);
  x = FIXx(x);
  SetAPen(amesa->rp, amesa->pixel);

#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_write_mono_ci_span()\n"));
#endif

  i = 0;
  while(i < n)
  {
    while((!(mask[i])) && (i < n))
    {
      i++;
      x++;
    }
  
    if(i < n)
    {
      Move(amesa->rp, x, y);
      while(mask[i] && (i < n))
      {
        i++;
        x++;
      }
      Draw(amesa->rp, x, y);
    }
  }
}

/**********************************************************************/
/*****           Array-based pixel drawing        *****/
/**********************************************************************/

/* Write an array of 32-bit index pixels with a boolean mask. */
static void
aros8bit_write_ci32_pixels( const GLcontext* ctx,
                 GLuint n, const GLint x[], const GLint y[],
                 const GLuint index[], const GLubyte mask[] )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i;
  struct RastPort *rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_write_ci32_pixels()\n"));
#endif

  for (i = 0; i < n; i++)
  {
    if (mask[i])
    {
      SetAPen(rp, amesa->penconv[index[i]]);
      WritePixel(rp, FIXx(x[i]), FIXy(y[i]));
    }
  }
}

/*
* Write an array of pixels with a boolean mask.  The current color
* index is used for all pixels.
*/
static void
aros8bit_write_mono_ci_pixels( const GLcontext* ctx,
                  GLuint n,
                  const GLint x[], const GLint y[],
                  const GLubyte mask[] )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i;
  struct RastPort *rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_write_mono_ci_pixels()\n"));
#endif

  SetAPen(rp, amesa->pixel);

  for (i = 0; i < n; i++)
  {
    if (mask[i])
    {
/*    write pixel x[i], y[i] using current index  */
      WritePixel(rp, FIXx(x[i]), FIXy(y[i]));
    }
  }
}

/**********************************************************************/
/*****          Read spans of pixels                *****/
/**********************************************************************/

/* Read a horizontal span of color-index pixels. */
static void
aros8bit_read_ci32_span( const GLcontext* ctx, GLuint n, GLint x, GLint y,
              GLuint index[])
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i;

#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_read_ci32_span()\n"));
#endif

  y = FIXy(y);
  x = FIXx(x);

  if(amesa->temprp && amesa->imageline)
  {
    ReadPixelLine8(amesa->rp, x, y, n, amesa->imageline, amesa->temprp);
    for(i = 0; i < n; i++)
    {
      index[i] = amesa->imageline[i];
    }
  } else {
    for (i = 0; i < n; i++, x++)
    {
      index[i] = ReadPixel(amesa->rp, x, y);
    }
  }
}

/**********************************************************************/
/*****             Read arrays of pixels              *****/
/**********************************************************************/

/* Read an array of color index pixels. */
static void
aros8bit_read_ci32_pixels( const GLcontext* ctx,
                GLuint n, const GLint x[], const GLint y[],
                GLuint index[], const GLubyte mask[] )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  struct RastPort *rp = amesa->rp;
  int i;

#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_read_ci32_pixels()\n"));
#endif
	
  for (i = 0; i < n; i++)
  {
    if (mask[i])
    {
/*     index[i] = read_pixel x[i], y[i] */
        index[i] = ReadPixel(rp, FIXx(x[i]), FIXy(y[i]));
    }
  }
}

/**********************************************************************/
/**********************************************************************/

static GLboolean
aros8bit_set_draw_buffer( GLcontext *ctx, GLenum mode )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_set_draw_buffer(ctx @ %x)\n", ctx));
#endif
  if (mode == GL_FRONT_LEFT)
  {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_set_draw_buffer: FRONT BUFFER\n"));
#endif
    return(GL_TRUE);
  } else if ((mode == GL_BACK_LEFT)&&(amesa->visual->db_flag)) {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_set_draw_buffer: BACK BUFFER\n"));
#endif
    return(GL_TRUE);
  } else {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_set_draw_buffer: UNSUPPORTED BUFFER!\n"));
#endif
    return(GL_FALSE);
  }
}

static void
aros8bit_set_read_buffer( GLcontext *ctx, GLframebuffer *buffer, GLenum mode )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_set_read_buffer(ctx @ %x) : TODO!\n", ctx));
#endif
  /* We dont support seperate read buffers */
  if (mode == GL_FRONT_LEFT)
  {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_set_read_buffer: FRONT BUFFER\n"));
#endif
  } else if ((mode == GL_BACK_LEFT)&&(amesa->visual->db_flag)) {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_set_read_buffer: BACK BUFFER\n"));
#endif
  } else {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_set_read_buffer: UNSUPPORTED BUFFER!\n"));
#endif
  }
}

static const GLubyte *
aros8bit_renderer_string( GLcontext *ctx, GLenum name)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_renderer_string()\n"));
#endif
	(void) ctx;
	switch (name)
	{
	case GL_RENDERER:
   	return "AROS 8bit Rasterizer";
   default:
      return NULL;
   }
}

/**********************************************************************/
/**********************************************************************/
	/* Initialize all the pointers in the DD struct.  Do this whenever   */
	/* a new context is made current or we change buffers via set_buffer! */

void
aros8bit_register_swrast_functions(GLcontext *ctx)
{
  SWcontext *swrast = SWRAST_CONTEXT( ctx);
// swrast->choose_line = arosTC_choose_line;
// swrast->choose_triangle = arosTC_choose_triangle;

// swrast->invalidate_line |= OSMESA_NEW_LINE;
// swrast->invalidate_triangle |= OSMESA_NEW_TRIANGLE;
}

void
aros8bit_update_state(GLcontext *ctx, GLuint new_state)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_update_state(ctx @ %x, state %d)\n", ctx, new_state));
#endif
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  struct swrast_device_driver *swdd = NULL;
  TNLcontext *tnl = NULL;

  _swrast_InvalidateState(ctx, new_state);
  _swsetup_InvalidateState(ctx, new_state);
  _ac_InvalidateState(ctx, new_state);
  _tnl_InvalidateState(ctx, new_state);

  ctx->Driver.GetString = aros8bit_renderer_string;
  ctx->Driver.GetBufferSize = aros8bit_buffer_size;
  ctx->Driver.Flush = aros8bit_flush;
  ctx->Driver.Finish = aros8bit_finish;

  ctx->Driver.UpdateState = aros8bit_update_state;

/* Software Rasterizer pixel paths */

  ctx->Driver.Accum = _swrast_Accum;
  ctx->Driver.Bitmap = _swrast_Bitmap;
  ctx->Driver.Clear = aros8bit_clear;
  ctx->Driver.ResizeBuffersMESA = _swrast_alloc_buffers;
  ctx->Driver.CopyPixels = _swrast_CopyPixels;
  ctx->Driver.DrawPixels = _swrast_DrawPixels;
  ctx->Driver.ReadPixels = _swrast_ReadPixels;

/* Software Texture Functions */

  ctx->Driver.ChooseTextureFormat = _mesa_choose_tex_format;
  ctx->Driver.TexImage1D = _mesa_store_teximage1d;
  ctx->Driver.TexImage2D = _mesa_store_teximage2d;
  ctx->Driver.TexImage3D = _mesa_store_teximage3d;
  ctx->Driver.TexSubImage1D = _mesa_store_texsubimage1d;
  ctx->Driver.TexSubImage2D = _mesa_store_texsubimage2d;
  ctx->Driver.TexSubImage3D = _mesa_store_texsubimage3d;
  ctx->Driver.TestProxyTexImage = _mesa_test_proxy_teximage;

  ctx->Driver.CopyTexImage1D = _swrast_copy_teximage1d;
  ctx->Driver.CopyTexImage2D = _swrast_copy_teximage2d;
  ctx->Driver.CopyTexSubImage1D = _swrast_copy_texsubimage1d;
  ctx->Driver.CopyTexSubImage2D = _swrast_copy_texsubimage2d;
  ctx->Driver.CopyTexSubImage3D = _swrast_copy_texsubimage3d;
  ctx->Driver.CopyColorTable = _swrast_CopyColorTable;
  ctx->Driver.CopyColorSubTable = _swrast_CopyColorSubTable;
  ctx->Driver.CopyConvolutionFilter1D = _swrast_CopyConvolutionFilter1D;
  ctx->Driver.CopyConvolutionFilter2D = _swrast_CopyConvolutionFilter2D;

  swdd = _swrast_GetDeviceDriverReference(ctx);
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_update_state: SWDD @ %x\n", swdd));
#endif
  /* Indexed Color functions: */
  swdd->WriteCI32Span       = aros8bit_write_ci32_span;
  swdd->WriteCI8Span        = aros8bit_write_ci8_span;
  swdd->WriteMonoCISpan     = aros8bit_write_mono_ci_span;
  swdd->WriteCI32Pixels     = aros8bit_write_ci32_pixels;
  swdd->WriteMonoCIPixels   = aros8bit_write_mono_ci_pixels;
  swdd->ReadCI32Span        = aros8bit_read_ci32_span;
  swdd->ReadCI32Pixels      = aros8bit_read_ci32_pixels;

/* Statechange callbacks */
  ctx->Driver.SetDrawBuffer = aros8bit_set_draw_buffer;
  swdd->SetReadBuffer = aros8bit_set_read_buffer;
  ctx->Driver.ClearIndex = aros8bit_clear_index;

/* TNL Driver Interface */
  tnl = TNL_CONTEXT(ctx);
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_update_state: TNL @ %x\n", tnl));
#endif
  tnl->Driver.RunPipeline = _tnl_run_pipeline;
  tnl->Driver.RenderStart = _swsetup_RenderStart;
  tnl->Driver.RenderFinish = _swsetup_RenderFinish;
  tnl->Driver.BuildProjectedVertices = _swsetup_BuildProjectedVertices;
  tnl->Driver.RenderPrimitive = _swsetup_RenderPrimitive;
  tnl->Driver.PointsFunc = _swsetup_Points;
  tnl->Driver.LineFunc = _swsetup_Line;
  tnl->Driver.TriangleFunc = _swsetup_Triangle;
  tnl->Driver.QuadFunc = _swsetup_Quad;
  tnl->Driver.ResetLineStipple = _swrast_ResetLineStipple;
  tnl->Driver.RenderInterp = _swsetup_RenderInterp;
  tnl->Driver.RenderCopyPV = _swsetup_RenderCopyPV;
  tnl->Driver.RenderClippedLine = _swsetup_RenderClippedLine;
  tnl->Driver.RenderClippedPolygon = _swsetup_RenderClippedPolygon;
}

/**********************************************************************/
/*****          AROS/Mesa private init/despose/resize   *****/
/**********************************************************************/

static void
aros8bit_buffer_size( GLcontext *ctx,GLuint *width, GLuint *height)
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_buffer_size(amesa @ %x)\n", amesa));
#endif

  *width = amesa->width;
  *height = amesa->height;

  if(!(( amesa->width  == (amesa->front_rp->Layer->bounds.MaxX - amesa->front_rp->Layer->bounds.MinX - amesa->left-amesa->right) ) 
  &&  ( amesa->height == (amesa->front_rp->Layer->bounds.MaxY - amesa->front_rp->Layer->bounds.MinY - amesa->bottom-amesa->top) )))
  {
    arosRasterizer_FreeOneLine(amesa);
  
    amesa->RealWidth = amesa->front_rp->Layer->bounds.MaxX - amesa->front_rp->Layer->bounds.MinX ;
    amesa->FixedWidth = amesa->RealWidth;

    amesa->RealHeight = amesa->front_rp->Layer->bounds.MaxY - amesa->front_rp->Layer->bounds.MinY;
    amesa->FixedHeight = amesa->RealHeight;
  
    amesa->width = amesa->RealWidth - amesa->left-amesa->right;
    *width = amesa->width;

    amesa->height = amesa->RealHeight - amesa->bottom-amesa->top;
    *height = amesa->height;

    amesa->depth = GetBitMapAttr(amesa->front_rp->BitMap, BMA_DEPTH);
  
    arosRasterizer_destroy_temp_raster( amesa->rp); /* deallocate temp raster */
  
    /* JAM: Added arosRasterizer_free_temp_rastport() */
    arosRasterizer_free_temp_rastport(amesa);
  
    if (amesa->visual->db_flag)
    {
      if (amesa->back_rp)
      {
        arosRasterizer_destroy_rastport(amesa->back_rp);/* Free double buffer */
      }

      if((amesa->back_rp = arosRasterizer_make_rastport(amesa->RealWidth, amesa->RealHeight, amesa->depth, amesa->front_rp->BitMap)) == NULL)
      {
        if (amesa->front_rp == NULL)
        {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_buffer_size: ERROR - Illegal state!!\n"));
#endif
          return;
        }
        amesa->rp = amesa->front_rp;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_buffer_size: ERROR - Failed to allocate BackBuffer rastport in specified size.\n"));
#endif
      }
      else
      {
        amesa->rp = amesa->back_rp;
      }
    }
  
    if(!arosRasterizer_make_temp_raster( amesa->rp ))
    {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_buffer_size: ERROR - Failed to allocate TmpRasterPort\n"));
#endif
    }
  
    /* JAM: added alloc_temp_rastport */
    arosRasterizer_alloc_temp_rastport(amesa);
  
    arosRasterizer_AllocOneLine(amesa);
  }
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_buffer_size(ctx=%x) = w:%d x h:%d\n",ctx,*width,*height));
D(bug("[AROSMESA:8bit] aros8bit_buffer_size:    amesa->RealWidth =%d\n", amesa->RealWidth));
D(bug("[AROSMESA:8bit] aros8bit_buffer_size:    amesa->RealHeight=%d\n", amesa->RealHeight));
D(bug("[AROSMESA:8bit] aros8bit_buffer_size:    amesa->width =%d\n", amesa->width));
D(bug("[AROSMESA:8bit] aros8bit_buffer_size:    amesa->height=%d\n", amesa->height));
D(bug("[AROSMESA:8bit] aros8bit_buffer_size:    amesa->left  =%d\n", amesa->left));
D(bug("[AROSMESA:8bit] aros8bit_buffer_size:    amesa->right  =%d\n", amesa->right));
D(bug("[AROSMESA:8bit] aros8bit_buffer_size:    amesa->top   =%d\n", amesa->top));
D(bug("[AROSMESA:8bit] aros8bit_buffer_size:    amesa->bottom=%d\n", amesa->bottom));
D(bug("[AROSMESA:8bit] aros8bit_buffer_size:    amesa->depth =%d\n", amesa->depth));
#endif
}

BOOL
aros8bit_Standard_init(AROSMesaContext amesa,struct TagItem *tagList)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_Standard_init()\n"));
#endif
  amesa->RealWidth = amesa->rp->Layer->bounds.MaxX - amesa->rp->Layer->bounds.MinX;
  amesa->FixedWidth = amesa->RealWidth;

  amesa->RealHeight = amesa->rp->Layer->bounds.MaxY - amesa->rp->Layer->bounds.MinY;
  amesa->FixedHeight = amesa->RealHeight;

  amesa->left = GetTagData(AMA_Left,0,tagList);
  amesa->right = GetTagData(AMA_Right,0,tagList);
  amesa->top = GetTagData(AMA_Top,0,tagList);
  amesa->bottom = GetTagData(AMA_Bottom,0,tagList);

  amesa->front_rp = amesa->rp;
  amesa->back_rp = NULL;
/*  amesa->rp = amesa->front_rp; */

  amesa->width = GetTagData(AMA_Width,amesa->RealWidth - amesa->left - amesa->right,tagList);
  amesa->height= GetTagData(AMA_Height,amesa->RealHeight - amesa->top - amesa->bottom,tagList);

/*  amesa->gl_ctx->BufferWidth = amesa->width; */
/*  amesa->gl_ctx->BufferHeight = amesa->height; */

  amesa->pixel = 0;	/* current drawing pen */

  AllocCMap(amesa->Screen);
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_Standard_init: Base dimensions set -:\n"));
D(bug("[AROSMESA:8bit] aros8bit_Standard_init:    amesa->RealWidth =%d\n", amesa->RealWidth));
D(bug("[AROSMESA:8bit] aros8bit_Standard_init:    amesa->RealHeight=%d\n", amesa->RealHeight));
D(bug("[AROSMESA:8bit] aros8bit_Standard_init:    amesa->width =%d\n", amesa->width));
D(bug("[AROSMESA:8bit] aros8bit_Standard_init:    amesa->height=%d\n", amesa->height));
D(bug("[AROSMESA:8bit] aros8bit_Standard_init:    amesa->left  =%d\n", amesa->left));
D(bug("[AROSMESA:8bit] aros8bit_Standard_init:    amesa->right  =%d\n", amesa->right));
D(bug("[AROSMESA:8bit] aros8bit_Standard_init:    amesa->top   =%d\n", amesa->top));
D(bug("[AROSMESA:8bit] aros8bit_Standard_init:    amesa->bottom=%d\n", amesa->bottom));
D(bug("[AROSMESA:8bit] aros8bit_Standard_init:    amesa->depth =%d\n", amesa->depth));
#endif

  if (amesa->visual->db_flag == GL_TRUE)
  {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] Allocating DOUBLE BUFFERED display inside aros8bit_Standard_init"));
#endif
    if((amesa->back_rp = arosRasterizer_make_rastport(amesa->RealWidth, amesa->RealHeight, amesa->depth, amesa->rp->BitMap))!=NULL)
    {
      ((GLcontext *)amesa->gl_ctx)->Color.DrawBuffer = GL_BACK;
      amesa->rp = amesa->back_rp;
    }
    else
    {
      printf("make_rastport Faild\n");
      ((GLcontext *)amesa->gl_ctx)->Color.DrawBuffer = GL_FRONT;
    }
  }
  else ((GLcontext *)amesa->gl_ctx)->Color.DrawBuffer = GL_FRONT;

  arosRasterizer_AllocOneLine(amesa); /* A linebuffer for WritePixelLine */

  if (!arosRasterizer_make_temp_raster( amesa->rp )) printf("Error allocating TmpRastPort\n");

  arosRasterizer_alloc_temp_rastport(amesa);
  arosRasterizer_allocarea(amesa->rp);

#ifdef ADISP_CYBERGFX
  amesa->InitDD = amesa->depth <= 8 ? aros8bit_update_state : arosTC_update_state;  /*standard drawing*/
#else
  amesa->InitDD = aros8bit_update_state;
#endif

  amesa->Dispose = arosRasterizer_Standard_Dispose;
  amesa->SwapBuffer = arosRasterizer_Standard_SwapBuffer;

  return(TRUE);
}

/* Disabled for now .. (nicja)
#ifdef AMESA_DOUBLEBUFFFAST
#include "AROSMesa_8bit_db.c"
#endif*/
