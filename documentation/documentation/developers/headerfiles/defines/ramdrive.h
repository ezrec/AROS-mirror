#ifndef DEFINES_RAMDRIVE_H
#define DEFINES_RAMDRIVE_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/workbench/devs/ramdrive.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for ramdrive
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __killrad0_WB(__RamdriveBase) \
        AROS_LC0(STRPTR, killrad0, \
        struct Device *, (__RamdriveBase), 7, Ramdrive)

#define killrad0() \
    __killrad0_WB(RamdriveBase)

#define __killrad_WB(__RamdriveBase, __arg1) \
        AROS_LC1(STRPTR, killrad, \
                  AROS_LCA(ULONG,(__arg1),D0), \
        struct Device *, (__RamdriveBase), 8, Ramdrive)

#define killrad(arg1) \
    __killrad_WB(RamdriveBase, (arg1))

__END_DECLS

#endif /* DEFINES_RAMDRIVE_H*/
