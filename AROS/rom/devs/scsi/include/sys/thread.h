/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SYS_THREAD_H
#define SYS_THREAD_H

#include <proto/exec.h>

struct thread {
    struct Task th_Task;
};

struct lwkt_token {
    struct SignalSemaphore tk_Semaphore;
};

typedef struct thread *thread_t;
typedef struct lwkt_token *lwkt_token_t;

#define hz              100     /* FIXME */
#define mycpuid         0
#define curthread       ((struct thread *)FindTask(NULL))

void lwkt_set_interrupt_support_thread(void);
int  lwkt_create (void (*func)(void *), void *, struct thread **, struct thread *, int, int, const char *, ...);

static inline void lwkt_token_init(lwkt_token_t tk, const char *name)
{
    tk->tk_Semaphore.ss_Link.ln_Name = (STRPTR)name;
    InitSemaphore(&tk->tk_Semaphore);
}

static inline void lwkt_gettoken(lwkt_token_t tk)
{
    ObtainSemaphore(&tk->tk_Semaphore);
}

static inline void lwkt_reltoken(lwkt_token_t tk)
{
    ReleaseSemaphore(&tk->tk_Semaphore);
}

#endif /* SYS_THREAD_H */
