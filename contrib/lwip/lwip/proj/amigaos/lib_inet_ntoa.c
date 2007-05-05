/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id$
*/

#include <stdio.h>

#include <exec/types.h>
#include <proto/exec.h>

#include "lwip/inet.h" /* ntohl() */

#include "localdata.h"

#include "socket_intern.h"

#define MYDEBUG
#include "debug.h"

/*****************************************************************************

    NAME */
#ifndef __AROS__
__asm char *LIB_Inet_NtoA(register __d0 unsigned long ip)
#else
	AROS_LH1(char *, LIB_Inet_NtoA,

/*  SYNOPSIS */
  AROS_LHA(unsigned long, ip, D0),

/*  LOCATION */
	struct Library *, SocketBase, 174, Socket)
#endif

/*  FUNCTION

	Convert network-format internet address
	to base 256 d.d.d.d representation.

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
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,SocketBase)

    D(bug("Inet_NtoA()\n"));

    ip = ntohl(ip);
    sprintf(LOCDATA(SocketBase)->inet_ntoa,"%ld.%ld.%ld.%ld",(ip>>24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);

    return LOCDATA(SocketBase)->inet_ntoa;

    AROS_LIBFUNC_EXIT
}
