/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *         Sebastian Bauer <sebauer@t-online.de>
 *
 * $Id: sys_arch.c,v 1.7 2002/07/09 22:26:02 sebauer Exp $
 */

#include <time.h>

#include <dos.h>

#include <exec/memory.h>
#include <dos/dostags.h>
#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>

#include "lwip/sys.h"
#include "lwip/def.h"

struct PtrMessage
{
  struct Message msg;
  void *ptr;
};

struct sys_mbox_msg {
  struct sys_mbox_msg *next;
  void *msg;
};

#define SYS_MBOX_SIZE 100

struct sys_mbox {
  u16_t first, last;
  void *msgs[SYS_MBOX_SIZE];
  struct sys_sem *mail;
  struct sys_sem *mutex;
};

/* The structure used for sys_sem->queue */
struct TaskNode
{
  struct MinNode node;
  struct Task *task;
  LONG sem_signal;
};


struct sys_sem {
  unsigned int c;
  struct SignalSemaphore sem;

  /* List of blocked tasks */
  struct MinList queue; /* members are struct TaskNpde */
};

struct ThreadData
{
    /* timeoutstuff */
    struct sys_timeouts timeouts;

    /* Timerstuff */
    struct MsgPort *TimerPort;
    struct timerequest *TimerReq;
    ULONG TimerOutstanding;

    /* For the semaphores */
    LONG sem_signal;
    struct TaskNode task_node; /* the elementes of the queuelist in a semaphore */
};

struct StartupMsg		    /* startup message sent to child */
{
    struct Message msg;
    int (*fp)(void *);              /* function we're going to call */
    void *global_data;              /* global data reg (A4)         */
    long return_code;               /* return code from process     */
    void *UserData;                 /* User-supplied data pointer   */
    struct Process *child;	    /* The child process itselv     */
};


/*-----------------------------------------------------------------------------------*/


static struct ThreadData mainThread;

/*-----------------------------------------------------------------------------------*/

static int Timer_Init(struct ThreadData *data)
{
    if (!(data->TimerPort = CreateMsgPort())) return 0;
    if (!(data->TimerReq = (struct timerequest *)CreateIORequest(data->TimerPort, sizeof(struct timerequest))))
    {
	DeleteMsgPort(data->TimerPort);
	return 0;
    }

    if (OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *)data->TimerReq, 0))
    {
	DeleteIORequest(data->TimerReq);
	DeleteMsgPort(data->TimerPort);
	return 0;
    }
    data->TimerOutstanding = 0;
    return 1;
}

/*-----------------------------------------------------------------------------------*/

static void Timer_Cleanup(struct ThreadData *data)
{
    /* free timer stuff */
    if (data->TimerReq)
    {
	if (data->TimerReq->tr_node.io_Device)
	{
	    while (data->TimerOutstanding)
	    {
		if (Wait(1L << data->TimerPort->mp_SigBit | 4096) & 4096)
		    break;
		data->TimerOutstanding--;
	    }
	    CloseDevice((struct IORequest *)data->TimerReq);
	}
	DeleteIORequest(data->TimerReq);
    }
    if (data->TimerPort)
	DeleteMsgPort(data->TimerPort);
}

/*-----------------------------------------------------------------------------------*/

static struct timerequest *Timer_Send(struct ThreadData *data, ULONG millis)
{
    struct timerequest *time = (struct timerequest*)AllocVec(sizeof(struct timerequest),MEMF_PUBLIC|MEMF_CLEAR);
    if (time)
    {
	*time = *data->TimerReq;
	time->tr_node.io_Command = TR_ADDREQUEST;
	time->tr_time.tv_secs = millis/1000;
	time->tr_time.tv_micro = (millis%1000)*1000;
	SendIO((struct IORequest*)time);

	data->TimerOutstanding++;
    }
    return time;
}

/*-----------------------------------------------------------------------------------*/

static void Timer_Abort(struct ThreadData *data, struct timerequest *time)
{
    if (!CheckIO((struct IORequest*)time)) AbortIO((struct IORequest*)time);
    WaitIO((struct IORequest*)time);
    FreeVec(time);
    data->TimerOutstanding--;
}

/*-----------------------------------------------------------------------------------*/

static int Thread_Init(struct ThreadData *data)
{
    data->timeouts.next = NULL;
    if (!Timer_Init(data)) return 0;
    data->sem_signal = AllocSignal(-1);  
    if (data->sem_signal == -1)
    {
	Timer_Cleanup(data);
	return 0;
    }
    data->task_node.task = FindTask(NULL);
    data->task_node.sem_signal = data->sem_signal;
    return 1;
}

/*-----------------------------------------------------------------------------------*/

static void Thread_Cleanup(struct ThreadData *data)
{
    if (data->sem_signal != -1) FreeSignal(data->sem_signal);
    Timer_Cleanup(data);
}

/*-----------------------------------------------------------------------------------*/

struct ThreadData *Thread_Alloc(void)
{
  struct ThreadData *data = AllocVec(sizeof(struct ThreadData),MEMF_CLEAR);
  if (!data) return NULL;
  if (!(Thread_Init(data)))
  {
  	FreeVec(data);
  	return NULL;
  }
  return data;
}

/*-----------------------------------------------------------------------------------*/

void Thread_Free(struct ThreadData *data)
{
	if (!data) return;
	Thread_Cleanup(data);
  FreeVec(data);
}

/*-----------------------------------------------------------------------------------*/
static int Thread_Entry(void)
{
    struct Process *proc;
    struct StartupMsg *mess;
    struct ExecBase *SysBase = *((struct ExecBase **)4);
    __regargs int (*fp)(void *, void *, void *);
    void *ud;
    struct ThreadData data;
         
    proc = (struct Process *)FindTask((char *)NULL);

    /* get the startup message */
    WaitPort(&proc->pr_MsgPort);
    mess = (struct StartupMsg *)GetMsg(&proc->pr_MsgPort);

    /* gather necessary info from message */
    fp = (__regargs int (*)(void *, void *, void *))mess->fp;
    ud = mess->UserData;

    /* replace this with the proper #asm for Aztec */
    putreg(REG_A4, (long)mess->global_data);

    ReplyMsg(mess);


    if (Thread_Init(&data))
    {
    	FindTask(NULL)->tc_UserData = &data;
//    	kprintf("Thread at 0x%lx initialized. Now jumping to user defined function\n",FindTask(NULL));
	fp(ud,ud,ud);
//    	kprintf("Thread at 0x%lx finished. Cleanup now\n",FindTask(NULL));
	Thread_Cleanup(&data);
    }
    return 0;
}


/*-----------------------------------------------------------------------------------*/
sys_mbox_t sys_mbox_new(void)
{
    struct sys_mbox *mbox;

    mbox = AllocVec(sizeof(struct sys_mbox),MEMF_PUBLIC|MEMF_CLEAR);
    if (!mbox) return SYS_MBOX_NULL;

    mbox->first = mbox->last = 0;
    mbox->mail = sys_sem_new(0);
    mbox->mutex = sys_sem_new(1);
  
#ifdef SYS_STATS
    stats.sys.mbox.used++;
    if(stats.sys.mbox.used > stats.sys.mbox.max) {
	stats.sys.mbox.max = stats.sys.mbox.used;
    }
#endif /* SYS_STATS */

//    kprintf("sys_mbox_new()=0x%lx\n",mbox);
    return (sys_mbox_t)mbox;
}
/*-----------------------------------------------------------------------------------*/
void sys_mbox_free(sys_mbox_t mbox)
{
//    kprintf("sys_mbox_free(mbox=0x%lx)\n",mbox);

    if(mbox != SYS_MBOX_NULL) {
#ifdef SYS_STATS
	stats.sys.mbox.used--;
#endif /* SYS_STATS */
	sys_sem_wait(mbox->mutex);
    
	sys_sem_free(mbox->mail);
	sys_sem_free(mbox->mutex);
	mbox->mail = mbox->mutex = NULL;
	FreeVec(mbox);
    }
}
/*-----------------------------------------------------------------------------------*/
void sys_mbox_post(sys_mbox_t mbox, void *msg)
{
    u8_t first;

    if (!mbox)
    {
//	kprintf("sys_mbox_post(mbox=0x%lx,data=0x%lx)  no mbox!\n",mbox,msg);
	return;
    }

//    kprintf("sys_mbox_post(mbox=0x%lx,data=0x%lx)\n",mbox,msg);

    sys_sem_wait(mbox->mutex);
    mbox->msgs[mbox->last] = msg;

    if(mbox->last == mbox->first) {
        first = 1;
    } else {
        first = 0;
    }
  
    mbox->last++;
    if(mbox->last == SYS_MBOX_SIZE) {
	mbox->last = 0;
    }

    if(first) {
      sys_sem_signal(mbox->mail);
    }
  
    sys_sem_signal(mbox->mutex);
}
/*-----------------------------------------------------------------------------------*/
u16_t sys_arch_mbox_fetch(sys_mbox_t mbox, void **msg, u16_t timeout)
{
    u16_t time = 1;
//    kprintf("sys_arch_mbox_fetch(mbox=0x%lx,timeout=%ld)\n",mbox,timeout);

    /* The mutex lock is quick so we don't bother with the timeout
       stuff here. */
    sys_arch_sem_wait(mbox->mutex, 0);
  
    while(mbox->first == mbox->last)
    {
        sys_sem_signal(mbox->mutex);
    
        /* We block while waiting for a mail to arrive in the mailbox. We
           must be prepared to timeout. */
        if(timeout != 0)
        {
            time = sys_arch_sem_wait(mbox->mail, timeout);
      
          /* If time == 0, the sem_wait timed out, and we return 0. */
            if(time == 0) return 0;
        } else sys_arch_sem_wait(mbox->mail, 0);
	sys_arch_sem_wait(mbox->mutex, 0);
    }
  
    if (msg)
	*msg = mbox->msgs[mbox->first];
  
    mbox->first++;
    if(mbox->first == SYS_MBOX_SIZE)
        mbox->first = 0;

    sys_sem_signal(mbox->mutex);
    return time;
}

/*-----------------------------------------------------------------------------------*/
sys_sem_t sys_sem_new(u8_t count)
{
    struct sys_sem *sem;

    if (!(sem = AllocVec(sizeof(struct sys_sem),MEMF_PUBLIC)))
	return NULL;

//    kprintf("sys_sem_new(%ld) sem at 0x%lx\n",count,sem);

    sem->c = count;
    InitSemaphore(&sem->sem);
    NewList((struct List*)&sem->queue);

    return (sys_sem_t)sem;
}
/*-----------------------------------------------------------------------------------*/
u16_t sys_arch_sem_wait(sys_sem_t sem, u16_t timeout)
{
    struct ThreadData *data = (struct ThreadData*)FindTask(NULL)->tc_UserData;
    u16_t time = 1;

//    kprintf("sys_arch_sem_wait(0x%lx,%ld)\n",sem,timeout);

    ObtainSemaphore(&sem->sem);

    if (sem->c <= 0)
    {
	if (timeout > 0)
	{
	    struct timeval val;
	    struct Device *TimerBase = data->TimerReq->tr_node.io_Device;
	    struct timerequest *req;
	    GetSysTime(&val);

	    if ((req = Timer_Send(data, timeout)))
	    {
	    	ULONG sigs;
	    	int signaled = 0;

		/* We add us now in the wait queue, so sys_sem_signal() knows which signal has to be set */
		AddTail((struct List*)&sem->queue,(struct Node*)&data->task_node);

		ReleaseSemaphore(&sem->sem);
		sigs = Wait((1UL<<data->sem_signal)|(1UL<<data->TimerPort->mp_SigBit));
		ObtainSemaphore(&sem->sem);

                /* Now we remove us again */
                Remove((struct Node*)&data->task_node);

                if (sigs & (1UL<<data->sem_signal))
		    signaled = 1;

		if (sigs & (1UL<<data->TimerPort->mp_SigBit))
		{
		    struct timerequest *treq;

		    while ((treq = (struct timerequest*)GetMsg(data->TimerPort)))
		    {
			FreeVec(treq);
			data->TimerOutstanding--;
		    }
		} else Timer_Abort(data,req);

		if (signaled)
		{
		    /* Calulate the amount of time spent */
		    struct timeval val2;
		    GetSysTime(&val2);
		    SubTime(&val2,&val);
		    time = val2.tv_micro / 1000 + val2.tv_secs * 1000;
		} else
		{
		    ReleaseSemaphore(&sem->sem);
		    return 0;
		}
	    }
	}   else
	{
	    /* We add us now in the wait queue, so sys_sem_signal() knows which signal has to be set */
	    AddTail((struct List*)&sem->queue,(struct Node*)&data->task_node);

	    ReleaseSemaphore(&sem->sem);
	    Wait(1UL<<data->sem_signal);
	    ObtainSemaphore(&sem->sem);

	    /* Now we remove us again */
	    Remove((struct Node*)&data->task_node);
        }
    }

    sem->c--;
    ReleaseSemaphore(&sem->sem);
    return time;
}
/*-----------------------------------------------------------------------------------*/
void sys_sem_signal(sys_sem_t sem)
{
//    kprintf("sys_sem_signal(0x%lx)\n",sem);

    ObtainSemaphore(&sem->sem);

    sem->c++;
    if(sem->c > 1)
      sem->c = 1;

    if (!IsListEmpty((struct List*)&sem->queue))
    {
	struct TaskNode *task_node = (struct TaskNode *)sem->queue.mlh_Head;
	Signal(task_node->task,(1UL<<task_node->sem_signal));
    }

    ReleaseSemaphore(&sem->sem);
}
/*-----------------------------------------------------------------------------------*/
void sys_sem_free(sys_sem_t sem)
{
//    kprintf("sys_sem_free(%lx)\n",sem);
    if (sem) FreeVec(sem);
}
/*-----------------------------------------------------------------------------------*/
void sys_init(void)
{
//    kprintf("sys_init() ");
    if (Thread_Init(&mainThread))
    {
//    	kprintf("successfull\n");
    	FindTask(NULL)->tc_UserData = &mainThread;
    }// else kprintf("failed\n");
}
/*-----------------------------------------------------------------------------------*/
struct sys_timeouts *sys_arch_timeouts(void)
{
    struct ThreadData *data = (struct ThreadData*)FindTask(NULL)->tc_UserData;

//    kprintf("sys_arch_timeouts()\n");
    return &data->timeouts;
}
/*-----------------------------------------------------------------------------------*/
void sys_thread_new(void (* function)(void *arg), void *arg)
{
    struct Process *pr;
    struct StartupMsg *start_msg;
    struct MsgPort *child_port;

//    kprintf("sys_thread_new()\n");

    if (!(start_msg = (struct StartupMsg *)AllocMem(sizeof(struct StartupMsg), MEMF_PUBLIC|MEMF_CLEAR)))
    {
	return;
    }

    if (!(start_msg->msg.mn_ReplyPort = CreateMsgPort()))
    {
	FreeMem((void *)start_msg, sizeof(*start_msg));
	return;
    }

    CacheClearU();

    pr = CreateNewProcTags(NP_Entry, Thread_Entry, TAG_DONE);

    if (!pr)
    {
	DeleteMsgPort(start_msg->msg.mn_ReplyPort);
	FreeMem((void *)start_msg, sizeof(*start_msg));
	return;
    }

    child_port = &pr->pr_MsgPort;

    /* Fill in the rest of the startup message */
    start_msg->msg.mn_Length = sizeof(struct StartupMsg);
    start_msg->msg.mn_Node.ln_Type = NT_MESSAGE;
    start_msg->child = pr;
   
    start_msg->global_data = (void *)getreg(REG_A4);  /* Save global data reg (A4) */
    start_msg->fp = function;                         /* Fill in function pointer */
    start_msg->UserData = arg;   
   
    /* Send startup message to child */
    PutMsg(child_port, (struct Message *)start_msg);
    WaitPort(start_msg->msg.mn_ReplyPort);
    FreeMem(start_msg,sizeof(struct StartupMsg));
}
/*-----------------------------------------------------------------------------------*/
