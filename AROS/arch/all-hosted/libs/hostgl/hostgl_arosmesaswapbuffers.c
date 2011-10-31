/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "hostgl_ctx_manager.h"
#include "hostgl_funcs.h"
#include <proto/exec.h>
#include <aros/debug.h>
#if defined(RENDERER_PBUFFER_WPA)
#include <proto/cybergraphics.h>
#include <proto/graphics.h>
#include <cybergraphx/cybergraphics.h>
static struct SignalSemaphore * GetX11SemaphoreFromBitmap(struct BitMap * bm);
#endif

/*****************************************************************************

    NAME */

      AROS_LH1(void, AROSMesaSwapBuffers,

/*  SYNOPSIS */ 
      AROS_LHA(AROSMesaContext, amesa, A0),

/*  LOCATION */
      struct Library *, MesaBase, 9, Mesa)

/*  FUNCTION
        Swaps the back with front buffers. MUST BE used to display the effect
        of rendering onto the target RastPort, since AROSMesa always work in
        double buffer mode.
 
    INPUTS
        amesa - GL rendering context on which swap is to be performed.
 
    RESULT
 
    BUGS

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    D(bug("[HostGL] TASK: 0x%x, SWAP 0x%x\n", FindTask(NULL), amesa->glXctx));

    if (amesa) 
    {
#if defined(RENDERER_SEPARATE_X_WINDOW)
        Display * dsp = NULL;
        HostGL_Lock();
        HostGL_UpdateGlobalGLXContext();
        dsp = HostGL_GetGlobalX11Display();
        GLXCALL(glXSwapBuffers, dsp, amesa->glXWindow);
        HostGL_UnLock();
#endif
#if defined(RENDERER_PBUFFER_WPA)
        LONG line = 0;
        LONG width = amesa->framebuffer->width;
        LONG height = amesa->framebuffer->height;
        
        HostGL_Lock();
        HostGL_UpdateGlobalGLXContext();
        ObtainSemaphore(GetX11SemaphoreFromBitmap(amesa->visible_rp->BitMap));
        GLCALL(glReadPixels, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, amesa->swapbuffer);
        ReleaseSemaphore(GetX11SemaphoreFromBitmap(amesa->visible_rp->BitMap));
        HostGL_UnLock();

        /* Flip image */
        for (line = 0; line < height / 2; line++)
        {
            ULONG * start = amesa->swapbuffer + (line * width);
            ULONG * end = amesa->swapbuffer + ((height - line - 1) * width);
            CopyMem(start, amesa->swapbufferline, width * 4);
            CopyMem(end, start, width * 4);
            CopyMem(amesa->swapbufferline, end, width * 4);
        }

        WritePixelArray(amesa->swapbuffer, 0, 0, width * 4, amesa->visible_rp, amesa->left, amesa->top, 
            width, height, RECTFMT_BGRA32);
#endif
        HostGL_CheckAndUpdateBufferSize(amesa);
    }

    AROS_LIBFUNC_EXIT
}

#if defined(RENDERER_PBUFFER_WPA)
/* HACK: EVIL HACK TO GET ACCESS TO X11 HIDD INTERNALS */
#include <proto/oop.h>
#define HIDD_BM_OBJ(bitmap)     (*(OOP_Object **)&((bitmap)->Planes[0]))

struct bitmap_data
{
    union
    {
        Window 	xwindow;
        Pixmap  pixmap;
    }               drawable;
    Window          masterxwindow;    
    Cursor          cursor;
    unsigned long   sysplanemask;
    Colormap        colmap;
    GC              gc; /* !!! This is an X11 GC, NOT a HIDD gc */
    Display         *display;
    int             screen;
    int             flags;    
};

struct fakefb_data
{
    OOP_Object *framebuffer;
    OOP_Object *fakegfxhidd;
};

struct x11_staticdata
{
    /* These two members MUST be in the beginning of this structure
       because they are exposed to disk-based part (see x11_class.h) */
    UBYTE	   	     keycode2rawkey[256];
    BOOL	   	     havetable;

    struct SignalSemaphore   sema; /* Protecting this whole struct */
    struct SignalSemaphore   x11sema;
};

struct x11clbase
{
    struct Library        library;
    
    struct x11_staticdata xsd;
};

#define XSD(cl)     	(&((struct x11clbase *)cl->UserData)->xsd)

static struct SignalSemaphore * GetX11SemaphoreFromBitmap(struct BitMap * bm)
{
    OOP_Object * hiddbm = HIDD_BM_OBJ(bm);
    struct fakefb_data * fakefb = (struct fakefb_data *)OOP_INST_DATA(OOP_OCLASS(hiddbm), hiddbm);
    return (&XSD(OOP_OCLASS(fakefb->framebuffer))->x11sema);
}
#endif
