#ifndef DEFINES_NVDISK_H
#define DEFINES_NVDISK_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/libs/nonvolatile/nvdisk/nvdisk.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for nvdisk
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __ReadData_WB(__nvdBase, __arg1, __arg2) \
        AROS_LC2(APTR, ReadData, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
                  AROS_LCA(STRPTR,(__arg2),A1), \
        struct Library *, (__nvdBase), 5, NVDisk)

#define ReadData(arg1, arg2) \
    __ReadData_WB(nvdBase, (arg1), (arg2))

#define __WriteData_WB(__nvdBase, __arg1, __arg2, __arg3, __arg4) \
        AROS_LC4(LONG, WriteData, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
                  AROS_LCA(STRPTR,(__arg2),A1), \
                  AROS_LCA(APTR,(__arg3),A2), \
                  AROS_LCA(LONG,(__arg4),A3), \
        struct Library *, (__nvdBase), 6, NVDisk)

#define WriteData(arg1, arg2, arg3, arg4) \
    __WriteData_WB(nvdBase, (arg1), (arg2), (arg3), (arg4))

#define __DeleteData_WB(__nvdBase, __arg1, __arg2) \
        AROS_LC2(BOOL, DeleteData, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
                  AROS_LCA(STRPTR,(__arg2),A1), \
        struct Library *, (__nvdBase), 7, NVDisk)

#define DeleteData(arg1, arg2) \
    __DeleteData_WB(nvdBase, (arg1), (arg2))

#define __MemInfo_WB(__nvdBase, __arg1) \
        AROS_LC1(BOOL, MemInfo, \
                  AROS_LCA(struct NVInfo *,(__arg1),A0), \
        struct Library *, (__nvdBase), 8, NVDisk)

#define MemInfo(arg1) \
    __MemInfo_WB(nvdBase, (arg1))

#define __SetProtection_WB(__nvdBase, __arg1, __arg2, __arg3) \
        AROS_LC3(BOOL, SetProtection, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
                  AROS_LCA(STRPTR,(__arg2),A1), \
                  AROS_LCA(LONG,(__arg3),D0), \
        struct Library *, (__nvdBase), 9, NVDisk)

#define SetProtection(arg1, arg2, arg3) \
    __SetProtection_WB(nvdBase, (arg1), (arg2), (arg3))

#define __GetItemList_WB(__nvdBase, __arg1) \
        AROS_LC1(struct MinList *, GetItemList, \
                  AROS_LCA(STRPTR,(__arg1),A0), \
        struct Library *, (__nvdBase), 10, NVDisk)

#define GetItemList(arg1) \
    __GetItemList_WB(nvdBase, (arg1))

__END_DECLS

#endif /* DEFINES_NVDISK_H*/
