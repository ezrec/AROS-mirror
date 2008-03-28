#include "includes.h"

#include <proto/socket.h>

extern int errno;	    /* defined by linker */

struct Library *SockBase;   /* defined here, not SockBase.c */

void
_STD_300_as225 (void)
{
	if (SockBase)
	{
		cleanup_sockets ();
		CloseLibrary (SockBase);
		SockBase = NULL;
	}

	return;
}

int
_STI_300_as225 (void)
{
	SockBase = OpenLibrary ("inet:libs/socket.library", 4);
	if (!SockBase)
	{
		return 1;
	}

	if (!setup_sockets (32, &errno))
	{
		/* on socket.library 6.1+, setup_sockets() cannot fail */
		/* on socket.library 4.2, it'll only fail if a 128 byte malloc() fails */
		_STD_300_as225 ();
		return 1;
	}

	return 0;
}

FILE *
fdopen (int fd, const char *mode)
{
	/* used only by WAIS */
	return NULL;
}

