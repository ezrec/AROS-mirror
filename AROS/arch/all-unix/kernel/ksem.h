/*
 * Copyright (C) 2013, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 * 
 * $Id$
 */

#ifndef KSEM_H
#define KSEM_H

#include <pthread.h>

struct ksem {
    int owner;
    int depth;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

int _ksem_init(struct KernelBase *KernelBase, struct ksem *ksem);
int _ksem_obtain(struct KernelBase *KernelBase, struct ksem *ksem);
int _ksem_release(struct KernelBase *KernelBase, struct ksem *ksem);

#define ksem_init(ksem)     _ksem_init(KernelBase, ksem)
#define ksem_obtain(ksem)   _ksem_obtain(KernelBase, ksem)
#define ksem_release(ksem)  _ksem_release(KernelBase, ksem)

#endif /* KSEM_H */
