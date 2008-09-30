#ifndef CLIB_REXXSUPPORT_PROTOS_H
#define CLIB_REXXSUPPORT_PROTOS_H

/*
    *** Automatically generated from 'rexxsupport.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
#include <rexx/storage.h>
AROS_LP2(ULONG, ArexxDispatch,
         AROS_LPA(struct RexxMsg *, msg, A0),
         AROS_LPA(STRPTR *, _retargstringptr, A1),
         LIBBASETYPEPTR, RexxSupportBase, 5, RexxSupport
);
AROS_LP1(void, ClosePortRsrc,
         AROS_LPA(struct RexxRsrc *, rsrc, A0),
         LIBBASETYPEPTR, RexxSupportBase, 6, RexxSupport
);

#endif /* CLIB_REXXSUPPORT_PROTOS_H */
