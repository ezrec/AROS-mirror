/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#include <string.h>
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
    if (!(locdata->pool = CreatePool(0,4096,4096)))
    {
	FreeVec(locdata);
	return NULL;
    }
    return locdata;
}

/**************************************************************************
 ...
**************************************************************************/
void LocalData_Dispose(struct Library *SocketBase, struct LocalData *locdata)
{
    if (!locdata) return;
    if (locdata->pool) DeletePool(locdata->pool);
    FreeVec(locdata);
}

/**************************************************************************
 Allocate some private memory (using pool). Will be freed in
 LocalData_Dispose or mem_free()
**************************************************************************/
void *ld_mem_allocate(struct Library *SocketBase, int size)
{
    int *mem = AllocPooled(LOCDATA(SocketBase)->pool,size+sizeof(int));
    if (!mem) return NULL;
    mem[0] = size;
    return mem+1;
}

/**************************************************************************
 Free the memory allocated by ld_mem_allocate
**************************************************************************/
void ld_mem_free(struct Library *SocketBase, void *mem)
{
    int *m = mem;
    if (m)
    {
	m--;
	FreePooled(LOCDATA(SocketBase)->pool,m,m[0]+sizeof(int));
    }
}

