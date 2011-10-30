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

    D(bug("TASK: 0x%x, MAKE CURRENT 0x%x\n", FindTask(NULL), amesa->glXctx));

    AROSMesaContext cur_ctx = HostGL_GetCurrentContext();

    if (amesa)
    {
        if (amesa != cur_ctx)
        {
            /* Recalculate buffer dimensions */
            AROSMesaRecalculateBufferWidthHeight(amesa);

            /* Attach */
            HostGL_SetCurrentContext(amesa);
            HOSTGL_PRE
            /* HOSTGL_PRE will set up correct context internally */
            HOSTGL_POST
        }            
    }
    else
    {
        /* Detach */
        HostGL_SetCurrentContext(NULL);
        Display * dsp = HostGL_GetGlobalX11Display();

        if (cur_ctx != NULL)
        {
            HOSTGL_PRE
            GLXCALL(glXMakeContextCurrent, dsp, None, None, NULL);
            HOSTGL_POST
        }
    }

    D(bug("TASK: 0x%x, MAKE CURRENT EXIT 0x%x\n", FindTask(NULL), amesa->glXctx));

    HostGL_UnLock();

    AROS_LIBFUNC_EXIT
}

