/* Prototypes for functions defined in
ip_output.c
 */

#ifndef NET_ROUTE_H
#include <net/route.h>
#endif

#ifndef SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifndef SYS_SOCKETVAR_H
#include <sys/socketvar.h>
#endif

int __stdargs ip_output(struct mbuf * m0,
			struct mbuf * opt,
			struct route * ro,
			int flags);

struct mbuf * ip_insertoptions(register struct mbuf * m,
                               struct mbuf * opt,
                               int * phlen);

int ip_optcopy(struct ip * ip,
               struct ip * jp);

int ip_ctloutput(int op,
                 struct socket * so,
                 int level,
                 int optname,
                 struct mbuf ** mp);

int ip_pcbopts(struct mbuf ** pcbopt,
               register struct mbuf * m);

