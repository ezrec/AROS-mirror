/*
    Copyright 2010, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <hidd/gallium.h>
#include <proto/oop.h>
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

    unsigned nr_pctx;
    struct pipe_context **pctx;

    struct pipe_surface *front;
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
        debug_printf("%s: unknown chipset nv%02x\n", __func__,
                 dev->chipset);
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
    
    return nvws->pscreen;
}

VOID METHOD(GALLIUMNOUVEAUDRIVER, Hidd_GalliumBaseDriver, QueryDepthStencil)
{
    *msg->depthbits = 24;
    *msg->stencilbits = 8;
}

VOID METHOD(GALLIUMNOUVEAUDRIVER, Hidd_GalliumBaseDriver, DisplaySurface)
{
    /* TODO : Implement */
}
