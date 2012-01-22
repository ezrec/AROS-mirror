/*
    Copyright 2011-2012, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <proto/exec.h>

/*****************************************************************************

    NAME */

      AROS_LH0(APTR, AROSMesaGetOpenGLStateTrackerApi,

/*  SYNOPSIS */ 

/*  LOCATION */
      struct Library *, MesaBase, 0, Mesa)

/*  FUNCTION
        This is a PRIVATE function used by egl.library to receive pointer to
        api structure of OpenGL. Do not use this function in your application.
 
    INPUTS
 
    RESULT
 
    BUGS

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    /* This function cannot be implemented in HostGL as it is Gallium specific */
    return NULL;

    AROS_LIBFUNC_EXIT
}
