/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id: lib_gethostbyname.c,v 1.2 2002/07/28 11:13:55 sebauer Exp $
*/

#include <string.h>

#include <exec/types.h>
#include <proto/exec.h>

#include "lwip/sys.h"
#include "lwip/sockets.h"

#include "inet_aton.h"

#include "localdata.h"
#include "socket_intern.h"

#define MYDEBUG
#include "debug.h"

/*****************************************************************************

    NAME */
#ifndef __AROS
__asm struct hostent *LIB_gethostbyname(register __a0 const char *name)
#else
	AROS_LH1(struct hostent *, LIB_gethostbyname,

/*  SYNOPSIS */
  AROS_LHA(const char *, name, A0),

/*  LOCATION */
	struct Library *, SocketBase, 210, Socket)
#endif

/*  FUNCTION

    INPUTS

    RESULT

    NOTES

    EXAMPLE

    BUGS
	The function itself is a bug ;-) Remove it!

    SEE ALSO

    INTERNALS

    HISTORY

*****************************************************************************/
{
    struct in_addr in;

    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,SocketBase)

    /* check first if it is a direct ip address */
    if (inet_aton(name,&in))
    {
	struct hostent *hostent;
	u32_t *ptr;

	ld_mem_free(SocketBase,LOCDATA(SocketBase)->gethostbyname_hostent);
	hostent = ld_mem_allocate(SocketBase,sizeof(struct hostent)+24+strlen(name));
	LOCDATA(SocketBase)->gethostbyname_hostent = hostent;
	if (!hostent) return NULL;
	ptr = (u32_t*)(hostent + 1);

        memset(hostent,0,sizeof(struct hostent)+32);
	hostent->h_addrtype = AF_INET;
	hostent->h_length = sizeof(struct in_addr);
	hostent->h_addr_list = (char**)ptr;
	*ptr = (u32_t)(ptr + 2); /* avoid side effects */
	*++ptr = NULL;
	*++ptr = in.s_addr;
	hostent->h_aliases = (char**)(++ptr);
	*ptr++ = NULL;
	hostent->h_name = strcpy((char*)ptr,name);

	return hostent;
    }


    D(bug("gethostbyname()\n"));

    return 0;

    AROS_LIBFUNC_EXIT

}
