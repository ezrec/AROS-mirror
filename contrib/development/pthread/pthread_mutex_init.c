/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/debug.h>

#include <pthread.h>

#include <proto/exec.h>

int pthread_mutex_init(
    pthread_mutex_t*           mutex,
    const pthread_mutexattr_t* attr)
{
#   warning Implement pthread_mutex_init()
    AROS_FUNCTION_NOT_IMPLEMENTED("pthread_mutex_init");
	
    InitSemaphore(&mutex->sigSem);
    mutex->Initialized = TRUE;
	
    return 0;
}
