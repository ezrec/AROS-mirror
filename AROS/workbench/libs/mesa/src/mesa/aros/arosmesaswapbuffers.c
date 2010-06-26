/*
    Copyright 2009-2010, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "arosmesa_funcs.h"
#include "state_tracker/st_public.h"
#include <proto/exec.h>
#include <proto/gallium.h>

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

    SAVE_REG
    
    PUT_MESABASE_IN_REG

    struct pipe_surface *surf;

    /* If we're swapping the buffer associated with the current context
    * we have to flush any pending rendering commands first.
    */
    st_notify_swapbuffers(amesa->framebuffer);

    /* ST_SURFACE_FRONT_LEFT is used because AROSMesa never renders directly to 
       display, thus FRONT LEFT buffer actually is a "back buffer" */
    st_get_framebuffer_surface(amesa->framebuffer, ST_SURFACE_FRONT_LEFT, &surf);

    if (surf) 
    {
        BltPipeSurfaceRastPort(surf, 0, 0, amesa->visible_rp, amesa->left, 
            amesa->top, amesa->width, amesa->height);
    }

    AROSMesaCheckAndUpdateBufferSize(amesa);

    RESTORE_REG

    AROS_LIBFUNC_EXIT
}


