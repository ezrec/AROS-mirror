/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "arosmesa_winsys.h"
#include "nouveau/nouveau_winsys.h"
#include "util/u_memory.h"

/* Code from DRM winsys */
#include "nouveau_winsys_pipe.h"
#include "nouveau_drmif.h"
/* Code from DRM winsys */

#define DEBUG 0
#include <aros/debug.h>

#include <proto/graphics.h>

static void
arosmesa_nouveau_display_surface(AROSMesaContext amesa,
                              struct pipe_surface *surf)
{
    struct pipe_context *pipe = amesa->st->pipe;
    
    struct Layer *L = amesa->visible_rp->Layer;
    struct ClipRect *CR;
    
    if (amesa->screen_surface == NULL)
    {
        D(bug("Screen surface not provided\n"));
        return;
    }
    
    if (L == NULL)
    {
        D(bug("Layer not provided\n"));
        /* FIXME: Implement rendering path - render at 0,0 full size of buffer? */
        return;
    }

    /* FIXME: Check if layer is visible at all? */

    LockLayerRom(L);
    
    CR = L->ClipRect;
    
    for (;NULL != CR; CR = CR->Next)
    {
/*        D(bug("Cliprect (%d, %d, %d, %d), lobs=%p\n",
            CR->bounds.MinX, CR->bounds.MinY, CR->bounds.MaxX, CR->bounds.MaxY,
            CR->lobs));*/

        /* I assume this means the cliprect is visible */
        if (NULL == CR->lobs)
        {
            /* FIXME: Use rp clip rect and AND operation? !!UNIFY!! approach with _aros_recalculate_buffer_width_height */
            LONG xmin, ymin, xmax, ymax;
            
            /* Greater wins */
            xmin = L->bounds.MinX + amesa->left;
            if (CR->bounds.MinX > xmin) xmin = CR->bounds.MinX;
            
            /* Smaller wins */
            xmax = L->bounds.MaxX - amesa->right;
            if (CR->bounds.MaxX < xmax) xmax = CR->bounds.MaxX;
            
            /* Greater wins */
            ymin = L->bounds.MinY + amesa->top;
            if (CR->bounds.MinY > ymin) ymin = CR->bounds.MinY;
            
            /* Smaller wins */
            ymax = L->bounds.MaxY - amesa->bottom;
            if (CR->bounds.MaxY < ymax) ymax = CR->bounds.MaxY;
            
            
            /* FIXME: clip last 4 parameters to actuall surface deminsions */
            pipe->surface_copy(pipe, amesa->screen_surface, 
                        xmin, 
                        ymin, 
                        surf, 
                        xmin - L->bounds.MinX - amesa->left, 
                        ymin - L->bounds.MinY - amesa->top, 
                        xmax - xmin + 1, 
                        ymax - ymin + 1);
        }
    }

    UnlockLayerRom(L);
    
    /* FIXME: There is still something wrong with this code:
       a) have another window under this window
       b) move this another window to front
       The hidden part of another window will not be redrawn until the other window is move
       as if moving another window to front did not change the cliprects of our window
       It migh assume that since the hidden window is now on top, it will simply override what
       our window has draw - but our window renders direclty to framebuffer, so it overrides the
       top window */
}

static int
arosmesa_open_nouveau_device(struct nouveau_device **dev)
{
    return nouveau_device_open(dev, "");
}

static void
arosmesa_nouveau_flush_frontbuffer(struct pipe_winsys *pws,
                     struct pipe_surface *surf,
                     void *context_private)
{
    /* No Op */
}

static void 
arosmesa_nouveau_update_buffer( struct pipe_winsys *ws, void *context_private )
{
    /* No Op */
}


static struct pipe_screen *
arosmesa_create_nouveau_screen( void )
{
    struct pipe_winsys *ws;
    struct nouveau_winsys *nvws;
    struct nouveau_device *dev = NULL;
    struct pipe_screen *(*init)(struct pipe_winsys *,
                    struct nouveau_winsys *);
    int ret;

    ret = arosmesa_open_nouveau_device(&dev);
    if (ret)
        return NULL;

    switch (dev->chipset & 0xf0) {
    /* NV04 codebase is not in-sync with gallium API */
//     case 0x00:
//         init = nv04_screen_create;
//         break;
    case 0x10:
        init = nv10_screen_create;
        break;
    case 0x20:
        init = nv20_screen_create;
        break;
    case 0x30:
        init = nv30_screen_create;
        break;
    case 0x40:
    case 0x60:
        init = nv40_screen_create;
        break;
    case 0x80:
    case 0x90:
    case 0xa0:
        init = nv50_screen_create;
        break;
    default:
        debug_printf("%s: unknown chipset nv%02x\n", __func__,
                 dev->chipset);
        return NULL;
    }

    ws = nouveau_pipe_winsys_new(dev);
    if (!ws) {
        nouveau_device_close(&dev);
        return NULL;
    }
    
    /* Install pipe_winsys function */
    ws->flush_frontbuffer = arosmesa_nouveau_flush_frontbuffer;
    ws->update_buffer = arosmesa_nouveau_update_buffer;

    nvws = nouveau_winsys_new(ws);
    if (!nvws) {
        ws->destroy(ws);
        return NULL;
    }

    nouveau_pipe_winsys(ws)->pscreen = init(ws, nvws);
    if (!nouveau_pipe_winsys(ws)->pscreen) {
        ws->destroy(ws);
        return NULL;
    }

    return nouveau_pipe_winsys(ws)->pscreen;    
}

static struct pipe_context *
arosmesa_create_nouveau_context( struct pipe_screen *screen )
{
    struct nouveau_pipe_winsys *nvpws = nouveau_screen(screen);
    struct pipe_context *(*init)(struct pipe_screen *, unsigned);
    unsigned chipset = nvpws->channel->device->chipset;
    int i;

    switch (chipset & 0xf0) {
    /* NV04 codebase is not in-sync with gallium API */        
//     case 0x00:
//         init = nv04_create;
//         break;
    case 0x10:
        init = nv10_create;
        break;
    case 0x20:
        init = nv20_create;
        break;
    case 0x30:
        init = nv30_create;
        break;
    case 0x40:
    case 0x60:
        init = nv40_create;
        break;
    case 0x80:
    case 0x90:
    case 0xa0:
        init = nv50_create;
        break;
    default:
        debug_printf("%s: unknown chipset nv%02x\n", __func__, chipset);
        return NULL;
    }

    /* Find a free slot for a pipe context, allocate a new one if needed */
    for (i = 0; i < nvpws->nr_pctx; i++) {
        if (nvpws->pctx[i] == NULL)
            break;
    }

    if (i == nvpws->nr_pctx) {
        nvpws->nr_pctx++;
        nvpws->pctx = realloc(nvpws->pctx,
                      sizeof(*nvpws->pctx) * nvpws->nr_pctx);
    }

    nvpws->pctx[i] = init(screen, i);
    return nvpws->pctx[i];
}

static struct pipe_buffer *
nouveau_drm_pb_from_handle(struct pipe_screen *pscreen, unsigned handle)
{
    struct nouveau_pipe_winsys *nvpws = nouveau_screen(pscreen);
    struct nouveau_device *dev = nvpws->channel->device;
    struct nouveau_pipe_buffer *nvpb;
    int ret;

    nvpb = CALLOC_STRUCT(nouveau_pipe_buffer);
    if (!nvpb)
        return NULL;

    ret = nouveau_bo_handle_ref(dev, handle, &nvpb->bo);
    if (ret) {
        debug_printf("%s: ref name 0x%08x failed with %d\n",
                 __func__, handle, ret);
        FREE(nvpb);
        return NULL;
    }

    pipe_reference_init(&nvpb->base.reference, 1);
    nvpb->base.screen = pscreen;
    nvpb->base.alignment = 0;
    nvpb->base.usage = PIPE_BUFFER_USAGE_GPU_READ_WRITE |
               PIPE_BUFFER_USAGE_CPU_READ_WRITE;
    nvpb->base.size = nvpb->bo->size;
    return &nvpb->base;
}

static void
arosmesa_nouveau_cleanup( struct pipe_screen * screen )
{
    if (screen)
    {
        /* First destroy the screen, then the winsys */
        struct pipe_winsys * winsys = screen->winsys;
        
        screen->destroy(screen);
        
        if (winsys)
        {
            winsys->destroy(winsys);
        }
    }
}

/* HACK!!! */
struct pipe_screen * protect_screen = NULL;

static void HACK_protect_screen_fb(int size)
{
    struct nouveau_pipe_winsys *nvpws = NULL;
    struct nouveau_device *dev = NULL;
    struct nouveau_bo *bo = NULL;
    
    /* Create a pipe_screen separate from any actual client */
    /* FIXME: This screen needs to be destroyed */
    if(!protect_screen) 
        protect_screen = arosmesa_create_nouveau_screen();
    else
        return;
    
    nvpws = nouveau_screen(protect_screen);
    dev = nvpws->channel->device;
    
    /* HACK - UNHOLY */
    /* IF WE ARE LUCKY:
    A) VISIBLE FRAMEBUFFER IS ALLOCATED AT FRAMEBUFFER OFFSET 0
    B) THERE ARE NO ALLOCATATION ON FB HEAP
    THEN WE WILL 'PROTECT' THE VISIBLE FRAMEBUFFER WITH THIS ALLOCATION */
    
    nouveau_bo_new(dev, NOUVEAU_BO_VRAM | NOUVEAU_BO_PIN, 0, size, &bo);
}

void HACK_unprotect_screen_fb(void)
{
    if (protect_screen) 
    {
        arosmesa_nouveau_cleanup(protect_screen);
        protect_screen = NULL;
    }
}



static struct pipe_surface *
arosmesa_nouveau_get_screen_surface(struct pipe_screen * screen, int width, int height, int bpp)
{
    struct pipe_surface *surface = NULL;
    struct pipe_texture *texture = NULL;
    struct pipe_texture templat;
    struct pipe_buffer *buf = NULL;
    unsigned pitch = width * bpp / 8;

    /* FIXME: This should be done at driver start! */
    HACK_protect_screen_fb(pitch * height);

    #warning hardcoded start of visible framebuffer
    /* FIXME: The third parameter represents the offset to start of visible framebuffer - this must be read, not hardcoded 
    nvidia hidd registers_base + x00600000 + (0x800/4)
    */

    buf = nouveau_drm_pb_from_handle(screen, 0 /*FIXME: THIS VALUE MATTERS!!!! - offset in GFX memory where visible framebuffer starts */);
    if (!buf)
        return NULL;

    memset(&templat, 0, sizeof(templat));
    templat.tex_usage = PIPE_TEXTURE_USAGE_PRIMARY |
                        NOUVEAU_TEXTURE_USAGE_LINEAR;
    templat.target = PIPE_TEXTURE_2D;
    templat.last_level = 0;
    templat.depth[0] = 1;
    switch(bpp)
    {
        case(16):
            templat.format = PIPE_FORMAT_R5G6B5_UNORM;
            break;
        default:
            /* FIXME: should fail for anything other than 32bit */
            templat.format = PIPE_FORMAT_A8R8G8B8_UNORM;
            break;
    }
    templat.width[0] = width;
    templat.height[0] = height;
    pf_get_block(templat.format, &templat.block);

    texture = screen->texture_blanket(screen,
                                        &templat,
                                        &pitch,
                                        buf);

    /* we don't need the buffer from this point on */
    pipe_buffer_reference(&buf, NULL);

    if (!texture)
        return NULL;

    surface = screen->get_tex_surface(screen, texture, 0, 0, 0,
                                        PIPE_BUFFER_USAGE_GPU_READ |
                                        PIPE_BUFFER_USAGE_GPU_WRITE);

    /* we don't need the texture from this point on */
    pipe_texture_reference(&texture, NULL);

    return surface;
}

static void
arosmesa_nouveau_query_depth_stencil(int color, int * depth, int * stencil)
{
    (*depth)    = 24;
    (*stencil)  = 8;
}

struct arosmesa_driver arosmesa_nouveau_driver = 
{
    .create_pipe_screen = arosmesa_create_nouveau_screen,
    .create_pipe_context = arosmesa_create_nouveau_context,
    .display_surface = arosmesa_nouveau_display_surface,
    .get_screen_surface = arosmesa_nouveau_get_screen_surface,
    .cleanup = arosmesa_nouveau_cleanup,
    .query_depth_stencil = arosmesa_nouveau_query_depth_stencil,
};



