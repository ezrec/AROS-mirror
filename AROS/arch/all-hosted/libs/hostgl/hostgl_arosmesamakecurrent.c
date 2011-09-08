/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "arosmesa_funcs.h"
#include <proto/exec.h>

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

    if (amesa)
    {
        /* TODO: check if not the same */
        /* struct st_context_iface * cur_ctx = glstapi->get_current(glstapi);*/
        
        /* if (amesa->st != cur_ctx) */
        {
            /* Recalculate buffer dimensions */
            AROSMesaRecalculateBufferWidthHeight(amesa);

            /* Attach */
            GLXCALL(glXMakeCurrent, amesa->XDisplay, amesa->XWindow, amesa->glXctx);
        }            
    }
    else
    {
        /* Detach */
        /* TODO: how to implement? XDisplay/GLXDrawable needed which are part of amesa */
    }

    AROS_LIBFUNC_EXIT
}

