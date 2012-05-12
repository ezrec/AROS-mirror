/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SYS_KTHREAD_H
#define SYS_KTHREAD_H

#include <sys/taskqueue.h>

int kthread_create(void (*func)(void *), void *arg, struct thread **tdp, const char *fmt, ...);

#endif /* SYS_KTHREAD_H */
