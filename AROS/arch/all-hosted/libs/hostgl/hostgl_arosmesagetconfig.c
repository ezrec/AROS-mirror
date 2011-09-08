/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#include "hostgl_types.h"
#include <proto/exec.h>

/*****************************************************************************

    NAME */

      AROS_LH3(void, AROSMesaGetConfig,

/*  SYNOPSIS */ 
      AROS_LHA(AROSMesaContext, amesa, A0),
      AROS_LHA(GLenum, pname, D0),
      AROS_LHA(GLint *, params, A1),

/*  LOCATION */
      struct Library *, MesaBase, 13, Mesa)

/*  FUNCTION

        Gets value of selected parameter
 
    INPUTS

        pname - enum value of parameter

        params - pointer to integer where the value is to be put

    RESULT

        None
 
    BUGS

    INTERNALS

    HISTORY

*****************************************************************************/
{
    AROS_LIBFUNC_INIT

    /* TODO: IMPLEMENT */

    AROS_LIBFUNC_EXIT
}


