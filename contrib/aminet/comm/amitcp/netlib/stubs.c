/*
 * stubs.c -- common stubs for bsdsocket.library
 *
 * Author: ppessi <Pekka.Pessi@hut.fi>
 *
 * Copyright © 1993 AmiTCP/IP Group, <AmiTCP-group@hut.fi>
 *                  Helsinki University of Technology, Finland.
 *
 * Created      : Thu Feb  3 11:40:25 1994 ppessi
 * Last modified: Thu Feb  3 18:27:30 1994 ppessi
 */

#include <netinet/in.h>

#undef _OPTINLINE
#include <proto/socket.h>

int 
select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exeptfds,
	 struct timeval *timeout)
{
  /* call WaitSelect with NULL signal mask pointer */
  return WaitSelect(nfds, readfds, writefds, exeptfds, timeout, NULL);
}

char * 
inet_ntoa(struct in_addr addr) 
{
  return Inet_NtoA(addr.s_addr);
}

struct in_addr 
inet_makeaddr(int net, int host)
{
  struct in_addr addr;
  addr.s_addr = Inet_MakeAddr(net, host);
  return addr;
}

unsigned long 
inet_lnaof(struct in_addr addr) 
{
  return Inet_LnaOf(addr.s_addr);
}

unsigned long   
inet_netof(struct in_addr addr)
{
  return Inet_NetOf(addr.s_addr);
}
