/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#include <exec/types.h>
#include <proto/exec.h>

#include "socket_intern.h"

#define MYDEBUG
#include "debug.h"

/**************************************************************************
 ...
**************************************************************************/
struct GlobalData *GlobalData_New(struct Library *SocketBase)
{
    struct GlobalData *gldata = AllocVec(sizeof(struct GlobalData),MEMF_PUBLIC|MEMF_CLEAR);
    if (!gldata) return NULL;
    InitSemaphore(&gldata->hosts_semaphore);
    return gldata;
}

/**************************************************************************
 ...
**************************************************************************/
void GlobalData_Dispose(struct Library *SocketBase, struct GlobalData *gldata)
{
    if (!gldata) return;
    FreeVec(gldata);
}

