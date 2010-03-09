/*
    Copyright 2010, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <hidd/gallium.h>
#include <proto/oop.h>
#include <aros/debug.h>

#include "softpipe/sp_texture.h"
#include "softpipe/sp_winsys.h"
#include "pipe/p_context.h"
#include "util/u_simple_screen.h"
#include "util/u_format.h"
#include "util/u_math.h"
#include "util/u_inlines.h"

#include "softpipe.h"

#include <proto/cybergraphics.h>
#include <proto/graphics.h>
#include <cybergraphx/cybergraphics.h>

#if (AROS_BIG_ENDIAN == 1)
#define AROS_PIXFMT RECTFMT_ARGB32   /* Big Endian Archs. */
#else
#define AROS_PIXFMT RECTFMT_BGRA32   /* Little Endian Archs. */
#endif

#define CyberGfxBase    (&BASE(cl->UserData)->sd)->SoftpipeCyberGfxBase

#undef HiddGalliumBaseDriverAttrBase
#define HiddGalliumBaseDriverAttrBase   (SD(cl)->hiddGalliumBaseDriverAB)

struct arosmesa_buffer
{
    struct pipe_buffer base;
    void *buffer; /* Real buffer pointer */
    void *data; /* Aligned buffer pointer (inside real buffer) */
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
    if (buffer->buffer == NULL) {
        /* Alligment */
        buffer->buffer = AllocVec(size + alignment - 1, MEMF_PUBLIC);
        buffer->data = (void *)(((IPTR)buffer->buffer + (alignment - 1)) & ~(alignment - 1));
    }

    return &buffer->base;
}

static struct pipe_buffer *
arosmesa_user_buffer_create(struct pipe_winsys *pws, void *ptr, unsigned bytes)
{
   struct arosmesa_buffer *buffer = AllocVec(sizeof(struct arosmesa_buffer), MEMF_PUBLIC|MEMF_CLEAR);
   pipe_reference_init(&buffer->base.reference, 1);
   buffer->base.size = bytes;
   buffer->data = ptr;

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

    if (amesa_buf->buffer) {
        FreeVec(amesa_buf->buffer);  
        amesa_buf->data = NULL;
        amesa_buf->buffer = NULL;
    }

    FreeVec(amesa_buf);
}

static struct pipe_buffer *
arosmesa_surface_buffer_create(struct pipe_winsys *winsys,
                         unsigned width, unsigned height,
                         enum pipe_format format,
                         unsigned usage,
                         unsigned tex_usage,
                         unsigned *stride)
{
    const unsigned alignment = 64;
    unsigned nblocksy;

    nblocksy = util_format_get_nblocksy(format, height);
    *stride = align(util_format_get_stride(format, width), alignment);

    return winsys->buffer_create(winsys, alignment,
                                    usage,
                                    *stride * nblocksy);
}

static void 
arosmesa_softpipe_flush_frontbuffer(struct pipe_winsys *ws,
                                struct pipe_surface *surf,
                                void *context_private)
{
    /* No Op */
}

static void 
arosmesa_softpipe_update_buffer( struct pipe_winsys *ws, void *context_private )
{
    /* No Op */
}

static void
arosmesa_softpipe_destroy( struct pipe_winsys *ws)
{
    FreeVec(ws);
}

static struct pipe_winsys *
arosmesa_create_softpipe_winsys( void )
{
    struct pipe_winsys *ws = NULL;

    ws = (struct pipe_winsys *)AllocVec(sizeof(struct pipe_winsys), MEMF_PUBLIC|MEMF_CLEAR);

    /* Fill in this struct with callbacks that pipe will need to
    * communicate with the window system, buffer manager, etc. 
    */
    ws->buffer_create = arosmesa_buffer_create;
    ws->user_buffer_create = arosmesa_user_buffer_create;
    ws->buffer_map = arosmesa_buffer_map;
    ws->buffer_unmap = arosmesa_buffer_unmap;
    ws->buffer_destroy = arosmesa_buffer_destroy;

    ws->surface_buffer_create = arosmesa_surface_buffer_create;

    ws->fence_reference = NULL; /* FIXME */
    ws->fence_signalled = NULL; /* FIXME */
    ws->fence_finish = NULL; /* FIXME */

    ws->flush_frontbuffer = arosmesa_softpipe_flush_frontbuffer;
    ws->update_buffer = arosmesa_softpipe_update_buffer;
    ws->get_name = NULL; /* FIXME */
    ws->destroy = arosmesa_softpipe_destroy;

    return ws;
}

OOP_Object *METHOD(GALLIUMSOFTPIPEDRIVER, Root, New)
{
    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg) msg);

    return o;
}

VOID METHOD(GALLIUMSOFTPIPEDRIVER, Root, Get)
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

APTR METHOD(GALLIUMSOFTPIPEDRIVER, Hidd_GalliumBaseDriver, CreatePipeScreen)
{
    struct pipe_winsys *winsys;
    struct pipe_screen *screen;

    winsys = arosmesa_create_softpipe_winsys();
    if (winsys == NULL)
        return NULL;

    screen = softpipe_create_screen(winsys);
    if (screen == NULL)
        goto fail;

    return screen;

fail:
    if (winsys && winsys->destroy)
        winsys->destroy(winsys);

    return NULL;
}

VOID METHOD(GALLIUMSOFTPIPEDRIVER, Hidd_GalliumBaseDriver, QueryDepthStencil)
{
    *msg->depthbits = 16;
    *msg->stencilbits = 8;
}

VOID METHOD(GALLIUMSOFTPIPEDRIVER, Hidd_GalliumBaseDriver, DisplaySurface)
{
    struct pipe_surface * surf = (struct pipe_surface *)msg->surface;
    struct softpipe_texture *spt = softpipe_texture(surf->texture);
    struct arosmesa_buffer *amesa_buf = (struct arosmesa_buffer *)(spt->buffer);
    struct RastPort * rp = CloneRastPort(msg->rastport);

    WritePixelArray(
        amesa_buf->data, 
        0,
        0,
        spt->stride[surf->level],
        rp, 
        msg->left, 
        msg->top, 
        msg->width, 
        msg->height, 
        AROS_PIXFMT);

    FreeRastPort(rp);
}
