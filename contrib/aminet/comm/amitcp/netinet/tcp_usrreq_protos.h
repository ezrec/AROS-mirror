/* Prototypes for functions defined in
tcp_usrreq.c
 */

#ifndef SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifndef SYS_SOCKETVAR_H
#include <sys/socketvar.h>
#endif

int tcp_usrreq(struct socket * so,
               int req,
               struct mbuf * m,
               struct mbuf * nam,
               struct mbuf * control);

int tcp_ctloutput(int op,
                  struct socket * so,
                  int level,
                  int optname,
                  struct mbuf ** mp);

int tcp_attach(struct socket * so);

struct tcpcb * tcp_disconnect(register struct tcpcb * tp);

struct tcpcb * tcp_usrclosed(register struct tcpcb * tp);

