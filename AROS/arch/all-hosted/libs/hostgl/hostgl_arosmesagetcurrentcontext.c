/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "hostgl_ctx_manager.h"
#include <proto/exec.h>

/*****************************************************************************

    NAME */

      AROS_LH0(AROSMesaContext, AROSMesaGetCurrentContext,

/*  SYNOPSIS */ 

/*  LOCATION */
      struct Library *, MesaBase, 10, Mesa)

/*  FUNCTION
        Returns the currently selected GL rendering context.
 
    INPUTS
 
    RESULT
        The GL rendering context which is currently active.
 
    BUGS

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    return HostGL_GetCurrentContext();

    AROS_LIBFUNC_EXIT
}

