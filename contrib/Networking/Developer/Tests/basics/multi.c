/*
    Copyright © 2003-2004, The AROS Development Team. All rights reserved.
    $Id$
 */

/******************************************
 * Filename : multi.c
 * Purpose  : handles data from up to three sockets and relays to others
 *            could easily be expanded
 * Author   : Simon Amor (simon@foobar.co.uk)
 * 
 * Warning  : This code is not fully functional - it's also not my usual
 *            coding style. Converting this to a set of functions is left
 *            as an exercise to the reader ;-)
 ***/

/*
 * WARNING: THIS CODE IS ONLY PARTIALLY FUNCTIONAL
 */

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
#else

#define TRUE   1
#define FALSE  0

#endif

/* network functions */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
/* FD_SET, FD_ISSET, FD_ZERO macros */
#include <sys/time.h>

#if defined(__AROS__)
struct Library *SocketBase;
#endif

int main()
{
  int opt=TRUE;
  int master_socket;
  struct sockaddr_in address;
  int addrlen;

  int new_socket;

  int client_socket[3];
  int max_clients=3;

  int activity, loop, loop2, valread;
  char buffer[1025];    /* data buffer of 1K */

  fd_set readfds;

#if defined(__AROS__)
  SocketBase = OpenLibrary("bsdsocket.library",0);
  if (!SocketBase) return NULL;
#endif
  
  char *message="Data-relay v0.1 (C)1996 Simon Amor <simon@foobar.co.uk>\n\r";

/* initialise all client_socket[] to 0 so not checked */
  for (loop=0; loop < max_clients; loop++) {
    client_socket[loop] = 0;
  }

/* create the master socket and check it worked */
  if ((master_socket = socket(AF_INET,SOCK_STREAM,0))==0) {
/* if socket failed then display error and exit */
    perror("Create master_socket");
    exit(EXIT_FAILURE);
  }

/* set master socket to allow multiple connections */
  if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, 
                 (char *)&opt, sizeof(opt))<0) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

/* type of socket created */
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
/* 7000 is the port to use for connections */
  address.sin_port = htons(7000);
/* bind the socket to port 7000 */
  if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) {
/* if bind failed then display error message and exit */
    perror("bind");
    exit(EXIT_FAILURE);
  }

/* try to specify maximum of 3 pending connections for the master socket */
  if (listen(master_socket, 3)<0) {
/* if listen failed then display error and exit */
    perror("listen");
    exit(EXIT_FAILURE);
  }

  while (1==1) {
    FD_ZERO(&readfds);
/* reason we say max_clients+3 is stdin,stdout,stderr take up the first 
 * couple of descriptors and we might as well allow a couple of extra.
 * If your program opens files at all you will want to allow enough extra.
 * Another option is to specify the maximum your operating system allows.
 */

/* setup which sockets to listen on */
    FD_SET(master_socket, &readfds);
    for (loop=0; loop<max_clients; loop++) {
      if (client_socket[loop] > 0) {
        FD_SET(client_socket[loop], &readfds);
      }
    }

/* wait for connection, forever if we have to */
    activity=select(max_clients+3, &readfds, NULL, NULL, NULL);
  
    if ((activity < 0) && (errno!=EINTR)) {
/* there was an error with select() */
    }
    if (FD_ISSET(master_socket, &readfds)) {
/* Open the new socket as 'new_socket' */
      addrlen=sizeof(address);
      if ((new_socket = accept(master_socket, (struct sockaddr *)&address, &addrlen))<0)
      {
/* if accept failed to return a socket descriptor, display error and exit */
        perror("accept");
        exit(EXIT_FAILURE);
      }
/* inform user of socket number - used in send and receive commands */
      printf("New socket is fd %d\n",new_socket);
  
/* transmit message to new connection */
      if (send(new_socket, message, strlen(message), 0) != strlen(message)) {
/* if send failed to send all the message, display error and exit */
        perror("send");
      }
      puts("Welcome message sent successfully");
/* add new socket to list of sockets */
      for (loop=0; loop<max_clients; loop++) {
        if (client_socket[loop] == 0) {
          client_socket[loop] = new_socket;
          printf("Adding to list of sockets as %d\n", loop);
          loop = max_clients;
        }
      }
    }
    for (loop=0; loop<max_clients; loop++) {
      if (FD_ISSET(client_socket[loop], &readfds)) {
        if ((valread = read(client_socket[loop], buffer, 1024)) < 0) {
          close(client_socket[loop]);
          client_socket[loop] = 0;
        } else {
/* set the terminating NULL byte on the end of the data read */
          buffer[valread] = 0;
          for (loop2=0; loop2<max_clients; loop2++) {
/* note, flags for send() are normally 0, see man page for details */
            send(client_socket[loop2], buffer, strlen(buffer), 0);
          }
        }
/* 
 * use read() to read from the socket into a buffer using something
 * similar to the following:
 *
 * If the read didn't cause an error then send buffer to all 
 * client sockets in the array - use for loop and send() just
 * as if you were sending it to one connection
 *
 * important point to note is that if the connection is char-by-char
 * the person will not be able to send a complete string and you will
 * need to use buffers for each connection, checking for overflows and
 * new line characters (\n or \r)
 */
      }
    }
  }

/* normally you should close the sockets here before program exits */
/* however, in this example the while() loop prevents getting here */
#if defined(__AROS__)
  CloseLibrary(SocketBase);
#endif
}
