/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id$
*/

#define DEBUG 1
#include <aros/debug.h>

#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>

int pthread_join(pthread_t thread, void** exit_value)
{
	struct AROSPThreadMessage *reply;
	struct aros_pthread_t* aros_pthread = thread.p; 

	kprintf("[pthread] join: Progressing... aros_pthread: %p, thread: %p, count: %d\n", aros_pthread, &thread, aros_pthread->count);

	if ( (aros_pthread == NULL) || (aros_pthread->count == 0) || (aros_pthread->replyport == NULL) ) 
	{
		D(bug("[pthread] join: thread deleted before, exiting...\n"));
		return -1;
	}

	/* wait child process so complete */
	D(bug("[pthread] join: wait for child to quit...\n"));
	WaitPort(aros_pthread->replyport);
	D(bug("[pthread] join: waitport finished...\n"));

	/* get function´s return value */
	reply = (struct AROSPThreadMessage *)GetMsg(aros_pthread->replyport);
	/* XXX: return value should be handled here: reply->ret */   

	D(bug("[pthread] join: msg catched..\n"));
       
	/* disactivate thread */         
	aros_pthread->count = 0;

	/* delete msg port & thread memory */
	DeleteMsgPort(aros_pthread->replyport);
	FreeMem(aros_pthread, sizeof(struct aros_pthread_t) );

	D(bug("[pthread] join: join finished...\n"));
	
	return 0;
}
