/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SYS_CONF_H
#define SYS_CONF_H

#include <machine/aros.h>
#include <sys/time.h>

#define UID_ROOT        0
#define GID_OPERATOR    0
#define GID_WHEEL       0

struct cdev {
    int         si_iosize_max;
    void       *si_drv1;
    int         si_uminor;
};

const char *devtoname (cdev_t dev);
cdev_t  reference_dev (cdev_t dev);
void    destroy_dev (cdev_t dev);
void    release_dev (cdev_t dev);

#endif /* SYS_CONF_H */
