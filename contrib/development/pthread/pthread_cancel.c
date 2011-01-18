/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id$
*/

#define DEBUG 1
#include <aros/debug.h>

#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>

int pthread_cancel(pthread_t thread)
{
	struct AROSPThreadMessage *reply;
	struct aros_pthread_t* aros_pthread = thread.p; 
	void *ret;
    	
	kprintf("[pthread] cancel: Progressing... aros_pthread: %p, thread: %p, count: %d\n", aros_pthread, &thread, aros_pthread->count);

	if ( (aros_pthread == NULL) || (aros_pthread->count == 0) || (aros_pthread->replyport == NULL) ) 
	{
		D(bug("[pthread] cancel: thread deleted before, exiting...\n"));
		return -1;
	}

	D(bug("[pthread] cancel: deleting thread.. \n"));  

	/* disactivate thread */
	aros_pthread->count = 0;

	/* delete msg port & thread task */
	DeleteMsgPort(aros_pthread->replyport);
	RemTask(aros_pthread->task);

	/* delete thread memory */
	FreeMem(aros_pthread, sizeof(struct aros_pthread_t) );

	D(bug("[pthread] cancel: thread deleted\n"));         	
	return 0;
}
