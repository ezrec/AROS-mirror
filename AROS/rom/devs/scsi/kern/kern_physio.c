/*
 * Copyright (C) 2012, The AROS Development Team
 * All right reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#include <sys/conf.h>
#include <sys/systm.h>
#include <sys/buf.h>

int physread(struct dev_read_args *ap)
{
    /* No 'user space' call, so nothing to do here */
    return -1;
}

int physwrite(struct dev_write_args *ap)
{
    /* No 'user space' call, so nothing to do here */
    return -1;
}


