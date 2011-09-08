/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "hostgl_types.h"
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
        GLXCALL(glXDestroyContext, amesa->XDisplay, amesa->glXctx);
#if defined(RENDERER_SEPARATE_X_WINDOW)
        GLXCALL(glXDestroyWindow, amesa->XDisplay, amesa->glXWindow);
        XCALL(XDestroyWindow, amesa->XDisplay, amesa->XWindow);
#endif
#if defined(RENDERER_PBUFFER_WPA)
        GLXCALL(glXDestroyPbuffer, amesa->XDisplay, amesa->glXPbuffer);
        FreeVec(amesa->swapbufferline);
        FreeVec(amesa->swapbuffer);
#endif
        XCALL(XCloseDisplay, amesa->XDisplay);
        
        FreeVec(amesa->framebuffer);
        AROSMesaDestroyContext(amesa);
    }

    AROS_LIBFUNC_EXIT
}

