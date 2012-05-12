/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SYS_MALLOC_H
#define SYS_MALLOC_H

#include <proto/exec.h>

struct malloc_type;

#define MALLOC_DEFINE(type,name,desc) struct malloc_type *type = NULL

#define M_DEVBUF        ((struct malloc_type *)NULL)
#define M_TEMP          ((struct malloc_type *)NULL)

#define M_ZERO          MEMF_CLEAR
#define M_INTWAIT       0
#define M_WAITOK        0
#define M_NOWAIT        0

#define MINALLOCSIZE    sizeof(void *)

static inline void *kmalloc(unsigned long size, const struct malloc_type *type, int flags)
{
    return AllocVec(size, MEMF_PUBLIC | flags);
}

static inline void kfree(void *addr, const struct malloc_type *type)
{
    FreeVec(addr);
}

#endif /* SYS_MALLOC_H */
