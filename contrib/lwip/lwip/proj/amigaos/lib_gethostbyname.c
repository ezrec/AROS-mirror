/*
    Copyright © 2002, The AROS Development Team. 
    All rights reserved.
    
    $Id: lib_gethostbyname.c,v 1.1 2002/07/27 20:35:59 sebauer Exp $
*/

#include <exec/types.h>
#include <proto/exec.h>

#include "socket_intern.h"

#define MYDEBUG
#include "debug.h"

/*****************************************************************************

    NAME */
#ifndef __AROS
__asm struct hostent *LIB_gethostbyname(register __a0 char *name)
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
    AROS_LIBFUNC_INIT
    AROS_LIBBASE_EXT_DECL(struct Library *,SocketBase)

    D(bug("gethostbyname()\n"));

    return 0;

    AROS_LIBFUNC_EXIT

}
