#ifndef CLIB_HOSTLIB_PROTOS_H
#define CLIB_HOSTLIB_PROTOS_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/arch/all-unix/hostlib/hostlib.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>


__BEGIN_DECLS

AROS_LP2(void *, HostLib_Open,
         AROS_LPA(const char *, filename, A0),
         AROS_LPA(char **, error, A1),
         LIBBASETYPEPTR, HostLibBase, 1, HostLib
);
AROS_LP2(int, HostLib_Close,
         AROS_LPA(void *, handle, A0),
         AROS_LPA(char **, error, A1),
         LIBBASETYPEPTR, HostLibBase, 2, HostLib
);
AROS_LP3(void *, HostLib_GetPointer,
         AROS_LPA(void *, handle, A0),
         AROS_LPA(const char *, symbol, A1),
         AROS_LPA(char **, error, A2),
         LIBBASETYPEPTR, HostLibBase, 3, HostLib
);
AROS_LP1(void, HostLib_FreeErrorStr,
         AROS_LPA(char *, error, A0),
         LIBBASETYPEPTR, HostLibBase, 4, HostLib
);

__END_DECLS

#endif /* CLIB_HOSTLIB_PROTOS_H */
