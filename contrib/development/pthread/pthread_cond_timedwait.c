/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/debug.h>

#include <pthread.h>

int pthread_cond_timedwait(
    pthread_cond_t*         cond,
    pthread_mutex_t*        mutex,
    const struct timespec* abstime)
{
#   warning Implement pthread_cond_timedwait()
    AROS_FUNCTION_NOT_IMPLEMENTED("pthread_cond_timedwait");
	
    return 0;
}
