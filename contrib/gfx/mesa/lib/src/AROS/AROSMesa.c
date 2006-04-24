/* $Id$ */

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

#include "AROSMesa_rast_common.h"

#ifdef ADISP_CYBERGFX
#ifdef __GNUC__
#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>
#else
#include <inline/cybergraphics.h>
#endif
#endif

#include <GL/AROSMesa.h>

#ifdef ADISP_CYBERGFX
#include "AROSMesa_rast_TC.h"
#endif

#ifdef ADISP_AGA
#include "AROSMesa_rast_8bit.h"
#endif

#ifdef __GNUC__
#include "../aros/misc/ht_colors_protos.h"
#else
#include "/aros/misc/ht_colors_protos.h"
#endif

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

/**********************************************************************/
/*****        Internal Data                     *****/
/**********************************************************************/

GLenum         LastError;        /* The last error generated*/
struct Library *CyberGfxBase = NULL;    /* optional base address for cybergfx */

/**********************************************************************/
/*****          AROS/Mesa API Functions          *****/
/**********************************************************************/
/*
 * Implement the client-visible AROS/Mesa interface functions defined
 * in Mesa/include/GL/AROSmesa.h
 *
 **********************************************************************/
/*
 * Implement the public AROS/Mesa interface functions defined
 * in Mesa/include/GL/AROSMesa.h
 */

//UBYTE Version[]="$VER: AROSMesa (linklib) 3.4.2";

/**********************************************************************/
/***** AROSMesaSetOneColor                     *****/
/**********************************************************************/
void GLAPIENTRY
AROSMesaSetOneColor(AROSMesaContext amesa, int index, float r, float g, float b)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA:TC] AROSMesaSetOneColor(amesa @ %x)\n", amesa));
#endif
  if (amesa->depth > 8)
  {
    amesa->penconv[index] = TC_ARGB((int)r * 255, (int)g * 255, (int)b * 255, 255);
  } else {
    amesa->penconv[index] = RGBA(amesa, r * 255, g * 255, b * 255, 255);
  }
}

/* Main AROSMesa API Functions */


AROSMesaVisual GLAPIENTRY
AROSMesaCreateVisualTags(long Tag1, ...)
{
  return AROSMesaCreateVisual((struct TagItem *)&Tag1);
}

AROSMesaVisual GLAPIENTRY
AROSMesaCreateVisual(struct TagItem *tagList)
{
  AROSMesaVisual v = NULL;
  int  indexBits, redBits, greenBits, blueBits, alphaBits, depthBits, stencilBits, accumBits;

#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateVisual(taglist @ %x)\n", tagList));
#endif

  AROSMesaContext amesa = (AROSMesaContext)GetTagData(AMA_Context, GL_FALSE, tagList);

  if (amesa == GL_FALSE)
  {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateVisual: ERROR - No Context\n"));
#endif
    return NULL;
  }

#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateVisual: amesa Context @ %x\n", amesa));
#endif

  if (!(amesa->rp))
  {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateVisual: ERROR - Context has no RastPort\n"));
#endif
    return NULL;
  }

#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateVisual: Using RastPort @ %x\n", amesa->rp));
#endif
  if ((v = (AROSMesaVisual)AllocVec(sizeof(struct arosmesa_visual),MEMF_PUBLIC|MEMF_CLEAR)))
  {
    v->rgb_flag = GL_FALSE;
    v->db_flag = GL_FALSE;
    v->alpha_flag = GL_TRUE;

    if (((BOOL)GetTagData(AMA_DoubleBuf, TRUE, tagList) == TRUE))
    	v->db_flag = GL_TRUE;

    if (((BOOL)GetTagData(AMA_AlphaFlag, TRUE, tagList) == FALSE))
      v->alpha_flag = GL_FALSE;

    if (CyberGfxBase != NULL)
    {
      amesa->depth = GetCyberMapAttr(amesa->rp->BitMap,CYBRMATTR_DEPTH);
    }
    else
    {
      amesa->depth = GetBitMapAttr(amesa->rp->BitMap,BMA_DEPTH);
    }

    if (amesa->depth > 8)
      v->rgb_flag = GL_TRUE;

#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateVisual: RastPort.BitMap Depth = %d\n", amesa->depth));
#endif

    if (v->rgb_flag)
    {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateVisual: Using RGB Mode\n"));
#endif
      indexBits = 0;
      redBits = CHAN_BITS;
      greenBits = CHAN_BITS;
      blueBits = CHAN_BITS;
      alphaBits = CHAN_BITS;
    } else {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateVisual: Using Indexed Color Mode\n"));
#endif
      indexBits = 8;
    /* color index mode */
      redBits = CHAN_BITS;
      greenBits = CHAN_BITS;
      blueBits = CHAN_BITS;
      alphaBits = CHAN_BITS;
    }
  
    /* Create core visual */
    depthBits = DEFAULT_SOFTWARE_DEPTH_BITS;
    stencilBits = 8;
    accumBits = 16;

    if (!(v->gl_visual = (IPTR)_mesa_create_visual( v->rgb_flag, 
                                                    v->db_flag,
                                                    GL_FALSE,               /* stereo */
                                                    redBits,
                                                    greenBits,
                                                    blueBits,
                                                    alphaBits,
                                                    indexBits,
                                                    depthBits,
                                                    stencilBits,
                                                    accumBits,
                                                    accumBits,
                                                    accumBits,
                                                    alphaBits ? accumBits : 0,
                                                    1)))
    {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateVisual: ERROR - couldnt create gl_visual\n"));
#endif
    
      FreeVec(v);
      return NULL;
    }
  }
  return v;
}

void GLAPIENTRY
AROSMesaDestroyVisual(AROSMesaVisual visual)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaDestroyVisual(visual @ %x)\n", visual));
#endif

  _mesa_destroy_visual( (GLvisual *)visual->gl_visual );
  FreeVec( visual );
}

AROSMesaBuffer GLAPIENTRY
AROSMesaCreateBuffer(AROSMesaVisual visual, int windowid)
{
  AROSMesaBuffer b = NULL;

#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateBuffer(visual @ %x)\n", visual));
#endif

  if ((b = (AROSMesaBuffer)AllocVec(sizeof(struct arosmesa_buffer), MEMF_PUBLIC|MEMF_CLEAR)))
  {
    /* other stuff */
    if (!(b->gl_buffer = (IPTR)_mesa_create_framebuffer( (GLvisual *)visual->gl_visual,
                                                   (((GLvisual *)visual->gl_visual)->depthBits) > 0,
                                                   (((GLvisual *)visual->gl_visual)->stencilBits) > 0,
                                                   (((GLvisual *)visual->gl_visual)->accumRedBits) > 0,
                                                   (((GLvisual *)visual->gl_visual)->alphaBits) > 0)))
    {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateBuffer: ERROR - Failed to create Mesa FrameBuffers\n"));
#endif
      FreeVec(b);
      return NULL;
    }
    else
    {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateBuffer: Context Buffers created\n"));
#endif
    }
  }
  else
  {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateBuffer: ERROR - Failed to create internal buffer record\n"));
#endif
  }
  return b;
}

void GLAPIENTRY
AROSMesaDestroyBuffer(AROSMesaBuffer b)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaDestroyBuffer(buffer @ %x)\n", b));
#endif
  _mesa_destroy_framebuffer( (GLframebuffer *)b->gl_buffer );
  FreeVec( b );
}

AROSMesaContext GLAPIENTRY
AROSMesaCreateContextTags(long Tag1, ...)
{
  return AROSMesaCreateContext((struct TagItem *)&Tag1);
}

AROSMesaContext GLAPIENTRY
AROSMesaCreateContext(struct TagItem *tagList)
{
  /* Create a new AROS/Mesa context */
  /* Be sure to initialize the following in the core Mesa context: */
  /* DrawBuffer, ReadBuffer */  /* @@@ IMPLEMENTERA ???*/

/*  GLfloat redscale,greenscale,bluescale,alphascale; */
/*  int     I; */
/*	int drawMode; */
  AROSMesaContext amesa = NULL;
  AROSMesaContext sharetmp = NULL;
  LastError = 0;

#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext(taglist @ %x)\n", tagList));
#endif

  /* try to open cybergraphics.library */
  if (CyberGfxBase == NULL)
  {
   if (!(CyberGfxBase = OpenLibrary((UBYTE*)"cybergraphics.library",0)))
     return NULL;
  }

  if (!(amesa = (AROSMesaContext)GetTagData(AMA_Context, GL_FALSE, tagList)))
  {

    /* allocate arosmesa_context struct initialized to zeros */
    if (!(amesa = (AROSMesaContext)AllocVec(sizeof(struct arosmesa_context), MEMF_PUBLIC|MEMF_CLEAR)))
    {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: Failed to allocate AROSMesaContext\n"));
#endif
      LastError = AMESA_OUT_OF_MEM;
      return NULL;
    }


    if (!(amesa->gl_ctx = AllocVec(sizeof(GLcontext), MEMF_PUBLIC|MEMF_CLEAR)))
    {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: Failed to allocate gl_ctx\n"));
#endif
      LastError = AMESA_OUT_OF_MEM;
      FreeVec(amesa);
      return NULL;
    }

#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: AROSMesaContext Allocated @ %x\n", amesa));
D(bug("[AROSMESA] AROSMesaCreateContext:          gl_ctx Allocated @ %x\n", amesa->gl_ctx));
#endif

/* CHANGE TO WORK EXCLUSIVELY WITH RASTPORTS! */
    amesa->Screen = (struct Screen *)GetTagData(AMA_Screen, 0, tagList);
    amesa->window = (struct Window *)GetTagData(AMA_Window, 0, tagList);
    amesa->rp = (struct RastPort *)GetTagData(AMA_RastPort, 0, tagList);

    if (amesa->Screen)
    {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: Screen @ %x\n", amesa->Screen));
#endif
      if (amesa->window)
      {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: window @ %x\n", amesa->window));
#endif
        if (!(amesa->rp))
        {
          /* Use the windows rastport */
          amesa->rp = amesa->window->RPort;
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: Windows RastPort @ %x\n", amesa->rp));
#endif
        }
      }
      else
      {
        if (!(amesa->rp))
        {
          /* Use the screens rastport */
          amesa->rp = &amesa->Screen->RastPort;
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: Screens RastPort @ %x\n", amesa->rp));
#endif
        }
      }
    }
    else
    {
    /* Not passed a screen */
      if (amesa->window)
      {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: Window @ %x\n", amesa->window));
#endif
        /* Use the windows Screen */
        amesa->Screen = amesa->window->WScreen;
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: Windows Screen @ %x\n", amesa->Screen));
#endif
        if (!(amesa->rp))
        {
          /* Use the windows rastport */
          amesa->rp = amesa->window->RPort;
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: Windows RastPort @ %x\n", amesa->rp));
#endif
        }
      }
      else
      {
    		/* Only Passed A Rastport */
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: Experimental: Using RastPort only!\n"));
#endif
      }
    }
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: Using RastPort @ %x\n", amesa->rp));
#endif

    if(!(amesa->visual = (AROSMesaVisual)GetTagData(AMA_Visual, 0, tagList)))
    {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: Creating new AROSMesaVisual\n"));
#endif
      if (!(amesa->visual = AROSMesaCreateVisualTags(AMA_Context, amesa, TAG_MORE, tagList, TAG_DONE)))
      {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: ERROR -  failed to create AROSMesaVisual\n"));
#endif
        FreeVec( amesa );
        LastError = AMESA_OUT_OF_MEM;
        return NULL;
      }
      amesa->flags = amesa->flags|0x1;
    }

#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: [ASSERT] RastPort @ %x\n", amesa->rp));
#endif

    if ((amesa->share = (AROSMesaContext )GetTagData(AMA_ShareGLContext, 0, tagList)))
      sharetmp = amesa->share;

    if (!_mesa_initialize_context( (GLcontext *)amesa->gl_ctx,
                                          (GLvisual *)amesa->visual->gl_visual,
                                          sharetmp ? (GLcontext *)sharetmp->gl_ctx
                                                   : (GLcontext *)NULL,
                                          (void *) amesa, GL_TRUE))
    {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: Mesa failed to initialize the context!\n"));
#endif
		AROSMesaDestroyVisual( amesa->visual );
      FreeVec( amesa );
      return NULL;
    }

#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: [ASSERT] RastPort @ %x\n", amesa->rp));
#endif

    _mesa_enable_sw_extensions((GLcontext *)amesa->gl_ctx);
    _mesa_enable_extension((GLcontext *)amesa->gl_ctx, "GL_ARB_texture_compression");
    ((GLcontext *)amesa->gl_ctx)->Driver.BaseCompressedTexFormat = _mesa_base_compressed_texformat;
    ((GLcontext *)amesa->gl_ctx)->Driver.CompressedTextureSize = _mesa_compressed_texture_size;
    ((GLcontext *)amesa->gl_ctx)->Driver.GetCompressedTexImage = _mesa_get_compressed_teximage;

#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: Enabled Mesa Software Extensions\n"));
#endif


/*  Disabled for now .. (nicja) */
#ifdef AMESA_DOUBLEBUFFFAST
/*    if(amesa->visual->db_flag == GL_TRUE)
    { */
#ifdef DEBUGPRINT
/*D(bug("[AROSMESA] AROSMesaCreateContext: Using Buffered Rendering\n"));*/
#endif
/*
      if (CyberGfxBase)
      {
        if ((arosTC_Standard_init_db(amesa,tagList)) == FALSE)
        {
          goto amccontextclean;
        }
      }
      else
      {
        if ((aros8bit_Standard_init_db(amesa,tagList)) == FALSE)
        {
          goto amccontextclean;
        }
      }
    }
    else
    {
*/
#endif
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: Using Direct Rendering on RastPort @ %x\n", amesa->rp));
#endif
      if ((CyberGfxBase)&&(amesa->visual->rgb_flag))
      {
        if ((arosTC_Standard_init(amesa, tagList)) == FALSE)
        {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: TrueColor Raterizer Failed\n"));
#endif
          goto amccontextclean;
        }
      }
      else
      {
        if ((aros8bit_Standard_init(amesa, tagList)) == FALSE)
        {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: 8bit Raterizer Failed\n"));
#endif
          goto amccontextclean;
        }
      }
#ifdef AMESA_DOUBLEBUFFFAST
    }
#endif

#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: AROS Rasterizer initialised.\n"));
#endif
  
    if(!(amesa->buffer = (AROSMesaBuffer)GetTagData(AMA_Buffer, 0, tagList)))
    {
      if(!(amesa->buffer = AROSMesaCreateBuffer(amesa->visual, (int)GetTagData(AMA_WindowID, 1, tagList))))
      {
        LastError = AMESA_OUT_OF_MEM;
        goto amccontextclean;
      }
      amesa->flags = (amesa->flags|0x2);
    }
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: Using framebuffer @ %x.\n", amesa->buffer));
#endif
  }
 
  _swrast_CreateContext((GLcontext *)amesa->gl_ctx);
  _ac_CreateContext((GLcontext *)amesa->gl_ctx);
  _tnl_CreateContext((GLcontext *)amesa->gl_ctx);
  _swsetup_CreateContext((GLcontext *)amesa->gl_ctx);

#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: Mesa CORE Contexts Created\n"));
#endif

  if (amesa->InitDD)
  {
    ((GLcontext *)amesa->gl_ctx)->Driver.UpdateState = amesa->InitDD;
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: Context Created\n"));
#endif
    return amesa;
  }
  else
  {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: ERROR - Driver functions table missing!!\n"));
#endif
  }
amccontextclean:
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaCreateContext: ERROR - Failed to create Context!!!\n"));
#endif
  if (amesa->visual)
    AROSMesaDestroyVisual(amesa->visual);

  _mesa_free_context_data((GLcontext *)amesa->gl_ctx);
  FreeVec(amesa->gl_ctx);
  FreeVec(amesa);
  return NULL;
}

void GLAPIENTRY
AROSMesaDestroyContext(AROSMesaContext amesa)
{
  /* destroy a AROS/Mesa context */
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaDestroyContext(amesa @ %x)\n", amesa));
#endif

  if (amesa->gl_ctx)
  {
    if (amesa->Dispose)
      (*amesa->Dispose)(amesa);

    _swsetup_DestroyContext((GLcontext *)amesa->gl_ctx);
    _tnl_DestroyContext((GLcontext *)amesa->gl_ctx);
    _ac_DestroyContext((GLcontext *)amesa->gl_ctx);
    _swrast_DestroyContext((GLcontext *)amesa->gl_ctx);

    if(amesa->flags&0x1) AROSMesaDestroyVisual(amesa->visual);
    if(amesa->flags&0x2) AROSMesaDestroyBuffer(amesa->buffer);
  
    _mesa_free_context_data((GLcontext *)amesa->gl_ctx);
    FreeVec(amesa->gl_ctx);
  }
  FreeVec(amesa);
}

void GLAPIENTRY
AROSMesaMakeCurrent(AROSMesaContext amesa, AROSMesaBuffer b)
{
  /* Make the specified context the current one */
  /* the order of operations here is very important! */

#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaMakeCurrent(amesa @ %x, buffer @ %x)\n", amesa, b));
#endif

  if (amesa && b)
  {
    if (amesa->InitDD)
        (*amesa->InitDD)((IPTR)amesa->gl_ctx, ~0);              /* Call Driver_init_routine */
    else
    {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaMakeCurrent: ERROR - Missing Driver Function table!\n"));
#endif
      return;
    }

    _mesa_make_current((GLcontext *)amesa->gl_ctx, (GLframebuffer *)b->gl_buffer);
    amesa->buffer = b;
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaMakeCurrent: set current mesa context/buffer\n"));
#endif



#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaMakeCurrent: initialised rasterizer driver functions\n"));
#endif

    if (((GLcontext *)amesa->gl_ctx)->Viewport.Width == 0)
    {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaMakeCurrent: call glViewport(0, 0, %d, %d)\n", amesa->width, amesa->height));
#endif
      _mesa_Viewport(0, 0, amesa->width, amesa->height);
      ((GLcontext *)amesa->gl_ctx)->Scissor.Width = amesa->width;
      ((GLcontext *)amesa->gl_ctx)->Scissor.Height = amesa->height;
    } else {
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaMakeCurrent: call gl_make_current()\n"));
#endif
      _mesa_make_current(NULL, NULL);
    }
  }
}

void GLAPIENTRY
AROSMesaSwapBuffers(AROSMesaContext amesa)
{        /* copy/swap back buffer to front if applicable */
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaSwapBuffers(amesa @ %x)\n", amesa));
#endif
  if (amesa->SwapBuffer)
    (*amesa->SwapBuffer)( amesa );
}

/* This is on the drawingboard */
/* Mostly for when future changes the library is still intact*/

BOOL GLAPIENTRY
AROSMesaSetDefs(struct TagItem *tagList)
{
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaSetDefs(taglist @ %x)\n", tagList));
#endif
/*
  struct TagItem *tag=NULL;
      
  tagValue=AMA_DrawMode;
  tag = FindTagItem(tagValue,tagList);
#ifdef DEBUGPRINT
  if (tag)
D(bug("[AROSMESA] AROSMesaSetDefs: Tag=%x, is %x/n",tagValue,tag->ti_Data));
  else
D(bug("[AROSMESA] AROSMesaSetDefs: Tag=%x is not specified/n",tagValue));
#endif
*/
  return FALSE;
}

/*
 Maybe a report error routine ??? like:
*/

GLenum GLAPIENTRY
AROSMesaReportError(AROSMesaContext amesa)
{
  GLenum error;
#ifdef DEBUGPRINT
D(bug("[AROSMESA] AROSMesaReportError(amesa @ %x)\n", amesa));
#endif
  error = LastError;
  LastError = 0;
  return(error);
}
