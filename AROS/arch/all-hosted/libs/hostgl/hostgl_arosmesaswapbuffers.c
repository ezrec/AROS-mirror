/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "hostgl_types.h"
#include <proto/exec.h>
#if defined(RENDERER_PBUFFER_WPA)
#include <proto/cybergraphics.h>
#include <proto/graphics.h>
#include <cybergraphx/cybergraphics.h>
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

    if (amesa) 
    {
#if defined(RENDERER_SEPARATE_X_WINDOW)
        GLXCALL(glXSwapBuffers, amesa->XDisplay, amesa->glXWindow);
#endif
#if defined(RENDERER_PBUFFER_WPA)
        LONG line = 0;
        LONG width = amesa->framebuffer->width;
        LONG height = amesa->framebuffer->height;
        GLCALL(glFlush);
        GLCALL(glReadPixels, 0, 0, width, height, GL_BGRA, GL_BYTE, amesa->swapbuffer);
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
    }

    /* TODO: check and update glx drawable size */
    /* AROSMesaCheckAndUpdateBufferSize(amesa); */

    AROS_LIBFUNC_EXIT
}

