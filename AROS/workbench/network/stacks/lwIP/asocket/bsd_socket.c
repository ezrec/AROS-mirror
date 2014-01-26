/**
 * Sockets BSD-Like API module
 *
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * Improved by Marc Boucher <marc@mbsi.ca> and David Haas <dhaas@alum.rpi.edu>
 *
 * Adapted for use in the AROS lwIP based bsdsockets.library, these portions
 * Copyright (c) 2014, The AROS Development Team
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 */

#define DEBUG 1

#include <string.h>
#include <sys/uio.h>

#include <aros/debug.h>
#include <proto/exec.h>
#include <libraries/bsdsocket.h>
#include <devices/sana2.h>

#include <lwip/opt.h>
#include <lwip/init.h>
#include <lwip/sockets.h>
#include <lwip/inet.h>
#include <lwip/ip4_addr.h>

#include "lwip/sockets.h"
#include "lwip/api.h"
#include "lwip/sys.h"
#include "lwip/igmp.h"
#include "lwip/inet.h"
#include "lwip/tcp.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/tcpip.h"
#include "lwip/pbuf.h"
#if LWIP_CHECKSUM_ON_COPY
#include "lwip/inet_chksum.h"
#endif

#include "asocket_intern.h"

err_t etharp_output(struct netif *netif, struct pbuf *q, ip_addr_t *ipaddr);

#define BSD_ASSERT(msg, iftrue) do { \
    if (iftrue) { \
        D(bug("bsd[%p] %s\n", bsd, msg)); \
    } } while (0)


#define IP4ADDR_PORT_TO_SOCKADDR(sin, ipXaddr, port) do { \
      (sin)->sin_len = sizeof(struct sockaddr_in); \
      (sin)->sin_family = AF_INET; \
      (sin)->sin_port = htons((port)); \
      inet_addr_from_ipaddr(&(sin)->sin_addr, ipX_2_ip(ipXaddr)); \
      memset((sin)->sin_zero, 0, SIN_ZERO_LEN); }while(0)
#define SOCKADDR4_TO_IP4ADDR_PORT(sin, ipXaddr, port) do { \
    inet_addr_to_ipaddr(ipX_2_ip(ipXaddr), &((sin)->sin_addr)); \
    (port) = ntohs((sin)->sin_port); }while(0)

#if LWIP_IPV6
#define IS_SOCK_ADDR_LEN_VALID(namelen)  (((namelen) == sizeof(struct sockaddr_in)) || \
                                         ((namelen) == sizeof(struct sockaddr_in6)))
#define IS_SOCK_ADDR_TYPE_VALID(name)    (((name)->sa_family == AF_INET) || \
                                         ((name)->sa_family == AF_INET6))
#define SOCK_ADDR_TYPE_MATCH(name, sock) \
       ((((name)->sa_family == AF_INET) && !(NETCONNTYPE_ISIPV6((sock)->conn->type))) || \
       (((name)->sa_family == AF_INET6) && (NETCONNTYPE_ISIPV6((sock)->conn->type))))
#define IP6ADDR_PORT_TO_SOCKADDR(sin6, ipXaddr, port) do { \
      (sin6)->sin6_len = sizeof(struct sockaddr_in6); \
      (sin6)->sin6_family = AF_INET6; \
      (sin6)->sin6_port = htons((port)); \
      (sin6)->sin6_flowinfo = 0; \
      inet6_addr_from_ip6addr(&(sin6)->sin6_addr, ipX_2_ip6(ipXaddr)); }while(0)
#define IPXADDR_PORT_TO_SOCKADDR(isipv6, sockaddr, ipXaddr, port) do { \
    if (isipv6) { \
      IP6ADDR_PORT_TO_SOCKADDR((struct sockaddr_in6*)(void*)(sockaddr), ipXaddr, port); \
    } else { \
      IP4ADDR_PORT_TO_SOCKADDR((struct sockaddr_in*)(void*)(sockaddr), ipXaddr, port); \
    } } while(0)
#define SOCKADDR6_TO_IP6ADDR_PORT(sin6, ipXaddr, port) do { \
    inet6_addr_to_ip6addr(ipX_2_ip6(ipXaddr), &((sin6)->sin6_addr)); \
    (port) = ntohs((sin6)->sin6_port); }while(0)
#define SOCKADDR_TO_IPXADDR_PORT(isipv6, sockaddr, ipXaddr, port) do { \
    if (isipv6) { \
      SOCKADDR6_TO_IP6ADDR_PORT((struct sockaddr_in6*)(void*)(sockaddr), ipXaddr, port); \
    } else { \
      SOCKADDR4_TO_IP4ADDR_PORT((struct sockaddr_in*)(void*)(sockaddr), ipXaddr, port); \
    } } while(0)
#define DOMAIN_TO_NETCONN_TYPE(domain, type) (((domain) == AF_INET) ? \
  (type) : (enum netconn_type)((type) | NETCONN_TYPE_IPV6))
#else /* LWIP_IPV6 */
#define IS_SOCK_ADDR_LEN_VALID(namelen)  ((namelen) == sizeof(struct sockaddr_in))
#define IS_SOCK_ADDR_TYPE_VALID(name)    ((name)->sa_family == AF_INET)
#define SOCK_ADDR_TYPE_MATCH(name, sock) 1
#define IPXADDR_PORT_TO_SOCKADDR(isipv6, sockaddr, ipXaddr, port) \
        IP4ADDR_PORT_TO_SOCKADDR((struct sockaddr_in*)(void*)(sockaddr), ipXaddr, port)
#define SOCKADDR_TO_IPXADDR_PORT(isipv6, sockaddr, ipXaddr, port) \
      SOCKADDR4_TO_IP4ADDR_PORT((struct sockaddr_in*)(void*)(sockaddr), ipXaddr, port)
#define DOMAIN_TO_NETCONN_TYPE(domain, netconn_type) (netconn_type)
#endif /* LWIP_IPV6 */

#define IS_SOCK_ADDR_TYPE_VALID_OR_UNSPEC(name)    (((name)->sa_family == AF_UNSPEC) || \
                                                    IS_SOCK_ADDR_TYPE_VALID(name))
#define SOCK_ADDR_TYPE_MATCH_OR_UNSPEC(name, sock) (((name)->sa_family == AF_UNSPEC) || \
                                                    SOCK_ADDR_TYPE_MATCH(name, sock))
#define IS_SOCK_ADDR_ALIGNED(name)      ((((mem_ptr_t)(name)) % 4) == 0)

/** Contains all internal pointers and states used for a socket */
struct bsd_sock {
  /** usage count **/
  int usage; 
  /** sockets currently are built on netconns, each socket has one netconn */
  struct netconn *conn;
  /** data that was left from the previous read */
  void *lastdata;
  /** offset in the data that was left from the previous read */
  u16_t lastoffset;
  /** number of times data was received, set by event_callback(),
      tested by the receive and select functions */
  s16_t rcvevent;
  /** number of times data was ACKed (free send buffer), set by event_callback(),
      tested by select */
  u16_t sendevent;
  /** error happened for this socket, set by event_callback(), tested by select */
  u16_t errevent; 
  /** counter of how many threads are waiting for this socket using select */
  int select_waiting;
  /** pointer back to the bsd global base **/
  struct bsd *bsd;
};

/** Description for a task waiting in select */
struct bsd_select_cb {
  /** Pointer to the next waiting task */
  struct bsd_select_cb *next;
  /** Pointer to the previous waiting task */
  struct bsd_select_cb *prev;
  /** readset passed to select */
  fd_set *readset;
  /** writeset passed to select */
  fd_set *writeset;
  /** unimplemented: exceptset passed to select */
  fd_set *exceptset;
  /** don't signal the same semaphore twice: set to 1 when signalled */
  int sem_signalled;
  /** semaphore to wake up a task waiting for select */
  sys_sem_t sem;
};

/** This struct is used to pass data to the set/getsockopt_internal
 * functions running in tcpip_thread context (only a void* is allowed) */
struct bsd_setgetsockopt_data {
  struct bsd *bsd;
  /** socket struct for which to change options */
  struct bsd_sock *sock;
  /** level of the option to process */
  int level;
  /** name of the option to process */
  int optname;
  /** set: value to set the option to
    * get: value of the option is stored here */
  void *optval;
  /** size of *optval */
  socklen_t *optlen;
  /** if an error occures, it is temporarily stored here */
  err_t err;
};

/** A struct sockaddr replacement that has the same alignment as sockaddr_in/
 *  sockaddr_in6 if instantiated.
 */
union sockaddr_aligned {
   struct sockaddr sa;
#if LWIP_IPV6
   struct sockaddr_in6 sin6;
#endif /* LWIP_IPV6 */
   struct sockaddr_in sin;
};

struct bsd_netif {
    struct MinNode bn_node;
    struct netif   bn_netif;
    char           bn_name[IFNAMSIZ+1];
    struct IOSana2Req bn_IOSana2Req;
    struct Task    *bn_task;
    struct MsgPort *bn_task_port;
};

struct bsd {
    struct SignalSemaphore bs_lock;
    struct MinList bs_netif_list;

    struct {
        /** The list of tasks waiting for select */
        struct bsd_select_cb *list;
        /** This counter is increased from lwip_select when the list is chagned
            and checked in event_callback to see if it has changed. */
        volatile int ctr;
    } bs_select_cb;
};

static void bsd_tcpip_done(void *ptr)
{
    Signal((struct Task *)ptr, SIGF_SINGLE);
}

struct bsd *bsd_init(void)
{
    struct bsd *bsd;

    if ((bsd = AllocVec(sizeof(*bsd), MEMF_ANY | MEMF_CLEAR))) {
        InitSemaphore(&bsd->bs_lock);

        NEWLIST(&bsd->bs_netif_list);

        /* Start TCP/IP from lwip */
        tcpip_init(bsd_tcpip_done, FindTask(NULL));
        Wait(SIGF_SINGLE);

        return bsd;
    }

    return NULL;
}

/* THIS CANNOT FAIL!
 * It may stall, or spawn a Task to finish cleaning up,
 * but it cannot fail.
 */
void bsd_expunge(struct bsd *bsd)
{
    FreeVec(bsd);
}

/** Table to quickly map an lwIP error (err_t) to a socket error
  * by using -err as an index */
static const int err_to_errno_table[] = {
  0,             /* ERR_OK          0      No error, everything OK. */
  ENOMEM,        /* ERR_MEM        -1      Out of memory error.     */
  ENOBUFS,       /* ERR_BUF        -2      Buffer error.            */
  EWOULDBLOCK,   /* ERR_TIMEOUT    -3      Timeout                  */
  EHOSTUNREACH,  /* ERR_RTE        -4      Routing problem.         */
  EINPROGRESS,   /* ERR_INPROGRESS -5      Operation in progress    */
  EINVAL,        /* ERR_VAL        -6      Illegal value.           */
  EWOULDBLOCK,   /* ERR_WOULDBLOCK -7      Operation would block.   */
  EADDRINUSE,    /* ERR_USE        -8      Address in use.          */
  EALREADY,      /* ERR_ISCONN     -9      Already connected.       */
  ECONNABORTED,  /* ERR_ABRT       -10     Connection aborted.      */
  ECONNRESET,    /* ERR_RST        -11     Connection reset.        */
  ENOTCONN,      /* ERR_CLSD       -12     Connection closed.       */
  ENOTCONN,      /* ERR_CONN       -13     Not connected.           */
  EIO,           /* ERR_ARG        -14     Illegal argument.        */
  -1,            /* ERR_IF         -15     Low-level netif error    */
};

#define ERR_TO_ERRNO_TABLE_SIZE \
  (sizeof(err_to_errno_table)/sizeof(err_to_errno_table[0]))

#define err_to_errno(err) \
  ((unsigned)(-(err)) < ERR_TO_ERRNO_TABLE_SIZE ? \
    err_to_errno_table[-(err)] : EIO)

/* Forward delcaration of some functions */
static void event_callback(struct netconn *conn, enum netconn_evt evt, u16_t len);
static void bsd_getsockopt_internal(void *arg);
static void bsd_setsockopt_internal(void *arg);

/**
 * Allocate a new socket for a given netconn.
 *
 * @param newconn the netconn for which to allocate a socket
 * @param accepted 1 if socket has been created by accept(),
 *                 0 if socket has been created by socket()
 * @return the 0 on success, or errno.
 */
static int alloc_socket(struct bsd *bsd, struct bsd_sock **new_sockp, struct netconn *newconn, int accepted)
{
  struct bsd_sock *sock;

  sock = AllocVec(sizeof(*sock), MEMF_ANY | MEMF_CLEAR);
  if (sock == NULL) {
      return ENOMEM;
  }

  sock->lastdata   = NULL;
  sock->lastoffset = 0;
  sock->rcvevent   = 0;
  /* TCP sendbuf is empty, but the socket is not yet writable until connected
   * (unless it has been created by accept()). */
  sock->sendevent  = (NETCONNTYPE_GROUP(newconn->type) == NETCONN_TCP ? (accepted != 0) : 1);
  sock->errevent   = 0;
  sock->select_waiting = 0;
  sock->usage = 1;
  sock->bsd = bsd;
  sock->conn       = newconn;
  newconn->callback_priv = sock;

  *new_sockp = sock;
  return 0;
}

/* Below this, the well-known socket functions are implemented.
 * Use google.com or opengroup.org to get a good description :-)
 *
 * Exceptions are documented!
 */
int bsd_accept(struct bsd *bsd, struct bsd_sock *sock, struct bsd_sock **new_sockp)
{
  struct bsd_sock *nsock;
  struct netconn *newconn;
  err_t err;
  SYS_ARCH_DECL_PROTECT(lev);

  D(bug("bsd_accept(%p)...\n", sock));

  if (netconn_is_nonblocking(sock->conn) && (sock->rcvevent <= 0)) {
    D(bug("bsd_accept(%p): returning EWOULDBLOCK\n", sock));
    return EWOULDBLOCK;
  }

  /* wait for a new connection */
  err = netconn_accept(sock->conn, &newconn);
  if (err != ERR_OK) {
    D(bug("bsd_accept(%p): netconn_acept failed, err=%d\n", sock, err));
    if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_TCP) {
      return EOPNOTSUPP;
    }
    return err_to_errno(err);
  }
  BSD_ASSERT("newconn != NULL", newconn != NULL);
  /* Prevent automatic window updates, we do this on our own! */
  netconn_set_noautorecved(newconn, 1);

  err = alloc_socket(bsd, &nsock, newconn, 1);
  if (err) {
    netconn_delete(newconn);
    return err;
  }
  BSD_ASSERT("newconn->callback == event_callback", newconn->callback == event_callback);

  /* See event_callback: If data comes in right away after an accept, even
   * though the server task might not have created a new socket yet.
   * In that case, newconn->socket is counted down (newconn->socket--),
   * so nsock->rcvevent is >= 1 here!
   */
  SYS_ARCH_PROTECT(lev);
  nsock->rcvevent += (s16_t)(-1 - newconn->socket);
  newconn->socket = 0;
  SYS_ARCH_UNPROTECT(lev);

  D(bug("bsd_accept(%p) returning new sock=%p", sock, nsock));

  *new_sockp = nsock;
  return 0;
}

int bsd_bind(struct bsd *bsd, struct bsd_sock *sock, const struct sockaddr *name, socklen_t namelen)
{
  ipX_addr_t local_addr;
  u16_t local_port;
  err_t err;

  if (!SOCK_ADDR_TYPE_MATCH(name, sock)) {
    /* sockaddr does not match socket type (IPv4/IPv6) */
    return err_to_errno(ERR_VAL);
  }

  /* check size, familiy and alignment of 'name' */
  LWIP_ERROR("bsd_bind: invalid address", (IS_SOCK_ADDR_LEN_VALID(namelen) &&
             IS_SOCK_ADDR_TYPE_VALID(name) && IS_SOCK_ADDR_ALIGNED(name)),
             return err_to_errno(ERR_ARG););
  LWIP_UNUSED_ARG(namelen);

  SOCKADDR_TO_IPXADDR_PORT((name->sa_family == AF_INET6), name, &local_addr, local_port);
  D(bug("bsd_bind(%p, addr=", sock));
  ipX_addr_debug_print(name->sa_family == AF_INET6, SOCKETS_DEBUG, &local_addr);
  D(bug(" port=%"U16_F")\n", local_port));

  err = netconn_bind(sock->conn, ipX_2_ip(&local_addr), local_port);

  if (err != ERR_OK) {
    D(bug("bsd_bind(%p) failed, err=%d\n", sock, err));
    return err_to_errno(err);
  }

  D(bug("bsd_bind(%p) succeeded\n", sock));
  return 0;
}


int bsd_close(struct bsd *bsd, struct bsd_sock *sock)
{
  int is_tcp = 0;
  void *lastdata;

  D(bug("bsd_close(%p)\n", sock));

  if (--sock->usage > 0)
      return 0;

  is_tcp = NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP;

  netconn_delete(sock->conn);

  lastdata         = sock->lastdata;
  if (lastdata != NULL) {
    if (is_tcp) {
      pbuf_free((struct pbuf *)lastdata);
    } else {
      netbuf_delete((struct netbuf *)lastdata);
    }
  }

  return 0;
}

int bsd_connect(struct bsd *bsd, struct bsd_sock *sock, const struct sockaddr *name, socklen_t namelen)
{
  err_t err;

  if (!SOCK_ADDR_TYPE_MATCH_OR_UNSPEC(name, sock)) {
    /* sockaddr does not match socket type (IPv4/IPv6) */
   return err_to_errno(ERR_VAL);
  }

  /* check size, familiy and alignment of 'name' */
  LWIP_ERROR("bsd_connect: invalid address", IS_SOCK_ADDR_LEN_VALID(namelen) &&
             IS_SOCK_ADDR_TYPE_VALID_OR_UNSPEC(name) && IS_SOCK_ADDR_ALIGNED(name),
             return err_to_errno(ERR_ARG); );
  LWIP_UNUSED_ARG(namelen);
  if (name->sa_family == AF_UNSPEC) {
    D(bug("bsd_connect(%p, AF_UNSPEC)\n", sock));
    err = netconn_disconnect(sock->conn);
  } else {
    ipX_addr_t remote_addr;
    u16_t remote_port;
    SOCKADDR_TO_IPXADDR_PORT((name->sa_family == AF_INET6), name, &remote_addr, remote_port);
    D(bug("bsd_connect(%p, addr=", sock));
    ipX_addr_debug_print(name->sa_family == AF_INET6, SOCKETS_DEBUG, &remote_addr);
    D(bug(" port=%"U16_F")\n", remote_port));

    err = netconn_connect(sock->conn, ipX_2_ip(&remote_addr), remote_port);
  }

  if (err != ERR_OK) {
    D(bug("bsd_connect(%p) failed, err=%d\n", sock, err));
    return err_to_errno(err);
    
  }

  D(bug("bsd_connect(%p) succeeded\n", sock));
  
  return 0;
}

/**
 * Set a socket into listen mode.
 * The socket may not have been used for another connection previously.
 *
 * @param s the socket to set to listening mode
 * @param backlog (ATTENTION: needs TCP_LISTEN_BACKLOG=1)
 * @return 0 on success, non-zero on failure
 */
int bsd_listen(struct bsd *bsd, struct bsd_sock *sock, int backlog)
{
  err_t err;

  D(bug("bsd_listen(%p, backlog=%d)\n", sock, backlog));

  /* limit the "backlog" parameter to fit in an u8_t */
  backlog = LWIP_MIN(LWIP_MAX(backlog, 0), 0xff);

  err = netconn_listen_with_backlog(sock->conn, (u8_t)backlog);

  if (err != ERR_OK) {
    D(bug("bsd_listen(%p) failed, err=%d\n", sock, err));
    if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_TCP) {
      return EOPNOTSUPP;
      return EOPNOTSUPP;
    }
    return err_to_errno(err);
    
  }

  
  return 0;
}

int bsd_recvmsg(struct bsd *bsd, struct bsd_sock *sock, struct msghdr *msg, int flags)
{
  void             *buf = NULL;
  struct pbuf      *p;
  u16_t            buflen, copylen;
  int              off = 0, i;
  u8_t             done = 0;
  err_t            err;
  struct sockaddr *from = (struct sockaddr *)msg->msg_name;
  socklen_t *fromlen = &msg->msg_namelen;

  for (i = 0; i < msg->msg_iovlen; i++) {
    void *mem = msg->msg_iov[i].iov_base;
    size_t len = msg->msg_iov[i].iov_len;

    D(bug("bsd_recvmsg(%p, [%d] { %p, %"SZT_F", 0x%x })\n", sock, i, mem, len, flags));

    done = 0;
    do {
        D(bug("bsd_recvmsg: top while sock->lastdata=%p\n", sock->lastdata));
        /* Check if there is data left from the last recv operation. */
        if (sock->lastdata) {
          buf = sock->lastdata;
        } else {
          /* If this is non-blocking call, then check first */
          if (((flags & MSG_DONTWAIT) || netconn_is_nonblocking(sock->conn)) && 
              (sock->rcvevent <= 0)) {
            if (off > 0) {
              /* update receive window */
              netconn_recved(sock->conn, (u32_t)off);
              /* already received data, return that */
              
              return off;
            }
            D(bug("bsd_recvmsg(%p): returning EWOULDBLOCK\n", sock));
            return EWOULDBLOCK;
            
          }

          /* No data was left from the previous operation, so we try to get
             some from the network. */
          if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) {
            err = netconn_recv_tcp_pbuf(sock->conn, (struct pbuf **)&buf);
          } else {
            err = netconn_recv(sock->conn, (struct netbuf **)&buf);
          }
          D(bug("bsd_recvmsg: netconn_recv err=%d, netbuf=%p\n",
            err, buf));

          if (err != ERR_OK) {
            if (off > 0) {
              /* update receive window */
              netconn_recved(sock->conn, (u32_t)off);
              /* already received data, return that */
              
              return off;
            }
            /* We should really do some error checking here. */
            D(bug("bsd_recvmsg(%s): buf == NULL, error is \"%s\"!\n",
              sock, lwip_strerr(err)));
            return err_to_errno(err);
            if (err == ERR_CLSD) {
              return 0;
            } else {
              
            }
          }
          BSD_ASSERT("buf != NULL", buf != NULL);
          sock->lastdata = buf;
        }

        if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) {
          p = (struct pbuf *)buf;
        } else {
          p = ((struct netbuf *)buf)->p;
        }
        buflen = p->tot_len;
        D(bug("bsd_recvmsg: buflen=%"U16_F" len=%"SZT_F" off=%d sock->lastoffset=%"U16_F"\n",
          buflen, len, off, sock->lastoffset));

        buflen -= sock->lastoffset;

        if (len > buflen) {
          copylen = buflen;
        } else {
          copylen = (u16_t)len;
        }

        /* copy the contents of the received buffer into
        the supplied memory pointer mem */
        pbuf_copy_partial(p, (u8_t*)mem + off, copylen, sock->lastoffset);

        off += copylen;

        if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) {
          BSD_ASSERT("invalid copylen, len would underflow", len >= copylen);
          len -= copylen;
          if ( (len <= 0) || 
               (p->flags & PBUF_FLAG_PUSH) || 
               (sock->rcvevent <= 0) || 
               ((flags & MSG_PEEK)!=0)) {
            done = 1;
          }
        } else {
          done = 1;
        }

        /* Check to see from where the data was.*/
        if (done) {
#if !SOCKETS_DEBUG
          if (from && fromlen)
#endif /* !SOCKETS_DEBUG */
          {
            u16_t port;
            ipX_addr_t tmpaddr;
            ipX_addr_t *fromaddr;
            union sockaddr_aligned saddr;
            D(bug("bsd_recvmsg(%p): addr=", sock));
            if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) {
              fromaddr = &tmpaddr;
              /* @todo: this does not work for IPv6, yet */
              netconn_getaddr(sock->conn, ipX_2_ip(fromaddr), &port, 0);
            } else {
              port = netbuf_fromport((struct netbuf *)buf);
              fromaddr = netbuf_fromaddr_ipX((struct netbuf *)buf);
            }
            IPXADDR_PORT_TO_SOCKADDR(NETCONNTYPE_ISIPV6(netconn_type(sock->conn)),
              &saddr, fromaddr, port);
            ipX_addr_debug_print(NETCONNTYPE_ISIPV6(netconn_type(sock->conn)),
              SOCKETS_DEBUG, fromaddr);
            D(bug(" port=%"U16_F" len=%d\n", port, off));
#if SOCKETS_DEBUG
            if (from && fromlen)
#endif /* SOCKETS_DEBUG */
            {
              if (*fromlen > saddr.sa.sa_len) {
                *fromlen = saddr.sa.sa_len;
              }
              MEMCPY(from, &saddr, *fromlen);
            }
          }
        }

        /* If we don't peek the incoming message... */
        if ((flags & MSG_PEEK) == 0) {
          /* If this is a TCP socket, check if there is data left in the
             buffer. If so, it should be saved in the sock structure for next
             time around. */
          if ((NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) && (buflen - copylen > 0)) {
            sock->lastdata = buf;
            sock->lastoffset += copylen;
            D(bug("bsd_recvmsg: lastdata now netbuf=%p\n", buf));
          } else {
            sock->lastdata = NULL;
            sock->lastoffset = 0;
            D(bug("bsd_recvmsg: deleting netbuf=%p\n", buf));
            if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) {
              pbuf_free((struct pbuf *)buf);
            } else {
              netbuf_delete((struct netbuf *)buf);
            }
          }
        }
      } while (!done);

      if ((off > 0) && (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP)) {
        /* update receive window */
        netconn_recved(sock->conn, (u32_t)off);
      }
  }
  
  return off;
}

int bsd_sendmsg(struct bsd *bsd, struct bsd_sock *sock, const struct msghdr *msg, int flags)
{
  err_t err;
  u16_t short_size;
  u16_t remote_port;
  int i, write_flags;
#if !LWIP_TCPIP_CORE_LOCKING
  struct netbuf buf;
#endif
  struct sockaddr *to = (struct sockaddr *)msg->msg_name;
  socklen_t tolen = msg->msg_namelen;

  write_flags = NETCONN_COPY |
    ((flags & MSG_MORE)     ? NETCONN_MORE      : 0) |
    ((flags & MSG_DONTWAIT) ? NETCONN_DONTBLOCK : 0);

  if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_TCP) {
#if LWIP_TCP
      err_t err = 0;
      for (i = 0; i < msg->msg_iovlen; i++) {
          err = netconn_write_partly(sock->conn, msg->msg_iov[i].iov_base, msg->msg_iov[i].iov_len, write_flags, NULL);
          if (err != ERR_OK)
              break;
      }
      return err_to_errno(err);
#else /* LWIP_TCP */
      LWIP_UNUSED_ARG(flags);
      return err_to_errno(ERR_ARG);
#endif
  }

  if ((to != NULL) && !SOCK_ADDR_TYPE_MATCH(to, sock)) {
    /* sockaddr does not match socket type (IPv4/IPv6) */
    return err_to_errno(ERR_VAL);
    
  }

  for (i = 0; i < msg->msg_iovlen; i++) {
    size_t size = msg->msg_iov[i].iov_len;
    void *data = msg->msg_iov[i].iov_base;

    /* @todo: split into multiple sendto's? */
    BSD_ASSERT("bsd_sendmsg: size must fit in u16_t", size <= 0xffff);
    short_size = (u16_t)size;
    LWIP_ERROR("bsd_sendmsg: invalid address", (((to == NULL) && (tolen == 0)) ||
               (IS_SOCK_ADDR_LEN_VALID(tolen) &&
               IS_SOCK_ADDR_TYPE_VALID(to) && IS_SOCK_ADDR_ALIGNED(to))),
               return err_to_errno(ERR_ARG); );
    LWIP_UNUSED_ARG(tolen);

#if LWIP_TCPIP_CORE_LOCKING
    /* Special speedup for fast UDP/RAW sending: call the raw API directly
       instead of using the netconn functions. */
      struct pbuf* p;
      ipX_addr_t *remote_addr;
      ipX_addr_t remote_addr_tmp;

#if LWIP_NETIF_TX_SINGLE_PBUF
      p = pbuf_alloc(PBUF_TRANSPORT, short_size, PBUF_RAM);
      if (p != NULL) {
#if LWIP_CHECKSUM_ON_COPY
        u16_t chksum = 0;
        if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_RAW) {
          chksum = LWIP_CHKSUM_COPY(p->payload, data, short_size);
        } else
#endif /* LWIP_CHECKSUM_ON_COPY */
        MEMCPY(p->payload, data, size);
#else /* LWIP_NETIF_TX_SINGLE_PBUF */
      p = pbuf_alloc(PBUF_TRANSPORT, short_size, PBUF_REF);
      if (p != NULL) {
        p->payload = (void*)data;
#endif /* LWIP_NETIF_TX_SINGLE_PBUF */

        if (to != NULL) {
          SOCKADDR_TO_IPXADDR_PORT(to->sa_family == AF_INET6,
            to, &remote_addr_tmp, remote_port);
          remote_addr = &remote_addr_tmp;
        } else {
          remote_addr = &sock->conn->pcb.ip->remote_ip;
#if LWIP_UDP
          if (NETCONNTYPE_GROUP(sock->conn->type) == NETCONN_UDP) {
            remote_port = sock->conn->pcb.udp->remote_port;
          } else
#endif /* LWIP_UDP */
          {
            remote_port = 0;
          }
        }

        LOCK_TCPIP_CORE();
        if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) == NETCONN_RAW) {
#if LWIP_RAW
          err = sock->conn->last_err = raw_sendto(sock->conn->pcb.raw, p, ipX_2_ip(remote_addr));
#else /* LWIP_RAW */
          err = ERR_ARG;
#endif /* LWIP_RAW */
        }
#if LWIP_UDP && LWIP_RAW
        else
#endif /* LWIP_UDP && LWIP_RAW */
        {
#if LWIP_UDP
#if LWIP_CHECKSUM_ON_COPY && LWIP_NETIF_TX_SINGLE_PBUF
          err = sock->conn->last_err = udp_sendto_chksum(sock->conn->pcb.udp, p,
            ipX_2_ip(remote_addr), remote_port, 1, chksum);
#else /* LWIP_CHECKSUM_ON_COPY && LWIP_NETIF_TX_SINGLE_PBUF */
          err = sock->conn->last_err = udp_sendto(sock->conn->pcb.udp, p,
            ipX_2_ip(remote_addr), remote_port);
#endif /* LWIP_CHECKSUM_ON_COPY && LWIP_NETIF_TX_SINGLE_PBUF */
#else /* LWIP_UDP */
          err = ERR_ARG;
#endif /* LWIP_UDP */
        }
        UNLOCK_TCPIP_CORE();
        
        pbuf_free(p);
      } else {
        err = ERR_MEM;
      }
    }
#else /* LWIP_TCPIP_CORE_LOCKING */
    /* initialize a buffer */
    buf.p = buf.ptr = NULL;
#if LWIP_CHECKSUM_ON_COPY
    buf.flags = 0;
#endif /* LWIP_CHECKSUM_ON_COPY */
    if (to) {
      SOCKADDR_TO_IPXADDR_PORT((to->sa_family) == AF_INET6, to, &buf.addr, remote_port);
    } else {
      remote_port = 0;
      ipX_addr_set_any(NETCONNTYPE_ISIPV6(netconn_type(sock->conn)), &buf.addr);
    }
    netbuf_fromport(&buf) = remote_port;


    D(bug("bsd_sendmsg(%p, data=%p, short_size=%"U16_F", flags=0x%x to=",
                sock, data, short_size, flags));
    ipX_addr_debug_print(NETCONNTYPE_ISIPV6(netconn_type(sock->conn)),
      SOCKETS_DEBUG, &buf.addr);
    D(bug(" port=%"U16_F"\n", remote_port));

    /* make the buffer point to the data that should be sent */
#if LWIP_NETIF_TX_SINGLE_PBUF
    /* Allocate a new netbuf and copy the data into it. */
    if (netbuf_alloc(&buf, short_size) == NULL) {
      err = ERR_MEM;
    } else {
#if LWIP_CHECKSUM_ON_COPY
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_RAW) {
        u16_t chksum = LWIP_CHKSUM_COPY(buf.p->payload, data, short_size);
        netbuf_set_chksum(&buf, chksum);
        err = ERR_OK;
      } else
#endif /* LWIP_CHECKSUM_ON_COPY */
      {
        err = netbuf_take(&buf, data, short_size);
      }
    }
#else /* LWIP_NETIF_TX_SINGLE_PBUF */
    err = netbuf_ref(&buf, data, short_size);
#endif /* LWIP_NETIF_TX_SINGLE_PBUF */
    if (err == ERR_OK) {
      /* send the data */
      err = netconn_send(sock->conn, &buf);
    }

    /* deallocated the buffer */
    netbuf_free(&buf);
#endif /* LWIP_TCPIP_CORE_LOCKING */
  }
  return (err == ERR_OK ? short_size : err_to_errno(err));
}

int bsd_socket(struct bsd *bsd, struct bsd_sock **sockp, int domain, int type, int protocol)
{
  struct netconn *conn;
  int i;

#if !LWIP_IPV6
  LWIP_UNUSED_ARG(domain); /* @todo: check this */
#endif /* LWIP_IPV6 */

  /* create a netconn */
  switch (type) {
  case SOCK_RAW:
    conn = netconn_new_with_proto_and_callback(DOMAIN_TO_NETCONN_TYPE(domain, NETCONN_RAW),
                                               (u8_t)protocol, event_callback);
    D(bug("bsd_socket(%s, SOCK_RAW, %d) = ",
                                 domain == PF_INET ? "PF_INET" : "UNKNOWN", protocol));
    break;
  case SOCK_DGRAM:
    conn = netconn_new_with_callback(DOMAIN_TO_NETCONN_TYPE(domain,
                 ((protocol == IPPROTO_UDPLITE) ? NETCONN_UDPLITE : NETCONN_UDP)) ,
                 event_callback);
    D(bug("bsd_socket(%s, SOCK_DGRAM, %d) = ",
                                 domain == PF_INET ? "PF_INET" : "UNKNOWN", protocol));
    break;
  case SOCK_STREAM:
    conn = netconn_new_with_callback(DOMAIN_TO_NETCONN_TYPE(domain, NETCONN_TCP), event_callback);
    D(bug("bsd_socket(%s, SOCK_STREAM, %d) = ",
                                 domain == PF_INET ? "PF_INET" : "UNKNOWN", protocol));
    if (conn != NULL) {
      /* Prevent automatic window updates, we do this on our own! */
      netconn_set_noautorecved(conn, 1);
    }
    break;
  default:
    D(bug("bsd_socket(%d, %d/UNKNOWN, %d) = -1\n",
                                 domain, type, protocol));
    return EINVAL;
    
  }

  if (!conn) {
    D(bug("-1 / ENOBUFS (could not create netconn)\n"));
    return ENOBUFS;
    
  }

  i = alloc_socket(bsd, sockp, conn, 0);

  if (i) {
    netconn_delete(conn);
    return i;
    
  }

  D(bug("%d\n", i));
  
  return i;
}

/**
 * Callback registered in the netconn layer for each socket-netconn.
 * Processes recvevent (data available) and wakes up tasks waiting for select.
 */
static void event_callback(struct netconn *conn, enum netconn_evt evt, u16_t len)
{
  int s;
  struct bsd *bsd;
  struct bsd_sock *sock;
  struct bsd_select_cb *scb;
  int last_select_cb_ctr;
  SYS_ARCH_DECL_PROTECT(lev);

  LWIP_UNUSED_ARG(len);

  /* Get socket */
  if (conn) {
    s = conn->socket;
    if (s < 0) {
      /* Data comes in right away after an accept, even though
       * the server task might not have created a new socket yet.
       * Just count down (or up) if that's the case and we
       * will use the data later. Note that only receive events
       * can happen before the new socket is set up. */
      SYS_ARCH_PROTECT(lev);
      if (conn->socket < 0) {
        if (evt == NETCONN_EVT_RCVPLUS) {
          conn->socket--;
        }
        SYS_ARCH_UNPROTECT(lev);
        return;
      }
      SYS_ARCH_UNPROTECT(lev);
    }

    sock = conn->callback_priv;
    if (!sock) {
      return;
    }
  } else {
    return;
  }

  bsd = sock->bsd;

  SYS_ARCH_PROTECT(lev);
  /* Set event as required */
  switch (evt) {
    case NETCONN_EVT_RCVPLUS:
      sock->rcvevent++;
      break;
    case NETCONN_EVT_RCVMINUS:
      sock->rcvevent--;
      break;
    case NETCONN_EVT_SENDPLUS:
      sock->sendevent = 1;
      break;
    case NETCONN_EVT_SENDMINUS:
      sock->sendevent = 0;
      break;
    case NETCONN_EVT_ERROR:
      sock->errevent = 1;
      break;
    default:
      BSD_ASSERT("unknown event", 0);
      break;
  }

  if (sock->select_waiting == 0) {
    /* noone is waiting for this socket, no need to check bsd->bs_select_cb.list */
    SYS_ARCH_UNPROTECT(lev);
    return;
  }

  /* Now decide if anyone is waiting for this socket */
  /* NOTE: This code goes through the bsd->bs_select_cb.list list multiple times
     ONLY IF a select was actually waiting. We go through the list the number
     of waiting select calls + 1. This list is expected to be small. */

  /* At this point, SYS_ARCH is still protected! */
again:
  for (scb = bsd->bs_select_cb.list; scb != NULL; scb = scb->next) {
    if (scb->sem_signalled == 0) {
      /* semaphore not signalled yet */
      int do_signal = 0;
      /* Test this select call for our socket */
      if (sock->rcvevent > 0) {
        if (scb->readset && FD_ISSET(s, scb->readset)) {
          do_signal = 1;
        }
      }
      if (sock->sendevent != 0) {
        if (!do_signal && scb->writeset && FD_ISSET(s, scb->writeset)) {
          do_signal = 1;
        }
      }
      if (sock->errevent != 0) {
        if (!do_signal && scb->exceptset && FD_ISSET(s, scb->exceptset)) {
          do_signal = 1;
        }
      }
      if (do_signal) {
        scb->sem_signalled = 1;
        /* Don't call SYS_ARCH_UNPROTECT() before signaling the semaphore, as this might
           lead to the select thread taking itself off the list, invalidagin the semaphore. */
        sys_sem_signal(&scb->sem);
      }
    }
    /* unlock interrupts with each step */
    last_select_cb_ctr = bsd->bs_select_cb.ctr;
    SYS_ARCH_UNPROTECT(lev);
    /* this makes sure interrupt protection time is short */
    SYS_ARCH_PROTECT(lev);
    if (last_select_cb_ctr != bsd->bs_select_cb.ctr) {
      /* someone has changed bsd->bs_select_cb.list, restart at the beginning */
      goto again;
    }
  }
  SYS_ARCH_UNPROTECT(lev);
}

/**
 * Unimplemented: Close one end of a full-duplex connection.
 * Currently, the full connection is closed.
 */
int bsd_shutdown(struct bsd *bsd, struct bsd_sock *sock, int how)
{
  err_t err;
  u8_t shut_rx = 0, shut_tx = 0;

  D(bug("bsd_shutdown(%p, how=%d)\n", sock, how));

  if (sock->conn != NULL) {
    if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_TCP) {
      return EOPNOTSUPP;
      return EOPNOTSUPP;
    }
  } else {
    return ENOTCONN;
    return ENOTCONN;
  }

  if (how == SHUT_RD) {
    shut_rx = 1;
  } else if (how == SHUT_WR) {
    shut_tx = 1;
  } else if(how == SHUT_RDWR) {
    shut_rx = 1;
    shut_tx = 1;
  } else {
    return EINVAL;
    return EINVAL;
  }
  err = netconn_shutdown(sock->conn, shut_rx, shut_tx);

  return err_to_errno(err);
  return (err == ERR_OK ? 0 : -1);
}

static int bsd__getaddrname(struct bsd *bsd, struct bsd_sock *sock, struct sockaddr *name, socklen_t *namelen, u8_t local)
{
  union sockaddr_aligned saddr;
  ipX_addr_t naddr;
  u16_t port;

  /* get the IP address and port */
  /* @todo: this does not work for IPv6, yet */
  netconn_getaddr(sock->conn, ipX_2_ip(&naddr), &port, local);
  IPXADDR_PORT_TO_SOCKADDR(NETCONNTYPE_ISIPV6(netconn_type(sock->conn)),
    &saddr, &naddr, port);

  D(bug("bsd__getaddrname(%p, addr=", sock));
  ipX_addr_debug_print(NETCONNTYPE_ISIPV6(netconn_type(sock->conn)),
    SOCKETS_DEBUG, &naddr);
  D(bug(" port=%"U16_F")\n", port));

  if (*namelen > saddr.sa.sa_len) {
    *namelen = saddr.sa.sa_len;
  }
  MEMCPY(name, &saddr, *namelen);

  return 0;
}

int bsd_getpeername(struct bsd *bsd, struct bsd_sock *s, struct sockaddr *name, socklen_t *namelen)
{
  return bsd__getaddrname(bsd, s, name, namelen, 0);
}

int bsd_getsockname(struct bsd *bsd, struct bsd_sock *s, struct sockaddr *name, socklen_t *namelen)
{
  return bsd__getaddrname(bsd, s, name, namelen, 1);
}

int bsd_getsockopt(struct bsd *bsd, struct bsd_sock *sock, int level, int optname, void *optval, socklen_t *optlen)
{
  err_t err = ERR_OK;
  struct bsd_setgetsockopt_data data;

  if ((NULL == optval) || (NULL == optlen)) {
    return EFAULT;
    
  }

  /* Do length and type checks for the various options first, to keep it readable. */
  switch (level) {
   
/* Level: SOL_SOCKET */
  case SOL_SOCKET:
    switch (optname) {
       
    case SO_ACCEPTCONN:
    case SO_BROADCAST:
    /* UNIMPL case SO_DEBUG: */
    /* UNIMPL case SO_DONTROUTE: */
    case SO_ERROR:
    case SO_KEEPALIVE:
    /* UNIMPL case SO_CONTIMEO: */
#if LWIP_SO_SNDTIMEO
    case SO_SNDTIMEO:
#endif /* LWIP_SO_SNDTIMEO */
#if LWIP_SO_RCVTIMEO
    case SO_RCVTIMEO:
#endif /* LWIP_SO_RCVTIMEO */
#if LWIP_SO_RCVBUF
    case SO_RCVBUF:
#endif /* LWIP_SO_RCVBUF */
    /* UNIMPL case SO_OOBINLINE: */
    /* UNIMPL case SO_SNDBUF: */
    /* UNIMPL case SO_RCVLOWAT: */
    /* UNIMPL case SO_SNDLOWAT: */
#if SO_REUSE
    case SO_REUSEADDR:
    case SO_REUSEPORT:
#endif /* SO_REUSE */
    case SO_TYPE:
    /* UNIMPL case SO_USELOOPBACK: */
      if (*optlen < sizeof(int)) {
        err = EINVAL;
      }
      break;

    case SO_NO_CHECK:
      if (*optlen < sizeof(int)) {
        err = EINVAL;
      }
#if LWIP_UDP
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_UDP ||
          ((udp_flags(sock->conn->pcb.udp) & UDP_FLAGS_UDPLITE) != 0)) {
        /* this flag is only available for UDP, not for UDP lite */
        err = EAFNOSUPPORT;
      }
#endif /* LWIP_UDP */
      break;

    default:
      D(bug("bsd_getsockopt(%d, SOL_SOCKET, UNIMPL: optname=0x%x, ..)\n",
                                  sock, optname));
      err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;
                     
/* Level: IPPROTO_IP */
  case IPPROTO_IP:
    switch (optname) {
    /* UNIMPL case IP_HDRINCL: */
    /* UNIMPL case IP_RCVDSTADDR: */
    /* UNIMPL case IP_RCVIF: */
    case IP_TTL:
    case IP_TOS:
      if (*optlen < sizeof(int)) {
        err = EINVAL;
      }
      break;
#if LWIP_IGMP
    case IP_MULTICAST_TTL:
      if (*optlen < sizeof(u8_t)) {
        err = EINVAL;
      }
      break;
    case IP_MULTICAST_IF:
      if (*optlen < sizeof(struct in_addr)) {
        err = EINVAL;
      }
      break;
    case IP_MULTICAST_LOOP:
      if (*optlen < sizeof(u8_t)) {
        err = EINVAL;
      }
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_UDP) {
        err = EAFNOSUPPORT;
      }
      break;
#endif /* LWIP_IGMP */

    default:
      D(bug("bsd_getsockopt(%d, IPPROTO_IP, UNIMPL: optname=0x%x, ..)\n",
                                  sock, optname));
      err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;
         
#if LWIP_TCP
/* Level: IPPROTO_TCP */
  case IPPROTO_TCP:
    if (*optlen < sizeof(int)) {
      err = EINVAL;
      break;
    }
    
    /* If this is no TCP socket, ignore any options. */
    if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_TCP)
      return 0;

    switch (optname) {
    case TCP_NODELAY:
    case TCP_KEEPALIVE:
#if LWIP_TCP_KEEPALIVE
    case TCP_KEEPIDLE:
    case TCP_KEEPINTVL:
    case TCP_KEEPCNT:
#endif /* LWIP_TCP_KEEPALIVE */
      break;
       
    default:
      D(bug("bsd_getsockopt(%d, IPPROTO_TCP, UNIMPL: optname=0x%x, ..)\n",
                                  sock, optname));
      err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;
#endif /* LWIP_TCP */

#if LWIP_IPV6
/* Level: IPPROTO_IPV6 */
  case IPPROTO_IPV6:
    switch (optname) {
    case IPV6_V6ONLY:
      if (*optlen < sizeof(int)) {
        err = EINVAL;
      }
      /* @todo: this does not work for datagram sockets, yet */
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_TCP)
        return 0;
      break;
    default:
      D(bug("bsd_getsockopt(%d, IPPROTO_IPV6, UNIMPL: optname=0x%x, ..)\n",
                                  sock, optname));
      err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;
#endif /* LWIP_IPV6 */

#if LWIP_UDP && LWIP_UDPLITE
/* Level: IPPROTO_UDPLITE */
  case IPPROTO_UDPLITE:
    if (*optlen < sizeof(int)) {
      err = EINVAL;
      break;
    }
    
    /* If this is no UDP lite socket, ignore any options. */
    if (!NETCONNTYPE_ISUDPLITE(netconn_type(sock->conn))) {
      return 0;
    }

    switch (optname) {
    case UDPLITE_SEND_CSCOV:
    case UDPLITE_RECV_CSCOV:
      break;
       
    default:
      D(bug("bsd_getsockopt(%d, IPPROTO_UDPLITE, UNIMPL: optname=0x%x, ..)\n",
                                  sock, optname));
      err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;
#endif /* LWIP_UDP && LWIP_UDPLITE*/
/* UNDEFINED LEVEL */
  default:
      D(bug("bsd_getsockopt(%d, level=0x%x, UNIMPL: optname=0x%x, ..)\n",
                                  sock, level, optname));
      err = ENOPROTOOPT;
  }  /* switch */

   
  if (err)
      return err;

  /* Now do the actual option processing */
  data.sock = sock;
  data.level = level;
  data.optname = optname;
  data.optval = optval;
  data.optlen = optlen;
  data.err = err;
  data.bsd = bsd;
  tcpip_callback(bsd_getsockopt_internal, &data);
  sys_arch_sem_wait(&sock->conn->op_completed, 0);
  /* maybe bsd_getsockopt_internal has changed err */
  err = data.err;

  return err;
}

static void bsd_getsockopt_internal(void *arg)
{
  struct bsd_sock *sock;
  int level, optname;
  void *optval;
  struct bsd_setgetsockopt_data *data;
  struct bsd *bsd = NULL;

  BSD_ASSERT("arg != NULL", arg != NULL);

  data = (struct bsd_setgetsockopt_data*)arg;
  bsd = data->bsd;
  sock = data->sock;
  level = data->level;
  optname = data->optname;
  optval = data->optval;

  switch (level) {

/* Level: SOL_SOCKET */
  case SOL_SOCKET:
    switch (optname) {

    /* The option flags */
    case SO_ACCEPTCONN:
    case SO_BROADCAST:
    /* UNIMPL case SO_DEBUG: */
    /* UNIMPL case SO_DONTROUTE: */
    case SO_KEEPALIVE:
    /* UNIMPL case SO_OOBINCLUDE: */
#if SO_REUSE
    case SO_REUSEADDR:
    case SO_REUSEPORT:
#endif /* SO_REUSE */
    /*case SO_USELOOPBACK: UNIMPL */
      *(int*)optval = ip_get_option(sock->conn->pcb.ip, optname);
      D(bug("bsd_getsockopt(%d, SOL_SOCKET, optname=0x%x, ..) = %s\n",
                                  sock, optname, (*(int*)optval?"on":"off")));
      break;

    case SO_TYPE:
      switch (NETCONNTYPE_GROUP(netconn_type(sock->conn))) {
      case NETCONN_RAW:
        *(int*)optval = SOCK_RAW;
        break;
      case NETCONN_TCP:
        *(int*)optval = SOCK_STREAM;
        break;
      case NETCONN_UDP:
        *(int*)optval = SOCK_DGRAM;
        break;
      default: /* unrecognized socket type */
        *(int*)optval = netconn_type(sock->conn);
        LWIP_DEBUGF(SOCKETS_DEBUG,
                    ("bsd_getsockopt(%d, SOL_SOCKET, SO_TYPE): unrecognized socket type %d\n",
                    sock, *(int *)optval));
      }  /* switch (netconn_type(sock->conn)) */
      D(bug("bsd_getsockopt(%d, SOL_SOCKET, SO_TYPE) = %d\n",
                  sock, *(int *)optval));
      break;

#if LWIP_SO_SNDTIMEO
    case SO_SNDTIMEO:
      *(int *)optval = netconn_get_sendtimeout(sock->conn);
      break;
#endif /* LWIP_SO_SNDTIMEO */
#if LWIP_SO_RCVTIMEO
    case SO_RCVTIMEO:
      *(int *)optval = netconn_get_recvtimeout(sock->conn);
      break;
#endif /* LWIP_SO_RCVTIMEO */
#if LWIP_SO_RCVBUF
    case SO_RCVBUF:
      *(int *)optval = netconn_get_recvbufsize(sock->conn);
      break;
#endif /* LWIP_SO_RCVBUF */
#if LWIP_UDP
    case SO_NO_CHECK:
      *(int*)optval = (udp_flags(sock->conn->pcb.udp) & UDP_FLAGS_NOCHKSUM) ? 1 : 0;
      break;
#endif /* LWIP_UDP*/
    default:
      BSD_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;

/* Level: IPPROTO_IP */
  case IPPROTO_IP:
    switch (optname) {
    case IP_TTL:
      *(int*)optval = sock->conn->pcb.ip->ttl;
      D(bug("bsd_getsockopt(%d, IPPROTO_IP, IP_TTL) = %d\n",
                  sock, *(int *)optval));
      break;
    case IP_TOS:
      *(int*)optval = sock->conn->pcb.ip->tos;
      D(bug("bsd_getsockopt(%d, IPPROTO_IP, IP_TOS) = %d\n",
                  sock, *(int *)optval));
      break;
#if LWIP_IGMP
    case IP_MULTICAST_TTL:
      *(u8_t*)optval = sock->conn->pcb.ip->ttl;
      D(bug("bsd_getsockopt(%d, IPPROTO_IP, IP_MULTICAST_TTL) = %d\n",
                  sock, *(int *)optval));
      break;
    case IP_MULTICAST_IF:
      inet_addr_from_ipaddr((struct in_addr*)optval, &sock->conn->pcb.udp->multicast_ip);
      D(bug("bsd_getsockopt(%d, IPPROTO_IP, IP_MULTICAST_IF) = 0x%"X32_F"\n",
                  sock, *(u32_t *)optval));
      break;
    case IP_MULTICAST_LOOP:
      if ((sock->conn->pcb.udp->flags & UDP_FLAGS_MULTICAST_LOOP) != 0) {
        *(u8_t*)optval = 1;
      } else {
        *(u8_t*)optval = 0;
      }
      D(bug("bsd_getsockopt(%d, IPPROTO_IP, IP_MULTICAST_LOOP) = %d\n",
                  sock, *(int *)optval));
      break;
#endif /* LWIP_IGMP */
    default:
      BSD_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;

#if LWIP_TCP
/* Level: IPPROTO_TCP */
  case IPPROTO_TCP:
    switch (optname) {
    case TCP_NODELAY:
      *(int*)optval = tcp_nagle_disabled(sock->conn->pcb.tcp);
      D(bug("bsd_getsockopt(%d, IPPROTO_TCP, TCP_NODELAY) = %s\n",
                  sock, (*(int*)optval)?"on":"off") );
      break;
    case TCP_KEEPALIVE:
      *(int*)optval = (int)sock->conn->pcb.tcp->keep_idle;
      D(bug("bsd_getsockopt(%d, IPPROTO_IP, TCP_KEEPALIVE) = %d\n",
                  sock, *(int *)optval));
      break;

#if LWIP_TCP_KEEPALIVE
    case TCP_KEEPIDLE:
      *(int*)optval = (int)(sock->conn->pcb.tcp->keep_idle/1000);
      D(bug("bsd_getsockopt(%d, IPPROTO_IP, TCP_KEEPIDLE) = %d\n",
                  sock, *(int *)optval));
      break;
    case TCP_KEEPINTVL:
      *(int*)optval = (int)(sock->conn->pcb.tcp->keep_intvl/1000);
      D(bug("bsd_getsockopt(%d, IPPROTO_IP, TCP_KEEPINTVL) = %d\n",
                  sock, *(int *)optval));
      break;
    case TCP_KEEPCNT:
      *(int*)optval = (int)sock->conn->pcb.tcp->keep_cnt;
      D(bug("bsd_getsockopt(%d, IPPROTO_IP, TCP_KEEPCNT) = %d\n",
                  sock, *(int *)optval));
      break;
#endif /* LWIP_TCP_KEEPALIVE */
    default:
      BSD_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;
#endif /* LWIP_TCP */

#if LWIP_IPV6
/* Level: IPPROTO_IPV6 */
  case IPPROTO_IPV6:
    switch (optname) {
    case IPV6_V6ONLY:
      *(int*)optval = ((sock->conn->flags & NETCONN_FLAG_IPV6_V6ONLY) ? 1 : 0);
      D(bug("bsd_getsockopt(%d, IPPROTO_IPV6, IPV6_V6ONLY) = %d\n",
                  sock, *(int *)optval));
      break;
    default:
      BSD_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;
#endif /* LWIP_IPV6 */

#if LWIP_UDP && LWIP_UDPLITE
  /* Level: IPPROTO_UDPLITE */
  case IPPROTO_UDPLITE:
    switch (optname) {
    case UDPLITE_SEND_CSCOV:
      *(int*)optval = sock->conn->pcb.udp->chksum_len_tx;
      D(bug("bsd_getsockopt(%p, IPPROTO_UDPLITE, UDPLITE_SEND_CSCOV) = %d\n",
                  sock, (*(int*)optval)) );
      break;
    case UDPLITE_RECV_CSCOV:
      *(int*)optval = sock->conn->pcb.udp->chksum_len_rx;
      D(bug("bsd_getsockopt(%p, IPPROTO_UDPLITE, UDPLITE_RECV_CSCOV) = %d\n",
                  sock, (*(int*)optval)) );
      break;
    default:
      BSD_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;
#endif /* LWIP_UDP */
  default:
    BSD_ASSERT("unhandled level", 0);
    break;
  } /* switch (level) */
  sys_sem_signal(&sock->conn->op_completed);
}

int bsd_setsockopt(struct bsd *bsd, struct bsd_sock *sock, int level, int optname, const void *optval, socklen_t optlen)
{
  err_t err = ERR_OK;
  struct bsd_setgetsockopt_data data;

  if (NULL == optval) {
    return EFAULT;
  }

  /* Do length and type checks for the various options first, to keep it readable. */
  switch (level) {

/* Level: SOL_SOCKET */
  case SOL_SOCKET:
    switch (optname) {

    case SO_BROADCAST:
    /* UNIMPL case SO_DEBUG: */
    /* UNIMPL case SO_DONTROUTE: */
    case SO_KEEPALIVE:
    /* UNIMPL case case SO_CONTIMEO: */
#if LWIP_SO_SNDTIMEO
    case SO_SNDTIMEO:
#endif /* LWIP_SO_SNDTIMEO */
#if LWIP_SO_RCVTIMEO
    case SO_RCVTIMEO:
#endif /* LWIP_SO_RCVTIMEO */
#if LWIP_SO_RCVBUF
    case SO_RCVBUF:
#endif /* LWIP_SO_RCVBUF */
    /* UNIMPL case SO_OOBINLINE: */
    /* UNIMPL case SO_SNDBUF: */
    /* UNIMPL case SO_RCVLOWAT: */
    /* UNIMPL case SO_SNDLOWAT: */
#if SO_REUSE
    case SO_REUSEADDR:
    case SO_REUSEPORT:
#endif /* SO_REUSE */
    /* UNIMPL case SO_USELOOPBACK: */
      if (optlen < sizeof(int)) {
        err = EINVAL;
      }
      break;
    case SO_NO_CHECK:
      if (optlen < sizeof(int)) {
        err = EINVAL;
      }
#if LWIP_UDP
      if ((NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_UDP) ||
          ((udp_flags(sock->conn->pcb.udp) & UDP_FLAGS_UDPLITE) != 0)) {
        /* this flag is only available for UDP, not for UDP lite */
        err = EAFNOSUPPORT;
      }
#endif /* LWIP_UDP */
      break;
    default:
      D(bug("bsd_setsockopt(%p, SOL_SOCKET, UNIMPL: optname=0x%x, ..)\n",
                  sock, optname));
      err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;

/* Level: IPPROTO_IP */
  case IPPROTO_IP:
    switch (optname) {
    /* UNIMPL case IP_HDRINCL: */
    /* UNIMPL case IP_RCVDSTADDR: */
    /* UNIMPL case IP_RCVIF: */
    case IP_TTL:
    case IP_TOS:
      if (optlen < sizeof(int)) {
        err = EINVAL;
      }
      break;
#if LWIP_IGMP
    case IP_MULTICAST_TTL:
      if (optlen < sizeof(u8_t)) {
        err = EINVAL;
      }
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_UDP) {
        err = EAFNOSUPPORT;
      }
      break;
    case IP_MULTICAST_IF:
      if (optlen < sizeof(struct in_addr)) {
        err = EINVAL;
      }
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_UDP) {
        err = EAFNOSUPPORT;
      }
      break;
    case IP_MULTICAST_LOOP:
      if (optlen < sizeof(u8_t)) {
        err = EINVAL;
      }
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_UDP) {
        err = EAFNOSUPPORT;
      }
      break;
    case IP_ADD_MEMBERSHIP:
    case IP_DROP_MEMBERSHIP:
      if (optlen < sizeof(struct ip_mreq)) {
        err = EINVAL;
      }
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_UDP) {
        err = EAFNOSUPPORT;
      }
      break;
#endif /* LWIP_IGMP */
      default:
        D(bug("bsd_setsockopt(%p, IPPROTO_IP, UNIMPL: optname=0x%x, ..)\n",
                    sock, optname));
        err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;

#if LWIP_TCP
/* Level: IPPROTO_TCP */
  case IPPROTO_TCP:
    if (optlen < sizeof(int)) {
      err = EINVAL;
      break;
    }

    /* If this is no TCP socket, ignore any options. */
    if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_TCP)
      return 0;

    switch (optname) {
    case TCP_NODELAY:
    case TCP_KEEPALIVE:
#if LWIP_TCP_KEEPALIVE
    case TCP_KEEPIDLE:
    case TCP_KEEPINTVL:
    case TCP_KEEPCNT:
#endif /* LWIP_TCP_KEEPALIVE */
      break;

    default:
      D(bug("bsd_setsockopt(%p, IPPROTO_TCP, UNIMPL: optname=0x%x, ..)\n",
                  sock, optname));
      err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;
#endif /* LWIP_TCP */

#if LWIP_IPV6
/* Level: IPPROTO_IPV6 */
  case IPPROTO_IPV6:
    switch (optname) {
    case IPV6_V6ONLY:
      if (optlen < sizeof(int)) {
        err = EINVAL;
      }

      /* @todo: this does not work for datagram sockets, yet */
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_TCP)
        return 0;

      break;
      default:
        D(bug("bsd_setsockopt(%p, IPPROTO_IPV6, UNIMPL: optname=0x%x, ..)\n",
                    sock, optname));
        err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;
#endif /* LWIP_IPV6 */

#if LWIP_UDP && LWIP_UDPLITE
/* Level: IPPROTO_UDPLITE */
  case IPPROTO_UDPLITE:
    if (optlen < sizeof(int)) {
      err = EINVAL;
      break;
    }

    /* If this is no UDP lite socket, ignore any options. */
    if (!NETCONNTYPE_ISUDPLITE(netconn_type(sock->conn)))
      return 0;

    switch (optname) {
    case UDPLITE_SEND_CSCOV:
    case UDPLITE_RECV_CSCOV:
      break;

    default:
      D(bug("bsd_setsockopt(%p, IPPROTO_UDPLITE, UNIMPL: optname=0x%x, ..)\n",
                  sock, optname));
      err = ENOPROTOOPT;
    }  /* switch (optname) */
    break;
#endif /* LWIP_UDP && LWIP_UDPLITE */
/* UNDEFINED LEVEL */
  default:
    D(bug("bsd_setsockopt(%p, level=0x%x, UNIMPL: optname=0x%x, ..)\n",
                sock, level, optname));
    err = ENOPROTOOPT;
  }  /* switch (level) */


  if (err != 0) {
    return err;
  }


  /* Now do the actual option processing */
  data.sock = sock;
  data.level = level;
  data.optname = optname;
  data.optval = (void*)optval;
  data.optlen = &optlen;
  data.err = err;
  data.bsd = bsd;
  tcpip_callback(bsd_setsockopt_internal, &data);
  sys_arch_sem_wait(&sock->conn->op_completed, 0);
  /* maybe bsd_setsockopt_internal has changed err */
  err = data.err;

  return err;
}

static void bsd_setsockopt_internal(void *arg)
{
  struct bsd_sock *sock;
  int level, optname;
  const void *optval;
  struct bsd_setgetsockopt_data *data;
  struct bsd *bsd = NULL;

  BSD_ASSERT("arg != NULL", arg != NULL);

  data = (struct bsd_setgetsockopt_data*)arg;
  bsd = data->bsd;
  sock = data->sock;
  level = data->level;
  optname = data->optname;
  optval = data->optval;

  switch (level) {

/* Level: SOL_SOCKET */
  case SOL_SOCKET:
    switch (optname) {

    /* The option flags */
    case SO_BROADCAST:
    /* UNIMPL case SO_DEBUG: */
    /* UNIMPL case SO_DONTROUTE: */
    case SO_KEEPALIVE:
    /* UNIMPL case SO_OOBINCLUDE: */
#if SO_REUSE
    case SO_REUSEADDR:
    case SO_REUSEPORT:
#endif /* SO_REUSE */
    /* UNIMPL case SO_USELOOPBACK: */
      if (*(int*)optval) {
        ip_set_option(sock->conn->pcb.ip, optname);
      } else {
        ip_reset_option(sock->conn->pcb.ip, optname);
      }
      D(bug("bsd_setsockopt(%p, SOL_SOCKET, optname=0x%x, ..) -> %s\n",
                  sock, optname, (*(int*)optval?"on":"off")));
      break;
#if LWIP_SO_SNDTIMEO
    case SO_SNDTIMEO:
      netconn_set_sendtimeout(sock->conn, (s32_t)*(int*)optval);
      break;
#endif /* LWIP_SO_SNDTIMEO */
#if LWIP_SO_RCVTIMEO
    case SO_RCVTIMEO:
      netconn_set_recvtimeout(sock->conn, *(int*)optval);
      break;
#endif /* LWIP_SO_RCVTIMEO */
#if LWIP_SO_RCVBUF
    case SO_RCVBUF:
      netconn_set_recvbufsize(sock->conn, *(int*)optval);
      break;
#endif /* LWIP_SO_RCVBUF */
#if LWIP_UDP
    case SO_NO_CHECK:
      if (*(int*)optval) {
        udp_setflags(sock->conn->pcb.udp, udp_flags(sock->conn->pcb.udp) | UDP_FLAGS_NOCHKSUM);
      } else {
        udp_setflags(sock->conn->pcb.udp, udp_flags(sock->conn->pcb.udp) & ~UDP_FLAGS_NOCHKSUM);
      }
      break;
#endif /* LWIP_UDP */
    default:
      BSD_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;

/* Level: IPPROTO_IP */
  case IPPROTO_IP:
    switch (optname) {
    case IP_TTL:
      sock->conn->pcb.ip->ttl = (u8_t)(*(int*)optval);
      D(bug("bsd_setsockopt(%p, IPPROTO_IP, IP_TTL, ..) -> %d\n",
                  sock, sock->conn->pcb.ip->ttl));
      break;
    case IP_TOS:
      sock->conn->pcb.ip->tos = (u8_t)(*(int*)optval);
      D(bug("bsd_setsockopt(%p, IPPROTO_IP, IP_TOS, ..)-> %d\n",
                  sock, sock->conn->pcb.ip->tos));
      break;
#if LWIP_IGMP
    case IP_MULTICAST_TTL:
      sock->conn->pcb.udp->ttl = (u8_t)(*(u8_t*)optval);
      break;
    case IP_MULTICAST_IF:
      inet_addr_to_ipaddr(&sock->conn->pcb.udp->multicast_ip, (struct in_addr*)optval);
      break;
    case IP_MULTICAST_LOOP:
      if (*(u8_t*)optval) {
        udp_setflags(sock->conn->pcb.udp, udp_flags(sock->conn->pcb.udp) | UDP_FLAGS_MULTICAST_LOOP);
      } else {
        udp_setflags(sock->conn->pcb.udp, udp_flags(sock->conn->pcb.udp) & ~UDP_FLAGS_MULTICAST_LOOP);
      }
      break;
    case IP_ADD_MEMBERSHIP:
    case IP_DROP_MEMBERSHIP:
      {
        /* If this is a TCP or a RAW socket, ignore these options. */
        struct ip_mreq *imr = (struct ip_mreq *)optval;
        ip_addr_t if_addr;
        ip_addr_t multi_addr;
        inet_addr_to_ipaddr(&if_addr, &imr->imr_interface);
        inet_addr_to_ipaddr(&multi_addr, &imr->imr_multiaddr);
        if(optname == IP_ADD_MEMBERSHIP){
          data->err = igmp_joingroup(&if_addr, &multi_addr);
        } else {
          data->err = igmp_leavegroup(&if_addr, &multi_addr);
        }
        if(data->err != ERR_OK) {
          data->err = EADDRNOTAVAIL;
        }
      }
      break;
#endif /* LWIP_IGMP */
    default:
      BSD_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;

#if LWIP_TCP
/* Level: IPPROTO_TCP */
  case IPPROTO_TCP:
    switch (optname) {
    case TCP_NODELAY:
      if (*(int*)optval) {
        tcp_nagle_disable(sock->conn->pcb.tcp);
      } else {
        tcp_nagle_enable(sock->conn->pcb.tcp);
      }
      D(bug("bsd_setsockopt(%p, IPPROTO_TCP, TCP_NODELAY) -> %s\n",
                  sock, (*(int *)optval)?"on":"off") );
      break;
    case TCP_KEEPALIVE:
      sock->conn->pcb.tcp->keep_idle = (u32_t)(*(int*)optval);
      D(bug("bsd_setsockopt(%p, IPPROTO_TCP, TCP_KEEPALIVE) -> %"U32_F"\n",
                  sock, sock->conn->pcb.tcp->keep_idle));
      break;

#if LWIP_TCP_KEEPALIVE
    case TCP_KEEPIDLE:
      sock->conn->pcb.tcp->keep_idle = 1000*(u32_t)(*(int*)optval);
      D(bug("bsd_setsockopt(%p, IPPROTO_TCP, TCP_KEEPIDLE) -> %"U32_F"\n",
                  sock, sock->conn->pcb.tcp->keep_idle));
      break;
    case TCP_KEEPINTVL:
      sock->conn->pcb.tcp->keep_intvl = 1000*(u32_t)(*(int*)optval);
      D(bug("bsd_setsockopt(%p, IPPROTO_TCP, TCP_KEEPINTVL) -> %"U32_F"\n",
                  sock, sock->conn->pcb.tcp->keep_intvl));
      break;
    case TCP_KEEPCNT:
      sock->conn->pcb.tcp->keep_cnt = (u32_t)(*(int*)optval);
      D(bug("bsd_setsockopt(%p, IPPROTO_TCP, TCP_KEEPCNT) -> %"U32_F"\n",
                  sock, sock->conn->pcb.tcp->keep_cnt));
      break;
#endif /* LWIP_TCP_KEEPALIVE */
    default:
      BSD_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;
#endif /* LWIP_TCP*/

#if LWIP_IPV6
/* Level: IPPROTO_IPV6 */
  case IPPROTO_IPV6:
    switch (optname) {
    case IPV6_V6ONLY:
      if (*(int*)optval) {
        sock->conn->flags |= NETCONN_FLAG_IPV6_V6ONLY;
      } else {
        sock->conn->flags &= ~NETCONN_FLAG_IPV6_V6ONLY;
      }
      D(bug("bsd_setsockopt(%p, IPPROTO_IPV6, IPV6_V6ONLY, ..) -> %d\n",
                  sock, ((sock->conn->flags & NETCONN_FLAG_IPV6_V6ONLY) ? 1 : 0)));
      break;
    default:
      BSD_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;
#endif /* LWIP_IPV6 */

#if LWIP_UDP && LWIP_UDPLITE
  /* Level: IPPROTO_UDPLITE */
  case IPPROTO_UDPLITE:
    switch (optname) {
    case UDPLITE_SEND_CSCOV:
      if ((*(int*)optval != 0) && ((*(int*)optval < 8) || (*(int*)optval > 0xffff))) {
        /* don't allow illegal values! */
        sock->conn->pcb.udp->chksum_len_tx = 8;
      } else {
        sock->conn->pcb.udp->chksum_len_tx = (u16_t)*(int*)optval;
      }
      D(bug("bsd_setsockopt(%p, IPPROTO_UDPLITE, UDPLITE_SEND_CSCOV) -> %d\n",
                  sock, (*(int*)optval)) );
      break;
    case UDPLITE_RECV_CSCOV:
      if ((*(int*)optval != 0) && ((*(int*)optval < 8) || (*(int*)optval > 0xffff))) {
        /* don't allow illegal values! */
        sock->conn->pcb.udp->chksum_len_rx = 8;
      } else {
        sock->conn->pcb.udp->chksum_len_rx = (u16_t)*(int*)optval;
      }
      D(bug("bsd_setsockopt(%p, IPPROTO_UDPLITE, UDPLITE_RECV_CSCOV) -> %d\n",
                  sock, (*(int*)optval)) );
      break;
    default:
      BSD_ASSERT("unhandled optname", 0);
      break;
    }  /* switch (optname) */
    break;
#endif /* LWIP_UDP */
  default:
    BSD_ASSERT("unhandled level", 0);
    break;
  }  /* switch (level) */
  sys_sem_signal(&sock->conn->op_completed);
}

static AROS_UFH3(BOOL, bsd__sana_CopyFromBuff,
        AROS_UFHA(APTR, to, A0),
        AROS_UFHA(struct pbuf *, from, A1),
        AROS_UFHA(ULONG, n, D0))
{
    AROS_USERFUNC_INIT

    u16_t rc;

    rc = pbuf_copy_partial(from, to, from->tot_len, 0);
    
    return (rc == from->tot_len) ? TRUE : FALSE;

    AROS_USERFUNC_EXIT
}

static AROS_UFH3(BOOL, bsd__sana_CopyToBuff,
        AROS_UFHA(struct pbuf *, to, A0),
        AROS_UFHA(APTR, from, A1),
        AROS_UFHA(ULONG, n, D0))
{
    AROS_USERFUNC_INIT

    err_t err;
    
    err = pbuf_take(to, from, n);
    return (err == ERR_OK) ? TRUE : FALSE;

    AROS_USERFUNC_EXIT
}

static err_t bsd__sana_error(const struct IOSana2Req *io)
{
    err_t err;

    switch(io->ios2_Error){
    case S2ERR_NO_ERROR: err = ERR_OK; break;
    case S2ERR_NO_RESOURCES: err = ERR_MEM; break;
    case S2ERR_BAD_ARGUMENT: err = ERR_VAL; break;
    case S2ERR_BAD_STATE: err = ERR_INPROGRESS; break;
    case S2ERR_BAD_ADDRESS: err = ERR_RTE; break;
    case S2ERR_MTU_EXCEEDED: err = ERR_BUF; break;
    case S2ERR_NOT_SUPPORTED: err = ERR_USE; break;
    case S2ERR_SOFTWARE: err = ERR_WOULDBLOCK; break;
    case S2ERR_OUTOFSERVICE: err = ERR_TIMEOUT; break;
    case S2ERR_TX_FAILURE: err = ERR_IF; break;
    default: err = ERR_IF;
    }

    return err;
}

#ifndef container_of
#define container_of(ptr, type, member) \
    (void *)((char *)(ptr) - offsetof(type, member))
#endif

static err_t bsd__sana_linkoutput(struct netif *netif, struct pbuf *p)
{
    struct bsd_netif *bn;
    struct MsgPort *mp;
    err_t err = ERR_MEM;

    bn = container_of(netif, struct bsd_netif, bn_netif);
    if ((mp = CreateMsgPort())) {
       bn->bn_IOSana2Req.ios2_Command = CMD_WRITE;
       bn->bn_IOSana2Req.ios2_Flags = SANA2IOF_RAW;
       bn->bn_IOSana2Req.ios2_PacketType = 0;
       bn->bn_IOSana2Req.ios2_DataLength = p->tot_len;
       bn->bn_IOSana2Req.ios2_Data = p;
       bn->bn_IOSana2Req.ios2_Req.io_Message.mn_ReplyPort = mp;
       DoIO(&bn->bn_IOSana2Req.ios2_Req);
       err = bsd__sana_error(&bn->bn_IOSana2Req);
    }

    return err;
}

static err_t bsd__sana_lo_init(struct netif *netif)
{
    NETIF_INIT_SNMP(netif, snmp_ifType_softwareLoopback, 0);
    netif->name[0] = 'l';
    netif->name[1] = 'o';
    netif->output = netif_loop_output;

    return ERR_OK;
}

#define BSD_READ_PER_NETIF      8

static void bsd__sana_task(struct bsd_netif *bn)
{
    struct MsgPort *mp = bn->bn_task_port;
    struct IOSana2Req *io;
    struct IOSana2Req io_read[BSD_READ_PER_NETIF];
    BOOL dead;
    err_t err;
    int i;

    /* Start up the SANA read queue */
    for (i = 0; i < BSD_READ_PER_NETIF; i++) {
        io_read[i] = bn->bn_IOSana2Req;
        io_read[i].ios2_Req.io_Message.mn_ReplyPort = mp;
        io_read[i].ios2_Command = CMD_READ;
        io_read[i].ios2_Flags = SANA2IOB_RAW;
        io_read[i].ios2_PacketType = 0;
        /* FIXME: handle what happends if the alloc fails */
        io_read[i].ios2_Data = pbuf_alloc(PBUF_RAW, bn->bn_netif.mtu, PBUF_POOL);
        /* Send the READ request off... we'll catch it later */
        SendIO(&io_read[i].ios2_Req);
    }

    while (!dead) {
        WaitPort(mp);
        io = (struct IOSana2Req *)GetMsg(mp);

        switch (io->ios2_Command) {
        case CMD_INVALID:
            dead = TRUE;
            break;
        case CMD_READ:
            if (io->ios2_Error) {
                D(bug("%s: SANA2 error (%d,%d) on CMD_READ\n", __func__, io->ios2_Error, io->ios2_WireError));
            } else {
                /* On CMD_READs, send the packet to the netif's input function */
                err = bn->bn_netif.input((struct pbuf *)io->ios2_Data, &bn->bn_netif);
                if (err != ERR_OK) {
                    D(bug("%s: lwIP netif error (%d) on input\n", __func__, err));
                }
            }
            /* Recycle the packet */
            SendIO((struct IORequest *)io);
            break;
        }
    }

    /* Clean up */
    for (i = 0; i < BSD_READ_PER_NETIF; i++) {
        AbortIO(&io_read[i].ios2_Req);
        WaitIO(&io_read[i].ios2_Req);
        pbuf_free((struct pbuf *)io_read[i].ios2_Data);
    }

    /* Reply on the CMD_INVALID death message */
    io->ios2_Error = 0;
    ReplyMsg(&io->ios2_Req.io_Message);

    return;
}

void bsd__sana_remove_cb(struct netif *netif)
{
    struct IORequest io;
    struct MsgPort *mp;
    struct bsd_netif *bn;
    
    bn = container_of(netif, struct bsd_netif, bn_netif);

    /* Send the Packet of Death */
    if ((mp = CreateMsgPort())) {
        io.io_Message.mn_ReplyPort = mp;
        io.io_Message.mn_Length = sizeof(io);
        io.io_Command = CMD_INVALID;
        PutMsg(bn->bn_task_port, &io.io_Message);
        WaitPort(mp);
        GetMsg(mp);
        DeleteMsgPort(mp);
    } else {
        D(bug("%s: Can't close netif %s - out of memory?\n", __func__, bn->bn_name));
    }
}

/* NOTE: bn->bn_IOSana2Req is still valid in this function! */
static err_t bsd__sana_init(struct netif *netif)
{
    struct bsd_netif *bn;
    struct IOSana2Req *io = &bn->bn_IOSana2Req;
    struct Sana2DeviceQuery dq;

    bn = container_of(netif, struct bsd_netif, bn_netif);

    /* Fill in some netif info */
    io->ios2_Command = S2_DEVICEQUERY;
    io->ios2_StatData = &dq;
    DoIO(&io->ios2_Req);
    if (io->ios2_Error)
        return bsd__sana_error(io);


    netif->mtu = dq.MTU;
    netif->hwaddr_len = (dq.AddrFieldSize + 7)/8;
    if (netif->hwaddr_len > NETIF_MAX_HWADDR_LEN) {
        /* We don't support this long of a station address */
        return ERR_IF;
    }

    io->ios2_Command = S2_GETSTATIONADDRESS;
    DoIO(&io->ios2_Req);
    if (io->ios2_Error)
        return bsd__sana_error(io);

    CopyMem(io->ios2_SrcAddr, netif->hwaddr, netif->hwaddr_len);
    if (dq.HardwareType == S2WireType_Ethernet) {
        netif->flags |= NETIF_FLAG_ETHERNET |
                        NETIF_FLAG_ETHARP   |
                        NETIF_FLAG_BROADCAST;
    }

    /* FIXME: Assume link is up? */
    netif->flags |= NETIF_FLAG_LINK_UP;

    netif->name[0] = 's';
    netif->name[1] = 'n';
    netif->output = etharp_output;
    netif->linkoutput = bsd__sana_linkoutput;
//    netif->link_callback = bsd__sana_link_cb;
    netif->remove_callback = bsd__sana_remove_cb;

    bn->bn_task = NewCreateTask(TASKTAG_PC, bsd__sana_task,
                                TASKTAG_NAME, bn->bn_name,
                                TASKTAG_ARG1, bn,
                                TASKTAG_TASKMSGPORT, &bn->bn_task_port,
                                TAG_END);

    return (bn->bn_task != NULL);
}

/* NOTE: This function returns -errno, or the ifindex of the interface
 */
static int bsd__sana_open(struct bsd *bsd, const char *name)
{
    struct bsd_netif *bn;

    bn = AllocVec(sizeof(*bn), MEMF_ANY | MEMF_CLEAR);
    if (bn == NULL)
        return -ENOMEM;

    do {
        if (name != NULL && name[0] != 0) {
            /* 'built in' devices */
            if (strcmp(name, "lo") == 0) {
                ip_addr_t loop_ipaddr, loop_netmask, loop_gw;
                IP4_ADDR(&loop_gw, 127,0,0,1);
                IP4_ADDR(&loop_ipaddr, 127,0,0,1);
                IP4_ADDR(&loop_netmask, 255,0,0,0);
                strcpy(bn->bn_name, "lo");
                netif_add(&bn->bn_netif, &loop_ipaddr, &loop_netmask, &loop_gw, NULL, bsd__sana_lo_init, tcpip_input);
                netif_set_up(&bn->bn_netif);
                break;
            } else {
                /* Look for a SANA II device/unit name */
                const char *suffix = strchr(name, '/');
                if (suffix != NULL) {
                    int unit;
                    char *cp;
                    unit = strtod(suffix+1, &cp);
                    if (unit >= 0 && cp != NULL && *cp == 0) {
                        /* DEVS : Networks / ifname . device \000 */
                        char path[4 + 1 + 8 + 1 + (IFNAMSIZ-2) + 1 + 6 + 1];
                        struct MsgPort *mp;

                        CopyMem("DEVS:Networks/", path, 4 + 1 + 8 + 1);
                        CopyMem(name, &path[4 + 1 + 8 + 1], suffix - name);
                        CopyMem(".device", &path[4 + 1 + 8 + 1 + (suffix - name)], 1 + 6 + 1);

                        if ((mp = CreateMsgPort())) {
                            struct TagItem tags[] = {
                                { S2_CopyToBuff, (IPTR)AROS_ASMSYMNAME(bsd__sana_CopyToBuff) },
                                { S2_CopyFromBuff, (IPTR)AROS_ASMSYMNAME(bsd__sana_CopyFromBuff) },
                                { TAG_END }};
                            bn->bn_IOSana2Req.ios2_Req.io_Message.mn_ReplyPort = mp;
                            bn->bn_IOSana2Req.ios2_Req.io_Message.mn_Length = sizeof(bn->bn_IOSana2Req);
                            bn->bn_IOSana2Req.ios2_BufferManagement = &tags;
                            if (0 == OpenDevice(name, unit, &bn->bn_IOSana2Req.ios2_Req, 0)) {
                                netif_add(&bn->bn_netif, NULL, NULL, NULL, NULL, bsd__sana_init, tcpip_input);
                                bn->bn_IOSana2Req.ios2_Req.io_Message.mn_ReplyPort = NULL;
                                DeleteMsgPort(mp);
                                break;
                            }
                            DeleteMsgPort(mp);
                        }
                    }
                }
            }
        }
        FreeVec(bn);
        bn = NULL;
    } while (0);

    if (bn) {
        ObtainSemaphore(&bsd->bs_lock);
        ADDTAIL(&bsd->bs_netif_list, &bn->bn_node);
        ReleaseSemaphore(&bsd->bs_lock);
        return 0;
    }

    return -ENODEV;
}
    
static int bsd__ioctl_netdevice(struct bsd *bsd, long cmd, void *argp)
{
    struct ifreq *ifr = argp;
    struct ifconf *ifconf = argp;
    struct bsd_netif *bn;
    char name[IFNAMSIZ+1];
    int rc, len;

    switch (cmd) {
    case SIOCGIFCONF:
        /* Get all the known SANA interfaces */
        ObtainSemaphore(&bsd->bs_lock);
        len = 0;
        ifr = ifconf->ifc_req;
        ForeachNode(&bsd->bs_netif_list, bn) {
            len += sizeof(*ifr);
            if (len < ifconf->ifc_len) {
                strncpy(ifr->ifr_name, bn->bn_name, IFNAMSIZ);
                ifr->ifr_addr.sa_len = 0;
                ifr->ifr_addr.sa_family = AF_UNSPEC;
            }
            ifr++;
        }
        ifconf->ifc_len = len;
        ReleaseSemaphore(&bsd->bs_lock);
        return 0;
        break;
    case SIOCGIFNAME:
        /* Get the name for an index */
        ObtainSemaphore(&bsd->bs_lock);
        ForeachNode(&bsd->bs_netif_list, bn) {
            if (bn->bn_netif.num == ifr->ifr_ifindex) {
                CopyMem(bn->bn_name, ifr->ifr_name, IFNAMSIZ);
                ReleaseSemaphore(&bsd->bs_lock);
                return 0;
            }
        }
        ReleaseSemaphore(&bsd->bs_lock);
        /* Not found! */
        return ENODEV;
        break;
    case SIOCGIFINDEX:
        /* See if we've already loaded this interface */
        memcpy(name, ifr->ifr_name, IFNAMSIZ);
        name[IFNAMSIZ] = 0;

        ObtainSemaphore(&bsd->bs_lock);
        ForeachNode(&bsd->bs_netif_list, bn) {
            if (strcmp(bn->bn_name, name) == 0) {
                ifr->ifr_ifindex = bn->bn_netif.num;
                ReleaseSemaphore(&bsd->bs_lock);
                return 0;
            }
        }
        ReleaseSemaphore(&bsd->bs_lock);

        /* Not found - let's see if we can start it */
        rc = bsd__sana_open(bsd, name);

        if (rc >= 0) {
            ifr->ifr_ifindex = rc;
            return 0;
        }
        return -rc;
        break;
    default:
        break;
    }

    return ENOTSUP;
}

int bsd_ioctl(struct bsd *bsd, struct bsd_sock *sock, long cmd, void *argp)
{
  u8_t val;
  int rc;
#if LWIP_SO_RCVBUF
  u16_t buflen = 0;
  s16_t recv_avail;
#endif /* LWIP_SO_RCVBUF */

  switch (cmd) {
#if LWIP_SO_RCVBUF || LWIP_FIONREAD_LINUXMODE
  case FIONREAD:
    if (!argp) {
      return EINVAL;
      
    }
#if LWIP_FIONREAD_LINUXMODE
    if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_TCP) {
      struct pbuf *p;
      if (sock->lastdata) {
        p = ((struct netbuf *)sock->lastdata)->p;
      } else {
        struct netbuf *rxbuf;
        err_t err;
        if (sock->rcvevent <= 0) {
          *((u16_t*)argp) = 0;
        } else {
          err = netconn_recv(sock->conn, &rxbuf);
          if (err != ERR_OK) {
            *((u16_t*)argp) = 0;
          } else {
            sock->lastdata = rxbuf;
            *((u16_t*)argp) = rxbuf->p->tot_len;
          }
        }
      }
      return 0;
    }
#endif /* LWIP_FIONREAD_LINUXMODE */

#if LWIP_SO_RCVBUF
    /* we come here if either LWIP_FIONREAD_LINUXMODE==0 or this is a TCP socket */
    SYS_ARCH_GET(sock->conn->recv_avail, recv_avail);
    if (recv_avail < 0) {
      recv_avail = 0;
    }
    *((u16_t*)argp) = (u16_t)recv_avail;

    /* Check if there is data left from the last recv operation. /maq 041215 */
    if (sock->lastdata) {
      struct pbuf *p = (struct pbuf *)sock->lastdata;
      if (NETCONNTYPE_GROUP(netconn_type(sock->conn)) != NETCONN_TCP) {
        p = ((struct netbuf *)p)->p;
      }
      buflen = p->tot_len;
      buflen -= sock->lastoffset;

      *((u16_t*)argp) += buflen;
    }

    D(bug("bsd_ioctl(%p, FIONREAD, %p) = %"U16_F"\n", sock, argp, *((u16_t*)argp)));
    
    return 0;
#else /* LWIP_SO_RCVBUF */
    break;
#endif /* LWIP_SO_RCVBUF */
#endif /* LWIP_SO_RCVBUF || LWIP_FIONREAD_LINUXMODE */

  case FIONBIO:
    val = 0;
    if (argp && *(u32_t*)argp) {
      val = 1;
    }
    netconn_set_nonblocking(sock->conn, val);
    D(bug("bsd_ioctl(%p, FIONBIO, %d)\n", sock, val));
    
    return 0;

  default:
    /* Try the netdevice socket ioctls */
    rc = bsd__ioctl_netdevice(bsd, cmd, argp);
    if (rc >= 0)
        return rc;
    break;
  } /* switch (cmd) */
  D(bug("bsd_ioctl(%p, UNIMPL: 0x%lx, %p)\n", sock, cmd, argp));
  return ENOSYS; /* not yet implemented */
}

/** A minimal implementation of fcntl.
 * Currently only the commands F_GETFL and F_SETFL are implemented.
 * Only the flag O_NONBLOCK is implemented.
 */
int bsd_fcntl(struct bsd *bsd, struct bsd_sock *sock, int cmd, int val)
{
  int ret = -1;

  switch (cmd) {
  case F_GETFL:
    ret = netconn_is_nonblocking(sock->conn) ? O_NONBLOCK : 0;
    break;
  case F_SETFL:
    if ((val & ~O_NONBLOCK) == 0) {
      /* only O_NONBLOCK, all other bits are zero */
      netconn_set_nonblocking(sock->conn, val & O_NONBLOCK);
      ret = 0;
    }
    break;
  default:
    D(bug("bsd_fcntl(%p, UNIMPL: %d, %d)\n", sock, cmd, val));
    break;
  }
  return ret;
}

int bsd_socket_release(struct bsd *bsd, struct bsd_sock *sock, int id)
{
    /* Nothing to do */

    return 0;
}

int bsd_socket_obtain(struct bsd *bsd, struct bsd_sock *sock)
{
    return 0;
}

int bsd_socket_dup(struct bsd *bsd, struct bsd_sock *olds, struct bsd_sock **news)
{
    *news = olds;
    olds->usage++;

    return 0;
}
