/*
 * daytime.c
 *
 * client for tcp daytime service
 *
 * adapted from "UNIX Network Programming, Vol.1, 2nd Edition" by W. Richard Stevens
 * for Amiga® OS with AmiTCP 4.x compatible network stacks
 *
 * should work with any compiler; link additional startup routine "startup.c" for Maxon C/C++ only
 */
#ifndef __AROS__
#include <pragma/exec_lib.h>
#include <pragma/dos_lib.h>
#include <pragma/socket_lib.h>
#else
#include <proto/dos.h>
#include <proto/exec.h>
#include <stdio.h>
#include <dos/dos.h>
#include <dos/rdargs.h>
#include <clib/dos_protos.h>
#include <defines/dos.h>
#include "aros.h"
#include <bsdsocket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#define AF_INET 2 /* we need to fix our includes */
#define SOCK_STREAM 1
#endif /* __AROS__ */
#ifndef SOCKETNAME
#define SOCKETNAME "bsdsocket.library"
#endif
/* #include <sys/socket.h> */
#include <string.h>


#define DAYTIMEPORT	13
#define LOCALHOSTIP	"127.0.0.1"


struct Library *SocketBase = NULL;


/* let's design our resource handling "re-usable" */
struct Library *openSockLib(void)
{
	return(OpenLibrary(SOCKETNAME, 4U));
}

void closeSockLib(struct Library **base)
{
	CloseLibrary(*base);
	*base = NULL;
}


LONG main(LONG argc, APTR argv)
{
	struct RDArgs *rdargs;
	STRPTR tmpl = "H=HOSTIP/K,P=PORT/K/N";
	LONG arg[2] = {0L, 0L};
	LONG rw = RETURN_OK;
	char *hostip = LOCALHOSTIP;
	int serviceport = DAYTIMEPORT;
	
	/* Workbench is not supported (this is a learning demo...) */
	if(argc)
	{
		/* convenient Amiga shell-parameter handling */
		rdargs = ReadArgs(tmpl, arg, NULL);
		if(rdargs)
		{
			hostip = arg[0] ? (char *) arg[0] : hostip;
			serviceport = arg[1] ? *((LONG *) arg[1]) : serviceport;
		}
		
		/* open bsdsocket.library */
		SocketBase = openSockLib();
		if(SocketBase)
		{
			long sockfd;
			
			/* open TCP access */
			sockfd = socket(AF_INET, SOCK_STREAM, 0L);
			if(sockfd >= 0L)
			{
				struct sockaddr_in servaddr;
				
				/* create TCP request */
				memset(&servaddr, 0L, sizeof(struct sockaddr_in));
				servaddr.sin_family			= AF_INET;
				servaddr.sin_port				= htons(serviceport);
				servaddr.sin_addr.s_addr	= inet_addr(hostip);	/* sin_addr == s_addr */
				if(servaddr.sin_addr.s_addr != (~0))
				{
					/* connect to service port on host [connect() has an internal CTRL_C handling] */
					if(connect(sockfd, (const struct sockaddr *) &servaddr, sizeof(struct sockaddr_in)) >= 0L)
					{
						char buffer[4];
						long rd;
						
						/* receive and print data (small buffer to force multiple recv()'s) */
						do
						{
							/* receive data */
							rd = recv(sockfd, buffer, 4, 0);
							if(rd)
							{
								/* buffered write data */
								FWrite(Output(), buffer, rd, 1);
								
								/* standard while()-loop emergency break */
								if(CheckSignal(SIGBREAKF_CTRL_C))
								{
									PutStr("^C -- user break\n");
									rw = RETURN_WARN;
									break;
								}
							}
						}
						while(rd > 0);
						
						/* empty buffers (not really required) */
						Flush(Output());
					}
					else
					{
						Printf("Could not connect to port %ld on \"%s\"\n", serviceport, hostip);
						rw = RETURN_ERROR;
					}
				}
				else
				{
					Printf("Not a valid network address: \"%s\"\n", hostip);
					rw = RETURN_ERROR;
				}
				
				/* release TCP access */
				CloseSocket(sockfd);
			}
			else
			{
				Printf("Error code %ld while opening network socket\n", sockfd);
				rw = RETURN_FAIL;
			}
			
			/* release bsdsocket.library */
			closeSockLib(&SocketBase);
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


