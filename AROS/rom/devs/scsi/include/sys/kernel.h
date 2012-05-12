/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SYS_KERNEL_H
#define SYS_KERNEL_H

#include <sys/linker_set.h>

#define TUNABLE_INT(path, value) /* Not implemented */
#define TUNABLE_INT_FETCH(path, var)    /* Nothing to do */

struct intr_config_hook {
    void    (*ich_func) (void *);
    void    *ich_arg;
    const char *ich_desc;
    int     ich_order;
};

int  config_intrhook_establish(struct intr_config_hook *);
void config_intrhook_disestablish(struct intr_config_hook *);

/* 'wakeup' is called on this for every tick */
extern int ticks;

#define SI_ORDER_FIRST          0x01    /* 0x0000001 */
#define SI_ORDER_SECOND         0x02    /* 0x0000002 */
#define SI_BOOT1_TUNABLES       0x07    /* 0x0700000 */
#define SI_BOOT1_LOCK           0x09    /* 0x0900000 */
#define SI_ORDER_MIDDLE         0x10    /* 0x1000000 */
#define SI_BOOT1_POST           0x18    /* 0x1800000 */
#define SI_SUB_DRIVERS          0x24    /* 0x2400000 */
#define SI_SUB_CONFIGURE        0x38    /* 0x3800000 */
#define SI_ORDER_ANY            0xff    /* 0xfffffff */

typedef void (*sysinit_nfunc_t) (void *);
typedef void (*sysinit_cfunc_t) (const void *);

struct sysinit {
        unsigned int    subsystem;              /* subsystem identifier*/
        unsigned int    order;                  /* init order within subsystem*/
        sysinit_cfunc_t func;                   /* function             */
        const void      *udata;                 /* multiplexer/argument */
        const char      *name;
};

DECLARESET(sysinit_set);

/*
 * Default: no special processing
 *
 * The C_ version of SYSINIT is for data pointers to const
 * data ( and functions taking data pointers to const data ).
 * At the moment it is no different from SYSINIT and thus
 * still results in warnings.
 *
 * The casts are necessary to have the compiler produce the
 * correct warnings when -Wcast-qual is used.
 *
 */
#define C_SYSINIT(uniquifier, subsystem, order, func, ident)    \
        static struct sysinit uniquifier ## _sys_init = {       \
                subsystem,                                      \
                order,                                          \
                func,                                           \
                ident,                                          \
                #uniquifier                                     \
        };                                                      \
        DATA_SET(sysinit_set,uniquifier ## _sys_init);

#define SYSINIT(uniquifier, subsystem, order, func, ident)      \
        C_SYSINIT(uniquifier, subsystem, order,                 \
        (sysinit_cfunc_t)(sysinit_nfunc_t)func, (void *)ident)

#endif /* SYS_KERNEL_H */
