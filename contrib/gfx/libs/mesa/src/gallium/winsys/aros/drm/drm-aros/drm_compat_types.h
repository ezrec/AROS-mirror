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
#define u8                          UBYTE
#define resource_size_t             IPTR
#define dma_addr_t                  IPTR
#define pgprot_t                    ULONG

#undef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ({          \
    const typeof(((type *)0)->member)*__mptr = (ptr);    \
             (type *)((char *)__mptr - offsetof(type, member)); })

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

typedef struct
{
    LONG count;
} atomic_t;

typedef struct
{
    atomic_t lock;
} spinlock_t;

typedef struct
{
    atomic_t lock;
} rwlock_t;

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
#define ALIGN(val, align)       (val + align - 1) & (~(align - 1))
#define BITS_TO_LONGS(x)        ((x / (sizeof(long) * 8)) + 1)

struct work_struct;
struct agp_bridge_data;

#define AGP_USER_MEMORY         1
#define AGP_USER_CACHED_MEMORY  2
#define ERESTARTSYS             782434897 /* Just some random value */

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
    ULONG last_starting_id;
    IPTR * pointers;
};

#include "drm_linux_list.h"

#endif /* _DRM_COMPAT_TYPES_ */
