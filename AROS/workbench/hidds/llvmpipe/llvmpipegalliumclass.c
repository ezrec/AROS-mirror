/*
    Copyright 2013, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <hidd/gallium.h>
#include <proto/oop.h>
#include <aros/debug.h>

#include <gallium/pipe/p_screen.h>
#include "llvmpipe/lp_texture.h"
#include "llvmpipe/lp_public.h"
#include "llvmpipe/lp_screen.h"
#include "util/u_format.h"
#include "util/u_math.h"
#include "state_tracker/sw_winsys.h"

#include "llvmpipe_intern.h"

#include <proto/cybergraphics.h>
#include <proto/graphics.h>
#include <cybergraphx/cybergraphics.h>

#if (AROS_BIG_ENDIAN == 1)
#define AROS_PIXFMT RECTFMT_ARGB32   /* Big Endian Archs. */
#else
#define AROS_PIXFMT RECTFMT_BGRA32   /* Little Endian Archs. */
#endif

#define CyberGfxBase    (&BASE(cl->UserData)->sd)->LLVMPipeCyberGfxBase

#undef HiddGalliumAttrBase
#define HiddGalliumAttrBase   (SD(cl)->hiddGalliumAB)

struct HiddLLVMPipeWinSys
{
    struct HIDDT_WinSys base;
    struct sw_winsys    swws;
};

/*  Displaytarget support code */
struct HiddLLVMPipeDisplaytarget
{
    APTR data;
};

struct HiddLLVMPipeDisplaytarget * HiddLLVMPipeDisplaytarget(struct sw_displaytarget * dt)
{
    return (struct HiddLLVMPipeDisplaytarget *)dt;
}

static struct sw_displaytarget *
HiddLLVMPipeCreateDisplaytarget(struct sw_winsys *winsys, unsigned tex_usage,
    enum pipe_format format, unsigned width, unsigned height,
    unsigned alignment, unsigned *stride)
{
    struct HiddLLVMPipeDisplaytarget * spdt =
        AllocVec(sizeof(struct HiddLLVMPipeDisplaytarget), MEMF_PUBLIC | MEMF_CLEAR);
    
    *stride = align(util_format_get_stride(format, width), alignment);
    spdt->data = AllocVec(*stride * height, MEMF_PUBLIC | MEMF_CLEAR);
    
    return (struct sw_displaytarget *)spdt;
}

static void
HiddLLVMPipeDestroyDisplaytarget(struct sw_winsys *ws, struct sw_displaytarget *dt)
{
    struct HiddLLVMPipeDisplaytarget * spdt = HiddLLVMPipeDisplaytarget(dt);
    
    if (spdt)
    {
        FreeVec(spdt->data);
        FreeVec(spdt);
    }
}

static void *
HiddLLVMPipeMapDisplaytarget(struct sw_winsys *ws, struct sw_displaytarget *dt,
    unsigned flags)
{
    struct HiddLLVMPipeDisplaytarget * spdt = HiddLLVMPipeDisplaytarget(dt);
    return spdt->data;
}

static void
HiddLLVMPipeUnMapDisplaytarget(struct sw_winsys *ws, struct sw_displaytarget *dt)
{
    /* No op */
}

/*  Displaytarget support code ends */

static struct HiddLLVMPipeWinSys *
HiddLLVMPipeCreateLLVMPipeWinSys(void)
{
    struct HiddLLVMPipeWinSys * ws = NULL;

    ws = (struct HiddLLVMPipeWinSys *)AllocVec
        (sizeof(struct HiddLLVMPipeWinSys), MEMF_PUBLIC|MEMF_CLEAR);

    /* Fill in this struct with callbacks that pipe will need to
    * communicate with the window system, buffer manager, etc. 
    */

    /* Since Mesa 7.9 softpipe mo longer uses pipe_winsys - it uses sw_winsys */
    ws->base.base.buffer_create         = NULL;
    ws->base.base.user_buffer_create    = NULL;
    ws->base.base.buffer_map            = NULL;
    ws->base.base.buffer_unmap          = NULL;
    ws->base.base.buffer_destroy        = NULL;
    ws->base.base.surface_buffer_create = NULL;
    ws->base.base.fence_reference       = NULL;
    ws->base.base.fence_signalled       = NULL;
    ws->base.base.fence_finish          = NULL;
    ws->base.base.flush_frontbuffer     = NULL;
    ws->base.base.get_name              = NULL;
    ws->base.base.destroy               = NULL;
    
    /* Fill in with functions is displaytarget is ever used*/
    ws->swws.destroy                            = NULL;
    ws->swws.is_displaytarget_format_supported  = NULL;
    ws->swws.displaytarget_create               = HiddLLVMPipeCreateDisplaytarget;
    ws->swws.displaytarget_from_handle          = NULL;
    ws->swws.displaytarget_get_handle           = NULL;
    ws->swws.displaytarget_map                  = HiddLLVMPipeMapDisplaytarget;
    ws->swws.displaytarget_unmap                = HiddLLVMPipeUnMapDisplaytarget;
    ws->swws.displaytarget_display              = NULL;
    ws->swws.displaytarget_destroy              = HiddLLVMPipeDestroyDisplaytarget;
    
    return ws;
}

OOP_Object *METHOD(LLVMPipeGallium, Root, New)
{
    o = (OOP_Object *)OOP_DoSuperMethod(cl, o, (OOP_Msg) msg);

    return o;
}

VOID METHOD(LLVMPipeGallium, Root, Get)
{
    ULONG idx;

    if (IS_GALLIUM_ATTR(msg->attrID, idx))
    {
        switch (idx)
        {
            /* Overload the property */
            case aoHidd_Gallium_GalliumInterfaceVersion:
                *msg->storage = GALLIUM_INTERFACE_VERSION;
                return;
        }
    }
    
    /* Use parent class for all other properties */
    OOP_DoSuperMethod(cl, o, (OOP_Msg)msg);
}

APTR METHOD(LLVMPipeGallium, Hidd_Gallium, CreatePipeScreen)
{
    struct HiddLLVMPipeWinSys * llvmpipews;
    struct pipe_screen *screen;

    llvmpipews = HiddLLVMPipeCreateLLVMPipeWinSys();
    if (llvmpipews == NULL)
        return NULL;

    screen = llvmpipe_create_screen(&llvmpipews->swws);
    if (screen == NULL)
        goto fail;

    /* Force a pipe_winsys pointer (Mesa 7.9 or never) */
    screen->winsys = (struct pipe_winsys *)llvmpipews;

    /* Preserve pointer to HIDD driver */
    llvmpipews->base.driver = o;

    return screen;

fail:
    if (llvmpipews && llvmpipews->base.base.destroy)
        llvmpipews->base.base.destroy((struct pipe_winsys *)llvmpipews);

    return NULL;
}

VOID METHOD(LLVMPipeGallium, Hidd_Gallium, DisplayResource)
{
    struct llvmpipe_resource * spr = llvmpipe_resource(msg->resource);
    struct sw_winsys * swws = llvmpipe_screen(spr->base.screen)->winsys;
    struct RastPort * rp = CreateRastPort();
    APTR * data = spr->data;

    if ((data == NULL) && (spr->dt != NULL))
        data = swws->displaytarget_map(swws, spr->dt, 0);

    if (data)
    {
        rp->BitMap = msg->bitmap;

        WritePixelArray(
            data, 
            msg->srcx,
            msg->srcy,
            spr->row_stride[0],
            rp, 
            msg->dstx, 
            msg->dsty, 
            msg->width, 
            msg->height, 
            AROS_PIXFMT);
    }

    if ((spr->data == NULL) && (data != NULL))
        swws->displaytarget_unmap(swws, spr->dt);

    FreeRastPort(rp);
}

