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

// #include <proto/exec.h>
// #include <proto/cybergraphics.h>
// #include <cybergraphx/cybergraphics.h>
// 
// #if (AROS_BIG_ENDIAN == 1)
// #define AROS_PIXFMT RECTFMT_ARGB32   /* Big Endian Archs. */
// #else
// #define AROS_PIXFMT RECTFMT_BGRA32   /* Little Endian Archs. */
// #endif

static struct pipe_surface * whole_screen;

static void
arosmesa_nouveau_display_surface(AROSMesaContext amesa,
                              struct pipe_surface *surf)
{
    struct pipe_context *pipe = amesa->st->pipe;
//     struct pipe_screen *screen = pipe->screen;
//     struct pipe_transfer *src_trans;
//     const void *src_map;
// 
//     if (!surf->texture)
//         return;
//    
//     src_trans = screen->get_tex_transfer(screen,
//                                         surf->texture,
//                                         surf->face,
//                                         surf->level,
//                                         surf->zslice,
//                                         PIPE_TRANSFER_READ,
//                                         0, 0, 300, 300);
//     src_map = screen->transfer_map(screen, src_trans);
//     
//      WritePixelArray(
//      src_map, 
//         0,
//         0,
//         304,
//         amesa->visible_rp, 
//         amesa->left, 
//         amesa->top, 
//         amesa->width, 
//         amesa->height, 
//         AROS_PIXFMT);
//         
//    screen->transfer_unmap(pipe->screen, src_trans);
//    
//    screen->tex_transfer_destroy(src_trans);

pipe->surface_copy(pipe, whole_screen, 0, 300, surf, 0, 0, 300, 300);

//  pipe->surface_fill(pipe, whole_screen, 300, 300, 300, 300, 0x00ff0000);
//  pipe->surface_fill(pipe, whole_screen, 350, 350, 300, 300, 0x0000ff00); 
//  pipe->surface_fill(pipe, whole_screen, 400, 400, 300, 300, 0x000000ff); 
//  pipe->surface_fill(pipe, whole_screen, 450, 450, 300, 300, 0xaaaaaaaa); 
    return;
}

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
        /* FIXME: compatibility between libdrm and gallium nouveau */
//     case 0x80:
//     case 0x90:
//     case 0xa0:
//         init = nv50_screen_create;
//         break;
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
        /* FIXME: compatibility between libdrm and gallium nouveau */
//     case 0x80:
//     case 0x90:
//     case 0xa0:
//         init = nv50_create;
//         break;
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
nouveau_drm_pb_from_handle(struct pipe_screen *pscreen, const char *name,
               unsigned handle)
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
arosmesa_nouveau_dummy(struct pipe_screen * screen)
{
   struct pipe_surface *surface = NULL;
   struct pipe_texture *texture = NULL;
   struct pipe_texture templat;
   struct pipe_buffer *buf = NULL;
    unsigned pitch = 1024 * 4; /* FIXME: What this represents? width * bpp / 8 */
   
   
   buf = nouveau_drm_pb_from_handle(screen, "front buffer", 123456 /*makes no difference anyway */);
   if (!buf)
      return;

   memset(&templat, 0, sizeof(templat));
   templat.tex_usage = PIPE_TEXTURE_USAGE_PRIMARY |
                       NOUVEAU_TEXTURE_USAGE_LINEAR;
   templat.target = PIPE_TEXTURE_2D;
   templat.last_level = 0;
   templat.depth[0] = 1;
   templat.format = PIPE_FORMAT_A8R8G8B8_UNORM; /*FIXME:format*/
   templat.width[0] = 1024 /*FIXME:width*/;
   templat.height[0] = 768 /*FIXME:height*/;
   pf_get_block(templat.format, &templat.block);

   texture = screen->texture_blanket(screen,
                                     &templat,
                                     &pitch,
                                     buf);

   /* we don't need the buffer from this point on */
   pipe_buffer_reference(&buf, NULL);

   if (!texture)
      return;

   surface = screen->get_tex_surface(screen, texture, 0, 0, 0,
                                     PIPE_BUFFER_USAGE_GPU_READ |
                                     PIPE_BUFFER_USAGE_GPU_WRITE);

   /* we don't need the texture from this point on */
   pipe_texture_reference(&texture, NULL);
   
   whole_screen = surface;
}

struct arosmesa_driver arosmesa_nouveau_driver = 
{
   .create_pipe_screen = arosmesa_create_nouveau_screen,
   .create_pipe_context = arosmesa_create_nouveau_context,
   .display_surface = arosmesa_nouveau_display_surface,
   .dummy = arosmesa_nouveau_dummy
};



