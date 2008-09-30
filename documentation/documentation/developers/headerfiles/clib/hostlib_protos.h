#ifndef CLIB_HOSTLIB_PROTOS_H
#define CLIB_HOSTLIB_PROTOS_H

/*
    *** Automatically generated from 'hostlib.conf'. Edits will be lost. ***
    Copyright © 1995-2008, The AROS Development Team. All rights reserved.
*/

#include <aros/libcall.h>
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

#endif /* CLIB_HOSTLIB_PROTOS_H */
