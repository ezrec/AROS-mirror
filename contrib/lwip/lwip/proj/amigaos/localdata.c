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
struct LocalData *LocalData_New(struct Library *SocketBase)
{
	struct LocalData *locdata = AllocVec(sizeof(struct LocalData),MEMF_PUBLIC|MEMF_CLEAR);
	if (!locdata) return NULL;
}

/**************************************************************************
 ...
**************************************************************************/
void LocalData_Dispose(struct Library *SocketBase, struct LocalData *locdata)
{
	if (!locdata) return;
}

