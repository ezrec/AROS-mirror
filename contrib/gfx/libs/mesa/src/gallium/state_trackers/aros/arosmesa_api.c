/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/


#include "arosmesa_internal.h"

#include "main/context.h"
#include "state_tracker/st_public.h"

#include <proto/exec.h>
#include <proto/utility.h>
#include <aros/debug.h>

#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>

struct Library * AROSMesaCyberGfxBase = NULL;    /* Base address for cybergfx */

#if (AROS_BIG_ENDIAN == 1)
#define AROS_PIXFMT RECTFMT_ARGB32   /* Big Endian Archs. */
#else
#define AROS_PIXFMT RECTFMT_BGRA32   /* Little Endian Archs. */
#endif


/*****************************************************************************/
/*                             PRIVATE FUNCTIONS                             */
/*****************************************************************************/

static void _aros_destroy_visual(AROSMesaVisual aros_vis)
{
    if (aros_vis)
    {
        FreeVec(aros_vis);
    }
}

static AROSMesaVisual aros_new_visual(struct TagItem *tagList)
{
    AROSMesaVisual aros_vis = NULL;
    GLvisual * vis = NULL;
    GLint  indexBits, redBits, greenBits, blueBits, alphaBits, depthBits, stencilBits, accumBits;
    
    D(bug("[AROSMESA] aros_new_visual\n"));

    /* Allocated memory for aros structure */
    aros_vis = AllocVec(sizeof(struct arosmesa_visual), MEMF_PUBLIC | MEMF_CLEAR);

    if (!aros_vis)
        return NULL;

    vis = GET_GL_VIS_PTR(aros_vis);

    /* Create core visual with default values */
    depthBits = DEFAULT_SOFTWARE_DEPTH_BITS;
    stencilBits = 8;
    accumBits = 16;
    indexBits = 0;
    redBits = CHAN_BITS;
    greenBits = CHAN_BITS;
    blueBits = CHAN_BITS;
    alphaBits = CHAN_BITS;
    
    /* Override default values */
    /* AMA_RGBMode, AMA_DoubleBuf and AMA_AlphaFlag are always GL_TRUE in this implementation */
    stencilBits     = !GetTagData(AMA_NoStencil, GL_FALSE, tagList) ? stencilBits : 0;
    accumBits       = !GetTagData(AMA_NoAccum, GL_FALSE, tagList) ? accumBits : 0;
    depthBits       = !GetTagData(AMA_NoDepth, GL_FALSE, tagList) ? depthBits : 0;

    /* Initialize mesa structure */
    if(!_mesa_initialize_visual(vis,
                                GL_TRUE,        /* RGB */
                                GL_FALSE,       /* Double buffer - AROSMesa uses front buffer as back buffer */
                                GL_FALSE,       /* stereo */
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
                                1)) /* FIXME: xlib code passes 0 - verify */
    {
        _aros_destroy_visual(aros_vis);
        return NULL;
    }

    return aros_vis;
}

static AROSMesaFrameBuffer aros_new_framebuffer(AROSMesaVisual visual)
{
    AROSMesaFrameBuffer aros_fb = NULL;
    enum pipe_format colorFormat, depthFormat, stencilFormat;
    ULONG width, height;

    D(bug("[AROSMESA] aros_new_framebuffer\n"));

    /* Allocated memory for aros structure */
    aros_fb = AllocVec(sizeof(struct arosmesa_framebuffer), MEMF_PUBLIC | MEMF_CLEAR);

    if (!aros_fb)
        return NULL;

    /* FIXME: calculate formats based on visual */
    colorFormat = PIPE_FORMAT_A8R8G8B8_UNORM;
    depthFormat = PIPE_FORMAT_Z16_UNORM;
    stencilFormat = PIPE_FORMAT_S8_UNORM;
    
    
    /* FIXME: calcula width/height from rastport */
    width = 300; height = 300;
    
    /* Create framebuffer, but we'll plug in our own renderbuffers below. */
    aros_fb->stfb = st_create_framebuffer(GET_GL_VIS_PTR(visual),
                                    colorFormat, depthFormat, stencilFormat,
                                    width, height,
                                    (void *) aros_fb);    
    
    
    return aros_fb;
}

static void 
aros_select_rastport(AROSMesaContext amesa, struct TagItem * tagList)
{
    amesa->screen = (struct Screen *)GetTagData(AMA_Screen, 0, tagList);
    amesa->window = (struct Window *)GetTagData(AMA_Window, 0, tagList);
    amesa->visible_rp = (struct RastPort *)GetTagData(AMA_RastPort, 0, tagList);

    if (amesa->screen)
    {
        D(bug("[AROSMESA] aros_select_rastport: Screen @ %x\n", amesa->screen));
        if (amesa->window)
        {
            D(bug("[AROSMESA] aros_select_rastport: Window @ %x\n", amesa->window));
            if (!(amesa->visible_rp))
            {
                /* Use the windows rastport */
                amesa->visible_rp = amesa->window->RPort;
                D(bug("[AROSMESA] aros_select_rastport: Windows RastPort @ %x\n", amesa->visible_rp));
            }
        }
        else
        {
            if (!(amesa->visible_rp))
            {
                /* Use the screens rastport */
                amesa->visible_rp = &amesa->screen->RastPort;
                D(bug("[AROSMESA] aros_select_rastport: Screens RastPort @ %x\n", amesa->visible_rp));
            }
        }
    }
    else
    {
        /* Not passed a screen */
        if (amesa->window)
        {
            D(bug("[AROSMESA] aros_select_rastport: Window @ %x\n", amesa->window));
            /* Use the windows Screen */
            amesa->screen = amesa->window->WScreen;
            D(bug("[AROSMESA] aros_select_rastport: Windows Screen @ %x\n", amesa->screen));

            if (!(amesa->visible_rp))
            {
                /* Use the windows rastport */
                amesa->visible_rp = amesa->window->RPort;
                D(bug("[AROSMESA] aros_select_rastport: Windows RastPort @ %x\n", amesa->visible_rp));
            }
        }
        else
        {
            /* Only Passed A Rastport */
            D(bug("[AROSMESA] aros_select_rastport: Using RastPort only!\n"));
        }
    }

    D(bug("[AROSMESA] aros_select_rastport: Using RastPort @ %x\n", amesa->visible_rp));
}











/*****************************************************************************/
/*                             HACKS                                         */
/*****************************************************************************/

#include "pipe/internal/p_winsys_screen.h"
#include "softpipe/sp_winsys.h"
#include "softpipe/sp_texture.h"
#include "pipe/p_context.h"
#include "util/u_math.h"

struct arosmesa_buffer
{
   struct pipe_buffer base;
   void *data;
   void *mapped;
};

static struct pipe_buffer *
arosmesa_buffer_create(struct pipe_winsys *pws, 
                        unsigned alignment, 
                        unsigned usage,
                        unsigned size)
{
   struct arosmesa_buffer *buffer = AllocVec(sizeof(struct arosmesa_buffer), MEMF_PUBLIC|MEMF_CLEAR);

   pipe_reference_init(&buffer->base.reference, 1);
   buffer->base.alignment = alignment;
   buffer->base.usage = usage;
   buffer->base.size = size;

   if (buffer->data == NULL) {
      /* FIXME: alligment */
      buffer->data = AllocVec(size, MEMF_PUBLIC);
   }

   return &buffer->base;
}

static void *
arosmesa_buffer_map(struct pipe_winsys *pws, struct pipe_buffer *buf,
              unsigned flags)
{
   struct arosmesa_buffer *amesa_buf = (struct arosmesa_buffer *)buf;
   amesa_buf->mapped = amesa_buf->data;
   return amesa_buf->mapped;
}

static void
arosmesa_buffer_unmap(struct pipe_winsys *pws, struct pipe_buffer *buf)
{
   struct arosmesa_buffer *amesa_buf = (struct arosmesa_buffer *)buf;
   amesa_buf->mapped = NULL;
}

static void
arosmesa_buffer_destroy(struct pipe_buffer *buf)
{
   struct arosmesa_buffer *amesa_buf = (struct arosmesa_buffer *)buf;

   if (amesa_buf->data) {
      FreeVec(amesa_buf->data);  
      amesa_buf->data = NULL;
   }

   FreeVec(amesa_buf);
}

static struct pipe_buffer *
arosmesa_surface_buffer_create(struct pipe_winsys *winsys,
                         unsigned width, unsigned height,
                         enum pipe_format format,
                         unsigned usage,
                         unsigned *stride)
{
   const unsigned alignment = 64;
   struct pipe_format_block block;
   unsigned nblocksx, nblocksy;

   pf_get_block(format, &block);
   nblocksx = pf_get_nblocksx(&block, width);
   nblocksy = pf_get_nblocksy(&block, height);
   *stride = align(nblocksx * block.size, alignment);

   return winsys->buffer_create(winsys, alignment,
                                usage,
                                *stride * nblocksy);
}


static void hack_initialize_gallium_structures(AROSMesaContext amesa)
{
    /* THIS FUNCTION IS A HACK TO GET FIRST VERSION OF CODE */
    /* ALL THIS SHOULD BE DONE IN WIN_SYS NOT IN STATE_TRACKER */
    /* SEE Xlib state_tracker/win_sys for proper implementation */
    
    struct pipe_winsys * ws = NULL;
    struct pipe_screen * screen = NULL;
    struct pipe_context * pipe = NULL;
    
    ws = (struct pipe_winsys *)AllocVec(sizeof(struct pipe_winsys), MEMF_PUBLIC|MEMF_CLEAR);
    
    /* FIXME: Attach winsys_functions */
    
    
      ws->buffer_create = arosmesa_buffer_create;
    /*  ws->base.user_buffer_create = xm_user_buffer_create;*/
      ws->buffer_map = arosmesa_buffer_map;
      ws->buffer_unmap = arosmesa_buffer_unmap;
      ws->buffer_destroy = arosmesa_buffer_destroy;

      ws->surface_buffer_create = arosmesa_surface_buffer_create;

      /*ws->base.fence_reference = xm_fence_reference;
      ws->base.fence_signalled = xm_fence_signalled;
      ws->base.fence_finish = xm_fence_finish;

      ws->base.flush_frontbuffer = xm_flush_frontbuffer;
      ws->base.get_name = xm_get_name;*/
    
    screen = softpipe_create_screen(ws);
    
    pipe = softpipe_create(screen);
    
    pipe->priv = (void *)amesa;
    
    amesa->st = st_create_context(pipe, GET_GL_VIS_PTR(amesa->visual), NULL);
    
    amesa->st->ctx->DriverCtx = amesa;
    
    /* NO ERROR CHECKING - THIS IS JUST A HACK */
}

static void hack_display(AROSMesaContext amesa, struct pipe_surface * surf)
{
   struct softpipe_texture *spt = softpipe_texture(surf->texture);
   struct arosmesa_buffer *amesa_buf = (struct arosmesa_buffer *)(spt->buffer);
    
       WritePixelArray(
        amesa_buf->data, 
        0,
        0,
        4 * 300 /*GET_GL_RB_PTR(amesa->renderbuffer)->Width*/, 
        amesa->visible_rp, 
        10 /*amesa->left*/, 
        20 /*amesa->top*/, 
        300 /*amesa->width*/, 
        300 /*amesa->height*/, 
        AROS_PIXFMT);
}















/*****************************************************************************/
/*                             PUBLIC FUNCTIONS                              */
/*****************************************************************************/



AROSMesaContext AROSMesaCreateContextTags(long Tag1, ...)
{
  return AROSMesaCreateContext((struct TagItem *)&Tag1);
}

AROSMesaContext AROSMesaCreateContext(struct TagItem *tagList)
{
    AROSMesaContext amesa = NULL;
    /* LastError = 0; */ /* FIXME: verify usage of LastError - should it be part of AROSMesaContext ? */
    
    /* Try to open cybergraphics.library */
    if (CyberGfxBase == NULL)
    {
        if (!(CyberGfxBase = OpenLibrary((UBYTE*)"cybergraphics.library",0)))
            return NULL;
    }
    
    /* Allocate arosmesa_context struct initialized to zeros */
    if (!(amesa = (AROSMesaContext)AllocVec(sizeof(struct arosmesa_context), MEMF_PUBLIC|MEMF_CLEAR)))
    {
        D(bug("[AROSMESA] AROSMesaCreateContext: ERROR - failed to allocate AROSMesaContext\n"));
        /* LastError = AMESA_OUT_OF_MEM; */ /* FIXME: verify usage of LastError - should it be part of AROSMesaContext ? */
        return NULL;
    }

    /* FIXME; shouldn't RastPort be part of framebuffer */
    aros_select_rastport(amesa, tagList);
    
    D(bug("[AROSMESA] AROSMesaCreateContext: Creating new AROSMesaVisual\n"));

    if (!(amesa->visual = aros_new_visual(tagList)))
    {
        D(bug("[AROSMESA] AROSMesaCreateContext: ERROR -  failed to create AROSMesaVisual\n"));
        FreeVec( amesa );
        /* LastError = AMESA_OUT_OF_MEM; */ /* FIXME: verify usage of LastError - should it be part of AROSMesaContext ? */
        return NULL;
    }
    
    /* FIXME: This is only hack of initilization. See Xlib state_tracker for proper initilization */
    hack_initialize_gallium_structures(amesa);
    
    /* FIXME: Provide rastport to framebuffer ? */
    amesa->framebuffer = aros_new_framebuffer(amesa->visual);
    
    return amesa;
}

void AROSMesaMakeCurrent(AROSMesaContext amesa)
{
    /* FIXME: if passed context is the same as current context, check buffer sizes */
    /* FIXME: if there was old context active, flush it and NULL the pointers to buffers */
    
    if (amesa)
    {
        st_make_current(amesa->st, amesa->framebuffer->stfb, amesa->framebuffer->stfb);
        
        /* FIXME: add buffers resizing */
        st_resize_framebuffer(amesa->framebuffer->stfb, 300/*width*/, 300 /*height*/);
        /* xmesa_check_and_update_buffer_size */
    }
    else
    {
        /* Detach */
        st_make_current( NULL, NULL, NULL );
    }
        
}

void AROSMesaSwapBuffers(AROSMesaContext amesa)
{
   struct pipe_surface *surf;

   /* If we're swapping the buffer associated with the current context
    * we have to flush any pending rendering commands first.
    */
   st_notify_swapbuffers(amesa->framebuffer->stfb);

   
   /* FIXME: should be ST_SURFACE_BACK_LEFT */
   st_get_framebuffer_surface(amesa->framebuffer->stfb, ST_SURFACE_FRONT_LEFT, &surf);
   
   /* FIXME: display should be the method of driver */
   /*if (surf) {
      driver.display_surface(b, surf);
   }*/
   
   hack_display(amesa, surf);

    /* FIXME: update size? */
   /* xmesa_check_and_update_buffer_size(NULL, b);*/
}

void AROSMesaDestroyContext(AROSMesaContext amesa)
{
}
