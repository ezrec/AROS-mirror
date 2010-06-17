#ifndef DEFINES_HOSTLIB_H
#define DEFINES_HOSTLIB_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/arch/all-unix/hostlib/hostlib.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for hostlib
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __HostLib_Open_WB(__HostLibBase, __arg1, __arg2) \
        AROS_LC2(void *, HostLib_Open, \
                  AROS_LCA(const char *,(__arg1),A0), \
                  AROS_LCA(char **,(__arg2),A1), \
        APTR , (__HostLibBase), 1, HostLib)

#define HostLib_Open(arg1, arg2) \
    __HostLib_Open_WB(HostLibBase, (arg1), (arg2))

#define __HostLib_Close_WB(__HostLibBase, __arg1, __arg2) \
        AROS_LC2(int, HostLib_Close, \
                  AROS_LCA(void *,(__arg1),A0), \
                  AROS_LCA(char **,(__arg2),A1), \
        APTR , (__HostLibBase), 2, HostLib)

#define HostLib_Close(arg1, arg2) \
    __HostLib_Close_WB(HostLibBase, (arg1), (arg2))

#define __HostLib_GetPointer_WB(__HostLibBase, __arg1, __arg2, __arg3) \
        AROS_LC3(void *, HostLib_GetPointer, \
                  AROS_LCA(void *,(__arg1),A0), \
                  AROS_LCA(const char *,(__arg2),A1), \
                  AROS_LCA(char **,(__arg3),A2), \
        APTR , (__HostLibBase), 3, HostLib)

#define HostLib_GetPointer(arg1, arg2, arg3) \
    __HostLib_GetPointer_WB(HostLibBase, (arg1), (arg2), (arg3))

#define __HostLib_FreeErrorStr_WB(__HostLibBase, __arg1) \
        AROS_LC1NR(void, HostLib_FreeErrorStr, \
                  AROS_LCA(char *,(__arg1),A0), \
        APTR , (__HostLibBase), 4, HostLib)

#define HostLib_FreeErrorStr(arg1) \
    __HostLib_FreeErrorStr_WB(HostLibBase, (arg1))

__END_DECLS

#endif /* DEFINES_HOSTLIB_H*/
