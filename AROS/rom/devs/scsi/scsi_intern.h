/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SCSI_INTERN_H
#define SCSI_INTERN_H

#include <exec/devices.h>

struct SCSIBase {
    struct Device sb_Device;
};

#endif /* SCSI_INTERN_H */
