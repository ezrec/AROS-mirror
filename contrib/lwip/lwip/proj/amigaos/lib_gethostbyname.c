/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#include <string.h>

/* We redefine the timeval here, because lwip defines an own timeval struct,
   should be changed in the future */
#define timeval timeval_amiga

#include <exec/types.h>
#include <proto/exec.h>

#include "inet_aton.h"
#include "localdata.h"
#include "socket_intern.h"

#undef timeval

#include "lwip/sys.h"
#include "lwip/sockets.h"

#define MYDEBUG
#include "debug.h"

/* Some helper macros */
#define TRAVERSE_MINLIST(l,name,type) \
  {\
    type *name;\
    for (name = (type*)(((struct MinList*)(l))->mlh_Head);\
         ((struct MinNode*)name)->mln_Succ;\
         name = (type*)((struct MinNode*)name)->mln_Succ)\

#define TRAVERSE_END }


/*****************************************************************************

    NAME */
#ifndef __AROS__
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
    struct hostent *ret = NULL;


/*      struct HostentNode *node; */


    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,SocketBase)

    D(bug("gethostbyname()\n"));

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

    /* Now try the static host list */
    ObtainSemaphoreShared(&GLDATA(SocketBase)->hosts_semaphore);

    TRAVERSE_MINLIST(&GLDATA(SocketBase)->static_hosts_list,node,struct HostentNode)
    {
    	int found = 0;

	if (node->hostent.h_name && !stricmp(node->hostent.h_name,name)) found = 1;

	if (!found)
	{
	    int i;
	    for (i=0;node->hostent.h_aliases[i];i++)
	    {
		if (!stricmp(node->hostent.h_aliases[0],name))
		{
		    found = 1;
		    break;
		}
	    }
	}

	if (found)
	{
	    struct hostent *hostent;
	    char **ptr;
	    int i;
	    int len = sizeof(struct hostent) + (((strlen(node->hostent.h_name)+1)+3)/4)*4;
	    int num_aliases = 0;
	    int num_addrs = 0;
	    char *alias_ptr;
	    char *list_ptr;

	    for (i=0;node->hostent.h_aliases[i];i++)
	    {
	    	num_aliases++;
	    	len += ((strlen(node->hostent.h_aliases[i])+1+3)/4)*4;
	    }

	    for (i=0;node->hostent.h_addr_list[i];i++)
	    {
		num_addrs++;
		len += node->hostent.h_length;
	    }

	    ld_mem_free(SocketBase,LOCDATA(SocketBase)->gethostbyname_hostent);

	    len += (num_aliases+1) * sizeof(char*) + (num_addrs+1) * sizeof(char*) + 60;
	    hostent = ld_mem_allocate(SocketBase,len);
	    memset(hostent,0,len);
	    if (!hostent) break;
	    ptr = (char**)(hostent + 1);

	    /* Copy aliases */
	    hostent->h_aliases = ptr;
	    alias_ptr = (char*)(ptr + num_aliases + 1);
	    for (i=0;i<num_aliases;i++)
	    {
	    	*ptr++ = alias_ptr;
		strcpy(alias_ptr,node->hostent.h_aliases[i]);
		alias_ptr += ((strlen(node->hostent.h_aliases[i])+1+3)/4*4);
	    }
	    *ptr++ = NULL;

	    /* Copy addr_list */
	    ptr = (char**)alias_ptr;
	    hostent->h_addr_list = ptr;
	    list_ptr = alias_ptr + (num_addrs+1) * sizeof(char*);
	    for (i=0;i<num_addrs;i++)
	    {
		*ptr++ = list_ptr;
		memcpy(list_ptr,node->hostent.h_addr_list[i],node->hostent.h_length);
		list_ptr += node->hostent.h_length;
	    }
	    *ptr++ = NULL;

	    hostent->h_name = list_ptr;
	    strcpy(list_ptr,node->hostent.h_name);
	    hostent->h_length = node->hostent.h_length;
	    hostent->h_addrtype = node->hostent.h_addrtype;

	    ret = hostent;
	}
    }
    TRAVERSE_END

    ReleaseSemaphore(&GLDATA(SocketBase)->hosts_semaphore);

    return ret;

    AROS_LIBFUNC_EXIT

}
