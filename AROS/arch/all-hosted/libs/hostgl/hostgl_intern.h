/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef HOSTGL_INTERN_H
#define HOSTGL_INTERN_H

#include <exec/semaphores.h>
#include <proto/exec.h>

extern struct SignalSemaphore GLOBAL_GLX_CONTEXT_SEM;

#define HOSTGL_PRE                                          \
    ObtainSemaphore(&GLOBAL_GLX_CONTEXT_SEM);

#define HOSTGL_POST                                         \
    ReleaseSemaphore(&GLOBAL_GLX_CONTEXT_SEM);

#endif /* HOSTGL_INTERN_H */
