/* Prototypes for functions defined in
udp_usrreq.c
 */

#ifndef NETINET_IN_PCB_H
#include <netinet/in_pcb.h>
#endif

#ifndef SYS_SOCKET_H
#include <sys/socket.h>
#endif

void udp_init(void);

void __stdargs udp_input(register struct mbuf * m,
			 int iphlen);

struct mbuf * udp_saveopt(caddr_t p,
                          register int size,
                          int type);

void udp_notify(register struct inpcb * inp,
		int errno);

void udp_ctlinput(int cmd,
                 struct sockaddr * sa,
                 register struct ip * ip);

int udp_output(register struct inpcb * inp,
               register struct mbuf * m,
               struct mbuf * addr,
               struct mbuf * control);

int udp_usrreq(struct socket * so,
               int req,
               struct mbuf * m,
               struct mbuf * addr,
               struct mbuf * control);

void udp_detach(struct inpcb * inp);


