#ifndef DEFINES_NONVOLATILE_H
#define DEFINES_NONVOLATILE_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/nonvolatile/nonvolatile.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for nonvolatile
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __GetCopyNV_WB(__NonvolatileBase, __arg1, __arg2, __arg3) \
        AROS_LC3(APTR, GetCopyNV, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
                  AROS_LCA(STRPTR,(__arg2),A1), \
                  AROS_LCA(BOOL,(__arg3),D1), \
        struct Library *, (__NonvolatileBase), 5, Nonvolatile)

#define GetCopyNV(arg1, arg2, arg3) \
    __GetCopyNV_WB(NonvolatileBase, (arg1), (arg2), (arg3))

#define __FreeNVData_WB(__NonvolatileBase, __arg1) \
        AROS_LC1NR(void, FreeNVData, \
                  AROS_LCA(APTR,(__arg1),A0), \
        struct Library *, (__NonvolatileBase), 6, Nonvolatile)

#define FreeNVData(arg1) \
    __FreeNVData_WB(NonvolatileBase, (arg1))

#define __StoreNV_WB(__NonvolatileBase, __arg1, __arg2, __arg3, __arg4, __arg5) \
        AROS_LC5(LONG, StoreNV, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
                  AROS_LCA(STRPTR,(__arg2),A1), \
                  AROS_LCA(APTR,(__arg3),A2), \
                  AROS_LCA(ULONG,(__arg4),D0), \
                  AROS_LCA(BOOL,(__arg5),D1), \
        struct Library *, (__NonvolatileBase), 7, Nonvolatile)

#define StoreNV(arg1, arg2, arg3, arg4, arg5) \
    __StoreNV_WB(NonvolatileBase, (arg1), (arg2), (arg3), (arg4), (arg5))

#define __DeleteNV_WB(__NonvolatileBase, __arg1, __arg2, __arg3) \
        AROS_LC3(BOOL, DeleteNV, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
                  AROS_LCA(STRPTR,(__arg2),A1), \
                  AROS_LCA(BOOL,(__arg3),D1), \
        struct Library *, (__NonvolatileBase), 8, Nonvolatile)

#define DeleteNV(arg1, arg2, arg3) \
    __DeleteNV_WB(NonvolatileBase, (arg1), (arg2), (arg3))

#define __GetNVInfo_WB(__NonvolatileBase, __arg1) \
        AROS_LC1(struct NVInfo *, GetNVInfo, \
                  AROS_LCA(BOOL,(__arg1),D1), \
        struct Library *, (__NonvolatileBase), 9, Nonvolatile)

#define GetNVInfo(arg1) \
    __GetNVInfo_WB(NonvolatileBase, (arg1))

#define __GetNVList_WB(__NonvolatileBase, __arg1, __arg2) \
        AROS_LC2(struct MinList *, GetNVList, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
                  AROS_LCA(BOOL,(__arg2),D1), \
        struct Library *, (__NonvolatileBase), 10, Nonvolatile)

#define GetNVList(arg1, arg2) \
    __GetNVList_WB(NonvolatileBase, (arg1), (arg2))

#define __SetNVProtection_WB(__NonvolatileBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(BOOL, SetNVProtection, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
                  AROS_LCA(STRPTR,(__arg2),A1), \
                  AROS_LCA(LONG,(__arg3),D2), \
                  AROS_LCA(BOOL,(__arg4),D1), \
        struct Library *, (__NonvolatileBase), 11, Nonvolatile)

#define SetNVProtection(arg1, arg2, arg3, arg4) \
    __SetNVProtection_WB(NonvolatileBase, (arg1), (arg2), (arg3), (arg4))

__END_DECLS

#endif /* DEFINES_NONVOLATILE_H*/
