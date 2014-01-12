/*
 * Copyright (C) 2014, The AROS Development Team.  All rights reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef BSD_SOCKET_H
#define BSD_SOCKET_H

#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#define BSD_DTABLESIZE_DEFAULT  64

struct bsd_global;      /* Global data */
struct bsd;             /* Per-opener private data */

struct bsd_global *bsd_global_init(void);
int bsd_global_expunge(struct bsd_global *bsd_global);

struct bsd *bsd_init(struct bsd_global *bsd_global);
void bsd_expunge(struct bsd *bsd);

int bsd_set_errno(struct bsd *bsd, void *errno_ptr, size_t errno_size);
int bsd_errno(struct bsd *bsd);

int bsd_accept(struct bsd *bsd, int s, struct sockaddr *addr, socklen_t *addrlen);
int bsd_bind(struct bsd *bsd, int s, const struct sockaddr *name, socklen_t namelen);
int bsd_shutdown(struct bsd *bsd, int s, int how);
int bsd_getpeername (struct bsd *bsd, int s, struct sockaddr *name, socklen_t *namelen);
int bsd_getsockname (struct bsd *bsd, int s, struct sockaddr *name, socklen_t *namelen);
int bsd_getsockopt (struct bsd *bsd, int s, int level, int optname, void *optval, socklen_t *optlen);
int bsd_setsockopt (struct bsd *bsd, int s, int level, int optname, const void *optval, socklen_t optlen);
int bsd_close(struct bsd *bsd, int s);
int bsd_connect(struct bsd *bsd, int s, const struct sockaddr *name, socklen_t namelen);
int bsd_listen(struct bsd *bsd, int s, int backlog);
int bsd_recv(struct bsd *bsd, int s, void *mem, size_t len, int flags);
int bsd_read(struct bsd *bsd, int s, void *mem, size_t len);
int bsd_recvfrom(struct bsd *bsd, int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
int bsd_send(struct bsd *bsd, int s, const void *dataptr, size_t size, int flags);
int bsd_sendto(struct bsd *bsd, int s, const void *dataptr, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);
int bsd_socket(struct bsd *bsd, int domain, int type, int protocol);
int bsd_write(struct bsd *bsd, int s, const void *dataptr, size_t size);
int bsd_select(struct bsd *bsd, int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout, ULONG *sigmask);
int bsd_ioctl(struct bsd *bsd, int s, long cmd, void *argp);
int bsd_fcntl(struct bsd *bsd, int s, int cmd, int val);

int bsd_getdtablesize(struct bsd *bsd);
int bsd_setdtablesize(struct bsd *bsd, int size);
struct in_addr bsd_inet_addr(struct bsd *bsd, const char *cp);
char *bsd_inet_ntoa(struct bsd *bsd, struct in_addr *addr);

int bsd_socket_obtain(struct bsd *bsd, int id, int domain, int type, int protocol);
int bsd_socket_release(struct bsd *bsd, int s, int id);
int bsd_socket_dup2(struct bsd *bsd, int olds, int news);

#endif /* BSD_SOCKET_H */
