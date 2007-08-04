/* $Id: stubs.c,v 1.1.1.2 2005/12/07 10:50:34 sonic_amiga Exp $
 *
 *      stubs.c - common stubs for bsdsocket.library
 *
 *      Copyright © 1994 AmiTCP/IP Group, 
 *                       Network Solutions Development Inc.
 *                       All rights reserved.
 */

#include <exec/types.h>
#include <sys/types.h>
#include <proto/socket.h>

#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <netinet/in_systm.h>
#include <net/if.h>
#include <net/route.h>
#include <netinet/in.h>
#include <netinet/in_var.h>

#include <netinet/ip.h>

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
