#ifndef MPEGA_INTERN_H
#define MPEGA_INTERN_H

/*
    Copyright © 1995-2012, The AROS Development Team. All rights reserved.
    $Id$

    Desc: Internal mpega.library definitions.
    Lang: English.
*/


#ifndef PROTO_EXEC_H
#include  <proto/exec.h>
#endif
#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

struct PrivateBase {
        struct Library pv_Lib;
        UWORD pv_Pad;
        struct ExecBase *pv_SysBase;
        struct DosLibrary *pv_DOSBase;
        BPTR pv_SegList;
};      


#endif /* MPEGA_INTERN_H */
