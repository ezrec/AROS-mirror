/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "hostgl_ctx_manager.h"
#include "hostgl_funcs.h"
#include <proto/exec.h>
#include <aros/debug.h>

/*****************************************************************************

    NAME */

      AROS_LH1(void, AROSMesaDestroyContext,

/*  SYNOPSIS */ 
      AROS_LHA(AROSMesaContext, amesa, A0),

/*  LOCATION */
      struct Library *, MesaBase, 7, Mesa)

/*  FUNCTION
        Destroys the GL rendering context and frees all resoureces.
 
    INPUTS
        amesa - pointer to GL rendering context. A NULL pointer will be
                ignored.
 
    RESULT
        The GL context is destroyed. Do no use it anymore.
 
    BUGS

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    if (amesa)
    {
        Display * dsp = NULL;
        AROSMesaContext cur_ctx = NULL;

        HostGL_Lock();

        dsp = HostGL_GetGlobalX11Display();
        cur_ctx = HostGL_GetCurrentContext();

        /* If the passed context is current context, detach it */
        if (amesa == cur_ctx)
        {
            HostGL_SetCurrentContext(NULL);
            HostGL_UpdateGlobalGLXContext();
        }

        GLXCALL(glXDestroyContext, dsp, amesa->glXctx);
#if defined(RENDERER_SEPARATE_X_WINDOW)
        GLXCALL(glXDestroyWindow, dsp, amesa->glXWindow);
        XCALL(XDestroyWindow, dsp, amesa->XWindow);
#endif
#if defined(RENDERER_PBUFFER_WPA)
        HostGL_DeAllocatePBuffer(amesa);
#endif
#if defined(RENDERER_PIXMAP_BLIT)
        HostGL_DeAllocatePixmap(amesa);
#endif
        XCALL(XFree, amesa->framebuffer->fbconfigs);
        FreeVec(amesa->framebuffer);
        AROSMesaDestroyContext(amesa);

        HostGL_UnLock();
    }

    AROS_LIBFUNC_EXIT
}

