/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id: $
*/

#include <aros/debug.h>

#include <pthread.h>

int pthread_create(
    pthread_t*            thread,
    const pthread_attr_t* attr,
    void*                 (*func) (void*),
    void*                 arg)
{
#   warning Implement pthread_create()
    AROS_FUNCTION_NOT_IMPLEMENTED("pthread_create");
	
    return 0;
}
