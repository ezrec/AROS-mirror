/*
 * Copyright (C) 2014, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 * 
 * $Id$
 */

#ifndef LWIP_ARCH_SYS_ARCH_H
#define LWIP_ARCH_SYS_ARCH_H

#include <lwip/err.h>

#include <exec/ports.h>
#include <exec/tasks.h>
#include <exec/semaphores.h>

#include <errno.h>

struct sys_prot;
typedef struct sys_prot *sys_prot_t;

sys_prot_t sys_arch_protect(void);
void sys_arch_unprotect(sys_prot_t state);

/* Semaphores */
#define SYS_SEM_NULL    NULL
struct sys_arch_sem;
typedef struct sys_arch_sem *sys_sem_t;

err_t sys_sem_new(sys_sem_t *sem, u8_t count);
void sys_sem_free(sys_sem_t *sem);

void sys_sem_signal(sys_sem_t *sem);
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout_ms);
#define sys_sem_set_invalid(x)  do { } while (0)
#define sys_sem_valid(x)        (1)

/* Mailboxes */
#define SYS_MBOX_NULL   NULL
struct sys_arch_mbox;
typedef struct sys_arch_mbox *sys_mbox_t;
err_t sys_mbox_new(sys_mbox_t *mbox, int size);
void sys_mbox_free(sys_mbox_t *mbox);
void sys_mbox_post(sys_mbox_t *mbox, void *msg);
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout_ms);
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg);
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg);
#define sys_mbox_set_invalid(x) do { *(x) = NULL; } while (0)
#define sys_mbox_valid(x)   (*(x) != NULL)

/* Get the per-thread timeout list for this thread */
struct sys_timeouts *sys_arch_timeouts(void);

typedef struct Task *sys_thread_t;
typedef void (*lwip_thread_fn)(void *arg);
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio);

void sys_init(void);


#endif /* LWIP_ARCH_SYS_ARCH_H */
