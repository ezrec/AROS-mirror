/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef MACHINE_ATOMIC_H
#define MACHINE_ATOMIC_H

#include <sys/types.h>

static inline u_int atomic_fetchadd_int(volatile u_int *ptr, u_int val)
{
    u_int old;
    Forbid();
    old = *ptr;
    *ptr += val;
    Permit();

    return old;
}

static inline u_int atomic_fetchsubtract_int(volatile u_int *ptr, u_int val)
{
    u_int old;
    Forbid();
    old = *ptr;
    *ptr -= val;
    Permit();

    return old;
}

static inline u_int atomic_set_int(volatile u_int *ptr, u_int mask)
{
    u_int old;
    Forbid();
    old = *ptr;
    *ptr |= mask;
    Permit();

    return old;
}

static inline u_int atomic_clear_int(volatile u_int *ptr, u_int mask)
{
    u_int old;
    Forbid();
    old = *ptr;
    *ptr &= ~mask;
    Permit();

    return old;
}

static inline int atomic_cmpset_int(volatile u_int *ptr, u_int old, u_int val)
{
    u_int res;
    Forbid();
    res = *ptr;
    if (res == old)
        *ptr = val;
    Permit();
    return (res == old);
}

#define atomic_add_int(p,v)     (void)atomic_fetchadd_int(p,v)
#define atomic_subtract_int(p,v)     (void)atomic_fetchsubtract_int(p,v)

#endif /* MACHINE_ATOMIC_H */
