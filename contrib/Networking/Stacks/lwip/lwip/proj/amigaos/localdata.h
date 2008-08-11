/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#ifndef _LOCALDATA_H
#define _LOCALDATA_H

#ifndef _NETDB_H
#include <netdb.h>
#endif

struct LocalData
{
    struct hostent *gethostbyname_hostent; /* for gethostbyname() */

    char inet_ntoa[40];

    void *pool; /* memory pool */
};

struct LocalData *LocalData_New(struct Library *SocketBase);
void LocalData_Dispose(struct Library *SocketBase, struct LocalData *locdata);

/* ld means localdata (mem_free() is used by lwio itself) */
void *ld_mem_allocate(struct Library *SocketBase, int size);
void ld_mem_free(struct Library *SocketBase, void *mem);

#endif
