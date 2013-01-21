/*
 * Copyright (C) 2013, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef CD_INTERN_H
#define CD_INTERN_H

#include <exec/devices.h>

#include LC_LIBDEFS_FILE

struct cdBase {
    struct Device cd_Device;
};

#endif /* CD_INTERN_H */
