  /*
  * UAE - The Un*x Amiga Emulator
  *
  * Thread support using AmigaOS processes
  *
  * Copyright 2003-2004 Richard Drummond
  */

/*
 * Simple emulation of POSIX sempahores use exec semaphores.
 * amd message passing.
 */
#include "sysconfig.h"
#include "sysdeps.h"

#include <exec/exec.h>
#include <exec/ports.h>
#include <exec/lists.h>
#include <dos/dosextens.h>
#include <dos/dostags.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>
#include <clib/alib_protos.h>

#include "threaddep/thread.h"

/*
 * CreateNewProc() on MorphOS needs to be told that code is PPC
 */
#ifdef __MORPHOS__
# define myCreateNewProcTags(...) CreateNewProcTags(NP_CodeType, CODETYPE_PPC, __VA_ARGS__)
#else
# define myCreateNewProcTags CreateNewProcTags
#endif

/*
 * Emulation of POSIX semaphore using AmigaOS semaphores
 * and message passing
 *
 * Needs work on cleaning up . . .
 */

static struct Process *sem_thread;
static struct MsgPort *sem_thread_port;

static int sem_count;

struct PSemaphoreMsg {
    struct Message     msg;
    struct PSemaphore *sem;
    ULONG              reply_sig;
    struct Task       *reply_task;
    int                command;
};

#define PSEMCMD_LOCK	0
#define PSEMCMD_UNLOCK	1
#define PSEMCMD_QUIT	2

static int putPSemaphoreMsg (int psemcmd, struct PSemaphore *psem)
{
    int result = -1;
    struct PSemaphoreMsg msg;
    ULONG sigs;

    memset (&msg, 0, sizeof msg);

    msg.msg.mn_Length = sizeof msg;
    msg.sem           = psem;
    msg.command       = psemcmd;
    msg.reply_sig     = AllocSignal (-1);
    msg.reply_task    = FindTask (NULL);

    if (msg.reply_sig != (ULONG)-1) {
	PutMsg (sem_thread_port, (struct Message *)&msg);

	sigs = Wait (1 << msg.reply_sig | SIGBREAKF_CTRL_C);
	FreeSignal (msg.reply_sig);

	if ((sigs & SIGBREAKF_CTRL_C) == 0)
	    result = 0;
    }
    return result;
}

static void replyPSemaphoreMsg( struct PSemaphoreMsg *msg)
{
   ReplyMsg ((struct Message *)msg);

   if (msg->reply_sig != (ULONG)-1)
       Signal (msg->reply_task, 1 << msg->reply_sig);
}

static void sem_thread_startup_msg(void)
{
    struct Process *self = (struct Process*) FindTask (NULL);
    WaitPort (&self->pr_MsgPort);
    ReplyMsg (GetMsg (&self->pr_MsgPort));
}

static void sem_thread_func (void)
{
    ULONG sigmask;
    int dont_exit = 1;
    struct PSemaphoreMsg *msg = NULL;

    sem_thread_port = CreateMsgPort ();
    sigmask = SIGBREAKF_CTRL_C | (1 << sem_thread_port->mp_SigBit);

    /* Wait for and reply to startup msg */
    sem_thread_startup_msg();

    /* handle semaphore commands */
    while (dont_exit) {
	ULONG sigs = Wait (sigmask);

	if ((sigs & SIGBREAKF_CTRL_C) != 0) {
	    dont_exit = 0;
	    continue;
	}

	while (dont_exit && (msg = (struct PSemaphoreMsg *) GetMsg (sem_thread_port))) {
	    struct PSemaphore *psem = msg->sem;


	    switch (msg->command) {
		case PSEMCMD_LOCK:
		    /* Don't reply to this yet. Just put the request in the waiting
		     * list and reply when we get an unlock. The requesting process
		     * will block until we do. */
		    AddTail ((struct List*)&psem->wait_queue, (struct Node *)msg);
		    break;

		case PSEMCMD_UNLOCK:
		    /* Grab a lock request from the waiting list and reply to
		     * it. This will wake up the blocked requesting process. */
		    {
			struct PSemaphoreMsg *wait_msg = (struct PSemaphoreMsg*)
			    RemHead((struct List*)&psem->wait_queue);

			replyPSemaphoreMsg (msg);

			if (wait_msg)
			    replyPSemaphoreMsg (wait_msg);
		    }
		    break;

		case PSEMCMD_QUIT:
		    /* And its good night from me. . . */
		    dont_exit = 0;
	    }
	}
   }

   /*
    * Clean up and exit
    */
   Forbid ();
   {
	struct PSemaphoreMsg *rem_msg;

	/* reply to any outstanding requests */
	while ((rem_msg = (struct PSemaphoreMsg *)GetMsg (sem_thread_port)) != NULL)
	    replyPSemaphoreMsg (rem_msg);

	/* reply to the quit msg */
        if (msg != NULL)
	    replyPSemaphoreMsg (msg);

	DeleteMsgPort (sem_thread_port);
	sem_thread_port = 0;
   }
   Permit();
}

static void stop_sem_thread (void)
{
    if (sem_thread_port)
	putPSemaphoreMsg (PSEMCMD_QUIT, NULL);
}

static int start_sem_thread (void)
{
    int result = -1;
    struct MsgPort *replyport = CreateMsgPort();
    struct Process *p;

    if (replyport) {
	p = myCreateNewProcTags (NP_Name, (ULONG)"UAE psem thread",
				 NP_StackSize, 2048,
				 NP_Entry, (ULONG)sem_thread_func,
				 TAG_DONE);
        if(p) {
	    struct Message msg;
	    msg.mn_ReplyPort = replyport;
	    msg.mn_Length = sizeof msg;
	    PutMsg (&p->pr_MsgPort, (struct Message*)&msg);
	    WaitPort (replyport);

	    atexit (stop_sem_thread);
	    result = 0;
	}
	DeleteMsgPort (replyport);
    }
    return result;
}



int uae_sem_init (uae_sem_t *sem, int pshared, unsigned int value)
{
    int result = 0;

    InitSemaphore (&sem->mutex);
    NewList ((struct List *)&sem->wait_queue);
    sem->value = value;
    sem->live  = 1;

    if (sem_count == 0)
	result = start_sem_thread();

    if (result != -1)
	sem_count++;

    return result;
}

int uae_sem_destroy (uae_sem_t *sem)
{
    ObtainSemaphore (&sem->mutex);

    sem->live = 0;
    while (++sem->value < 1)
       putPSemaphoreMsg (PSEMCMD_UNLOCK, sem);

    ReleaseSemaphore (&sem->mutex);

    if (--sem_count == 0)
	putPSemaphoreMsg (PSEMCMD_QUIT, NULL);

    return 0;
}

int uae_sem_wait (uae_sem_t *sem)
{
    int result = -1;

    ObtainSemaphore (&sem->mutex);

    if (sem->live) {
	if (sem->value > 0) {
	    --sem->value;
	    result = 0;
        } else {
	    ReleaseSemaphore (&sem->mutex);
	    result = putPSemaphoreMsg (PSEMCMD_LOCK, sem);
	    ObtainSemaphore (&sem->mutex);

	    if (result != -1)
                --sem->value;
	}
    }
    ReleaseSemaphore (&sem->mutex);
    return result;
}

int uae_sem_trywait (uae_sem_t *sem)
{
    int result = -1;

    ObtainSemaphore (&sem->mutex);

    if (sem->live) {
	if (sem->value > 0) {
	--sem->value;
	    result = 0;
        }
    }
    ReleaseSemaphore (&sem->mutex);
    return result;
}

int uae_sem_get_value (uae_sem_t *sem)
{
    int value;

    ObtainSemaphore (&sem->mutex);
    value = sem->value;
    ReleaseSemaphore (&sem->mutex);

    return value;
}

int uae_sem_post (uae_sem_t *sem)
{
    int result = -1;

    ObtainSemaphore (&sem->mutex);

    if (sem->live) {
        sem->value++;

	if (sem->value <= 1)
	    result = putPSemaphoreMsg (PSEMCMD_UNLOCK, sem);
    }
    ReleaseSemaphore (&sem->mutex);

    return result;
}




/*
 * Support routines for threading
 */

struct startupmsg
{
    struct Message msg;
    void           *(*func) (void *);
    void           *arg;
};

static void do_thread (void)
{
   struct Process *pr = (struct Process *) FindTask (NULL);
   struct startupmsg *msg;
   void *(*func) (void *);
   void *arg;

   WaitPort (&pr->pr_MsgPort);
   msg = (struct startupmsg *) GetMsg(&pr->pr_MsgPort);
   func = msg->func;
   arg  = msg->arg;
   ReplyMsg ((struct Message*)msg);

   func (arg);
}

int uae_start_thread (void *(*f) (void *), void *arg, uae_thread_id *foo)
{
    struct MsgPort *replyport = CreateMsgPort();

    if (replyport) {
	*foo = (struct Task *)myCreateNewProcTags (NP_Output, Output(),
						   NP_Input, Input(),
						   NP_Name, (ULONG)"UAE thread",
						   NP_CloseOutput, FALSE,
						   NP_CloseInput, FALSE,
						   NP_StackSize, 16384,
						   NP_Entry, (ULONG)do_thread,
						   TAG_DONE);

	if(*foo) {
	    struct startupmsg msg;

	    msg.msg.mn_ReplyPort = replyport;
	    msg.msg.mn_Length = sizeof msg;
	    msg.func = f;
	    msg.arg = arg;
	    PutMsg (&((struct Process*)*foo)->pr_MsgPort, (struct Message*)&msg);
	    WaitPort (replyport);
	}
	DeleteMsgPort (replyport);
    }

    return *foo!=0;
}
