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
 * $Id$
 */

#include <time.h>

#include <exec/memory.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>

#include "lwip/sys.h"
#include "lwip/def.h"

struct sys_sem {
  unsigned int c;
  struct SignalSemaphore sem;
};

struct PtrMessage
{
  struct Message msg;
  void *ptr;
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
    }
    return time;
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
    return 1;
}

/*-----------------------------------------------------------------------------------*/

static void Thread_Cleanup(struct ThreadData *data)
{
    if (data->sem_signal != -1) FreeSignal(data->sem_signal);
    Timer_Cleanup(data);
}


/*-----------------------------------------------------------------------------------*/
void sys_arch_block(u16_t time)
{
    kprintf("block for %ld ms\n",time);
}
/*-----------------------------------------------------------------------------------*/
sys_mbox_t sys_mbox_new(void)
{
    sys_mbox_t mbox = (sys_mbox_t)CreateMsgPort();
    kprintf("sys_mbox_new()=0x%lx\n",mbox);
    return mbox;
}
/*-----------------------------------------------------------------------------------*/
void sys_mbox_free(sys_mbox_t mbox)
{
    kprintf("sys_mbox_free(mbox=0x%lx)\n",mbox);
//    if (mbox) DeleteMsgPort((struct MsgPort*)mbox);
}
/*-----------------------------------------------------------------------------------*/
void sys_mbox_post(sys_mbox_t mbox, void *data)
{
    struct PtrMessage *msg;

    if (!mbox)
    {
	kprintf("sys_mbox_post(mbox=0x%lx,data=0x%lx)  no mbox!\n",mbox,data);
	return;
    }

    msg = AllocVec(sizeof(struct PtrMessage),MEMF_CLEAR|MEMF_PUBLIC);
    kprintf("sys_mbox_post(mbox=0x%lx,data=0x%lx)  msg at 0x%lx\n",mbox,data,msg);
//    if (msg)
//    {
//    	msg->msg.mn_Length = sizeof(struct PtrMessage);
//    	msg->ptr = data;
//    	PutMsg((struct MsgPort*)mbox,(struct Message*)msg);
//    }
}
/*-----------------------------------------------------------------------------------*/
u16_t sys_arch_mbox_fetch(sys_mbox_t mbox, void **data, u16_t timeout)
{
    kprintf("sys_arch_mbox_fetch(mbox=0x%lx,timeout=%d)\n",mbox,timeout);
    return 0;
}
/*-----------------------------------------------------------------------------------*/
sys_sem_t sys_sem_new(u8_t count)
{
    struct sys_sem *sem;

    if (!(sem = AllocVec(sizeof(struct sys_sem),MEMF_PUBLIC)))
	return NULL;

    kprintf("sys_sem_new(%ld) sem at 0x%lx\n",count,sem);

    sem->c = count;
    InitSemaphore(&sem->sem);

    return (sys_sem_t)sem;
}
/*-----------------------------------------------------------------------------------*/
u16_t sys_arch_sem_wait(sys_sem_t sem, u16_t timeout)
{
    kprintf("sys_arch_sem_wait(0x%lx,%ld)\n",sem,timeout);
    sys_arch_block(timeout);
    return 0;
}
/*-----------------------------------------------------------------------------------*/
void sys_sem_signal(sys_sem_t sem)
{
    kprintf("sys_sem_signal(0x%lx)\n",sem);
    return;
}
/*-----------------------------------------------------------------------------------*/
void sys_sem_free(sys_sem_t sem)
{
    kprintf("sys_sem_free(%lx)\n",sem);
    if (sem) FreeVec(sem);
}
/*-----------------------------------------------------------------------------------*/
void sys_init(void)
{
    kprintf("sys_init() ");
    if (Thread_Init(&mainThread))
    {
    	kprintf("successfull\n");
    	FindTask(NULL)->tc_UserData = &mainThread;
    } else kprintf("failed\n");
}
/*-----------------------------------------------------------------------------------*/
struct sys_timeouts *sys_arch_timeouts(void)
{
    struct ThreadData *data = (struct ThreadData*)FindTask(NULL)->tc_UserData;

    kprintf("sys_arch_timeouts()\n");
    return &data->timeouts;
}
/*-----------------------------------------------------------------------------------*/
void sys_thread_new(void (* function)(void *arg), void *arg)
{
    kprintf("sys_thread_new()\n");
}
/*-----------------------------------------------------------------------------------*/
