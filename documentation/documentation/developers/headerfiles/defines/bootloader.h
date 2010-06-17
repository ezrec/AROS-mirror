#ifndef DEFINES_BOOTLOADER_H
#define DEFINES_BOOTLOADER_H

/*
    *** Automatically generated from '/home/mazze/projects/aros-src/arch/all-unix/bootloader/bootloader.conf'. Edits will be lost. ***
    Copyright © 1995-2010, The AROS Development Team. All rights reserved.
*/

/*
    Desc: Defines for bootloader
*/

#include <aros/libcall.h>
#include <exec/types.h>
#include <aros/preprocessor/variadic/cast2iptr.hpp>

__BEGIN_DECLS


#define __GetBootInfo_WB(__BootLoaderBase, __arg1) \
        AROS_LC1(APTR, GetBootInfo, \
                  AROS_LCA(ULONG,(__arg1),D0), \
        APTR , (__BootLoaderBase), 1, Bootloader)

#define GetBootInfo(arg1) \
    __GetBootInfo_WB(BootLoaderBase, (arg1))

__END_DECLS

#endif /* DEFINES_BOOTLOADER_H*/
