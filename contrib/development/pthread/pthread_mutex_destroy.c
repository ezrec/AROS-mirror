/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/debug.h>

#include <pthread.h>

int pthread_mutex_destroy(pthread_mutex_t* mutex)
{
#   warning Implement pthread_mutex_destroy()
    AROS_FUNCTION_NOT_IMPLEMENTED("pthread_mutex_destroy");
	
    mutex->Initialized = FALSE;
	
    return 0;
}
