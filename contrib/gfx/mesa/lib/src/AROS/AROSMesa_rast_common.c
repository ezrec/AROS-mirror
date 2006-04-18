/* $Id: AROSMesa_rast_common.c 24318 2006-04-15 17:52:01Z NicJA $*/

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

struct RastPort *arosRasterizer_make_rastport( int width, int height, int depth, struct BitMap *friendbm );
void arosRasterizer_destroy_rastport( struct RastPort *rp );
BOOL arosRasterizer_make_temp_raster( struct RastPort *rp );
void arosRasterizer_destroy_temp_raster( struct RastPort *rp );
void arosRasterizer_AllocOneLine(AROSMesaContext amesa);
void arosRasterizer_FreeOneLine(AROSMesaContext amesa);

BOOL arosRasterizer_alloc_temp_rastport(AROSMesaContext amesa);
void arosRasterizer_free_temp_rastport(AROSMesaContext amesa);

/**********************************************************************/
/*****        Miscellaneous device driver funcs                   *****/
/**********************************************************************/

GLboolean
arosRasterizer_set_buffer( GLcontext *ctx, GLenum mode )
{
	/* set the current drawing/reading buffer, return GL_TRUE or GL_FALSE */
	/* for success/failure */
	
/*   AROSMesaContext amesa = (AROSMesaContext) ctx->DriverCtx; */

/* aros8bit_setup_DD_pointers(); in ddsample is this right?????*/
#ifdef DEBUGPRINT
D(bug("[AROSMESA:RAST] arosRasterizer_set_buffer() : TODO\n"));
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

/* JAM: temporary rastport allocation and freeing */

BOOL
arosRasterizer_alloc_temp_rastport(AROSMesaContext amesa)
{
  struct RastPort *temprp = NULL;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:RAST] arosRasterizer_alloc_temp_rastport()\n"));
#endif

  if(( temprp = CreateRastPort()))
  {
    if((temprp->BitMap = AllocBitMap(amesa->FixedWidth, 1, amesa->rp->BitMap->Depth, 0, amesa->rp->BitMap)))
    {
/* temprp->BytesPerRow == (((width+15)>>4)<<1) */
      amesa->temprp = temprp;
      return TRUE;
    }
    FreeRastPort(temprp);
  }
  printf("Error allocating temporary rastport");
  return FALSE;
}

void
arosRasterizer_free_temp_rastport(AROSMesaContext amesa)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:RAST] arosRasterizer_free_temp_rastport()\n"));
#endif
  if(amesa->temprp)
  {
    FreeBitMap(amesa->temprp->BitMap);
    FreeRastPort(amesa->temprp);
  }
  amesa->temprp = NULL;
}

/*
 * Create a new rastport to use as a back buffer.
 * Input:  width, height - size in pixels
 *    depth - number of bitplanes
 */

struct RastPort *
arosRasterizer_make_rastport( int width, int height, int depth, struct BitMap *friendbm )
{
  struct RastPort *rp = NULL;
  struct BitMap *bm = NULL;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:RAST] arosRasterizer_make_rastport()\n"));
#endif


  if ((bm = AllocBitMap(width, height, depth, BMF_CLEAR|BMF_INTERLEAVED, friendbm)))
  {
    if ((rp = CreateRastPort()))
    {
      rp->BitMap = bm;
      return rp;
    }
    else
    {
      FreeBitMap(bm);
      return NULL;
    }
  }
  else return NULL;
}

/*
 * Deallocate a rastport.
 */

void
arosRasterizer_destroy_rastport( struct RastPort *rp )
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:RAST] arosRasterizer_destroy_rastport() : TODO\n"));
#endif
  WaitBlit();
  FreeBitMap( rp->BitMap );
  FreeRastPort( rp );
}

/* 
** Color_buf is a array of pens equals the drawing area
** it's for faster dubbelbuffer rendering
** Whent it's time for bufferswitch just use c2p and copy.
*/

UBYTE*
arosRasterizer_alloc_penbackarray( int width, int height, int bytes)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:RAST] arosRasterizer_alloc_penbackarray()\n"));
#endif
  return(AllocVec(width*height*bytes, MEMF_PUBLIC|MEMF_CLEAR));
}

void
arosRasterizer_destroy_penbackarray(UBYTE *buf)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:RAST] arosRasterizer_destroy_penbackarray()\n"));
#endif
  FreeVec(buf);
}

/*
** Construct a temporary raster for use by the given rasterport.
** Temp rasters are used for polygon drawing.
*/

BOOL
arosRasterizer_make_temp_raster( struct RastPort *rp )
{
  BOOL           OK = TRUE;
  unsigned long  width, height;
  PLANEPTR       p = NULL;
  struct         TmpRas *tmpras = NULL;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:RAST] arosRasterizer_make_temp_raster()\n"));
#endif

  if(rp == 0)
  {
    printf("Zero rp\n");
    return(FALSE);
  }

  width = rp->BitMap->BytesPerRow * 8;
  height = rp->BitMap->Rows;

  /* allocate structures */
  if((p = AllocRaster( width, height )))
  {
    if(( tmpras = (struct TmpRas *) AllocVec( sizeof(struct TmpRas), MEMF_ANY)))
    {
      InitTmpRas( tmpras, p, ((width+15)>>4)*height );
      rp->TmpRas = tmpras;
    } else {
      OK = FALSE;
    }
  }
  else
  {
    return(FALSE);
  }

  if (OK)
  {
    return(TRUE);
  }
  else {
    printf("Error when allocationg TmpRas\n");
    if (tmpras)
    {
      FreeVec(tmpras);
    }
    if (p)
    {
      FreeRaster(p, width, height);
    }
    return(FALSE);
  }
}

BOOL
arosRasterizer_allocarea(struct RastPort *rp )
{
  BOOL            OK = TRUE;
  struct AreaInfo *areainfo = NULL;
  UWORD           *pattern = NULL;
  APTR            vbuffer = NULL;

#ifdef DEBUGPRINT
D(bug("[AROSMESA:RAST] arosRasterizer_allocarea()\n"));
#endif

  areainfo = (struct AreaInfo *) AllocVec( sizeof(struct AreaInfo), MEMF_ANY );

  if(areainfo != 0)
  {
    pattern = (UWORD *) AllocVec( sizeof(UWORD),MEMF_ANY);
    if(pattern != 0)
    {
      *pattern = 0xffff;    /*@@@ org: 0xffffffff*/
      vbuffer = (APTR) AllocVec( MAX_POLYGON * 5 * sizeof(WORD),MEMF_ANY);
      if(vbuffer != 0)
      {
        /* initialize */
        InitArea( areainfo, vbuffer, MAX_POLYGON );
        /* bind to rastport */
        rp->AreaPtrn = pattern;
        rp->AreaInfo = areainfo;
        rp->AreaPtSz = 0;
      } else OK=FALSE;
    } else OK=FALSE;
  } else OK=FALSE;

  if (OK) return (OK);
  else {
    printf("Error when allocationg AreaBuffers\n");
    if (vbuffer) FreeVec(vbuffer);
    if (pattern) FreeVec(pattern);
    if (areainfo) FreeVec(areainfo);
    return(OK);
  }
}

void
arosRasterizer_freearea(struct RastPort *rp)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:RAST] arosRasterizer_freearea()\n"));
#endif
  if (rp->AreaInfo)
  {
    if (rp->AreaInfo->VctrTbl) FreeVec(rp->AreaInfo->VctrTbl);
    if (rp->AreaPtrn)
    {
      FreeVec(rp->AreaPtrn);
      rp->AreaPtrn = NULL;
    }
    FreeVec(rp->AreaInfo);
    rp->AreaInfo = NULL;
  }
}

/*
** Destroy a temp raster.
*/

void
arosRasterizer_destroy_temp_raster( struct RastPort *rp )
{
  /* bitmap */
  unsigned long width, height;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:RAST] arosRasterizer_temp_raster()\n"));
#endif

  width = rp->BitMap->BytesPerRow*8;
  height = rp->BitMap->Rows;

  if (rp->TmpRas)
  {
    if(rp->TmpRas->RasPtr) FreeRaster( rp->TmpRas->RasPtr, width, height );
    FreeVec( rp->TmpRas );
    rp->TmpRas = NULL;
  }
}

void
arosRasterizer_AllocOneLine( AROSMesaContext amesa )
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:RAST] arosRasterizer_AllocOneLine()\n"));
#endif

  if(amesa->imageline) FreeVec(amesa->imageline);
  if (amesa->depth<=8)
  {
    amesa->imageline = AllocVec((amesa->width+15) & 0xfffffff0,MEMF_ANY);      /* One Line */
  } else {
    amesa->imageline = AllocVec(((amesa->width+3) & 0xfffffffc)*4,MEMF_ANY);   /* One Line */
  }
}

void
arosRasterizer_FreeOneLine( AROSMesaContext amesa )
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:RAST] arosRasterizer_FreeOneLine()\n"));
#endif
  if(amesa->imageline)
  {
    FreeVec(amesa->imageline);
    amesa->imageline = NULL;
  }
}

/**********************************************************************/
/*****          AROS/Mesa private init/despose/resize   *****/
/**********************************************************************/

void
arosRasterizer_buffer_size( GLcontext *ctx, GLuint *width, GLuint *height)
{
  AROSMesaContext amesa = (AROSMesaContext)ctx->DriverCtx;
#ifdef DEBUGPRINT
D(bug("[AROSMESA:RAST] arosRasterizer_buffer_size()\n"));
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
D(bug("[AROSMESA:RAST] arosRasterizer_buffer_size(ctx=%x) = w:%d x h:%d\n",ctx,*width,*height));
D(bug("[AROSMESA:RAST] arosRasterizer_buffer_size:    amesa->RealWidth  =%d\n", amesa->RealWidth));
D(bug("[AROSMESA:RAST] arosRasterizer_buffer_size:    amesa->RealHeight =%d\n", amesa->RealHeight));
D(bug("[AROSMESA:RAST] arosRasterizer_buffer_size:    amesa->width      =%d\n", amesa->width));
D(bug("[AROSMESA:RAST] arosRasterizer_buffer_size:    amesa->height     =%d\n", amesa->height));
D(bug("[AROSMESA:RAST] arosRasterizer_buffer_size:    amesa->left       =%d\n", amesa->left));
D(bug("[AROSMESA:RAST] arosRasterizer_buffer_size:    amesa->right      =%d\n", amesa->right));
D(bug("[AROSMESA:RAST] arosRasterizer_buffer_size:    amesa->top        =%d\n", amesa->top));
D(bug("[AROSMESA:RAST] arosRasterizer_buffer_size:    amesa->bottom     =%d\n", amesa->bottom));
D(bug("[AROSMESA:RAST] arosRasterizer_buffer_size:    amesa->depth      =%d\n", amesa->depth));
#endif
}

void
arosRasterizer_Standard_Dispose(AROSMesaContext amesa)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:RAST] arosRasterizer_Standard_Dispose(amesa @ %x)\n", amesa));
#endif

  arosRasterizer_FreeOneLine(amesa);
  if (amesa->depth <= 8)
  {
    FreeCMap(amesa->Screen);
    arosRasterizer_freearea(amesa->rp);
    arosRasterizer_destroy_temp_raster(amesa->rp);
    /* JAM: Added arosRasterizer_free_temp_rastport() */
    arosRasterizer_free_temp_rastport(amesa);
  }
  
  if (amesa->visual->rgb_flag)
  {
    if (amesa->rgb_buffer)
    {
      printf("free(c->rgb_buffer)\n");
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
arosRasterizer_Standard_SwapBuffer(AROSMesaContext amesa)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:RAST] arosRasterizer_Standard_SwapBuffer(amesa @ %x)\n", amesa));
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
  else
  {
#ifdef DEBUGPRINT
D(bug("[AROSMESA:RAST] arosRasterizer_Standard_SwapBuffer: NOP - SINGLE buffer\n"));
#endif
  }
}
