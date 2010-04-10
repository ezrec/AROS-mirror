/*
    Copyright 2010, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <hidd/gallium.h>
#include <proto/oop.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <aros/debug.h>

#include "nouveau.h"

#include "util/u_memory.h"
#include "util/u_inlines.h"
#include "nouveau/nouveau_screen.h"
#include "nouveau/nouveau_drmif.h"
#include "nouveau/nouveau_winsys.h"

#undef HiddGalliumBaseDriverAttrBase
#define HiddGalliumBaseDriverAttrBase   (SD(cl)->hiddGalliumBaseDriverAB)

struct HiddNouveauWinSys {
    struct pipe_winsys base;

    struct pipe_screen *pscreen;

    /* Surface covering the whole visible screen */
    struct pipe_surface * visiblescreen;
};

static INLINE struct HiddNouveauWinSys *
HiddNouveauWinSys(struct pipe_winsys *ws)
{
    return (struct HiddNouveauWinSys *)ws;
}

static void 
HiddNouveauFlushFrontBuffer( struct pipe_screen *screen,
                            struct pipe_surface *surf,
                            void *context_private )
{
    /* No Op */
}

static void
HiddNouveauDestroyWinSys(struct pipe_winsys *ws)
{
    struct HiddNouveauWinSys *nvws = HiddNouveauWinSys(ws);

    nouveau_device_close(&(nouveau_screen(nvws->pscreen)->device));
    FREE(nvws);
}

static struct pipe_buffer *
HiddNouveauPipeBufferFromHandle(struct pipe_screen *pscreen, ULONG handle)
{
    struct nouveau_device *dev = nouveau_screen(pscreen)->device;
    struct pipe_buffer *pb;
    int ret;

    pb = CALLOC(1, sizeof(struct pipe_buffer) + sizeof(struct nouveau_bo*));
    if (!pb)
        return NULL;

    ret = nouveau_bo_handle_ref(dev, handle, (struct nouveau_bo**)(pb+1));
    if (ret) {
        D(bug("%s: ref name 0x%08x failed with %d\n",
                 __func__, handle, ret));
        FREE(pb);
        return NULL;
    }

    pipe_reference_init(&pb->reference, 1);
    pb->screen = pscreen;
    pb->alignment = 0;
    pb->usage = PIPE_BUFFER_USAGE_GPU_READ_WRITE |
            PIPE_BUFFER_USAGE_CPU_READ_WRITE;
    pb->size = nouveau_bo(pb)->size;
    
    return pb;
}

/* ========================================================================== */
#include <proto/cybergraphics.h>
#include <proto/intuition.h>
#include <cybergraphx/cybergraphics.h>

static BOOL HiddNouveauGetScreenParameters(LONG * width, LONG * height, LONG * bpp)
{
    /* HACK */
    /* This only works, because in VESA mode it is not possible to change resolution at
       runtime or have screens of different resolution, so the resolution is always
       fixed to default public screen resolution */
    struct Screen * pubscreen;
    struct Library * CyberGfxBase = NULL;
    
    CyberGfxBase = OpenLibrary("cybergraphics.library", 0);
    
    if (!CyberGfxBase)
        return FALSE;
    
    pubscreen = LockPubScreen(NULL);
    
    if (!pubscreen)
        return FALSE;

    *bpp    = GetCyberMapAttr(pubscreen->RastPort.BitMap, CYBRMATTR_BPPIX) * 8;
    *width  = GetCyberMapAttr(pubscreen->RastPort.BitMap, CYBRMATTR_WIDTH);
    *height = GetCyberMapAttr(pubscreen->RastPort.BitMap, CYBRMATTR_HEIGHT); 
    
    UnlockPubScreen(NULL, pubscreen);
    
    CloseLibrary(CyberGfxBase);

    return TRUE;
}

/* ========================================================================== */

static struct pipe_surface *
HiddNouveauGetScreenSurface(struct pipe_screen * screen)
{
    struct pipe_surface *surface = NULL;
    struct pipe_texture *texture = NULL;
    struct pipe_texture templat;
    struct pipe_buffer *buf = NULL;
    LONG width = 0;
    LONG height = 0;
    LONG bpp = 0;
    unsigned pitch = 0;

    if (!HiddNouveauGetScreenParameters(&width, &height, &bpp))
        return NULL;
    
    pitch = width * bpp / 8;

    buf = HiddNouveauPipeBufferFromHandle(screen, 1 /* driver makes sure object with ID 1 is visible framebuffer */);
    if (!buf)
        return NULL;

    memset(&templat, 0, sizeof(templat));
    templat.tex_usage = PIPE_TEXTURE_USAGE_PRIMARY |
                        NOUVEAU_TEXTURE_USAGE_LINEAR;
    templat.target = PIPE_TEXTURE_2D;
    templat.last_level = 0;
    templat.depth0 = 1;
    switch(bpp)
    {
        case(16):
            templat.format = PIPE_FORMAT_B5G6R5_UNORM;
            break;
        case(32):
            templat.format = PIPE_FORMAT_B8G8R8A8_UNORM;
            break;
        default:
            /* Fail */
            pipe_buffer_reference(&buf, NULL);
            return NULL;
    }
    templat.width0 = width;
    templat.height0 = height;

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

/* METHODS */
OOP_Object *METHOD(GALLIUMNOUVEAUDRIVER, Root, New)
{
    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg) msg);

    return o;
}

VOID METHOD(GALLIUMNOUVEAUDRIVER, Root, Get)
{
    ULONG idx;

    if (IS_GALLIUMBASEDRIVER_ATTR(msg->attrID, idx))
    {
        switch (idx)
        {
            /* Overload the property */
            case aoHidd_GalliumBaseDriver_GalliumInterfaceVersion:
                *msg->storage = GALLIUM_INTERFACE_VERSION;
                return;
        }
    }
    
    /* Use parent class for all other properties */
    OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
}

APTR METHOD(GALLIUMNOUVEAUDRIVER, Hidd_GalliumBaseDriver, CreatePipeScreen)
{
    struct HiddNouveauWinSys *nvws;
    struct pipe_winsys *ws;
    struct nouveau_device *dev = NULL;
    struct pipe_screen *(*init)(struct pipe_winsys *,
                    struct nouveau_device *);
    int ret;

    ret = nouveau_device_open(&dev, "");

    if (ret)
        return NULL;

    switch (dev->chipset & 0xf0) 
    {
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
        D(bug("%s: unknown chipset nv%02x\n", __func__,
                 dev->chipset));
        return NULL;
    }


    nvws = CALLOC_STRUCT(HiddNouveauWinSys);
    if (!nvws) {
        nouveau_device_close(&dev);
        return NULL;
    }
    ws = &nvws->base;
    ws->destroy = HiddNouveauDestroyWinSys;

    nvws->pscreen = init(ws, dev);
    if (!nvws->pscreen) {
        ws->destroy(ws);
        return NULL;
    }
    
    nvws->pscreen->flush_frontbuffer = HiddNouveauFlushFrontBuffer;

    /* Get surface for the whole screen */
    nvws->visiblescreen = HiddNouveauGetScreenSurface(nvws->pscreen);
    
    return nvws->pscreen;
}

VOID METHOD(GALLIUMNOUVEAUDRIVER, Hidd_GalliumBaseDriver, QueryDepthStencil)
{
    *msg->depthbits = 24;
    *msg->stencilbits = 8;
}

VOID METHOD(GALLIUMNOUVEAUDRIVER, Hidd_GalliumBaseDriver, DisplaySurface)
{
    struct Layer *L = ((struct RastPort *)msg->rastport)->Layer;
    struct ClipRect *CR;
    struct Rectangle renderableLayerRect;
    struct pipe_context * pipe = (struct pipe_context *)msg->context;
    struct pipe_surface * surface = (struct pipe_surface *)msg->surface;
    struct pipe_surface * visiblescreen = ((struct HiddNouveauWinSys*)pipe->winsys)->visiblescreen;

    if (visiblescreen == NULL)
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

    if (!IsLayerVisible(L))
        return;

    LockLayerRom(L);
    
    renderableLayerRect.MinX = L->bounds.MinX + msg->left;
    renderableLayerRect.MaxX = L->bounds.MaxX - msg->right;
    renderableLayerRect.MinY = L->bounds.MinY + msg->top;
    renderableLayerRect.MaxY = L->bounds.MaxY - msg->bottom;
    
    /*  Do not clip renderableLayerRect to screen rast port. CRs are already clipped and unclipped 
        layer coords are needed: see surface_copy */
    
    CR = L->ClipRect;
    
    for (;NULL != CR; CR = CR->Next)
    {
        D(bug("Cliprect (%d, %d, %d, %d), lobs=%p\n",
            CR->bounds.MinX, CR->bounds.MinY, CR->bounds.MaxX, CR->bounds.MaxY,
            CR->lobs));

        /* I assume this means the cliprect is visible */
        if (NULL == CR->lobs)
        {
            struct Rectangle result;
            
            if (AndRectRect(&renderableLayerRect, &CR->bounds, &result))
            {
                /* This clip rect intersects renderable layer rect */
                
                /* FIXME: clip last 4 parameters to actuall surface deminsions */
                pipe->surface_copy(pipe, visiblescreen, 
                            result.MinX, 
                            result.MinY, 
                            surface, 
                            result.MinX - L->bounds.MinX - msg->left, 
                            result.MinY - L->bounds.MinY - msg->top, 
                            result.MaxX - result.MinX + 1, 
                            result.MaxY - result.MinY + 1);
            }
        }
    }

    /* Flush all copy operations done */
    pipe->flush(pipe, PIPE_FLUSH_RENDER_CACHE, NULL);

    UnlockLayerRom(L);
}

VOID METHOD(GALLIUMNOUVEAUDRIVER, Hidd_GalliumBaseDriver, DestroyPipeScreen)
{
    struct pipe_screen * screen = (struct pipe_screen *)msg->screen;

    if (screen)
    {
        /* Release visible screen surface reference */
        if (((struct HiddNouveauWinSys*)screen->winsys)->visiblescreen)
            pipe_surface_reference(&(((struct HiddNouveauWinSys*)screen->winsys)->visiblescreen), NULL);

        screen->destroy(screen);
    }
}
