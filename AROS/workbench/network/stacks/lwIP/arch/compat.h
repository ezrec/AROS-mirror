/*
 * Copyright (C) 2014, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 * 
 * $Id$
 */

#ifndef LWIP_ARCH_COMPAT_H
#define LWIP_ARCH_COMPAT_H

/* Get the AROS constansts */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/un.h>

#include <fcntl.h>

/* We will provide the definitions of lwip/inet.h, to
 * be compatible with the definitions in AROS
 */
#define __LWIP_INET_H__

#define inet_addr_from_ipaddr(target_inaddr, source_ipaddr) ((target_inaddr)->s_addr = ip4_addr_get_u32(source_ipaddr))
#define inet_addr_to_ipaddr(target_ipaddr, source_inaddr)   (ip4_addr_set_u32(target_ipaddr, (source_inaddr)->s_addr))

/* We will provide the definitions of lwip/inet6.h, to
 * be compatible with the definitions in AROS
 */
#define __LWIP_INET6_H__

#define inet6_addr_to_ip6addr(target_ip6addr, source_in6addr)   { \
    (target_ip6addr)->addr[0] = (source_in6addr)->un.u32_addr[0]; \
    (target_ip6addr)->addr[1] = (source_in6addr)->un.u32_addr[1]; \
    (target_ip6addr)->addr[2] = (source_in6addr)->un.u32_addr[2]; \
    (target_ip6addr)->addr[3] = (source_in6addr)->un.u32_addr[3];}

#define inet6_addr_from_ip6addr(target_in6addr, source_ip6addr) { \
    (target_in6addr)->un.u32_addr[0] = (source_ip6addr)->addr[0]; \
    (target_in6addr)->un.u32_addr[1] = (source_ip6addr)->addr[1]; \
    (target_in6addr)->un.u32_addr[2] = (source_ip6addr)->addr[2]; \
    (target_in6addr)->un.u32_addr[3] = (source_ip6addr)->addr[3];}


/* We will provide the definitions of lwip/sockets.h, to
 * be compatible with the definitions in AROS
 */
#define __LWIP_SOCKETS_H__
#define SIN_ZERO_LEN 8

#define LWIP_IPV6   1
#define LWIP_SOCKET 1

void lwip_socket_init(void);

int lwip_accept(int s, struct sockaddr *addr, socklen_t *addrlen);
int lwip_bind(int s, const struct sockaddr *name, socklen_t namelen);
int lwip_shutdown(int s, int how);
int lwip_getpeername (int s, struct sockaddr *name, socklen_t *namelen);
int lwip_getsockname (int s, struct sockaddr *name, socklen_t *namelen);
int lwip_getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen);
int lwip_setsockopt (int s, int level, int optname, const void *optval, socklen_t optlen);
int lwip_close(int s);
int lwip_connect(int s, const struct sockaddr *name, socklen_t namelen);
int lwip_listen(int s, int backlog);
int lwip_recv(int s, void *mem, size_t len, int flags);
int lwip_read(int s, void *mem, size_t len);
int lwip_recvfrom(int s, void *mem, size_t len, int flags,
      struct sockaddr *from, socklen_t *fromlen);
int lwip_send(int s, const void *dataptr, size_t size, int flags);
int lwip_sendto(int s, const void *dataptr, size_t size, int flags,
    const struct sockaddr *to, socklen_t tolen);
int lwip_socket(int domain, int type, int protocol);
int lwip_write(int s, const void *dataptr, size_t size);
int lwip_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                struct timeval *timeout);
int lwip_ioctl(int s, long cmd, void *argp);
int lwip_fcntl(int s, int cmd, int val);


#endif /* LWIP_ARCH_COMPAT_H */
