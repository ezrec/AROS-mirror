/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved. 
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. Neither the name of the Institute nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: sockets.c,v 1.2 2001/12/13 08:32:17 adam Exp $
 */

#include "lwip/debug.h"
#include "lwip/api.h"

#include "lwip/sockets.h"

#define NUM_SOCKETS 10

static struct netconn *sockets[NUM_SOCKETS];

/*-----------------------------------------------------------------------------------*/
int
lwip_accept(int s, struct sockaddr *addr, int *addrlen)
{
  struct netconn *conn, *newconn;
  struct ip_addr *naddr;
  u16_t port;
  u16_t i;

  if(s > NUM_SOCKETS) {
    /* errno = EBADF; */
    return -1;
  }
  
  conn = sockets[s];

  if(conn == NULL) {
    /* errno = EBADF; */
    return -1;
  }
  
  newconn = netconn_accept(conn);
    
  /* get the IP address and port of the remote host */
  netconn_peer(conn, &naddr, &port);
  
  ((struct sockaddr_in *)addr)->sin_addr.s_addr = naddr->addr;
  ((struct sockaddr_in *)addr)->sin_port = port;

  /* allocate a new socket identifier */
  for(i = 0; i < NUM_SOCKETS; ++i) {
    if(sockets[i] == NULL) {
      sockets[i] = newconn;
      return i;
    }
  }
  netconn_delete(newconn);
  /* errno = ENOBUFS; */
  return -1;
}
/*-----------------------------------------------------------------------------------*/
int
lwip_bind(int s, struct sockaddr *name, int namelen)
{
  struct netconn *conn;
  struct ip_addr remote_addr;
  u16_t remote_port;
  err_t err;
  
  if(s > NUM_SOCKETS) {
    /* errno = EBADF; */
    return -1;
  }
  
  conn = sockets[s];

  if(conn == NULL) {
    /* errno = EBADF; */
    return -1;
  }
  

  remote_addr.addr = ((struct sockaddr_in *)name)->sin_addr.s_addr;
  remote_port = ((struct sockaddr_in *)name)->sin_port;
  
  err = netconn_bind(conn, &remote_addr, ntohs(remote_port));

  if(err != ERR_OK) {
    /* errno = ... */
    return -1;
  }

  return 0;
}
/*-----------------------------------------------------------------------------------*/
int
lwip_close(int s)
{
  struct netconn *conn;

  DEBUGF(SOCKETS_DEBUG, ("close: socket %d\n", s));
  
  if(s > NUM_SOCKETS) {
    /* errno = EBADF; */
    return -1;
  }
  
  conn = sockets[s];

  if(conn == NULL) {
    /* errno = EBADF; */
    return -1;
  }
  sockets[s] = NULL;
  netconn_delete(conn);
  return 0;
}
/*-----------------------------------------------------------------------------------*/
int
lwip_connect(int s, struct sockaddr *name, int namelen)
{
  struct netconn *conn;
  struct ip_addr remote_addr;
  u16_t remote_port;
  err_t err;
  
  if(s > NUM_SOCKETS) {
    /* errno = EBADF; */
    return -1;
  }
  
  conn = sockets[s];

  if(conn == NULL) {
    /* errno = EBADF; */
    return -1;
  }
  
  remote_addr.addr = ((struct sockaddr_in *)name)->sin_addr.s_addr;
  remote_port = ((struct sockaddr_in *)name)->sin_port;
  
  err = netconn_connect(conn, &remote_addr, ntohs(remote_port));

  if(err != ERR_OK) {
    /* errno = ... */
    return -1;
  }

  return 0;
}
/*-----------------------------------------------------------------------------------*/
int
lwip_listen(int s, int backlog)
{
  struct netconn *conn;
  err_t err;
  
  if(s > NUM_SOCKETS) {
    /* errno = EBADF; */
    return -1;
  }
  
  conn = sockets[s];

  if(conn == NULL) {
    /* errno = EBADF; */
    return -1;
  }
 
  err = netconn_listen(conn);

  if(err != ERR_OK) {
    /* errno = ... */
    return -1;
  }

  return 0;
}
/*-----------------------------------------------------------------------------------*/
int
lwip_recv(int s, void *mem, int len, unsigned int flags)
{
  struct netconn *conn;
  struct netbuf *buf;
  u16_t buflen, copylen;
  
  
  if(s > NUM_SOCKETS) {
    /* errno = EBADF; */
    return -1;
  }
  
  conn = sockets[s];

  if(conn == NULL) {
    /* errno = EBADF; */
    return -1;
  }
  
  buf = netconn_recv(conn);

  if(buf == NULL) {
    return 0;
  }
  
  buflen = netbuf_len(buf);

  if(len > buflen) {
    copylen = buflen;
  } else {
    copylen = len;
  }
    
  
  /* copy the contents of the received buffer into
     the supplied memory pointer mem */
  netbuf_copy(buf, mem, copylen);
  netbuf_delete(buf);

  /* if the length of the received data is larger than
     len, this data is discarded and we return len.
     otherwise we return the actual length of the received
     data */
  if(len > buflen) {
    return buflen;
  } else {
    return len;
  }
}
/*-----------------------------------------------------------------------------------*/
int
lwip_read(int s, void *mem, int len)
{
  return lwip_recv(s, mem, len, 0);
}
/*-----------------------------------------------------------------------------------*/
int
lwip_recvfrom(int s, void *mem, int len, unsigned int flags,
	 struct sockaddr *from, int *fromlen)
{
  struct netconn *conn;
  struct netbuf *buf;
  struct ip_addr *addr;
  u16_t port;
  u16_t buflen;
  
  if(s > NUM_SOCKETS) {
    /* errno = EBADF; */
    return -1;
  }
  
  conn = sockets[s];

  if(conn == NULL) {
    /* errno = EBADF; */
    return -1;
  }
  
  buf = netconn_recv(conn);
  
  if(buf == NULL) {
    return 0;
  }
  
  buflen = netbuf_len(buf);
    
  /* copy the contents of the received buffer into
     the supplied memory pointer */
  netbuf_copy(buf, mem, len);

  addr = netbuf_fromaddr(buf);
  port = htons(netbuf_fromport(buf));
  ((struct sockaddr_in *)from)->sin_addr.s_addr = addr->addr;
  ((struct sockaddr_in *)from)->sin_port = port;
  *fromlen = sizeof(struct sockaddr_in);
  netbuf_delete(buf);

  /* if the length of the received data is larger than
     len, this data is discarded and we return len.
     otherwise we return the actual length of the received
     data */
  if(len > buflen) {
    return buflen;
  } else {
    return len;
  }
}
/*-----------------------------------------------------------------------------------*/
int
lwip_send(int s, void *data, int size, unsigned int flags)
{
  struct netconn *conn;
  struct netbuf *buf;
  err_t err;

  DEBUGF(SOCKETS_DEBUG, ("send: socket %d, size %d\n", s, size));
    
  if(s > NUM_SOCKETS) {
    /* errno = EBADF; */
    return -1;
  }
  
  conn = sockets[s];

  if(conn == NULL) {
    /* errno = EBADF; */
    return -1;
  }
  
  switch(netconn_type(conn)) {
  case NETCONN_UDP:
    /* create a buffer */
    buf = netbuf_new();

    if(buf == NULL) {
      /* errno = ENOBUFS; */
      return -1;
    }
    
    /* make the buffer point to the data that should
       be sent */
    netbuf_ref(buf, data, size);

    /* send the data */
    err = netconn_send(conn, buf);

    /* deallocated the buffer */
    netbuf_delete(buf);
    break;
  case NETCONN_TCP:
    err = netconn_write(conn, data, size, NETCONN_COPY);
    break;
  default:
    err = ERR_ARG;
    break;
  }
  if(err != ERR_OK) {
    /* errno = ... */
    return -1;    
  }
    
  return size;
}
/*-----------------------------------------------------------------------------------*/
int
lwip_sendto(int s, void *data, int size, unsigned int flags,
       struct sockaddr *to, int tolen)
{
  struct netconn *conn;
  struct ip_addr remote_addr, *addr;
  u16_t remote_port, port;
  int ret;

  if(s > NUM_SOCKETS) {
    /* errno = EBADF; */
    return -1;
  }
  
  conn = sockets[s];

  if(conn == NULL) {
    /* errno = EBADF; */
    return -1;
  }

  /* get the peer if currently connected */
  netconn_peer(conn, &addr, &port);
  
  remote_addr.addr = ((struct sockaddr_in *)to)->sin_addr.s_addr;
  remote_port = ((struct sockaddr_in *)to)->sin_port;
  netconn_connect(conn, &remote_addr, remote_port);
  
  ret = lwip_send(s, data, size, flags);

  /* reset the remote address and port number
     of the connection */
  netconn_connect(conn, addr, port);
  return ret;
}
/*-----------------------------------------------------------------------------------*/
int
lwip_socket(int domain, int type, int protocol)
{
  struct netconn *conn;
  int i;

  /* create a netconn */
  switch(type) {
  case SOCK_DGRAM:
    conn = netconn_new(NETCONN_UDP);
    break;
  case SOCK_STREAM:
    conn = netconn_new(NETCONN_TCP);
    break;
  default:
    /* errno = ... */
    return -1;
  }

  if(conn == NULL) {
    DEBUGF(SOCKETS_DEBUG, ("socket: could not create netconn.\n"));
    /* errno = ENOBUFS; */
    return -1;
  }
  
  /* find an empty place in the sockets[] list */
  for(i = 0; i < NUM_SOCKETS; ++i) {
    if(sockets[i] == NULL) {
      sockets[i] = conn;
      DEBUGF(SOCKETS_DEBUG, ("socket: allocated numner %d\n", i));
      return i;
    }
  }
  netconn_delete(conn);
  /* errno = ENOBUFS; */
  return -1;
}
/*-----------------------------------------------------------------------------------*/
int
lwip_write(int s, void *data, int size)
{
  struct netconn *conn;
  err_t err;

  DEBUGF(SOCKETS_DEBUG, ("write: socket %d, size %d\n", s, size));
  
  if(s > NUM_SOCKETS) {
    /* errno = EBADF; */
    return -1;
  }
  
  conn = sockets[s];

  if(conn == NULL) {
    /* errno = EBADF; */
    return -1;
  }
  
  switch(netconn_type(conn)) {
  case NETCONN_UDP:
    return lwip_send(s, data, size, 0);

  case NETCONN_TCP:
    err = netconn_write(conn, data, size, NETCONN_COPY);
    break;
  default:
    err = ERR_ARG;
    break;
  }
  if(err != ERR_OK) {
    /* errno = ... */
    return -1;
  }
  return size;
}
/*-----------------------------------------------------------------------------------*/
