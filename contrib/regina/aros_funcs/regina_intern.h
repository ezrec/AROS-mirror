#ifndef REGINA_INTERN_H
#define REGINA_INTERN_H

/*
    (C) 2001 AROS - The Amiga Research OS
    $Id$

    Desc: Internal definitions for regina.library
    Lang: English
*/

#ifndef EXEC_TYPES_H
#   include <exec/types.h>
#endif
#ifndef EXEC_LIBRARIES_H
#   include <exec/libraries.h>
#endif
#ifndef LIBCORE_BASE_H
#   include <libcore/base.h>
#endif
#ifndef AROS_LIBCALL_H
#   include <aros/libcall.h>
#endif

#include "rexx.h"

#define INCL_REXXSAA
#include "rexxsaa.h"

#include "libdefs.h"

LIBBASETYPE;

#define SysBase (((struct LibHeader *) ReginaBase)->lh_SysBase)

/****************************************************************************************/


LIBBASETYPE
{
    struct LibHeader		lh;
    tsd_t *                     tsd;
};

/****************************************************************************************/

#endif /* REGINA_INTERN_H */
