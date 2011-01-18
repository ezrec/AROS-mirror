/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/debug.h>

#include <pthread.h>

#include <proto/exec.h>

int pthread_mutex_lock(pthread_mutex_t* mutex)
{
#   warning Implement pthread_mutex_lock()
    AROS_FUNCTION_NOT_IMPLEMENTED("pthread_mutex_lock");
	
    ObtainSemaphore(&mutex->sigSem);
	
    return 0;
}
