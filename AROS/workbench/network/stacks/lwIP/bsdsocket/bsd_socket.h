/*
 * Copyright (C) 2014, The AROS Development Team.  All rights reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef BSD_SOCKET_H
#define BSD_SOCKET_H

#include <netinet/in.h>

int bsd_accept(int s, struct sockaddr *addr, socklen_t *addrlen);
int bsd_bind(int s, const struct sockaddr *name, socklen_t namelen);
int bsd_shutdown(int s, int how);
int bsd_getpeername (int s, struct sockaddr *name, socklen_t *namelen);
int bsd_getsockname (int s, struct sockaddr *name, socklen_t *namelen);
int bsd_getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen);
int bsd_setsockopt (int s, int level, int optname, const void *optval, socklen_t optlen);
int bsd_close(int s);
int bsd_connect(int s, const struct sockaddr *name, socklen_t namelen);
int bsd_listen(int s, int backlog);
int bsd_recv(int s, void *mem, size_t len, int flags);
int bsd_read(int s, void *mem, size_t len);
int bsd_recvfrom(int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
int bsd_send(int s, const void *dataptr, size_t size, int flags);
int bsd_sendto(int s, const void *dataptr, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);
int bsd_socket(int domain, int type, int protocol);
int bsd_write(int s, const void *dataptr, size_t size);
int bsd_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout);
int bsd_ioctl(int s, long cmd, void *argp);
int bsd_fcntl(int s, int cmd, int val);

int bsd_getdtablesize(void);
in_addr_t bsd_inet_addr(const char *cp);
char *bsd_inet_ntoa(struct in_addr *addr);

#endif /* BSD_SOCKET_H */
