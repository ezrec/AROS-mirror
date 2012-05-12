/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef MACHINE_AROS_H
#define MACHINE_AROS_H

#include <strings.h>

#include <exec/types.h>
#include <hardware/cpu/memory.h>

#define EJUSTRETURN     -2
#define ERESTART        -1
#define EINVAL          1
#define E2BIG           2
#define ENXIO           3
#define EACCES          4
#define EIO             5
#define ENOSPC          6
#define ENOTTY          7
#define EOPNOTSUPP      8
#define ENODEV          9
#define EPERM           10
#define ENOTSUP         11
#define ENOENT          12
#define EBUSY           13
#define ENOMEM          14
#define EBADF           15
#define EINPROGRESS     16
#define EADDRINUSE      17
#define ECANCELED       18
#define ETIMEDOUT       19
#define EAGAIN          20
#define ESRCH           21
#define ENOLCK          22
#define EFTYPE          23
#define EWOULDBLOCK     24
#define EFAULT          25

#define __packed        __attribute__((__packed__))
#define __printflike(a,b)
#define NELEM(x)        ((sizeof(x)/sizeof((x)[0])))
#define NBBY            8

typedef BOOL    boolean_t;
typedef IPTR vm_offset_t;

#define FREAD   1
#define FWRITE  2

struct cdev;
typedef struct cdev *cdev_t;

#define MAXPHYS (128 * 1024)    /* max raw I/O transfer size */

#define htonl(x)        AROS_LONG2BE(x)

#endif /* MACHINE_AROS_H */
