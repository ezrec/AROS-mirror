/*
 * Copyright (C) 2014, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 * 
 * $Id$
 */

#ifndef BSD_SOCKET_H
#define BSD_SOCKET_H

struct bsd;

struct bsd *bsd_init(struct bsd_global *bsd_global);
void bsd_expunge(struct bsd *bsd);

int bsd_accept(struct bsd *bsd, int s, struct sockaddr *addr, socklen_t *addrlen);
int bsd_bind(struct bsd *bsd, int s, const struct sockaddr *name, socklen_t namelen);
int bsd_close(struct bsd *bsd, int s);
int bsd_connect(struct bsd *bsd, int s, const struct sockaddr *name, socklen_t namelen);
int bsd_listen(struct bsd *bsd, int s, int backlog);
int bsd_recvfrom(struct bsd *bsd, int s, void *mem, size_t len, int flags,
int bsd_read(struct bsd *bsd, int s, void *mem, size_t len);
int bsd_recv(struct bsd *bsd, int s, void *mem, size_t len, int flags);
int bsd_send(struct bsd *bsd, int s, const void *data, size_t size, int flags);
int bsd_sendto(struct bsd *bsd, int s, const void *data, size_t size, int flags,
int bsd_socket(struct bsd *bsd, int domain, int type, int protocol);
int bsd_write(struct bsd *bsd, int s, const void *data, size_t size);
int bsd_select(struct bsd *bsd, int maxfdp1,
int bsd_shutdown(struct bsd *bsd, int s, int how);
int bsd_getpeername(struct bsd *bsd, int s, struct sockaddr *name, socklen_t *namelen);
int bsd_getsockname(struct bsd *bsd, int s, struct sockaddr *name, socklen_t *namelen);
int bsd_getsockopt(struct bsd *bsd, int s, int level, int optname, void *optval, socklen_t *optlen);
int bsd_setsockopt(struct bsd *bsd, int s, int level, int optname, const void *optval, socklen_t optlen);
void bsd__sana_remove_cb(struct netif *netif);
int bsd_ioctl(struct bsd *bsd, int s, long cmd, void *argp);
int bsd_fcntl(struct bsd *bsd, int s, int cmd, int val);
int bsd_getdtablesize(struct bsd *bsd);
int bsd_setdtablesize(struct bsd *bsd, int size);
struct in_addr bsd_inet_addr(struct bsd *bsd, const char *cp);
char *bsd_inet_ntoa(struct bsd *bsd, struct in_addr *addr);
int bsd_socket_release(struct bsd *bsd, int s, int id);
int bsd_socket_obtain(struct bsd *bsd, int id);
int bsd_socket_dup2(struct bsd *bsd, int olds, int news);

#endif /* BSD_SOCKET_H */
