/*Prototypes for if_loop.c ??? */

#ifndef _SYS_SOCKET_H_
#include <sys/socket.h>
#endif
#ifndef _NET_IF_H_
#include <net/if.h>
#endif

void loattach(void);
int looutput(struct ifnet *, 
	     register struct mbuf *, 
	     struct sockaddr *, 
	     register struct rtentry *);
void lortrequest(int, struct rtentry *, struct sockaddr *);
int loioctl(register struct ifnet *, int, caddr_t);
