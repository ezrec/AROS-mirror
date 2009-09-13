/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "arosmesa_winsys.h"
#include "pipe/internal/p_winsys_screen.h"
#include "softpipe/sp_texture.h"
#include "softpipe/sp_winsys.h"
#include "util/u_math.h"

#include <proto/exec.h>
#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>

#if (AROS_BIG_ENDIAN == 1)
#define AROS_PIXFMT RECTFMT_ARGB32   /* Big Endian Archs. */
#else
#define AROS_PIXFMT RECTFMT_BGRA32   /* Little Endian Archs. */
#endif

/* FIXME: Don't use context. Use structure having access to rastport instead */
static void
arosmesa_softpipe_display_surface(AROSMesaContext amesa,
                              struct pipe_surface *surf)
{
   struct softpipe_texture *spt = softpipe_texture(surf->texture);
   struct arosmesa_buffer *amesa_buf = (struct arosmesa_buffer *)(spt->buffer);
     WritePixelArray(
     amesa_buf->data, 
        0,
        0,
        spt->stride[surf->level],
        amesa->visible_rp, 
        amesa->left, 
        amesa->top, 
        amesa->width, 
        amesa->height, 
        AROS_PIXFMT);
}

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

static void
arosmesa_flush_frontbuffer(struct pipe_winsys *pws,
                     struct pipe_surface *surf,
                     void *context_private)
{
    /* FIXME: Is this correction implementation */
    AROSMesaContext amesa = (AROSMesaContext) context_private;
    arosmesa_softpipe_display_surface(amesa, surf);
}

static struct pipe_winsys *
arosmesa_create_softpipe_winsys( void )
{
    /* FIXME: Will this work for shared version ? */
    static struct pipe_winsys *ws = NULL;

    if (!ws) 
    {
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

        ws->flush_frontbuffer = arosmesa_flush_frontbuffer;
        ws->get_name = NULL; /* FIXME */
    }

    return ws;
}


static struct pipe_screen *
arosmesa_create_softpipe_screen( void )
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
    /* FIXME: implement */
    /* if (winsys)
        winsys->destroy( winsys ); */

    return NULL;
}


static struct pipe_context *
arosmesa_create_softpipe_context( struct pipe_screen *screen )
{
    struct pipe_context *pipe;

    pipe = softpipe_create(screen);
    if (pipe == NULL)
        goto fail;

    /* FIXME: Is that needed ? - filled in in AROSMesaCreateContext */
    /*pipe->priv = context_private;*/
    return pipe;

fail:
    /* FIXME: Free stuff here */
    return NULL;
}

struct arosmesa_driver arosmesa_softpipe_driver = 
{
   .create_pipe_screen = arosmesa_create_softpipe_screen,
   .create_pipe_context = arosmesa_create_softpipe_context,
   .display_surface = arosmesa_softpipe_display_surface
};



