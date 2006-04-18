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
#ifdef __GNUC__
#ifndef __AROS__
#include <inline/exec.h>
#include <inline/intuition.h>
#include <inline/graphics.h>
#include <inline/utility.h>
#else
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#endif
#else
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>
#include "context.h"
#include "dd.h"
#include "xform.h"
#include "macros.h"
#include "vb.h"

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
aros8bit_finish( void )
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_finish()\n"));
#endif
  /* implements glFinish if possible */
}

static void
aros8bit_flush( void )
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_flush()\n"));
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
aros8bit_clear_color( GLcontext *ctx,
                GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
  /* implement glClearColor */
  /* color components are floats in [0,1] */
  /* usually just save the value in the context struct */
  /*printf("aros8bit_clear_color=glClearColor(%d,%d,%d,%d)\n",r,g,b,a);*/
  /* @@@ TODO FREE COLOR IF NOT USED */
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_clear_color()\n"));
#endif
  amesa->clearpixel = RGBA(amesa, r, g, b, a);
/*  amesa->clearpixel=RGBA(r,g,b,a);  Orginal */  

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

  SetAPen(amesa->rp, amesa->clearpixel);
  if(all)
  {
    RectFill(amesa->rp, FIXx(ctx->Viewport.X), (FIXy(ctx->Viewport.Y) - ctx->Viewport.Height) + 1, (FIXx(ctx->Viewport.X) + ctx->Viewport.Width) - 1, FIXy(ctx->Viewport.Y));
    if (amesa->visual->rgb_flag)
    {
/*
      int I;
      for(I=0;I<=255;I++) */
      /* Dealocate pens is in RGB mode */
      /*
      {
        while (amesa->mypen[I]!=0)      */
        /* TODO This may free some others pen also */
        /*
        {
          amesa->mypen[I]-=1;
          ReleasePen(amesa->Screen->ViewPort.ColorMap,I);
        }
      }
*/
    }
  }
  else
  {
    if(amesa->rp != 0)
    {
/*    printf("RectFill(%x,%d,%d,%d,%d)\n",amesa->rp,FIXx(x),FIXy(y)-height,width,FIXy(y));*/
      RectFill(amesa->rp, FIXx(x), (FIXy(y) - height), width, FIXy(y));
    }
    else
    {
      printf("Serious error amesa->rp=0 detected in aros8bit_clear() in file AROSMesa_8bit.c\n");
    }
  }
}

static void
aros8bit_set_index( GLcontext *ctx,GLuint index )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_set_index()\n"));
#endif
  /* Set the amesa color index. */
/*printf("aros8bit_set_index(%d)\n",index);*/
  amesa->pixel = amesa->penconv[index];
}

static void
aros8bit_set_color( GLcontext *ctx,GLubyte r, GLubyte g, GLubyte b, GLubyte a )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_set_color()\n"));
#endif
/*printf("aros8bit_set_color(%d,%d,%d,%d)\n",r,g,b,a);*/

  /* Set the current RGBA color. */
  /* r is in 0..255.RedScale */
  /* g is in 0..255.GreenScale */
  /* b is in 0..255.BlueScale */
  /* a is in 0..255.AlphaScale */
  amesa->pixel = RGBA(amesa, r, g, b, a);
  /*(a << 24) | (r << 16) | (g << 8) | b;*/
}

static GLboolean
aros8bit_index_mask( GLcontext *ctx,GLuint mask )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_index_mask()\n"));
#endif
  /* implement glIndexMask if possible, else return GL_FALSE */
/*printf("aros8bit_index_mask(%x)\n",mask);*/
  amesa->rp->Mask = (UBYTE)mask;

  return(GL_TRUE);
}

static GLboolean
aros8bit_color_mask( GLcontext *ctx,GLboolean rmask, GLboolean gmask,
                    GLboolean bmask, GLboolean amask)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_color_mask()\n"));
#endif
  /* implement glColorMask if possible, else return GL_FALSE */
  return(GL_FALSE);

}

/**********************************************************************/
/*****      Accelerated point, line, polygon rendering    *****/
/**********************************************************************/

/*
 *  Render a number of points by some hardware/OS accerated method
 */
/*
static void
aros8bit_points_function(GLcontext *ctx,GLuint first, GLuint last )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  struct vertex_buffer *VB = ctx->VB;

  int i, col;
  struct RastPort *rp = amesa->rp;
// printf("aros8bit_points_function\n");

  if (VB->MonoColor) {
    // draw all points using the current color (set_color)
    SetAPen(rp, amesa->pixel);
//    printf("VB->MonoColor\n");
    for (i = first; i <= last; i++)
    {
      if (VB->ClipMask[i] == 0)
      {
        // compute window coordinate
        int x, y;
        x = FIXx((GLint) (VB->Win[i][0]));
        y = FIXy((GLint) (VB->Win[i][1]));
        WritePixel(rp, x, y);
//    printf("WritePixel(%d,%d)\n",x,y);
      }
    }
  } else {
    // each point is a different color
//    printf("!VB.MonoColor\n");

    for (i = first; i <= last; i++)
    {
      if ((VB->ClipMask[i] == 0))
      {
        int x, y;
        x = FIXx((GLint) (VB->Win[i][0]));
        y = FIXy((GLint) (VB->Win[i][1]));
        col =* VB->Color[i];
        SetAPen(rp, amesa->penconv[col]);
        WritePixel(rp, x, y);
//    printf("WritePixel(%d,%d)\n",x,y);
      }
    }
  }
} */

/*
static points_func
aros8bit_choose_points_function( GLcontext *ctx )
{
//printf("aros8bit_choose_points_function\n");
  // Examine the current rendering state and return a pointer to a
  // fast point-rendering function if possible.
  if (ctx->Point.Size == 1.0 && !ctx->Point.SmoothFlag && ctx->RasterMask == 0
       && !ctx->Texture.Enabled ) { //&&  ETC, ETC
    return aros8bit_points_function;
  }
  else return NULL;
}*/

 /*
  *  Render a line by some hardware/OS accerated method 
  */

/*
static void
aros8bit_line_function( GLcontext *ctx,GLuint v0, GLuint v1, GLuint pv )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  struct vertex_buffer *VB = ctx->VB;
  int x0, y0, x1, y1;
// printf("aros8bit_line_function\n");

  if (VB->MonoColor) SetAPen(amesa->rp, amesa->pixel);
  else SetAPen(amesa->rp, amesa->penconv[*VB->Color[pv]]);

  x0 = FIXx((int) (VB->Win[v0][0]));
  y0 = FIXy((int) (VB->Win[v0][1]));
  x1 = FIXx((int) (VB->Win[v1][0]));
  y1 = FIXy((int) (VB->Win[v1][1]));

  Move(amesa->rp, x0, y0);
  Draw(amesa->rp, x1, y1);
} */

/*
static line_func
aros8bit_choose_line_function( GLcontext *ctx)
{
  //printf("aros8bit_choose_line_function\n");

  // Examine the current rendering state and return a pointer to a
  // fast line-rendering function if possible.

  if (ctx->Line.Width==1.0 
      && !ctx->Line.SmoothFlag
      && !ctx->Line.StippleFlag
	   && ctx->Light.ShadeModel==GL_FLAT
	   && ctx->RasterMask==0
	   && !ctx->Texture.Enabled ) //&&  ETC, ETC
  {
    return aros8bit_line_function;
  }
  else return NULL;
} */

/**********************************************************************/
/*****          Optimized polygon rendering         *****/
/**********************************************************************/

/*
 * Draw a filled polygon of a single color. If there is hardware/OS support
 * for polygon drawing use that here.   Otherwise, call a function in
 * polygon.c to do the drawing.
 */

#warning "TODO: removed next [obsolete] function to compile on 2.6"
/*static void
aros8bit_polygon_function( GLcontext *ctx,GLuint n, GLuint vlist[], GLuint pv )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  struct vertex_buffer *VB = ctx->VB;
  
  int i, j;
  struct RastPort *rp=amesa->rp;
  
  // Render a line by some hardware/OS accerated method
  // printf("aros8bit_polygon_function\n");
  
  if (VB->MonoColor) SetAPen(rp,amesa->pixel);
  else SetAPen(rp,amesa->penconv[*VB->Color[pv]]);
  
  AreaMove(rp, FIXx((int) VB->Win[0][0]), FIXy( (int) VB->Win[0][1]));
  
  for (i=1;i<n;i++) {
    j=vlist[i];
    AreaDraw(rp, FIXx((int) VB->Win[j][0]), FIXy( (int) VB->Win[j][1]));
  }
  AreaEnd(rp );
}*/

#warning "TODO: removed next [obsolete] function to compile on 2.6"
/*static polygon_func
aros8bit_choose_polygon_function( GLcontext *ctx )
{*/
  /* printf("aros8bit_choose_polygon_function\n");*/

  /* Examine the current rendering state and return a pointer to a */
  /* fast polygon-rendering function if possible. */
/*
  if (!ctx->Polygon.SmoothFlag && !ctx->Polygon.StippleFlag
     && ctx->Light.ShadeModel==GL_FLAT && ctx->RasterMask==0
     && !ctx->Texture.Enabled )*/ /*&&  ETC, ETC */
/*  {
    return aros8bit_polygon_function;
  }
  else 
  {
    return NULL;
  }
}
*/

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

/* Write a horizontal span of RGBA color pixels with a boolean mask. */
static void
aros8bit_write_rgba_span( const GLcontext *ctx, GLuint n, GLint x, GLint y,
                const GLubyte rgba[][4], const GLubyte mask[])
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i, ant;
  UBYTE *dp = NULL;

  struct RastPort *rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_write_rgba_span(ant=%d,x=%d,y=%d)", n, x, y));
#endif

  if((dp = amesa->imageline) && amesa->temprp)
  {       /* if imageline allocated then use fastversion */
#ifdef DEBUGPRINT
D(bug("FAST "));
#endif
    if (mask)
    {
#ifdef DEBUGPRINT
D(bug("mask\n"));
#endif
      ant = 0;
      for (i = 0; i < n; i++)       /* draw pixel (x[i],y[i]) */
      {
        if (mask[i])
        {
            ant++;
            *dp = RGBA(amesa, rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP], rgba[i][ACOMP]);
            dp++;
        }
        else
        {
          if(ant)
          {
            WritePixelLine8(rp, x, y, ant, amesa->imageline, amesa->temprp);
/*D(bug("[AROSMESA:8bit] WritePixelLine8(ant=%d,x=%d,y=%d)\n",ant,x,y));*/
            dp = amesa->imageline;
            ant = 0;
            x = x + ant;
          }
          x++;
        }
      }

      if(ant)
      {
        WritePixelLine8(rp, x, y, ant, amesa->imageline, amesa->temprp);
/*printf("WritePixelLine8(ant=%d,x=%d,y=%d)\n",ant,x,y);*/
      }
    }
    else
    {
#ifdef DEBUGPRINT
D(bug("nomask\n"));
#endif
      for (i = 0; i < n; i++)           /* draw pixel (x[i],y[i])*/
      {
        *dp = RGBA(amesa, rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP], rgba[i][ACOMP]);
        dp++;
      }
      WritePixelLine8(rp, x, y, n, amesa->imageline, amesa->temprp);
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
          SetAPen(rp, RGBA(amesa, rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP], rgba[i][ACOMP]));
          WritePixel(rp, x, y);
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
        SetAPen(rp, RGBA(amesa, rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP], rgba[i][ACOMP]));
        WritePixel(rp, x, y);
      }
    }
  }
}

/* Write a horizontal span of RGB color pixels with a boolean mask. */
static void
aros8bit_write_rgb_span( const GLcontext *ctx, GLuint n, GLint x, GLint y,
                const GLubyte rgba[][3], const GLubyte mask[])
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i, ant;
  UBYTE *dp = NULL;

  struct RastPort *rp = amesa->rp;

  y = FIXy(y);
  x = FIXx(x);
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_write_color_span(ant=%d,x=%d,y=%d)", n, x, y));
#endif

  if((dp = amesa->imageline) && amesa->temprp)
  {       /* if imageline allocated then use fastversion */
#ifdef DEBUGPRINT
D(bug("FAST "));
#endif
    if (mask)
    {
#ifdef DEBUGPRINT
D(bug("mask\n"));
#endif
      ant=0;
      for (i = 0; i < n; i++)       /* draw pixel (x[i],y[i]) */
      {
        if (mask[i])
        {
            ant++;
            *dp = RGB(amesa, rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP]);
            dp++;
        }
        else
        {
          if(ant)
          {
            WritePixelLine8(rp, x, y, ant, amesa->imageline, amesa->temprp);
/*D(bug("[AROSMESA:8bit] WritePixelLine8(ant=%d,x=%d,y=%d)\n",ant,x,y));*/
            dp = amesa->imageline;
            ant = 0;
            x = x + ant;
          }
          x++;
        }
      }

      if(ant)
      {
        WritePixelLine8(rp, x, y, ant, amesa->imageline, amesa->temprp);
/*D(bug("[AROSMESA:8bit] WritePixelLine8(ant=%d,x=%d,y=%d)\n",ant,x,y));*/
      }
    }
    else
    {
#ifdef DEBUGPRINT
D(bug("nomask\n"));
#endif
      for (i = 0; i < n; i++)           /* draw pixel (x[i],y[i])*/
      {
        *dp = RGB(amesa, rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP]);
        dp++;
      }
      WritePixelLine8(rp, x, y, n, amesa->imageline, amesa->temprp);
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
          SetAPen(rp, RGB(amesa, rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP]));
          WritePixel(rp, x, y);
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
        SetAPen(rp, RGB(amesa, rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP]));
        WritePixel(rp, x, y);
      }
    }
  }
}

/*
* Write a horizontal span of pixels with a boolean mask.  The current color
* is used for all pixels.
*/

static void
aros8bit_write_mono_rgba_span( const GLcontext *ctx,
                    GLuint n, GLint x, GLint y,
                    const GLubyte mask[])
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i;
  struct RastPort *rp = amesa->rp;

#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_write_mono_rgba_span(%d,%d,%d)\n", n, x, y));
#endif

  y = FIXy(y);
  x = FIXx(x);
  SetAPen(rp, amesa->pixel);

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
      Move(rp, x, y);
      while(mask[i] && (i < n))
      {
        i++;
        x++;
      }
      Draw(rp, x, y);
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

/* Write an array of RGBA pixels with a boolean mask. */
static void
aros8bit_write_rgba_pixels( const GLcontext* ctx,
                 GLuint n, const GLint x[], const GLint y[],
                 const GLubyte rgba[][4], const GLubyte mask[] )
{
   AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i;
  struct RastPort *rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_write_rgba_pixels()\n"));
#endif

  for (i = 0; i < n; i++)
  {
    if (mask[i])
    {
/*    write pixel x[i], y[i] using red[i],green[i],blue[i],alpha[i] */
      SetAPen(rp, RGBA(amesa, rgba[i][RCOMP], rgba[i][GCOMP], rgba[i][BCOMP], rgba[i][ACOMP]));
      WritePixel(rp, FIXx(x[i]), FIXy(y[i]));
    }
  }
}

/*
* Write an array of pixels with a boolean mask.  The current color
* is used for all pixels.
*/
static void
aros8bit_write_mono_rgba_pixels( const GLcontext* ctx,
                  GLuint n,
                  const GLint x[], const GLint y[],
                  const GLubyte mask[] )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  int i;
  struct RastPort *rp = amesa->rp;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_write_mono_rgba_pixels()\n"));
#endif

  SetAPen(rp, amesa->pixel);

  for (i = 0; i < n; i++)
  {
    if (mask[i])
    {
/*    write pixel x[i], y[i] using current color*/
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

/* Read a horizontal span of color pixels. */
static void
aros8bit_read_rgba_span( const GLcontext* ctx,
              GLuint n, GLint x, GLint y,
              GLubyte rgba[][4] )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  struct ColorMap *cm = amesa->Screen->ViewPort.ColorMap;
  int i, col;
  ULONG ColTab[3];

#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_read_rgba_span()\n"));
#endif

  y = FIXy(y);
  x = FIXx(x);

  if(amesa->imageline && amesa->temprp)
  {
    ReadPixelLine8(amesa->rp, x, y, n,amesa->imageline, amesa->temprp);
    for(i = 0; i < n; i++)
    {
      GetRGB32(cm, amesa->imageline[i], 1, ColTab);
      rgba[i][RCOMP] = ColTab[0] >> 24;
      rgba[i][GCOMP] = ColTab[1] >> 24;
      rgba[i][BCOMP] = ColTab[2] >> 24;
      rgba[i][ACOMP] = 255;
    }
  } else {
    for (i=0; i<n; i++, x++)
    {
      col = ReadPixel(amesa->rp, x, y);
      GetRGB32(cm, col, 1, ColTab);

      rgba[i][RCOMP] = ColTab[0] >> 24;
      rgba[i][GCOMP] = ColTab[1] >> 24;
      rgba[i][BCOMP] = ColTab[2] >> 24;
      rgba[i][ACOMP] = 255;
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

/* Read an array of color pixels. */
static void
aros8bit_read_rgba_pixels( const GLcontext* ctx,
                GLuint n, const GLint x[], const GLint y[],
                GLubyte rgba[][4], const GLubyte mask[] )
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
  struct RastPort *rp = amesa->rp;
  int i, col;
  ULONG ColTab[3];

#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_read_rgba_pixels()\n"));
#endif

  for (i = 0; i < n; i++)
  {
    if (mask[i])
    {
      col = ReadPixel(rp, FIXx(x[i]), FIXy(y[i]));

      GetRGB32(amesa->Screen->ViewPort.ColorMap, col, 1, ColTab);

      rgba[i][RCOMP] = ColTab[0] >> 24;
      rgba[i][GCOMP] = ColTab[1] >> 24;
      rgba[i][BCOMP] = ColTab[2] >> 24;
      rgba[i][ACOMP] = 255;
    }
  }
}

/**********************************************************************/
/**********************************************************************/

static GLboolean
aros8bit_logicop( GLcontext* ctx, GLenum op )
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_logicop()\n"));
#endif
  /* can't implement */
  return GL_FALSE;
}

static void
aros8bit_dither( GLcontext* ctx, GLboolean enable )
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_dither()\n"));
#endif
  /* currently unsuported */
}

static GLboolean
aros8bit_set_buffer( GLcontext *ctx,GLenum mode )
{
	/* set the current drawing/reading buffer, return GL_TRUE or GL_FALSE */
	/* for success/failure */
	
/*   AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx; */

/* aros8bit_setup_DD_pointers(); in ddsample is this right?????*/
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_set_buffer() : TODO\n"));
#endif
	
#warning "TODO: implement a set of buffers"
  if (mode == GL_FRONT)
  {
    return(GL_TRUE);
  }
  else if (mode == GL_BACK)
  {
    return(GL_TRUE);
  }
  else
  {
    return(GL_FALSE);
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
aros8bit_update_state( GLcontext *ctx )
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] Inside aros8bit_update_state ctx=%x\n", ctx));
#endif

  ctx->Driver.GetString = aros8bit_renderer_string;
  ctx->Driver.UpdateState = aros8bit_update_state;
//  ctx->Driver.Finish = aros8bit_finish;
//  ctx->Driver.Flush = aros8bit_flush;

  ctx->Driver.SetDrawBuffer = aros8bit_set_buffer;
  ctx->Driver.Color = aros8bit_set_color;
  ctx->Driver.Index = aros8bit_set_index;
  ctx->Driver.ClearIndex = aros8bit_clear_index;
  ctx->Driver.ClearColor = aros8bit_clear_color;
  ctx->Driver.Clear = aros8bit_clear;
  
//  ctx->Driver.IndexMask = aros8bit_index_mask;
//  ctx->Driver.ColorMask = aros8bit_color_mask;

  ctx->Driver.GetBufferSize = aros8bit_buffer_size;

//  ctx->Driver.LogicOp = aros8bit_logicop;
//  ctx->Driver.Dither = aros8bit_dither;
  
  ctx->Driver.PointsFunc = NULL;
  ctx->Driver.LineFunc = NULL;
  ctx->Driver.TriangleFunc = NULL;

  /* RGB/RGBA functions: */
  ctx->Driver.WriteRGBASpan       = aros8bit_write_rgba_span;
  ctx->Driver.WriteRGBSpan        = aros8bit_write_rgb_span;
  ctx->Driver.WriteRGBAPixels     = aros8bit_write_rgba_pixels;
  ctx->Driver.WriteMonoRGBASpan   = aros8bit_write_mono_rgba_span;
  ctx->Driver.WriteMonoRGBAPixels = aros8bit_write_mono_rgba_pixels;
  ctx->Driver.ReadRGBASpan        = aros8bit_read_rgba_span;
  ctx->Driver.ReadRGBAPixels      = aros8bit_read_rgba_pixels;

  /* Indexed Color functions: */
  ctx->Driver.WriteCI32Span       = aros8bit_write_ci32_span;
  ctx->Driver.WriteCI8Span        = aros8bit_write_ci8_span;
  ctx->Driver.WriteMonoCISpan     = aros8bit_write_mono_ci_span;
  ctx->Driver.WriteCI32Pixels     = aros8bit_write_ci32_pixels;
  ctx->Driver.WriteMonoCIPixels   = aros8bit_write_mono_ci_pixels;
  ctx->Driver.ReadCI32Span        = aros8bit_read_ci32_span;
  ctx->Driver.ReadCI32Pixels      = aros8bit_read_ci32_pixels;
}

/**********************************************************************/
/*****          AROS/Mesa private init/despose/resize   *****/
/**********************************************************************/

static void
aros8bit_buffer_size( GLcontext *ctx,GLuint *width, GLuint *height)
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_buffer_size()\n"));
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
      if (amesa->back_rp) arosRasterizer_destroy_rastport(amesa->back_rp);/* Free double buffer */
      if((amesa->back_rp = arosRasterizer_make_rastport(amesa->RealWidth, amesa->RealHeight, amesa->depth, amesa->front_rp->BitMap)) == NULL) {
        amesa->rp = amesa->front_rp;
        printf("To little mem free. Couldn't allocate Dubblebuffer in this size.\n");
      }
      else amesa->rp = amesa->back_rp;
    }
  
    if(!arosRasterizer_make_temp_raster( amesa->rp )) printf("Error allocating TmpRasterPort\n");
  
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

/* The following 2 functions will be moved
   shortly since theyre generic to both truecolor
   and indexed rasterizers */

void
aros8bit_Standard_Dispose(AROSMesaContext amesa)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_Standard_Dispose()\n"));
#endif

  arosRasterizer_FreeOneLine(amesa);
  if (amesa->depth <= 8)
  {
    FreeCMap(amesa->Screen);
    arosRasterizer_freearea(amesa->rp);
    arosRasterizer_destroy_temp_raster( amesa->rp);
    /* JAM: Added arosRasterizer_free_temp_rastport() */
    arosRasterizer_free_temp_rastport(amesa);
  }
  
  if (amesa->visual->rgb_flag)
  {
    if (amesa->rgb_buffer)
    {
      printf("free(amesa->rgb_buffer)\n");
      free( amesa->rgb_buffer );
    }
  }
  
  if (amesa->back_rp)
  {
    arosRasterizer_destroy_rastport( amesa->back_rp );
    amesa->back_rp = NULL;
  }
}

void
aros8bit_Standard_SwapBuffer(AROSMesaContext amesa)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:8bit] aros8bit_Standard_SwapBuffer()\n"));
#endif

  if (amesa->back_rp)
  {
    UBYTE minterm = 0xc0;
/*    int x = amesa->left; */
/*    int y = amesa->RealHeight-amesa->bottom-amesa->height; */

    ClipBlit( amesa->back_rp, FIXx(((GLcontext *)amesa->gl_ctx)->Viewport.X), (FIXy(((GLcontext *)amesa->gl_ctx)->Viewport.Y) - ((GLcontext *)amesa->gl_ctx)->Viewport.Height) + 1,  /* from */
       amesa->front_rp, FIXx(((GLcontext *)amesa->gl_ctx)->Viewport.X), (FIXy(((GLcontext *)amesa->gl_ctx)->Viewport.Y) - ((GLcontext *)amesa->gl_ctx)->Viewport.Height) + 1,  /* to */
       ((GLcontext *)amesa->gl_ctx)->Viewport.Width, ((GLcontext *)amesa->gl_ctx)->Viewport.Height,  /* size */
       minterm );
    // ClipBlit( amesa->back_rp, x, y,
      /* from */
      // amesa->front_rp, x,y,
      /* to */
      // amesa->width, amesa->height,
      /* size */
      // minterm );

/* TODO Use these cordinates insted more efficent if you only use part of screen
    RectFill(amesa->rp,FIXx(CC.Viewport.X),FIXy(CC.Viewport.Y)-CC.Viewport.Height+1,
          FIXx(CC.Viewport.X)+CC.Viewport.Width-1,FIXy(CC.Viewport.Y));*/
  }
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
