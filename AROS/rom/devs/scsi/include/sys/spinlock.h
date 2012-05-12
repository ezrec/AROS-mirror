/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SYS_SPINLOCK_H
#define SYS_SPINLOCK_H

#include <proto/exec.h>

struct spinlock {
    BOOL sp_Initialized;
    struct SignalSemaphore sp_Semaphore;
};

#define SPINLOCK_INITIALIZER(head)	{ .sp_Initialized = FALSE; }

#endif /* SYS_SPINLOCK_H */
