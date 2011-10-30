/*
    Copyright 2011, The AROS Development Team. All rights reserved.
    $Id$
*/

#ifndef HOSTGL_CTX_MANAGER_H
#define HOSTGL_CTX_MANAGER_H

#include "hostgl_types.h"

AROSMesaContext HostGL_GetCurrentContext();
VOID HostGL_SetCurrentContext(AROSMesaContext ctx);
VOID HostGL_Lock();
VOID HostGL_UnLock();
VOID HostGL_SetGlobalGLXContext();

#define HOSTGL_PRE                                          \
    HostGL_Lock();                                          \
    HostGL_SetGlobalGLXContext();

#define HOSTGL_POST                                         \
    HostGL_UnLock();

#endif /* HOSTGL_CTX_MANAGER_H */
