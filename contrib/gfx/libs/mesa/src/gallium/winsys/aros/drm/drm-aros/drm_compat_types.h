/*
    Copyright 2009, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef _DRM_COMPAT_TYPES_
#define _DRM_COMPAT_TYPES_

#include <exec/types.h>
#include <exec/semaphores.h>
#include <sys/types.h>
#include <stdbool.h>
#include <errno.h>
#define __user
#define __iomem
#define __force
#define __must_check
#define __u32                       ULONG
#define __s32                       LONG
#define __u16                       UWORD
#define __u64                       UQUAD
#define u16                         UWORD
#define u32                         ULONG
#define u8                          BYTE
#define resource_size_t             IPTR
#define dma_addr_t                  IPTR
/* TODO: Implement spinlocks in busy-cpu way? */
#define spinlock_t                  struct SignalSemaphore
/* FIXME: atomic_t does not really deliver atomic operations */
#define atomic_t                    int
#define pgprot_t                    ULONG

#undef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({          \
    const typeof(((type *)0)->member)*__mptr = (ptr);    \
             (type *)((char *)__mptr - offsetof(type, member)); })

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

/* Page handling */
struct page
{
    APTR address;
    APTR allocated_buffer;
};

#undef PAGE_SIZE
#define PAGE_SHIFT              12
#define PAGE_SIZE               ((1UL) << PAGE_SHIFT)
#define PAGE_MASK               (~(PAGE_SIZE-1))
#define PAGE_ALIGN(addr)        (APTR)(((IPTR)(addr) + PAGE_SIZE - 1) & PAGE_MASK)

struct work_struct;

/* Reference counted objects implementation */
struct kref
{
    atomic_t refcount;
};

/* Mutex emulation */
struct mutex
{
    struct SignalSemaphore semaphore;
};

/* IDR handling */
struct idr
{
    ULONG size;
    ULONG occupied;
    IPTR * pointers;
};

#include "drm_linux_list.h"

#endif /* _DRM_COMPAT_TYPES_ */
