/* $Id: AROSMesa_TC.c 24318 2006-04-15 17:52:01Z NicJA $ */

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

#include "AROSMesa_intern.h"
#include <GL/AROSMesa.h>

#include "AROSMesa_rast_common.h"
#ifdef ADISP_AGA
#include "AROSMesa_rast_8bit.h"
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


/** set the correct storage format for pixels **/

#if (AROS_BIG_ENDIAN == 1)
#define AROS_PIXFMT RECFMT_ARGB    /* Big Endian Archs. */
#else
#define AROS_PIXFMT RECTFMT_BGRA32   /* Little Endian Archs. */
#endif

/**********************************************************************/
/*****        Some Usefull code                   *****/
/**********************************************************************/

/*
  The Drawing area is defined by:

  CC.Viewport.X = x;
  CC.Viewport.Width = width;
  CC.Viewport.Y = y;
  CC.Viewport.Height = height;
*/

static void arosTC_buffer_size( GLcontext *ctx, GLuint *width, GLuint *height);

/* Functions from Rasterizer_common */
extern void arosRasterizer_Standard_SwapBuffer(AROSMesaContext amesa);
extern void arosRasterizer_Standard_Dispose(AROSMesaContext amesa);
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
arosTC_finish( GLcontext *ctx )
{
  /* implements glFinish if possible */
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_finish(ctx @ %x) : Unimplemented - glFinish()\n", ctx));
#endif
}

static void
arosTC_flush( GLcontext *ctx )
{
  /* implements glFlush if possible */
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_flush(ctx @ %x) : Unimplemented - glFlush()\n", ctx));
#endif
}

/* implements glClearColor - Set the color used to clear the color buffer. */
static void
arosTC_clear_color( GLcontext *ctx,
                    const GLchan color[4] )
{
#warning "TODO: Free pen color if not used"
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  amesa->clearpixel = TC_ARGB(color[RCOMP], color[GCOMP], color[BCOMP], color[ACOMP]);
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_clear_color(c=%x,r=%d,g=%d,b=%d,a=%d) = %x\n", ctx, color[RCOMP], color[GCOMP], color[BCOMP], color[ACOMP], amesa->clearpixel));
#endif
}

/*
* Clear the specified region of the color buffer using the clear color
* or index as specified by one of the two functions above.
*/
static void
arosTC_clear(GLcontext* ctx, GLbitfield mask,
                GLboolean all, GLint x, GLint y, GLint width, GLint height)
{
  /*
  * If all==GL_TRUE, clear whole buffer
  */
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  struct RastPort *clear_Rast = NULL;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_clear(all:%d,x:%d,y:%d,w:%d,h:%d)\n", all, x, y, width, height));
#endif

  if ((mask & (DD_FRONT_LEFT_BIT)))
  {  
    if(amesa->front_rp != NULL)
    {
      if(all)
      {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_clear: front_rp->Clearing viewport (ALL)\n"));
#endif
        FillPixelArray (amesa->front_rp, FIXx(ctx->Viewport.X), (FIXy(ctx->Viewport.Y) - ctx->Viewport.Height) + 1, /*FIXx(ctx->Viewport.X)+*/ctx->Viewport.Width - FIXx(ctx->Viewport.X), ctx->Viewport.Height/*FIXy(ctx->Viewport.Y)*/, amesa->clearpixel);
      }
      else
      {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_clear: front_rp->Clearing Area\n"));
#endif
        FillPixelArray (amesa->front_rp, FIXx(x), FIXy(y) - height, width, FIXy(y), amesa->clearpixel);
      }
      mask &= ~DD_FRONT_LEFT_BIT;
    }
    else
    {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_clear: Serious ERROR amesa->front_rp = NULL detected\n"));
#endif
    }
  }

  if ((mask & (DD_BACK_LEFT_BIT)))
  {  
    if(amesa->back_rp != NULL)
    {
      if(all)
      {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_clear: back_rp->Clearing viewport (ALL)\n"));
#endif
        FillPixelArray (amesa->back_rp, FIXx(ctx->Viewport.X), (FIXy(ctx->Viewport.Y) - ctx->Viewport.Height) + 1, /*FIXx(ctx->Viewport.X)+*/ctx->Viewport.Width - FIXx(ctx->Viewport.X), ctx->Viewport.Height/*FIXy(ctx->Viewport.Y)*/, amesa->clearpixel);
      }
      else
      {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_clear: back_rp->Clearing Area\n"));
#endif
        FillPixelArray (amesa->back_rp, FIXx(x), FIXy(y) - height, width, FIXy(y), amesa->clearpixel);
      }
      mask &= ~DD_BACK_LEFT_BIT;
    }
    else
    {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_clear: Serious ERROR amesa->back_rp = NULL detected\n"));
#endif
    }    
  }
  
  if (mask)
    _swrast_Clear( ctx, mask, all, x, y, width, height ); /* Remaining buffers to be cleared .. */
}

/**********************************************************************/
/*****         Span-based pixel drawing           *****/
/**********************************************************************/

/* Write a horizontal span of RGBA color pixels with a boolean mask. */
static void
arosTC_write_rgba_span3( const GLcontext *ctx, GLuint n, GLint x, GLint y,
                const GLubyte rgba[][4], const GLubyte mask[])
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i, drawbits_counter;
  ULONG *dp = NULL;
  struct RastPort *rp = amesa->rp;

  y = FIXy(y);
  x = FIXx(x);

#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_write_rgba_span3(count=%d,x=%d,y=%d)", n, x, y));
#endif

  if((dp = (ULONG*)amesa->imageline))
  {       /* if imageline allocated then use fastversion */
#ifdef DEBUGPRINT
D(bug("FAST "));
#endif
    if (mask)
    {
#ifdef DEBUGPRINT
D(bug("mask\n"));
#endif
      drawbits_counter = 0;
      for (i = 0; i < n; i++)       /* draw pixel (x[i],y[i]) */
      {
        if (mask[i])
        {
          /* Mask Bit Set, So Draw */
          drawbits_counter++;
          *dp = TC_ARGB(rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP], rgba[i][ACOMP]);
          dp++;
        }
        else
        {
          if(drawbits_counter)
          {
            WritePixelArray(amesa->imageline, 0, 0, 4 * drawbits_counter, rp, x, y, drawbits_counter, 1, AROS_PIXFMT);
/*D(bug("[AROSMESA:TC] arosTC_write_rgba_span3: WritePixelArray(ant=%d,x=%d,y=%d)\n", ant, x, y));*/
            dp = (ULONG*)amesa->imageline;
            x = x + drawbits_counter;
            drawbits_counter = 0;
          }
          else
          {
            x++;
            dp++;
          }
        }
      }

      if(drawbits_counter)
      {
        WritePixelArray(amesa->imageline, 0, 0, 4 * drawbits_counter, rp, x, y, drawbits_counter, 1, AROS_PIXFMT);
/*D(bug("[AROSMESA:TC] arosTC_write_rgba_span3: WritePixelArray(ant=%d,x=%d,y=%d)\n",ant,x,y));*/
      }
    }
    else
    {
#ifdef DEBUGPRINT
D(bug("nomask\n"));
#endif
      for (i = 0; i < n; i++)           /* draw pixel (x[i],y[i])*/
      {
        *dp = TC_ARGB(rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP], rgba[i][ACOMP]);
        dp++;
      }
      WritePixelArray(amesa->imageline, 0, 0, 4 * n, rp, x, y, n, 1, AROS_PIXFMT);
    }
  }
  else
  {  /* Slower version */
#ifdef DEBUGPRINT
D(bug("SLOW "));
#endif
    if (mask)
    {
#ifdef DEBUGPRINT
D(bug("mask\n"));
#endif
      /* draw some pixels */
      for (i = 0; i < n; i++, x++)
      {
        if (mask[i])
        {
          /* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
          WriteRGBPixel(rp, x, y, TC_ARGB(rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP], rgba[i][ACOMP]));
        }
      }
    }
    else
    {
#ifdef DEBUGPRINT
D(bug("nomask\n"));
#endif
      /* draw all pixels */
      for (i = 0; i < n; i++, x++)
      {
        /* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
        WriteRGBPixel(rp, x, y,TC_ARGB(rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP], rgba[i][ACOMP]));
      }
    }
  }
}

/* Write a horizontal span of RGB color pixels with a boolean mask. */
static void
arosTC_write_rgb_span3( const GLcontext *ctx, GLuint n, GLint x, GLint y,
                const GLubyte rgba[][3], const GLubyte mask[])
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i, drawbits_counter;
  ULONG *dp = NULL;
  struct RastPort *rp = amesa->rp;

  y = FIXy(y);
  x = FIXx(x);
  
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_write_rgb_span3(ant=%d,x=%d,y=%d)", n, x, y));
#endif

  if((dp = (ULONG*)amesa->imageline))
  {       /* if imageline allocated then use fastversion */
#ifdef DEBUGPRINT
D(bug("FAST "));
#endif
    if (mask)
    {
#ifdef DEBUGPRINT
D(bug("mask\n"));
#endif
      drawbits_counter = 0;
      for (i = 0; i < n; i++)       /* draw pixel (x[i],y[i]) */
      {
        if (mask[i])
        {
          drawbits_counter++;
          *dp = TC_ARGB(rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP], 0xff);
          dp++;
        }
        else
        {
          if(drawbits_counter)
          {
            WritePixelArray(amesa->imageline, 0, 0, 4 * drawbits_counter, rp, x, y, drawbits_counter, 1, AROS_PIXFMT);
/*D(bug("[AROSMESA:TC] WritePixelArray(ant=%d,x=%d,y=%d)\n",ant,x,y));*/
            dp = (ULONG*)amesa->imageline;
            x = x + drawbits_counter;
            drawbits_counter = 0;
          }
          else
          {
            x++;
            dp++;
          }
        }
      }

      if(drawbits_counter)
      {
        WritePixelArray(amesa->imageline, 0, 0, 4 * drawbits_counter, rp, x, y, drawbits_counter, 1, AROS_PIXFMT);
/*D(bug("[AROSMESA:TC] WritePixelArray(ant=%d,x=%d,y=%d)\n",ant,x,y));*/
      }
    }
    else
    {
#ifdef DEBUGPRINT
D(bug("nomask\n"));
#endif
      for (i = 0; i < n; i++)           /* draw pixel (x[i],y[i])*/
      {
        *dp = TC_ARGB(rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP],0xff);
        dp++;
      }
      WritePixelArray(amesa->imageline, 0, 0, 4 * n, rp, x, y, n, 1, AROS_PIXFMT);
    }
    
  }
  else
  {  /* Slower version */
#ifdef DEBUGPRINT
D(bug("SLOW "));
#endif
    if (mask)
    {
#ifdef DEBUGPRINT
D(bug("mask\n"));
#endif
      /* draw some pixels */
      for (i=0; i<n; i++, x++)
      {
        if (mask[i])
        {
          /* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
          WriteRGBPixel(rp, x, y, TC_ARGB(rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP], 0xff));
        }
      }
    }
    else
    {
#ifdef DEBUGPRINT
D(bug("nomask\n"));
#endif
      /* draw all pixels */
      for (i = 0; i < n; i++, x++)
      {
        /* draw pixel x,y using color red[i]/green[i]/blue[i]/alpha[i] */
        WriteRGBPixel(rp, x, y, TC_ARGB(rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP], 0xff));
      }
    }
  }
}

/*
* Write a horizontal span of pixels with a boolean mask.  The current color
* is used for all pixels.
*/

static void
arosTC_write_monocolor_span3( const GLcontext *ctx,
                    GLuint n, GLint x, GLint y,
                    const GLchan color[4], const GLubyte mask[])
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i, j;
  struct RastPort *rp = amesa->rp;

#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_write_monocolor_span3(n:%d,x:%d,y:%d)\n", n, x, y));
#endif

  y = FIXy(y);
  x = FIXx(x);

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
      j = 0;
      while(mask[i] && (i < n))
      {
        i++;
        j++;
      }
      FillPixelArray(rp, x, y, j, 1,  TC_ARGB(color[RCOMP],color[GCOMP],color[BCOMP],0x255));
    }
  }
}

/**********************************************************************/
/*****          Read spans of pixels                *****/
/**********************************************************************/

static void
MyReadPixelArray(UBYTE *a, ULONG b, ULONG c, ULONG d, struct RastPort *e,ULONG f, ULONG g,ULONG h, ULONG i,ULONG j)
{
  ReadPixelArray(a,b,c,d,e,f,g,h,i,j);
}

/* Read a horizontal span of color pixels. */
static void
arosTC_read_rgba_span3( const GLcontext* ctx,
              GLuint n, GLint x, GLint y,
              GLubyte rgba[][4] )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i;
  ULONG col;

#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_read_color_span()\n"));
#endif

  y = FIXy(y);
  x = FIXx(x);

  if(amesa->imageline)
  {
    MyReadPixelArray(amesa->imageline, 0, 0, n * 4, amesa->rp, x, y, n, 1, AROS_PIXFMT);
    for(i = 0; i < n; i++)
    {
      col = ((ULONG*)amesa->imageline)[i];
      rgba[i][RCOMP] = (col & 0xff0000) >> 16;
      rgba[i][GCOMP] = (col & 0xff00) >> 8;
      rgba[i][BCOMP] = col & 0xff;
      rgba[i][ACOMP] = 255;
    }
  } else
    for (i=0; i<n; i++, x++)
    {
      col = ReadRGBPixel(amesa->rp, x, y);
      rgba[i][RCOMP] = (col & 0xff0000) >> 16;
      rgba[i][GCOMP] = (col & 0xff00) >> 8;
      rgba[i][BCOMP] = col & 0xff;
      rgba[i][ACOMP] = 255;
    }
}

/**********************************************************************/
/*****           Array-based pixel drawing        *****/
/**********************************************************************/

/* Write an array of RGBA pixels with a boolean mask. */
static void
arosTC_write_rgba_pixels3( const GLcontext* ctx,
                 GLuint n, const GLint x[], const GLint y[],
                 const GLubyte rgba[][4], const GLubyte mask[] )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i;
  struct RastPort *rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_write_rgba_pixels3()\n"));
#endif

  for (i = 0; i < n; i++)
  {
    if (mask[i])
    {
/*    write pixel x[i], y[i] using red[i],green[i],blue[i],alpha[i] */
      WriteRGBPixel(rp, FIXx(x[i]), FIXy(y[i]), TC_ARGB(rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP], rgba[i][ACOMP]));
    }
  }
}

/*
* Write an array of pixels with a boolean mask.  The current color
* is used for all pixels.
*/
static void
arosTC_write_monocolor_pixels3( const GLcontext* ctx,
                  GLuint n,
                  const GLint x[], const GLint y[],
                  const GLchan color[4], const GLubyte mask[] )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i;
  struct RastPort *rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_write_monocolor_pixels3()\n"));
#endif

  for (i = 0; i < n; i++)
  {
    if (mask[i])
    {
/*    write pixel x[i], y[i] using current color*/
      WriteRGBPixel(rp, FIXx(x[i]), FIXy(y[i]), TC_ARGB(color[RCOMP],color[GCOMP],color[BCOMP],0x255));
    }
  }
}

/**********************************************************************/
/*****             Read arrays of pixels              *****/
/**********************************************************************/

/* Read an array of color pixels. */
static void
arosTC_read_rgba_pixels3( const GLcontext* ctx,
                GLuint n, const GLint x[], const GLint y[],
                GLubyte rgba[][4], const GLubyte mask[] )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i, col;
  struct RastPort *rp = amesa->rp;

/*  ULONG ColTab[3];*/
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_read_rgba_pixels3()\n"));
#endif

  for (i = 0; i < n; i++)
  {
    if (mask[i])
    {
      col = ReadRGBPixel(rp, FIXx(x[i]), FIXy(y[i]));

      rgba[i][RCOMP] = (col&0xff0000) >> 16;
      rgba[i][GCOMP] = (col&0xff00) >> 8;
      rgba[i][BCOMP] = col & 0xff;
      rgba[i][ACOMP] = 255;
    }
  }
}

/**********************************************************************/
/**********************************************************************/

static GLboolean
arosTC_set_draw_buffer( GLcontext *ctx, GLenum mode )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_set_draw_buffer(ctx @ %x)\n", ctx));
#endif
  if (mode == GL_FRONT_LEFT)
  {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_set_draw_buffer: FRONT BUFFER\n"));
#endif
    return(GL_TRUE);
  } else if ((mode == GL_BACK_LEFT)&&(amesa->visual->db_flag)) {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_set_draw_buffer: BACK BUFFER\n"));
#endif
    return(GL_TRUE);
  } else {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_set_draw_buffer: UNSUPPORTED BUFFER!\n"));
#endif
    return(GL_FALSE);
  }
}

static void
arosTC_set_read_buffer( GLcontext *ctx, GLframebuffer *buffer, GLenum mode )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_set_read_buffer(ctx @ %x) : TODO!\n", ctx));
#endif
  /* We dont support seperate read buffers */
  if (mode == GL_FRONT_LEFT)
  {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_set_read_buffer: FRONT BUFFER\n"));
#endif
  } else if ((mode == GL_BACK_LEFT)&&(amesa->visual->db_flag)) {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_set_read_buffer: BACK BUFFER\n"));
#endif
  } else {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_set_read_buffer: UNSUPPORTED BUFFER!\n"));
#endif
  }
}

static const GLubyte *
arosTC_renderer_string(GLcontext *ctx, GLenum name)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_renderer_string()\n"));
#endif
	(void) ctx;
	switch (name)
	{
	case GL_RENDERER:
   	return "AROS TrueColor Rasterizer";
   default:
      return NULL;
   }
}

/**********************************************************************/
/**********************************************************************/
  /* Initialize all the pointers in the DD struct.  Do this whenever   */
  /* a new context is made current or we change buffers via set_buffer! */

void
arosTC_register_swrast_functions(GLcontext *ctx)
{
  SWcontext *swrast = SWRAST_CONTEXT( ctx);
// swrast->choose_line = arosTC_choose_line;
// swrast->choose_triangle = arosTC_choose_triangle;

// swrast->invalidate_line |= AROSMESA_NEW_LINE;
// swrast->invalidate_triangle |= AROSMESA_NEW_TRIANGLE;
}

void
arosTC_update_state(GLcontext *ctx, GLuint new_state)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_update_state(ctx @ %x, state %d)\n", ctx, new_state));
#endif
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  struct swrast_device_driver *swdd = NULL;
  TNLcontext *tnl = NULL;

  _swrast_InvalidateState(ctx, new_state);
  _swsetup_InvalidateState(ctx, new_state);
  _ac_InvalidateState(ctx, new_state);
  _tnl_InvalidateState(ctx, new_state);

  ctx->Driver.GetString = arosTC_renderer_string;
  ctx->Driver.GetBufferSize = arosTC_buffer_size;
  ctx->Driver.Flush = arosTC_flush;
  ctx->Driver.Finish = arosTC_finish;

  ctx->Driver.UpdateState = arosTC_update_state;

/* Software Rasterizer pixel paths */

  ctx->Driver.Accum = _swrast_Accum;
  ctx->Driver.Bitmap = _swrast_Bitmap;
  ctx->Driver.Clear = arosTC_clear;
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
D(bug("[AROSMESA:TC] arosTC_update_state: SWDD @ %x\n", swdd));
#endif
/* RGB/RGBA functions: */
  swdd->WriteRGBASpan       = arosTC_write_rgba_span3;
  swdd->WriteRGBSpan        = arosTC_write_rgb_span3;
  swdd->WriteRGBAPixels     = arosTC_write_rgba_pixels3;
  swdd->WriteMonoRGBASpan   = arosTC_write_monocolor_span3;
  swdd->WriteMonoRGBAPixels = arosTC_write_monocolor_pixels3;
  swdd->ReadRGBASpan       = arosTC_read_rgba_span3;
  swdd->ReadRGBAPixels     = arosTC_read_rgba_pixels3;

/* Statechange callbacks */
  ctx->Driver.SetDrawBuffer = arosTC_set_draw_buffer;
  swdd->SetReadBuffer = arosTC_set_read_buffer;
  ctx->Driver.ClearColor = arosTC_clear_color;

/* TNL Driver Interface */
  tnl = TNL_CONTEXT(ctx);
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_update_state: TNL @ %x\n", tnl));
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
/*****          AROS/Mesa private misc procedures    *****/
/**********************************************************************/

static void
arosTC_buffer_size( GLcontext *ctx, GLuint *width, GLuint *height)
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_buffer_size(amesa @ %x)\n", amesa));
#endif

  *width = amesa->width;
  *height = amesa->height;

  if(!(( amesa->width  == (amesa->front_rp->Layer->bounds.MaxX - amesa->front_rp->Layer->bounds.MinX - amesa->left-amesa->right) )
  && ( amesa->height == (amesa->front_rp->Layer->bounds.MaxY - amesa->front_rp->Layer->bounds.MinY - amesa->bottom-amesa->top) )))
  {
    arosRasterizer_FreeOneLine(amesa);

    amesa->RealWidth = amesa->front_rp->Layer->bounds.MaxX - amesa->front_rp->Layer->bounds.MinX;
    amesa->FixedWidth = amesa->RealWidth;

    amesa->RealHeight= amesa->front_rp->Layer->bounds.MaxY - amesa->front_rp->Layer->bounds.MinY;
    amesa->FixedHeight = amesa->RealHeight;

    amesa->width = amesa->RealWidth - amesa->left-amesa->right;
    *width = amesa->width;

    amesa->height = amesa->RealHeight - amesa->bottom-amesa->top;
    *height = amesa->height;
    
    amesa->depth = GetCyberMapAttr(amesa->front_rp->BitMap, CYBRMATTR_DEPTH);

    if (amesa->visual->db_flag)
    {
      if (amesa->back_rp)
      {
        arosRasterizer_destroy_rastport(amesa->back_rp); /* Free double buffer */
      }

      if((amesa->back_rp = arosRasterizer_make_rastport(amesa->RealWidth, amesa->RealHeight, amesa->depth, amesa->front_rp->BitMap)) == NULL)
      {
        if (amesa->front_rp == NULL)
        {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_buffer_size: ERROR - Illegal state!!\n"));
#endif
          return;
        }
        amesa->rp = amesa->front_rp;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_buffer_size: ERROR - Failed to allocate BackBuffer rastport in specified size.\n"));
#endif
      }
      else
      {
        amesa->rp = amesa->back_rp;
      }
    }

    arosRasterizer_AllocOneLine(amesa);
  }

#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_buffer_size(ctx=%x) = w:%d x h:%d\n", ctx, *width, *height));
#endif
    
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_buffer_size:    amesa->RealWidth =%d\n", amesa->RealWidth));
D(bug("[AROSMESA:TC] arosTC_buffer_size:    amesa->RealHeight=%d\n", amesa->RealHeight));
D(bug("[AROSMESA:TC] arosTC_buffer_size:    amesa->width =%d\n", amesa->width));
D(bug("[AROSMESA:TC] arosTC_buffer_size:    amesa->height=%d\n", amesa->height));
D(bug("[AROSMESA:TC] arosTC_buffer_size:    amesa->left  =%d\n", amesa->left));
D(bug("[AROSMESA:TC] arosTC_buffer_size:    amesa->bottom=%d\n", amesa->bottom));
D(bug("[AROSMESA:TC] arosTC_buffer_size:    amesa->depth =%d\n", amesa->depth));
#endif
}

/**********************************************************************/
/*****          AROS/Mesa private init/despose/resize     *****/
/**********************************************************************/

BOOL
arosTC_Standard_init(AROSMesaContext amesa, struct TagItem *tagList)
{
	struct Rectangle rastcliprect;
	struct TagItem crptags[] =
	{
	  { RPTAG_ClipRectangle      , (IPTR)&rastcliprect },
	  { RPTAG_ClipRectangleFlags , (RPCRF_RELRIGHT | RPCRF_RELBOTTOM) },
	  { TAG_DONE }
	};

#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_Standard_init(amesa @ %x, taglist @ %x)\n", amesa, tagList));
#endif

  if (!(amesa->rp))
  {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_Standard_init: WARNING - NULL RastPort in context\n"));
#endif
    if (!(amesa->rp = (struct RastPort *)GetTagData(AMA_RastPort, 0, tagList)))
    {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_Standard_init: ERROR - Launched with NULL RastPort\n"));
#endif
      return FALSE;
    }
  }	  
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_Standard_init: Using RastPort @ %x\n", amesa->rp));
#endif

  amesa->rp = CloneRastPort(amesa->rp);

#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_Standard_init: Cloned RastPort @ %x\n", amesa->rp));
#endif

  amesa->RealWidth = amesa->rp->Layer->bounds.MaxX - amesa->rp->Layer->bounds.MinX;
  amesa->FixedWidth = amesa->RealWidth;
  
  amesa->RealHeight = amesa->rp->Layer->bounds.MaxY - amesa->rp->Layer->bounds.MinY;
  amesa->FixedHeight = amesa->RealHeight;

  amesa->left = GetTagData(AMA_Left, 0, tagList);
  amesa->right = GetTagData(AMA_Right, 0, tagList);
  amesa->top = GetTagData(AMA_Top, 0, tagList);
  amesa->bottom = GetTagData(AMA_Bottom, 0, tagList);

  amesa->front_rp = amesa->rp;
  amesa->back_rp = NULL;

  amesa->width = GetTagData(AMA_Width, (amesa->RealWidth - amesa->left - amesa->right), tagList);
  amesa->height = GetTagData(AMA_Height, (amesa->RealHeight - amesa->top - amesa->bottom), tagList);

  amesa->pixel = 0;        /* current drawing/clearing pens */
  amesa->clearpixel = 0;   /* current drawing/clearing pens */

  if (amesa->window)
  {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_Standard_init: Clipping Rastport to Window's dimensions\n"));
#endif
    rastcliprect.MinX = amesa->window->BorderLeft;	 /* AROS: Clip the rastport to the visible area */
    rastcliprect.MinY = amesa->window->BorderTop;
    rastcliprect.MaxX = -amesa->window->BorderRight;
    rastcliprect.MaxY = -amesa->window->BorderBottom;
    SetRPAttrsA(amesa->rp, crptags);
  }

#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_Standard_init: Context Base dimensions set -:\n"));
D(bug("[AROSMESA:TC] arosTC_Standard_init:    amesa->RealWidth  = %d\n", amesa->RealWidth));
D(bug("[AROSMESA:TC] arosTC_Standard_init:    amesa->RealHeight = %d\n", amesa->RealHeight));
D(bug("[AROSMESA:TC] arosTC_Standard_init:    amesa->width      = %d\n", amesa->width));
D(bug("[AROSMESA:TC] arosTC_Standard_init:    amesa->height     = %d\n", amesa->height));
D(bug("[AROSMESA:TC] arosTC_Standard_init:    amesa->left       = %d\n", amesa->left));
D(bug("[AROSMESA:TC] arosTC_Standard_init:    amesa->right      = %d\n", amesa->right));
D(bug("[AROSMESA:TC] arosTC_Standard_init:    amesa->top        = %d\n", amesa->top));
D(bug("[AROSMESA:TC] arosTC_Standard_init:    amesa->bottom     = %d\n", amesa->bottom));
D(bug("[AROSMESA:TC] arosTC_Standard_init:    amesa->depth      = %d\n", amesa->depth));
#endif

  if (amesa->Screen)
  {
    if (amesa->depth == 0)
    {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_Standard_init: WARNING - Illegal RastPort Depth, attempting to correct\n"));
#endif
       amesa->depth = GetCyberMapAttr(amesa->rp->BitMap, CYBRMATTR_DEPTH);
    }

    if (amesa->depth <= 8)
    {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_Standard_init: Allocating CMap : "));
#endif
       AllocCMap(amesa->Screen);
#ifdef DEBUGPRINT
D(bug("done!\n"));
#endif
    }
  }

  if (amesa->visual->db_flag == GL_TRUE)
  {
    if((amesa->back_rp = arosRasterizer_make_rastport(amesa->RealWidth, amesa->RealHeight, amesa->depth, amesa->rp->BitMap)) != NULL)
    {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_Standard_init: Allocated BackBuffer RastPort (DOUBLEBUFFER)\n"));
#endif
      ((GLcontext *)amesa->gl_ctx)->Color.DrawBuffer = GL_BACK;
      amesa->rp = amesa->back_rp;
    }
    else
    {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_Standard_init: ERROR - make_rastport FAILED!\n"));
#endif
      ((GLcontext *)amesa->gl_ctx)->Color.DrawBuffer = GL_FRONT;
    }
  }
  else
  {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_Standard_init: Using SINGLE RastPort Buffer\n"));
#endif
    ((GLcontext *)amesa->gl_ctx)->Color.DrawBuffer = GL_FRONT;
  }

  arosRasterizer_AllocOneLine(amesa); /* A linebuffer for WritePixelLine */

#ifdef ADISP_AGA
  amesa->InitDD = amesa->depth <= 8 ? aros8bit_update_state : arosTC_update_state;  /*standard drawing*/
#else
  amesa->InitDD = arosTC_update_state;
#endif

  amesa->Dispose = arosRasterizer_Standard_Dispose;
  amesa->SwapBuffer = arosRasterizer_Standard_SwapBuffer;

  return TRUE;
}

/* Disabled for now .. (nicja)
#ifdef AMESA_DOUBLEBUFFFAST
#include "AROSMesa_TC_db.c"
#endif*/
