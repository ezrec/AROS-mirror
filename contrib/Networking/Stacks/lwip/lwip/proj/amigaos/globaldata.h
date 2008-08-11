/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#ifndef _GLOBALDATA_H
#define _GLOBALDATA_H

#ifndef EXEC_SEMAPHORES_H
#include <exec/semaphores.h>
#endif

#ifndef _NETDB_H
#include <netdb.h>
#endif

struct HostentNode 
{
    struct MinNode node;
    struct hostent hostent;
};

struct GlobalData
{
  APTR pool;

	struct SignalSemaphore hosts_semaphore; /* Semaphore for the hosts database */
	struct MinList static_hosts_list; /* struct HostentNode * */
	struct MinList dns_hosts_list; /* struct HostentNode * */
};

struct GlobalData *GlobalData_New(struct Library *SocketBase);
void GlobalData_Dispose(struct Library *SocketBase, struct GlobalData *gldata);

void *gl_mem_allocate(struct Library *SocketBase, int size);
void gl_mem_free(struct Library *SocketBase, void *mem);

#endif
