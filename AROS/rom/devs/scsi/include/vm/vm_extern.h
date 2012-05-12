/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifndef SYS_VM_EXTERN_H
#define SYS_VM_EXTERN_H

static inline int useracc(caddr_t addr, int len, int rw)
{
    return 1;
}

#endif /* SYS_VM_EXTERN_H */
