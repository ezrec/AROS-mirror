/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#ifndef _GLOBALDATA_H
#define _GLOBALDATA_H

struct GlobalData
{
	struct SignalSemaphore hosts_semaphore;
};

struct GlobalData *GlobalData_New(struct Library *SocketBase);
void GlobalData_Dispose(struct Library *SocketBase, struct GlobalData *gldata);

#endif
