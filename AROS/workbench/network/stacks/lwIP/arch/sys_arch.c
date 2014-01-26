/*
 * Copyright (C) 2014, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 * 
 * $Id$
 */

#include <devices/timer.h>

#include <proto/timer.h>
#include <proto/exec.h>

#include <lwip/opt.h>
#include <lwip/sys.h>


struct sys_prot { /* unused */ };

struct sys_arch_thread {
    struct Node wait;
    struct MsgPort mp;
    struct timerequest tr;
};

static void _sys_arch_thread_init(struct sys_arch_thread *tinfo)
{
    struct Task *me = FindTask(NULL);

    tinfo->wait.ln_Name = (APTR)me;
    tinfo->wait.ln_Pri = AllocSignal(-1);

    tinfo->mp.mp_Flags = PA_SIGNAL;
    tinfo->mp.mp_Node.ln_Type = NT_MSGPORT;
    NEWLIST(&tinfo->mp.mp_MsgList);
    tinfo->mp.mp_SigBit = AllocSignal(-1);
    tinfo->mp.mp_SigTask = me;

    tinfo->tr.tr_node.io_Message.mn_ReplyPort = &tinfo->mp;
    tinfo->tr.tr_node.io_Message.mn_Length = sizeof(tinfo->tr);

    OpenDevice("timer.device", UNIT_MICROHZ, &tinfo->tr.tr_node, 0);
}

static void _sys_arch_thread_delete(struct sys_arch_thread *tinfo)
{
    FreeSignal(tinfo->wait.ln_Pri);
    CloseDevice(&tinfo->tr.tr_node);
    FreeSignal(tinfo->mp.mp_SigBit);
}

static ULONG _sys_arch_thread_sigwait(struct sys_arch_thread *tinfo, ULONG sigmask, ULONG timeout_ms)
{
    struct timeval start, end;
    struct Device *TimerBase = tinfo->tr.tr_node.io_Device;
    ULONG wait_ms;

    GetSysTime(&start);
    if (timeout_ms > 0) {
        ULONG timemask, mask;

        timemask = 1 << tinfo->tr.tr_node.io_Message.mn_ReplyPort->mp_SigBit;

        tinfo->tr.tr_node.io_Command = TR_ADDREQUEST;
        tinfo->tr.tr_time.tv_secs = timeout_ms / 1000;
        tinfo->tr.tr_time.tv_micro = (timeout_ms % 1000) * 1000;
        SendIO(&tinfo->tr.tr_node);

        D(bug("%s[%p]: Wait(sigmask=0x%08x | timemask=0x%08x)\n",
                __func__, FindTask(NULL), sigmask, timemask));
        mask = Wait(sigmask | timemask);
        D(bug("%s[%p]: Wait(sigmask=0x%08x | timemask=0x%08x)\n",
                __func__, FindTask(NULL), sigmask, timemask, mask));

        if (!(mask & sigmask)) {
            WaitIO(&tinfo->tr.tr_node);
            D(bug("%s[%p]: SYS_ARCH_TIMEOUT\n",
                    __func__, FindTask(NULL)));
            return SYS_ARCH_TIMEOUT;
        }
        AbortIO(&tinfo->tr.tr_node);
        WaitIO(&tinfo->tr.tr_node);
    } else {
        D(bug("%s[%p]: Wait(wait=0x%08x)\n",
                __func__, FindTask(NULL), sigmask));
        Wait(sigmask);
        D(bug("%s[%p]: Wait(wait=0x%08x) = 0x%08x\n",
                __func__, FindTask(NULL), sigmask, sigmask));
    }

    GetSysTime(&end);
    SubTime(&end, &start);
    wait_ms = (end.tv_micro / 1000) + (end.tv_secs * 1000);
    D(bug("%s[%p]: wait_ms=%d\n",
            __func__, FindTask(NULL), wait_ms));

    return wait_ms;
}

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

struct sys_arch_mbox {
    struct SignalSemaphore lock;
    sys_sem_t sem;
    struct List avail;
    struct List ready;
    struct Node mq[];
};

err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    struct sys_arch_mbox *mx;

    D(bug("%s[%p]: &mbox=%p, size=%d\n",
                __func__, FindTask(NULL), mbox, size));

    if (size == 0)
        size = 64;

    mx = AllocVec(sizeof(*mx) + sizeof(struct Node)*size, MEMF_ANY | MEMF_CLEAR);
    if (mx != NULL) {
        int i;

        InitSemaphore(&mx->lock);
        sys_sem_new(&mx->sem, 0);
        NEWLIST(&mx->avail);
        NEWLIST(&mx->ready);
        for (i = 0; i < size; i++)
            ADDTAIL(&mx->avail, &mx->mq[i]);

        (*mbox) = mx;
    } else {
        *mbox = NULL;
    }

    D(bug("%s[%p]: mbox=%p, size=%d\n",
                __func__, FindTask(NULL), *mbox, size));

    return (*mbox == NULL) ? ERR_MEM : ERR_OK;
}

void sys_mbox_free(sys_mbox_t *mbox)
{
    struct sys_arch_mbox *mx = (*mbox);

    sys_sem_free(&mx->sem);
    FreeVec(*mbox);
}

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    struct sys_arch_mbox *mx = (*mbox);
    struct Node *mnode;

    D(bug("%s[%p]: mx=%p, msg=%p\n",
                __func__, FindTask(NULL), mx, msg));

    ObtainSemaphore(&mx->lock);
    mnode = REMHEAD(&mx->avail);
    if (mnode) {
        mnode->ln_Name = msg;
        ADDTAIL(&mx->ready, mnode);
        sys_sem_signal(&mx->sem);
    }
    ReleaseSemaphore(&mx->lock);

    D(bug("%s[%p]: mx=%p, msg=%p, mnode=%p\n",
                __func__, FindTask(NULL), mx, msg, mnode));
    return (mnode == NULL) ? ERR_WOULDBLOCK : ERR_OK;
}

void sys_mbox_post(sys_mbox_t *mbox, void *msg)
{
    while (sys_mbox_trypost(mbox, msg) != ERR_OK);
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout_ms)
{
    struct sys_arch_mbox *mx = (*mbox);
    u32_t ms, waited_ms = 0;
    struct Node *mnode;

    D(bug("%s[%p]: mx=%p, &msg=%p, timeout_ms=%d\n",
                __func__, FindTask(NULL), mx, msg, timeout_ms));

    ObtainSemaphore(&mx->lock);
    mnode = REMHEAD(&mx->ready);
    while (!mnode) {
        ReleaseSemaphore(&mx->lock);
        if (waited_ms >= timeout_ms)
            return SYS_ARCH_TIMEOUT;
        ms = sys_arch_sem_wait(&mx->sem, timeout_ms - waited_ms);
        if (ms == SYS_ARCH_TIMEOUT) {
            return SYS_ARCH_TIMEOUT;
        }
        waited_ms += ms;
        ObtainSemaphore(&mx->lock);
        mnode = REMHEAD(&mx->ready);
    }
    *msg = (void *)mnode->ln_Name;
    ADDTAIL(&mx->avail, mnode);
    ReleaseSemaphore(&mx->lock);

    D(bug("%s[%p]: mx=%p, &msg=%p, mnode=%p, waited_ms=%d\n",
                __func__, FindTask(NULL), mx, msg, mnode, waited_ms));
    return waited_ms;
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
    struct sys_arch_mbox *mx = (*mbox);
    struct Node *mnode;

    D(bug("%s[%p]: mx=%p, &msg=%p\n",
                __func__, FindTask(NULL), mx, msg));

    ObtainSemaphore(&mx->lock);
    mnode = REMHEAD(&mx->ready);
    if (mnode) {
        *msg = (void *)mnode->ln_Name;
        ADDTAIL(&mx->avail, mnode);
    }
    ReleaseSemaphore(&mx->lock);

    D(bug("%s[%p]: mx=%p, &msg=%p, mnode=%p\n",
                __func__, FindTask(NULL), mx, msg, mnode));
    return (mnode == NULL) ? SYS_MBOX_EMPTY : 0;
}

struct sys_arch_sem {
    struct SignalSemaphore lock;
    struct List waiting;
    u8_t count;
};

err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
    struct sys_arch_sem *asem;

    asem = AllocVec(sizeof(*asem), MEMF_CLEAR | MEMF_ANY);
    if (asem == NULL) {
        *sem = NULL;
        return ERR_MEM;
    }

    InitSemaphore(&asem->lock);
    NEWLIST(&asem->waiting);
    asem->count = count;

    *sem = asem;

    return ERR_OK;
}

void sys_sem_free(sys_sem_t *sem)
{
    FreeVec(*sem);
}

void sys_sem_signal(sys_sem_t *sem)
{
    struct sys_arch_sem *asem = *sem;
    struct Node *wait;

    D(bug("%s[%p]: asem=%p\n",
            __func__, FindTask(NULL), asem));
    ObtainSemaphore(&asem->lock);
    asem->count++;
    ForeachNode(&asem->waiting, wait) {
        struct Task *task = (struct Task *)wait->ln_Name;
        ULONG sigmask = 1 << wait->ln_Pri;
        D(bug("%s[%p]: Signal(%p,waitmask=0x%08x)\n",
                __func__, FindTask(NULL), asem, task, sigmask));
        Signal(task, sigmask);
    }
    ReleaseSemaphore(&asem->lock);
}

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout_ms)
{
    struct sys_arch_sem *asem = (*sem);
    struct sys_arch_thread *tinfo = FindTask(NULL)->tc_UserData, stack_tinfo;
    ULONG total_ms;
    BOOL free_tinfo = FALSE;

    if (tinfo == NULL) {
        _sys_arch_thread_init(&stack_tinfo);
        tinfo = &stack_tinfo;
        free_tinfo = TRUE;
    }

    D(bug("%s[%p]: asem=%p, timeout_ms=%d, tinfo=%p(%s)\n",
                __func__, FindTask(NULL), asem, timeout_ms, tinfo, free_tinfo ? "stack" : "task"));

    ObtainSemaphore(&asem->lock);
    while (asem->count == 0) {
        ULONG wait_ms;

        if (timeout_ms && (total_ms >= timeout_ms)) {
            D(bug("%s[%p]: timeout: %d >= %d, count=%d\n",
                    __func__, FindTask(NULL), total_ms, timeout_ms, asem->count));
            ReleaseSemaphore(&asem->lock);
            return SYS_ARCH_TIMEOUT;
        }

        ADDTAIL(&asem->waiting, &tinfo->wait);
        ReleaseSemaphore(&asem->lock);

        wait_ms = _sys_arch_thread_sigwait(tinfo, 1 << tinfo->wait.ln_Pri, timeout_ms);
        if (wait_ms == SYS_ARCH_TIMEOUT) {
            ObtainSemaphore(&asem->lock);
            REMOVE(&tinfo->wait);
            ReleaseSemaphore(&asem->lock);
            D(bug("%s[%p]: timeout: %d\n",
                    __func__, FindTask(NULL), timeout_ms));
            return SYS_ARCH_TIMEOUT;
        }

        total_ms += wait_ms;

        ObtainSemaphore(&asem->lock);
        REMOVE(&tinfo->wait);
    }
    D(bug("%s[%p]: count=%d\n",
            __func__, FindTask(NULL), asem->count));
    asem->count--;
    ReleaseSemaphore(&asem->lock);

    if (free_tinfo)
        _sys_arch_thread_delete(tinfo);

    D(bug("%s[%p]: total_ms=%d\n",
            __func__, FindTask(NULL), total_ms));

    return total_ms;
}

static void _sys_thread_init(lwip_thread_fn thread, void *arg)
{
    struct sys_arch_thread tinfo;

    _sys_arch_thread_init(&tinfo);

    FindTask(NULL)->tc_UserData = &tinfo;

    D(bug("%s[%p]: thread(arg)=%p(%p) INIT\n", __func__, FindTask(NULL), thread, arg));
    thread(arg);
    D(bug("%s[%p]: thread(arg)=%p(%p) EXIT\n", __func__, FindTask(NULL), thread, arg));

    _sys_arch_thread_delete(&tinfo);
}

sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio)
{
    if (stacksize == 0)
        stacksize = AROS_STACKSIZE;

    return NewCreateTask(
            TASKTAG_NAME, name,
            TASKTAG_PC, _sys_thread_init,
            TASKTAG_STACKSIZE, stacksize,
            TASKTAG_ARG1, thread,
            TASKTAG_ARG2, arg,
            TASKTAG_PRI, prio,
            TASKTAG_TCBEXTRASIZE, sizeof(struct sys_arch_thread),
            TAG_END);
}
