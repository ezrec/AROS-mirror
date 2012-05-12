/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SYS_IOCTL_COMPAT_H
#define SYS_IOCTL_COMPAT_H

#include <sys/ioctl.h>

#define IOCTLTRIM       _IOW('t', 128, off_t[2])

#endif /* SYS_IOCTL_COMPAT_H */
