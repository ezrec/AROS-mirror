/*
    Copyright © 2003-2004, The AROS Development Team. All rights reserved.
    $Id$
 */
 
/******************************************
 * Filename : send.c
 * Purpose  : accepts and transmits to a simple connection
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
  int addrlen;
  int new_socket;
  char *message="Hello, your connection has been accepted.\n\rBut it will be terminated in about 5 seconds\n\r";

#if defined(__AROS__)
  SocketBase = OpenLibrary("bsdsocket.library",0);
  if (!SocketBase) return NULL;
#endif
  
/* create the master socket and check it worked */
  if ((socket_desc=socket(AF_INET,SOCK_STREAM,0))==0)
  {
/* if socket failed then display error and exit */
    perror("Create socket");
    exit(EXIT_FAILURE);
  }

/* type of socket created */
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
/* 7000 is the port to use for connections */
  address.sin_port = htons(7000);
/* bind the socket to port 7000 */
  if (bind(socket_desc,(struct sockaddr *)&address,sizeof(address))<0)
  {
/* if bind failed then display error message and exit */
    perror("bind");
    exit(EXIT_FAILURE);
  }

/* try to specify maximum of 3 pending connections for the master socket */
  if (listen(socket_desc,3)<0)
  {
/* if listen failed then display error and exit */
    perror("listen");
    exit(EXIT_FAILURE);
  }

/* accept one connection, wait if no connection pending */
  addrlen=sizeof(address);
  if ((new_socket=accept(socket_desc,(struct sockaddr *)&address,&addrlen))<0)
  {
/* if accept failed to return a socket descriptor, display error and exit */
    perror("accept");
    exit(EXIT_FAILURE);
  }
/* inform user of socket number - used in send and receive commands */
  printf("New socket is %d\n",new_socket);

/* transmit message to new connection */
  if (send(new_socket,message,strlen(message),0)!=strlen(message))
  {
/* if send failed to send all the message, display error and exit */
    perror("send");
    exit(EXIT_FAILURE);
  }
  puts("Message sent successfully");
#if !defined(__AROS__)
sleep(5);
#endif

/* shutdown master socket properly */
  close(socket_desc);

#if defined(__AROS__)
  CloseLibrary(SocketBase);
#endif
}
