/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SYS_THREAD2_H
#define SYS_THREAD2_H

#include <proto/exec.h>

#define crit_enter()    Forbid()
#define crit_exit()     Permit();

#endif /* SYS_THREAD2_H */
