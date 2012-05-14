/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SYS_SYSCTL_H
#define SYS_SYSCTL_H

#ifdef CONFIG_SYSCTL

#define SYSCTL_DECL(x)
#define SYSCTL_NODE(parent, nbr, name, access, handler, descr)

struct sysctl_oid;
struct sysctl_req {
    void *newptr;
};

struct sysctl_ctx_list { };

#define SYSCTL_NODE(parent, nbr, name, access, handler, descr)
#define SYSCTL_HANDLER_ARGS     struct sysctl_oid *oidp, void *arg1, int arg2, struct sysctl_req *req
#define SYSCTL_PROC(a1, a2, a3, a4, a5, a6, a7, a8, a9) static const void *a1 = (void *)a7; static inline const void *get_##a1(void) { return a1; }
#define SYSCTL_INT(parent, nbr, name, access, ptr, val, descr)
#define SYSCTL_ADD_NODE(ctx, parent, nbr, name, access, handler, descr) NULL
#define SYSCTL_ADD_PROC(ctx, parent, nbr, name, access, ptr, arg, handler, fmt, descr)
#define SYSCTL_ADD_INT(ctx, parent, nbr, name, access, ptr, val, descr)

int sysctl_handle_int(SYSCTL_HANDLER_ARGS);
int sysctl_handle_string(SYSCTL_HANDLER_ARGS);
int     sysctl_ctx_init(struct sysctl_ctx_list *clist);
int     sysctl_ctx_free(struct sysctl_ctx_list *clist);
#endif

#endif /* SYS_SYSCTL_H */
