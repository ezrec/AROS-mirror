/*
    Copyright © 2007, The AROS Development Team. All rights reserved.
    $Id$
*/

#include <aros/debug.h>

#include <pthread.h>

#include <proto/exec.h>

int pthread_mutex_trylock(pthread_mutex_t* mutex)
{
    if (AttemptSemaphore(&mutex->sigSem)) {
        return 0;
    } else {
        return -1;
    }
}
