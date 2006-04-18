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
#include "../context.h"
#include "../dd.h"
#include "../xform.h"
#include "../macros.h"
#include "../vb.h"

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
arosTC_finish( void )
{
  /* implements glFinish if possible */
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] ararosTC_finish() : Unimplemented - glFinish()\n"));
#endif
}

static void
arosTC_flush( void )
{
  /* implements glFlush if possible */
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] ararosTC_flush() : Unimplemented - glFlush()\n"));
#endif
}

/* implements glClearIndex - Set the color index used to clear the color buffer. */
static void
arosTC_clear_index( GLcontext *ctx, GLuint index )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  amesa->clearpixel = amesa->penconv[index];
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_clear_index(c=%x,i=%d) = %x\n", ctx, index, amesa->clearpixel));
#endif
}

/* implements glClearColor - Set the color used to clear the color buffer. */
static void
arosTC_clear_color( GLcontext *ctx,
              GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
#warning "TODO: Free pen color if not used"
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  amesa->clearpixel = TC_ARGB(r, g, b, a);
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_clear_color(c=%x,r=%d,g=%d,b=%d,a=%d) = %x\n",ctx, r, g, b, a, amesa->clearpixel));
#endif
}

/*
* Clear the specified region of the color buffer using the clear color
* or index as specified by one of the two functions above.
*/
static GLbitfield
arosTC_clear(GLcontext* ctx, GLbitfield mask,
                GLboolean all, GLint x, GLint y, GLint width, GLint height)
{
  /*
  * If all==GL_TRUE, clear whole buffer
  */
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_clear(all:%d,x:%d,y:%d,w:%d,h:%d)\n", all, x, y, width, height));
#endif

  if(amesa->rp != NULL)
  {
    if(all)
    {
D(bug("[AROSMESA:TC] arosTC_clear: Clearing viewport (ALL)\n"));
      FillPixelArray (amesa->rp, FIXx(ctx->Viewport.X), (FIXy(ctx->Viewport.Y) - ctx->Viewport.Height) + 1, /*FIXx(ctx->Viewport.X)+*/ctx->Viewport.Width - FIXx(ctx->Viewport.X), ctx->Viewport.Height/*FIXy(ctx->Viewport.Y)*/, amesa->clearpixel);
    }
    else
    {
D(bug("[AROSMESA:TC] arosTC_clear: Clearing Area\n"));
      FillPixelArray (amesa->rp, FIXx(x), FIXy(y) - height, width, FIXy(y), amesa->clearpixel);
    }
  }
  else
  {
D(bug("[AROSMESA:TC] arosTC_clear: Serious ERROR amesa->rp = NULL detected\n"));
  }

  return mask; /* Return mask of buffers remaining to be cleared */
}

/* Set the current color index. */
static void
arosTC_set_index( GLcontext *ctx, GLuint index )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;

  /* Set the amesa color index. */
  amesa->pixel = amesa->penconv[index];
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_set_index(c=%x,i=%d) = %x\n", ctx, index,amesa->pixel));
#endif
}

static void
arosTC_set_color( GLcontext *ctx,
                  GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;

  /* Set the current RGBA color. */
  /* r is in 0..255.RedScale */
  /* g is in 0..255.GreenScale */
  /* b is in 0..255.BlueScale */
  /* a is in 0..255.AlphaScale */
  amesa->pixel = TC_ARGB(r,g,b,a);
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_set_color(c=%x,r=%d,g=%d,b=%d,a=%d) = %x\n", ctx, r, g, b, a, amesa->pixel));
#endif
}

/* Set the index mode bitplane mask. */
static GLboolean
arosTC_index_mask( GLcontext *ctx,GLuint mask )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;

  /* implement glIndexMask if possible, else return GL_FALSE */
D(bug("[AROSMESA:TC] arosTC_index_mask(%x)\n", mask));
  //amesa->rp->Mask = (UBYTE)mask;

  return (GL_FALSE);
}

/* Set the RGBA drawing mask. */
static GLboolean
arosTC_color_mask( GLcontext *ctx,GLboolean rmask, GLboolean gmask,
                    GLboolean bmask, GLboolean amask)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_color_mask()\n"));
#endif
  /* implement glColorMask if possible, else return GL_FALSE */
  return (GL_FALSE);
}

/**********************************************************************/
/*****      Accelerated point, line, polygon rendering    *****/
/**********************************************************************/

/*
 *  Render a number of points by some hardware/OS accerated method
 */

/*
static void arosTC_fast_points_function(GLcontext *ctx,GLuint first, GLuint last )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  struct vertex_buffer *VB = ctx->VB;
  int i, col;
  struct RastPort *rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_fast_points_function(f:%d, l:%d)\n", first, last));
#endif

  if (VB->MonoColor) {
    // draw all points using the current color (set_color)
//    D(bug("[AROSMESA:TC] VB->MonoColor\n"));
    for (i = first ; i <= last ; i++)
    {
      if (VB->ClipMask[i] == 0)
      {
        // compute window coordinate
        int x, y;
        x = FIXx((GLint) (VB->Win[i][0]));
        y = FIXy((GLint) (VB->Win[i][1]));
        WriteRGBPixel(rp, x, y, amesa->pixel);
//    printf("WriteRGBPixel(%d,%d)\n", x, y);
      }
    }
  } else {
    // each point is a different color 
//    D(bug("[AROSMESA:TC] !VB.MonoColor\n"));

    for (i = first ; i <= last ; i++)
    {
      if ((VB->ClipMask[i] == 0))
      {
        int x, y;
        x = FIXx((GLint) (VB->Win[i][0]));
        y = FIXy((GLint) (VB->Win[i][1]));
        col =* VB->Color[i];
        WriteRGBPixel(rp, x, y, amesa->penconv[col]);
//    D(bug("[AROSMESA:TC] WriteRGBPixel(%d,%d)\n", x, y));
      }
    }
  }
} */

/*
static points_func arosTC_choose_points_function( GLcontext *ctx )
{
//D(bug("[AROSMESA:TC] arosTC_choose_points_function\n"));
  // Examine the current rendering state and return a pointer to a
  // fast point-rendering function if possible.
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] ararosTC_choose_points_function()\n"));
#endif
  if ((ctx->Point.Size == 1.0)
     && (!(ctx->Point.SmoothFlag))
     && (ctx->RasterMask == 0)
     && (!(ctx->Texture.Enabled))) //&&  ETC, ETC
  {
    return arosTC_fast_points_function;
  }
  else {
    return NULL;
  }
} */


/*
*  Render a line by some hardware/OS accerated method 
*/

static line_func
arosTC_choose_line_function( GLcontext *ctx)
{
D(bug("[AROSMESA:TC] arosTC_choose_line_function()\n"));

  /* Examine the current rendering state and return a pointer to a */
  /* fast line-rendering function if possible. */

  return NULL;
}

/**********************************************************************/
/*****          Optimized polygon rendering         *****/
/**********************************************************************/

/*
 * Draw a filled polygon of a single color. If there is hardware/OS support
 * for polygon drawing use that here.   Otherwise, call a function in
 * polygon.c to do the drawing.
 */

#warning "TODO: removed next [obsolete] function to compile on 2.6"
/*static polygon_func arosTC_choose_polygon_function( GLcontext *ctx )
{*/
  /* D(bug("[AROSMESA:TC] arosTC_choose_polygon_function\n"));*/

  /* Examine the current rendering state and return a pointer to a */
  /* fast polygon-rendering function if possible. */

/*  return NULL;
}*/

/**********************************************************************/
/*****         Span-based pixel drawing           *****/
/**********************************************************************/

/* Write a horizontal span of 32-bit color-index pixels with a boolean mask. */
static void
arosTC_write_ci32_span(const GLcontext *ctx,
                GLuint n, GLint x, GLint y,
                const GLuint index[],
                const GLubyte mask[] )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i, drawbits_counter;
  ULONG *dp = NULL;
  unsigned long *penconv = amesa->penconv;
  struct RastPort *rp = amesa->rp;

#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_write_ci32_span(n:%d,x:%d,y:%d)\n", n, x, y));
#endif
  
  y = FIXy(y);
  x = FIXx(x);
  if((dp = (ULONG*)amesa->imageline))
  {           /* if imageline has been
            allocated then use fastversion */

    drawbits_counter = 0;
    for (i = 0 ; i<n ; i++)
    { /* draw pixel (x[i],y[i]) using index[i] */
      if (mask[i])
      {
        drawbits_counter++;
/*        x++;*/
        *dp = penconv[index[i]];
        dp++;
      } else {
        if(drawbits_counter)
        {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_write_ci32_span: WritePixelArray()\n"));
#endif
          WritePixelArray(amesa->imageline, 0, 0, 4 * drawbits_counter, rp, x, y, drawbits_counter, 1, RECTFMT_ARGB);
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
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_write_ci32_span: WritePixelArray()\n"));
#endif
      WritePixelArray(amesa->imageline, 0, 0, 4 * drawbits_counter, rp, x, y, drawbits_counter, 1, RECTFMT_ARGB);
    }

  } else {      /* Slower */
    for (i = 0 ; i < n ; i++)
    {
      if (mask[i])
      {
        /* draw pixel (x[i],y[i]) using index[i] */
        WriteRGBPixel(rp, x + i, y, penconv[index[i]]);
      }
    }
  }
}

/* Write a horizontal span of 8-bit color-index pixels with a boolean mask. */
static void
arosTC_write_ci8_span(const GLcontext *ctx,
                GLuint n, GLint x, GLint y,
                const GLubyte index[],
                const GLubyte mask[] )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i, drawbits_counter;
  ULONG *dp = NULL;
  unsigned long *penconv = amesa->penconv;
  struct RastPort *rp = amesa->rp;

#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_write_ci8_span(n:%d,x:%d,y:%d)\n",n,x,y));
#endif

  y = FIXy(y);
  x = FIXx(x);
  if((dp = (ULONG*)amesa->imageline))
  {           /* if imageline have been
            allocated then use fastversion */
    drawbits_counter = 0;
    for (i = 0 ; i < n ; i++)
    { /* draw pixel (x[i],y[i]) using index[i] */
      if (mask[i])
      {
        drawbits_counter++;
/*        x++;*/
        *dp = penconv[index[i]];
        dp++;
      } else {
        if(drawbits_counter)
        {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_write_ci8_span: WritePixelArray()\n"));
#endif
          WritePixelArray(amesa->imageline, 0, 0, 4 * drawbits_counter, rp, x, y, drawbits_counter, 1, RECTFMT_ARGB);
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
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_write_ci8_span: WritePixelArray()\n"));
#endif
      WritePixelArray(amesa->imageline, 0, 0, 4 * drawbits_counter, rp, x, y, drawbits_counter, 1, RECTFMT_ARGB);
    }

  } else {      /* Slower */
    for (i = 0 ; i < n ; i++)
    {
      if (mask[i])
      {
        /* draw pixel (x[i],y[i]) using index[i] */
        WriteRGBPixel(rp, x + i, y, penconv[index[i]]);
      }
    }
  }
}

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
            WritePixelArray(amesa->imageline, 0, 0, 4 * drawbits_counter, rp, x, y, drawbits_counter, 1, RECTFMT_ARGB);
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
        WritePixelArray(amesa->imageline, 0, 0, 4 * drawbits_counter, rp, x, y, drawbits_counter, 1, RECTFMT_ARGB);
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
      WritePixelArray(amesa->imageline, 0, 0, 4 * n, rp, x, y, n, 1, RECTFMT_ARGB);
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
            WritePixelArray(amesa->imageline, 0, 0, 4 * drawbits_counter, rp, x, y, drawbits_counter, 1, RECTFMT_ARGB);
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
        WritePixelArray(amesa->imageline, 0, 0, 4 * drawbits_counter, rp, x, y, drawbits_counter, 1, RECTFMT_ARGB);
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
      WritePixelArray(amesa->imageline, 0, 0, 4 * n, rp, x, y, n, 1, RECTFMT_ARGB);
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
* Write a horizontal span of pixels with a boolean mask.  The current
* color index is used for all pixels.
*/
static void
arosTC_write_mono_ci_span(const GLcontext* ctx,
                 GLuint n,GLint x,GLint y,
                 const GLubyte mask[])
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i, j;
  y = FIXy(y);
  x = FIXx(x);

#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_write_mono_ci_span()\n"));
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
      j = 0;
      while(mask[i] && (i < n))
      {
        i++;
        j++;
      }
      FillPixelArray (amesa->rp, x, y, j, 1, amesa->pixel);
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
                    const GLubyte mask[])
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
      FillPixelArray(rp, x, y, j, 1, amesa->pixel);
    }
  }
}

/**********************************************************************/
/*****          Read spans of pixels                *****/
/**********************************************************************/

/* Read a horizontal span of color-index pixels. */
static void
arosTC_read_ci32_span( const GLcontext* ctx, GLuint n, GLint x, GLint y,
              GLuint index[])
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i;

#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_read_ci32_span()\n"));
#endif

  y = FIXy(y);
  x = FIXx(x);

  /* Currently this is wrong ARGB-values are NOT indexes !!!*/
  if(amesa->imageline)
  {
    ReadPixelArray(amesa->imageline, 0, 0, 4 * n, amesa->rp, x, y, n, 1, RECTFMT_ARGB);
    for(i = 0; i < n; i++)
    {
      index[i] = ((ULONG*)amesa->imageline)[i];
    }
  } else {
    for (i = 0; i < n; i++, x++)
    {
      index[i] = ReadRGBPixel(amesa->rp, x, y);
    }
  }
}

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
    MyReadPixelArray(amesa->imageline, 0, 0, n * 4, amesa->rp, x, y, n, 1, RECTFMT_ARGB);
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

/* Write an array of 32-bit index pixels with a boolean mask. */
static void
arosTC_write_ci32_pixels( const GLcontext* ctx,
                 GLuint n, const GLint x[], const GLint y[],
                 const GLuint index[], const GLubyte mask[] )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i;
  struct RastPort *rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_write_ci32_pixels()\n"));
#endif

  for (i = 0; i < n; i++)
  {
    if (mask[i])
    {
      WriteRGBPixel(rp, FIXx(x[i]), FIXy(y[i]), amesa->pixel);
    }
  }
}

/*
* Write an array of pixels with a boolean mask.  The current color
* index is used for all pixels.
*/
static void
arosTC_write_mono_ci_pixels( const GLcontext* ctx,
                  GLuint n,
                  const GLint x[], const GLint y[],
                  const GLubyte mask[] )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i;
  struct RastPort *rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_write_mono_ci_pixels()\n"));
#endif

  for (i = 0; i < n; i++)
  {
    if (mask[i])
    {
/*    write pixel x[i], y[i] using current index  */
      WriteRGBPixel(rp, FIXx(x[i]), FIXy(y[i]), amesa->pixel);
    }
  }
}

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
                  const GLubyte mask[] )
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
      WriteRGBPixel(rp, FIXx(x[i]), FIXy(y[i]), amesa->pixel);
    }
  }
}

/**********************************************************************/
/*****             Read arrays of pixels              *****/
/**********************************************************************/

/* Read an array of color index pixels. */
static void
arosTC_read_ci32_pixels( const GLcontext* ctx,
                GLuint n, const GLint x[], const GLint y[],
                GLuint index[], const GLubyte mask[] )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i;
  struct RastPort *rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_read_ci32_pixels()\n"));
#endif

  for (i = 0; i < n; i++)
  {
    if (mask[i])
    {
/*     index[i] = read_pixel x[i], y[i] */
      index[i] = ReadRGBPixel(rp, FIXx(x[i]), FIXy(y[i])); /* that's wrong! we get ARGB!*/
    }
  }
}

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
arosTC_logicop( GLcontext* ctx, GLenum op )
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_logicop()\n"));
#endif
  /* Let mesa perform the op in software */
  return GL_FALSE;
}

static void
arosTC_dither( GLcontext* ctx, GLboolean enable )
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_dither()\n"));
#endif
  /* enable/disable dithering - currently unsuported */
}

static GLboolean
arosTC_set_draw_buffer( GLcontext *ctx, GLenum mode )
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_set_draw_buffer()\n"));
#endif
  if (mode == GL_FRONT_LEFT)
  {
    return(GL_TRUE);
  } else {
    return(GL_FALSE);
  }
}

static void
arosTC_set_read_buffer( GLcontext *ctx, GLframebuffer *buffer, GLenum mode )
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_set_read_buffer()\n"));
#endif
  /* We dont support seperate read buffers */
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
arosTC_update_state(GLcontext *ctx)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] arosTC_update_state(ctx @ %x)\n", ctx));
#endif
  ctx->Driver.GetString = arosTC_renderer_string;
  ctx->Driver.UpdateState = arosTC_update_state;
//  ctx->Driver.Finish = arosTC_finish;
//  ctx->Driver.Flush = arosTC_flush;

  ctx->Driver.SetDrawBuffer = arosTC_set_draw_buffer;
  ctx->Driver.SetReadBuffer = arosTC_set_read_buffer;
  ctx->Driver.Color = arosTC_set_color;
  ctx->Driver.Index = arosTC_set_index;
  ctx->Driver.ClearIndex = arosTC_clear_index;
  ctx->Driver.ClearColor = arosTC_clear_color;
  ctx->Driver.Clear = arosTC_clear;

  ctx->Driver.GetBufferSize = arosTC_buffer_size;
  
//  ctx->Driver.IndexMask = arosTC_index_mask;
//  ctx->Driver.ColorMask = arosTC_color_mask;

//  ctx->Driver.LogicOp = arosTC_logicop;
//  ctx->Driver.Dither = arosTC_dither;

 
  ctx->Driver.PointsFunc = NULL;
  ctx->Driver.LineFunc = arosTC_choose_line_function( ctx );
  ctx->Driver.TriangleFunc = NULL;
  
  /* RGB/RGBA functions: */
  ctx->Driver.WriteRGBASpan       = arosTC_write_rgba_span3;
  ctx->Driver.WriteRGBSpan        = arosTC_write_rgb_span3;
  ctx->Driver.WriteRGBAPixels     = arosTC_write_rgba_pixels3;
  ctx->Driver.WriteMonoRGBASpan   = arosTC_write_monocolor_span3;
  ctx->Driver.WriteMonoRGBAPixels = arosTC_write_monocolor_pixels3;
  ctx->Driver.ReadRGBASpan       = arosTC_read_rgba_span3;
  ctx->Driver.ReadRGBAPixels     = arosTC_read_rgba_pixels3;

  /* Indexed Color functions: */
  ctx->Driver.WriteCI32Span       = arosTC_write_ci32_span;
  ctx->Driver.WriteCI8Span        = arosTC_write_ci8_span;
  ctx->Driver.WriteMonoCISpan     = arosTC_write_mono_ci_span;
  ctx->Driver.WriteCI32Pixels     = arosTC_write_ci32_pixels;
  ctx->Driver.WriteMonoCIPixels   = arosTC_write_mono_ci_pixels;
  ctx->Driver.ReadCI32Span       = arosTC_read_ci32_span;
  ctx->Driver.ReadCI32Pixels     = arosTC_read_ci32_pixels;
}

/**********************************************************************/
/*****          AROS/Mesa private misc procedures    *****/
/**********************************************************************/

static void
MyWritePixelArray(void *a, int b, int c, int d, struct RastPort *e, int f, int g, int h, int i, int j)
{
  WritePixelArray(a, b, c, d, e, f, g, h, i, j);
}


static void
arosTC_buffer_size( GLcontext *ctx, GLuint *width, GLuint *height)
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;

D(bug("[AROSMESA:TC] arosTC_buffer_size(amesa @ %x)\n", amesa));

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
D(bug("[AROSMESA:TC] arosTC_buffer_size: ERROR - Illegal state!!\n"));
          return;
        }
        amesa->rp = amesa->front_rp;
D(bug("[AROSMESA:TC] arosTC_buffer_size: ERROR - Failed to allocate BackBuffer rastport in specified size.\n"));
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
