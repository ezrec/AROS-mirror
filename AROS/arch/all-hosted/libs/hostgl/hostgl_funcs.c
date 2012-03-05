/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>
#include <proto/utility.h>
#include <proto/graphics.h>
#include <proto/oop.h>
#include "hostgl_ctx_manager.h"
#include "hostgl_funcs.h"
#include "arosmesa_funcs.h"

#define SETFBATTR(attribute, value)     \
    {                                   \
        fbattributes[i++] = attribute;  \
        fbattributes[i++] = value;      \
    }

BOOL HostGL_FillFBAttributes(LONG * fbattributes, LONG size, struct TagItem *tagList)
{
    LONG i = 0;
    BOOL noDepth, noStencil, noAccum;
    noStencil   = GetTagData(AMA_NoStencil, GL_FALSE, tagList);
    noAccum     = GetTagData(AMA_NoAccum, GL_FALSE, tagList);
    noDepth     = GetTagData(AMA_NoDepth, GL_FALSE, tagList);

#if defined(RENDERER_SEPARATE_X_WINDOW)
    SETFBATTR(GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT)
    SETFBATTR(GLX_DOUBLEBUFFER, True);
#endif

#if defined(RENDERER_PBUFFER_WPA)
    SETFBATTR(GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT)
    SETFBATTR(GLX_DOUBLEBUFFER, False);
#endif

#if defined(RENDERER_PIXMAP_BLIT)
    SETFBATTR(GLX_DRAWABLE_TYPE, GLX_PIXMAP_BIT)
    SETFBATTR(GLX_DOUBLEBUFFER, False);
#endif

    SETFBATTR(GLX_RENDER_TYPE, GLX_RGBA_BIT)
    SETFBATTR(GLX_RED_SIZE, 1)
    SETFBATTR(GLX_GREEN_SIZE, 1)
    SETFBATTR(GLX_BLUE_SIZE, 1)
    SETFBATTR(GLX_ALPHA_SIZE, 0)

    if (noDepth)
        SETFBATTR(GLX_DEPTH_SIZE, 0)
    else
        SETFBATTR(GLX_DEPTH_SIZE, 24)

    if (noStencil)
        SETFBATTR(GLX_STENCIL_SIZE, 0)
    else
        SETFBATTR(GLX_STENCIL_SIZE, 8)

    if (noAccum)
    {
        SETFBATTR(GLX_ACCUM_RED_SIZE, 0)
        SETFBATTR(GLX_ACCUM_GREEN_SIZE, 0)
        SETFBATTR(GLX_ACCUM_BLUE_SIZE, 0)
        SETFBATTR(GLX_ACCUM_ALPHA_SIZE, 0)
    }
    else
    {
        SETFBATTR(GLX_ACCUM_RED_SIZE, 16)
        SETFBATTR(GLX_ACCUM_GREEN_SIZE, 16)
        SETFBATTR(GLX_ACCUM_BLUE_SIZE, 16)
        SETFBATTR(GLX_ACCUM_ALPHA_SIZE, 16)
    }

    SETFBATTR(None, None)

    return TRUE;
}

#if defined(RENDERER_PBUFFER_WPA)
/* This function assumes all storages are NULL and/or have been freed */
VOID HostGL_AllocatePBuffer(AROSMesaContext amesa)
{
    Display * dsp = HostGL_GetGlobalX11Display();
    LONG pbufferattributes[] =
    {
        GLX_PBUFFER_WIDTH,   0,
        GLX_PBUFFER_HEIGHT,  0,
        GLX_LARGEST_PBUFFER, False,
        None
    };

    pbufferattributes[1] = amesa->framebuffer->width;
    pbufferattributes[3] = amesa->framebuffer->height;
    amesa->glXPbuffer = GLXCALL(glXCreatePbuffer, dsp, amesa->framebuffer->fbconfigs[0], pbufferattributes);

    amesa->swapbuffer       = AllocVec(amesa->framebuffer->width * amesa->framebuffer->height * SWAPBUFFER_BPP, MEMF_ANY);
    amesa->swapbufferline   = AllocVec(amesa->framebuffer->width * SWAPBUFFER_BPP, MEMF_ANY);
}

VOID HostGL_DeAllocatePBuffer(AROSMesaContext amesa)
{
    Display * dsp = HostGL_GetGlobalX11Display();

    if (amesa->glXPbuffer) GLXCALL(glXDestroyPbuffer, dsp, amesa->glXPbuffer);
    if (amesa->swapbufferline) FreeVec(amesa->swapbufferline);
    if (amesa->swapbuffer) FreeVec(amesa->swapbuffer);

    amesa->glXPbuffer = None;
    amesa->swapbufferline = NULL;
    amesa->swapbuffer = NULL;
}

static VOID HostGL_ResizePBuffer(AROSMesaContext amesa) /* Must be called with lock held */
{
    AROSMesaContext cur_ctx = HostGL_GetCurrentContext();
    BOOL isCurrent = (cur_ctx == amesa);
    Display * dsp = HostGL_GetGlobalX11Display();

    /* If current, detach */
    if (isCurrent)
        GLXCALL(glXMakeContextCurrent, dsp, None, None, NULL);

    /* Destroy and recreate (using new size) */
    HostGL_DeAllocatePBuffer(amesa);
    HostGL_AllocatePBuffer(amesa);

    /* If current, attach */
    if (isCurrent)
        GLXCALL(glXMakeContextCurrent, dsp, cur_ctx->glXPbuffer, cur_ctx->glXPbuffer, cur_ctx->glXctx);

    amesa->framebuffer->resized = FALSE;
}
#endif

#if defined(RENDERER_PIXMAP_BLIT)
/* This function assumes all storages are NULL and/or have been freed */
VOID HostGL_AllocatePixmap(AROSMesaContext amesa)
{
    Display     *dsp = HostGL_GetGlobalX11Display();
    OOP_Object  *hiddbm;
    Pixmap       pixmap;
    
    amesa->glXPixmapBM = AllocBitMap(amesa->framebuffer->width, amesa->framebuffer->height, 0, BMF_MINPLANES, amesa->visible_rp->BitMap);

#define HIDD_BM_OBJ(bitmap)     (*(OOP_Object **)&((bitmap)->Planes[0]))
#warning "HACK: Get X pixmap handle which is first field in X11 bitmap hidd inst data"
    
    if(amesa->glXPixmapBM)
    {
        hiddbm = HIDD_BM_OBJ(amesa->glXPixmapBM);    
        pixmap = *(Pixmap *)(OOP_INST_DATA(OOP_OCLASS(hiddbm), hiddbm));
        
        amesa->glXPixmap = GLXCALL(glXCreateGLXPixmap, dsp, amesa->visinfo, pixmap);
    }
}

VOID HostGL_DeAllocatePixmap(AROSMesaContext amesa)
{
    Display * dsp = HostGL_GetGlobalX11Display();

    if (amesa->glXPixmap) GLXCALL(glXDestroyGLXPixmap, dsp, amesa->glXPixmap);
    if (amesa->glXPixmapBM) FreeBitMap(amesa->glXPixmapBM);
    
    amesa->glXPixmap = None;
    amesa->glXPixmapBM = NULL;
}

static VOID HostGL_ResizePixmap(AROSMesaContext amesa) /* Must be called with lock held */
{
    AROSMesaContext cur_ctx = HostGL_GetCurrentContext();
    BOOL isCurrent = (cur_ctx == amesa);
    Display * dsp = HostGL_GetGlobalX11Display();

    /* If current, detach */
    if (isCurrent)
        GLXCALL(glXMakeContextCurrent, dsp, None, None, NULL);

    /* Destroy and recreate (using new size) */
    HostGL_DeAllocatePixmap(amesa);
    HostGL_AllocatePixmap(amesa);

    /* If current, attach */
    if (isCurrent)
        GLXCALL(glXMakeContextCurrent, dsp, cur_ctx->glXPixmap, cur_ctx->glXPixmap, cur_ctx->glXctx);

    amesa->framebuffer->resized = FALSE;
}
#endif

VOID HostGL_CheckAndUpdateBufferSize(AROSMesaContext amesa)
{
    AROSMesaRecalculateBufferWidthHeight(amesa);
#if defined(RENDERER_PBUFFER_WPA)
    if (amesa->framebuffer->resized)
    {
        HostGL_Lock();
        HostGL_UpdateGlobalGLXContext();
        HostGL_ResizePBuffer(amesa);
        HostGL_UnLock();
    }
#endif
#if defined(RENDERER_PIXMAP_BLIT)
    if (amesa->framebuffer->resized)
    {
        HostGL_Lock();
        HostGL_UpdateGlobalGLXContext();
        HostGL_ResizePixmap(amesa);
        HostGL_UnLock();
    }
#endif
}
