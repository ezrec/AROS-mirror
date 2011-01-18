/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id$
*/

#define DEBUG 1
#include <aros/debug.h>

#include <pthread.h>

#include <dos/dostags.h>
#include <proto/dos.h>      /* CreateNewProc */

#include <stdlib.h>
#include <stdio.h>

void Proxy(void)
{
	struct AROSPThreadMessage *msg;
	struct aros_pthread_t* aros_pthread;

	struct Task *child = FindTask(NULL);

	pthread_t* t = FindTask(NULL)->tc_UserData;
	aros_pthread = t->p; 

	D(bug("[pthread] proxy: started.\n"));
	
	/* Make sure, we don't block the parent. */
	//SetTaskPri(child, -1);

    	D(bug("[pthread] proxy: Progressing...\n"));
        
        msg = &(aros_pthread->msg);

	/* start embedded function */
        msg->ret = msg->func(msg->arg);     

	/* notification about function beeing finished, send function´s return value */
    	D(bug("[pthread] proxy: sending return msg...\n"));
	PutMsg(aros_pthread->replyport, msg);

	/* quit the child, exec.library will reply the startup msg internally. */
	D(bug("[pthread] proxy: Done.\n"));

}


int pthread_create(
    pthread_t*            thread,
    const pthread_attr_t* attr,
    void*                 (*func) (void*),
    void*                 arg)
{
	struct Task *parent;
	struct aros_pthread_t* aros_pthread; 
            
	D(bug("[pthread] Create thread...\n"));
        
	if ( (thread == NULL) ) return NULL;

	thread->x = 0;
    
	parent = FindTask(NULL);

	//aros_pthread = (struct aros_pthread_t *) malloc ( sizeof(struct aros_pthread_t) );
	aros_pthread = (struct aros_pthread_t *) AllocMem(  sizeof(struct aros_pthread_t), MEMF_PUBLIC|MEMF_CLEAR);
	//(*thread).p = aros_pthread;
	thread->p = aros_pthread;

	kprintf("[pthread] create: data allocated.. aros_pthread: %x, thread: %p, count: %x\n", aros_pthread, thread, aros_pthread->count);

	/* make sure main thread runs with sane priority */
	SetTaskPri(parent, 0);

	/* Things we'll pass down to the child by utilizing NP_StartupMsg */
	aros_pthread->msg.func = func; // function;
	aros_pthread->msg.arg  = arg;
	aros_pthread->msg.ret  = NULL;

	aros_pthread->replyport = CreateMsgPort();

	if (aros_pthread->replyport != NULL)
	{
		struct Process *child;

		aros_pthread->msg.msg.mn_Node.ln_Type = NT_MESSAGE;
		aros_pthread->msg.msg.mn_ReplyPort    = aros_pthread->replyport;
		aros_pthread->msg.msg.mn_Length       = sizeof(struct AROSPThreadMessage);

		child = CreateNewProcTags(
			NP_Entry,	Proxy,
			//NP_StartupMsg,   (ULONG)&t->msg,
			NP_UserData,	(IPTR)thread,
			NP_Name,	(ULONG)"pthread",
			NP_StackSize,	8192,			
			TAG_DONE);
        
		if (child != NULL) 
		{
			aros_pthread->count = 1;
			aros_pthread->task = child;
			D(bug("[pthread] Child process launched.\n"));
						
			return 0;
		}
		DeleteMsgPort(aros_pthread->replyport);
	}
	
	D(bug("[pthread] Child process failed.\n"));

	/* Free everything at once */
	//DeletePool(pool);
	FreeMem(aros_pthread, sizeof(struct aros_pthread_t) );
    
	return -1;	
}
