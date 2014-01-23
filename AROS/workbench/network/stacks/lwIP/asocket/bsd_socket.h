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
struct bsd_sock;

struct bsd *bsd_init(void);
void bsd_expunge(struct bsd *bsd);

int bsd_accept(struct bsd *bsd, struct bsd_sock *s, struct bsd_sock **new_s);
int bsd_bind(struct bsd *bsd, struct bsd_sock *s, const struct sockaddr *name, socklen_t namelen);
int bsd_close(struct bsd *bsd, struct bsd_sock *s);
int bsd_connect(struct bsd *bsd, struct bsd_sock *s, const struct sockaddr *name, socklen_t namelen);
int bsd_listen(struct bsd *bsd, struct bsd_sock *s, int backlog);
int bsd_recvmsg(struct bsd *bsd, struct bsd_sock *s, struct msghdr *msg, int flags);
int bsd_sendmsg(struct bsd *bsd, struct bsd_sock *s, const struct msghdr *msg, int flags);
int bsd_socket(struct bsd *bsd, struct bsd_sock **new_s, int domain, int type, int protocol);
int bsd_shutdown(struct bsd *bsd, struct bsd_sock *s, int how);
int bsd_getpeername(struct bsd *bsd, struct bsd_sock *s, struct sockaddr *name, socklen_t *namelen);
int bsd_getsockname(struct bsd *bsd, struct bsd_sock *s, struct sockaddr *name, socklen_t *namelen);
int bsd_getsockopt(struct bsd *bsd, struct bsd_sock *s, int level, int optname, void *optval, socklen_t *optlen);
int bsd_setsockopt(struct bsd *bsd, struct bsd_sock *s, int level, int optname, const void *optval, socklen_t optlen);
int bsd_ioctl(struct bsd *bsd, struct bsd_sock *s, long cmd, void *argp);
int bsd_fcntl(struct bsd *bsd, struct bsd_sock *s, int cmd, int val);
int bsd_socket_dup(struct bsd *bsd, struct bsd_sock *olds, struct bsd_sock **news);

#endif /* BSD_SOCKET_H */
