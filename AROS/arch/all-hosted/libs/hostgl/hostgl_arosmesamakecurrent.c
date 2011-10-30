/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>
#include "arosmesa_funcs.h"
#include "hostgl_ctx_manager.h"
#include <aros/debug.h>

/*****************************************************************************

    NAME */

      AROS_LH1(void, AROSMesaMakeCurrent,

/*  SYNOPSIS */ 
      AROS_LHA(AROSMesaContext, amesa, A0),

/*  LOCATION */
      struct Library *, MesaBase, 8, Mesa)

/*  FUNCTION
        Make the selected GL rendering context active.
 
    INPUTS
        amesa - GL rendering context to be made active for all following GL
                calls.
 
    RESULT
 
    BUGS

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    HostGL_Lock();

    D(bug("[HostGL] TASK: 0x%x, MAKE CURRENT 0x%x\n", FindTask(NULL), amesa->glXctx));

    if (amesa)
    {
        AROSMesaContext cur_ctx = HostGL_GetCurrentContext();

        if (amesa != cur_ctx)
        {
            /* Recalculate buffer dimensions */
            AROSMesaRecalculateBufferWidthHeight(amesa);

            /* Attach */
            HostGL_SetCurrentContext(amesa);
            HostGL_UpdateGlobalGLXContext();
        }            
    }
    else
    {
        /* Detach */
        HostGL_SetCurrentContext(NULL);
        HostGL_UpdateGlobalGLXContext();
    }

    HostGL_UnLock();

    AROS_LIBFUNC_EXIT
}

