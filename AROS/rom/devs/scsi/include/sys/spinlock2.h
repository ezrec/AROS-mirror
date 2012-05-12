/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SYS_SPINLOCK2_H
#define SYS_SPINLOCK2_H

#include <sys/spinlock.h>

static inline void spin_init(struct spinlock *spin)
{
    InitSemaphore(&spin->sp_Semaphore);
    spin->sp_Initialized = TRUE;
}

static inline void spin_uninit(struct spinlock *spin)
{
    spin->sp_Initialized = FALSE;
}

static inline void spin_lock(struct spinlock *spin)
{
    if (!spin->sp_Initialized)
        spin_init(spin);
    ObtainSemaphore(&spin->sp_Semaphore);
}

static inline void spin_unlock(struct spinlock *spin)
{
    ReleaseSemaphore(&spin->sp_Semaphore);
}



#endif /* SYS_SPINLOCK2_H */
