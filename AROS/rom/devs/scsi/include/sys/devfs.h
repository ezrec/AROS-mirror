/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SYS_DEVFS_H
#define SYS_DEVFS_H

#include <sys/event.h>

struct devfs_bitmap { };
#define DEVFS_CLONE_BITMAP(name)       devfs_clone_bitmap_##name
#define DEVFS_DECLARE_CLONE_BITMAP(name)        struct devfs_bitmap DEVFS_CLONE_BITMAP(name)

static inline void devfs_clone_bitmap_put(struct devfs_bitmap *bitmap, int unit)
{
}

#endif /* SYS_DEVFS_H */
