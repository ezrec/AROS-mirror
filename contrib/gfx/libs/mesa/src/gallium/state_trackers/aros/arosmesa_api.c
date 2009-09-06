/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/


#include "arosmesa_internal.h"

#include "main/context.h"

#include <proto/exec.h>
#include <proto/utility.h>
#include <aros/debug.h>

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



/*****************************************************************************/
/*                             HACKS                                         */
/*****************************************************************************/

#include "pipe/internal/p_winsys_screen.h"
#include "softpipe/sp_winsys.h"
#include "state_tracker/st_public.h"
#include "pipe/p_context.h"

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
    
    /*
          ws->base.buffer_create = xm_buffer_create;
      ws->base.user_buffer_create = xm_user_buffer_create;
      ws->base.buffer_map = xm_buffer_map;
      ws->base.buffer_unmap = xm_buffer_unmap;
      ws->base.buffer_destroy = xm_buffer_destroy;

      ws->base.surface_buffer_create = xm_surface_buffer_create;

      ws->base.fence_reference = xm_fence_reference;
      ws->base.fence_signalled = xm_fence_signalled;
      ws->base.fence_finish = xm_fence_finish;

      ws->base.flush_frontbuffer = xm_flush_frontbuffer;
      ws->base.get_name = xm_get_name;*/
    
    screen = softpipe_create_screen(ws);
    
    pipe = softpipe_create(screen);
    
    pipe->priv = (void *)amesa;
    
    amesa->st = st_create_context(pipe, NULL, /*GET_GL_VIS_PTR(amesa->visual),*/ NULL);
    
    /* NO ERROR CHECKING - THIS IS JUST A HACK */
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
    
    /* Allocate arosmesa_context struct initialized to zeros */
    if (!(amesa = (AROSMesaContext)AllocVec(sizeof(struct arosmesa_context), MEMF_PUBLIC|MEMF_CLEAR)))
    {
        D(bug("[AROSMESA] AROSMesaCreateContext: ERROR - failed to allocate AROSMesaContext\n"));
        /* LastError = AMESA_OUT_OF_MEM; */ /* FIXME: verify usage of LastError - should it be part of AROSMesaContext ? */
        return NULL;
    }

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
    
    
    return amesa;
}

void AROSMesaMakeCurrent(AROSMesaContext amesa)
{
}

void AROSMesaSwapBuffers(AROSMesaContext amesa)
{
}

void AROSMesaDestroyContext(AROSMesaContext amesa)
{
}
