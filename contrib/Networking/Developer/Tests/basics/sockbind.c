/*
    Copyright © 2003-2004, The AROS Development Team. All rights reserved.
    $Id$
 */
 
 /******************************************
 * Filename : sockbind.c
 * Purpose  : demonstrates socket() and bind()
 * Author   : Simon Amor (simon@foobar.co.uk)
 ***/

#include <stdio.h>

/* for EXIT_FAILURE and EXIT_SUCCESS */
#include <stdlib.h>

#if defined(__AROS__)
#include <proto/exec.h>
#include <proto/socket.h>
#include <errno.h>
#include <string.h>
#warning TODO: Should be in an include!!
#define close   CloseSocket
#define ioctl   IoctlSocket
#define select(a,b,c,d,e) WaitSelect(a,b,c,d,e,NULL)
#endif

/* network functions */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#if defined(__AROS__)
struct Library *SocketBase;
#endif

int main()
{
  int socket_desc;
  struct sockaddr_in address;

#if defined(__AROS__)
  SocketBase = OpenLibrary("bsdsocket.library",0);
  if (!SocketBase) return NULL;
#endif
  
/* create the master socket and check it worked ok */
  socket_desc=socket(AF_INET,SOCK_STREAM,0);
/* if there was an error creating the socket */
/* then display error and exit program */
  if (socket_desc==0)
  {
    perror("Create socket");
    exit(EXIT_FAILURE);
  }

/* type of socket created */
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
/* 7000 is the port to use for connections */
  address.sin_port = htons(7000);
/* bind the socket to port 7000 */
  bind(socket_desc,(struct sockaddr *)&address,sizeof(address));

#if !defined(__AROS__)
sleep(10);
#endif

/* shutdown master socket properly */
  close(socket_desc);
  
#if defined(__AROS__)
  CloseLibrary(SocketBase);
#endif
}
