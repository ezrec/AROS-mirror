/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
*/

/* We redefine the timeval here, because lwip defines an own timeval
 * struct */
#define timeval timeval_amiga

#include <exec/types.h>
#include <clib/alib_protos.h>
#include <proto/exec.h>

#undef timeval

#include "lwip/sys.h"
#include "lwip/sockets.h"

#include "socket_intern.h"

#define MYDEBUG
#include "debug.h"

#ifndef BYTE_ORDER
#error "define a proper BYTE_ORDER"
#endif

#if BYTE_ORDER == BIG_ENDIAN
#define LOCALHOST 0x7f000001
#else
#define LOCALHOST 0x0100007f
#endif

struct HostentNode localnode;
char *local_aliases[] = {"local",NULL};
struct in_addr local = {LOCALHOST};
struct in_addr *local_addr[] = {&local,NULL};

/**************************************************************************
 ...
**************************************************************************/
struct GlobalData *GlobalData_New(struct Library *SocketBase)
{
    struct GlobalData *gldata = AllocVec(sizeof(struct GlobalData),MEMF_PUBLIC|MEMF_CLEAR);
    if (!gldata) return NULL;
    if (!(gldata->pool = CreatePool(MEMF_PUBLIC,4096,4096)))
    {
	FreeVec(gldata);
	return NULL;
    }
    InitSemaphore(&gldata->hosts_semaphore);
    NewList((struct List*)&gldata->static_hosts_list);
    NewList((struct List*)&gldata->dns_hosts_list);

    /* hardcoded */
    localnode.hostent.h_name = "localhost";
    localnode.hostent.h_aliases = local_aliases;
    localnode.hostent.h_length = sizeof(struct in_addr);
    localnode.hostent.h_addr_list = (char**)local_addr;
    localnode.hostent.h_addrtype = AF_INET;

    AddTail((struct List*)&gldata->static_hosts_list,(struct Node*)&localnode);

    return gldata;
}

/**************************************************************************
 ...
**************************************************************************/
void GlobalData_Dispose(struct Library *SocketBase, struct GlobalData *gldata)
{
    if (!gldata) return;
    if (gldata->pool) DeletePool(gldata->pool);
    FreeVec(gldata);
}

/**************************************************************************
 ...
**************************************************************************/
void *gl_mem_allocate(struct Library *SocketBase, int size)
{
    int *mem = AllocPooled(GLDATA(SocketBase)->pool,size+sizeof(int));
    if (!mem) return NULL;
    mem[0] = size;
    return mem+1;
}

/**************************************************************************
 ...
**************************************************************************/
void gl_mem_free(struct Library *SocketBase, void *mem)
{
    int *m = mem;
    if (m)
    {
	m--;
	FreePooled(LOCDATA(SocketBase)->pool,m,m[0]+sizeof(int));
    }
}

