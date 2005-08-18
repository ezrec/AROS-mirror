/*
 *	dayserv.c
 *
 * server for tcp daytime service (Amiga® format)
 *
 * adapted from "UNIX Network Programming, Vol.1, 2nd Edition" by W. Richard Stevens
 * for Amiga® OS with AmiTCP 4.x compatible network stacks
 *
 *	important changes:
 *		· returns a "seconds.microseconds" format, not the unix standard format
 *
 * should work with any compiler:
 * 	· link additional startup routine "startup.c" for Maxon C/C++ only
 *		· uses (small & fast) external routine SPrintF for Maxon C/C++ only
 */
#ifndef __AROS__
#include <pragma/exec_lib.h>
#include <pragma/dos_lib.h>
#include <pragma/intuition_lib.h>
#include <pragma/socket_lib.h>
#else
#include <proto/dos.h>
#include <proto/exec.h>
#include <stdio.h>
#include <dos/dos.h>
#include <clib/dos_protos.h>
#include <defines/dos.h>
#include <arpa/inet.h> 
#include <sys/types.h>
#include <defines/intuition.h>
#define AF_INET 2 /* we need to fix our includes */
#define SOCK_STREAM 1
#endif /* __AROS__ */
#ifndef SOCKETNAME
#define SOCKETNAME "bsdsocket.library"
#endif

/* #include <sys/socket.h> */
#include <string.h>


#define DAYTIMEPORT	13
#define LISTENQUEUE	5	/* this is a table entry - "5" means the predefined TCP/IP queue count ! */

#ifdef __MAXON__
#define vsprintf(a,b,c) SPrintF(b,c,a)
#else
#include <stdio.h>
#endif


struct Library *SocketBase = NULL, *IntuitionBase = NULL;


/* external assembler function */
#ifdef __MAXON__
void SPrintF(register __a0 STRPTR fmtstring, register __a1 APTR data, register __a3 UBYTE *buffer);
#endif


/* resource handling */
BOOL openLib(void)
{
	BOOL rw = FALSE;
	
	SocketBase = OpenLibrary(SOCKETNAME, 4U);
	IntuitionBase = OpenLibrary("intuition.library", 37U);
	
	if(SocketBase && IntuitionBase)
	{
		rw = TRUE;
	}
	
	return(rw);
}

void closeLib(void)
{
	CloseLibrary(IntuitionBase);
	IntuitionBase = NULL;
	
	CloseLibrary(SocketBase);
	SocketBase = NULL;
}


/* main function */
LONG main(LONG argc, APTR argv)
{
	struct RDArgs *rdargs;
	STRPTR tmpl = "PORT/N";
	LONG arg[1] = {0L};
	LONG rw;
	int serviceport = DAYTIMEPORT;
	
	/* shell only - this is a learning demo */
	if(argc)
	{
		/* get port number - usually 13 is already used */
		rdargs = ReadArgs(tmpl, arg, NULL);
		if(rdargs)
		{
			serviceport = arg[0] ? *((LONG *) arg[0]) : serviceport;
		}
		
		/* open libraries */
		if(openLib())
		{
			long listenfd, connfd;
			struct sockaddr_in servaddr;
			
			/* create listener socket */
			listenfd = socket(AF_INET, SOCK_STREAM, 0L);
			if(listenfd >= 0L)
			{
				/* specify service and interface */
				memset(&servaddr, 0L, sizeof(struct sockaddr_in));
				servaddr.sin_family			= AF_INET;
				servaddr.sin_port				= htons(serviceport);
				servaddr.sin_addr.s_addr	= htonl(INADDR_ANY);	/* sin_addr == s_addr */
				
				/* add our server to the port... */
				if(bind(listenfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) >= 0L)
				{
					/* ...and start listening for requests */
					if(listen(listenfd, LISTENQUEUE) >= 0L)
					{
						/* there is no normal exit from an endless loop, so RETURN_OK is never used */
						for(;;)
						{
							/* standard for()-loop emergency break */
							if(CheckSignal(SIGBREAKF_CTRL_C))
							{
								PutStr("^C -- user break\n");
								rw = RETURN_WARN;
								break;
							}
							
							/* serve client - accept() has a ctrl_c handling itself */
							connfd = accept(listenfd, NULL, NULL);
							if(connfd >= 0L)
							{
								char buf[20];
								ULONG c[2];
								
								CurrentTime(&(c[0]), &(c[1]));
								vsprintf(buf, "%10ld.%06ld\r\n", c);
								
								/* return timeval format (seconds.microseconds) */
								send(connfd, buf, strlen(buf), 0L);
								
								/* the server closes the client connection */
								CloseSocket(connfd);
							}
							else
							{
								Printf("Error code %ld while receiving request\n", connfd);
							}
						}/* for() */
					}
					else
					{
						Printf("Could not start server on port %ld\n", serviceport);
						rw = RETURN_FAIL; 
					}
				}
				else
				{
					Printf("Could not reserve port %ld\n", serviceport);
					rw = RETURN_FAIL;
				}
			}
			else
			{
				Printf("Error code %ld while opening network socket\n", listenfd);
				rw = RETURN_FAIL;
			}
			
			closeLib();
		}
		else
		{
			Printf(	"Error opening \"%s\" version %ld\n"
						"You must start AmiTCP 4.x, or a compatible TCP/IP stack\n",
					 	SOCKETNAME, 4L);
			
			rw = RETURN_FAIL;
		}
		
		FreeArgs(rdargs);
	}
	
	return(rw);
}

