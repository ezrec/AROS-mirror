/*
 * Copyright (C) 2014, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 * 
 * $Id$
 */

#include <proto/exec.h>

#include <lwip/opt.h>
#include <lwip/sys.h>

struct sys_prot { /* unused */ };

void sys_init(void)
{
}

sys_prot_t sys_arch_protect(void)
{
    Forbid();
    return NULL;
}

void sys_arch_unprotect(sys_prot_t state)
{
    Permit();
}

err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    *mbox = CreateMsgPort();

    return (*mbox == NULL) ? ERR_MEM : ERR_OK;
}

void sys_mbox_free(sys_mbox_t *mbox)
{
    DeleteMsgPort(*mbox);
}

struct sys_mbox_msg {
    struct Message hdr;
    void *msg;
};


err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    struct sys_mbox_msg *m;

    if ((m = AllocMem(sizeof(*m), MEMF_ANY))) {
        m->hdr.mn_ReplyPort = *mbox;
        m->hdr.mn_Length = sizeof(*m);
        m->msg = msg;
        PutMsg(*mbox, &m->hdr);
        return ERR_OK;
    }

   return ERR_MEM;
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    while (sys_mbox_trypost(mbox, msg) != ERR_OK);
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout_ms)
{
    struct sys_mbox_msg *m;

    /* FIXME: Timeouts! */
    WaitPort(*mbox);
    m = (struct sys_mbox_msg *)GetMsg(*mbox);
    *msg = m->msg;
    FreeMem(m, sizeof(*m));

    return 0;   // ms elapsed while waiting
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    struct sys_mbox_msg *m;

    m = (struct sys_mbox_msg *)GetMsg(*mbox);
    if (m) {
        *msg = m->msg;
        FreeMem(m, sizeof(*m));
        return 0;
    } else {
        return SYS_MBOX_EMPTY;
    }
}

err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
    InitSemaphore(sem);
    while (count--)
        ObtainSemaphore(sem);

    return ERR_OK;
}

void sys_sem_free(sys_sem_t *sem)
{
}

void sys_sem_signal(sys_sem_t *sem)
{
    ReleaseSemaphore(sem);
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout_ms)
{
    /* FIXME: Use AttemptSemaphore() with timeouts */
    ObtainSemaphore(sem);

    return 0;
}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
    if (stacksize == 0)
        stacksize = AROS_STACKSIZE;

    return NewCreateTask(
            TASKTAG_NAME, name,
            TASKTAG_PC, thread,
            TASKTAG_STACKSIZE, stacksize,
            TASKTAG_ARG1, arg,
            TASKTAG_PRI, prio,
            TAG_END);
}
