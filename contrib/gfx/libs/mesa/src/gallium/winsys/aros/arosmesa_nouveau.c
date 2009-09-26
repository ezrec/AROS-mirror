/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "arosmesa_winsys.h"
#include "nouveau/nouveau_winsys.h"

/* Code from DRM winsys */
#include "nouveau_winsys_pipe.h"
#include "nouveau_drmif.h"
/* Code from DRM winsys */

static int
arosmesa_open_nouveau_device(struct nouveau_device **dev)
{
    return nouveau_device_open(dev, "DUMMY");
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
    case 0x00:
        init = nv04_screen_create;
        break;
//     case 0x10:
//         init = nv10_screen_create;
//         break;
    case 0x20:
        init = nv20_screen_create;
        break;
//     case 0x30:
//         init = nv30_screen_create;
//         break;
    case 0x40:
    case 0x60:
        init = nv40_screen_create;
        break;
/*    case 0x80:
    case 0x90:
    case 0xa0:
        init = nv50_screen_create;
        break;*/
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
    case 0x00:
        init = nv04_create;
        break;
/*    case 0x10:
        init = nv10_create;
        break;*/
    case 0x20:
        init = nv20_create;
        break;
/*    case 0x30:
        init = nv30_create;
        break;*/
    case 0x40:
    case 0x60:
        init = nv40_create;
        break;
/*    case 0x80:
    case 0x90:
    case 0xa0:
        init = nv50_create;
        break;*/
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

struct arosmesa_driver arosmesa_nouveau_driver = 
{
   .create_pipe_screen = arosmesa_create_nouveau_screen,
   .create_pipe_context = arosmesa_create_nouveau_context,
   .display_surface = NULL
};



