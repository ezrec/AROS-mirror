/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/debug.h>

#include <pthread.h>

#include <proto/exec.h>

pthread_t pthread_self(void)
{	
//    return (pthread_t) FindTask(NULL);

#   warning Implement pthread_self()
    AROS_FUNCTION_NOT_IMPLEMENTED("pthread_self");
	
	pthread_t temp;
	return temp;
}
