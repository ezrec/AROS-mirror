/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id: $
*/

#include <aros/debug.h>

#include <pthread.h>

#include <proto/exec.h>

pthread_t pthread_self(void)
{	
    return (pthread_t) FindTask(NULL);
}
