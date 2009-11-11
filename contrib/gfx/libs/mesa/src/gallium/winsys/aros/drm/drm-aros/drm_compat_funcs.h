/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef _DRM_COMPAT_FUNCS_
#define _DRM_COMPAT_FUNCS_

#include <proto/exec.h>

#include "drm_compat_types.h"

#define writel(val, addr)           (*(volatile ULONG*)(addr) = (val))
#define readl(addr)                 (*(volatile ULONG*)(addr))
#define writeb(val, addr)           (*(volatile BYTE*)(addr) = (val))
#define readb(addr)                 (*(volatile BYTE*)(addr))
#define kzalloc(size, flags)        AllocVec(size, MEMF_ANY | MEMF_CLEAR)
#define kmalloc(size, flags)        AllocVec(size, MEMF_ANY)
#define kfree(objp)                 FreeVec(objp)
#define roundup(x, y)               ((((x) + ((y) - 1)) / (y)) * (y))
#define lower_32_bits(n)            ((u32)(n))
#define upper_32_bits(n)            ((u32)(((n) >> 16) >> 16))

void iowrite32(u32 val, void * addr);
unsigned int ioread32(void * addr);
void iowrite8(u8 val, void * addr);
unsigned int ioread8(void * addr);

#endif /* _DRM_COMPAT_FUNCS_ */
