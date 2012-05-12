/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef MACHINE_ENDIAN_H
#define MACHINE_ENDIAN_H

#include <sys/types.h>

static inline u_int16_t __bswap16(u_int16_t x)
{
    return (((x) & 0xff) << 8) | (((x) >> 8) & 0xff);
}

#endif /* MACHINE_ENDIAN_H */
